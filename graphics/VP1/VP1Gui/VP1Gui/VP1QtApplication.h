/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////
//                                                     //
//  Header file for class VP1QtApplication             //
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

#ifndef VP1QTAPPLICATION_H
#define VP1QTAPPLICATION_H

// include C++
//#include <stddef.h> // this to fix the 'ptrdiff_t' does not name a type error with Qt (http://qt-project.org/forums/viewthread/16992)

// include Qt
#include <QApplication>
#include <QMessageBox>




/* reimplementaion of the main QApplication class,
 * to reimplement the notify() function,
 * in order to catch C++ exceptions
 */
class VP1QtApplication: public QApplication {
   Q_OBJECT
public:
   VP1QtApplication(int &argc, char **argv): QApplication(argc, argv) {};
   virtual ~VP1QtApplication() {};
   virtual bool notify(QObject *rec, QEvent *ev);
};

#endif


