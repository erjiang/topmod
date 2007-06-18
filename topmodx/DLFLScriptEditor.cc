#ifdef WITH_PYTHON

#include "DLFLScriptEditor.hh"

#include <QtGui>

static QStream *pyResultStream;

DLFLScriptEditor::DLFLScriptEditor( QWidget *parent, Qt::WindowFlags f ) : QWidget(parent,f), dlfl_module(NULL),dlfl_dict(NULL) {

  mTextEdit = new QTextEdit;
  mLineEdit = new Editor;
  QObject::connect(mLineEdit, SIGNAL(ctrlReturnPressed()), this, SLOT(executeCommand()));
  QObject::connect( this, SIGNAL(addToHistory(const QString&)), mLineEdit, SLOT(appendHistory(const QString&)) );

  QPalette whiteOnBlack, whiteOnGrey;
  mInputBgColor = QColor(0,0,5,255);
  whiteOnBlack.setColor(QPalette::Active, QPalette::Text, Qt::white );
  whiteOnBlack.setColor(QPalette::Active, QPalette::Base, mInputBgColor );

  mOutputBgColor = QColor(20,20,25,255);
  whiteOnGrey.setColor(QPalette::Active, QPalette::Text, Qt::white );
  whiteOnGrey.setColor(QPalette::Active, QPalette::Base, mOutputBgColor );

  mTextEdit->setPalette( whiteOnGrey );
  mLineEdit->setPalette( whiteOnBlack );

  mTextEdit->setReadOnly(true);

  pyhigh = new PythonHighlighter(mTextEdit);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(mTextEdit);
  mainLayout->addWidget(mLineEdit);
  setLayout(mainLayout);

  PyInit( );
}

DLFLScriptEditor::~DLFLScriptEditor( ) {
  if( Py_IsInitialized() )
    Py_Finalize( );
}


void DLFLScriptEditor::executeCommand( ) {
  pyResultStream = new QStream(std::cout);
  QString command = mLineEdit->toPlainText();
  QStringList cmdList = command.split('\n',QString::SkipEmptyParts);
  QStringList resultList;

  QString result;

  mTextEdit->moveCursor( QTextCursor::End );

  if( !command.isEmpty() ) {
    if( Py_IsInitialized() ) {
      PyObject *main = PyImport_AddModule("__main__");
      PyObject *dlfl = PyImport_AddModule("dlfl");
      PyObject* main_dict = PyModule_GetDict( main );
      PyObject* dlfl_dict = PyModule_GetDict( dlfl );

      PyRun_SimpleString( "import sys, __main__" );
      PyRun_SimpleString( "__main__.mio = MyIO()" );
      PyRun_SimpleString( "sys.stdout = __main__.mio" );

      for(int i = 0; i < cmdList.size(); i++ ) {
	if( cmdList.at(i).contains(QRegExp("\\bload\\("))) {
	  QStringList list = cmdList.at(i).split("\"", QString::SkipEmptyParts);
	  emit requestObject(list.at(1)); // 3 parts: load(, filename.obj, and )
	  emit cmdExecuted();
	  continue;
	}

	const char *cmd = cmdList.at(i).toLocal8Bit().constData();
	pyResultStream->string.clear();
	PyObject *rstring = PyRun_String( cmd, Py_file_input, main_dict, dlfl_dict );
	PyRun_SimpleString( "if(len(__main__.mio.s) > 0 ): __main__.__result = __main__.mio.s[0]");

	if( rstring != NULL ) {
	  PyObject *resultObject = PyDict_GetItemString( main_dict, "__result" );
	  if( resultObject != NULL ) {
	    char *string = PyString_AsString( PyObject_Str(resultObject) );
	    result = QString( string );
	    Py_DECREF( resultObject );
	  }
	  Py_DECREF( rstring );
	  emit cmdExecuted();
	} else {
	  //PyErr_Print();
	  PyObject *object, *data, *traceback;
	  PyErr_Fetch( &object, &data, &traceback );
	  PyObject * traceStr = PyObject_Str( data );
	  char *string = PyString_AsString( traceStr );
	  result = QString( string );
	}
	resultList << result; // append result to resultList
      } // for loop
    } // end Py_IsInitialized
    emit addToHistory(command);
  } else { // enter when command.isEmpty() == true
    command = QString("");
    result = QString("");
  }
    
  for(int i = 0; i < cmdList.size(); i++ ) {
    mTextEdit->insertPlainText( "\n" + cmdList.at(i) );
    if( i < resultList.size() ) {
      QString res = resultList.at(i);
      if( !res.isEmpty() && res != QString("None") )
	mTextEdit->insertPlainText( "\n# "+ res ); // # is a comment in python
    }
  }
  mLineEdit->clear();

  QScrollBar *vBar = mTextEdit->verticalScrollBar();
  vBar->triggerAction(QAbstractSlider::SliderToMaximum);
  mLineEdit->goToHistoryStart( );

  delete pyResultStream;
}

void DLFLScriptEditor::PyInit( ) {
  Py_Initialize( );

  if( Py_IsInitialized() ) {
    // Import the DLFL Module
    dlfl_module = PyImport_ImportModule("dlfl");
    PyRun_SimpleString( "import sys, __main__" );
    PyRun_SimpleString( "def my_displayhook(o): __main__.__result=o" );
    //PyRun_SimpleString( "sys.displayhook=my_displayhook" );

    PyRun_SimpleString( "class MyIO:\n\tdef __init__(self): self.s=[]\n\tdef write(self,x): self.s.append(x)" );

    if( dlfl_module != NULL )
      dlfl_dict = PyModule_GetDict( dlfl_module );

    if( dlfl_module != NULL && dlfl_dict != NULL ) {
      // Setup the Python DLFL Module C API
      PyObject *c_api_object = PyDict_GetItemString( dlfl_dict, "_C_API" );
      if (PyCObject_Check(c_api_object)) {
	PyDLFL_API = (void **)PyCObject_AsVoidPtr(c_api_object);
      }
      // Tell the module we're working inside the GUI
      PyDLFL_UsingGUI(true);
    }

    // Print out the standard version info and such
    const char *version = Py_GetVersion( );
    QString qversion(version);
    mTextEdit->append("Python " + qversion + "\n" );
    if( dlfl_module != NULL )
      mTextEdit->append("from dlfl import *\n");
    else {
      mTextEdit->append("no dlfl python module found");
      mTextEdit->insertPlainText("\n");
      PyErr_Print();
    }
  }
}

// A slot to pass the GUI object pointer to the DLFLModule
void DLFLScriptEditor::loadObject( DLFLObject *obj, QString fileName ) {
  if( dlfl_module == NULL && dlfl_dict == NULL )
    return;
  PyDLFL_PassObject( obj );
  // Print out the equivalent command to loading an object
  QString command = tr("load(\"")+fileName+tr("\")");
  mTextEdit->insertPlainText( "\n" + command );
}

#endif // WITH_PYTHON
