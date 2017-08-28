/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1Gui/VP1Authenticator.h"
#include "VP1Base/VP1QtUtils.h"

#include <QNetworkAccessManager>
#include <QSslError>
#include <QNetworkCookie>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QBuffer>
#include <QStringMatcher>

#include <iostream>
#include <QMetaType>

//______________________ Imp ______________________________
class VP1Authenticator::Imp {

public:
  Imp(VP1Authenticator*,QString);
  ~Imp();

  // Gets value of the HTML tag attribute. Returns an empty string if attribute not found
  QString getTagAttributeVal(const QString& tag, const QString& attribute);

  // Connect signals of the QNetworkReply to slots of the VP1Authenticator
  void connectToAuthenticator(VP1Authenticator* authenticator);

  // Display error message in the TextEdit box
  void displayError(QString);

  VP1Authenticator* m_theClass;
  QNetworkAccessManager* m_netmanager;
  QNetworkReply* m_netreply;
  QFile* m_log;

  int stage;
  QString m_fileInfoUrl;
  QString m_loginServer;
};

VP1Authenticator::Imp::Imp(VP1Authenticator* theClass
			   ,QString fileInfoUrl)
  : m_theClass(theClass)
  , m_netmanager(new QNetworkAccessManager())
  , m_netreply(0)
  , m_log(0)
  , stage(1)
  , m_fileInfoUrl(fileInfoUrl)
  , m_loginServer("login.cern.ch")
{
  if(VP1QtUtils::environmentVariableIsOn("VP1_AUTH_ENABLELOG")) {
    QString logname("vp1live-auth.log");
    if(QFileInfo(logname).exists() && !QFile(logname).remove())
      std::cerr << "VP1Authenticator ERROR: Unable to remove old logfile" << std::endl;
    else {
      m_log = new QFile(logname);
      if(!m_log->open(QIODevice::WriteOnly)) {
	std::cerr << "VP1Authenticator ERROR: Unable to open new logfile for writing" << std::endl;
	delete m_log;
	m_log = 0;
      }
    }
  }
}

VP1Authenticator::Imp::~Imp()
{
  // deleting m_netmanager is not my responsibility :)
  delete m_netreply;

  if(m_log) {
    m_log->close();
    delete m_log;
  }
}

QString VP1Authenticator::Imp::getTagAttributeVal(const QString& tag, const QString& attribute)
{
  QStringMatcher startMatcher(attribute + "=\"",Qt::CaseInsensitive);  
  QStringMatcher endMatcher("\"");

  int startPos = startMatcher.indexIn(tag);
  if(startPos==-1) 
    return QString("");
  else {
    int endPos = endMatcher.indexIn(tag,startPos+startMatcher.pattern().size());
    if(endPos==-1) // something is wrong with this tag
      return QString("");
    else
      return tag.mid(startPos+startMatcher.pattern().size(),endPos-startPos-startMatcher.pattern().size());
  }
}

void VP1Authenticator::Imp::connectToAuthenticator(VP1Authenticator* authenticator)
{
  connect(m_netreply, SIGNAL(finished()),
	  authenticator, SLOT(finished()));
  connect(m_netreply, SIGNAL(error(QNetworkReply::NetworkError)),
	  authenticator, SLOT(error(QNetworkReply::NetworkError)));
  connect(m_netreply, SIGNAL(sslErrors(const QList<QSslError>&)),
	  authenticator, SLOT(sslErrors(const QList<QSslError>&)));
}

void VP1Authenticator::Imp::displayError(QString message)
{
  m_theClass->teditError->setVisible(true);
  m_theClass->setFixedSize(400,350);
  m_theClass->teditError->append(message);
  m_theClass->inpPhr->clear();
  m_theClass->inpPers->setEnabled(true);
  m_theClass->inpPhr->setEnabled(true);
  m_theClass->inpPhr->setFocus();
  if(m_log) {
    QByteArray logBa(QString(message+"\n").toStdString().c_str());
    m_log->write(logBa);
  }
}

//__________________________ Main Class __________________________________
VP1Authenticator::VP1Authenticator(QWidget* parent, QString fileInfoUrl)
  : QDialog(parent)
  , d(new Imp(this,fileInfoUrl))
{
  setupUi(this);
  setFixedSize(400,200);

  // Configure GUI properties
  teditError->setVisible(false);
  teditError->setLineWrapMode(QTextEdit::NoWrap);
  pbtnLogin->setDefault(true);

  // Connect GUI signals to slots
  connect(pbtnLogin,SIGNAL(clicked()),this,SLOT(loginClicked()));
  connect(pbtnCancel,SIGNAL(clicked()),this,SLOT(reject()));

  qRegisterMetaType<QList<QSslError> >("QList<QSslError>");
}

VP1Authenticator::~VP1Authenticator()
{
  delete d;
}

//____________ Https/SSL slots________________

bool VP1Authenticator::connectToServer()
{ 
  QUrl fileinfoUrl(d->m_fileInfoUrl);
  QNetworkRequest netrequest(fileinfoUrl);
  d->m_netreply = d->m_netmanager->get(netrequest);
  d->connectToAuthenticator(this);

  return true;
}

void VP1Authenticator::finished()
{
  QString message("\n\nVP1Authenticator done. STAGE " + QString::number(d->stage) + "\n");
  
  QUrl redirectionUrl;
  QList<QNetworkCookie> cookielist;

  // ******** Collect some information for the log ********

  // ** Headers
  QVariant val = d->m_netreply->header(QNetworkRequest::SetCookieHeader);
  if(val.type()==QVariant::Invalid) {
    message += QString("  No set cookies\n");
  } else if (!val.canConvert<QList<QNetworkCookie> >()){
    message += QString("  Cannot convert to the list of cookies\n");
  } else {
    cookielist = val.value<QList<QNetworkCookie> >();
    for(int ii=0; ii<cookielist.size(); ++ii) {
      const QNetworkCookie& _cookie = cookielist.at(ii);
      message += ("  Received cookie #" + QString::number(ii) + "\n");
      message += ("  *** Path:    " + _cookie.path() + "\n");
      message += ("  *** Domain:  " + _cookie.domain() + "\n");
      message += (QString("  *** Secure:  ") + (_cookie.isSecure() ? "YES" : "NO") + "\n");
      message += (QString("  *** Session: ") + (_cookie.isSessionCookie() ? "YES" : "NO") + "\n");
      message += ("  *** Name:    " + QString(_cookie.name().constData()) + "\n");
      message += ("  *** Value:   " + QString(_cookie.value().constData()) + "\n");
    }
  }

  val = d->m_netreply->header(QNetworkRequest::ContentTypeHeader);
  if(val.type()==QVariant::Invalid) {
    message += QString("  No ContentType\n");
  } else if (!val.canConvert<QString>()) {
    message += QString("  Cannot convert Content Type to String\n");
  } else {
    QString conttype = val.value<QString>();
    message += QString("  Content type: " + conttype  + "\n");
  }

  val = d->m_netreply->header(QNetworkRequest::ContentLengthHeader);
  if(val.type()==QVariant::Invalid) {
    message += QString("  No ContentLength\n");
  } else if (!val.canConvert<int>()) {
    message += QString("  Cannot convert Content Length to int\n");
  } else {
    int contlength = val.value<int>();
    message += QString("  Content Length: " + QString::number(contlength) + "\n");
  }

  val = d->m_netreply->header(QNetworkRequest::LocationHeader);
  if(val.type()==QVariant::Invalid) {
    message += QString("  No Location\n");
  } else if (!val.canConvert<QUrl>()) {
    message += QString("  Cannot convert Content Length to QUrl\n");
  } else {
    QUrl _url = val.value<QUrl>();
    message += QString("  Location URL " + _url.toString() + "\n");
  }

  // ** Attributes
  val = d->m_netreply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  if(val.type()==QVariant::Invalid) {
    message += QString("  No StatusCode Attribute\n");
  } else if (!val.canConvert<int>()) {
    message += QString("  Cannot convert StatusCode to int\n");
  } else {
    int sc = val.value<int>();
    message += QString("  StatusCode : " +QString::number(sc) + "\n");
  }

  val = d->m_netreply->attribute(QNetworkRequest::RedirectionTargetAttribute);
  if(val.type()==QVariant::Invalid) {
    message += QString("  No Redirection Attribute\n");
  } else if (!val.canConvert<QUrl>()) {
    message += QString("  Cannot convert Redirection to QUrl\n");
  } else {
    redirectionUrl = val.value<QUrl>();
    message += QString("  Redirection : " + redirectionUrl.toString() + "\n");
  }

  message += QString("\n HTML response >>>>>\n");

  QByteArray logMessage(message.toStdString().c_str());

  // ** Write reply to log **
  QByteArray ba = d->m_netreply->readAll();
  logMessage = logMessage.append(ba);
  logMessage = logMessage.append("\n<<<<< HTML response\n\n\n");
  if(d->m_log)
    d->m_log->write(logMessage);


  // *** Stage 5:
  // Final response from the server. It may happen that the
  // authentication was successfull, however the user is not
  // authorized to access the requested resource
  //
  // Check that...
  if(d->stage==5) {
    QString replyBody(ba.data());
    if(replyBody.contains("authorization failed", Qt::CaseInsensitive)) {
      QString errMessage("Authorization Failed");
      d->displayError(errMessage);
      d->m_netmanager->deleteLater();
      d->m_netmanager = new QNetworkAccessManager();
      d->stage=1;
      d->m_netreply=0;
      return;
    } 
    
    if(d->m_netreply->error()==QNetworkReply::NoError) 
      authenticationSuccessful(d->m_netmanager);
  }

  // ** Check for errors **
  if(d->m_netreply->error()!=QNetworkReply::NoError) {
      QString errMessage("Network error occured during authentication phase\n");
      errMessage += QString("Error code " + QString::number((int)d->m_netreply->error()) + "\nExplanation on http://doc.trolltech.com/4.4/qnetworkreply.html#NetworkError-enum");
      d->displayError(errMessage);
      d->stage=1;
      return;
  }

  // *** Stage 1:
  // Received a reply redirecting us to login.cern.ch
  // Take the redirection URL and issue GET request for it
  if(d->stage==1) {
    if(redirectionUrl.isEmpty() || redirectionUrl.host() != d->m_loginServer) {
      QString errMessage("Wrong URL: " + d->m_fileInfoUrl + "\nPlease fix the URL and restart the job");
      d->displayError(errMessage);	
      d->stage=1;
      return;
    } else {
      QNetworkRequest netrequest(redirectionUrl);
      d->m_netreply = d->m_netmanager->get(netrequest);
      if(d->m_log) {
	QByteArray baLog("Get request sent\n_______________________________________________________\n\n");
	d->m_log->write(baLog);
      }
      d->connectToAuthenticator(this);
      d->stage++;
      return;
    }
  }

  // *** Stage 2:
  // Received authentication form
  // Parse contents of the authentication form
  // look for input tags and collect their attributes
  if(d->stage==2) {
    QString replyBody(ba.data());

    QString newRequestBody("__EVENTTARGET=&__EVENTARGUMENT=&__LASTFOCUS=");

    QStringMatcher inputStartMatcher("<input ",Qt::CaseInsensitive);
    QStringMatcher inputEndMatcher("/>");

    int inputStart = inputStartMatcher.indexIn(replyBody,0);

    while(inputStart!=-1) {
      int inputEnd = inputEndMatcher.indexIn(replyBody,inputStart+inputStartMatcher.pattern().size());
      if(inputEnd==-1) // something is wrong with this tag
	break;
      else {
	// Let's parse it
	QString tag = replyBody.mid(inputStart,inputEnd-inputStart);
	QString typeVal = d->getTagAttributeVal(tag,"type");
	QString nameVal = d->getTagAttributeVal(tag,"name");
	QString valueVal = d->getTagAttributeVal(tag,"value").replace(" ","+");
	
	if(QString::compare(typeVal,"text",Qt::CaseInsensitive)==0)
	  valueVal = inpPers->text();
	else if(QString::compare(typeVal,"password",Qt::CaseInsensitive)==0)
	  valueVal = inpPhr->text();

	if(QString::compare(typeVal,"checkbox",Qt::CaseInsensitive)!=0) {
	  QByteArray encodedNameVal = QUrl::toPercentEncoding(nameVal);
	  if(QString::compare(typeVal,"submit",Qt::CaseInsensitive)==0 
	     || QString::compare(typeVal,"text",Qt::CaseInsensitive)==0
	     || QString::compare(typeVal,"password",Qt::CaseInsensitive)==0 ) {
	    newRequestBody+=("&"+QString(encodedNameVal)+"="+valueVal);
	  } else {
	    QByteArray encodedValueVal = QUrl::toPercentEncoding(valueVal);
	    if(newRequestBody.size()!=0)
	      newRequestBody+="&";
	    newRequestBody+=(QString(encodedNameVal)+"="+QString(encodedValueVal));
	  }
	}

	// move to the next input
	inputStart = inputStartMatcher.indexIn(replyBody,inputEnd+inputEndMatcher.pattern().size());
      }
    }

    QByteArray newBody;
    newBody=newBody.insert(0,newRequestBody);

    QString logMessage = "New Request Length: " + QString::number(newBody.size()) + "\n";
    logMessage += ("New Request Body:\n" + newRequestBody.replace(inpPhr->text(),"xxx") + "\n");

    // Get form action
    // !!! Hardwire this for now:
    QString actionUrlString("https://"+d->m_loginServer);

    QStringMatcher actionStartMatcher("action=\"",Qt::CaseInsensitive);
    QStringMatcher actionEndMatcher("\"");
    int actionStart = actionStartMatcher.indexIn(replyBody,0);
    if(actionStart!=-1) {
      int actionEnd = actionEndMatcher.indexIn(replyBody,actionStart+actionStartMatcher.pattern().size());
      if(actionEnd!=-1)
	actionUrlString += replyBody.mid(actionStart+actionStartMatcher.pattern().size(),actionEnd-actionStart-actionStartMatcher.pattern().size());
    }

    logMessage += ("New URL: " + actionUrlString + "\n");
    QByteArray actionUrlBa(actionUrlString.toStdString().c_str());
    QString actionUrlStringDecoded = QUrl::fromPercentEncoding(actionUrlBa);
    logMessage += ("Decoded URL: " + actionUrlStringDecoded.replace("&amp;","&") 
		   + "\n\nPost request sent\n_______________________________________________________\n\n");

    // Send Post request:
    QNetworkRequest netrequest(QUrl(actionUrlStringDecoded.replace("&amp;","&")));
    netrequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    netrequest.setHeader(QNetworkRequest::ContentLengthHeader,newBody.size());
    d->m_netreply = d->m_netmanager->post(netrequest,newBody);
    if(d->m_log) {
      QByteArray baLog(logMessage.toStdString().c_str());
      d->m_log->write(baLog);
    }
    d->connectToAuthenticator(this);
    d->stage++;
    return;
  }

  // *** Stage 3:
  // Received response from the autentication attempt
  //
  // Check if the authentication was successful. This can be done 
  // either of these two ways:
  //   1. Check number of set cookies, if it = 0 then authentication failed
  //   2. Look for 'Logon failure' string in the response body
  // We implement the option #1
  //
  // If the authentication considered successfull then parse contents of the 
  // response, look for input tags and collect their attributes
  // and compose a new POST request
  if(d->stage==3) {

    if(cookielist.size()==0) {
      // Authentication failed
      QString errMessage("Authentication failed, please try again\n");
      d->displayError(errMessage);
      d->stage=1;
      return;
    }

    QString replyBody(ba.data());
    QByteArray excludeFromEncoding(" ");
    QString newRequestBody("");

    QStringMatcher inputStartMatcher("<input ",Qt::CaseInsensitive);
    QStringMatcher inputEndMatcher("/>");

    int inputStart = inputStartMatcher.indexIn(replyBody,0);

    while(inputStart!=-1) {

      // It can happen that /> is in the middle of some string, for example it can
      // be part of value="...". We need to take this into account as well      
      int inputEnd = inputStart;
      int quoteCount = 1; // just to be able to enter the next loop
      int quotePos = inputStart;
      QStringMatcher quoteMatcher("\"");

      while(quoteCount%2!=0) {
	inputEnd = inputEndMatcher.indexIn(replyBody,quotePos);
	if(inputEnd==-1) 
	  break;
	quoteCount = 0;
	quotePos = inputStart;

	while(true) {
	  quotePos = quoteMatcher.indexIn(replyBody,quotePos);
	  if(quotePos==-1||quotePos>inputEnd)
	    break;
	  quoteCount++;
	  quotePos++;
	}
      }

      if(inputEnd==-1) // something is wrong with this tag
	break;
      else {
	// Let's parse it
	QString tag = replyBody.mid(inputStart,inputEnd-inputStart);
	QString typeVal = d->getTagAttributeVal(tag,"type");
	QString nameVal = d->getTagAttributeVal(tag,"name");
	QString valueVal = d->getTagAttributeVal(tag,"value");

	if(QString::compare(typeVal,"text",Qt::CaseInsensitive)==0)
	  valueVal = inpPers->text();
	else if(QString::compare(typeVal,"password",Qt::CaseInsensitive)==0)
	  valueVal = inpPhr->text();

	if(QString::compare(typeVal,"checkbox",Qt::CaseInsensitive)!=0 &&
	   QString::compare(typeVal,"submit",Qt::CaseInsensitive)!=0) {
	  QByteArray encodedNameVal = QUrl::toPercentEncoding(nameVal);
	  QString valueVal1 = valueVal.replace("&lt;","<");
	  QString valueVal2 = valueVal.replace("&quot;","\"");
	  QByteArray encodedValueVal = QUrl::toPercentEncoding(valueVal2,excludeFromEncoding);
	  if(newRequestBody.size()!=0)
	    newRequestBody+="&";
	  newRequestBody+=(QString(encodedNameVal)+"="+QString(encodedValueVal).replace(" ","+"));
	}

	// move to the next input
	inputStart = inputStartMatcher.indexIn(replyBody,inputEnd+inputEndMatcher.pattern().size());
      }
    }

    QByteArray newBody;
    newBody=newBody.insert(0,newRequestBody);

    QString logMessage = "New Request Length: " + QString::number(newBody.size()) + "\n";
    logMessage += ("New Request Body:\n" + newRequestBody + "\n");
    
    // Get form action
    QString actionUrlString("");

    QStringMatcher actionStartMatcher("action=\"",Qt::CaseInsensitive);
    QStringMatcher actionEndMatcher("\"");
    int actionStart = actionStartMatcher.indexIn(replyBody,0);
    if(actionStart!=-1) {
      int actionEnd = actionEndMatcher.indexIn(replyBody,actionStart+actionStartMatcher.pattern().size());
      if(actionEnd!=-1)
	actionUrlString = replyBody.mid(actionStart+actionStartMatcher.pattern().size(),actionEnd-actionStart-actionStartMatcher.pattern().size());
    }

    logMessage += ("New URL: " + actionUrlString + "\n");

    // Send Post request:
    QNetworkRequest netrequest;
    netrequest.setUrl(actionUrlString);
    netrequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    netrequest.setHeader(QNetworkRequest::ContentLengthHeader,newBody.size());
    d->m_netreply = d->m_netmanager->post(netrequest,newBody);
    logMessage += ("\n\nPost request sent\n_______________________________________________________\n\n");
    if(d->m_log) {
      QByteArray baLog(logMessage.toStdString().c_str());
      d->m_log->write(baLog);
    }
    d->connectToAuthenticator(this);
    d->stage++;
    return;
  }

  // *** Stage 4:
  // Recieved a final redirection to the requested resource
  // Just try to get it
  if(d->stage==4) {
    QNetworkRequest netrequest(redirectionUrl);
    d->m_netreply = d->m_netmanager->get(netrequest);
    if(d->m_log) {
      QByteArray baLog("Get request sent\n_______________________________________________________\n\n");
      d->m_log->write(baLog);
    }
    d->connectToAuthenticator(this);
    d->stage++;
    return;
  }
}

void VP1Authenticator::error(QNetworkReply::NetworkError err)
{
  if(d->m_log) {
    QString message("VP1Authenticator error. STAGE " + QString::number(d->stage) + ", error code: " + QString::number((int)err) + "\n");
    QByteArray ba(message.toStdString().c_str());
    d->m_log->write(ba);
  }
}

void VP1Authenticator::sslErrors(const QList<QSslError>& errlist) 
{
  if(d->m_log) {
    QString message("VP1Authenticator SSL errors. STAGE " + QString::number(d->stage) + "\n");
    for(int ii=0; ii<errlist.size(); ++ii)
      message += ("   " + QString::number((int)errlist.at(ii).error()) + ", " + errlist.at(ii).errorString() + "\n");
    QByteArray ba(message.toStdString().c_str());
    d->m_log->write(ba);
  }
  d->m_netreply->ignoreSslErrors();
}

//____________ GUI slots________________
void VP1Authenticator::loginClicked()
{
  // Hide error box, if visible
  if(teditError->isVisible()) {
    teditError->setVisible(false);
    setFixedSize(400,200);
    teditError->clear();
  } 

  // Set focus on the login box, if empty
  if(inpPers->text().isEmpty()) {
    inpPers->setFocus();
    return;
  }

  // Set focus on the pass box, if empty
  if(inpPhr->text().isEmpty()) {
    inpPhr->setFocus();
    return;
  }

  inpPers->setEnabled(false);
  inpPhr->setEnabled(false);
  connectToServer();
}

QNetworkAccessManager* VP1Authenticator::networkAccessManager()
{
  return d->m_netmanager;
}
