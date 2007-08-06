/****************************************************************************
 **
 **
 ****************************************************************************/
#include <QtGui>

#include "BasicsMode.hh"

/*!
	\ingroup gui
	@{
	
	\class BasicsMode
	\brief Basics Operations including insert-edge and delete-edge.
	
	\note 
	
	\see BasicsMode
*/

/*!
* \brief Constructor
* 
* @param parent the MainWindow widget
* @param sm the shortcut manager class for adding a custom shortcut to each menu action or icon
* @param actionList the master list of actions for use with the CommandCompleter class
* 
*/
BasicsMode::BasicsMode(QWidget *parent, QShortcutManager *sm, QWidget *actionList)
		: QWidget(parent) {
		
	setParent(0);
	mParent = parent;
	
	//here we set the default mode for when the application is executed.
	((MainWindow*)mParent)->setMode(MainWindow::InsertEdge);
	
	mInsertEdgeWidget = new QWidget;
  mDeleteEdgeWidget = new QWidget;
  mCollapseEdgeWidget = new QWidget;
  mSubdivideEdgeWidget = new QWidget;
  mConnectEdgesWidget = new QWidget;
  mSpliceCornersWidget = new QWidget;
  mTransformsWidget = new QWidget;

	//each mode widget will each be added to the 
	//ToolOptionsDockWidget of the MainWindow class
	setupInsertEdge();
	setupDeleteEdge();
	setupCollapseEdge();
	setupSubdivideEdge();
	setupConnectEdges();
	setupSpliceCorners();
	setupTransforms();
	
	mInsertEdgeAction = new QAction(tr("Insert Edge"),this);
	mInsertEdgeAction->setIcon(QIcon(":/images/insert_edge.png"));
	mInsertEdgeAction->setCheckable(true);
	mInsertEdgeAction->setChecked(true);
	// sm->registerAction(mInsertEdgeAction, "Basics Modes", "9");
	mInsertEdgeAction->setStatusTip(tr("Enter Insert Edge Mode"));
	mInsertEdgeAction->setToolTip(tr("Insert Edge Mode"));
	connect(mInsertEdgeAction, SIGNAL(triggered()), this, SLOT(triggerInsertEdge()));
	connect(mInsertEdgeAction, SIGNAL(hovered()), ((MainWindow*)mParent), SLOT(setAnimatedHelpImage()));
	actionList->addAction(mInsertEdgeAction);	

	mDeleteEdgeAction = new QAction(tr("Delete Edge"),this);
	mDeleteEdgeAction->setIcon(QIcon(":/images/delete_edge.png"));
	mDeleteEdgeAction->setCheckable(true);
	// sm->registerAction(mDeleteEdgeAction, "Basics Modes", "9");
	mDeleteEdgeAction->setStatusTip(tr("Enter Delete Edge Mode"));
	mDeleteEdgeAction->setToolTip(tr("Delete Edge Mode"));
	connect(mDeleteEdgeAction, SIGNAL(triggered()), this, SLOT(triggerDeleteEdge()));
	actionList->addAction(mDeleteEdgeAction);

	mCollapseEdgeAction = new QAction(tr("Collapse Edge"),this);
	mCollapseEdgeAction->setIcon(QIcon(":/images/collapse_edge.png"));
	mCollapseEdgeAction->setCheckable(true);
	// sm->registerAction(mCollapseEdgeAction, "Basics Modes", "9");
	mCollapseEdgeAction->setStatusTip(tr("Enter Collapse Edge Mode"));
	mCollapseEdgeAction->setToolTip(tr("Collapse Edge Mode"));
	connect(mCollapseEdgeAction, SIGNAL(triggered()), this, SLOT(triggerCollapseEdge()));
	actionList->addAction(mCollapseEdgeAction);

	mSubdivideEdgeAction = new QAction(tr("Subdivide Edge"),this);
	mSubdivideEdgeAction->setIcon(QIcon(":/images/subdivide_edge.png"));
	mSubdivideEdgeAction->setCheckable(true);
	// sm->registerAction(mDeleteEdgeAction, "Basics Modes", "9");
	mSubdivideEdgeAction->setStatusTip(tr("Enter Subdivide Edge Mode"));
	mSubdivideEdgeAction->setToolTip(tr("Subdivide Edge Mode"));
	connect(mSubdivideEdgeAction, SIGNAL(triggered()), this, SLOT(triggerSubdivideEdge()));
	actionList->addAction(mSubdivideEdgeAction);
	
	mConnectEdgesAction = new QAction(tr("Connect Edges"),this);
	mConnectEdgesAction->setIcon(QIcon(":/images/connect_edges.png"));
	mConnectEdgesAction->setCheckable(true);
	// sm->registerAction(mDeleteEdgeAction, "Basics Modes", "9");
	mConnectEdgesAction->setStatusTip(tr("Enter Connect Edges Mode"));
	mConnectEdgesAction->setToolTip(tr("Connect Edges Mode"));
	connect(mConnectEdgesAction, SIGNAL(triggered()), this, SLOT(triggerConnectEdges()));
	actionList->addAction(mConnectEdgesAction);
	
	mSpliceCornersAction = new QAction(tr("Splice Corners"),this);
	mSpliceCornersAction->setIcon(QIcon(":/images/splice_corners.png"));
	mSpliceCornersAction->setCheckable(true);
	// sm->registerAction(mDeleteEdgeAction, "Basics Modes", "9");
	mSpliceCornersAction->setStatusTip(tr("Enter Splice Corners Mode"));
	mSpliceCornersAction->setToolTip(tr("Splice Corners Mode"));
	connect(mSpliceCornersAction, SIGNAL(triggered()), this, SLOT(triggerSpliceCorners()));
	actionList->addAction(mSpliceCornersAction);

	mTransformsAction = new QAction(tr("Transforms"),this);
	mTransformsAction->setIcon(QIcon(":/images/transforms.png"));
	mTransformsAction->setCheckable(true);
	// sm->registerAction(mDeleteEdgeAction, "Basics Modes", "9");
	mTransformsAction->setStatusTip(tr("Enter Transforms Mode"));
	mTransformsAction->setToolTip(tr("Transforms Mode"));
	connect(mTransformsAction, SIGNAL(triggered()), this, SLOT(triggerTransforms()));
	actionList->addAction(mTransformsAction);
		
}

QMenu* BasicsMode::getMenu(){
	mBasicsMenu = new QMenu(tr("Basics"));
	
	mBasicsMenu->addAction(mInsertEdgeAction);
	mBasicsMenu->addAction(mDeleteEdgeAction);	
	mBasicsMenu->addAction(mCollapseEdgeAction);	
	mBasicsMenu->addAction(mSubdivideEdgeAction);
	mBasicsMenu->addAction(mConnectEdgesAction);	
	mBasicsMenu->addAction(mSpliceCornersAction);
	mBasicsMenu->addAction(mTransformsAction);
	
	return mBasicsMenu;
}

void BasicsMode::triggerInsertEdge(){
	
	((MainWindow*)mParent)->setToolOptions(mInsertEdgeWidget);
	((MainWindow*)mParent)->setMode(MainWindow::InsertEdge);
}

void BasicsMode::triggerDeleteEdge(){
	
	((MainWindow*)mParent)->setToolOptions(mDeleteEdgeWidget);
	((MainWindow*)mParent)->setMode(MainWindow::DeleteEdge);
}

void BasicsMode::triggerCollapseEdge(){
	
	((MainWindow*)mParent)->setToolOptions(mCollapseEdgeWidget);
	((MainWindow*)mParent)->setMode(MainWindow::CollapseEdge);
}

void BasicsMode::triggerSubdivideEdge(){
	
	((MainWindow*)mParent)->setToolOptions(mSubdivideEdgeWidget);
	((MainWindow*)mParent)->setMode(MainWindow::SubdivideEdge);
}

void BasicsMode::triggerConnectEdges(){
	
	((MainWindow*)mParent)->setToolOptions(mConnectEdgesWidget);
	((MainWindow*)mParent)->setMode(MainWindow::ConnectEdges);
}

void BasicsMode::triggerSpliceCorners(){
	
	((MainWindow*)mParent)->setToolOptions(mSpliceCornersWidget);
	((MainWindow*)mParent)->setMode(MainWindow::SpliceCorners);
}

void BasicsMode::triggerTransforms(){
	
	((MainWindow*)mParent)->setToolOptions(mTransformsWidget);
	((MainWindow*)mParent)->setMode(MainWindow::NormalMode);
}

void BasicsMode::addActions(QActionGroup *actionGroup, QToolBar *toolBar, QStackedWidget *stackedWidget){
	
	actionGroup->addAction(mInsertEdgeAction);	
	actionGroup->addAction(mDeleteEdgeAction);	
	actionGroup->addAction(mCollapseEdgeAction);	
	actionGroup->addAction(mSubdivideEdgeAction);	
	actionGroup->addAction(mConnectEdgesAction);	
	actionGroup->addAction(mSpliceCornersAction);
	actionGroup->addAction(mTransformsAction);
		
	toolBar->addAction(mInsertEdgeAction);
	toolBar->addAction(mDeleteEdgeAction);	
	toolBar->addAction(mCollapseEdgeAction);	
	toolBar->addAction(mSubdivideEdgeAction);	
	toolBar->addAction(mConnectEdgesAction);	
	toolBar->addAction(mSpliceCornersAction);
	toolBar->addAction(mTransformsAction);
	
	stackedWidget->addWidget(mInsertEdgeWidget);
	stackedWidget->addWidget(mDeleteEdgeWidget);	
	stackedWidget->addWidget(mCollapseEdgeWidget);	
	stackedWidget->addWidget(mSubdivideEdgeWidget);	
	stackedWidget->addWidget(mConnectEdgesWidget);	
	stackedWidget->addWidget(mSpliceCornersWidget);
	stackedWidget->addWidget(mTransformsWidget);

}

QDoubleSpinBox *BasicsMode::createDoubleSpinBox(QGridLayout *layout, QLabel *label, QString s, double low, double high, double step, double value, double decimals, int row, int col){
	label = new QLabel(s);
	QDoubleSpinBox *spinbox = new QDoubleSpinBox;
	spinbox->setRange(low, high);
	spinbox->setSingleStep(step);
	spinbox->setValue(value);
	spinbox->setDecimals(decimals);
	spinbox->setMaximumSize(75,25);
	layout->addWidget(label,row,col);
  layout->addWidget(spinbox,row,col+1);

	return spinbox;
}

void BasicsMode::setupInsertEdge() {
	
	mInsertEdgeLayout = new QGridLayout;
	// mInsertEdgeLayout->setMargin(0);
	QLabel *noOptionsLabel = new QLabel(tr("No Options for this tool."));
	mInsertEdgeLayout->addWidget(noOptionsLabel,0,0);
	
	mInsertEdgeLayout->setRowStretch(4,1);
	mInsertEdgeLayout->setColumnStretch(2,1);
	mInsertEdgeWidget->setWindowTitle(tr("Insert Edge"));
	mInsertEdgeWidget->setLayout(mInsertEdgeLayout);	
}

void BasicsMode::setupDeleteEdge() {
	
	mDeleteEdgeLayout = new QGridLayout;
	// mDeleteEdgeLayout->setMargin(0);
	
	//cleanup checkbox
	QCheckBox *cleanupCheckBox = new QCheckBox(tr("Cleanup"),this);
	connect(cleanupCheckBox, SIGNAL(stateChanged(int)),
          ((MainWindow*)mParent), SLOT(toggleDeleteEdgeCleanupFlag(int)));
	mDeleteEdgeLayout->addWidget(cleanupCheckBox,0,0);	
	
	mDeleteEdgeLayout->setRowStretch(1,1);
	mDeleteEdgeLayout->setColumnStretch(2,1);
	mDeleteEdgeWidget->setWindowTitle(tr("Delete Edge Mode"));
	mDeleteEdgeWidget->setLayout(mDeleteEdgeLayout);	
}

void BasicsMode::setupCollapseEdge() {
	
	mCollapseEdgeLayout = new QGridLayout;
	// mCollapseEdgeLayout->setMargin(0);
	QLabel *noOptionsLabel = new QLabel(tr("No Options for this tool."));
	mCollapseEdgeLayout->addWidget(noOptionsLabel,0,0);
	
	mCollapseEdgeLayout->setRowStretch(4,1);
	mCollapseEdgeLayout->setColumnStretch(2,1);
	mCollapseEdgeWidget->setWindowTitle(tr("Collapse Edge"));
	mCollapseEdgeWidget->setLayout(mCollapseEdgeLayout);	
}

void BasicsMode::setupSubdivideEdge() {
	
	mSubdivideEdgeLayout = new QGridLayout;
	// mSubdivideEdgeLayout->setMargin(0);
	
	//number of subdivisions spinbox
	numSubdivsSpinBox = createDoubleSpinBox(mSubdivideEdgeLayout, numSubdivsLabel, tr("# Subdivisions"), 1, 10, 1, 1, 0, 0,0);
	connect(numSubdivsSpinBox, SIGNAL(valueChanged(double)), ((MainWindow*)mParent), SLOT(changeNumSubDivs(double)));

	// mSubdivideEdgeLayout->addStretch(1);
	mSubdivideEdgeLayout->setRowStretch(1,1);
	mSubdivideEdgeLayout->setColumnStretch(2,1);
	mSubdivideEdgeWidget->setWindowTitle(tr("Subdivide Edge Mode"));
	mSubdivideEdgeWidget->setLayout(mSubdivideEdgeLayout);	
	
}

void BasicsMode::setupConnectEdges(){
	
	mConnectEdgesLayout = new QGridLayout;
	// mConnectEdgesLayout->setMargin(0);
	QLabel *noOptionsLabel = new QLabel(tr("No Options for this tool."));
	mConnectEdgesLayout->addWidget(noOptionsLabel,0,0);
	
	mConnectEdgesLayout->setRowStretch(0,1);
	mConnectEdgesLayout->setColumnStretch(2,1);
	mConnectEdgesWidget->setWindowTitle(tr("Connect Edges"));
	mConnectEdgesWidget->setLayout(mConnectEdgesLayout);	
	
}

void BasicsMode::setupSpliceCorners(){
	
	mSpliceCornersLayout = new QGridLayout;
	// mSpliceCornersLayout->setMargin(0);
	mSpliceCornersLayout->setRowStretch(4,1);
	mSpliceCornersLayout->setColumnStretch(2,1);
	
	QLabel *noOptionsLabel = new QLabel(tr("No Options for this tool."));
	mSpliceCornersLayout->addWidget(noOptionsLabel,0,0);
	
	mSpliceCornersWidget->setWindowTitle(tr("Splice Corners"));
	mSpliceCornersWidget->setLayout(mSpliceCornersLayout);
	
}

void BasicsMode::setupTransforms(){
	
	mTransformsLayout = new QGridLayout;
	mTransformsLayout->setVerticalSpacing(1);
	mTransformsLayout->setHorizontalSpacing(1);
	// mTransformsLayout->setMargin(0);
	
	// transformLabel = new QLabel(tr("Translate:"));
	// mTransformsLayout->addWidget(transformLabel,0,0,1,2);
	
	xPosSpinBox = createDoubleSpinBox(mTransformsLayout, xPosLabel, tr("X-translate"), -100.0, 100.0, 0.5, 0.0, 1, 1,0);
	connect(xPosSpinBox, SIGNAL(valueChanged(double)), ((MainWindow*)mParent), SLOT(translatex(double)));
	
	yPosSpinBox = createDoubleSpinBox(mTransformsLayout, yPosLabel, tr("Y-translate"), -100.0, 100.0, 0.5, 0.0, 1, 2,0);
	connect(yPosSpinBox, SIGNAL(valueChanged(double)), ((MainWindow*)mParent), SLOT(translatey(double)));
	
	zPosSpinBox = createDoubleSpinBox(mTransformsLayout, zPosLabel, tr("Z-translate"), -100.0, 100.0, 0.5, 0.0, 1, 3,0);
	connect(zPosSpinBox, SIGNAL(valueChanged(double)), ((MainWindow*)mParent), SLOT(translatez(double)));
	
	// scaleLabel = new QLabel(tr("Scale:"));
	// mTransformsLayout->addWidget(scaleLabel,4,0,1,2);
	
	//x scale
	xScaleSpinBox = createDoubleSpinBox(mTransformsLayout, xScaleLabel, tr("X-scale"), 0.1, 10.0, 0.1, 1.0, 1, 5,0);
	connect(xScaleSpinBox, SIGNAL(valueChanged(double)), ((MainWindow*)mParent), SLOT(scalex(double)));

	yScaleSpinBox = createDoubleSpinBox(mTransformsLayout, yScaleLabel, tr("Y-scale"), 0.1, 10.0, 0.1, 1.0, 1, 6,0);
	connect(yScaleSpinBox, SIGNAL(valueChanged(double)), ((MainWindow*)mParent), SLOT(scaley(double)));

	zScaleSpinBox = createDoubleSpinBox(mTransformsLayout, zScaleLabel, tr("Z-scale"), 0.1, 10.0, 0.1, 1.0, 1, 7,0);
	connect(zScaleSpinBox, SIGNAL(valueChanged(double)), ((MainWindow*)mParent), SLOT(scalez(double)));
	
	QPushButton *freezeTransformsButton = new QPushButton(tr("&Freeze Transforms"));
	connect(freezeTransformsButton, SIGNAL(clicked()), this, SLOT(freezeTransforms()));
	mTransformsLayout->addWidget(freezeTransformsButton,8,0,1,2);

	mTransformsLayout->setRowStretch(9,1);
	mTransformsLayout->setColumnStretch(2,1);
	mTransformsWidget->setWindowTitle(tr("Transforms Mode"));
	mTransformsWidget->setLayout(mTransformsLayout);	
}

void BasicsMode::freezeTransforms()
{
	((MainWindow*)mParent)->freezeTransforms();
	xPosSpinBox->setValue(0.0);
	yPosSpinBox->setValue(0.0);
	zPosSpinBox->setValue(0.0);
	xScaleSpinBox->setValue(1.0);
	yScaleSpinBox->setValue(1.0);
	zScaleSpinBox->setValue(1.0);
}

void BasicsMode::retranslateUi(){
	
}
