/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LAROFCBINP
#define LARRAWCONDITIONS_LAROFCBINP

class LArOFCBinP {
 public:
  LArOFCBinP() : m_bin(ERRORCODE) {}
  LArOFCBinP(int b) : m_bin(b) {}
  bool isEmpty() const {return (m_bin==ERRORCODE);};
  int m_bin;
  enum {ERRORCODE=-999}; 
};

#endif
