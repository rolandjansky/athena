/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FREADXROOTD_H
#define FREADXROOTD_H

#include "EventStorage/fRead.h"

class fReadXRootD : public fRead
{
 public:
  fReadXRootD();
  ~fReadXRootD();

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
};

#endif
