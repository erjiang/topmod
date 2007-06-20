#include "GLWidget.hh"
#include "DLFLSelection.hh"

//-- Define static members --//

DLFLLocatorPtrArray GLWidget::sel_lptr_array;
/*DLFLVertexPtrArray GLWidget::sel_vptr_array;
DLFLEdgePtrArray GLWidget::sel_eptr_array;
DLFLFacePtrArray GLWidget::sel_fptr_array;
DLFLFaceVertexPtrArray GLWidget::sel_fvptr_array;
extern DLFLVertexPtrArray DLFLObject::sel_vptr_array; // List of selected DLFLVertex pointers
extern DLFLEdgePtrArray DLFLObject::sel_eptr_array; // List of selected DLFLEdge pointers
extern DLFLFacePtrArray DLFLObject::sel_fptr_array; // List of selected DLFLFace pointers
extern DLFLFaceVertexPtrArray DLFLObject::sel_fvptr_array; // List of selected DLFLFaceVertex pointers
*/
// QGLContext *cx;
// QGLFormat f;
// f.setStereo( TRUE );
// QGLFormat::setDefaultFormat( f );
// cx->setFormat( f );
// if ( !cx->create() )
// // printf("FALSE \n");
// QMessageBox::about(this, tr("false"),tr("false"));
// if ( !cx->format().stereo() )
// 
// // 			QMessageBox::about(this, tr("false"),tr("false"));
// QGLFormat f;
// f.setStereo(true);
// QGLContext *cx = new QGLContext(f);
// // cx->setFormat(f);
// if (!cx->create())
// exit(0); // no OpenGL support, or cannot render on the specified paintdevice
// if (!cx->format().stereo())
// exit(0); // could not create stereo context

GLWidget::GLWidget(int w, int h, VPView v, DLFLRendererPtr rp, QColor color, QColor vcolor, DLFLObjectPtr op, TMPatchObjectPtr pop, const QGLFormat & format, QWidget * parent ) 
 : 	QGLWidget(format, parent, NULL), /*viewport(w,h,v),*/ object(op), patchObject(pop), renderer(rp), 
		mRenderColor(color), mViewportColor(vcolor),/*grid(ZX,20.0,10),*/ showgrid(false), showaxes(false), mUseGPU(true) { 

	// Vector3d neweye = eye - center;
	// double eyedist = norm(neweye);
	// Matrix4x4 lmat = Transformation::lookat(neweye,Vector3d(0,0,0),up);
	// transform = lmat;
	// transform.translate(-center);
	// camera.setEye(Vector3d(0,0,eyedist));
	
	
	//take cross product of lookat and up vector - and you're given one that looks right...
	//then cross that with the lookat and you get the real up
	Vector3d Aim(0,0,0);
	Vector3d Pos(10,10,10);
	Vector3d Up(-0.40824829046,0.81649658093,-0.40824829046);
	
	// pos X up
	// 	A  X B
	// double ax = -10, ay = -10, az = -10;
	// double bx = 0, by = 1, bz = 0;
	// 
	// double cx = ay*bz - az*by;
	// double cy = az*bx - ax*bz;
	// double cz = ax*by - ay*bx;
	// 
	// double dx = cy*az - cz*ay;
	// double dy = cz*ax - cx*az;
	// double dz = cx*ay - cy*ax;
	// 
	// double sqr = sqrt(dx*dx + dy*dy + dz*dz); 
	
	
	// std::cout << dx/sqr << " " << dy/sqr << " " << dz/sqr << "\n\n\n";
	// Vector3d vv(dx,dy,dz);
	// normalize(vv);
	// std::cout << vv << "\n";
	// 
	// Vector3d right = (-(normalized(Pos))) % Up;
	// // Up = Vector3d(-0.408248,0.816497,-0.408248);//right % (-Pos);
	// Up = normalized(right) % (-(normalized(Pos)));
	// normalize(Up);
	// std::cout << Up << std::endl;
	// mCamera = new Camera2( eye, lookat, Vector3d(-0.408248,0.816497,-0.408248));
	mCamera = new Camera2( Pos, Aim, Up);
  // ArbitraryRotate(Vector3d(1,0,0),Vector3d(0,1,0),Vector3d(0,0,1),45.0f,45.0f,mCamera->Pos,mCamera->Aim);

	// Vector3d WindowX,WindowY,WindowZ;
	// WindowX.set(1, 0, 0);
	// WindowY.set(0, 1, 0);
	// 
	// RotateX(&WindowX, 45);
	// RotateY(&WindowX, 45);
	// WindowX[2] = -WindowX[2];
	// 
	// RotateX(&WindowY, 45);
	// RotateY(&WindowY, 45);
	// WindowY[2] = -WindowY[2];
	// 
	// WindowZ = WindowX % WindowY;
	// normalize(WindowZ);
	// 
	// ArbitraryRotate(WindowX, WindowY, WindowZ, 0, 0, Pos, Aim);
	// ArbitraryRotate(Vector3d(1, 0, 0), Vector3d(0, 1, 0), Vector3d(0, 0, 1), 0, 0, Pos, Aim);

	// Up = WindowY;
	// normalize(Up);

	
	// if( patchObject ) { delete patchObject; patchObject = 0; }
}

GLWidget::~GLWidget(){ 
#ifdef GPU_OK
	cgDestroyContext( cg.context );
#endif
}

void GLWidget::redraw() {
	repaint();
}

void GLWidget::initializeGL( ) {
		
	setAutoBufferSwap( true );
	glClearColor(mViewportColor.redF(),mViewportColor.greenF(),mViewportColor.blueF(),mViewportColor.alphaF());
	mCamera->PerspectiveDisplay(width(),height());
	// viewport.resize(this->size().width(),this->size().height());
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);									// Set Line Antialiasing
	glEnable(GL_BLEND);																			// Enable Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);			// Type Of Blending To Use
	
	//initialize light position and color
	plight.position.set(50,25,0);
	// mLightPosition.set(50,25,0);
	
	plight.warmcolor.set(1,1,0.6);
	// mLightColor = QColor(255,255,153);
	
	plight.coolcolor.set(0.2,0.2,0.4);
	
	plight.intensity = 2.0;
	// mLightIntensity = 2.0;
	
	mGlobalAmbient = QColor(25,25,25);
	
	//enable gl lighting for use with cg functions
	enableGLLights();

	mShowVertexIDs = false;
	mShowEdgeIDs   = false;
	mShowFaceIDs   = false;
	mShowFaceVertexIDs = false;
	mShowSelectedIDs = false;
	mShowHUD = false;
	mBrushSize = 2.5;
	mShowBrush = false;
	setMouseTracking(true);
	
	locatorPtr = new DLFLLocator(); // brianb
	
	#ifdef GPU_OK
	if (mUseGPU)
		initCg();
	#endif
	
}

void GLWidget::enableGLLights(){
	// ENABLE THE LIGHTING
  glPushMatrix(); {
    glMatrixMode( GL_MODELVIEW);
    glLoadIdentity();
    GLfloat lmodel_ambient[] = {0.1,0.1,0.2,1.0};
    GLfloat lmodel_diffuse[] = {0.8,0.7,0.5,1.0};
    GLfloat lmodel_specular[] = {0.5,0.5,0.5,1.0};
    GLfloat spotDir[]=	{0.0,-1.0,0.0};
		GLfloat light_position[] = { 2.0, 20.0, 5.0, 1.0 }; //{ -1.0, 30.0, 5.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lmodel_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lmodel_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lmodel_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    //GLfloat lmodel_diffuse2[] = {0.4,0.4,0.4,1.0};
    //glLightfv(GL_LIGHT1, GL_DIFFUSE, lmodel_diffuse);
    //glLightfv(GL_LIGHT1, GL_POSITION, light_position2);  
  } glPopMatrix();
  glEnable(GL_LIGHT0);
  //glEnable(GL_LIGHT1);
 
}

void GLWidget::paintEvent(QPaintEvent *event){
	
	#ifdef GPU_OK
	if (mUseGPU){
	  cgGLBindProgram( cg.vertProgram );
	  // cgGLBindProgram( cg.fragProgram );
	  cgGLEnableProfile( cg.vertProfile );
	  // cgGLEnableProfile( cg.fragProfile );
	
		//set cg parameters here
		
		//global ambient light
		cgGLSetParameter3f(cg.globalAmbient, mGlobalAmbient.redF(), mGlobalAmbient.greenF(), mGlobalAmbient.blueF() );
		//send lighing info
		cgGLSetParameter3f(cg.lightColor, plight.warmcolor.r, plight.warmcolor.g, plight.warmcolor.b );
		cgGLSetParameter3f(cg.lightPosition, plight.position[0], plight.position[1], plight.position[2] );
		
		//test material
		const float emissive[3] = {0.1,  0.1,  0.1},
	              ambient[3]  = {0.33, 0.22, 0.03},
	              diffuse[3]  = {0.78, 0.57, 0.11},
	              specular[3] = {0.99, 0.91, 0.81},
	              shininess = 27.8;

	  cgSetParameter3fv(cg.Ke, emissive);
	  cgSetParameter3fv(cg.Ka, ambient);
	  cgSetParameter3fv(cg.Kd, diffuse);
	  cgSetParameter3fv(cg.Ks, specular);
	  cgSetParameter1f(cg.shininess,  shininess);
	  
	  // lookThruLight( );
	  // glViewport(0,0,texRenderSize,texRenderSize);
	  //     glMatrixMode(GL_PROJECTION);
	  //     glLoadIdentity();
	  //     // gluPerspective( spotAtten*2, 1, NEAR, FAR );
	  //     //glOrtho( -50, 50, -50, 50, NEAR, FAR );
	  //     glMatrixMode( GL_MODELVIEW);
	  //     glLoadIdentity();
	  //     gluLookAt(light_position[0],light_position[1],light_position[2],0.0f,0.0f,0.0f,0,0,1);
    //glTranslatef(-light_position[0],-light_position[2],-light_position[1]);
    //glRotatef(90,1,0,0);

  	//set light matrix
		//this is probably wrong dammit
	  cgGLSetStateMatrixParameter( 	cg.worldToLight, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY); 

		
	
	}
	#endif // GPU_OK

	//clear to a white background
	//   glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	// glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//   glLoadIdentity();
	//   glMatrixMode(GL_MODELVIEW);
	//   glViewport(0,0, width(), height() );	
	//   glLoadIdentity();
	// //transform the camera
	//   mCamera->PerspectiveDisplay(width(),height());

	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// viewport.resize(width(),height());
	// mCamera->PerspectiveDisplay(width(),height());

	if( renderer ){
		renderer->initialize();	
	}
	
	glClearColor(mViewportColor.redF(),mViewportColor.greenF(),mViewportColor.blueF(),mViewportColor.alphaF());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// setupViewport(width(), height());
	glEnable(GL_DEPTH_TEST);
	// Multiply by the transformation matrix. dave and stuart*
	// Matrix4x4 m4 = inverse(viewport.apply_transform().matrix());
	// viewport.apply_transform();
	mCamera->PerspectiveDisplay(width(),height());
	
	#ifdef GPU_OK
	if (mUseGPU){
		//send camera parameters
		cgGLSetParameter3f(cg.eyePosition, mCamera->getPos()[0], mCamera->getPos()[1], mCamera->getPos()[2] );
	  cgGLSetStateMatrixParameter( cg.camToWorld,	CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE );
	  cgGLSetStateMatrixParameter( cg.camToWorldIT,	CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_TRANSPOSE );		
	}
	#endif
	// Draw the axes if required. Use thick lines
	if ( showaxes ) {
		glLineWidth(3.0);
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(2,0xAAAA);
		glBegin(GL_LINES);
		glColor3f(0.5,0.25,0.25);                     // X Axis - Red
		glVertex3f(0,0,0); glVertex3f(-10,0,0);
		glColor3f(0.25,0.5,0.25);                     // Z Axis - Green
		glVertex3f(0,0,0); glVertex3f(0,0,-10);
		glColor3f(0.25,0.25,0.5);                     // Y Axis - Blue
		glVertex3f(0,0,0); glVertex3f(0,-10,0);
		glEnd();
		glDisable(GL_LINE_STIPPLE);
		glBegin(GL_LINES);
		glColor3f(0.6,0.25,0.25);                     // X Axis - Red
		glVertex3f(0,0,0); glVertex3f(10,0,0);
		glColor3f(0.25,0.6,0.25);                     // Z Axis - Green
		glVertex3f(0,0,0); glVertex3f(0,0,10);
		glColor3f(0.25,0.25,0.6);                     // Y Axis - Blue
		glVertex3f(0,0,0); glVertex3f(0,10,0);
		glEnd();
		glLineWidth(3.0);
	}	
	glDepthRange(0,1);
	
	if ( renderer ) {
		renderer->setCgData(cg);
	  renderer->render(object);
	  if(patchObject)
	    renderer->render(patchObject);
	}
	// Show any selected items.
	// Adjust the depthrange so selected items are shown clearly
	glDepthRange(0,1-0.0005-0.0005);
	drawSelected();
	glDepthRange(0,1);
	
	GLdouble model[4][4], proj[4][4];
	GLint view[4];
	
	#ifndef Q_WS_QWS
	glGetDoublev(GL_MODELVIEW_MATRIX, &model[0][0]);
	glGetDoublev(GL_PROJECTION_MATRIX, &proj[0][0]);
	glGetIntegerv(GL_VIEWPORT, &view[0]);
	#endif
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f,(GLfloat)width()/(GLfloat)height(),0.1f,100.0f);
	
	glPopAttrib();
  glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// glTranslated(0.0, 0.0, -50.0);	
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
	
		
	#ifdef WITH_VERSE
	if (VerseConnected & T_VERSE_CONNECTED){
		// painter.drawText(rectangle, Qt::AlignCenter,tr("verse connected"));
		// t_verse_update();
	}
	#endif
	
	drawBrush(&painter);
	drawHUD(&painter);
	drawSelectedIDs(&painter, &model[0][0], &proj[0][0], &view[0]);
	drawIDs(&painter, &model[0][0], &proj[0][0], &view[0]); // draw vertex, edge and face ids
	  
	painter.end();
	
	#ifdef GPU_OK
	if (mUseGPU){
	  cgGLDisableProfile( cg.vertProfile );
	  // cgGLDisableProfile( cg.fragProfile );
	}
	#endif // GPU_OK
}

void GLWidget::resizeGL( int width, int height ){
	// setupViewport(width,height);
	// drawText(this->size().width(),this->size().height());	
	if (height==0)	height=1; 							//Making Height Equal One
	glViewport( 0, 0, (GLint)width, (GLint)height );
	
	// viewport.resize(this->size().width(),this->size().height());
}

void GLWidget::setupViewport(int width, int height){	
	// mScene->resize(width,height);
  // setup viewport, projection etc.:

	// glViewport(0, 0, width, height);
	// // glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	// // glLoadIdentity();												// Reset The Projection Matrix
	// // // Calculate The Aspect Ratio Of The Window
	// // gluPerspective(60.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	// glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
}

void GLWidget::drawText(int width, int height ){	
}

void GLWidget::drawBrush(QPainter *painter){
	if (mShowBrush){
		QBrush brush = QBrush(QColor(255,0,0,127));
		QPen pen = QPen(brush,4.0);
		painter->setPen(pen);
		painter->setBrush(Qt::NoBrush);
		//get the mouse position from global coordinate system
		int x = mapFromGlobal(QCursor::pos()).x();
		int y = mapFromGlobal(QCursor::pos()).y();
		painter->drawEllipse(x - mBrushSize/2, y - mBrushSize/2, mBrushSize, mBrushSize);
		painter->drawLine(x-10,y,x+10,y);
		painter->drawLine(x,y-10,x,y+10);
	}
}

//the Heads up display should show the current object's 
//number of vertices, faces, edges, the genus
//number of selected vertices, edges, faces, face vertices, and eventually objects
//other data that could be shown will be added later
void GLWidget::drawHUD(QPainter *painter){	
	if (mShowHUD){
		QString s = "Vertices: " + QString("%1").arg((uint)object->num_vertices()) +
		 						"\nEdges: " + QString("%1").arg((uint)object->num_edges()) +
								"\nFaces: " + QString("%1").arg((uint)object->num_faces()) +
								"\nGenus: " + QString("%1").arg(object->genus());

		QString s2 = "Sel. Vertices:" + QString("%1").arg(numSelectedVertices()) +
		 						"\nSel. Edges: " + QString("%1").arg(numSelectedEdges()) +
								"\nSel. Faces: " + QString("%1").arg(numSelectedFaces()) +
								"\nSel. Face-Verts: " + QString("%1").arg(numSelectedFaceVertices());
								
		QFont font("verdana", 14);
		QFontMetrics fm(font);
		painter->setFont(font);
		QRect r(fm.boundingRect(s));
		painter->setPen(Qt::NoPen);
		QBrush brush = QBrush(QColor(0,0,0,127));
		painter->setBrush(brush);
		QRectF rectangle(0.0, height()-r.height()*6, width(), height());
		// painter->drawRoundRect(QRect(3.0,3.0,rectangle.width(),rectangle.height()),25,25);
		painter->drawRect(rectangle);
		painter->setPen(Qt::white);
		painter->drawText(width()/5*3.5, rectangle.top()+5,150, rectangle.height(),Qt::AlignRight, s);
		painter->drawText(width()/5*2.5, rectangle.top()+5,150,rectangle.height(),Qt::AlignRight, s2);
	}
}

void GLWidget::drawIDs( QPainter *painter, const GLdouble *model, const GLdouble *proj, const GLint	*view) {
	glDisable(GL_DEPTH_TEST);
	GLdouble win_x = 0, win_y = 0, win_z = 0;	
	double min_dist=100000000.0, max_dist=-100000000.0, x,y,z,d;
	int count, min_alpha = 25, max_alpha = 255;
	// Vector3d coord = viewport.camera.getPos(); //use this to find the dist from the camera
	Vector3d coord = mCamera->getPos(); //use this to find the dist from the camera
	QBrush brush;
	
	/* Draw Vertex IDs */
	if( mShowVertexIDs ) {
		DLFLVertexPtrArray vparray;
		object->getVertices(vparray);
		double vlsqr_array[vparray.size()];
		count = 0;
		DLFLVertexPtrArray::iterator it;
		for( it = vparray.begin(); it != vparray.end(); it++) {
			Vector3d dist = (*it)->coords - coord;
			vlsqr_array[count] = dist.lengthsqr();
			max_dist = (vlsqr_array[count] > max_dist ) ? vlsqr_array[count] : max_dist;
			min_dist = (vlsqr_array[count] < min_dist ) ? vlsqr_array[count] : min_dist;
			count++;
		}
		count =0;
		for( it = vparray.begin(); it != vparray.end(); it++) {
			Vector3d point = (*it)->coords;
			point.get(x,y,z);		
			QString id = QString::number((*it)->getID() );
			d = (vlsqr_array[count]-min_dist)*(max_alpha-min_alpha)/(max_dist - min_dist) + min_alpha;
			gluProject(x, y, z, model, proj, view, &win_x, &win_y, &win_z);
			win_y = height() - win_y; // y is inverted
			painter->setPen(Qt::NoPen);
			brush = QBrush(QColor(mVertexIDBgColor.red(),mVertexIDBgColor.green(),mVertexIDBgColor.blue(),min_alpha+max_alpha-d));
			painter->setBrush(brush);
			QRectF rectangle(win_x, win_y, 35, 20);
			painter->drawRoundRect(rectangle,6,6);
			painter->setPen(QColor(255,255,255,min_alpha+max_alpha-d));
			painter->drawText(rectangle, Qt::AlignCenter,id);
			count++;
		}
	}

	/* Draw Edge IDs */
	if( mShowEdgeIDs ) {
		DLFLEdgePtrArray eparray;
		object->getEdges(eparray);
		double elsqr_array[eparray.size()];
		count = 0;
		DLFLEdgePtrArray::iterator it;
		for( it = eparray.begin(); it != eparray.end(); it++) {
			Vector3d dist = (*it)->getMidPoint() - coord;
			elsqr_array[count] = dist.lengthsqr();
			max_dist = (elsqr_array[count] > max_dist ) ? elsqr_array[count] : max_dist;
			min_dist = (elsqr_array[count] < min_dist ) ? elsqr_array[count] : min_dist;
			count++;
		}
		count=0;
		for( it = eparray.begin(); it != eparray.end(); it++) {
			QString id = QString::number( (*it)->getID() );
			(*it)->getMidPoint().get(x,y,z);
			d = (elsqr_array[count]-min_dist)*(max_alpha-min_alpha)/(max_dist - min_dist) + min_alpha;
			gluProject(x, y, z, model, proj, view, &win_x, &win_y, &win_z);
			win_y = height() - win_y; // y is inverted
			painter->setPen(Qt::NoPen);
			// std::cout<< min_alpha << " " << max_alpha << " "<< d << std::endl;
			brush = QBrush(QColor(mEdgeIDBgColor.red(),mEdgeIDBgColor.green(),mEdgeIDBgColor.blue(),min_alpha+max_alpha-d));
			painter->setBrush(brush);
			QRectF rectangle(win_x, win_y, 35, 20);
			painter->drawRoundRect(rectangle,6,6);
			painter->setPen(QColor(255,255,255,min_alpha+max_alpha-d));
			painter->drawText(rectangle, Qt::AlignCenter,id);
			count++;
		}
	}
	
	/* Draw Face IDs */
	if( mShowFaceIDs ) {
		DLFLFacePtrArray fparray;
		object->getFaces(fparray);
		double flsqr_array[fparray.size()];
		count = 0;
		DLFLFacePtrArray::iterator it;
		for( it = fparray.begin(); it != fparray.end(); it++) {
			Vector3d dist = (*it)->geomCentroid() - coord;
			flsqr_array[count] = dist.lengthsqr();
			max_dist = (flsqr_array[count] > max_dist ) ? flsqr_array[count] : max_dist;
			min_dist = (flsqr_array[count] < min_dist ) ? flsqr_array[count] : min_dist;
			count++;
		}
		count=0;
		for( it = fparray.begin(); it != fparray.end(); it++) {
			QString id = QString::number( (*it)->getID() );
			Vector3d point = (*it)->geomCentroid();// + (*it)->getNormal();
			point.get(x,y,z);
			d = (flsqr_array[count]-min_dist)*(max_alpha-min_alpha)/(max_dist - min_dist) + min_alpha;
			gluProject(x, y, z, model, proj, view, &win_x, &win_y, &win_z);
			win_y = height() - win_y; // y is inverted
			painter->setPen(Qt::NoPen);
			brush = QBrush(QColor(mFaceIDBgColor.red(),mFaceIDBgColor.green(),mFaceIDBgColor.blue(),min_alpha+max_alpha-d));
			painter->setBrush(brush);
			QRectF rectangle(win_x, win_y, 35, 20);
			painter->drawRoundRect(rectangle,6,6);
			painter->setPen(QColor(255,255,255,min_alpha+max_alpha-d));
			painter->drawText(rectangle, Qt::AlignCenter,id);
			count++;
		}
	}

	glEnable(GL_DEPTH_TEST);
}

void GLWidget::drawSelectedIDs( QPainter *painter, const GLdouble *model, const GLdouble *proj, const GLint	*view) {
	if (mShowSelectedIDs){
		glDisable(GL_DEPTH_TEST);
	
		double x,y,z;
		QBrush brush;
		QRectF rectangle;
		QString id;
		GLdouble win_x = 0, win_y = 0, win_z = 0;
	
		if( !mShowVertexIDs ) {
			if ( !object->sel_vptr_array.empty() ){
				DLFLVertexPtrArray::iterator first, last;
				first = object->sel_vptr_array.begin(); last = object->sel_vptr_array.end();
				while ( first != last ){
					QString id = QString::number( (*first)->getID() );
					double x,y,z;
					Vector3d point = (*first)->coords;// + (*it)->getNormal();
					point.get(x,y,z);
					win_x = 0, win_y = 0, win_z = 0;
					gluProject(x, y, z, model, proj, view, &win_x, &win_y, &win_z);
					win_y = height() - win_y; // y is inverted
					painter->setPen(Qt::NoPen);
					brush = QBrush(QColor(mVertexIDBgColor));
					painter->setBrush(brush);
					rectangle = QRectF(win_x, win_y, 40, 20);
					painter->drawRoundRect(rectangle,6,6);
					painter->setPen(Qt::white);
					painter->drawText(rectangle, Qt::AlignCenter,id);
					++first;
				}
			}
		}
	
		if( !mShowEdgeIDs ) {
			if ( !object->sel_eptr_array.empty() ){
				DLFLEdgePtrArray::iterator first, last;
				first = object->sel_eptr_array.begin(); last = object->sel_eptr_array.end();
				while ( first != last ){
					QString id = QString::number( (*first)->getID() );
					(*first)->getMidPoint().get(x,y,z);
					win_x = 0, win_y = 0, win_z = 0;
					gluProject(x, y, z, model, proj, view, &win_x, &win_y, &win_z);
					win_y = height() - win_y; // y is inverted
					painter->setPen(Qt::NoPen);
					QBrush brush = QBrush(QColor(mEdgeIDBgColor));
					painter->setBrush(brush);
					rectangle = QRectF(win_x, win_y, 40, 20);
					painter->drawRoundRect(rectangle,6,6);
					painter->setPen(Qt::white);
					painter->drawText(rectangle, Qt::AlignCenter,id);
					++first;
				}
			}
		}

		if( !mShowFaceIDs ) {
			if ( !object->sel_fptr_array.empty() ){
				DLFLFacePtrArray::iterator first, last;
				first = object->sel_fptr_array.begin(); last = object->sel_fptr_array.end();
				while ( first != last )	{
					QString id = QString::number( (*first)->getID() );
					double x,y,z; 
					Vector3d point = (*first)->geomCentroid();// + (*it)->getNormal();
					point.get(x,y,z);
					win_x = 0, win_y = 0, win_z = 0;
					gluProject(x, y, z, model, proj, view, &win_x, &win_y, &win_z);
					win_y = height() - win_y; // y is inverted
					painter->setPen(Qt::NoPen);
					brush = QBrush(QColor(mFaceIDBgColor));
					painter->setBrush(brush);
					rectangle = QRectF(win_x, win_y, 40, 20);
					painter->drawRoundRect(rectangle,6,6);
					painter->setPen(Qt::white);
					painter->drawText(rectangle, Qt::AlignCenter,id);
					++first;
				}
			}
		}

		if( !mShowFaceVertexIDs ) {
			if ( !object->sel_fvptr_array.empty() ) {
				DLFLFaceVertexPtrArray::iterator first, last;
				first = object->sel_fvptr_array.begin(); last = object->sel_fvptr_array.end();
				while ( first != last ){
					QString id = QString::number( (*first)->vertex->getID() );
					double x,y,z; 
					Vector3d point = (*first)->vertex->coords;
					point.get(x,y,z);
					win_x = 0, win_y = 0, win_z = 0;
					gluProject(x, y, z, model, proj, view, &win_x, &win_y, &win_z);
					win_y = height() - win_y; // y is inverted
					painter->setPen(Qt::NoPen);
					brush = QBrush(QColor(mVertexIDBgColor));
					painter->setBrush(brush);
					rectangle = QRectF(win_x, win_y, 40, 20);
					painter->drawRoundRect(rectangle,6,6);
					painter->setPen(Qt::white);
					painter->drawText(rectangle, Qt::AlignCenter,id);
					++first;
				}
			}
		}
	
		glEnable(GL_DEPTH_TEST);
	}
}

void GLWidget::toggleFullScreen( ) {
	if( !mIsFullScreen ) {    
		move(0,0);
		resize( QApplication::desktop()->size() );
		resizeGL( QApplication::desktop()->size().width(), QApplication::desktop()->size().height() );
		//setFocusPolicy( Qt::StrongFocus );
		//setWindowFlags( Qt::FramelessWindowHint );
		show();
	} else {
		//setWindowFlags( Qt::Widget );
		resize( 700, 800 );
		show();
	}
	mIsFullScreen != mIsFullScreen;
}

// Subroutine for selecting a Vertex
DLFLVertexPtr GLWidget::selectVertex(int mx, int my) {
	GLuint selectBuf[1024];
	uint closest;
	GLuint dist;
	long hits, index;
	DLFLVertexPtr sel(NULL);

	glSelectBuffer(1024,selectBuf);
	glRenderMode(GL_SELECT);

	glInitNames(); glPushName(0);

	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	// viewport.camera.enterSelectionMode(mx,my,30,30,vp); // Reduced sensitivity for picking points
	mCamera->enterSelectionMode(mx,my,30,30,vp); // Reduced sensitivity for picking points

	// Make sure earlier matrices are preserved, since multiple windows
	// seem to be sharing the same matrix stacks
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// viewport.reshape();
	// viewport.apply_transform();
	// glViewport(0,0,width(),height());
	mCamera->PerspectiveDisplay(width(),height());
	// viewport.apply_transform();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderVerticesForSelect(object);
	glFlush();

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	mCamera->leaveSelectionMode();

	hits = glRenderMode(GL_RENDER);
	if ( hits > 0 ){
		closest = 0; dist = 0xffffffff;
		while ( hits ){
			index = (hits-1)*4;
			if ( selectBuf[index+1] < dist ){
				dist = selectBuf[index+1];
				closest = selectBuf[index+3];
			}
			hits--;
		}
		// closest now has the id of the selected vertex
		sel = DLFLObject::vparray[closest];
	}
	return sel;
}

// Subroutine for selecting a Vertex
DLFLLocatorPtr GLWidget::selectLocator(int mx, int my) // brianb
{
  GLuint selectBuf[1024];
  uint closest;
  GLuint dist;
  long hits, index;

  glSelectBuffer(1024,selectBuf);
  glRenderMode(GL_SELECT);

  glInitNames(); glPushName(0);

  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  // viewport.camera.enterSelectionMode(mx,my,10,10,vp); // Reduced sensitivity for picking points
  mCamera->enterSelectionMode(mx,my,10,10,vp); // Reduced sensitivity for picking points

  // Make sure earlier matrices are preserved, since multiple windows
  // seem to be sharing the same matrix stacks

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  // viewport.reshape();
  // viewport.apply_transform();
	mCamera->PerspectiveDisplay(width(),height());
  // viewport.apply_transform();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  renderLocatorsForSelect();
  glFlush();

  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  mCamera->leaveSelectionMode();
  hits = glRenderMode(GL_RENDER);
  if ( hits > 0 )
  {
    closest = 0; dist = 0xffffffff;
    while ( hits )
    {
      index = (hits-1)*4;
      if ( selectBuf[index+1] < dist )
      {
        dist = selectBuf[index+1];
        closest = selectBuf[index+3];
      }
      hits--;
    }
    
    // closest now has the hit axis
    GLWidget::locatorPtr->setSelectedAxis(closest);

    // Only one locator for now, return pointer to DLFLViewport::locator
    return GLWidget::locatorPtr;
  }
  
  return NULL;
}  // brianb

// Subroutine for selecting an Edge
DLFLEdgePtr GLWidget::selectEdge(int mx, int my) {
	GLuint selectBuf[1024];
	uint closest;
	GLuint dist;
	long hits, index;
	DLFLEdgePtr sel(NULL);

	glSelectBuffer(1024,selectBuf);
	glRenderMode(GL_SELECT);

	glInitNames(); glPushName(0);

	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	mCamera->enterSelectionMode(mx,my,10,10,vp);

	// Make sure earlier matrices are preserved, since multiple windows
	// seem to be sharing the same matrix stacks
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// viewport.reshape();
	// viewport.apply_transform();
	mCamera->PerspectiveDisplay(width(),height());
	// viewport.apply_transform();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderEdgesForSelect(object);
	glFlush();

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	mCamera->leaveSelectionMode();

	hits = glRenderMode(GL_RENDER);
	if ( hits > 0 ){
		closest = 0; dist = 0xffffffff;
		while ( hits ){
			index = (hits-1)*4;
			if ( selectBuf[index+1] < dist ){
				dist = selectBuf[index+1];
				closest = selectBuf[index+3];
			}
			hits--;
		}
		// closest now has the id of the selected edge
		sel = DLFLObject::eparray[closest];
	}
	return sel;
}

// Subroutine for selecting a Face
DLFLFacePtr GLWidget::selectFace(int mx, int my) {
	GLuint selectBuf[1024];
	uint closest;
	GLuint dist;
	long hits, index;
	DLFLFacePtr sel(NULL);

	glSelectBuffer(1024,selectBuf);
	glRenderMode(GL_SELECT);

	glInitNames(); glPushName(0);

	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	mCamera->enterSelectionMode(mx,my,2.5,2.5,vp);

	// Make sure earlier matrices are preserved, since multiple windows
	// seem to be sharing the same matrix stacks
	// glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// viewport.reshape();
	// viewport.apply_transform();
	mCamera->PerspectiveDisplay(width(),height());
	// viewport.apply_transform();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderFacesForSelect(object);
	glFlush();

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	mCamera->leaveSelectionMode();

	hits = glRenderMode(GL_RENDER);
	if ( hits > 0 ){
		closest = 0; dist = 0xffffffff;
		while ( hits ){
			index = (hits-1)*4;
			if ( selectBuf[index+1] < dist ){
				dist = selectBuf[index+1];
				closest = selectBuf[index+3];
			}
			hits--;
		}
		// closest now has the id of the selected face
		sel = DLFLObject::fparray[closest];
	}	
	return sel;
}

// Subroutine for selecting multiple faces at once
DLFLFacePtrArray GLWidget::selectFaces(int mx, int my) {
	glEnable(GL_CULL_FACE);
	GLuint selectBuf[1024];
	uint closest;
	GLuint dist;
	long hits, index;
	DLFLFacePtr sel(NULL);
	DLFLFacePtrArray fparray;

	glSelectBuffer(1024,selectBuf);
	glRenderMode(GL_SELECT);

	glInitNames(); glPushName(0);

	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	mCamera->enterSelectionMode(mx,my,mBrushSize,mBrushSize,vp);

	// Make sure earlier matrices are preserved, since multiple windows
	// seem to be sharing the same matrix stacks
	// glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// viewport.reshape();
	// viewport.apply_transform();
	mCamera->PerspectiveDisplay(width(),height());
	// viewport.apply_transform();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderFacesForSelect(object);
	glFlush();

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	mCamera->leaveSelectionMode();

	hits = glRenderMode(GL_RENDER);
	if ( hits > 0 ){
		closest = 0; dist = 0xffffffff;
		while ( hits ){
			index = (hits-1)*4;
			// if ( selectBuf[index+1] < dist ){
				// dist = selectBuf[index+1];
				// closest = selectBuf[index+3];
			// }
			
			//if (DLFLObject::fparray[selectBuf[index+3]].normalCentroid()*camera.getPos()){
			if(!isSelected(DLFLObject::fparray[selectBuf[index+3]]))
				fparray.push_back(DLFLObject::fparray[selectBuf[index+3]]);
			//}
			hits--;
		}
		// closest now has the id of the selected face
		//sel = DLFLObject::fparray[closest];
	}	
	glDisable(GL_CULL_FACE);
	return fparray;
}

// Subroutine for selecting multiple faces at once
DLFLFacePtrArray GLWidget::deselectFaces(int mx, int my) {
	glEnable(GL_CULL_FACE);
	GLuint selectBuf[1024];
	uint closest;
	GLuint dist;
	long hits, index;
	DLFLFacePtr sel(NULL);
	DLFLFacePtrArray fparray;

	glSelectBuffer(1024,selectBuf);
	glRenderMode(GL_SELECT);

	glInitNames(); glPushName(0);

	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	mCamera->enterSelectionMode(mx,my,mBrushSize,mBrushSize,vp);

	// Make sure earlier matrices are preserved, since multiple windows
	// seem to be sharing the same matrix stacks
	// glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// viewport.reshape();
	// viewport.apply_transform();
	mCamera->PerspectiveDisplay(width(),height());
	// viewport.apply_transform();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderFacesForSelect(object);
	glFlush();

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	mCamera->leaveSelectionMode();

	hits = glRenderMode(GL_RENDER);
	if ( hits > 0 ){
		closest = 0; dist = 0xffffffff;
		while ( hits ){
			index = (hits-1)*4;
			// if ( selectBuf[index+1] < dist ){
				// dist = selectBuf[index+1];
				// closest = selectBuf[index+3];
			// }
			
			//if (DLFLObject::fparray[selectBuf[index+3]].normalCentroid()*camera.getPos()){
			if(isSelected(DLFLObject::fparray[selectBuf[index+3]]))
				fparray.push_back(DLFLObject::fparray[selectBuf[index+3]]);
			//}
			hits--;
		}
		// closest now has the id of the selected face
		//sel = DLFLObject::fparray[closest];
	}	
	glDisable(GL_CULL_FACE);
	return fparray;
}

// Subroutine for selecting a FaceVertex (Corner) within a Face
DLFLFaceVertexPtr GLWidget::selectFaceVertex(DLFLFacePtr fp, int mx, int my) {
	GLuint selectBuf[1024];
	uint closest;
	GLuint dist;
	long hits, index;
	DLFLFaceVertexPtr sel(NULL);

	glSelectBuffer(1024,selectBuf);
	glRenderMode(GL_SELECT);

	glInitNames(); glPushName(0);

	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	mCamera->enterSelectionMode(mx,my,40,40,vp);
	// Make sure earlier matrices are preserved, since multiple windows
	// seem to be sharing the same matrix stacks
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// viewport.reshape();
	// viewport.apply_transform();
	mCamera->PerspectiveDisplay(width(),height());
	// viewport.apply_transform();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderFaceVerticesForSelect(fp);
	glFlush();

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	mCamera->leaveSelectionMode();

	hits = glRenderMode(GL_RENDER);
	if ( hits > 0 ){
		closest = 0; dist = 0xffffffff;
		while ( hits ){
			index = (hits-1)*4;
			if ( selectBuf[index+1] < dist ){
				dist = selectBuf[index+1];
				closest = selectBuf[index+3];
			}
			hits--;
		}
		// closest now has the id of the selected face vertex
		sel = DLFLObject::fvparray[closest];
	}
	return sel;
}

// Draw the selected items
void GLWidget::drawSelected(void) {
  if ( !sel_lptr_array.empty() ) {
    sel_lptr_array[0]->render();
  }

  if ( !object->sel_vptr_array.empty() ) {
    glPointSize(mSelectedVertexThickness);
    //glBegin(GL_POINTS);
    glColor4f(mSelectedVertexColor.redF(),mSelectedVertexColor.greenF(),mSelectedVertexColor.blueF(),mSelectedVertexColor.alphaF());
    DLFLVertexPtrArray::iterator first, last;
    first = object->sel_vptr_array.begin(); last = object->sel_vptr_array.end();
    while ( first != last ){
      GeometryRenderer::instance()->renderVertex(*first);
      ///(*first)->render(); 
      ++first;
    }
    //glEnd();
    glPointSize(1.0);
  }

	if ( !object->sel_eptr_array.empty() ){
		glLineWidth(mSelectedEdgeThickness);
		glColor4f(mSelectedEdgeColor.redF(),mSelectedEdgeColor.greenF(),mSelectedEdgeColor.blueF(),mSelectedEdgeColor.alphaF());
		DLFLEdgePtrArray::iterator first, last;
		first = object->sel_eptr_array.begin(); last = object->sel_eptr_array.end();
		while ( first != last ){
		  glBegin(GL_LINES); {
		    GeometryRenderer::instance()->renderEdge(*first);
		  } glEnd();
		  ///(*first)->render(); 
		  ++first;
		}
		glLineWidth(3.0);
	}

	if ( !object->sel_fptr_array.empty() ){
	  glLineWidth(mSelectedEdgeThickness);
	  glColor4f(mSelectedFaceColor.redF(),mSelectedFaceColor.greenF(),mSelectedFaceColor.blueF(),mSelectedFaceColor.alphaF());
	  DLFLFacePtrArray::iterator first, last;
	  first = object->sel_fptr_array.begin(); last = object->sel_fptr_array.end();
	  while ( first != last )	{
	    GeometryRenderer::instance()->renderFace(*first,false);
	    ///(*first)->render_FVN(); 
	    ++first;
	  }
	  glLineWidth(3.0);
	}

	if ( !object->sel_fvptr_array.empty() ) {
		glPointSize(mSelectedVertexThickness);
		glColor4f(mSelectedVertexColor.redF(),mSelectedVertexColor.greenF(),mSelectedVertexColor.blueF(),mSelectedVertexColor.alphaF());
		glBegin(GL_POINTS);
		DLFLFaceVertexPtrArray::iterator first, last;
		first = object->sel_fvptr_array.begin(); last = object->sel_fvptr_array.end();
		while ( first != last ){
		  GeometryRenderer::instance()->renderFaceVertex(*first,false);
		  ///(*first)->render(); 
		  ++first;
		}
		glEnd();
		glPointSize(1.5);
	}
}

#ifdef GPU_OK

void GLWidget::initCg( ) {
	if (mUseGPU){
	  // Create Context
	  cg.context = cgCreateContext( );
	  // Vertex Profile
	  cg.vertProfile = cgGLGetLatestProfile( CG_GL_VERTEX );
	  cgGLSetOptimalOptions( cg.vertProfile );
	  // Frag Profile
	  // cg.fragProfile = cgGLGetLatestProfile( CG_GL_FRAGMENT );
	  // cgGLSetOptimalOptions( cg.fragProfile );
	  // Vertex Program
	  char *programName = new char[256];
	  sprintf( programName, "%s", "vertShader.cg" );
	  cg.vertProgram = cgCreateProgramFromFile( cg.context, CG_SOURCE, programName, cg.vertProfile, NULL, NULL );
	  checkCgError( cg.context, 5 );
	  // Fragment Program
	  // sprintf( programName, "%s", "fragShader.cg" );
	  // cg.fragProgram = cgCreateProgramFromFile( cg.context, CG_SOURCE, programName, cg.fragProfile, NULL, NULL );
	  delete [] programName;
	  programName = 0;

		// Bind Variables to Cg Parameters
		cg.camToWorld = cgGetNamedParameter( cg.vertProgram, "camToWorld");
		cg.camToWorldIT = cgGetNamedParameter( cg.vertProgram, "camToWorldIT");
		cg.worldToLight = cgGetNamedParameter( cg.vertProgram, "worldToLight");
		// cg.texture = cgGetNamedParameter( cg.fProgram, "texture");
		// cg.shadowMap = cgGetNamedParameter( cg.fProgram, "shadowMap" );
		// cg.renderToTexSize = cgGetNamedParameter( cg.fProgram, "renderToTexSize" );
		// cg.objectID = cgGetNamedParameter( cg.fProgram, "objectID" );
		checkCgError( cg.context, 5 );
		// cg.attenDegrees = cgGetNamedParameter( cg.fProgram, "attenDegrees" );
		cg.eyePosition = cgGetNamedParameter( cg.vertProgram, "eyePosition" );
		// cg.velocity = cgGetNamedParameter( cg.vertProgram, "velocity" );
		// cg.objCenter = cgGetNamedParameter( cg.vertProgram, "objCenter" );
		
		//from book
		cg.globalAmbient = cgGetNamedParameter( cg.vertProgram, "globalAmbient" );
		cg.lightColor = cgGetNamedParameter( cg.vertProgram, "lightColor" );
		cg.lightPosition = cgGetNamedParameter( cg.vertProgram, "lightPosition" );
		cg.Ke = cgGetNamedParameter( cg.vertProgram, "Ke" );
		cg.Kd = cgGetNamedParameter( cg.vertProgram, "Kd" );
		cg.Ka = cgGetNamedParameter( cg.vertProgram, "Ka" );
		cg.Ks = cgGetNamedParameter( cg.vertProgram, "Ks" );
		cg.shininess = cgGetNamedParameter( cg.vertProgram, "shininess" );

	  cgGLLoadProgram( cg.vertProgram );
	  checkCgError( cg.context, 1 );
	  // cgGLLoadProgram( cg.fragProgram );
	  // checkCgError( cg.context, 2 );
	}
}

#endif // GPU_OK

void GLWidget::wheelEvent(QWheelEvent *event) {

	// int x = event->x(), y = event->y();
	// int numDegrees = event->delta() / 8;
	// int numSteps = numDegrees / 15;
	// double z;
	mCamera->HandleMouseWheel(event->delta());
	repaint();

	// if ( viewport.current() == VPRotate) {
	// 	viewport.handle_rotate(VPRelease,x,y); // Stop rotating
	// 	//viewport.handle_zoom(VPPush,x,y); // Start zooming
	// 	//QCursor::setShape(Qt::SizeAllCursor);
	// } 
	// else  if ( 	!(QApplication::keyboardModifiers() == Qt::AltModifier) && 
	// 						!(QApplication::keyboardModifiers() == Qt::ShiftModifier) &&
	// 						!(QApplication::keyboardModifiers() == (Qt::ShiftModifier | Qt::AltModifier) ) ){
	// 	// QMessageBox::about(this, tr("false"),tr("numDegrees = %1").arg(numDegrees));
	// 	viewport.handle_zoom(VPPush,x,y); // Start zooming
	// 	viewport.send_to_current(VPPush,x,y);
	// 	// updateGL();
	// 	repaint();
	// 
	// 	viewport.handle_zoom(VPPush,x+numDegrees*3,y); // Start zooming
	// 	//QMessageBox::about(this, tr("false"),tr("z = %1").arg(z));
	// 	viewport.send_to_current(VPPush,x,y);		
	// 	viewport.handle_zoom(VPRelease,x+numDegrees,y); // Start zooming
	// 	viewport.send_to_current(VPRelease,x,y);		
	// 	//updateGL();
	// 	repaint();
	// 
	// 	// 	//QCursor::setShape(Qt::SizeAllCursor);
	// }
	// else  if ( QApplication::keyboardModifiers() == Qt::ShiftModifier ){
	// 
	// 	viewport.handle_pan(VPPush,x,y); 
	// 	viewport.send_to_current(VPPush,x,y);
	// 
	// 	viewport.handle_pan(VPPush,x+numDegrees*2,y);
	// 	viewport.send_to_current(VPPush,x,y);		
	// 
	// 	viewport.handle_pan(VPRelease,x+numDegrees*2,y);
	// 	viewport.send_to_current(VPRelease,x,y);
	// 
	// 	//updateGL();
	// 	repaint();
	// }
	// else  if ( QApplication::keyboardModifiers() == Qt::AltModifier ){
	// 
	// 	viewport.handle_pan(VPPush,x,y); // Start panning up and down
	// 	viewport.send_to_current(VPPush,x,y);
	// 
	// 	viewport.handle_pan(VPPush,x,y+numDegrees*2); // Start panning up and down		
	// 	viewport.send_to_current(VPPush,x,y);		
	// 
	// 	viewport.handle_pan(VPRelease,x,y+numDegrees*2); // Start panning up and down
	// 	viewport.send_to_current(VPRelease,x,y);
	// 
	// 	//updateGL();
	// 	repaint();
	// }
	// else  if ( QApplication::keyboardModifiers() == (Qt::AltModifier | Qt::ShiftModifier) ){
	// 
	// 	viewport.handle_rotate(VPPush,x,y); 
	// 	viewport.send_to_current(VPPush,x,y);
	// 
	// 	viewport.handle_rotate(VPPush,x+numDegrees*2,y); 
	// 	viewport.send_to_current(VPPush,x,y);		
	// 
	// 	viewport.handle_rotate(VPRelease,x+numDegrees*2,y); 
	// 	viewport.send_to_current(VPRelease,x,y);
	// 
	// 	//updateGL();
	// 	repaint();
	// }
	// else  if ( QApplication::keyboardModifiers() == (Qt::ShiftModifier | Qt::MetaModifier) ){
	// 
	// 	viewport.handle_rotate(VPPush,x,y); 
	// 	viewport.send_to_current(VPPush,x,y);
	// 
	// 	viewport.handle_rotate(VPPush,x,y+numDegrees*2); 
	// 	viewport.send_to_current(VPPush,x,y);		
	// 
	// 	viewport.handle_rotate(VPRelease,x,y+numDegrees*2); 
	// 	viewport.send_to_current(VPRelease,x,y);
	// 
	// 	//updateGL();
	// 	repaint();
	// }
}
void GLWidget::mousePressEvent(QMouseEvent *event) {
	
	if ( QApplication::keyboardModifiers() == Qt::AltModifier || 
			(event->buttons() == Qt::LeftButton && QApplication::keyboardModifiers() == (Qt::ShiftModifier | Qt::AltModifier)) ){
		mCamera->HandleMouseEvent(event->button(), event->type(), event->x(), event->y());
		repaint();	
	}	
	else event->ignore();
	
	// if ( QApplication::keyboardModifiers() == Qt::AltModifier ) {
	// 	int x = event->x(), y = event->y();
	// 
	// 	if ( event->buttons() == Qt::LeftButton ) {
	// 		if ( viewport.current() == VPPan) {
	// 			viewport.handle_pan(VPRelease,x,y); // Stop panning
	// //viewport.handle_rotate(VPPush,x,y); // Start rotating
	// 			setCursor(Qt::ArrowCursor);
	// 		}
	// 		else {
	// 			viewport.handle_rotate(VPPush,x,y); // Start rotating
	// // 	set the qt cursor - make a custom cursor later
	// 			setCursor(Qt::CrossCursor);
	// 		}
	// 		// 	viewport.send_to_current(GLWidget::translateEvent(event),x,y);
	// 		// 	this->updateGL();
	// 
	// 	} 
	// 	else if ( event->buttons() == Qt::RightButton ) {
	// 		if ( viewport.current() == VPRotate) {
	// 			viewport.handle_rotate(VPRelease,x,y); // Stop rotating
	// //viewport.handle_zoom(VPPush,x,y); // Start zooming
	// 			setCursor(Qt::ArrowCursor);
	// 		} 
	// 		else {
	// 			viewport.handle_zoom(VPPush,x,y); // Start zooming
	// 			setCursor(Qt::SizeAllCursor);
	// 		}
	// 	}
	// 	else if ( event->buttons() == Qt::MidButton ) {
	// 		if ( viewport.current() == VPRotate ) {
	// 			viewport.handle_rotate(VPRelease,x,y); // Stop rotating
	// //viewport.handle_pan(VPPush,x,y);
	// 			setCursor(Qt::ArrowCursor);
	// 		} 
	// 		else {
	// 			viewport.handle_pan(VPPush,x,y); // Start panning
	// 			setCursor(Qt::CrossCursor);
	// 		}
	// 	}
	// }
	// else if ( (event->buttons() == Qt::LeftButton && QApplication::keyboardModifiers() == (Qt::ShiftModifier | Qt::AltModifier)) ){
	// 	int x = event->x(), y = event->y();
	// 
	// 	if ( viewport.current() == VPRotate ) {
	// 		viewport.handle_rotate(VPRelease,x,y); // Stop rotating
	// 		//viewport.handle_pan(VPPush,x,y);
	// 		setCursor(Qt::ArrowCursor);
	// 	} 
	// 	else {
	// 		viewport.handle_pan(VPPush,x,y); // Start panning
	// 		setCursor(Qt::CrossCursor);
	// 	}
	// }
	// else if ( event->buttons() == Qt::RightButton && QApplication::keyboardModifiers() == Qt::ShiftModifier){
	// 	// event->ignore();
	// 	if (!isBrushVisible()) showBrush();
	// 	mBrushStartX = event->x();
	// 	// mBrushStartY = event->y();
	// }
	// else event->ignore();

}

// this has been modified for the QT interface, so it handles focus automatically
void GLWidget::mouseMoveEvent(QMouseEvent *event) {	
	if ( QApplication::keyboardModifiers() == Qt::AltModifier || 
			(event->buttons() == Qt::LeftButton && QApplication::keyboardModifiers() == (Qt::ShiftModifier | Qt::AltModifier)) ){
		mCamera->HandleMouseMotion(event->x(),event->y());
		repaint();	
	}	
	else event->ignore();
	
	// int x = event->x(), y = event->y();
	// if (isBrushVisible()) repaint();
	// 
	// 
	// if ( QApplication::keyboardModifiers() == Qt::AltModifier ) {
	// 	if ( viewport.send_to_current(GLWidget::translateEvent(event),x,y) ) {
	// 		repaint();
	// 	}
	// } 
	// else if ( QApplication::keyboardModifiers() == (Qt::AltModifier | Qt::ShiftModifier) ) {
	// 	if ( viewport.send_to_current(GLWidget::translateEvent(event),x,y) ) {
	// 		repaint();
	// 	}
	// }
	// else if ( event->buttons() == Qt::RightButton && QApplication::keyboardModifiers() == Qt::ShiftModifier){
	// 	// event->ignore();
	// 	setBrushSize(mBrushSize+event->x()-mBrushStartX);
	// }
	// else event->ignore();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
	if ( QApplication::keyboardModifiers() == Qt::AltModifier || 
			(event->buttons() == Qt::LeftButton && QApplication::keyboardModifiers() == (Qt::ShiftModifier | Qt::AltModifier)) ){
		mCamera->HandleMouseEvent(event->button(), event->type(), event->x(), event->y());
		repaint();	
	}	
	else event->ignore();

	// int x = event->x(), y = event->y();
	// if ( QApplication::keyboardModifiers() == Qt::AltModifier ) {
	// 	if ( viewport.send_to_current(GLWidget::translateEvent(event),x,y) ) {
	// 		//updateGL();
	// 	repaint();
	// 	}
	// } 
	// else if ( QApplication::keyboardModifiers() == (Qt::AltModifier | Qt::ShiftModifier) ) {
	// 	if ( viewport.send_to_current(GLWidget::translateEvent(event),x,y) ) {
	// 		//updateGL();
	// 	repaint();
	// 	}
	// }
	// else if ( event->buttons() == Qt::RightButton && QApplication::keyboardModifiers() == Qt::ShiftModifier){
	// 	mBrushStartX = 0;
	// 	// mBrushStartY = 0;
	// }
	// else event->ignore();
}

void GLWidget::setViewportColor(QColor c){
	mViewportColor = c;
	redraw();
}

QColor GLWidget::getViewportColor(){
	return mViewportColor;
}

void GLWidget::setRenderColor(QColor c){
	mRenderColor = c;
	GeometryRenderer *gr = GeometryRenderer::instance();
	gr->renderColor[0] = c.redF();
	gr->renderColor[1] = c.greenF();
	gr->renderColor[2] = c.blueF();
	gr->renderColor[3] = c.alphaF();
	redraw();
}

QColor GLWidget::getRenderColor(){
	return mRenderColor;
}

void GLWidget::setWireframeColor(QColor c){
	mWireframeColor = c;
	if(renderer)
	  renderer->setWireframeColor(mWireframeColor);
	redraw();
}

QColor GLWidget::getWireframeColor(){
	return mWireframeColor;
}

void GLWidget::setSilhouetteColor(QColor c){
	mSilhouetteColor = c;
	if( renderer )
	  renderer->setSilhouetteColor(mSilhouetteColor);
	redraw();
}

QColor GLWidget::getSilhouetteColor(){
	return mSilhouetteColor;
}

void GLWidget::setWireframeThickness(double t){
	mWireframeThickness = t;
	if( renderer )
	  renderer->setWireframeThickness(t);
	redraw();
}

double GLWidget::getWireframeThickness(){
	return mWireframeThickness;
}

void GLWidget::setVertexThickness(double t){
	mVertexThickness = t;
	if( renderer )
	  renderer->setVertexThickness(t);
	redraw();
}
double GLWidget::getVertexThickness(){
	return mVertexThickness;
}

void GLWidget::setSilhouetteThickness(double t){
	mSilhouetteThickness = t;
	if( renderer )
	  renderer->setSilhouetteThickness(t);
	redraw();
}

double GLWidget::getSilhouetteThickness(){
	return mSilhouetteThickness;
}

void GLWidget::setSelectedVertexThickness(double t){
	mSelectedVertexThickness = t;
	redraw();
}

double GLWidget::getSelectedVertexThickness(){
	return mSelectedVertexThickness;
}

void GLWidget::setSelectedEdgeThickness(double t){
	mSelectedEdgeThickness = t;
	redraw();
}

double GLWidget::getSelectedEdgeThickness(){
	return mSelectedEdgeThickness;
}

void GLWidget::setSelectedEdgeColor(QColor c){
	mSelectedEdgeColor = c;
	redraw();
}

QColor GLWidget::getSelectedEdgeColor(){
	return mSelectedEdgeColor;
}

void GLWidget::setSelectedFaceColor(QColor c){
	mSelectedFaceColor = c;
	redraw();
}

QColor GLWidget::getSelectedFaceColor(){
	return mSelectedFaceColor;
}

void GLWidget::setSelectedVertexColor(QColor c){
	mSelectedVertexColor = c;
	redraw();
}

QColor GLWidget::getSelectedVertexColor(){
	return mSelectedVertexColor;
}

void GLWidget::setVertexIDBgColor(QColor c){
	mVertexIDBgColor = c;
	redraw();	
}

QColor GLWidget::getVertexIDBgColor(){
	return mVertexIDBgColor;
}

void GLWidget::setFaceIDBgColor(QColor c){
	mFaceIDBgColor = c;
	redraw();	
}

QColor GLWidget::getFaceIDBgColor(){
	return mFaceIDBgColor;
}

void GLWidget::setEdgeIDBgColor(QColor c){
	mEdgeIDBgColor = c;
	redraw();	
}

QColor GLWidget::getEdgeIDBgColor(){
	return mEdgeIDBgColor;
}

void GLWidget::switchTo(VPView view){
	// viewport.switchTo(view);
	// 
	// if ( view == VPPersp ) // Change camera settings to zoom in closer in perspective view
	// 	viewport.setPerspView(10,10,10,0,0,0,0,1,0);
}
//for vertex locator moving...
void erase_dlp(DLFLLocatorPtr lp)  { delete lp; } // brianb

// Global operations (don't require selection)
void GLWidget::recomputeNormals(void)     // Recompute normals and lighting
{
	object->computeNormals();
	computeLighting( object, patchObject, &plight);
}

void GLWidget::recomputeLighting(void)                // Recompute lighting
{
	computeLighting( object, patchObject, &plight);
}

void GLWidget::recomputePatches(void) // Recompute the patches for patch rendering
{
  if(patchObject)
    patchObject->updatePatches(object);
}

void GLWidget::setWarmLightColor(QColor c){
	plight.warmcolor.set(c.redF(),c.greenF(),c.blueF());
	recomputeLighting();
	redraw();
}

void GLWidget::setCoolLightColor(QColor c){
	plight.coolcolor.set(c.redF(),c.greenF(),c.blueF());
	recomputeLighting();
	redraw();
}

void GLWidget::setLightIntensity(double i){
	plight.intensity = i;
	recomputeLighting();
	redraw();
}
