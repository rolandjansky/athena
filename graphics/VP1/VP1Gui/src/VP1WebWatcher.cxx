/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1WebWatcher                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Gui/VP1WebWatcher.h"

#include <QUrl>
#include <QThread>
#include <QDateTime>

#include <iostream>


// TODO: replace QHttp with QNetworkManager!! See: http://stackoverflow.com/a/26182590
// #include <QHttp>
// #include <QHttpRequestHeader>
// #include <QHttpResponseHeader>

//Special values for result strings:
static const QString VP1WebWatcher_PREFIX = "vp1webwatcher_";
static const QString VP1WebWatcher_Unknown = VP1WebWatcher_PREFIX+"unknown";
static const QString VP1WebWatcher_ResultNotReady = VP1WebWatcher_PREFIX+"result_not_ready";
static const QString VP1WebWatcher_UrlInvalid = VP1WebWatcher_PREFIX+"url_invalid";
static const QString VP1WebWatcher_httpProblems = VP1WebWatcher_PREFIX+"http_problems";
static const QString VP1WebWatcher_FileNotExist = VP1WebWatcher_PREFIX+"file_not_existant";

//____________________________________________________________________
class VP1WebWatcher::Imp {
public:
  class HttpThread : public QThread {
  public:
    //________________________________________
    HttpThread(const QString& url, VP1WebWatcher*ww )
      : QThread(),
	m_url(url),
	m_result(VP1WebWatcher_ResultNotReady),
	m_httpStartTime(0),
        // m_http(0),
	m_watcher(ww) {}

    //________________________________________
    ~HttpThread() { /*delete m_http;*/ }

    //________________________________________
    QString result() const { return m_result; }
    //For special values, see static variables. All other values will
    //be the value of the "last-modified" value of the http response
    //header.

    //________________________________________
    QString url() const { return m_url; }

    //________________________________________
    int httpStartTime() const { return m_httpStartTime; }

    //________________________________________
    void run()
    {
      // TODO: replace with QNetworkManager!!

      std::cout << "WARNING!! - VP1WebWatcher::run() needs to be ported to QNetworkManager. Returning..." << std::endl;

  //     QUrl qurl(m_url);
  //     if (!qurl.isValid()) {
	// m_result = VP1WebWatcher_UrlInvalid;
	// return;
  //     }
  //     m_http = new QHttp;
  //     QHttpRequestHeader header("HEAD", m_url);
  //     header.setValue("Host", qurl.host());
  //     header.setValue("User-Agent", "ATLASVP1");
  //     m_http->setHost(qurl.host());
  //     m_httpStartTime = QDateTime::currentDateTime().toTime_t();
  //     m_http->request(header);
  //     connect(m_http,SIGNAL(done(bool)),m_watcher,SLOT(httpRequestDone(bool)),Qt::QueuedConnection);
  //     exec();
  //     m_http->deleteLater();
  //     m_http = 0;
    }

    //________________________________________
    bool handleDone(bool /*error*/, QObject * /*sender*/) {

      // TODO: replace with QNetworkManager!!
      std::cout << "WARNING!! - VP1WebWatcher::handleDone() needs to be ported to QNetworkManager. Returning..." << std::endl;

  //     if (sender!=m_http)
	// return false;
  //     if (!m_http) {
	// quit();
  //      	return false;
  //     }
  //     if (error) {
	// m_result = VP1WebWatcher_httpProblems;
	// quit();
	// return true;
  //     }
  //     QHttpResponseHeader response = m_http->lastResponse();
  //     if (!response.isValid()||!response.hasKey("last-modified")) {
	// int sc = response.statusCode();
	// m_result =  (sc==404||sc==410)? VP1WebWatcher_FileNotExist : VP1WebWatcher_httpProblems;
	// quit();
	// return true;
  //     }
  //     m_result = response.value("last-modified");
  //     quit();
  //     return true;
    return true;

    }

    // TODO: replace with QNetworkManager!!
    // QHttp * http() const {
    //   return m_http;
    // }

  private:
    const QString m_url;
    QString m_result;
    unsigned m_httpStartTime;
    // QHttp * m_http;
    VP1WebWatcher* m_watcher;
  };

  //___________________________________________________________________
  Imp(VP1WebWatcher *tc,int ri) : theclass(tc), recheckInterval_ms(ri) {}
  VP1WebWatcher * theclass;
  const int recheckInterval_ms;

  class WatchedUrl {
  public:
    WatchedUrl(const QString& u) : url(u), thread(0), lastResult(VP1WebWatcher_Unknown) {}
    ~WatchedUrl() { ensureEndThread(thread); }
    QString url;
    HttpThread* thread;
    QString lastResult;
  };
  QList<WatchedUrl*> watchedUrls;


  static void ensureEndThread(HttpThread*& /*thread*/) {

    // if (!thread)
    //   return;
    // thread->blockSignals(true);
    // if (thread->http())
    //   thread->http()->blockSignals(true);
    // thread->quit();
    // if (!thread->wait(50)) {//Put 50 -> 0 to test the terminate fallback.
    //   thread->terminate();
    //   thread->wait();//Ensure synchronisation (so we can safely delete)
    // }
    // thread->deleteLater();
    // thread=0;
  }

  void startDownload(WatchedUrl* /*wu*/) {

    // ensureEndThread(wu->thread);
    // wu->thread = new Imp::HttpThread(wu->url,theclass);
    // wu->thread->start();
  }

};


//____________________________________________________________________
VP1WebWatcher::VP1WebWatcher(int recheckInterval_ms, QObject * parent)
  : QObject(parent), d(new Imp(this,recheckInterval_ms))
{
  startTimer(recheckInterval_ms);
}

//____________________________________________________________________
VP1WebWatcher::VP1WebWatcher(const QStringList& urls, int recheckInterval_ms, QObject * parent)
  : QObject(parent), d(new Imp(this,recheckInterval_ms))
{
  addUrls(urls);
  startTimer(recheckInterval_ms);
}

//____________________________________________________________________
VP1WebWatcher::~VP1WebWatcher()
{
  while(!d->watchedUrls.isEmpty())
    removeUrl(d->watchedUrls.front()->url);
  delete d;
}

//____________________________________________________________________
void VP1WebWatcher::httpRequestDone(bool error)
{
  foreach(Imp::WatchedUrl*wu, d->watchedUrls)
    if (wu->thread && wu->thread->handleDone(error, sender())) {
      bool changed = wu->lastResult != wu->thread->result();
      wu->lastResult = wu->thread->result();
      d->ensureEndThread(wu->thread);
      if (changed)
	emit urlChanged(wu->url);
      return;
    }
  //We could not find the thread with that http instance. We must
  //assume it was terminated.
}

//____________________________________________________________________
bool VP1WebWatcher::isWatchingUrl(const QString&u) const
{
  foreach (Imp::WatchedUrl*wu, d->watchedUrls) {
    if (wu->url == u)
      return true;
  }
  return false;
}

//____________________________________________________________________
void VP1WebWatcher::addUrl(const QString&/*u*/)
{
  // if (isWatchingUrl(u))
  //   return;
  // Imp::WatchedUrl * wu = new Imp::WatchedUrl(u);
  // d->watchedUrls << wu;
  // d->startDownload(wu);
}

//____________________________________________________________________
void VP1WebWatcher::addUrls(const QStringList&/*l*/)
{
  // foreach (QString u, l)
  //   addUrl(u);
}

//____________________________________________________________________
QStringList VP1WebWatcher::urls() const
{
  QStringList l;
  // foreach(Imp::WatchedUrl*wu,d->watchedUrls)
  //   l << wu->url;
  return l;
}

//____________________________________________________________________
void VP1WebWatcher::removeUrl(const QString&/*u*/)
{
  // foreach(Imp::WatchedUrl*wu,d->watchedUrls)
  //   if (wu->url == u) {
  //     d->watchedUrls.removeAll(wu);
  //     delete wu;
  //     return;
  //   }
}

//____________________________________________________________________
void VP1WebWatcher::removeUrls(const QStringList&/*l*/)
{
  // foreach (QString u, l)
  //   removeUrl(u);
}

//____________________________________________________________________
void VP1WebWatcher::timerEvent(QTimerEvent*)
{
  // const unsigned currentTime = QDateTime::currentDateTime().toTime_t();
  // foreach(Imp::WatchedUrl*wu,d->watchedUrls) {
  //   if (!wu->thread) {
  //     d->startDownload(wu);
  //   } else {
  //     //Thread is running. Check that it didn't run for too long.
  //     //
  //     //(No matter what, we never restart running thread that have been
  //     //running for less than a second, or no less than twice the
  //     //recheckInterval!)
  //     if (wu->thread->httpStartTime()>0 && currentTime - wu->thread->httpStartTime() > (unsigned(d->recheckInterval_ms) * 1000 * 2 + 1000)) {
	// d->ensureEndThread(wu->thread);
	// d->startDownload(wu);
  //     }
  //   }
  // }
}

//____________________________________________________________________
VP1WebWatcher::RESULT VP1WebWatcher::lastResult(const QString& url)
{
  foreach(Imp::WatchedUrl*wu, d->watchedUrls)
    if (wu->url==url) {
      if (wu->lastResult==VP1WebWatcher_UrlInvalid)
	return INVALID_URL;
      if (wu->lastResult==VP1WebWatcher_httpProblems)
	return CONNECTION_PROBLEMS;
      if (wu->lastResult==VP1WebWatcher_FileNotExist)
	return NOT_ON_SERVER;
      if (wu->lastResult==VP1WebWatcher_Unknown)
	return UNKNOWN;
      if (wu->lastResult==VP1WebWatcher_ResultNotReady)
	return UNKNOWN;
      return EXISTS;
    }
  return UNKNOWN;
}

//____________________________________________________________________
QString VP1WebWatcher::lastResultToString(const QString& url)
{
  switch (lastResult(url)) {
  case UNKNOWN: return "UNKNOWN";
  case INVALID_URL: return "INVALID_URL";
  case CONNECTION_PROBLEMS: return "CONNECTION_PROBLEMS";
  case NOT_ON_SERVER: return "NOT_ON_SERVER";
  case EXISTS: return "EXISTS";
  default:
    return "UNKNOWN";
  }
}

//____________________________________________________________________
QDateTime VP1WebWatcher::lastModTime(const QString& url)
{
  foreach(Imp::WatchedUrl*wu, d->watchedUrls)
    if (wu->url==url) {
      if (wu->lastResult.startsWith(VP1WebWatcher_PREFIX))
	return QDateTime();//invalid
      QString lastmod = wu->lastResult;
      lastmod = lastmod.simplified();
      lastmod.chop(4);
      QDateTime dt(QDateTime::fromString(lastmod,"ddd, dd MMM yyyy hh:mm:ss"));
      dt.setTimeSpec(Qt::UTC);
      return dt;
    }
  return QDateTime();//invalid
}
