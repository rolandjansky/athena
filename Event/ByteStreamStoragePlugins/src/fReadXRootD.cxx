/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ers/ers.h"
#include <fcntl.h>

#include "fReadXRootD.h"
#include "EventStorage/EventStorageIssues.h"

// external XRootD functions from ROOT net/xrootd/src/xrootd/src/XrdPosix/XrdPosixXrootd.hh
class XrdPosixCallBack;
class XrdPosixXrootd {
public:
  static int Open(const char *path, int oflag, mode_t mode=0, XrdPosixCallBack *cbP=0);
  static int Close(int fildes);
  static size_t Read(int fildes, void *buf, size_t nbyte);
  static off_t Lseek(int fildes, off_t offset, int whence);
};

fReadXRootD::fReadXRootD()
{
  m_pfd = 0;
}

fReadXRootD::~fReadXRootD()
{
  this->closeFile();
}

bool fReadXRootD::isOpen()
{
  return m_pfd != 0;
}

bool fReadXRootD::isEoF()
{
//xrd eof??
  return false;
}

bool fReadXRootD::fileExists(std::string fName) const
{
  int pfd = XrdPosixXrootd::Open(fName.c_str(), O_RDONLY);
  if(pfd == 0) return false;
  XrdPosixXrootd::Close(pfd);
  return true;
}

void fReadXRootD::openFile(std::string fName)
{
  if(this->isOpen()) this->closeFile();
  m_pfd = XrdPosixXrootd::Open(fName.c_str(), O_RDONLY);
}

void fReadXRootD::closeFile()
{
  if(m_pfd != 0) XrdPosixXrootd::Close(m_pfd);
  m_pfd = 0;
}

void fReadXRootD::readData(char *buffer, unsigned int sizeBytes)
{
  if (sizeBytes==0) return;
  if(this->isOpen())
  {
    unsigned int totalRead=0,ntry=0;
    while(sizeBytes > totalRead)
    {
      int ret = XrdPosixXrootd::Read(m_pfd,buffer,sizeBytes);
      totalRead += ret; ++ntry;
      if(ntry>5) {
        std::stringstream mystream;
        mystream << "Problem reading from the data file. "
                    <<"fReadXRootD::readData asked to read "<<sizeBytes
                    <<" bytes and managed to read only "<<totalRead
                    <<" bytes.";
        EventStorage::ReadingIssue ci(ERS_HERE, mystream.str().c_str());
        ers::warning(ci);
        return;
      }
    }
  }
}

int64_t fReadXRootD::getPosition()
{
  if(this->isOpen()) return  XrdPosixXrootd::Lseek(m_pfd, 0, SEEK_CUR);
  return -1;
}

void fReadXRootD::setPosition(int64_t p)
{
  if(this->isOpen()) XrdPosixXrootd::Lseek(m_pfd, (long long)p, SEEK_SET);
}

void fReadXRootD::setPositionFromEnd(int64_t p)
{
  if(this->isOpen()) XrdPosixXrootd::Lseek(m_pfd, (long long)p, SEEK_END);
}

fRead * fReadXRootD::newReader() const
{
  fReadXRootD * nfr = new fReadXRootD();
  return (fRead *)nfr;
}

extern "C" {
  fRead * fReadFactory()
  {
    fReadXRootD * nfr = new fReadXRootD();
    return (fRead *)nfr;
  }
}
