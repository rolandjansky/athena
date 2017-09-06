/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////
//                                                     //
//  Implementation file for class VP1QtApplication     //
//                                                     //
//  Author: Riccardo Maria BIANCHI <rbianchi@cern.ch>  //
//                                                     //
//  This class reimplements the basic notify() method  //
//  of the QApplication class, in order to catch       //
//  exceptions from C++, and to avoid fatal errors     //
//  like:                                              //
//  "Qt has caught an exception thrown from an event   //
//  handler. Throwing exceptions from an event handler //
//  is not supported in Qt. You must reimplement       //
//  QApplication::notify() and catch all exceptions    //
//  there.                                             //
//                                                     //
//                                                     //
//  Initial version: 19 November 2012                  //
//                                                     //
/////////////////////////////////////////////////////////


// include VP1
#include "VP1Gui/VP1QtApplication.h"
#include "VP1Base/VP1Msg.h"

// include Qt
#include <QEvent>

// include C++
#include <typeinfo>
//#include <stddef.h> // this to fix the 'ptrdiff_t' does not name a type error with Qt (http://qt-project.org/forums/viewthread/16992)


bool VP1QtApplication::notify(QObject *rec, QEvent *ev)
{
   //std::cout << (3, "Called VP1QtApplication::notify()") << std::endl;

	// only for debug
	//VP1Msg::message("VP1QtApplication:notify()");

   // for normal events we pass to QApplication:notify()...
   try {
	   return QApplication::notify(rec, ev);
   }
   catch (std::exception &e) {
	   VP1Msg::message("VP1QtApplication: std::exception: ");
	   qWarning("VP1QtApplication: Error '%s' sending event '%s' to object '%s' ('%s')",
		   e.what(), typeid(*ev).name(), qPrintable(rec->objectName()),
		   typeid(*rec).name());
       qWarning("throwing the exception again...");
       throw e;
	   return false;
   }
   catch (char const *str) {
	   VP1Msg::message("VP1QtApplication: EXCEPTION: " + QString(str) );
	   return false;
   }
   catch (...) {
	   VP1Msg::message("VP1QtApplication: Unknown exception!");
	   return false;
   }
   //VP1Msg::message("VP1QtApplication: outside catch..."); // only for DEBUG
   return false;
}
