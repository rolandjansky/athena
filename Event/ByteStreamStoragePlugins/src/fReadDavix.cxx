/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ers/ers.h"
#include <fcntl.h>

#include "fReadDavix.h"
#include "EventStorage/EventStorageIssues.h"

// http://dmc-docs.web.cern.ch/dmc-docs/docs/davix-epel/html/lib-examples.html
// https://root.cern.ch/doc/master/TDavixFile_8cxx_source.html

static int TDavixFile_http_authn_cert_X509(void *userdata, const Davix::SessionInfo &info, Davix::X509Credential *cert, Davix::DavixError **err) {

  (void) userdata; // keep quiete compilation warnings
  (void) info;
  // user proxy
  std::string ucert, ukey;
  if ( std::getenv("X509_USER_PROXY")) {
    ucert = ukey = std::getenv("X509_USER_PROXY");
  }
  
  if (ucert.empty() || ukey.empty()) {
    Davix::DavixError::setupError(err, "fReadDavix.cxx",
				  Davix::StatusCode::AuthentificationError,
				  "Could not set the user's proxy or certificate");
    return -1;
  }
  return cert->loadFromFilePEM(ukey, ucert, "", err);
}

fReadDavix::fReadDavix()
{
  m_pfd = 0;
  m_offset = 0;
  m_fd = nullptr;

  m_davixParam = new Davix::RequestParams();
  m_err = NULL;
  m_pos = new Davix::DavPosix(&m_c);
  //m_pos = &m_c;


  // enableGridMode
  const char *env_var = NULL;
  if( ( env_var = std::getenv("X509_CERT_DIR")) == NULL){
    env_var = "/etc/grid-security/certificates/";
  }
  m_davixParam->addCertificateAuthorityPath(env_var);
  m_davixParam->setTransparentRedirectionSupport(true);
  m_cert = new Davix::X509Credential();
  m_davixParam->setClientCertCallbackX509(&TDavixFile_http_authn_cert_X509, NULL);

}

fReadDavix::~fReadDavix()
{
  this->closeFile();
}

bool fReadDavix::isOpen()
{
  return m_pfd != 0;
}

bool fReadDavix::isEoF()
{

  return false;
}

bool fReadDavix::fileExists(std::string fName) const
{
  Davix::DavixError* err2 = NULL;
  DAVIX_FD* pfd = m_pos->open(m_davixParam, fName.c_str(), O_RDONLY, &err2);
  if(pfd == 0) return false;
  m_pos->close(pfd, &err2);
  return true;
}

void fReadDavix::openFile(std::string fName)
{
  if(this->isOpen()) this->closeFile();
  m_fd = m_pos->open(m_davixParam, fName.c_str(), O_RDONLY, &m_err);
  m_offset = 0;
  m_pfd = 1;
}

void fReadDavix::closeFile()
{
  if(m_pfd != 0) m_pos->close(m_fd, &m_err);
  m_pfd = 0;
}

void fReadDavix::readData(char *buffer, unsigned int sizeBytes)
{
  if (sizeBytes==0) return;
  if(this->isOpen())
  {
    unsigned int totalRead=0,ntry=0;
    while(sizeBytes > totalRead)
    {
      ssize_t ret = m_pos->pread(m_fd, buffer, sizeBytes, m_offset, &m_err);
      if (ret < 0) {
        std::stringstream mystream;
	mystream << "fReadDavix::readData: can not read data with davix " << m_err->getErrMsg().c_str() << " "  << m_err->getStatus();
	Davix::DavixError::clearError(&m_err);
        EventStorage::ReadingIssue ci(ERS_HERE, mystream.str().c_str());
        ers::warning(ci);
        return;
      } else {
	m_offset += ret;
      }
      totalRead += ret; ++ntry;
      if(ntry>5) {
        std::stringstream mystream;
        mystream << "Problem reading from the data file. "
                    <<"fReadDavix::readData asked to read "<<sizeBytes
                    <<" bytes and managed to read only "<<totalRead
                    <<" bytes.";
        EventStorage::ReadingIssue ci(ERS_HERE, mystream.str().c_str());
        ers::warning(ci);
        return;
      }
    }
  }
}

int64_t fReadDavix::getPosition()
{
  if(this->isOpen()) return m_offset;
 
  return -1;
}

void fReadDavix::setPosition(int64_t p)
{
  if(this->isOpen()) m_offset = p;
}

void fReadDavix::setPositionFromEnd(int64_t p)
{
  dav_off_t ret;
  if(this->isOpen()) {
    ret = m_pos->lseek64(m_fd, p, SEEK_END, &m_err);
    m_offset = ret;
  }
}

fRead * fReadDavix::newReader() const
{
  fReadDavix * nfr = new fReadDavix();
  return (fRead *)nfr;
}

extern "C" {
  fRead * fReadFactory()
  {
    fReadDavix * nfr = new fReadDavix();
    return (fRead *)nfr;
  }
}
