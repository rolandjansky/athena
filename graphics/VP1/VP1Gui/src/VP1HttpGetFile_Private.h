/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Internal classes for VP1HttpGetFile                       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include <QThread>
#include <QMutex>
#include <QTimer>
#include <QDateTime>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QUrl>

//#include <QHttp> // Qt 4
//#include <QNetworkAccessManager> // TODO: use this to port to Qt5

#include "VP1Gui/VP1MD5Sum.h"

class VP1HttpGetFile_DownloadThread : public QThread {

  Q_OBJECT

public:

  VP1HttpGetFile_DownloadThread( const QString& urltofile,
				 const QString& localtargetfile,
				 const QString& expectedMD5Sum,
				 const QString& data,
				 QObject * parent = 0);
  virtual ~VP1HttpGetFile_DownloadThread(){}

  //For accessing the results:
  bool errors() const { m_mutex.lock(); bool err=!m_errorString.isEmpty(); m_mutex.unlock(); return err; }
  QString errorString() const { m_mutex.lock(); QString err = m_errorString; m_mutex.unlock(); return err; }

  QString urltofile() const  { return m_urltofile; }
  QString localtargetfile() const { return m_localtargetfile; }
  QString expectedMD5Sum() const { return m_expectedMD5Sum; }
  QString data() const { return m_data; }

protected:
  void run();
public slots:
  void checkForStall();
private slots:
  void done(bool);
  void dataReadProgress();
private:
  const QString m_urltofile;
  const QString m_localtargetfile;
  const QString m_expectedMD5Sum;
  const QString m_data;
  QString m_errorString;
  mutable QMutex m_mutex;//Protects m_errors;
//  QHttp * m_http; // Old Qt4 code --> TODO: need to be ported to Qt5!!
  enum DOWNLOADFINISHSTATUS { NOTFINISHED, NOERRORS, HTTPERROR, STALLEDERROR, SIZEDECREASEDERROR };
  DOWNLOADFINISHSTATUS m_downloadFinishedStatus;//-1: not finished, 0: error, 1: no-error
  unsigned m_lastChangeTime;
  qint64 m_lastcheckedsize;//-2 never checked, -1 does not exists.
  enum CLEANUPOPT { CLEANUPLOCALFILE, DONTTOUCHLOCALFILE};
  void endInFailure(const QString&, CLEANUPOPT );
};

//Remember that we are also included from the qt moc file:
#ifdef VP1HttpGetFile_cxx

//____________________________________________________________________
VP1HttpGetFile_DownloadThread::VP1HttpGetFile_DownloadThread( const QString& urltofile,
							      const QString& localtargetfile,
							      const QString& expectedMD5Sum,
							      const QString& data,
							      QObject * parent )
  : QThread(parent),
    m_urltofile(urltofile),
    m_localtargetfile(localtargetfile),
    m_expectedMD5Sum(expectedMD5Sum),
    m_data(data),
    m_errorString("Download not finished"),
//    m_http(0),
    m_downloadFinishedStatus(NOTFINISHED),
    m_lastChangeTime(0),
    m_lastcheckedsize(-2)
{
}

//____________________________________________________________________
void VP1HttpGetFile_DownloadThread::endInFailure(const QString& err, CLEANUPOPT opt)
{
  m_mutex.lock();

  if (opt==CLEANUPLOCALFILE&&QFile::exists(m_localtargetfile))
    QFile::remove(m_localtargetfile);

  m_errorString = err.isEmpty() ? "Unspecified error" : err;
  m_mutex.unlock();
}

//____________________________________________________________________
void VP1HttpGetFile_DownloadThread::checkForStall()
{
	// TODO: to be ported to Qt5!!
//  if (!m_http)
//    return;
//  unsigned currentTime = QDateTime::currentDateTime().toTime_t();
//  if (currentTime>m_lastChangeTime+10) {//10s
//    m_downloadFinishedStatus = STALLEDERROR;
//    quit();
//  }
}

//____________________________________________________________________
void VP1HttpGetFile_DownloadThread::done(bool error)
{
  m_downloadFinishedStatus = error ? HTTPERROR : NOERRORS;
  quit();
}

//____________________________________________________________________
void VP1HttpGetFile_DownloadThread::dataReadProgress()
{
  //We monitor that filesize of files downloaded is strictly
  //increasing (and that a created file does not disappear):
  QFileInfo fi(m_localtargetfile);
  qint64 currentsize = fi.exists() ? fi.size() : -1;
  if (currentsize < m_lastcheckedsize) {
    m_downloadFinishedStatus = SIZEDECREASEDERROR;
    quit();
  }

  //For stall-detection:
  if (currentsize > m_lastcheckedsize)
    m_lastChangeTime = QDateTime::currentDateTime().toTime_t();

  m_lastcheckedsize = currentsize;
}

//____________________________________________________________________
void VP1HttpGetFile_DownloadThread::run() {

  // NOTE!!  TODO: This has to be ported to Qt 5!!!
  /*
  ////////////////////////
  //  Input validation  //
  ////////////////////////

  //Input validation ==> MD5 sum

  if ( !m_expectedMD5Sum.isEmpty() && !VP1MD5Sum::validMD5Sum(m_expectedMD5Sum) ) {
    endInFailure("Invalid target md5sum: "+m_expectedMD5Sum,CLEANUPLOCALFILE);
    return;
  }

  //Input validation ==> URL

  if (m_urltofile.isEmpty()) {
    endInFailure("Empty URL",DONTTOUCHLOCALFILE);
    return;
  }

  QUrl url(m_urltofile.contains("://") ? m_urltofile : "http://" + m_urltofile);
  if (!url.isValid()||url.host().isEmpty()) {
    endInFailure("Invalid URL: "+m_urltofile,DONTTOUCHLOCALFILE);
    return;
  }
  //Input validation ==> Target file

  //File can not exist already and we abort if we are already
  //downloading a file to that location:
  QFileInfo fi(m_localtargetfile);
  if (fi.exists()) {
    //In one case we might still end well: if we know the target
    //checksum and if the file has it (this also does away with
    //several problems of "simultaneous downloads started":
    if (!m_expectedMD5Sum.isEmpty()&&VP1MD5Sum::sumMatches(m_localtargetfile,m_expectedMD5Sum))
      return;
    endInFailure ("Download target already exists: "+m_localtargetfile,DONTTOUCHLOCALFILE);
    return;
  }
  if (!fi.dir().exists()) {
    endInFailure("Directory ("+fi.dir().absolutePath()+") for download target does not exist: "+m_localtargetfile,DONTTOUCHLOCALFILE);
    return;
  }

  //Open target file:
  QFile file(m_localtargetfile);
  if (!file.open(QIODevice::WriteOnly)) {
    endInFailure("Could not open file for write access: "+m_localtargetfile,DONTTOUCHLOCALFILE);
    return;
  }

  ///////////////////////////////////
  //  HTTP request and event loop  //
  ///////////////////////////////////

  m_http = new QHttp(0);

  connect(m_http,SIGNAL(done(bool)),this,SLOT(done(bool)));
  connect(m_http,SIGNAL(dataReadProgress(int,int)),this,SLOT(dataReadProgress()));

  QHttpRequestHeader header("GET", url.path()+(url.hasQuery()?"?"+url.encodedQuery():QString("")));
  // ^^^ Todo: We could support username/passwd part of url also.
  header.setValue("Host", url.host());
  header.setValue("User-Agent", "ATLASVP1");
  m_http->setHost(url.host());
  m_lastChangeTime = QDateTime::currentDateTime().toTime_t();

  //Go:
  m_http->request(header,0,&file);
  QTimer timer(0);
  connect(&timer, SIGNAL(timeout()), this, SLOT(checkForStall()));
  timer.start(1000);//1s
  exec();
  timer.stop();

  //Cleanup http:
  m_http->blockSignals(true);
  if (m_http->hasPendingRequests())
    m_http->clearPendingRequests();
  if (m_http->currentId()!=0)
    m_http->abort();
  delete m_http;
  m_http = 0;

  file.close();

  //Errors?
  if (m_downloadFinishedStatus!=NOERRORS) {
    switch(m_downloadFinishedStatus) {
    case HTTPERROR:
      endInFailure("Download finished with error "+m_http->errorString(),CLEANUPLOCALFILE);
      return;
    case STALLEDERROR:
      endInFailure("Download stalled",CLEANUPLOCALFILE);
      return;
    case SIZEDECREASEDERROR:
      endInFailure("Aborted since file was either removed or decreased in size.",CLEANUPLOCALFILE);
      return;
    default:
      endInFailure("Unexpected download status (SHOULD NEVER HAPPEN)",CLEANUPLOCALFILE);
      return;
    }
  }

  /////////////////////
  //  Checksum test  //
  /////////////////////

  if (!m_expectedMD5Sum.isEmpty()) {
    bool match = VP1MD5Sum::sumMatches(m_localtargetfile,m_expectedMD5Sum);
    if (!match) {
      endInFailure("Checksum did not match",CLEANUPLOCALFILE);
      return;
    }
  }

  //All ok:

  m_mutex.lock();
  m_errorString = "";
  m_mutex.unlock();
 */

}

#endif
