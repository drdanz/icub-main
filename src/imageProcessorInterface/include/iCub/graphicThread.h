// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _GRAPHICTHREAD_H_
#define _GRAPHICTHREAD_H_

//=============================================================================
// GTK Includes 
//=============================================================================
#include <gtk/gtk.h>

//within Project Include

#include <iCub/YARPImgRecv.h>
#include <iCub/YarpImage2Pixbuf.h>

//YARP include
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

using namespace yarp::os;
using namespace yarp::sig;


class graphicThread : public RateThread{
private:
    /**
	* maximum value for the gtk sliding control
	*/
	double maxAdj;
	/**
	* maximum value for the gtk sliding control
	*/
	double minAdj;
	/**
	* step adjustment for the gtk sliding control
	*/
	double stepAdj;

public:
    /**
    * default constructor
    */
    graphicThread();
    /**
    * destructor
    */
    ~graphicThread();
    /**
    *	initialization of the thread 
    */
    bool threadInit();
    /**
    * active loop of the thread
    */
    void run();
    /**
    *	releases the thread
    */
    void threadRelease();

    /**
    * function that safely close this thread
    */
    void close();
    /**
	* sets the module up
	*/
	void setUp();
    /**
	* sets the adjustments in the window
	*/
	void setAdjs();
    /**
    * set the reference to the module that has istantiated this thread
    * @param module reference to the module
    */
    void setImageProcessModule(void *module);
    /**
	* creates the main Window
	*/
	GtkWidget* createMainWindow(); //
	/**
	* menuFileSingle callback
	*/
	static gint menuFileSingle_CB(GtkWidget *widget, GdkEventExpose *event, gpointer data);
	/**
	* menufileset callback
	*/
	gint menuFileSet_CB(GtkWidget *widget, GdkEventExpose *event, gpointer data);
	/**
	* creates the menubar
	*/
	GtkWidget* createMenubar();
	/**
	* updates status bar
	*/
	void updateStatusbar (GtkStatusbar  *statusbar);
	/**
	* creates some objects necessary for the window
	*/
	void createObjects();
    /**
    * open  all the necessary ports
    */
    bool openPorts();
};

#endif //__GRAPHICTHREAD_H_

//----- end-of-file --- ( next line intentionally left blank ) ------------------
