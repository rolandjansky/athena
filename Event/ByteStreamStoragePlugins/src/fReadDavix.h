/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FREADDAVIX_H
#define FREADDAVIX_H

#include "CxxUtils/checker_macros.h"
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
  int  m_pfd; // current file, used as bool to check if file is open
  int64_t m_offset;
  Davix::Context m_c;
  Davix::RequestParams *m_davixParam;
  Davix::DavixError* m_err;
  Davix::DavPosix *m_pos ATLAS_THREAD_SAFE;  // Davix documentation says this is thread-safe
  Davix::X509Credential *m_cert;
  DAVIX_FD* m_fd; // davix pointer to current file

};

#endif
