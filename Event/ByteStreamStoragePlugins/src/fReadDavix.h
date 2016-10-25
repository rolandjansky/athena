/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FREADDAVIX_H
#define FREADDAVIX_H

#include "EventStorage/fRead.h"
#include "davix.hpp"

class fReadDavix : public fRead
{
 public:
  fReadDavix();
  ~fReadDavix();

  bool isOpen();
  bool isEoF();
  bool fileExists(std::string fName) const;
  void openFile(std::string fName);
  void closeFile();
  void readData(char *buffer, unsigned int sizeBytes);
  int64_t getPosition();
  void setPosition(int64_t p);
  void setPositionFromEnd(int64_t p);
  fRead * newReader() const;

 private:
  int  m_pfd; // current file
  int64_t fOffset;
  Davix::Context c;
  Davix::RequestParams *davixParam;
  Davix::DavixError* err;
  Davix::DavPosix *pos;
  Davix::X509Credential *cert;
  DAVIX_FD* fd; 

};

#endif
