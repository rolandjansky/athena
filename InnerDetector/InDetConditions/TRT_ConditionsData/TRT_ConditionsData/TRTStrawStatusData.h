/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////
// TRTStrawStatusData.h                       
//Ashfaq Ahmad 17/02/2005
/////////////////////////////////

#ifndef TRTSTRAWSTATUSDATA_H
#define TRTSTRAWSTATUSDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include "Identifier/Identifier.h"
#include <map>

class TRTStrawStatusData{

public:
  //constructor
  TRTStrawStatusData();

  //destructor
  virtual ~TRTStrawStatusData();

   void setStatus(const Identifier, const int);
  int findStatus(const Identifier) const;
  //should return vector of masked/noisy/dead channels according to 
  //the status selected (int status is, masked/noisy/dead = 1/2/3)

  std::vector<Identifier> scan_entries(int ) const;

private:

  typedef std::map<Identifier, int> statusMap;
 statusMap m_statMap;


};

CLASS_DEF( TRTStrawStatusData , 247680275 , 1 )

#endif  //TRTSTRAWSTATUSDATA_H

