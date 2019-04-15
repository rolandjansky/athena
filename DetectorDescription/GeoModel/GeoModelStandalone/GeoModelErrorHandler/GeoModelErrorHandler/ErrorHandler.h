/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ErrorHandler.h
 *
 *  Created on: May 29, 2016
 *      Author: rbianchi
 *
 *  Source:
 *  - http://brianmilco.blogspot.ch/2011/11/color-debug-output-with-qt-and-qdebug.html
 */

#ifndef ERRORHANDLER_H_
#define ERRORHANDLER_H_

#include <QtGlobal>
#include <QString>

//void errorHandler(QtMsgType type, const char *msg)
void errorHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)

{

	const char* msgChar = msg.toStdString().c_str();

    switch (type) {

        case QtDebugMsg:

            fprintf(stderr, "%s\n", msgChar);

            break;

        case QtWarningMsg:

            fprintf(stderr, "\033[1;33mWarning\033[0m: %s\n", msgChar);

            break;

        case QtCriticalMsg:

            fprintf(stderr, "\033[31mCritical\033[0m: %s\n", msgChar);

            break;

        case QtFatalMsg:

            fprintf(stderr, "\033[31mFatal\033[0m: %s\n", msgChar);

            abort();

        case QtInfoMsg:

            break;


    }

}


#endif /* ERRORHANDLER_H_ */
