/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCCRAWDATACONTAINER_P1_H
#define MUON_CSCCRAWDATACONTAINER_P1_H

/*

Persistent represenation of a container of collections of CSC Raw Hits,
Author: Marcin Nowak
        CERN, December, 2005

*/

#include "MuonEventAthenaPool/CscRawDataCollection_p1.h"


class CscRawDataContainer_p1
   : public std::vector<CscRawDataCollection_p1>
{
public:
  // Default constructor 
  CscRawDataContainer_p1() {}

  //typedef  std::vector<CscRawDataCollection_p1>	BASE;
  //typedef  BASE:const_iterator			const_iterator;

  //friend class CscRawDataContainerCnv_p1;
  
/*
  void		fillTransient(CscRawDataContainer* transObj, MsgStream &log) const;
  void		fillPersistent(const CscRawDataContainer* transObj, MsgStream &log);

  CscRawDataContainer*		createTransient(MsgStream &log) const;  
  static CscRawDataContainer_p1* createPersistent(const CscRawDataContainer* transObj, MsgStream &log);
*/
};


#endif




