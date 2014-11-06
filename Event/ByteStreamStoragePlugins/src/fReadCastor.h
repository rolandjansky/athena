/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FREADCASTOR_H
#define FREADCASTOR_H

#include "EventStorage/fRead.h"

#include <stdio.h>

class fReadCastor : public fRead
{
 public:
  fReadCastor(); 
  ~fReadCastor();

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
  FILE *m_pfd; // current file
};

#endif 

