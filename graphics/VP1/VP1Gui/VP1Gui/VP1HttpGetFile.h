/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1HttpGetFile                      //
//                                                            //
//  Description: Download files in the background and get     //
//               notified upon completion. Optionally         //
//               require correct md5sum.                      //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1HTTPGETFILE_H
#define VP1HTTPGETFILE_H

#include <QObject>
#include <QByteArray>

class VP1HttpGetFile : public QObject {

  Q_OBJECT

public:

  VP1HttpGetFile(QObject * parent = 0);
  virtual ~VP1HttpGetFile();

  void setMaxNumberOfPendingDownloads(unsigned);
  unsigned maxNumberOfPendingDownloads() const;//Default is 8. A value of 0 disables limit.
  unsigned numberOfPendingDownloads() const;

  bool isDownloading(const QString& urltofile) const;
  bool isDownloadingTo(const QString& localtargetfile) const;

  QString startDownload( const QString& urltofile,
			 const QString& localtargetfile,
			 const QString& md5sum = "",//md5sum in hex format (like linux md5sum program outputs)
			 const QString& data = "" );

  //Returns non-empty string in case download could not be started
  //(too many pending downloads or already downloading to that target).

signals:
  //If download was started succesfully, it will always end with one
  //of the following signals being emitted at a later time:
  void downloadSuccessful( const QString& urltofile,
			   const QString& localtargetfile,
			   const QString& data );
  void downloadFailed( const QString& error,
		       const QString& urltofile,
		       const QString& localtargetfile,
		       const QString& data );
private:
  class Imp;
  Imp * d;
private slots:
  void downloadThreadFinished();
  void downloadThreadTerminated();
};

//Todo: Set accepts-gzip header?


#endif
