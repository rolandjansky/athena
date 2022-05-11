/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////
// TRTStrawStatusData.h                       
//Ashfaq Ahmad 17/02/2005
/////////////////////////////////

#ifndef TRTSTRAWSTATUSDATA_H
#define TRTSTRAWSTATUSDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include <vector>

class TRTStrawStatusData{

public:
  //constructor
  TRTStrawStatusData();

  //destructor
  virtual ~TRTStrawStatusData();

  void setStatus(const IdentifierHash& hashID, const int);
  int  findStatus(const IdentifierHash& hashID) const;
  int  size() const { return m_status.size();}

private:

  std::vector<int> m_status;
  
};

CLASS_DEF( TRTStrawStatusData , 247680275 , 1 )

#endif  //TRTSTRAWSTATUSDATA_H

