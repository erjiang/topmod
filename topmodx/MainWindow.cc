/****************************************************************************
 **
 **
 ****************************************************************************/

#include <QtGui>
#include <QtOpenGL>
#include "MainWindow.hh"

/*!
	\ingroup gui
	@{
	
	\class MainWindow
	\brief The Main Window of the application.
	
	\note 
	
	\see MainWindow
*/

#ifdef WITH_PYTHON
#include <Python.h>
#endif

WireframeRendererPtr MainWindow::wired;              	//!< WireframeRenderer Pointer
NormalRendererPtr MainWindow::normal;                	//!< NormalRenderer Pointer
LitRendererPtr MainWindow::lit;                       //!< LitRenderer Pointer
TexturedRendererPtr MainWindow::textured;          		//!< TexturedRenderer Pointer
TexturedLitRendererPtr MainWindow::texturedlit; 			//!< TexturedLitRenderer Pointer
PatchRendererPtr MainWindow::patch;		       					//!< PatchRenderer Pointer

//-- Parameters used in various operations on the DLFL object --//
//-- See header file for explanations --//

int MainWindow::drag_startx = 0;
int MainWindow::drag_starty = 0;
bool MainWindow::is_editing = false;

// Edge deletion
bool MainWindow::delete_edge_cleanup = true;

// Handles
int MainWindow::num_segments = 10;
int MainWindow::max_segments = -1;
bool MainWindow::symmetric_weights = true;
double MainWindow::nwt1 = 5.0;
double MainWindow::nwt2 = 5.0;
int MainWindow::num_extra_twists = 0;

// Extrusions
double MainWindow::extrude_dist = 2.0;
double MainWindow::extrude_rot = 0.0;
double MainWindow::extrude_scale = 1.0;
int MainWindow::num_extrusions = 1;
double MainWindow::ds_ex_twist = 0.0;
bool MainWindow::dual_mesh_edges_check = false;
bool MainWindow::hexagonalize_dodeca_extrude = false;
double MainWindow::extrude_bendT = 0;
double MainWindow::extrude_bendB = 0;

// Edge subdivision
int MainWindow::num_e_subdivs = 2;

// Split valence 2 vertices
double MainWindow::vertex_split_offset=-0.1;

// Crust modeling
double MainWindow::crust_thickness = 0.5;
double MainWindow::crust_scale_factor = 0.9;
bool MainWindow::crust_cleanup = true;

// Wireframe modeling
double MainWindow::wireframe_thickness = 0.25;

// Column modeling
double MainWindow::column_thickness = 0.25;
int MainWindow::column_segments = 4;

// Subdivision
double MainWindow::twist_factor = 0.0;
double MainWindow::weight_factor = 0.0;
bool MainWindow::doo_sabin_check = true;
double MainWindow::vertex_cutting_offset = 0.25;
double MainWindow::pentagonal_offset = 0.0;
double MainWindow::pentagonal_scale = 0.75;
bool MainWindow::accurate_dual = true;
double MainWindow::checkerboard_thickness = 0.33;

// Added by Doug
double MainWindow::star_offset = 0.0;
double MainWindow::fractal_offset = 1.0;

// Added by Eric
double MainWindow::substellate_height = 0.0;
double MainWindow::substellate_curve = 0.0;

// Added by bei&cansin dec 09
double MainWindow::domeLength_factor = 1.0 ;
double MainWindow::domeScale_factor = 1.0  ;
double MainWindow::dual1264_scale_factor = 0.7 ;

// Added by bei dec 11
double MainWindow::dooSabinBCnewScale_factor = 1.0;
double MainWindow::dooSabinBCnewLength_factor = 1.0;

// Added by bei 12-11-03
double MainWindow::loopLength_factor = 1.0;

// Face subdivision
bool MainWindow::use_quads = true;

// Tile texturing
int MainWindow::tile_tex_n = 2;

// Multi-face handles
MainWindow::MFHAlgo MainWindow::mfh_algo = MainWindow::ConvexHull;
double MainWindow::mfh_scale_factor = 1.0;
double MainWindow::mfh_extrude_dist = 0.5;
bool MainWindow::mfh_use_max_offsets = false;
double MainWindow::mfh_min_adj_factor = 0.01;
bool MainWindow::mfh_make_connections = true;

// Menger Sponge creation
double MainWindow::sponge_thickness = 0.67; // Thickness for Menger Sponge
double MainWindow::sponge_collapse_threshold = 0.0; // Threshold factor for edge collapse
bool MainWindow::sponge_fractional_thickness = false; // Thickness is fractional

//conical sculpting initialization from ozgur
double MainWindow::cutOffsetE_factor = 0.25;
double MainWindow::cutOffsetV_factor = 0.25;
double MainWindow::pnormalBendS_factor = 0;
double MainWindow::pnormalBendT_factor = 0;
bool   MainWindow::global_cut = false;
int    MainWindow::numberofPeels = 90;
bool   MainWindow::selected_cut=true;

//-- Selection count, used as index into selection lists --//
int MainWindow::num_sel_verts = 0;
int MainWindow::num_sel_edges = 0;
int MainWindow::num_sel_faces = 0;
int MainWindow::num_sel_faceverts = 0;

bool MainWindow::deselect_verts = false;
bool MainWindow::deselect_edges = false;
bool MainWindow::deselect_faces = false;
bool MainWindow::deselect_faceverts = false;

// for face loop selection 
DLFLEdgePtr MainWindow::face_loop_start_edge = NULL;

/**
 * asdflkjasdf
 * asdfl;jkas;df
 **/
MainWindow::MainWindow(char *filename) : object(), mode(NormalMode), undoList(), redoList(), 
																				 undolimit(20), useUndo(true), mIsModified(false), mIsPrimitive(false), mWasPrimitive(false), mSpinBoxMode(None) {
	//initialize the OpenGL Window GLWidget
	QGLFormat fmt;
	//initialize renderer
	createRenderers();
	setMouseTracking(true);

	active = new GLWidget(500,600, lit, QColor(255,255,255,255),QColor(255,255,255,255) , &object, fmt, this);
	// active->switchTo(VPPersp);	
	setRenderer(lit);
	active->redraw();
	active->setMinimumSize(400,400);
	active->setFocusPolicy(Qt::StrongFocus);
	// active->setResizeEnabled(true);
	//status bar
	mStatusBar = new QStatusBar(this);
	//statusbar is not working!!! it throws off glwidget's pick coordinates.... :(
	setStatusBar(mStatusBar);
	setAttribute(Qt::WA_AcceptDrops, true);
	setWindowFlags(Qt::Window);
	setWindowTitle(tr("newfile[*] - TopMod"));
	// cWidget = new QWidget( );	
	// QWidget *shwidget = new QWidget;
	// QVBoxLayout *vblayout = new QVBoxLayout;
	// vblayout->addWidget(active);
	// shwidget->setLayout(vblayout);
	//accept file drop events
	setAcceptDrops(true);
	/** Setup Main Layout and add the glwidget to it **/
	// layout = new QBoxLayout( QBoxLayout::LeftToRight, this );
	// layout->addWidget(active);
	// layout->setMargin(0);
	// vblayout->setMargin(0);
	// cWidget->setLayout( layout );
	setCentralWidget( active);
	//store user's desktop size
	QWidget *d = QApplication::desktop();
	int w=d->width();  // returns screen width
  int h=d->height(); // returns screen height

#ifdef WITH_PYTHON
	//the script editor widget will be placed into a QDockWidget
	//and will be dockable in the top and bottom sections of the main window	
	mScriptEditorDockWidget = new QDockWidget(tr("Script Editor"), this);
	mScriptEditor = new DLFLScriptEditor( &object, mScriptEditorDockWidget );
	mScriptEditorDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	mScriptEditorDockWidget->setWidget(mScriptEditor);
	addDockWidget(Qt::BottomDockWidgetArea, mScriptEditorDockWidget);
	mScriptEditorDockWidget->hide();
	mScriptEditorDockWidget->setFloating(true);
	mScriptEditorDockWidget->setGeometry(20,h-320,500,300);
	//mScriptEditorDockWidget->setMaximumHeight(200);
	connect( this, SIGNAL(loadedObject(DLFLObject*,QString)),mScriptEditor, SLOT(loadObject(DLFLObject*,QString)) );
	connect( this, SIGNAL(echoCommand(QString)),mScriptEditor, SLOT(echoCommand(QString)) );
	connect( mScriptEditor, SIGNAL(makingChange()), this, SLOT(undoPush()) );
	connect( mScriptEditor, SIGNAL(cmdExecuted()), this, SLOT(recomputeAll()) );
	connect( mScriptEditor, SIGNAL(cmdExecuted()), this->getActive(), SLOT(update()) );
	connect( mScriptEditor, SIGNAL(requestObject(QString)), this, SLOT(openFile(QString)) );

	if( !Py_IsInitialized() )
		Py_Initialize( );
#endif

#ifdef WITH_VERSE
	mVerseDialog = VerseTopMod::Instance(this);
	mVerseDialogDockWidget = new QDockWidget(tr("Verse-TopMod"), this);
	mVerseDialogDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	mVerseDialogDockWidget->setWidget(mVerseDialog);
	addDockWidget(Qt::BottomDockWidgetArea, mVerseDialogDockWidget);
	mVerseDialogDockWidget->hide();
	mVerseDialogDockWidget->setMaximumHeight(200);
#endif

	//Tool options dockwidget must be initialized before setting up the actions
	//the main tool options DockWidget
	mToolOptionsDockWidget = new QDockWidget(tr("Tool Options - Insert Edge"),this);
	// mToolOptionsDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable /* | QDockWidget::DockWidgetClosable*/);
	mToolOptionsDockWidget->setAllowedAreas(Qt::NoDockWidgetArea);
	mToolOptionsStackedWidget = new QStackedWidget();
	// mToolOptionsDockWidget->hide();
	mToolOptionsDockWidget->setWidget(mToolOptionsStackedWidget);
	mToolOptionsDockWidget->setFloating(true);
	// mToolOptionsDockWidget->move(width() , 150);
	// mToolOptionsTitleBarLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	// mToolOptionsTitleBarWidget = new QWidget;
	// mToolOptionsTitleBarWidget->setLayout(mToolOptionsTitleBarLayout);
	// mToolOptionsDockWidget->setTitleBarWidget(mToolOptionsTitleBarWidget);
	// mToolOptionsDockWidget->titleBarWidget()->setContentsMargins(0,0,0,0);
	// mToolOptionsDockWidget->move(0,22);
	// mToolOptionsDockWidget->setWindowTitle("Insert Edge Mode");
	// addDockWidget(Qt::TopDockWidgetArea, mToolOptionsDockWidget);
	
	//animated help dockwidget
	initializeAnimatedHelp();
	
	//make a new instance of QShortcutManager
	sm = new QShortcutManager();
	mActionModel = new QStandardItemModel();
	mActionListWidget = new QWidget;

	//instantiate toolbars
	mBasicsMode = new BasicsMode(this, sm, mActionListWidget);
	mExtrusionMode = new ExtrusionMode(this, sm, mActionListWidget );
	// mConicalMode = new ConicalMode(this, sm);
	mRemeshingMode = new RemeshingMode(this, sm, mActionListWidget);
	mHighgenusMode = new HighgenusMode(this, sm, mActionListWidget);
	mTexturingMode = new TexturingMode(this, sm, mActionListWidget);

	//startup dialog
	mStartupDialogDockWidget = new QDockWidget(tr("Learning Movies"),this);
	
	createActions();
	mCommandCompleter = new CommandCompleter(mActionListWidget, this);
	createToolBars();
	createMenus();
	//initialize the help file...
	initializeHelp();
	//style sheet editor
	mStyleSheetEditor = new StyleSheetEditor;
	//preference dialog
	mSettings = new QSettings("TopMod", "Topological Mesh Modeler");
	mPreferencesDialog = new TopModPreferences(mSettings, mStyleSheetEditor, sm, this);	
	
	// //testing shortcut context
	// for (int i = 0; i < mActionListWidget->actions().count(); i++){
	// 	mActionListWidget->actions().at(i)->setShortcutContext(Qt::WidgetShortcut);
	// }
	
	//reposition floating windows:
	mToolOptionsDockWidget->setGeometry(10 + this->x()/*this->active->width()-mToolOptionsDockWidget->width()*/,this->y()+150,mToolOptionsDockWidget->width(),mToolOptionsDockWidget->height());
	// mToolOptionsDockWidget->setFixedSize(mToolOptionsDockWidget->width(),mToolOptionsDockWidget->height());
	
	//must happen after preference file is loaded
	createStartupDialog();
	
}

void MainWindow::createActions() {

	//File Menu Actions
	mOpenAct = new QAction(QIcon(":images/open.png"),tr("&Open..."), this);
	sm->registerAction(mOpenAct, "File Menu", "CTRL+O");
	mOpenAct->setStatusTip(tr("Open an existing file"));
	connect(mOpenAct, SIGNAL(triggered()), this, SLOT(openFile()));
	mActionListWidget->addAction(mOpenAct);
	
	mSaveAct = new QAction(QIcon(":images/saveas.png"),tr("&Save"), this);
	sm->registerAction(mSaveAct, "File Menu", "CTRL+S");
	mSaveAct->setStatusTip(tr("Save the document to disk"));
	connect(mSaveAct, SIGNAL(triggered()), this, SLOT(saveFile()));
	mActionListWidget->addAction(mSaveAct);

	mSaveAsAct = new QAction(QIcon(":images/saveas.png"),tr("Save &As..."), this);
	sm->registerAction(mSaveAsAct, "File Menu", "CTRL+SHIFT+S");
	mSaveAsAct->setStatusTip(tr("Save the document under a new name"));
	connect(mSaveAsAct, SIGNAL(triggered()), this, SLOT(saveFileAs()));
	mActionListWidget->addAction(mSaveAsAct);

	mSavePatchesAct = new QAction(QIcon(":images/saveas.png"),tr("Save &Patch OBJ..."), this);
	sm->registerAction(mSavePatchesAct, "File Menu", "");
	mSavePatchesAct->setStatusTip(tr("Save a bezier patch .obj file"));
	connect(mSavePatchesAct, SIGNAL(triggered()), this, SLOT(saveFileBezierOBJ()));
	mActionListWidget->addAction(mSavePatchesAct);

	loadTextureAct = new QAction(tr("Load &Texture..."), this);
	sm->registerAction(loadTextureAct, "File Menu", "CTRL+L");
	loadTextureAct->setStatusTip(tr("Load Texture from file"));
	connect(loadTextureAct, SIGNAL(triggered()), this, SLOT(load_texture()));
	mActionListWidget->addAction(loadTextureAct);

	printInfoAct = new QAction(tr("Print &Information"), this);
	sm->registerAction(printInfoAct, "File Menu", "CTRL+P");
	printInfoAct->setStatusTip(tr("Print Information to the console"));
	connect(printInfoAct, SIGNAL(triggered()), this, SLOT(printSummary()));
	mActionListWidget->addAction(printInfoAct);

	printFaceListAct = new QAction(tr("Print &Face List"), this);
	sm->registerAction(printFaceListAct, "File Menu", "CTRL+ALT+P");
	printFaceListAct->setStatusTip(tr("Print Face List to the console"));
	// connect(printFaceListAct, SIGNAL(triggered()), this, SLOT(saveAs()));
	mActionListWidget->addAction(printFaceListAct);

	printVertexListAct = new QAction(tr("Save &As..."), this);
	sm->registerAction(printVertexListAct, "File Menu", "CTRL+SHIFT+P");
	printVertexListAct->setStatusTip(tr("Save the document under a new name"));
	// connect(printVertexListAct, SIGNAL(triggered()), this, SLOT(saveAs()));
	mActionListWidget->addAction(printVertexListAct);

	printEdgeListAct = new QAction(tr("Print &Edge List"), this);
	sm->registerAction(printEdgeListAct, "File Menu", "CTRL+ALT+SHIFT+P");
	printEdgeListAct->setStatusTip(tr("Print Edge list to the console"));
	// connect(printEdgeListAct, SIGNAL(triggered()), this, SLOT(saveAs()));
	mActionListWidget->addAction(printEdgeListAct);

	mPrintCVListAct = new QAction(tr("Print &CV List"), this);
	sm->registerAction(mPrintCVListAct, "File Menu", "");
	mPrintCVListAct->setStatusTip(tr("Print CV list to the console"));
	// connect(mPrintCVListAct, SIGNAL(triggered()), this, SLOT(saveAs()));
	mActionListWidget->addAction(mPrintCVListAct);

	mExitAct = new QAction(tr("E&xit"), this);
	sm->registerAction(mExitAct, "File Menu", "CTRL+Q");
	mExitAct->setStatusTip(tr("Exit the application"));
	connect(mExitAct, SIGNAL(triggered()), this, SLOT(close()));
	// mActionListWidget->addAction(mExitAct);
	
	//quick command quicksilver like interface
	mQuickCommandAct = new QAction(tr("Quick Command"), this);
	QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
	connect(shortcut, SIGNAL(activated()), this, SLOT(getCommand()));
	mQuickCommandAct->setStatusTip(tr("Quick Command Access with Autocompletion"));
	connect(mQuickCommandAct, SIGNAL(triggered()), this, SLOT(getCommand()));
	// mActionListWidget->addAction(mQuickCommandAct);
	
	//Edit Menu Actions
	mUndoAct = new QAction(QIcon(":images/undo.png"), tr("&Undo"), this);
	sm->registerAction(mUndoAct, "Edit Menu", "CTRL+Z");
	mUndoAct->setStatusTip(tr("Undo the last operation"));
	connect(mUndoAct, SIGNAL(triggered()), this, SLOT(undo()));
	mActionListWidget->addAction(mUndoAct);

	mRedoAct = new QAction(QIcon(":images/redo.png"), tr("&Redo"), this);
	sm->registerAction(mRedoAct, "Edit Menu", "CTRL+SHIFT+Z");
	mUndoAct->setStatusTip(tr("Redo the last operation"));
	connect(mRedoAct, SIGNAL(triggered()), this, SLOT(redo()));
	mActionListWidget->addAction(mRedoAct);

	mClearUndoListAct = new QAction(tr("&Clear Undo List"), this);
	sm->registerAction(mClearUndoListAct, "Edit Menu", "CTRL+SHIFT+Z");
	mClearUndoListAct->setStatusTip(tr("Clear the Undo List to free up some memory"));
	connect(mClearUndoListAct, SIGNAL(triggered()), this, SLOT(clearUndoList()));
	mActionListWidget->addAction(mClearUndoListAct);

	//View Menu Actions
	mPerspViewAct = new QAction( tr("&Reset Camera"), this);
	sm->registerAction(mPerspViewAct, "View Menu", "F");
	mPerspViewAct->setStatusTip(tr("Reset Camera Position to default"));
	connect(mPerspViewAct, SIGNAL(triggered()), this, SLOT(switchPerspView()));
	mActionListWidget->addAction(mPerspViewAct);

	mTopViewAct = new QAction( tr("&Top View"), this);
	sm->registerAction(mTopViewAct, "View Menu", "");
	mTopViewAct->setStatusTip(tr("Switch to Top View"));
	connect(mTopViewAct, SIGNAL(triggered()), this, SLOT(switchTopView()));
	// mActionListWidget->addAction(mTopViewAct);

	mBottomViewAct = new QAction( tr("&Bottom View"), this);
	sm->registerAction(mBottomViewAct, "View Menu", "");
	mBottomViewAct->setStatusTip(tr("Switch to Bottom View"));
	connect(mBottomViewAct, SIGNAL(triggered()), this, SLOT(switchBottomView()));
	// mActionListWidget->addAction(mBottomViewAct);

	mFrontViewAct = new QAction( tr("&Front View"), this);
	sm->registerAction(mFrontViewAct, "View Menu", "");
	mFrontViewAct->setStatusTip(tr("Switch to Front View"));
	connect(mFrontViewAct, SIGNAL(triggered()), this, SLOT(switchFrontView()));
	// mActionListWidget->addAction(mFrontViewAct);

	mBackViewAct = new QAction( tr("B&ack View"), this);
	sm->registerAction(mBackViewAct, "View Menu", "");
	mBackViewAct->setStatusTip(tr("Switch to Back View"));
	connect(mBackViewAct, SIGNAL(triggered()), this, SLOT(switchBackView()));
	// mActionListWidget->addAction(mBackViewAct);

	mLeftViewAct = new QAction( tr("&Left View"), this);
	sm->registerAction(mLeftViewAct, "View Menu", "");
	mLeftViewAct->setStatusTip(tr("Switch to Left View"));
	connect(mLeftViewAct, SIGNAL(triggered()), this, SLOT(switchLeftView()));
	// mActionListWidget->addAction(mLeftViewAct);

	mRightViewAct = new QAction( tr("&Right View"), this);
	sm->registerAction(mRightViewAct, "View Menu", "");
	mRightViewAct->setStatusTip(tr("Switch to Right View"));
	connect(mRightViewAct, SIGNAL(triggered()), this, SLOT(switchRightView()));
	// mActionListWidget->addAction(mRightViewAct);

	//Display Menu Actions
	mFullscreenAct = new QAction(tr("&Full Screen"), this);
	mFullscreenAct->setCheckable(true);
	sm->registerAction( mFullscreenAct, "Display Menu", "M");
	mFullscreenAct->setStatusTip(tr("Toggle Full Screen"));
	connect(mFullscreenAct, SIGNAL(triggered()), active, SLOT(toggleFullScreen()) );
	mActionListWidget->addAction(mFullscreenAct);

	showVerticesAct = new QAction(tr("Show &Vertices"), this);
	showVerticesAct->setCheckable(true);
	sm->registerAction(showVerticesAct, "Display Menu", "V");
	// showVerticesAct->setStatusTip(tr("Copy the current selection's contents to the "
	connect(showVerticesAct, SIGNAL(triggered()), this->getActive(), SLOT(toggleVertices()));
	mActionListWidget->addAction(showVerticesAct);

	mShowFaceIDsAct = new QAction(tr("Show &Face IDs"), this);
	mShowFaceIDsAct->setCheckable(true);
	sm->registerAction(mShowFaceIDsAct, "Display Menu", "CTRL+F");
	connect(mShowFaceIDsAct, SIGNAL(triggered()), this->getActive(), SLOT(toggleFaceIDs()));
	mActionListWidget->addAction(mShowFaceIDsAct);

	mShowEdgeIDsAct = new QAction(tr("Show &Edge IDs"), this);
	mShowEdgeIDsAct->setCheckable(true);
	sm->registerAction(mShowEdgeIDsAct, "Display Menu", "CTRL+E");
	connect(mShowEdgeIDsAct, SIGNAL(triggered()), this->getActive(), SLOT(toggleEdgeIDs()));
	mActionListWidget->addAction(mShowEdgeIDsAct);

	mShowVertexIDsAct = new QAction(tr("Show &Vertex IDs"), this);
	mShowVertexIDsAct->setCheckable(true);
	sm->registerAction(mShowVertexIDsAct, "Display Menu", "CTRL+V");
	connect(mShowVertexIDsAct, SIGNAL(triggered()), this->getActive(), SLOT(toggleVertexIDs()));
	mActionListWidget->addAction(mShowVertexIDsAct);

	mShowSelectedIDsAct = new QAction(tr("Show &Selected IDs"), this);
	mShowSelectedIDsAct->setCheckable(true);
	sm->registerAction(mShowSelectedIDsAct, "Display Menu", "SHIFT+S");
	connect(mShowSelectedIDsAct, SIGNAL(triggered()), this->getActive(), SLOT(toggleSelectedIDs()));
	mActionListWidget->addAction(mShowSelectedIDsAct);

	showSilhouetteAct = new QAction(tr("Show &Silhouette"), this);
	showSilhouetteAct->setCheckable(true);
	sm->registerAction(showSilhouetteAct, "Display Menu", "S" );
	// showSilhouetteAct->setStatusTip((tr"Blah"));
	connect(showSilhouetteAct, SIGNAL(triggered()), this->getActive(), SLOT(toggleSilhouette()));
	mActionListWidget->addAction(showSilhouetteAct);

	showWireframeAct = new QAction(tr("Show &Wireframe"), this);
	showWireframeAct->setCheckable(true);
	showWireframeAct->setChecked(true);
	sm->registerAction(showWireframeAct, "Display Menu", "W");
	// showWireframeAct->setStatusTip(tr("Copy the current selection's contents to the "
	connect(showWireframeAct, SIGNAL(triggered()), this->getActive(), SLOT(toggleWireframe()));
	mActionListWidget->addAction(showWireframeAct);

	showCoordinateAxesAct = new QAction(tr("Show &Coordinate Axes"), this);
	showCoordinateAxesAct->setCheckable(true);
	sm->registerAction(showCoordinateAxesAct, "Display Menu", "X");
	// showCoordinateAxesAct->setStatusTip(tr("Copy the current selection's contents to the "
	connect(showCoordinateAxesAct, SIGNAL(triggered()), this->getActive(), SLOT(toggleAxes()));
	mActionListWidget->addAction(showCoordinateAxesAct);

	objectOrientationAct = new QAction(tr("Reverse Object"), this);
	objectOrientationAct->setCheckable(true);
	sm->registerAction(objectOrientationAct, "Display Menu", "R");
	// objectOrienationAct->setStatusTip(tr("Copy the current selection's contents to the "
	connect(objectOrientationAct, SIGNAL(triggered()), this->getActive(), SLOT(toggleObjectOrientation()));
	mActionListWidget->addAction(objectOrientationAct);

	mShowNormalsAct = new QAction(tr("Show &Normals"), this);
	mShowNormalsAct->setCheckable(true);
	sm->registerAction(mShowNormalsAct, "Display Menu", "N");
	// objectOrienationAct->setStatusTip(tr("Copy the current selection's contents to the "
	connect(mShowNormalsAct, SIGNAL(triggered()), this->getActive(), SLOT(toggleNormals()));
	mActionListWidget->addAction(mShowNormalsAct);

	mShowFaceCentroidsAct = new QAction(tr("Show &Face Centroids"), this);
	mShowFaceCentroidsAct->setCheckable(true);
	sm->registerAction(mShowFaceCentroidsAct, "Display Menu", "C");
	// objectOrienationAct->setStatusTip(tr("Copy the current selection's contents to the "
	connect(mShowFaceCentroidsAct, SIGNAL(triggered()), this->getActive(), SLOT(toggleFaceCentroids()));
	mActionListWidget->addAction(mShowFaceCentroidsAct);

	showGridAct = new QAction(tr("Show &Grid"), this);
	showGridAct->setCheckable(true);
	showGridAct->setEnabled(false);
	sm->registerAction(showGridAct, "Display Menu", "G");
	// showGridAct->setStatusTip(tr("Copy the current selection's contents to the "
	connect(showGridAct, SIGNAL(triggered()), getActive(), SLOT(toggleGrid()));
	mActionListWidget->addAction(showGridAct);

	showHUDAct = new QAction(tr("Show &Heads Up Display"), this);
	showHUDAct->setCheckable(true);
	sm->registerAction(showHUDAct, "Display Menu", "H");
	showHUDAct->setStatusTip(tr("Show the Heads Up Display"));
	connect(showHUDAct, SIGNAL(triggered()), this->getActive(), SLOT(toggleHUD()));
	mActionListWidget->addAction(showHUDAct);

	#ifdef GPU_OK
	mUseGPUAct = new QAction(tr("&Use GPU Shading"), this);
	mUseGPUAct->setCheckable(true);
	mUseGPUAct->setChecked(true);	
	sm->registerAction(mUseGPUAct, "Display Menu", "L");
	mUseGPUAct->setStatusTip(tr("Use GPU Shading"));
	connect(mUseGPUAct, SIGNAL(triggered()), this->getActive(), SLOT(toggleGPU()));
	mActionListWidget->addAction(mUseGPUAct);
	#endif
	
	mAntialiasingAct = new QAction(tr("Toggle &Antialiasing"), this);
	mAntialiasingAct->setCheckable(true);
	mAntialiasingAct->setChecked(false);	
	sm->registerAction(mAntialiasingAct, "Display Menu", "K");
	mAntialiasingAct->setStatusTip(tr("Toggle Antialiasing"));
	connect(mAntialiasingAct, SIGNAL(triggered()), this->getActive(), SLOT(toggleAntialiasing()));
	mActionListWidget->addAction(mAntialiasingAct);

#ifdef WITH_PYTHON
	mShowScriptEditorAct = mScriptEditorDockWidget->toggleViewAction();//new QAction(tr("Show Script &Editor"), this);
	mShowScriptEditorAct->setStatusTip( tr("Show the script editor to execute DLFL commands") );
	sm->registerAction(mShowScriptEditorAct, "Window Menu", "SHIFT+CTRL+E");
	mActionListWidget->addAction(mShowScriptEditorAct);
#endif

#ifdef WITH_VERSE
	mShowVerseDialogAct = mVerseDialogDockWidget->toggleViewAction();//new QAction(tr("Show Verse &Dialog"), this);
	// mShowVerseDialogAct->setText(// true);
	mShowVerseDialogAct->setStatusTip( tr("Show the verse dialog to view verse server connection status") );
	// connect(mShowVerseDialogAct, SIGNAL(triggered()), this, SLOT(showHideVerseDialog()));
	sm->registerAction(mShowVerseDialogAct, "Window Menu", "SHIFT+CTRL+V");
	mActionListWidget->addAction(mShowVerseDialogAct);
#endif	

	mShowToolOptionsAct = mToolOptionsDockWidget->toggleViewAction();
	mShowToolOptionsAct->setStatusTip( tr("Show the tool options window") );
	sm->registerAction(mShowToolOptionsAct, "Window Menu", "O");
	mActionListWidget->addAction(mShowToolOptionsAct);

	mShowStartupDialogAct = mStartupDialogDockWidget->toggleViewAction();
	mShowStartupDialogAct->setStatusTip( tr("Show the startup screen with links to video tutorials") );
	sm->registerAction(mShowStartupDialogAct, "Window Menu", "SHIFT+S");
	mActionListWidget->addAction(mShowStartupDialogAct);
	
	mShowAnimatedHelpAct = mAnimatedHelpDockWidget->toggleViewAction();// new QAction(tr("Show Animated Hel&p"), this);
	// mShowAnimatedHelpAct->setCheckable(true);
	mShowAnimatedHelpAct->setStatusTip( tr("Show/Hide the animated help window") );
	// connect(mShowAnimatedHelpAct, SIGNAL(triggered()), this, SLOT(showHideAnimatedHelp()));
	sm->registerAction(mShowAnimatedHelpAct, "Window Menu", "SHIFT+H");
	mActionListWidget->addAction(mShowAnimatedHelpAct);

		//Renderer Menu Actions
	wireframeRendererAct = new QAction(tr("&Wireframe Renderer"), this);
	wireframeRendererAct->setCheckable(true);
	sm->registerAction(wireframeRendererAct, "Renderer Menu", "1");
	wireframeRendererAct->setStatusTip(tr("Switch the current renderer to Wireframe"));
	connect(wireframeRendererAct, SIGNAL(triggered()), this, SLOT(useWireframeRenderer()));	
	mActionListWidget->addAction(wireframeRendererAct);

	normalRendererAct = new QAction(tr("&Normal Renderer"), this);
	normalRendererAct->setCheckable(true);
	sm->registerAction(normalRendererAct, "Renderer Menu", "2");
	normalRendererAct->setStatusTip(tr("Switch the current renderer to Normal"));
	connect(normalRendererAct, SIGNAL(triggered()), this, SLOT(useNormalRenderer()));
	mActionListWidget->addAction(normalRendererAct);

	lightedRendererAct = new QAction(tr("&Lighted Renderer"), this);
	lightedRendererAct->setCheckable(true);
	sm->registerAction(lightedRendererAct, "Renderer Menu", "3");
	lightedRendererAct->setStatusTip(tr("Switch the current renderer to Lighted"));
	connect(lightedRendererAct, SIGNAL(triggered()), this, SLOT(useLightedRenderer()));
	mActionListWidget->addAction(lightedRendererAct);

	texturedRendererAct = new QAction(tr("&Textured Renderer"), this);
	texturedRendererAct->setCheckable(true);
	sm->registerAction(texturedRendererAct, "Renderer Menu", "4");
	texturedRendererAct->setStatusTip(tr("Switch the current renderer to Textured"));
	connect(texturedRendererAct, SIGNAL(triggered()), this, SLOT(useTexturedRenderer()));
	mActionListWidget->addAction(texturedRendererAct);

	texturedLightedAct = new QAction(tr("Te&xtured Lighted Renderer"), this);
	texturedLightedAct->setCheckable(true);
	sm->registerAction(texturedLightedAct, "Renderer Menu", "5");
	texturedLightedAct->setStatusTip(tr("Switch the current renderer to Textured Lit"));
	connect(texturedLightedAct, SIGNAL(triggered()), this, SLOT(useTexturedLitRenderer()));
	mActionListWidget->addAction(texturedLightedAct);

	patchRendererAct = new QAction(tr("&Patch Renderer"), this);
	patchRendererAct->setCheckable(true);
	sm->registerAction(patchRendererAct, "Renderer Menu", "6");
	patchRendererAct->setStatusTip(tr("Switch the current renderer to Patch"));
	connect(patchRendererAct, SIGNAL(triggered()), this, SLOT(usePatchRenderer()));
	mActionListWidget->addAction(patchRendererAct);

	mRendererActionGroup = new QActionGroup(this);
	mRendererActionGroup->setExclusive(true);
	mRendererActionGroup->addAction(wireframeRendererAct);
	mRendererActionGroup->addAction(normalRendererAct);
	mRendererActionGroup->addAction(lightedRendererAct);
	mRendererActionGroup->addAction(texturedRendererAct);
	mRendererActionGroup->addAction(texturedLightedAct);
	mRendererActionGroup->addAction(patchRendererAct);
	lightedRendererAct->setChecked(true);

	//PRIMITIVES MENU ACTIONS
	pCubeAct = new QAction(QIcon(":/images/prim_cube.png"), tr("&Cube"), this);
	sm->registerAction(pCubeAct, "Primitives Menu", "CTRL+1");
	pCubeAct->setStatusTip(tr("Load a Cube"));
	connect(pCubeAct, SIGNAL(triggered()), this, SLOT(loadCube()));
	mActionListWidget->addAction(pCubeAct);

	pOctahedronAct = new QAction(QIcon(":/images/prim_octahedron.png"), tr("&Octahedron"), this);
	sm->registerAction(pOctahedronAct, "Primitives Menu", "CTRL+2");
	pOctahedronAct->setStatusTip(tr("Load an octahedron"));
	connect(pOctahedronAct, SIGNAL(triggered()), this, SLOT(loadOctahedron()));
	mActionListWidget->addAction(pOctahedronAct);

	pTetrahedronAct = new QAction(QIcon(":/images/prim_tetrahedron.png"), tr("&Tetrahedron"), this);
	sm->registerAction(pTetrahedronAct, "Primitives Menu", "CTRL+3");
	pTetrahedronAct->setStatusTip(tr("Load a tetrahedron"));
	connect(pTetrahedronAct, SIGNAL(triggered()), this, SLOT(loadTetrahedron()));
	mActionListWidget->addAction(pTetrahedronAct);

	pDodecahedronAct = new QAction(QIcon(":/images/prim_dodecahedron.png"), tr("&Dodecahedron"), this);
	sm->registerAction(pDodecahedronAct, "Primitives Menu", "CTRL+4");
	pDodecahedronAct->setStatusTip(tr("Load a dodecahedron"));
	connect(pDodecahedronAct, SIGNAL(triggered()), this, SLOT(loadDodecahedron()));
	mActionListWidget->addAction(pDodecahedronAct);

	pIcosahedronAct = new QAction(QIcon(":/images/prim_icosahedron.png"), tr("&Icosahedron"), this);
	sm->registerAction(pIcosahedronAct, "Primitives Menu", "CTRL+5");
	pIcosahedronAct->setStatusTip(tr("Load an icosahedron"));
	connect(pIcosahedronAct, SIGNAL(triggered()), this, SLOT(loadIcosahedron()));
	mActionListWidget->addAction(pIcosahedronAct);

	pSoccerBallAct = new QAction(QIcon(":/images/prim_soccerball.png"), tr("&Soccer ball"), this);
	sm->registerAction(pSoccerBallAct, "Primitives Menu", "CTRL+6");
	pSoccerBallAct->setStatusTip(tr("Load a soccer ball"));
	connect(pSoccerBallAct, SIGNAL(triggered()), this, SLOT(loadSoccerball()));
	mActionListWidget->addAction(pSoccerBallAct);

	pGeodesicAct = new QAction(QIcon(":/images/prim_geodesic.png"), tr("&Geodesic Dome"), this);
	sm->registerAction(pGeodesicAct, "Primitives Menu", "CTRL+7");
	pGeodesicAct->setStatusTip(tr("Load a geodesic dome"));
	connect(pGeodesicAct, SIGNAL(triggered()), this, SLOT(loadGeodesic()));
	mActionListWidget->addAction(pGeodesicAct);

	//Object Menu Actions
	subdivideAllEdgesAct = new QAction(tr("Subdivide All &Edges"), this);
	sm->registerAction(subdivideAllEdgesAct, "Settings", "");
	// subdivideAllEdgesAct->setStatusTip(tr("Copy the current selection's contents to the "
	connect(subdivideAllEdgesAct, SIGNAL(triggered()), this, SLOT(subdivideAllEdges()));
	mActionListWidget->addAction(subdivideAllEdgesAct);

	planarizeAllFacesAct = new QAction(tr("Planarize All &Faces"), this);
	sm->registerAction(planarizeAllFacesAct, "Settings", "");
	// planarizeAllFacesAct->setStatusTip(tr("Copy the current selection's contents to the "
	connect(planarizeAllFacesAct, SIGNAL(triggered()), this, SLOT(planarizeFaces()));
	mActionListWidget->addAction(planarizeAllFacesAct);

	makeObjectSphericalAct = new QAction(tr("Make &Object Spherical"), this);
	sm->registerAction(makeObjectSphericalAct, "Settings", "");
	// makeObjectSphericalAct->setStatusTip(tr("Copy the current selection's contents to the "
	connect(makeObjectSphericalAct, SIGNAL(triggered()), this, SLOT(spheralizeObject()));
	mActionListWidget->addAction(makeObjectSphericalAct);

	makeObjectSmoothAct = new QAction(tr("Make Object &Smooth"), this);
	sm->registerAction(makeObjectSmoothAct, "Settings", "");
	// makeObjectSmoothAct->setStatusTip(tr("Copy the current selection's contents to the "
	connect(makeObjectSmoothAct, SIGNAL(triggered()), this, SLOT(smoothMesh()));
	mActionListWidget->addAction(makeObjectSmoothAct);

	createCrustAct = new QAction(tr("&Create Crust"), this);
	sm->registerAction(createCrustAct, "Settings", "");
	// createCrustAct->setStatusTip(tr("Copy the current selection's contents to the "
	connect(createCrustAct, SIGNAL(triggered()), this, SLOT(createCrust()));
	mActionListWidget->addAction(createCrustAct);

	computeLightingAct = new QAction(tr("Compute &Lighting"), this);
	sm->registerAction(computeLightingAct, "Settings", "");
	// computeLightingAct->setStatusTip(tr("Copy the current selection's contents to the "
	connect(computeLightingAct, SIGNAL(triggered()), getActive(), SLOT(recomputeLighting()));
	mActionListWidget->addAction(computeLightingAct);

	computeNormalsAndLightingAct = new QAction(tr("Compute &Normals and Lighting"), this);
	sm->registerAction(computeNormalsAndLightingAct, "Settings", "");
	// computeNormalsAndLightingAct->setStatusTip(tr("Copy the current selection's contents to the "
	connect(computeNormalsAndLightingAct, SIGNAL(triggered()), getActive(), SLOT(recomputeNormals()));
	mActionListWidget->addAction(computeNormalsAndLightingAct);

	assignTextureCoordinatesAct = new QAction(tr("Assign &Texture Coordinates"), this);
	sm->registerAction(assignTextureCoordinatesAct, "Settings", "");
	// assignTextureCoordinatesAct->setStatusTip(tr("Copy the current selection's contents to the "
	connect(assignTextureCoordinatesAct, SIGNAL(triggered()), this, SLOT(assignTileTexCoords()));
	mActionListWidget->addAction(assignTextureCoordinatesAct);

	// //SELECTION MENU ACTIONS
	selectVertexAct = new QAction(tr("Select &Vertex"), this);
	sm->registerAction(selectVertexAct, "Selection", "SHIFT+V");
	selectVertexAct->setStatusTip(tr("Select a Vertex"));
	connect(selectVertexAct, SIGNAL(triggered()), this, SLOT(select_vertex()));
	mActionListWidget->addAction(selectVertexAct);

	mEditVertexAct = new QAction(tr("Edit Verte&x"), this);
	sm->registerAction(mEditVertexAct, "Selection", "SHIFT+V");
	mEditVertexAct->setStatusTip(tr("Select and Move Vertices One at a time."));
	connect(mEditVertexAct, SIGNAL(triggered()), this, SLOT(edit_vertex()));
	mActionListWidget->addAction(mEditVertexAct);

	selectFaceAct = new QAction(tr("Select &Face"), this);
	sm->registerAction(selectFaceAct, "Selection", "SHIFT+F");
	selectFaceAct->setStatusTip(tr("Select One Face. Just for practice. :)"));
	connect(selectFaceAct, SIGNAL(triggered()), this, SLOT(select_face() ) );
	mActionListWidget->addAction(selectFaceAct);

	selectFaceLoopAct = new QAction(tr("Select Face Loo&p"), this);
	sm->registerAction(selectFaceLoopAct, "Selection", "SHIFT+P");
	selectFaceLoopAct->setStatusTip(tr("Select a Face Loop."));
	connect(selectFaceLoopAct, SIGNAL(triggered()), this, SLOT(select_face_loop() ) );
	mActionListWidget->addAction(selectFaceLoopAct);

	selectMultipleFacesAct = new QAction(tr("Select &Multiple Faces"), this);
	sm->registerAction(selectMultipleFacesAct, "Selection", "SHIFT+G");
	connect( selectMultipleFacesAct , SIGNAL( triggered() ), this, SLOT( select_multiple_faces() ) );
	mActionListWidget->addAction(selectMultipleFacesAct);

	selectSimilarFacesAct = new QAction(tr("Select &Similar Faces"), this);
	sm->registerAction(selectSimilarFacesAct, "Selection", "SHIFT+G");
	connect( selectSimilarFacesAct , SIGNAL( triggered() ), this, SLOT( select_similar_faces() ) );
	mActionListWidget->addAction(selectSimilarFacesAct);

	selectCheckerboardFacesAct = new QAction(tr("C&heckerboard Select Faces"), this);
	sm->registerAction(selectCheckerboardFacesAct, "Selection", "");
	connect( selectCheckerboardFacesAct , SIGNAL( triggered() ), this, SLOT( select_checkerboard_faces() ) );
	mActionListWidget->addAction(selectCheckerboardFacesAct);

	selectAllAct = new QAction(tr("Select &All"), this);
	sm->registerAction(selectAllAct, "Selection", "CTRL+A");
	connect( selectAllAct , SIGNAL( triggered() ), this, SLOT( selectAll() ) );
	mActionListWidget->addAction(selectAllAct);

	selectInverseAct = new QAction(tr("Select &Inverse"), this);
	sm->registerAction(selectInverseAct, "Selection", "CTRL+I");
	connect( selectInverseAct , SIGNAL( triggered() ), this, SLOT( selectInverse() ) );
	mActionListWidget->addAction(selectInverseAct);

	selectEdgeAct = new QAction(tr("Select &Edge"), this);
	sm->registerAction(selectEdgeAct, "Selection", "SHIFT+E");
	selectEdgeAct->setStatusTip(tr("Select one Edge"));
	connect( selectEdgeAct , SIGNAL( triggered() ), this,SLOT( select_edge() ) );
	mActionListWidget->addAction(selectEdgeAct);

	selectEdgeLoopAct = new QAction(tr("Select Edge &Loop"), this);
	sm->registerAction(selectEdgeLoopAct, "Selection", "SHIFT+L");
	selectEdgeLoopAct->setStatusTip(tr("Select an Edge Loop"));
	connect( selectEdgeLoopAct , SIGNAL( triggered() ), this,SLOT( select_edge_loop() ) );
	mActionListWidget->addAction(selectEdgeLoopAct);

	selectCornerAct = new QAction(tr("Select &Corner"), this);
	sm->registerAction(selectCornerAct, "Selection", "SHIFT+C");
	selectCornerAct->setStatusTip(tr("Select a Face Vertex"));
	connect( selectCornerAct , SIGNAL( triggered() ), this, SLOT( select_corner() ) );
	mActionListWidget->addAction(selectCornerAct);

	exitSelectionModeAct = new QAction(tr("E&xit Selection Mode"), this);
	sm->registerAction(exitSelectionModeAct, "Selection", "SHIFT+X");
	connect( exitSelectionModeAct , SIGNAL( triggered() ), this, SLOT( exit_selection_mode() ) );
	mActionListWidget->addAction(exitSelectionModeAct);

	clearSelectedModeAct = new QAction(tr("&Clear Selected"), this);
	sm->registerAction(clearSelectedModeAct, "Selection", "Escape");
	connect( clearSelectedModeAct , SIGNAL( triggered() ), this, SLOT( clear_selected() ) );
	mActionListWidget->addAction(clearSelectedModeAct);

	mSelectVerticesMaskAct = new QAction(tr("Select &Vertices"), this);
	mSelectVerticesMaskAct->setCheckable(true);
	sm->registerAction(mSelectVerticesMaskAct, "Selection", "");
	mSelectVerticesMaskAct->setStatusTip(tr("Select by Component type: Vertices"));
	connect( mSelectVerticesMaskAct , SIGNAL( triggered() ), this, SLOT( selectionMaskVertices() ) );
	mActionListWidget->addAction(mSelectVerticesMaskAct);

	mSelectEdgesMaskAct = new QAction(tr("Select &Edges"), this);
	mSelectEdgesMaskAct->setCheckable(true);
	sm->registerAction(mSelectEdgesMaskAct, "Selection", "");
	mSelectEdgesMaskAct->setStatusTip(tr("Select by Component type: Edges"));
	connect( mSelectEdgesMaskAct , SIGNAL( triggered() ), this, SLOT( selectionMaskEdges() ) );
	mActionListWidget->addAction(mSelectEdgesMaskAct);

	mSelectFacesMaskAct = new QAction(tr("Select &Faces"), this);
	mSelectFacesMaskAct->setCheckable(true);
	sm->registerAction(mSelectFacesMaskAct, "Selection", "");
	mSelectFacesMaskAct->setStatusTip(tr("Select by Component type: Faces"));
	connect( mSelectFacesMaskAct , SIGNAL( triggered() ), this, SLOT( selectionMaskFaces() ) );
	mActionListWidget->addAction(mSelectFacesMaskAct);

	mSelectFaceVerticesMaskAct = new QAction(tr("Select &Face Vertex"), this);
	mSelectFaceVerticesMaskAct->setCheckable(true);
	sm->registerAction(mSelectFaceVerticesMaskAct, "Selection", "");
	mSelectFaceVerticesMaskAct->setStatusTip(tr("Select by Component type: Face-Vertices"));
	connect( mSelectFaceVerticesMaskAct , SIGNAL( triggered() ), this, SLOT( selectionMaskCorners() ) );
	mActionListWidget->addAction(mSelectFaceVerticesMaskAct);

	mSelectionMaskActionGroup = new QActionGroup(this);
	mSelectionMaskActionGroup->setExclusive(true);
	mSelectionMaskActionGroup->addAction(mSelectVerticesMaskAct);
	mSelectionMaskActionGroup->addAction(mSelectFacesMaskAct);
	mSelectionMaskActionGroup->addAction(mSelectEdgesMaskAct);
	mSelectionMaskActionGroup->addAction(mSelectFaceVerticesMaskAct);
	mSelectFaceVerticesMaskAct->setChecked(true);

	//SETTINGS ACTIONS	
	mPreferencesAct = new QAction(tr("&Preferences"), this);
	sm->registerAction(mPreferencesAct, "Settings", "CTRL+,");
	mPreferencesAct->setStatusTip(tr("Open the Preferences Dialog"));
	connect(mPreferencesAct, SIGNAL(triggered()), this, SLOT(openPreferences()));	
	mActionListWidget->addAction(mPreferencesAct);

	//LANGUAGE MENU BAR ACTIONS
	englishAct = new QAction(tr("English"),this);
	//sm->connect( englishAct , SIGNAL( triggered() ), SLOT  ( configure() ) );
	sm->registerAction(englishAct, "Languages", "CTRL+F7");
	mActionListWidget->addAction(englishAct);

	spanishAct = new QAction(tr("Spanish"),this);
	//sm->connect( spanishAct , SIGNAL( triggered() ), SLOT  ( configure() ) );
	sm->registerAction(spanishAct, "Languages", "CTRL+F8");
	mActionListWidget->addAction(spanishAct);

	germanAct = new QAction(tr("German"),this);
	//sm->connect( germanAct , SIGNAL( triggered() ), SLOT  ( configure() ) );
	sm->registerAction(germanAct, "Languages", "CTRL+F9");
	mActionListWidget->addAction(germanAct);

	frenchAct = new QAction(tr("French"),this);
	//sm->connect( frenchAct , SIGNAL( triggered() ), SLOT  ( configure() ) );
	sm->registerAction(frenchAct, "Languages", "CTRL+F10");
	mActionListWidget->addAction(frenchAct);

	turkishAct = new QAction(tr("Turkish"),this);
	//sm->connect( turkishAct , SIGNAL( triggered() ), SLOT  ( configure() ) );
	sm->registerAction(turkishAct, "Languages", "CTRL+F11");
	mActionListWidget->addAction(turkishAct);

	catalanAct = new QAction(tr("Catalan"),this);
	// sm->connect( catalanAct , SIGNAL( triggered() ), SLOT  ( configure() ) );
	sm->registerAction(catalanAct, "Languages", "CTRL+F12");
	mActionListWidget->addAction(catalanAct);

#ifdef WITH_VERSE
	//verse menu actions
	mVerseConnectLocalhostAct = new QAction(tr("Connect to localhost..."), this);
	mVerseConnectLocalhostAct->setStatusTip( tr("Connect to localhost") );
	connect(mVerseConnectLocalhostAct, SIGNAL(triggered()), mVerseDialog, SLOT(connectLocalhost()));
	sm->registerAction(mVerseConnectLocalhostAct, "Verse Menu", "");
	mActionListWidget->addAction(mVerseConnectLocalhostAct);

	mVerseConnectAct = new QAction(tr("Connect to host..."), this);
	mVerseConnectAct->setStatusTip( tr("Connect to host...") );
	connect(mVerseConnectAct, SIGNAL(triggered()), mVerseDialog, SLOT(connectHost()));
	sm->registerAction(mVerseConnectAct, "Verse Menu", "");
	mActionListWidget->addAction(mVerseConnectAct);

	mVerseDisconnectAct = new QAction(tr("Disconnect session"), this);
	mVerseDisconnectAct->setStatusTip( tr("Disconnect Verse Session") );
	connect(mVerseDisconnectAct, SIGNAL(triggered()), mVerseDialog, SLOT(disconnectHost()));
	sm->registerAction(mVerseDisconnectAct, "Verse Menu", "");
	mActionListWidget->addAction(mVerseDisconnectAct);

	mVerseDisconnectAllAct = new QAction(tr("Disconnect All Sessions"), this);
	mVerseDisconnectAllAct->setStatusTip( tr("Disconnect All Sessions") );
	connect(mVerseDisconnectAllAct, SIGNAL(triggered()), mVerseDialog, SLOT(disconnectAll()));
	sm->registerAction(mVerseDisconnectAllAct, "Verse Menu", "");
	mActionListWidget->addAction(mVerseDisconnectAllAct);

	mVerseStartServerAct = new QAction(tr("Start Verse Server"), this);
	mVerseStartServerAct->setStatusTip( tr("Disconnect All Nodes") );
	connect(mVerseStartServerAct, SIGNAL(triggered()), mVerseDialog, SLOT(startServer()));
	sm->registerAction(mVerseStartServerAct, "Verse Menu", "");
	mActionListWidget->addAction(mVerseStartServerAct);

	mVerseKillServerAct = new QAction(tr("Kill Verse Server"), this);
	mVerseKillServerAct->setStatusTip( tr("Kill the Local Verse server process") );
	connect(mVerseKillServerAct, SIGNAL(triggered()), mVerseDialog, SLOT(killServer()));
	sm->registerAction(mVerseKillServerAct, "Verse Menu", "");
	mActionListWidget->addAction(mVerseKillServerAct);
#endif

	mPerformRemeshingAct = new QAction(tr("Perform Remeshing"), this);
	mPerformRemeshingAct->setStatusTip( tr("Perform the current remeshing scheme") );
	connect(mPerformRemeshingAct, SIGNAL(triggered()), this, SLOT(performRemeshing()));
	sm->registerAction(mPerformRemeshingAct, "Remeshing Menu", "CTRL+R");
	mActionListWidget->addAction(mPerformRemeshingAct);

	//help menu actions
	mHelpAct = new QAction(tr("&Online User Manual"), this);
	mHelpAct->setStatusTip( tr("View the User Manual on the TopMod Wiki") );
	connect(mHelpAct, SIGNAL(triggered()), this, SLOT(help()));
	sm->registerAction(mHelpAct, "Help Menu", "F1");
	mActionListWidget->addAction(mHelpAct);

	mTopModWebAct = new QAction(tr("&TopMod on the Web"), this);
	mTopModWebAct->setStatusTip( tr("Go to the TopMod web page") );
	connect(mTopModWebAct, SIGNAL(triggered()), this, SLOT(topModWeb()));
	sm->registerAction(mTopModWebAct, "Help Menu", "");
	mActionListWidget->addAction(mTopModWebAct);

	mTopModResearchAct = new QAction(tr("&TopMod Research Papers"), this);
	mTopModResearchAct->setStatusTip( tr("Go to the TopMod Research web page") );
	connect(mTopModResearchAct, SIGNAL(triggered()), this, SLOT(topModWeb()));
	sm->registerAction(mTopModResearchAct, "Help Menu", "");
	mActionListWidget->addAction(mTopModResearchAct);

	mCheckForUpdatesAct = new QAction(tr("&Check for Updates"), this);
	mCheckForUpdatesAct->setStatusTip( tr("Check for updates to TopMod Online") );
	connect(mCheckForUpdatesAct, SIGNAL(triggered()), this, SLOT(checkForUpdates()));
	sm->registerAction(mCheckForUpdatesAct, "Help Menu", "");
	mActionListWidget->addAction(mCheckForUpdatesAct);

	mAboutAct = new QAction(tr("&About TopMod"), this);
	mAboutAct->setStatusTip( tr("About TopMod") );
	connect(mAboutAct, SIGNAL(triggered()), this, SLOT(about()));
	sm->registerAction(mAboutAct, "Help Menu", "");
	mActionListWidget->addAction(mAboutAct);

	mAboutQtAct = new QAction(tr("About &Qt"), this);
	mAboutQtAct->setStatusTip( tr("About Qt") );
	connect(mAboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	sm->registerAction(mAboutQtAct, "Help Menu", "");
	mActionListWidget->addAction(mAboutQtAct);
	
	mHideToolBarsAct = new QAction(tr("Hide All ToolBars"), this);
	mHideToolBarsAct->setStatusTip( tr("Hide All ToolBars") );
	connect(mHideToolBarsAct, SIGNAL(triggered()), this, SLOT(hideAllToolBars()));
	sm->registerAction(mHideToolBarsAct, "Window Menu", "T");
	mActionListWidget->addAction(mHideToolBarsAct);

	mShowToolBarsAct = new QAction(tr("Show All ToolBars"), this);
	mShowToolBarsAct->setStatusTip( tr("Show All ToolBars") );
	connect(mShowToolBarsAct, SIGNAL(triggered()), this, SLOT(showAllToolBars()));
	sm->registerAction(mShowToolBarsAct, "Window Menu", "SHIFT+T");
	mActionListWidget->addAction(mShowToolBarsAct);

}

void MainWindow::createMenus(){
#ifdef __APPLE__
	menuBar = new QMenuBar(0);
	setMenuBar(menuBar);
	// setUnifiedTitleAndToolBarOnMac(true);
#else
	menuBar = new QMenuBar(this);
	setMenuBar(menuBar);
#endif

	mFileMenu = new QMenu(tr("&File"));
	menuBar->addMenu(mFileMenu);
	mFileMenu->setTearOffEnabled(true);

	mFileMenu->addAction(mOpenAct);
	mFileMenu->addAction(mSaveAct);
	mFileMenu->addAction(mSaveAsAct);
	mFileMenu->addAction(mSavePatchesAct);
#ifdef WITH_VERSE
	mFileMenu->addSeparator();
	mVerseMenu = new QMenu(tr("&Verse"));
	mFileMenu->addMenu(mVerseMenu);
	mVerseMenu->addAction(mVerseStartServerAct);
	// mVerseMenu->addAction(mVerseKillServerAct);
	mVerseMenu->addSeparator();
	mVerseMenu->addAction(mVerseConnectLocalhostAct);
	mVerseMenu->addAction(mVerseConnectAct);
	// mVerseMenu->removeAction(mVerseConnectAct);
	mVerseMenu->addSeparator();
	// mVerseMenu->addAction(mVerseDisconnectAct);
	// mVerseMenu->addAction(mVerseDisconnectAllAct);
#endif
	mFileMenu->addSeparator();
	mFileMenu->addAction(loadTextureAct);
	mFileMenu->addAction(printInfoAct);
	mFileMenu->addAction(printFaceListAct);
	mFileMenu->addAction(printVertexListAct);
	mFileMenu->addAction(printEdgeListAct);
	mFileMenu->addAction(mPrintCVListAct);
	mFileMenu->addSeparator();
	mFileMenu->addAction(mExitAct);

	mEditMenu = new QMenu(tr("&Edit"));
	menuBar->addMenu(mEditMenu);
	mEditMenu->addAction(mUndoAct);
	mEditMenu->addAction(mRedoAct);
	mEditMenu->setTearOffEnabled(true);
	mEditMenu->addSeparator();
	mEditMenu->addAction(mClearUndoListAct);
	mEditMenu->addSeparator();
	mEditMenu->addAction(mPreferencesAct);

	// mViewMenu = new QMenu(tr("&View"));
	// mViewMenu->setTearOffEnabled(true);
	// menuBar->addMenu(mViewMenu);
	// mViewMenu->addAction(mPerspViewAct);
	// mViewMenu->addAction(mTopViewAct);
	// mViewMenu->addAction(mBottomViewAct);
	// mViewMenu->addAction(mLeftViewAct);
	// mViewMenu->addAction(mRightViewAct);
	// mViewMenu->addAction(mFrontViewAct);
	// mViewMenu->addAction(mBackViewAct);

	mDisplayMenu = new QMenu(tr("&Display"));
	mDisplayMenu->setTearOffEnabled(true);
	menuBar->addMenu(mDisplayMenu);
	//this is now a submenu of Display to make everything on the menu bar more compact
	mRendererMenu = new QMenu(tr("&Renderer"));
	mRendererMenu->setTearOffEnabled(true);
	// menuBar->addMenu(rendererMenu);
	mDisplayMenu->addMenu(mRendererMenu);
	mRendererMenu->addAction(wireframeRendererAct);
	mRendererMenu->addAction(normalRendererAct);
	mRendererMenu->addAction(lightedRendererAct);
	mRendererMenu->addAction(texturedRendererAct);
	// texturedRendererAct->setEnabled(false);
	mRendererMenu->addAction(texturedLightedAct);
	// texturedLightedAct->setEnabled(false);
	mRendererMenu->addSeparator()->setText(tr("Special Mode??"));
	mRendererMenu->addAction(patchRendererAct);
	
	mDisplayMenu->addAction(mPerspViewAct);
	mDisplayMenu->addAction(showVerticesAct);
	//ID display submenu
	mShowIDsMenu = new QMenu(tr("&Show IDs"));
	mDisplayMenu->addMenu(mShowIDsMenu);
	mShowIDsMenu->setTearOffEnabled(true);
	mShowIDsMenu->addAction(mShowFaceIDsAct);
	mShowIDsMenu->addAction(mShowEdgeIDsAct);
	mShowIDsMenu->addAction(mShowVertexIDsAct);
	mShowIDsMenu->addAction(mShowSelectedIDsAct);
	//more view options
	mDisplayMenu->addAction(showSilhouetteAct);
	mDisplayMenu->addAction(showWireframeAct);
	mDisplayMenu->addAction(showCoordinateAxesAct);
	// mDisplayMenu->addAction(showGridAct); //removed for now 
	mDisplayMenu->addAction(showHUDAct);
	#ifdef GPU_OK
	mDisplayMenu->addAction(mUseGPUAct);
	#endif
	mDisplayMenu->addAction(mAntialiasingAct);
	mDisplayMenu->addAction(objectOrientationAct);
	mDisplayMenu->addAction(mShowNormalsAct);
	mDisplayMenu->addAction(mShowFaceCentroidsAct);
	mDisplayMenu->addAction(mFullscreenAct);

	mPrimitivesMenu = new QMenu(tr("&Primitives"));
	mPrimitivesMenu->setTearOffEnabled(true);
	menuBar->addMenu(mPrimitivesMenu);
	mPrimitivesMenu->addAction(pCubeAct);
	mPrimitivesMenu->addAction(pOctahedronAct);
	mPrimitivesMenu->addAction(pTetrahedronAct);
	mPrimitivesMenu->addAction(pDodecahedronAct);
	mPrimitivesMenu->addAction(pIcosahedronAct);
	mPrimitivesMenu->addAction(pSoccerBallAct);
	mPrimitivesMenu->addAction(pGeodesicAct);

	mSelectionMenu = new QMenu(tr("&Selection"));
	menuBar->addMenu(mSelectionMenu);
	mSelectionMenu->addAction(selectAllAct);
	mSelectionMenu->addAction(selectInverseAct);
	mSelectionMenu->addAction(exitSelectionModeAct);
	mSelectionMenu->addAction(clearSelectedModeAct);
	mSelectionMenu->addSeparator();
	mSelectionMenu->addAction(selectEdgeAct);
	mSelectionMenu->addAction(selectEdgeLoopAct);
	mSelectionMenu->addSeparator();
	mSelectionMenu->addAction(selectVertexAct);
	mSelectionMenu->addAction(mEditVertexAct);
	mSelectionMenu->addAction(selectCornerAct);
	mSelectionMenu->addSeparator();
	mSelectionMenu->addAction(selectFaceAct);
	mSelectionMenu->addAction(selectFaceLoopAct);
	mSelectionMenu->addAction(selectMultipleFacesAct);
	mSelectionMenu->addAction(selectSimilarFacesAct);
	mSelectionMenu->addAction(selectCheckerboardFacesAct);
	// mSelectionMenu->addSeparator();
	
	mToolsMenu = new QMenu(tr("&Tools"));
	mToolsMenu->setTearOffEnabled(true);
	mToolsMenu->addMenu(mBasicsMode->getMenu());
	mToolsMenu->addMenu(mExtrusionMode->getMenu());
	// mToolsMenu->addMenu(mConicalMode->getMenu());
	mToolsMenu->addMenu(mHighgenusMode->getMenu());
	mToolsMenu->addMenu(mTexturingMode->getMenu());
	mToolsMenu->addAction(mQuickCommandAct);
	menuBar->addMenu(mToolsMenu);

	mRemeshingMenu = mRemeshingMode->getMenu();
	mRemeshingMenu->addAction(mPerformRemeshingAct);
	mRemeshingMenu->setTearOffEnabled(true);
	menuBar->addMenu(mRemeshingMenu);		

	mObjectMenu = new QMenu(tr("&Object"));
	mObjectMenu->setTearOffEnabled(true);
	mToolsMenu->addMenu(mObjectMenu);
	mObjectMenu->addAction(subdivideAllEdgesAct);
	mObjectMenu->addAction(planarizeAllFacesAct);
	mObjectMenu->addAction(makeObjectSphericalAct);
	mObjectMenu->addAction(makeObjectSmoothAct);
	mObjectMenu->addAction(createCrustAct);
	mObjectMenu->addAction(computeLightingAct);
	mObjectMenu->addAction(computeNormalsAndLightingAct);
	mObjectMenu->addAction(assignTextureCoordinatesAct);

	//selection Masks (vertices / edges / faces / face-vertices)
	mSelectionMaskMenu = new QMenu(tr("Selection &Masks"));
	mSelectionMenu->addMenu(mSelectionMaskMenu);
	mSelectionMaskMenu->addAction(mSelectVerticesMaskAct);
	mSelectionMaskMenu->addAction(mSelectFacesMaskAct);
	mSelectionMaskMenu->addAction(mSelectEdgesMaskAct);
	mSelectionMaskMenu->addAction(mSelectFaceVerticesMaskAct);

	mWindowMenu = new QMenu(tr("&Window"));
	menuBar->addMenu(mWindowMenu);
	mWindowMenu->addAction(mShowStartupDialogAct);
	mWindowMenu->addAction(mShowToolOptionsAct);
	#ifdef WITH_PYTHON
	mWindowMenu->addAction(mShowScriptEditorAct);
	#endif
	mWindowMenu->addAction(mShowAnimatedHelpAct);
	#ifdef WITH_VERSE
	mWindowMenu->addAction(mShowVerseDialogAct);
	#endif
	// mWindowMenu->addAction(mShowToolBarsAct);
	// mWindowMenu->addAction(mHideToolBarsAct);
	mWindowMenu->addSeparator();
	mWindowMenu->addAction(mEditToolBarAct);
	// mWindowMenu->addAction(mSelectionMaskToolBarAct);
	mWindowMenu->addAction(mPrimitivesToolBarAct);
	mWindowMenu->addAction(mToolsToolBarAct);
	mWindowMenu->addAction(mExtrusionToolBarAct);
	// mWindowMenu->addAction(mConicalToolBarAct);
	mWindowMenu->addAction(mHighgenusToolBarAct);
	mWindowMenu->addAction(mTexturingToolBarAct);
	mWindowMenu->addAction(mRemeshingToolBarAct);

	mHelpMenu = new QMenu(tr("&Help"));
	menuBar->addMenu(mHelpMenu);
	mHelpMenu->addAction(mHelpAct);
	mHelpMenu->addAction(mCheckForUpdatesAct);
	mHelpMenu->addAction(mTopModWebAct);
	mHelpMenu->addAction(mTopModResearchAct);
	mHelpMenu->addSeparator();
	mHelpMenu->addAction(mAboutAct);
	mHelpMenu->addAction(mAboutQtAct);
	
	// mLanguageMenu = new QMenu(tr("&Language"));
	// mLanguageMenu->setTearOffEnabled(true);
	// settingsMenu->addMenu(mLanguageMenu);
	// mLanguageMenu->addAction(englishAct);
	// mLanguageMenu->addAction(spanishAct);
	// mLanguageMenu->addAction(germanAct);
	// mLanguageMenu->addAction(frenchAct);
	// mLanguageMenu->addAction(turkishAct);
	// mLanguageMenu->addAction(catalanAct);
	
	mRightClickMenu = new QMenu();

}

void MainWindow::createToolBars() {

	mEditToolBar = new QToolBar(tr("Edit"),this);
	mEditToolBar->setFloatable(true);
	addToolBar(Qt::TopToolBarArea,mEditToolBar);
	mEditToolBar->setFloatable(true);
	mEditToolBar->addAction(mOpenAct);
	mEditToolBar->addAction(mSaveAsAct);
	mEditToolBar->addAction(mUndoAct);
	mEditToolBar->addAction(mRedoAct);
	mEditToolBar->setIconSize(QSize(32,32));

	//selection masks toolbar
	mSelectionMaskToolBar = new QToolBar(tr("Selection Masks"));
	mSelectionMaskToolBar->hide();
	mSelectionMaskToolBar->setFloatable(true);
	//addToolBar(Qt::TopToolBarArea,mSelectionMaskToolBar);
	mSelectionMaskToolBar->addAction(mSelectVerticesMaskAct);
	mSelectionMaskToolBar->addAction(mSelectFacesMaskAct);
	mSelectionMaskToolBar->addAction(mSelectEdgesMaskAct);
	mSelectionMaskToolBar->addAction(mSelectFaceVerticesMaskAct);
	mSelectionMaskToolBar->setOrientation(Qt::Vertical);

	mPrimitivesToolBar = new QToolBar(tr("Primitives"),this);
	mPrimitivesToolBar->setFloatable(true);
	addToolBar(Qt::TopToolBarArea,mPrimitivesToolBar);
	mPrimitivesToolBar->setIconSize(QSize(32,32));
	mPrimitivesToolBar->addAction(pCubeAct);
	mPrimitivesToolBar->addAction(pOctahedronAct);
	mPrimitivesToolBar->addAction(pTetrahedronAct);
	mPrimitivesToolBar->addAction(pDodecahedronAct);
	mPrimitivesToolBar->addAction(pIcosahedronAct);
	mPrimitivesToolBar->addAction(pSoccerBallAct);
	mPrimitivesToolBar->addAction(pGeodesicAct);

	//basic tools - six buttons
	mToolsToolBar = new QToolBar(tr("Tools"),this);
	mToolsToolBar->setFloatable(true);
	addToolBar(Qt::TopToolBarArea,mToolsToolBar);
	mToolsToolBar->setIconSize(QSize(32,32));

	addToolBarBreak();

	mExtrusionToolBar = new QToolBar(tr("Extrusion Tools"),this);
	mExtrusionToolBar->setFloatable(true);
	addToolBar(Qt::TopToolBarArea,mExtrusionToolBar);
	mExtrusionToolBar->setIconSize(QSize(32,32));

	// mConicalToolBar = new QToolBar(tr("Conical Tools"),this);
	// addToolBar(Qt::TopToolBarArea,mConicalToolBar);
	// mConicalToolBar->setIconSize(QSize(32,32));

	mHighgenusToolBar = new QToolBar(tr("High Genus Tools"),this);
	mHighgenusToolBar->setFloatable(true);
	addToolBar(Qt::TopToolBarArea,mHighgenusToolBar);
	mHighgenusToolBar->setIconSize(QSize(32,32));

	mTexturingToolBar = new QToolBar(tr("Texturing Tools"),this);
	mTexturingToolBar->setFloatable(true);
	addToolBar(Qt::TopToolBarArea,mTexturingToolBar);
	mTexturingToolBar->setIconSize(QSize(32,32));

	mRemeshingToolBar = new QToolBar(tr("Remeshing Tools"),this);
	mRemeshingToolBar->setAllowedAreas(Qt::RightToolBarArea);
	mRemeshingToolBar->setOrientation(Qt::Vertical);
	mRemeshingToolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
	mRemeshingToolBar->setFloatable(false);
	// mRemeshingToolBar->setFloating(true);
	addToolBar(Qt::RightToolBarArea,mRemeshingToolBar);
	// mRemeshingToolBar->setIconSize(QSize(16,16));
	mRemeshingToolBar->hide();

	//tools ction group initialization
	mToolsActionGroup = new QActionGroup(this);
	mToolsActionGroup->setExclusive(true);

	mBasicsMode->addActions(mToolsActionGroup, mToolsToolBar, mToolOptionsStackedWidget);	
	mExtrusionMode->addActions(mToolsActionGroup, mExtrusionToolBar, mToolOptionsStackedWidget);
	// mConicalMode->addActions(mToolsActionGroup, mConicalToolBar, mToolOptionsStackedWidget);

	mRemeshingActionGroup = new QActionGroup(this);
	mRemeshingMode->addActions(mToolsActionGroup, mRemeshingToolBar, mToolOptionsStackedWidget);

	mHighgenusMode->addActions(mToolsActionGroup, mHighgenusToolBar, mToolOptionsStackedWidget);
	mTexturingMode->addActions(mToolsActionGroup, mTexturingToolBar, mToolOptionsStackedWidget);

	//window menu toolbar display actions
	mEditToolBarAct 					= mEditToolBar->toggleViewAction();         
	// mSelectionMaskToolBarAct 	= mSelectionMaskToolBar->toggleViewAction();
	mPrimitivesToolBarAct     = mPrimitivesToolBar->toggleViewAction();
	mToolsToolBarAct				 	= mToolsToolBar->toggleViewAction();
	mExtrusionToolBarAct 			= mExtrusionToolBar->toggleViewAction();
	// mConicalToolBarAct				= mConicalToolBar->toggleViewAction();
	mHighgenusToolBarAct    	= mHighgenusToolBar->toggleViewAction();
	mTexturingToolBarAct  	  = mTexturingToolBar->toggleViewAction();
	mRemeshingToolBarAct	    = mRemeshingToolBar->toggleViewAction();

}

void MainWindow::createStartupDialog(){
	
	//startup screen - links to video tutorials and free quicktime download

	// mStartupDialogDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable /* | QDockWidget::DockWidgetClosable*/);
	mStartupDialogDockWidget->setAllowedAreas(Qt::NoDockWidgetArea);
	// mStartupDialogDockWidget->hide();
	
	mStartupDialogWidget = new QWidget;
	mStartupDialogLayout = new QGridLayout;

	//actions for the startup screen
	mTutorialNavigationAct = new QAction(QIcon(":/images/tutorial_01.png"),tr("Navigation Basics"), this);
	connect(mTutorialNavigationAct, SIGNAL(triggered()), this, SLOT(loadNavigationTutorial()));
	mTutorialNavigationButton = new QToolButton;
	mTutorialNavigationButton->setFixedWidth(200);
	mTutorialNavigationButton->setIconSize(QSize(48,48));
	mTutorialNavigationButton->setDefaultAction(mTutorialNavigationAct);
	mTutorialNavigationButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	mStartupDialogLayout->addWidget(mTutorialNavigationButton,1,0);
	
	mTutorialInterfaceAct = new QAction(QIcon(":/images/tutorial_02.png"),tr("Interface Basics"), this);
	connect(mTutorialInterfaceAct, SIGNAL(triggered()), this, SLOT(loadInterfaceTutorial()));
	mTutorialInterfaceButton = new QToolButton;
	mTutorialInterfaceButton->setFixedWidth(200);
	mTutorialInterfaceButton->setIconSize(QSize(48,48));
	mTutorialInterfaceButton->setDefaultAction(mTutorialInterfaceAct);
	mTutorialInterfaceButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	mStartupDialogLayout->addWidget(mTutorialInterfaceButton,2,0);

	mTutorialBasicAct = new QAction(QIcon(":/images/tutorial_03.png"),tr("Basic Operations"), this);
	connect(mTutorialBasicAct, SIGNAL(triggered()), this, SLOT(loadBasicTutorial()));
	mTutorialBasicButton = new QToolButton;
	mTutorialBasicButton->setFixedWidth(200);
	mTutorialBasicButton->setIconSize(QSize(48,48));
	mTutorialBasicButton->setDefaultAction(mTutorialBasicAct);
	mTutorialBasicButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	mStartupDialogLayout->addWidget(mTutorialBasicButton,3,0);

	mTutorialExtrusionAct = new QAction(QIcon(":/images/tutorial_04.png"),tr("Extrusion Operations"), this);
	connect(mTutorialExtrusionAct, SIGNAL(triggered()), this, SLOT(loadExtrusionTutorial()));
	mTutorialExtrusionButton = new QToolButton;
	mTutorialExtrusionButton->setFixedWidth(200);
	mTutorialExtrusionButton->setIconSize(QSize(48,48));
	mTutorialExtrusionButton->setDefaultAction(mTutorialExtrusionAct);
	mTutorialExtrusionButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	mStartupDialogLayout->addWidget(mTutorialExtrusionButton,4,0);

	mTutorialRemeshingAct = new QAction(QIcon(":/images/tutorial_05.png"),tr("Remeshing Operations"), this);
	connect(mTutorialRemeshingAct, SIGNAL(triggered()), this, SLOT(loadRemeshingTutorial()));
	mTutorialRemeshingButton = new QToolButton;
	mTutorialRemeshingButton->setFixedWidth(200);
	mTutorialRemeshingButton->setIconSize(QSize(48,48));
	mTutorialRemeshingButton->setDefaultAction(mTutorialRemeshingAct);
	mTutorialRemeshingButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	mStartupDialogLayout->addWidget(mTutorialRemeshingButton,1,1);

	mTutorialHighgenusAct = new QAction(QIcon(":/images/tutorial_06.png"),tr("High genus Operations"), this);
	connect(mTutorialHighgenusAct, SIGNAL(triggered()), this, SLOT(loadHighgenusTutorial()));
	mTutorialHighgenusButton = new QToolButton;
	mTutorialHighgenusButton->setFixedWidth(200);
	mTutorialHighgenusButton->setIconSize(QSize(48,48));
	mTutorialHighgenusButton->setDefaultAction(mTutorialHighgenusAct);
	mTutorialHighgenusButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	mStartupDialogLayout->addWidget(mTutorialHighgenusButton,2,1);

	mTutorialTexturingAct = new QAction(QIcon(":/images/tutorial_07.png"),tr("Texturing Operations"), this);
	connect(mTutorialTexturingAct, SIGNAL(triggered()), this, SLOT(loadTexturingTutorial()));
	mTutorialTexturingButton = new QToolButton;
	mTutorialTexturingButton->setFixedWidth(200);
	mTutorialTexturingButton->setIconSize(QSize(48,48));
	mTutorialTexturingButton->setDefaultAction(mTutorialTexturingAct);
	mTutorialTexturingButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	mStartupDialogLayout->addWidget(mTutorialTexturingButton,3,1);
	
	//show on startup? checkbox
	mShowStartupDialogAtStartupCheckBox = new QCheckBox(tr("Show this dialog at startup"));
	mShowStartupDialogAtStartupCheckBox->setChecked(mShowStartupDialogAtStartup);
	connect(mShowStartupDialogAtStartupCheckBox,SIGNAL(stateChanged(int)),this,SLOT(setShowStartupDialogAtStartup(int)));
	mStartupDialogLayout->addWidget(mShowStartupDialogAtStartupCheckBox,5,0);
	
	//download quicktime link label
	QLabel *quicktimeLabel = new QLabel(tr("Quicktime 7.2 or greater recommended."));
	QLabel *downloadQuicktimeLabel = new QLabel(tr("<h5>Quicktime 7.2 or greater recommended.<br /><a href=\"http://apple.com/quicktime/\">Download now.</a></h5>"));
	downloadQuicktimeLabel->setOpenExternalLinks(true);
	mStartupDialogLayout->addWidget(downloadQuicktimeLabel,5,1, Qt::AlignRight);
		
	mStartupDialogLayout->setColumnStretch(2,1);
	mStartupDialogLayout->setRowStretch(6,1);
	mStartupDialogWidget->setLayout(mStartupDialogLayout);
	mStartupDialogDockWidget->setWidget(mStartupDialogWidget);
	mStartupDialogDockWidget->setFloating(true);
	mStartupDialogDockWidget->move(this->width()/2 + this->x() - mStartupDialogDockWidget->width()/2, this->y() + height()/2 - mStartupDialogDockWidget->height()/2);
	if (!mShowStartupDialogAtStartup)
		mStartupDialogDockWidget->hide();
}

void MainWindow::setShowStartupDialogAtStartup(int b){
	mShowStartupDialogAtStartup = (bool)b;
}

bool MainWindow::getShowStartupDialogAtStartup(){
	return mShowStartupDialogAtStartup;
}

void MainWindow::loadNavigationTutorial(){
	QDesktopServices::openUrl(QUrl::fromLocalFile("video/tutorial_navigation.mov"));
}

void MainWindow::loadInterfaceTutorial(){
	QDesktopServices::openUrl(QUrl::fromLocalFile("video/tutorial_interface.mov"));
}

void MainWindow::loadBasicTutorial(){
	QDesktopServices::openUrl(QUrl::fromLocalFile("video/tutorial_basic.mov"));
}

void MainWindow::loadExtrusionTutorial(){
	QDesktopServices::openUrl(QUrl::fromLocalFile("video/tutorial_extrusion.mov"));
}

void MainWindow::loadRemeshingTutorial(){
	QDesktopServices::openUrl(QUrl::fromLocalFile("video/tutorial_remeshing.mov"));
}

void MainWindow::loadHighgenusTutorial(){
	QDesktopServices::openUrl(QUrl::fromLocalFile("video/tutorial_highgenus.mov"));
}

void MainWindow::loadTexturingTutorial(){
	QDesktopServices::openUrl(QUrl::fromLocalFile("video/tutorial_texturing.mov"));
}

void MainWindow::showAllToolBars(){
	mEditToolBar->show();
	mSelectionMaskToolBar->show();
	mPrimitivesToolBar->show();
	mToolsToolBar->show();
	mExtrusionToolBar->show();
	mConicalToolBar->show();
	mHighgenusToolBar->show();
	mTexturingToolBar->show();
	mRemeshingToolBar->show();
	
}

void MainWindow::hideAllToolBars(){
	mEditToolBar->hide();
	mSelectionMaskToolBar->hide();
	mPrimitivesToolBar->hide();
	mToolsToolBar->hide();
	mExtrusionToolBar->hide();
	mConicalToolBar->hide();
	mHighgenusToolBar->hide();
	mTexturingToolBar->hide();
	mRemeshingToolBar->hide();
	
}

void MainWindow::setToolOptions(QWidget *optionsWidget) {
	mToolOptionsDockWidget->setWindowTitle(tr("Tool Options - ") + optionsWidget->windowTitle());
	mToolOptionsStackedWidget->setCurrentWidget(optionsWidget);
	// show or hide the dockwidget options
	// if (optionsWidget->windowTitle() != "" && mToolOptionsDockWidget->isHidden())
		// mToolOptionsDockWidget->show();
	// else if (!mToolOptionsDockWidget->isHidden() && optionsWidget->windowTitle() == "")
		// mToolOptionsDockWidget->hide();
}

void MainWindow::createStatusBar() {
	statusBar()->showMessage(tr("Welcome to TopMod"));
}

void MainWindow::closeEvent(QCloseEvent *event) {
//close the help file if it's open... not sure this is necessary
	// if (mAssistantClient)
	// 	mAssistantClient->closeAssistant();

	mPreferencesDialog->saveSettings();
	
	if (maybeSave()) {	
		event->accept();
	} 
	else event->ignore();
}

bool MainWindow::maybeSave() {
	if (this->isModified()) {
		int ret = QMessageBox::warning(this, tr("TopMod"),
																	 tr("The document has been modified.\n"
																			"Do you want to save your changes?"),
																	 QMessageBox::Yes | QMessageBox::Default,
																	 QMessageBox::No,
																	 QMessageBox::Cancel | QMessageBox::Escape);
		if (ret == QMessageBox::Yes)
			return this->saveFile();
		else if (ret == QMessageBox::Cancel)
			return false;
	}
	return true;
}

void MainWindow::loadFile(QString fileName) {
	openFile(fileName);
	this->setCurrentFile(fileName);
	statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(QString fileName) {
	this->setCurrentFile(fileName);
	statusBar()->showMessage(tr("File saved"), 2000);
	return true;
}

void MainWindow::openFile(QString fileName){
	QFile file(fileName);
	file.open(QIODevice::ReadOnly);
	QFileInfo info(file);
	QByteArray ba = info.absoluteFilePath().toLatin1();
	const char *filename = ba.data();
	mWasPrimitive = false;
	mIsPrimitive = false;
	if (!curFile.isEmpty()){
		undoPush();
		setModified(false);
	}

	readObject(filename);
#ifdef WITH_PYTHON
	DLFLObjectPtr obj = &object;
	if( obj )
		emit loadedObject(obj,fileName);
#endif
	active->createPatchObject( );
	active->recomputePatches();
	active->recomputeNormals();
	setCurrentFile(fileName);
	active->redraw();
}

void MainWindow::about() {
	QMessageBox::about(this, tr("About TopMod"),
										 tr("The About page for TopMod is still in the works \n stay tuned... \n"));
}

void MainWindow::initializeHelp(){
	// mAssistantClient = new QAssistantClient(QLibraryInfo::location(QLibraryInfo::BinariesPath), this);
	//   QStringList arguments;
	//   arguments << "-profile" << QString("userdoc") + QDir::separator() + QString("topmod.adp");
	//   mAssistantClient->setArguments(arguments);
}

/** 
* \brief show the help file in the QAssistant help file viewer 
*/
void MainWindow::help() {
	// mAssistantClient->openAssistant();//showPage(QString("userdoc/index.html"));
	QDesktopServices::openUrl(QUrl("http://www.ends170.com/topmod/wiki"));
}

/**
* \brief will eventually check for updates on the TopMod website and download the necessary files
*/
void MainWindow::checkForUpdates(){
	QDesktopServices::openUrl(QUrl("http://www.ends170.com/topmod"));
}

/**
* \brief opens the current TopMod homepage in the user's default web browser
*/
void MainWindow::topModWeb(){
	QDesktopServices::openUrl(QUrl("http://www.ends170.com/topmod"));
}


void MainWindow::topModResearch(){
	QDesktopServices::openUrl(QUrl("http://www-viz.tamu.edu/faculty/ergun/research/topology/index.html"));
}


void MainWindow::documentWasModified() {
	setWindowModified(this->isModified());
}

void MainWindow::on_editStyleAction_triggered() {
	mStyleSheetEditor->show();
	mStyleSheetEditor->activateWindow();
}

void MainWindow::openPreferences() {
	mPreferencesDialog->display();
}

#ifdef WITH_VERSE
void MainWindow::verseConnected(){
	mVerseMenu->insertAction(mVerseConnectLocalhostAct, mVerseDisconnectAct);
	mVerseMenu->removeAction(mVerseConnectLocalhostAct);
	mVerseMenu->removeAction(mVerseConnectAct);
}

void MainWindow::verseDisconnected(){
	mVerseMenu->insertAction(mVerseDisconnectAct,mVerseConnectLocalhostAct);
	mVerseMenu->insertAction(mVerseDisconnectAct,mVerseConnectAct);
	mVerseMenu->removeAction(mVerseDisconnectAct);
}
void MainWindow::verseStarted(){
	mVerseMenu->insertAction(mVerseStartServerAct, mVerseKillServerAct);
	mVerseMenu->removeAction(mVerseStartServerAct);	
}
void MainWindow::verseKilled(){
	mVerseMenu->insertAction(mVerseKillServerAct, mVerseStartServerAct);
	mVerseMenu->removeAction(mVerseKillServerAct);		
}
#endif

void MainWindow::createRenderers(){
	wired = new WireframeRenderer();
	// wired->setRenderFlags(DLFLRenderer::ShowWireframe);

	normal = new NormalRenderer();
	// normal->setRenderFlags(DLFLRenderer::ShowWireframe);

	lit = new LitRenderer();
	// lit->setRenderFlags(DLFLRenderer::ShowWireframe);

	textured = new TexturedRenderer();
	// textured->setRenderFlags(DLFLRenderer::ShowWireframe);

	texturedlit = new TexturedLitRenderer();
	// texturedlit->setRenderFlags(DLFLRenderer::ShowWireframe);

	patch = new PatchRenderer();
	// patch->setRenderFlags(DLFLRenderer::ShowWireframe);
}
void MainWindow::destroyRenderers(){
	delete wired;
	delete normal;
	delete lit;
	delete textured;
	delete texturedlit;
	delete patch;
};

void MainWindow::setUndoLimit(int limit) {
	undolimit = limit;
}

void MainWindow::toggleUndo(void) {
	if ( useUndo ) useUndo = false;
	else useUndo = true;
}

void MainWindow::doDrag(int x, int y) { // brianb
	int drag_endx = x;
	int drag_endy = y;

	GLdouble obj_world[3],  // Object world coordinates
		obj_window[3], // Object window coordinates 
		ms_window[3],  // Mouse start drag window
		ms_world[3],   // Mouse start drag world
		me_window[3],  // Mouse end drag window
		me_world[3];   // Mouse end drag world
	GLdouble modelMatrix[16], projMatrix[16];
	GLint viewport[4];
	GLint realy;
	DLFLVertexPtr vptr;
	// Viewport* viewp;

	switch ( mode ) {
	case EditVertex:
		if (GLWidget::numSelectedLocators() > 0)
			{
				if (!is_editing) {
					undoPush();
					is_editing = true;
				}
				vptr = active->getLocatorPtr()->getActiveVertex();

				// Save previous transformations
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();

				// Apply current transformation
				// viewp = active->getViewport();
				// viewp->reshape();
				// viewp->apply_transform();

				// Get the info
				glGetIntegerv(GL_VIEWPORT, viewport);
				glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
				glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

				obj_world[0] = vptr->getCoords()[0]; 
				obj_world[1] = vptr->getCoords()[1]; 
				obj_world[2] = vptr->getCoords()[2];

				// Project object coordinates to window coordinates (to get accurate window depth)
				gluProject(obj_world[0],obj_world[1],obj_world[2],
									 modelMatrix,projMatrix,viewport,
									 &obj_window[0],&obj_window[1],&obj_window[2]);

				// Set start and end window coordinates using depth coordinate found above
				ms_window[0] = drag_startx;  ms_window[1] = drag_starty;  ms_window[2] = obj_window[2];
				me_window[0] = drag_endx;    me_window[1] = drag_endy;    me_window[2] = obj_window[2];

				// Unproject start drag window coordinates to world coordinates
				gluUnProject(ms_window[0],ms_window[1],ms_window[2],
										 modelMatrix, projMatrix, viewport,
										 &ms_world[0],&ms_world[1],&ms_world[2]);

				// Unproject end drag window coordinates to world coordinates
				gluUnProject(me_window[0],me_window[1],me_window[2],
										 modelMatrix, projMatrix, viewport,
										 &me_world[0],&me_world[1],&me_world[2]);

				// Switch on locked axis and update object world position
				switch (active->getLocatorPtr()->getSelectedAxis())
					{
					case 0: // X-axis
						obj_world[0] = obj_world[0] + me_world[0] - ms_world[0];     
						break;

					case 1: // Y-axis
						obj_world[1] = obj_world[1] + me_world[1] - ms_world[1];                     
						break;

					case 2: // Z-axis
						obj_world[2] = obj_world[2] + me_world[2] - ms_world[2];                 
						break;

					case 3:  // User can drag freely along viewing place
					default: 
						obj_world[0] = obj_world[0] + me_world[0] - ms_world[0];             
						obj_world[1] = obj_world[1] + me_world[1] - ms_world[1];
						obj_world[2] = obj_world[2] + me_world[2] - ms_world[2];
						break;
					}

				vptr->setCoords(Vector3d(obj_world[0],obj_world[1],obj_world[2]));

				// Reset drag start points
				startDrag(drag_endx,drag_endy);

				// Restore previous transformations
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();

				redraw();
			}
		break;

	default:
		doSelection(x,y);
		break;
	}
}// brianb

// Do selection of various entities depending on current mode
void MainWindow::doSelection(int x, int y) {
	DLFLVertexPtr svptr = NULL;
	DLFLEdgePtr septr = NULL;
	DLFLFacePtr sfptr = NULL;
	DLFLFaceVertexPtr sfvptr = NULL;
	DLFLLocatorPtr slptr = NULL; // brianb

	DLFLEdgePtrArray septrarr;
	DLFLEdgePtrArray::iterator eit;
	DLFLFacePtrArray sfptrarr;
	DLFLFacePtrArray::iterator first, last;

	switch ( mode ) {

	case EditVertex:     // brianb
		slptr = active->getLocatorPtr();
		svptr = active->getLocatorPtr()->getActiveVertex();
		if (svptr == NULL) {
			svptr = active->selectVertex(x,y);
			slptr->setActiveVertex(svptr);
		}
		// Test for locator selection
		if (slptr->getActiveVertex() != NULL)	{
			slptr = active->selectLocator(x,y);
			if (slptr != NULL) {
				active->setSelectedLocator(0,slptr);
				startDrag(x,y);
			}
			else {
				active->getLocatorPtr()->setActiveVertex(NULL);
				active->clearSelectedLocators();
			} 
		}
		break;
	case SelectVertex:
	case MarkVertex:
	case CutVertex://ozgur
		std::cout<< "selecting a vertex\n";
		svptr = active->selectVertex(x,y);
		active->setSelectedVertex(num_sel_verts,svptr);
		break;
	case MultiSelectVertex :
		svptr = active->selectVertex(x,y);
		if ( !active->isSelected(svptr) )
			active->setSelectedVertex(num_sel_verts,svptr);
		break;
	case DeleteEdge :
	case SubdivideEdge :
	case CollapseEdge :
	case SelectEdge :
	case CutEdge :
	case TruncateEdge :
	case MarkEdge ://ozgur
		septr = active->selectEdge(x,y);
		active->setSelectedEdge(num_sel_edges,septr);
		break;
	case CutEdgeandVertex://ozgur
		septr = active->selectEdge(x,y);
		svptr = active->selectVertex(x,y);
		active->setSelectedEdge(num_sel_edges,septr);
		active->setSelectedVertex(num_sel_verts,svptr);
		break;
	case MultiSelectEdge :
		septr = active->selectEdge(x,y);
		if ( !active->isSelected(septr) )
			active->setSelectedEdge(num_sel_edges,septr);
		break;
	case SelectEdgeLoop:
		if (QApplication::keyboardModifiers() != Qt::ShiftModifier){
			active->clearSelectedEdges();
		}
		septr = active->selectEdge(x,y);
		if (septr && QApplication::keyboardModifiers() == Qt::ControlModifier && active->isSelected(septr)){
			deselect_edges = true;
			active->clearSelectedEdge(septr);
			num_sel_edges--;
			getEdgeLoopSelection(septr);
			deselect_edges = false;
		}
		else if ( septr && !active->isSelected(septr)){
			active->setSelectedEdge(num_sel_edges,septr);
			num_sel_edges++;
			getEdgeLoopSelection(septr);
		}
		active->redraw();
		break;
	case SelectFace :
	case ExtrudeFace :
	case ExtrudeFaceDS :
	case ExtrudeDualFace :
	case ExtrudeFaceDodeca :
	case ExtrudeFaceIcosa :
	case StellateFace :
	case DoubleStellateFace :
	case CrustModeling :
	case ConnectFaces :
	case CutFace://ozgur
		sfptr = active->selectFace(x,y);
		active->setSelectedFace(num_sel_faces,sfptr);
		break;
	case SelectSimilarFaces :
		//clear selection if shift isn't down
		if (QApplication::keyboardModifiers() != Qt::ShiftModifier)
			active->clearSelectedFaces();
		sfptr = active->selectFace(x,y);
		if (sfptr){
			active->setSelectedFace(num_sel_faces,sfptr);
			num_sel_faces++;
			DLFLFacePtrArray sfptrarray;
			vector<DLFLFacePtr>::iterator it;
			DLFL::selectMatchingFaces(&object, sfptr, sfptrarray);
			for (it = sfptrarray.begin(); it != sfptrarray.end(); it++){
				if (!active->isSelected(*it)){
					active->setSelectedFace(num_sel_faces,*it);
					num_sel_faces++;
				}
			}
		}
		active->redraw();
		break;
	case SelectFaceLoop:
		if (QApplication::keyboardModifiers() != Qt::ShiftModifier){
			active->clearSelectedFaces();
		}
		septr = active->selectEdge(x,y);
		active->setSelectedEdge(num_sel_edges,septr);
		if ( septr ){
			if (QApplication::keyboardModifiers() == Qt::ControlModifier){ // deselect
				face_loop_start_edge = septr;
				getFaceLoopSelection(septr, true, NULL, false);
			}
			else { //select
				face_loop_start_edge = septr;
				getFaceLoopSelection(septr, true, NULL, true);
			}
		}
		active->redraw();
		break;
	case ExtrudeMultipleFaces :
	case MultiSelectFace :
	case SubdivideFace :
		// No duplicates allowed
		// sfptr = active->selectFace(x,y);
		// if ( !active->isSelected(sfptr) ){
		// 	active->setSelectedFace(num_sel_faces,sfptr);
		// 	num_sel_faces++;
		// }
		// else if ( active->isSelected(sfptr) && QApplication::keyboardModifiers() == Qt::ControlModifier) {
		// 	active->clearSelectedFace(sfptr);
		// 	num_sel_faces--;
		// }
		// if (QApplication::keyboardModifiers() != Qt::ShiftModifier){
		// 	active->clearSelectedFaces();
		// }
		if ( QApplication::keyboardModifiers() == Qt::ControlModifier) {
			sfptrarr = active->deselectFaces(x,y);
			first = sfptrarr.begin(); last = sfptrarr.end();
			while ( first != last ){
				active->clearSelectedFace(*first);
				++first;
				num_sel_faces--;
			}
			active->redraw();
			sfptrarr.clear();
		}
		else {
			sfptrarr = active->selectFaces(x,y);
			first = sfptrarr.begin(); last = sfptrarr.end();
			while ( first != last ){
				active->setSelectedFace(num_sel_faces,*first);
				++first;
				num_sel_faces++;
			}
			active->redraw();
			sfptrarr.clear();
		}
		break;
	case SelectCheckerboard :
		if (QApplication::keyboardModifiers() != Qt::ShiftModifier){
			active->clearSelectedFaces();
		}
		//get one selected face
		sfptr = active->selectFace(x,y);
		if (sfptr && active->isSelected(sfptr) && QApplication::keyboardModifiers() == Qt::ControlModifier){
			deselect_edges = true;
			active->clearSelectedFace(sfptr);
			num_sel_faces--;
			getCheckerboardSelection(sfptr);
			deselect_edges = false;
		}
		else if (sfptr && !active->isSelected(sfptr) ){
			active->setSelectedFace(num_sel_faces,sfptr);
			num_sel_faces++;
			getCheckerboardSelection(sfptr);
		}		
		active->redraw();
		sfptrarr.clear();
		break;
	case SelectFaceVertex :
	case ReorderFace :
	case InsertEdge :
	case SpliceCorners :
	case ConnectFaceVertices :
	case BezierConnectFaces :
	case HermiteConnectFaces :
		sfptr = active->selectFace(x,y);
		active->setSelectedFace(num_sel_faces,sfptr);
		if ( sfptr )
			{
				sfvptr = active->selectFaceVertex(sfptr,x,y);
				active->setSelectedFaceVertex(num_sel_faceverts,sfvptr);
			}
		break;

	case MultiSelectFaceVertex :
		sfptr = active->selectFace(x,y);
		active->setSelectedFace(num_sel_faces,sfptr);
		if ( sfptr )
			{
				sfvptr = active->selectFaceVertex(sfptr,x,y);
				if ( !active->isSelected(sfvptr) )
					active->setSelectedFaceVertex(num_sel_faceverts,sfvptr);
			}
		break;

	case ConnectEdges :
		sfptr = active->selectFace(x,y);
		active->setSelectedFace(num_sel_faces,sfptr);
		if ( sfptr )
			{
				septr = active->selectEdge(x,y);
				active->setSelectedEdge(num_sel_edges,septr);
			}
		break;
	};	
	if ( svptr != NULL || septr != NULL || sfptr != NULL ) redraw();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
	// QMessageBox::about(this,tr("mimetype"),tr("%1").arg(event->mimeData()->formats().join(",")));
	// if (event->mimeData()->hasFormat("text/uri-list"))
	// 	event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event) {
	//textBrowser->setPlainText(event->mimeData()->text());
	//mimeTypeCombo->clear();
	//mimeTypeCombo->addItems(event->mimeData()->formats());
	// QMessageBox::about(this,tr("mimetype"),tr("%1").arg(event->mimeData()->data("text/uri-list").data));

	// event->acceptProposedAction();
}

// Handle keyboard and mouse events
void MainWindow::mousePressEvent(QMouseEvent *event) {

	if ( event->buttons() == Qt::LeftButton && mode != NormalMode ){
		doSelection(event->x(),height()-mStatusBar->height()-event->y());
	}
	// else if ( event->buttons() == Qt::RightButton && QApplication::keyboardModifiers() == Qt::ShiftModifier){
	// 	// event->ignore();
	// 	if (!active->isBrushVisible()) active->showBrush();
	// 	mBrushStartX = event->x();
	// 	// mBrushStartY = event->y();
	// }
	else if (event->buttons() == Qt::RightButton){
		getRightClickMenu();
		mRightClickMenu->popup(mapToGlobal(event->pos()));
	}
	else event->ignore();
}

/**
* \brief load custom right click menu options based on the current mode of operation
* 
*/
void MainWindow::getRightClickMenu(){
	mRightClickMenu->clear();
	//add generic items for now to test it out
	
	switch (mode){
		NormalMode :
		break;
		SelectVertex :
		break;
		SelectEdge :
		break;
		SelectFace :
		break;
		SelectFaceVertex :
		break;
		MultiSelectVertex :
		break;
		MultiSelectEdge :
		break;
		MultiSelectFace :
		break;
		MultiSelectFaceVertex :
		break;
		SelectCheckerboard :
		break;
		InsertEdge :
		break;
		DeleteEdge :
		break;
		SubdivideEdge :
		break;
		CollapseEdge :
		break;
		SpliceCorners :
		break;
		ConnectEdges :
		break;
		ExtrudeFace :
		break;
		ExtrudeFaceDS :
		break;
		ExtrudeDualFace :
		break;
		ExtrudeFaceDodeca :
		break;
		ExtrudeFaceIcosa :
		break;
		ExtrudeMultipleFaces :
		break;
		StellateFace :
		break;
		DoubleStellateFace  :
		break;
		ConnectFaceVertices :
		break;
		ConnectFaces :
		break;
		BezierConnectFaces :
		break;
		HermiteConnectFaces :
		break;
		ReorderFace :
		break;
		SubdivideFace :
		break;
		CrustModeling :
		break;
		CutEdge :
		break;
		CutVertex :
		break;
		CutEdgeandVertex  :
		break;
		CutFace  :
		break;
		TruncateEdge :
		break;
		MarkEdge :
		break;
		MarkVertex :
		break;
		ConvexHullMode :
		break;
		EditVertex :
		break;
		SelectEdgeLoop :
		break;
		SelectFaceLoop :
		break;
		SelectSimilarFaces :
		break;
		
		default:
		break;
	};
	
	switch (selectionmask){
		case MaskVertices:
		break;
		case MaskEdges: 
			// mRightClickMenu->addAction(selectEdgeAct);
			mRightClickMenu->addAction(selectEdgeLoopAct);
		break;
		case MaskFaces://face stuff
			mRightClickMenu->addAction(selectFaceAct);
			mRightClickMenu->addAction(selectFaceLoopAct);
			mRightClickMenu->addAction(selectMultipleFacesAct);
			mRightClickMenu->addAction(selectSimilarFacesAct);
			mRightClickMenu->addAction(selectCheckerboardFacesAct);
		break;
		case MaskCorners:
		break;
		default:
		break;
	};
	
	mRightClickMenu->addAction(selectAllAct);
	mRightClickMenu->addAction(selectInverseAct);
	mRightClickMenu->addAction(clearSelectedModeAct);		
	mRightClickMenu->addSeparator();
	mRightClickMenu->addAction(mEditVertexAct);
	mRightClickMenu->addSeparator();
	mRightClickMenu->addMenu(mToolsMenu);
	mRightClickMenu->addMenu(mRemeshingMenu);
}


void MainWindow::mouseMoveEvent(QMouseEvent *event) {
	// if (active->isBrushVisible()) active->redraw();
	if ( mode != NormalMode && event->buttons() == Qt::LeftButton)
		// doSelection(event->x(),this->size().height()-event->y() );
		doDrag(event->x(),this->size().height()-mStatusBar->height()-event->y() );
	// else if ( event->buttons() == Qt::RightButton && QApplication::keyboardModifiers() == Qt::ShiftModifier){
	// 	// event->ignore();
	// 	setBrushSize(mBrushSize+event->x()-mBrushStartX);
	// }
	else if (mSpinBoxMode != None){
		double d = (event->x()-mStartDragX)/20;
		switch(mSpinBoxMode){
		case One: if (mSpinBoxOne!=0) mSpinBoxOne->setValue(mSpinBoxOne->value()+d);
			break;
		case Two: if (mSpinBoxTwo!=0) mSpinBoxTwo->setValue(mSpinBoxTwo->value()+d);
			break;
		case Three: if (mSpinBoxThree!=0) mSpinBoxThree->setValue(mSpinBoxThree->value()+d);
			break;
		case Four: if (mSpinBoxFour!=0) mSpinBoxFour->setValue(mSpinBoxFour->value()+d);
			break;
		case Five: if (mSpinBoxFive!=0) mSpinBoxFive->setValue(mSpinBoxFive->value()+d);
			break;
		case Six: if (mSpinBoxSix!=0) mSpinBoxSix->setValue(mSpinBoxSix->value()+d);
			break;
		default:
			break;
		};
	}
	
	else event->ignore();
}

//if the user holds down certain keys, allow them to
//change various properties in the scene
void MainWindow::keyPressEvent(QKeyEvent *event){
		
	mStartDragX = mapFromGlobal(QCursor::pos()).x();
	mStartDragY = mapFromGlobal(QCursor::pos()).y();
	
	switch (event->key()){
	case Qt::Key_Y : mSpinBoxMode = One;
		break;
	case Qt::Key_U : mSpinBoxMode = Two;
		break;
	case Qt::Key_I : mSpinBoxMode = Three;
		break;
	case Qt::Key_O : mSpinBoxMode = Four;
		break;
	case Qt::Key_P : mSpinBoxMode = Five;
		break;
	default : mSpinBoxMode = None;
		break;
	};
}

void MainWindow::keyReleaseEvent(QKeyEvent *event){
	mSpinBoxMode = None;
}

void MainWindow::setSpinBoxes(QDoubleSpinBox *one,QDoubleSpinBox *two,QDoubleSpinBox *three,QDoubleSpinBox *four,QDoubleSpinBox *five,QDoubleSpinBox *six ){
	mSpinBoxOne = one;
	mSpinBoxTwo = two;
	mSpinBoxThree = three;
	mSpinBoxFour = four;
	mSpinBoxFive = five;
	mSpinBoxSix = six;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)  {
	QString cmd;
	// The mouse was dragged or released
	// Send this event to the subroutine handling the current event, if any
	// If we are not currently doing anything, nothing will happen
	// If ALT key is pressed we are mainpulating the view which is handled by the viewport
	if ( mode != NormalMode )
		{
			switch ( mode )
				{
				case EditVertex :       // brianb
					is_editing = false;
					if ( active->numSelectedVertices() >= 1 )	{
						DLFLVertexPtr vp = active->getSelectedVertex(0);
						vp->print();
						// active->clearSelectedVertices();
						// num_sel_verts = 0;
						redraw();
					}
					break;
				case SelectVertex :
					if ( active->numSelectedVertices() >= 1 )
						{
							DLFLVertexPtr vp = active->getSelectedVertex(0);
							vp->print();
							active->clearSelectedVertices();
							num_sel_verts = 0;
							redraw();
						}
					break;
				case SelectEdge :
					if ( active->numSelectedEdges() >= 1 ) {
						DLFLEdgePtr septr = active->getSelectedEdge(0);
						if (septr) {
							//septr->print();
							//num_sel_edges = 0;
						}
						active->clearSelectedEdges();
						redraw();
					}
					break;
				case SelectEdgeLoop:
					if ( active->numSelectedEdges() >= 1 ){
						DLFLEdgePtr septr = active->getSelectedEdge(0);
						if (septr)
							getEdgeLoopSelection(septr);
					}
					// active->clearSelectedEdges();
					active->redraw();			
					break;
				case SelectFaceLoop:
					if ( active->numSelectedEdges() >= 1 ){
						DLFLEdgePtr septr = active->getSelectedEdge(0);
						if (septr){
							if (QApplication::keyboardModifiers() == Qt::ControlModifier){ // deselect
								face_loop_start_edge = septr;
								getFaceLoopSelection(septr, true, NULL, false);
							}
							else { //select
								face_loop_start_edge = septr;
								getFaceLoopSelection(septr, true, NULL, true);
							}
						}
					}
					active->clearSelectedEdges();
					active->redraw();
					break;
				case SelectFace :
					if ( active->numSelectedFaces() >= 1 )
						{
							DLFLFacePtr fp = active->getSelectedFace(0);
							fp->print();
							active->clearSelectedFaces();
							num_sel_faces = 0;
							redraw();
						}
					break;
				case SelectSimilarFaces :
					if ( active->numSelectedFaces() >= 1 ){
						DLFLFacePtr sfptr = active->getSelectedFace(0);			
						if (sfptr){
							DLFLFacePtrArray sfptrarray;
							vector<DLFLFacePtr>::iterator it;
							DLFL::selectMatchingFaces(&object, sfptr, sfptrarray);
							for (it = sfptrarray.begin(); it != sfptrarray.end(); it++){
								if (!active->isSelected(*it)){
									active->setSelectedFace(num_sel_faces,*it);
									num_sel_faces++;
								}
							}
							redraw();
						}
					}
					break;
				case SelectFaceVertex :
					if ( active->numSelectedFaceVertices() >= 1 )
						{
							DLFLFaceVertexPtr fvp = active->getSelectedFaceVertex(0);
							fvp->print();
							active->clearSelectedFaceVertices();
							active->clearSelectedFaces();
							num_sel_faceverts = 0; num_sel_faces = 0;
							redraw();
						}
					break;
				case InsertEdge :
					if ( active->numSelectedFaceVertices() >= 2 )
						{
							DLFLFaceVertexPtr sfvptr1, sfvptr2;
							sfvptr1 = active->getSelectedFaceVertex(0);
							sfvptr2 = active->getSelectedFaceVertex(1);
							if ( sfvptr1 && sfvptr2 )
								{
									DLFLMaterialPtr mptr = sfvptr1->getFacePtr()->material();
									undoPush();
									setModified(true);
#if WITH_PYTHON
									cmd = QString( "insertEdge((" );
									cmd += QString().setNum(sfvptr1->getFaceID()) + QString(",");
									cmd += QString().setNum(sfvptr1->getVertexID()) + QString("),(");
									cmd += QString().setNum(sfvptr2->getFaceID()) + QString(",");
									cmd += QString().setNum(sfvptr2->getVertexID()) + QString("))");
									emit echoCommand(cmd);
									if( Py_IsInitialized() ) {
										PyRun_SimpleString( "from dlfl import *");
										PyRun_SimpleString( cmd.toLocal8Bit().constData() );
									}
#else
									DLFL::insertEdge(&object,sfvptr1,sfvptr2,false,mptr);
#endif
									active->clearSelectedFaces();
									active->clearSelectedFaceVertices();
									num_sel_faceverts = 0; num_sel_faces = 0;
									active->recomputePatches();
									active->recomputeNormals();
									redraw();   
								}
						}
					else if ( active->numSelectedFaceVertices() == 1 )
						{
							num_sel_faceverts=1; num_sel_faces=1;
						}
					break;
				case DeleteEdge :
					if ( active->numSelectedEdges() >= 1 )
						{
							DLFLEdgePtr septr = active->getSelectedEdge(0);
							if ( septr )
								{
									undoPush();
									setModified(true);
#if WITH_PYTHON
									cmd = QString( "deleteEdge(" );
									cmd += QString().setNum(septr->getID()) + QString(")");
									emit echoCommand(cmd);
#endif
									DLFL::deleteEdge( &object, septr, MainWindow::delete_edge_cleanup);
									active->recomputePatches();
									active->recomputeNormals();
								}
							active->clearSelectedEdges();
							redraw();
						}
					break;
				case SubdivideEdge :
					if ( active->numSelectedEdges() >= 1 )
						{
							DLFLEdgePtr septr = active->getSelectedEdge(0);
							if ( septr )
								{
									undoPush();
									setModified(true);
#if WITH_PYTHON
									cmd = QString( "subdivideEdge(" );
									cmd += QString().setNum(septr->getID()) + QString(")");
									emit echoCommand(cmd);
#endif
									DLFL::subdivideEdge(&object, num_e_subdivs,septr);
									active->recomputePatches();
									active->recomputeNormals();
								}
							active->clearSelectedEdges();
							redraw();
						}
					break;
				case CollapseEdge :
					if ( active->numSelectedEdges() >= 1 )
						{
							DLFLEdgePtr septr = active->getSelectedEdge(0);
							if ( septr )
								{
									undoPush();
									setModified(true);
#if WITH_PYTHON
									cmd = QString( "collapseEdge(" );
									cmd += QString().setNum(septr->getID()) + QString(")");
									emit echoCommand(cmd);
#endif
									DLFL::collapseEdge(&object,septr);
									active->recomputePatches();
									active->recomputeNormals();
								}
							active->clearSelectedEdges();
							redraw();
						}
					break;
				case SpliceCorners :
					if ( active->numSelectedFaceVertices() >= 2 )
						{
							DLFLFaceVertexPtr sfvptr1, sfvptr2;
							sfvptr1 = active->getSelectedFaceVertex(0);
							sfvptr2 = active->getSelectedFaceVertex(1);
							if ( sfvptr1 && sfvptr2 )
								{
									DLFLMaterialPtr mptr = sfvptr1->getFacePtr()->material();
									undoPush();
									setModified(true);
									//object.spliceCorners(sfvptr1,sfvptr2);
									DLFL::spliceCorners(&object,sfvptr1,sfvptr2);
									active->clearSelectedFaces();
									active->clearSelectedFaceVertices();
									num_sel_faceverts = 0; num_sel_faces = 0;
									active->recomputePatches();
									active->recomputeNormals();
									redraw();   
								}
						}
					else if ( active->numSelectedFaceVertices() == 1 )
						{
							num_sel_faceverts=1; num_sel_faces=1;
						}
					break;
				case ConnectFaces :
					if ( active->numSelectedFaces() >= 2 )
						{
							DLFLFacePtr sfptr1, sfptr2;
							sfptr1 = active->getSelectedFace(0);
							sfptr2 = active->getSelectedFace(1);
							if ( sfptr1 && sfptr2 )
								{
									undoPush();
									setModified(true);
									DLFL::connectFaces(&object,sfptr1,sfptr2,num_segments);
									active->recomputePatches();
									active->recomputeNormals();
									active->clearSelectedFaces();
									redraw();   
								}
						}
					else if ( active->numSelectedFaces() == 1 )
						{
							num_sel_faces = 1;
						}
					break;
				case ConnectFaceVertices :
					if ( active->numSelectedFaceVertices() >= 2 )
						{
							DLFLFaceVertexPtr sfvptr1, sfvptr2;
							sfvptr1 = active->getSelectedFaceVertex(0);
							sfvptr2 = active->getSelectedFaceVertex(1);
							if ( sfvptr1 && sfvptr2 )
								{
									undoPush();
									setModified(true);
									DLFL::connectFaces(&object,sfvptr1,sfvptr2,num_segments, max_segments);
									active->clearSelectedFaces();
									active->clearSelectedFaceVertices();
									num_sel_faceverts = 0; num_sel_faces = 0;
									active->recomputePatches();
									active->recomputeNormals();
									redraw();   
								}
						}
					else if ( active->numSelectedFaceVertices() == 1 )
						{
							num_sel_faceverts = 1; num_sel_faces = 1;
						}
					break;
				case ConnectEdges :
					if ( active->numSelectedEdges() >= 2 )
						{
							DLFLEdgePtr septr1, septr2;
							DLFLFacePtr sfptr1, sfptr2;
							septr1 = active->getSelectedEdge(0);
							septr2 = active->getSelectedEdge(1);
							sfptr1 = active->getSelectedFace(0);
							sfptr2 = active->getSelectedFace(1);
							if ( septr1 && septr2 )
								{
									undoPush();
									setModified(true);
									DLFL::connectEdges(&object,septr1,sfptr1,septr2,sfptr2);
									active->clearSelectedEdges();
									active->clearSelectedFaces();
									num_sel_edges = 0; num_sel_faces = 0;
									redraw();   
								}
						}
					else if ( active->numSelectedEdges() == 1 )
						{
							num_sel_edges = 1; num_sel_faces = 1;
						}
					break;
				case ExtrudeFace :
					if ( active->numSelectedFaces() >= 1 )
						{
							DLFLFacePtr sfptr = active->getSelectedFace(0);
							if ( sfptr )
								{
									undoPush();
									setModified(true);
									DLFL::extrudeFace(&object,sfptr,extrude_dist,num_extrusions,extrude_rot,extrude_scale);
									active->recomputePatches();
									active->recomputeNormals();
								}
							active->clearSelectedFaces();
							redraw();
						}
					break;
				case ExtrudeMultipleFaces :
					if ( active->numSelectedFaces() >= 1 )
						{
							DLFLFacePtrArray sfptrarr = active->getSelectedFaces();
							if ( sfptrarr[0] )
								{
									undoPush();
									setModified(true);
									vector<DLFLFacePtr>::iterator it;
									for(it = sfptrarr.begin(); it != sfptrarr.end(); it++) {
										DLFL::extrudeFace(&object,*it,extrude_dist,num_extrusions,extrude_rot,extrude_scale);
									}
									active->recomputePatches();
									active->recomputeNormals();						
								}
							active->clearSelectedFaces();
							redraw();
						}
					break;
				case ExtrudeFaceDS :
					if ( active->numSelectedFaces() >= 1 )
						{
							DLFLFacePtr sfptr = active->getSelectedFace(0);
							if ( sfptr )
								{
									undoPush();
									setModified(true);
									DLFL::extrudeFaceDS(&object,sfptr,extrude_dist,num_extrusions,
																			ds_ex_twist,extrude_scale);
									active->recomputePatches();
									active->recomputeNormals();
								}
							active->clearSelectedFaces();
							redraw();
						}
					break;
				case ExtrudeDualFace :
					if ( active->numSelectedFaces() >= 1 )
						{
							DLFLFacePtr sfptr = active->getSelectedFace(0);
							if ( sfptr )
								{
									undoPush();
									setModified(true);
									DLFL::extrudeDualFace(&object,sfptr,extrude_dist,num_extrusions,
																				extrude_rot,extrude_scale,
																				dual_mesh_edges_check);
									active->recomputePatches();
									active->recomputeNormals();
								}
							active->clearSelectedFaces();
							redraw();
						}
					break;
				case ExtrudeFaceDodeca :
					if ( active->numSelectedFaces() >= 1 )
						{
							DLFLFacePtr sfptr = active->getSelectedFace(0);
							if ( sfptr )
								{
									undoPush();
									setModified(true);
									DLFL::extrudeFaceDodeca(&object,sfptr,extrude_dist,num_extrusions,
																					ds_ex_twist,extrude_scale,
																					hexagonalize_dodeca_extrude);
									active->recomputePatches();
									active->recomputeNormals();
								}
							active->clearSelectedFaces();
							redraw();
						}
					break;
				case ExtrudeFaceIcosa :
					if ( active->numSelectedFaces() >= 1 )
						{
							DLFLFacePtr sfptr = active->getSelectedFace(0);
							if ( sfptr )
								{
									undoPush();
									setModified(true);
									DLFL::extrudeFaceIcosa(&object,sfptr,extrude_dist,num_extrusions,
																				 ds_ex_twist,extrude_scale);
									active->recomputePatches();
									active->recomputeNormals();
								}
							active->clearSelectedFaces();
							redraw();
						}
					break;
				case StellateFace :
					if ( active->numSelectedFaces() >= 1 )
						{
							DLFLFacePtr sfptr = active->getSelectedFace(0);
							if ( sfptr )
								{
									undoPush();
									setModified(true);
									DLFL::stellateFace(&object,sfptr,extrude_dist);
									active->recomputePatches();
									active->recomputeNormals();
								}
							active->clearSelectedFaces();
							redraw();
						}
					break;
				case DoubleStellateFace :
					if ( active->numSelectedFaces() >= 1 )
						{
							DLFLFacePtr sfptr = active->getSelectedFace(0);
							if ( sfptr )
								{
									undoPush();
									setModified(true);
									DLFL::doubleStellateFace(&object,sfptr,extrude_dist);
									active->recomputePatches();
									active->recomputeNormals();
								}
							active->clearSelectedFaces();
							redraw();
						}
					break;
				case CrustModeling :
					if ( active->numSelectedFaces() >= 1 )
						{
							DLFLFacePtr sfptr = active->getSelectedFace(0);
							if ( sfptr )
								{
									// No undo for hole punching in crust modeling mode
									// because the ids in the recreated object
									// will be different
									if ( QApplication::keyboardModifiers() == Qt::ShiftModifier )
										{
											DLFL::tagMatchingFaces(&object,sfptr);
											DLFL::punchHoles(&object);
											active->recomputePatches();
											active->recomputeNormals();
										}
									else
										DLFL::cmMakeHole(&object,sfptr,crust_cleanup);
									//                                                  active->recomputeNormals();
								}
							active->clearSelectedFaces();
							redraw();
						}
					break;
				case BezierConnectFaces :
					if ( active->numSelectedFaceVertices() >= 2 )
						{
							DLFLFaceVertexPtr sfvptr1, sfvptr2;
							sfvptr1 = active->getSelectedFaceVertex(0);
							sfvptr2 = active->getSelectedFaceVertex(1);
							if ( sfvptr1 && sfvptr2 )
								{
									undoPush();
									setModified(true);
									DLFL::bezierConnectFaces(&object,sfvptr1,sfvptr2,
																					 num_segments,nwt1,nwt2);
									active->clearSelectedFaces();
									active->clearSelectedFaceVertices();
									num_sel_faceverts = 0; num_sel_faces = 0;
									active->recomputePatches();
									active->recomputeNormals();
									redraw();   
								}
						}
					else if ( active->numSelectedFaceVertices() == 1 )
						{
							num_sel_faceverts = 1; num_sel_faces = 1;
						}
					break;
				case HermiteConnectFaces :
					if ( active->numSelectedFaceVertices() >= 2 )
						{
							DLFLFaceVertexPtr sfvptr1, sfvptr2;
							sfvptr1 = active->getSelectedFaceVertex(0);
							sfvptr2 = active->getSelectedFaceVertex(1);
							if ( sfvptr1 && sfvptr2 )
								{
									undoPush();
									setModified(true);
									if ( symmetric_weights )
										DLFL::hermiteConnectFaces(&object,sfvptr1,sfvptr2,
																							num_segments,nwt1,nwt1,
																							max_segments,num_extra_twists);
									else
										DLFL::hermiteConnectFaces(&object,sfvptr1,sfvptr2,
																							num_segments,nwt1,nwt2,
																							max_segments,num_extra_twists);
									active->clearSelectedFaces();
									active->clearSelectedFaceVertices();
									num_sel_faceverts = 0; num_sel_faces = 0;
									active->recomputePatches();
									active->recomputeNormals();
									redraw();
								}
						}
					else if ( active->numSelectedFaceVertices() == 1 )
						{
							num_sel_faceverts = 1; num_sel_faces = 1;
						}
					break;
				case ReorderFace :
					if ( active->numSelectedFaceVertices() >= 1 )
						{
							DLFLFaceVertexPtr sfvptr = active->getSelectedFaceVertex(0);
							if ( sfvptr ) sfvptr->getFacePtr()->reorder(sfvptr);
							redraw();
						}
					break;
				case SubdivideFace :
				case MultiSelectFace :
					// num_sel_faces++;
					break;
					//from ozgur
				case CutEdge :
					if ( active->numSelectedEdges() >= 1 )
						{
							DLFLEdgePtr septr = active->getSelectedEdge(0);
							if ( septr )
								{
									undoPush();
									septr->ismarked = 1- septr->ismarked;
									// DLFL::cutEdge( &object, septr, peelDistance_factor,pnormalBendS_factor,pnormalBendT_factor, peel_all_edges);
								}
							active->clearSelectedEdges();
							redraw();
						}
					break;
				case TruncateEdge :
					if ( active->numSelectedEdges() >= 1 )
						{
							DLFLEdgePtr septr = active->getSelectedEdge(0);
							if ( septr )
								{
									undoPush();
									septr->ismarked = 1- septr->ismarked;
									// DLFL::cutEdge( &object, septr, peelDistance_factor,pnormalBendS_factor,pnormalBendT_factor, peel_all_edges);
								}
							active->clearSelectedEdges();
							redraw();
						}
					break;
				case CutVertex :
					if ( active->numSelectedVertices() >= 1 )
						{
							DLFLVertexPtr svptr = active->getSelectedVertex(0);
							if ( svptr )
								{
									undoPush();
									svptr->ismarked = 1 - svptr->ismarked;
								}
							// active->clearSelectedVertices();
							redraw();
						}
					break;

				case CutEdgeandVertex :
					if ( active->numSelectedEdges() >= 1 )
						{	
							if ( active->numSelectedEdges() >= 1 ) {
								DLFLVertexPtr svptr = active->getSelectedVertex(0);
								DLFLEdgePtr septr = active->getSelectedEdge(0);
								if ( septr ) {
									if (svptr) {
										undoPush();
										septr->ismarked = 1- septr->ismarked;
										// DLFL::cutEdge( &object, septr, peelDistance_factor,pnormalBendS_factor,pnormalBendT_factor, peel_all_edges);
									}
								}
								active->clearSelectedEdges();
								active->clearSelectedVertices();
								redraw();
							}
						}
					break;
				case MarkEdge:
					if ( active->numSelectedEdges() >= 1 )
						{
							DLFLEdgePtr septr = active->getSelectedEdge(0);
							if ( septr )
								{
									undoPush();
									septr->ismarked = 1- septr->ismarked;
								}
							active->clearSelectedEdges();
							redraw();
						}
					break;

				case MarkVertex :
					if ( active->numSelectedVertices() >= 1 )
						{
							DLFLVertexPtr svptr = active->getSelectedVertex(0);
							if ( svptr )
								{
									undoPush();
									svptr->ismarked = 1 - svptr->ismarked;
								}
							active->clearSelectedVertices();
							redraw();
						}
					break;

				case CutFace :
					if ( active->numSelectedFaces() >= 1 )
						{
							DLFLFacePtr sfptr = active->getSelectedFace(0);
							if ( sfptr )
								{
									undoPush();
									sfptr->ismarked = 1 - sfptr->ismarked;
								}
							active->clearSelectedFaces();
							redraw();
						}
					break;
				}//end switch (mode)

		}//end if (mode != NormalMode)
	// else if ( event->buttons() == Qt::RightButton && QApplication::keyboardModifiers() == Qt::ShiftModifier){
	// 	// event->ignore();
	// 	mBrushStartX = 0;
	// 	// mBrushStartY = 0;
	// }
	else event->ignore();
}//end function mousereleaseevent


void MainWindow::testConvexHull(void) {
	// For testing convex hull subroutine
	Vector3dArray vertices;
	vertices.resize(8);
	vertices[0].set(0,0,0);
	vertices[1].set(0,10,0);
	vertices[2].set(10,10,0);
	vertices[3].set(10,0,0);
	vertices[4].set(0,0,10);
	vertices[5].set(0,10,10);
	vertices[6].set(10,10,10);
	vertices[7].set(10,0,10);
	DLFLConvexHull convexhull;
	convexhull.createHull(vertices);

	ofstream file;
	file.open("convexhull.dlfl");
	convexhull.writeDLFL(file);
	file.close();
}

void MainWindow::performRemeshing(void) {

	switch ( remeshingscheme )
		{
		case Dual :
			createDual();
			break;
		case Root3 :
			subdivideRoot3();
			break;
		case DualVertexTrunc :
			subdivideSqrt3();
			break;
		case GlobalStellate :
			globalStellate();
			break;
		case Star :
			subdivideStar();
			break;
		case Generic1264 :
			subdivide1264();
			break;
		case Honeycomb :
			subdivideHoneycomb();
			break;
		case VertexTrunc :
			subdivideVertexCutting();
			break;
		case DualGeneric1264 :
			subdivideDual1264();
			break;
		case LinearVertexInsertion :
			subdivideLinearVertexInsertion();
			break;
		case CatmullClark :
			subdivideCatmullClark();
			break;
		case ModifiedStellate :
			subStellate1();
			break;
		case DooSabin :
			subdivideDooSabin();
			break;
		case CornerCutting :
			subdivideCornerCutting();
			break;
		case Simplest :
			subdivideSimplest();
			break;
		case Pentagonal :
			subdividePentagonal();
			break;
		case CubicPentagonal :
			subdivideCubicPentagonal();
			break;
		case DualPentagonal :
			subdivideDualPentagonal();
			break;
		case LoopStyle :
			subdivideLoopStyle();
			break;
		case Loop :
			subdivideLoop();
			break;
		case Root4 :
			subdivideRoot4();
			break;
		case DualLoop :
			subdivideDualLoop();
			break;
		case GlobalCubicExtrude :
			cout << "NOT YET IMPLEMENTED!!!" << endl;
			break;
		case CheckerBoard :
			subdivideCheckerBoard();
			break;
		case DualGlobalCubicExtrude :
			cout << "NOT YET IMPLEMENTED!!!" << endl;
			break;
		case DualCheckerBoard :
			subdivideDualCheckerBoard();
			break;
		case PentagonPreserving :
			subdividePentagonPreserving();
			break;
		case DualPentagonPreserving :
			subdivideDualPentagonPreserving();
			break;
		case HexagonPreserving :
			subdivideRoot4();
			break;
		case DualHexagonPreserving :
			subdivideDualHexagonPreserving();
			break;
		case Fractal :
			subdivideFractal();
			break;
		case ModifiedDoubleStellate :
			subStellate2();
			break;
		case Dome :
			subdivideDome();
			break;
		case DooSabinBC :
			subdivideDooSabinBC();
			break;
		case DooSabinBCNew :
			subdivideDooSabinBCNew();
			break;
		}
	setMode(mode);
	redraw();
}

// Change the renderer for all viewports
void MainWindow::setRenderer(DLFLRendererPtr rp) {
	active->setRenderer(rp);
}

// Return pointer to the active GLWidget
GLWidget *MainWindow::getActive() {
	return active;
}

// Override redraw() method to mark subwindows also for redraw
void MainWindow::redraw() {
	active->redraw();
}

//--- Methods to perform various operations ---//

// Switch to specified operating mode
void MainWindow::setMode(Mode m) {
	mode = m;
	
	switch ( mode )	{
		// case BezierMode: // brianb
		//       object.bezierDefaults();
		//       break;
	case EditVertex :    // brianb
	case SelectVertex :
	case MultiSelectVertex :
	case CutVertex :
	case MarkVertex :
		setSelectionMask(MainWindow::MaskVertices);
		MainWindow::num_sel_verts = 0;
		break;
	case SelectEdge :
	case SelectEdgeLoop :
	case MultiSelectEdge :
	case DeleteEdge :
	case SubdivideEdge :
	case CollapseEdge :
	case ConnectEdges :
	case CutEdge :
	case TruncateEdge :
	case MarkEdge :
		setSelectionMask(MainWindow::MaskEdges);
		MainWindow::num_sel_edges = 0;
		break;
	case SelectFace :
	case SelectFaceLoop :
	case MultiSelectFace :
	case SelectCheckerboard :
	case ExtrudeFace :
	case ExtrudeFaceDS :
	case ExtrudeDualFace :
	case ExtrudeFaceDodeca :
	case ExtrudeFaceIcosa :
	case ExtrudeMultipleFaces :
	case StellateFace :
	case DoubleStellateFace :
	case ConnectFaces :
	case BezierConnectFaces :
	case HermiteConnectFaces :
	case ReorderFace :
	case SubdivideFace :
	case CrustModeling :
	case CutFace :
	case SelectSimilarFaces :
		setSelectionMask(MainWindow::MaskFaces);
		MainWindow::num_sel_faces = 0;
		break;
	case SelectFaceVertex :
	case MultiSelectFaceVertex :
	case InsertEdge :
	case SpliceCorners :
	case ConnectFaceVertices :
		setSelectionMask(MainWindow::MaskCorners);
		MainWindow::num_sel_faceverts = 0;
		break;
	case NormalMode:
		setSelectionMask(MainWindow::MaskObject);
	default :
		// Nothing to be done for other modes except clearing selection lists
		setSelectionMask(MainWindow::MaskObject);									
		MainWindow::clearSelected();
		break;
	}
	// if (mode != MultiSelectFace)
		// active->hideBrush();
	// else active->showBrush();	
	
	QString s;
	//this switch statement is for setting the string for the Heads up display
	switch (mode){
		case NormalMode: s = tr("Normal Mode");
		break;
		case SelectVertex: s = tr("Select Vertex");
		break;
		case SelectEdge: s = tr("Select Edge");
		break;
		case SelectFace: s = tr("Select Face");
		break;
		case SelectFaceVertex: s = tr("Select Corner");
		break;
		case MultiSelectVertex: s = tr("Multi-Select Vertex");
		break;
		case MultiSelectEdge: s = tr("Multi-Select Edge");
		break;
		case MultiSelectFace: s = tr("Multi-Select Face");
		break;
		case MultiSelectFaceVertex: s = tr("Multi-Select Corner");
		break;
		case SelectCheckerboard: s = tr("Select Checkerboard");
		break;
		case InsertEdge: s = tr("Insert Edge");
		break;
		case DeleteEdge: s = tr("Delete Edge");
		break;
		case SubdivideEdge: s = tr("Subdivide Edge");
		break;
		case CollapseEdge: s = tr("Collapse Edge");
		break;
		case SpliceCorners: s = tr("Splice Corners");
		break;
		case ConnectEdges: s = tr("Connect Edges");
		break;
		case ExtrudeFace: s = tr("Cubical Extrusion");
		break;
		case ExtrudeFaceDS: s = tr("Doo Sabin Extrusion");
		break;
		case ExtrudeDualFace: s = tr("Dual Extrusion");
		break;
		case ExtrudeFaceDodeca: s = tr("Dodecahedral Extrusion");
		break;
		case ExtrudeFaceIcosa: s = tr("Icosahedral Extrusion");
		break;
		case ExtrudeMultipleFaces: s = tr("Extrude Multiple Faces");
		break;
		case StellateFace: s = tr("Stellate Face");
		break;
		case DoubleStellateFace : s = tr("Double Stellate Face");
		break;
		case ConnectFaceVertices: s = tr("Connect Corners");
		break;
		case ConnectFaces: s = tr("Connect Faces");
		break;
		case BezierConnectFaces: s = tr("Bezier Connect Faces");
		break;
		case HermiteConnectFaces: s = tr("Hermite Connect Faces");
		break;
		case ReorderFace: s = tr("Reorder Face");
		break;
		case SubdivideFace: s = tr("Subdivide Face");
		break;
		case CrustModeling: s = tr("Crust Modeling");
		break;
		case CutEdge: s = tr("Cut Edge");
		break;
		case CutVertex: s = tr("Cut Vertex");
		break;
		case CutEdgeandVertex : s = tr("Cut Edge and Vertex");
		break;
		case CutFace : s = tr("Cut Face");
		break;
		case TruncateEdge: s = tr("Truncate Edge");
		break;
		case MarkEdge: s = tr("Mark Edge");
		break;
		case MarkVertex: s = tr("Mark Vertex");
		break;
		case ConvexHullMode: s = tr("Convex Hull Mode");
		break;
		case EditVertex: s = tr("Edit Vertex");
		break;
		case SelectEdgeLoop: s = tr("Select Edge Loop");
		break;
		case SelectFaceLoop: s = tr("Select Face Loop");
		break;
		case SelectSimilarFaces: s = tr("Select Similar Faces");
	};
	active->setModeString(s);
}

void MainWindow::setSelectionMask(SelectionMask m){
	selectionmask = m;
	
	active->clearSelected();
	active->repaint();
	
	switch(selectionmask){
	case MaskVertices : active->setSelectionMaskString(tr("Vertices"));
	break;
	case MaskEdges :	active->setSelectionMaskString(tr("Edges"));
	break;
	case MaskFaces :	active->setSelectionMaskString(tr("Faces"));
	break;
	case MaskCorners :	active->setSelectionMaskString(tr("Corners"));
	break;
	default:
	break;
	}
}

void MainWindow::setRemeshingScheme(RemeshingScheme scheme) {
	remeshingscheme = scheme;
	QString s;
	
	switch (remeshingscheme){		
		case Dual : s = tr("Dual");
		break;
		case Root3 : s = tr("Root-3");
		break;
		case DualVertexTrunc : s = tr("Dual Vertex Truncation");
		break;
		case GlobalStellate : s = tr("Global Stellate");
		break;
		case Star : s = tr("Star");
		break;
		case Generic1264 : s = tr("Generic 12.6.4");
		break;
		case Honeycomb : s = tr("Honeycomb");
		break;
		case VertexTrunc : s = tr("Vertex Truncation");
		break;
		case DualGeneric1264 : s = tr("Dual Generic 12.6.4");
		break;
		case LinearVertexInsertion : s = tr("Linear Vertex Truncation");
		break;
		case CatmullClark : s = tr("Catmull-Clark");
		break;
		case ModifiedStellate : s = tr("Modified Stellate");
		break;
		case DooSabin : s = tr("Doo Sabin");
		break;
		case CornerCutting : s = tr("Corner Cutting");
		break;
		case Simplest : s = tr("Simplest");
		break;
		case Pentagonal : s = tr("Pentagonal");
		break;
		case CubicPentagonal : s = tr("Cubic Pentagonal");
		break;
		case DualPentagonal : s = tr("Dual Pentagonal");
		break;
		case LoopStyle : s = tr("Loop Style");
		break;
		case Loop : s = tr("Loop");
		break;
		case Root4 : s = tr("Root4");
		break;
		case DualLoop : s = tr("Dual Loop");
		break;
		case GlobalCubicExtrude : s = tr("Global Cubic Extrude");
		break;
		case CheckerBoard : s = tr("Checkerboard");
		break;
		case DualGlobalCubicExtrude : s = tr("Dual Global Cubic Extrude");
		break;
		case DualCheckerBoard : s = tr("Dual Checkerboard");
		break;
		case PentagonPreserving : s = tr("Pentagon Preserving");
		break;
		case DualPentagonPreserving : s = tr("Dual Pentagon Preserving");
		break;
		case HexagonPreserving : s = tr("Hexagon Preserving");
		break;
		case DualHexagonPreserving : s = tr("Dual Hexagon Preserving");
		break;
		case Fractal : s = tr("Fractal");
		break;
		case ModifiedDoubleStellate : s = tr("Modified Double Stellate");
		break;
		case Dome : s = tr("Dome");
		break;
		case DooSabinBC : s = tr("Doo Sabin BC");
		break;
		case DooSabinBCNew :  s = tr("Doo Sabin BC New");
		break;
		default: s = tr("None");
		break;
	};
	active->setRemeshingSchemeString(s);
}


// Read the DLFL object from a file
void MainWindow::readObject(const char * filename) {
	active->clearSelected();
	ifstream file;
	file.open(filename);
	if ( strstr(filename,".dlfl") || strstr(filename,".DLFL") )
		object.readDLFL(file);
	else if ( strstr(filename,".obj") || strstr(filename,".OBJ") )
		object.readObject(file);
	file.close();
}

// Read the DLFL object from a file
void MainWindow::readObjectQFile(QString filename) {
	active->clearSelected();
	QFile file(filename);
	file.open(QIODevice::ReadOnly);

	QByteArray ba = file.readAll();
	const char *filecontents = ba.data();
	string str(filecontents);
	istringstream filestring(str);

	if ( filename.indexOf(".dlfl") == filename.length()-4 || filename.indexOf(".dlfl") == filename.length()-4 )
		object.readDLFL(filestring);
	else if ( filename.indexOf(".OBJ") == filename.length()-4 || filename.indexOf(".obj") == filename.length()-4 )
		object.readObject(filestring);
	file.close();

#ifdef WITH_PYTHON
	DLFLObjectPtr obj = &object;
	if( obj )
		emit loadedObject(obj,filename);
#endif
	active->createPatchObject( );
}

// Read the DLFL object from a file - use alternate OBJ reader for OBJ files
void MainWindow::readObjectAlt(const char * filename) {
	active->clearSelected();
	ifstream file;
	file.open(filename);
	if ( strstr(filename,".dlfl") || strstr(filename,".DLFL") )
		object.readDLFL(file);
	else if ( strstr(filename,".obj") || strstr(filename,".OBJ") )
		object.readObjectAlt(file);
	file.close();
}

void MainWindow::readObjectOBJ(const char * filename) {
	active->clearSelected();
	ifstream file;
	file.open(filename);
	object.readObject(file);
	file.close();
}

void MainWindow::readObjectDLFL(const char * filename) {
	active->clearSelected();
	ifstream file;
	file.open(filename);
	object.readDLFL(file);
	file.close();
}

// Write the DLFL object to a file
void MainWindow::writeObject(const char * filename, bool with_normals, bool with_tex_coords) {
	ofstream file;
	file.open(filename);
	if ( strstr(filename,".dlfl") || strstr(filename,".DLFL") )
		object.writeDLFL(file);
	else if ( strstr(filename,".obj") || strstr(filename,".OBJ") )
		object.writeObject(file,with_normals,with_tex_coords);
	file.close();
}

void MainWindow::writeObjectOBJ(const char * filename, bool with_normals, bool with_tex_coords) {
	ofstream file;
	file.open(filename);
	// object.writeObject(file,with_normals,with_tex_coords);
	file.close();
}

/* stuart - bezier export */
void MainWindow::writePatchOBJ( const char *filename ) {
	if( active->patchobject() != NULL ) {
		ofstream file;
		file.open(filename);
		active->patchobject()->objPatchWrite(file);
	}
}

void MainWindow::writeObjectDLFL(const char * filename) {
	ofstream file;
	file.open(filename);
	object.writeDLFL(file);
	file.close();
}

// File handling
void MainWindow::openFile(void) {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File..."),
																									"$HOME", tr("All Supported Files (*.obj *.dlfl);;Wavefront OBJ Files (*.obj);;DLFL Files (*.dlfl);;All Files (*)"),
																									0, QFileDialog::DontUseSheet);
	if (!fileName.isEmpty()){
		if (!curFile.isEmpty()){
			undoPush();
			setModified(false);
		}
		QByteArray ba = fileName.toLatin1();
		const char *filename = ba.data();
		mWasPrimitive = false;
		mIsPrimitive = false;
		readObject(filename);
#ifdef WITH_PYTHON
		// Emit and send to python script editor
		DLFLObjectPtr obj = &object;
		if( obj )
			emit loadedObject(obj,fileName);
#endif
		active->recomputePatches();
		active->recomputeNormals();
		setCurrentFile(fileName);
		active->redraw();
	}
}


bool MainWindow::saveFile(bool with_normals, bool with_tex_coords) {
	if (!curFile.isEmpty() && !mIsPrimitive){
		QByteArray ba = curFile.toLatin1();
		const char *filename = ba.data();
		writeObject(filename,with_normals,with_tex_coords);
		return true;
	}
	else {
		QString fileName = QFileDialog::getSaveFileName(this,
																										tr("Save File As..."),
																										curFile,
																										tr("All Supported Files (*.obj *.dlfl);;Wavefront OBJ Files (*.obj);;DLFL Files (*.dlfl);;All Files (*)"),
																										0, QFileDialog::DontUseSheet);
		if (!fileName.isEmpty()){
			if (!fileName.indexOf(".obj") || !fileName.indexOf(".dlfl") || !fileName.indexOf(".OBJ") || !fileName.indexOf(".DLFL"))
				fileName.append(".dlfl");
			setCurrentFile(fileName);	
			QByteArray ba = fileName.toLatin1();
			const char *filename = ba.data();
			writeObject(filename,with_normals,with_tex_coords);
			return true;
		}
	}
	return false;
}

bool MainWindow::saveFileAs(bool with_normals, bool with_tex_coords) {
	QString fileName = QFileDialog::getSaveFileName(this,
																									tr("Save File As..."),
																									curFile,
																									tr("All Supported Files (*.obj *.dlfl);;Wavefront OBJ Files (*.obj);;DLFL Files (*.dlfl);;All Files (*)"),
																									0, QFileDialog::DontUseSheet );
	if (!fileName.isEmpty()){
		if (!fileName.indexOf(".obj") || !fileName.indexOf(".dlfl") || !fileName.indexOf(".OBJ") || !fileName.indexOf(".DLFL"))
			fileName.append(".dlfl");
		setCurrentFile(fileName);				
		QByteArray ba = fileName.toLatin1();
		const char *filename = ba.data();
		writeObject(filename,with_normals,with_tex_coords);
		return true;
	}
	return false;
}

void MainWindow::openFileOBJ(void) {	
	// QString fileName = QFileDialog::getOpenFileName(this,
	// 																								tr("Open File..."),
	// 																								"$HOME",
	// 																								tr("Wavefront OBJ Files (*.obj);;All Files (*)"));
	// if (!fileName.isEmpty()) {
	// 	
	// 	setCurrentFile(fileName);
	// 	undoPush();
	// 	setModified(true);
	// 	QByteArray ba = fileName.toLatin1();
	// 	const char *filename = ba.data();
	// 	readObjectOBJ(filename);
	// 	active->recomputePatches();
	// 	active->recomputeNormals();
	// }
}

void MainWindow::saveFileOBJ(bool with_normals, bool with_tex_coords) {
	// QString fileName = QFileDialog::getSaveFileName(this,
	// 																								tr("Save File As..."),
	// 																								curFile,
	// 																								tr("Wavefront OBJ Files (*.obj);;All Files (*)"));
	// if (!fileName.isEmpty()){
	// 	QByteArray ba = fileName.toLatin1();
	// 	const char *filename = ba.data();
	// 	writeObjectOBJ(filename,with_normals,with_tex_coords);
	// }
}

/* stuart - bezier export */
bool MainWindow::saveFileBezierOBJ( ) {
	QString fileName = QFileDialog::getSaveFileName(this,
																									tr("Save Bezier Patch (OBJ)..."),
																									curFile,
																									tr("Wavefront OBJ Files (*.obj);;All Files (*)"),
																									0, QFileDialog::DontUseSheet);
	if (!fileName.isEmpty()){
		QByteArray ba = fileName.toLatin1();
		const char *filename = ba.data();
		writePatchOBJ(filename);
		return true;
	}
	return false;
}

void MainWindow::openFileDLFL(void) {
	// QString fileName = QFileDialog::getOpenFileName(this,
	// 																								tr("Open File..."),
	// 																								"$HOME",
	// 																								tr("DLFL Files (*.dlfl);;All Files (*)"));
	// if (!fileName.isEmpty()){
	// 	setCurrentFile(fileName);
	// 	undoPush();
	// 	setModified(true);
	// 	QByteArray ba = fileName.toLatin1();
	// 	const char *filename = ba.data();
	// 	readObjectDLFL(filename);
	// 	active->recomputePatches();
	// 	active->recomputeNormals();
	// }
}

void MainWindow::saveFileDLFL(void) {

	// QString fileName = QFileDialog::getSaveFileName(this,
	// 																								tr("Save File As..."),
	// 																								curFile,
	// 																								tr("DLFL Files (*.dlfl);;All Files (*)"));
	// if (!fileName.isEmpty()){
	// 	QByteArray ba = fileName.toLatin1();
	// 	const char *filename = ba.data();
	// 	writeObjectDLFL(filename);
	// }
}

void MainWindow::setCurrentFile(QString fileName) {

	curFile = fileName;
	QString shownName;
	if (curFile.isEmpty())
		shownName = "untitled.obj";
	else
		shownName = QFileInfo(curFile).fileName();

	setWindowTitle( tr("%1[*] - %2").arg(shownName).arg(tr("TopMod")));
	setWindowModified(false);
}

// void MainWindow::cleanupForExit(void) // Do memory cleanup if any before exit {
// // Nothing to be done
// }

void MainWindow::loadCube(){	
	if (isModified())
		undoPush();
	setModified(false);
	mIsPrimitive = true;
	mWasPrimitive = true;
	setCurrentFile(tr("cube.obj"));
	readObjectQFile(":/cube.obj");
	//active->recomputePatches();
	active->recomputeNormals();
	active->redraw();
}

void MainWindow::loadOctahedron(){	
	if (isModified())
		undoPush();
	setModified(false);
	mIsPrimitive = true;
	mWasPrimitive = true;
	setCurrentFile(tr("octahedron.obj"));
	readObjectQFile(":/octahedron.obj");
	//active->recomputePatches();
	active->recomputeNormals();
	active->redraw();
}

void MainWindow::loadTetrahedron(){	
	if (isModified())
		undoPush();
	setModified(false);
	mIsPrimitive = true;
	mWasPrimitive = true;
	setCurrentFile(tr("tetrahedron.obj"));
	readObjectQFile(":/tetrahedron.obj");
	//active->recomputePatches();
	active->recomputeNormals();
	active->redraw();
}

void MainWindow::loadDodecahedron(){	
	if (isModified())
		undoPush();
	setModified(false);
	mIsPrimitive = true;
	mWasPrimitive = true;
	setCurrentFile(tr("dodecahedron.obj"));
	readObjectQFile(":/dodecahedron.obj");
	//active->recomputePatches();
	active->recomputeNormals();
	active->redraw();
}

void MainWindow::loadIcosahedron(){	
	if (isModified())
		undoPush();
	setModified(false);	
	mIsPrimitive = true;
	mWasPrimitive = true;
	setCurrentFile(tr("icosahedron.obj"));
	readObjectQFile(":/icosahedron.obj");
	//active->recomputePatches();
	active->recomputeNormals();
	active->redraw();
}

void MainWindow::loadSoccerball(){	
	if (isModified())
		undoPush();
	setModified(false);
	mIsPrimitive = true;
	mWasPrimitive = true;
	setCurrentFile(tr("soccerball.obj"));
	readObjectQFile(":/soccerball.obj");
	//active->recomputePatches();
	active->recomputeNormals();
	active->redraw();
}

void MainWindow::loadGeodesic(){	
	if (isModified())
		undoPush();
	setModified(false);
	mIsPrimitive = true;
	mWasPrimitive = true;
	setCurrentFile(tr("geodesic.obj"));
	readObjectQFile(":/geodesic.obj");
	//active->recomputePatches();
	active->recomputeNormals();
	active->redraw();
}

bool MainWindow::isModified(){
	return mIsModified;
}

void MainWindow::setModified(bool isModified){
	mIsModified = isModified;
	setWindowModified(mIsModified);	
}

void MainWindow::switchPerspView(){
	active->resetCamera();
	// active->switchTo(VPPersp);
	active->redraw();
}

void MainWindow::switchTopView(){
	// active->switchTo(VPTop);
	active->redraw();
}

void MainWindow::switchBottomView(){
	// active->switchTo(VPBottom);
	active->redraw();
}

void MainWindow::switchRightView(){
	// active->switchTo(VPRight);
	active->redraw();
}

void MainWindow::switchLeftView(){
	// active->switchTo(VPLeft);
	active->redraw();
}

void MainWindow::switchFrontView(){
	// active->switchTo(VPFront);
	active->redraw();
}

void MainWindow::switchBackView(){
	// active->switchTo(VPBack);
	active->redraw();
}

//recurse through selected faces to get a list of checkerboard selection for the entire object
void MainWindow::getCheckerboardSelection(DLFLFacePtr fptr) {
	// fparray.clear();
	// sfptr = active->selectFace(x,y);
	if (fptr){
		int numShared = 0;
		DLFLFacePtrArray fparray;
		vector<DLFLFacePtr>::iterator it;
		fptr->getNeighboringFaces(fparray);
		for ( it = fparray.begin(); it != fparray.end(); it++){
			if (fptr->sharesOneVertex((*it)) && !active->isSelected(*it)){
				numShared++;
				if (deselect_edges){
					active->clearSelectedFace(*it);
					num_sel_faces--;
				}
				else {
					active->setSelectedFace(num_sel_faces,*it);
					num_sel_faces++;					
				}
				getCheckerboardSelection((*it));
			}
		}//end for loop
		if (numShared == 0) return; //break out of recursive loop is there are no one vertex sharing faces
	}		
}

//recurse through selected edge to get a list of 
void MainWindow::getEdgeLoopSelection(DLFLEdgePtr eptr) {
	if (eptr){
		//first check to see if edge only connects to only three other edges
		DLFLVertexPtr vp1, vp2;
		DLFLEdgePtrArray eparray;
		vector<DLFLEdgePtr>::iterator it;
		eptr->getVertexPointers(vp1,vp2);
		if (vp1->valence() == 4 && vp2->valence() == 4){
			//get edges conected to vertex 1
			eparray.clear();
			vp1->getEdges(eparray);
			//loop through them to find the one that doesn't share a face
			for (it = eparray.begin(); it != eparray.end(); it++){
				if ( !coFacial(*it,eptr) ){
					if (deselect_edges){
						active->clearSelectedEdge(*it);
						num_sel_edges--;
						getEdgeLoopSelection(*it);
					}
					else if (!active->isSelected(*it)){
						active->setSelectedEdge(num_sel_edges,*it);
						num_sel_edges++;
						getEdgeLoopSelection(*it);
					}
				}
			}//end for loop
			//get edges conected to vertex 2
			eparray.clear();
			vp2->getEdges(eparray);
			//loop through them to find the one that doesn't share a face
			for (it = eparray.begin(); it != eparray.end(); it++){
				if ( !coFacial(*it,eptr) ){
					if (deselect_edges){
						active->clearSelectedEdge(*it);
						num_sel_edges--;
						getEdgeLoopSelection(*it);
					}
					else if (!active->isSelected(*it)){
						active->setSelectedEdge(num_sel_edges,*it);
						num_sel_edges++;
						getEdgeLoopSelection(*it);
					}
				}
			}//end for loop
			return;
		}//end if vp->valence
		return;
	}	
}

//recurse through selected edge to get a list of 
void MainWindow::getFaceLoopSelection(DLFLEdgePtr eptr, bool start, DLFLFacePtr face_loop_marker, bool select_face_loop) {
	if ( (eptr == face_loop_start_edge) && !start) 
		return;

	int idx = 0;
	DLFLEdgePtrArray edges;
	vector<DLFLEdgePtr>::iterator it;
	DLFLFacePtr fptr1, fptr2;
	//get the two faces corresponding to this edge ptr
	eptr->getFacePointers(fptr1,fptr2);
	//check if the two faces are quads
	if (fptr1 && fptr1->numFaceVertexes() == 4 && !(fptr1 == face_loop_marker) ){
		// cout << select_face_loop << "\t" << !active->isSelected(fptr1) << std::endl;
		if (/*select_face_loop &&*/ !active->isSelected(fptr1)){
			active->setSelectedFace(num_sel_faces,fptr1);
			num_sel_faces++;
		}
		/*else if (active->isSelected(fptr1)){
			active->clearSelectedFace(fptr1);
			num_sel_faces--;
			}*/
		fptr1->getEdges(edges);
		idx =0;
		for (it = edges.begin(); it != edges.end(); it++){
			if (*it == eptr){
				getFaceLoopSelection(edges[(idx+2)%4],false,fptr1, select_face_loop);
			}
			idx++;
		}//end for loop
	}
	if (fptr2 && fptr2->numFaceVertexes() == 4 && !(fptr2 == face_loop_marker) ){
		if (/*select_face_loop &&*/!active->isSelected(fptr2)){
			active->setSelectedFace(num_sel_faces,fptr2);
			num_sel_faces++;
		}
		/*else if (active->isSelected(fptr2)){
			active->clearSelectedFace(fptr2);
			num_sel_faces--;				
			}*/
		fptr2->getEdges(edges);
		idx =0;
		for (it = edges.begin(); it != edges.end(); it++){
			if (*it == eptr){
				getFaceLoopSelection(edges[(idx+2)%4], false, fptr2, select_face_loop);
				// return;
			}
			idx++;
		}//end for loop
		// }//end if fptr2	
	}
}
