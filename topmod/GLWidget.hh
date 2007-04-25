#ifndef _GL_WIDGET_H
#define _GL_WIDGET_H

#include "Viewport.hh"
//#include "include/Graphics/Grid.hh"
#include <QtOpenGL>
#include <QGLContext>
#include <QCursor>

#include <QBrush>
#include <QImage>
#include <QPen>

// #undef Point
//3d spacenav include???
// #include <ConnexionClientAPI.h>

// #import <3DConnexionClient/ConnexionClientAPI.h>
//#include <ConnexionClient.h>

// #include "TdxDeviceWrappers.hh"

// #define Point

#include <QApplication>
#include <QKeyEvent>
#include <QFont>
#include <QPushButton>

#include "DLFLObject.hh"
#include "DLFLRenderer.hh"

class QCursor;
class QImage;
// class QPaintEvent;
class GLWidget;

// typedef GLWidget * GLWidgetPtr;

class GLWidget : public QGLWidget {
  Q_OBJECT        // must include this if you use Qt signals/slots
  
public :
  // GLWidget( QWidget *parent = 0 ) : QGLWidget(parent) { 
  // 
  // };

	   // Constructor
	GLWidget(int x, int y, int w, int h, QWidget *parent = 0 );
	   // Constructor
	GLWidget(int x, int y, int w, int h, DLFLRendererPtr rp, DLFLObjectPtr op=NULL, QWidget *parent = 0 );

	GLWidget(	int w, int h, DLFLRendererPtr rp, DLFLObjectPtr op, const QGLFormat & format, QWidget * parent = 0 );

	~GLWidget( );

public slots :
  void toggleFullScreen( );

protected :
  
  void initializeGL( );
  void paintGL( );

	void drawText( int width, int height );
	
	void resizeGL( int width, int height );
	
	void setupViewport(int width, int height);

	// void showEvent(QShowEvent *event);

	void paintOverlayGL();
	void initializeOverlayGL();
	void resizeOverlayGL(int width, int height);
	
  bool mIsFullScreen;

	   // Selection lists - these are shared by all viewports
	static DLFLVertexPtrArray sel_vptr_array; // List of selected DLFLVertex pointers
	static DLFLEdgePtrArray sel_eptr_array; // List of selected DLFLEdge pointers
	static DLFLFacePtrArray sel_fptr_array; // List of selected DLFLFace pointers
	static DLFLFaceVertexPtrArray sel_fvptr_array; // List of selected DLFLFaceVertex pointers

	Viewport viewport;                     // Viewport for the window

	   // Each viewport will have its own object to display. But since only
	   // the pointer is stored, different viewports can share the same object.
	DLFLObjectPtr object;	                 // Pointer to DLFLObject


	   // Each viewport will have its own renderer. But since only the pointer
	   // is stored different viewports can share the same renderer object.
	DLFLRendererPtr renderer;

	   // Each viewport will have its own grid
	// Grid grid;                                        // Display grid

	   // Boolean flags - these are viewport specific
	bool showgrid;                           // Should grid be shown?
	bool showaxes;                           // Should axes be shown?

	QImage image;

public :

	QCursor *cursor;
	void redraw();
	
	   //--- Initialize the selection lists ---//
	static void initializeSelectionLists(int num)
	  {
	       // Reserves memory for the arrays to avoid reallocation
	       // It is not mandatory to call this function, but recommended
	    sel_vptr_array.reserve(num);
	    sel_eptr_array.reserve(num);
	    sel_fptr_array.reserve(num);
	    sel_fvptr_array.reserve(num);
	  }

	   //--- Add items to the selection lists - check for NULL pointers ---//
	static void addToSelection(DLFLVertexPtr vp)
	  {
	    if ( vp ) sel_vptr_array.push_back(vp);
	  }

	static void addToSelection(DLFLEdgePtr ep)
	  {
	    if ( ep ) sel_eptr_array.push_back(ep);
	  }

	static void addToSelection(DLFLFacePtr fp)
	  {
	    if ( fp ) sel_fptr_array.push_back(fp);
	  }

	static void addToSelection(DLFLFaceVertexPtr fvp)
	  {
	    if ( fvp ) sel_fvptr_array.push_back(fvp);
	  }

	   //--- Check if given item is there in the selection list ---//
	static bool isSelected(DLFLVertexPtr vp)
	  {
	    bool found = false;
	    if ( vp )
	       {
	         for (uint i=0; i < sel_vptr_array.size(); ++i)
	            if ( sel_vptr_array[i] == vp )
	               {
	                 found = true; break;
	               }
	       }
	    return found;
	  }

	static bool isSelected(DLFLEdgePtr ep)
	  {
	    bool found = false;
	    if ( ep )
	       {
	         for (uint i=0; i < sel_eptr_array.size(); ++i)
	            if ( sel_eptr_array[i] == ep )
	               {
	                 found = true; break;
	               }
	       }
	    return found;
	  }

	static bool isSelected(DLFLFacePtr fp)
	  {
	    bool found = false;
	    if ( fp )
	       {
	         for (uint i=0; i < sel_fptr_array.size(); ++i)
	            if ( sel_fptr_array[i] == fp )
	               {
	                 found = true; break;
	               }
	       }
	    return found;
	  }

	static bool isSelected(DLFLFaceVertexPtr fvp)
	  {
	    bool found = false;
	    if ( fvp )
	       {
	         for (uint i=0; i < sel_fvptr_array.size(); ++i)
	            if ( sel_fvptr_array[i] == fvp )
	               {
	                 found = true; break;
	               }
	       }
	    return found;
	  }

	   //--- Set the selected item at given index ---//
	   //--- If size of array is smaller than index item will be added to end of array ---//
	static void setSelectedVertex(int index, DLFLVertexPtr vp)
	  {
	    if ( vp && index >= 0 )
	       {
	         if ( (uint) index < sel_vptr_array.size() ) sel_vptr_array[index] = vp;
	         else sel_vptr_array.push_back(vp);
	       }
	  }

	static void setSelectedEdge(int index, DLFLEdgePtr ep)
	  {
	    if ( ep && index >= 0 )
	       {
	         if ( (uint) index < sel_eptr_array.size() ) sel_eptr_array[index] = ep;
	         else sel_eptr_array.push_back(ep);
	       }
	  }

	static void setSelectedFace(int index, DLFLFacePtr fp)
	  {
	    if ( fp && index >= 0 )
	       {
	         if ( (uint) index < sel_fptr_array.size() ) sel_fptr_array[index] = fp;
	         else sel_fptr_array.push_back(fp);
	       }
	  }

	static void setSelectedFaceVertex(int index, DLFLFaceVertexPtr fvp)
	  {
	    if ( fvp && index >= 0 )
	       {
	         if ( (uint) index < sel_fvptr_array.size() ) sel_fvptr_array[index] = fvp;
	         else sel_fvptr_array.push_back(fvp);
	       }
	  }

	   //--- Return the selected items at given index ---//
	static DLFLVertexPtr getSelectedVertex(int index)
	  {
	    if ( (uint) index < sel_vptr_array.size() ) return sel_vptr_array[index];
	    return NULL;
	  }

	static DLFLEdgePtr getSelectedEdge(int index)
	  {
	    if ( (uint) index < sel_eptr_array.size() ) return sel_eptr_array[index];
	    return NULL;
	  }

	static DLFLFacePtr getSelectedFace(int index)
	  {
	    if ( (uint) index < sel_fptr_array.size() ) return sel_fptr_array[index];
	    return NULL;
	  }

	static DLFLFaceVertexPtr getSelectedFaceVertex(int index)
	  {
	    if ( (uint) index < sel_fvptr_array.size() ) return sel_fvptr_array[index];
	    return NULL;
	  }

	   //--- Find the number of items in the selection lists ---//
	static int numSelectedVertices(void)
	  {
	    return sel_vptr_array.size();
	  }

	static int numSelectedEdges(void)
	  {
	    return sel_eptr_array.size();
	  }

	static int numSelectedFaces(void)
	  {
	    return sel_fptr_array.size();
	  }

	static int numSelectedFaceVertices(void)
	  {
	    return sel_fvptr_array.size();
	  }

	   //--- Clear individual selection lists ---//
	static void clearSelectedVertices(void)
	  {
	    sel_vptr_array.clear();
	  }

	static void clearSelectedEdges(void)
	  {
	    sel_eptr_array.clear();
	  }

	static void clearSelectedFaces(void)
	  {
	    sel_fptr_array.clear();
	  }

	static void clearSelectedFaceVertices(void)
	  {
	    sel_fvptr_array.clear();
	  }

	   // Reset all selection lists
	static void clearSelected(void)
	  {
	    sel_vptr_array.clear();
	    sel_eptr_array.clear();
	    sel_fptr_array.clear();
	    sel_fvptr_array.clear();
	  }

	   // Draw the selected items
	static void drawSelected(void);

	   // Subroutine to translate FLTK events to Viewport events
	static VPMouseEvent translateEvent(QMouseEvent *event)
	  {
			// QMessageBox::about(NULL, tr("About TopMod"),tr("%1").arg(event->type()));
	    return ( (event->type() == 2/*QEvent::mouseButtonPress*/) ? VPPush :
	             ( (event->type() == 3/*QEvent::mouseButtonRelease*/) ? VPRelease :
	               ( (event->type() == 5/*QEvent::mouseMove*/) ? VPDrag : VPUnknown ) ) );
	  }

	void switchTo(VPView view)
	  {
	       // Change grid orientation for front/back and left/right views
	    // switch ( view )
	    //    {
	    //      case VPFront :
	    //      case VPBack :
	    //                   // grid.setPlane(XY);
	    //                   break;
	    //      case VPLeft :
	    //      case VPRight :
	    //                   // grid.setPlane(YZ);
	    //                   break;
	    //      default :
	    //                   // grid.setPlane(ZX);
	    //    }
	    viewport.switchTo(view);
	    if ( view == VPPersp )
	          // Change camera settings to zoom in closer in perspective view
	       viewport.setPerspView(10,10,10,0,0,0,0,1,0);
	  }

	   // Toggle grid display on/off
	void toggleGrid(void)
	  {
	    // if ( showgrid == true ) showgrid = false;
// 	    else showgrid = true;
// 			this->updateGL();
	  }

	   // Toggle axis display on/off
	void toggleAxes(void)
	  {
	    if ( showaxes == true ) showaxes = false;
	    else showaxes = true;
			this->updateGL();
	  }

	   // Set the object which should be shown in this viewport
	void setObject(DLFLObjectPtr op)
	  {
	    object = op;
	  }

	   // Set the renderer for this viewport
	void setRenderer(DLFLRendererPtr rp)
	  {
	    renderer = rp;
			this->updateGL();
	  }

	   // Set the render flags
	void setRenderFlags(int rflags)
	  {
	    if ( renderer ) renderer->setRenderFlags(rflags);
			this->updateGL();
	  }

	   // Toggle wireframe
	void toggleWireframe(void)
	  {
	    if ( renderer ) renderer->toggleWireframe();
			this->updateGL();
	  }

	   // Toggle silhouette
	void toggleSilhouette(void)
	  {
	    if ( renderer ) renderer->toggleSilhouette();
			this->updateGL();
	
	  }

	   // Toggle vertices
	void toggleVertices(void)
	  {
	    if ( renderer ) renderer->toggleVertices();
			this->updateGL();
	  }

	   // Toggle object orientation
	void toggleObjectOrientation(void)
	  {
	    if ( renderer ) renderer->toggleObjectOrientation();
			this->updateGL();
	
	  }

		   // Toggle object orientation
		void toggleNormals(void)
		  {
		    if ( renderer ) renderer->toggleNormals();
				this->updateGL();

		  }
		
	   // Get the render flags
	int getRenderFlags(void) const
	  {
	    if ( renderer )
	       return renderer->getRenderFlags();
	    return -1;
	  }

	   // Subroutines for selecting Vertices, Edges, Faces and FaceVertices (Corners)
	DLFLVertexPtr selectVertex(int mx, int my);
	DLFLEdgePtr selectEdge(int mx, int my);
	DLFLFacePtr selectFace(int mx, int my);
	DLFLFaceVertexPtr selectFaceVertex(DLFLFacePtr fp, int mx, int my);

	   // Implement the draw method of Fl_Gl_Window base class
	//void draw(void);

	   // Event handler for the viewport. handles only mouse events when glwidget has focus
	   // which relate to navigational controls (ALT + push/drag/release)
	void mouseMoveEvent(QMouseEvent *event);
	
	void mousePressEvent(QMouseEvent *event);
	
	void mouseReleaseEvent(QMouseEvent *event);
	
	void wheelEvent(QWheelEvent *event);
	
public:
	
	//for the space navigator SDK
	// static void spaceNavEvent(io_connect_t connection, natural_t messageType, void *messageArgument);



};

#endif 
