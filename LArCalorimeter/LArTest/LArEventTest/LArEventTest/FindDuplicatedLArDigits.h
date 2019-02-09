//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/** This algrithm produces a column-wise NTuple
    out of raw data. Only the finalize
    method is used, initalize and execute are empty.
    The key of the container is given by the jobOption 
    'ContainerKey' and only a maximum of 'NSamples' 
    will be written to the ntuple.
   * @author R. Lafaye
   * 19. 1. 2005

 
*/

#ifndef FINDDUPLICATEDLARDIGITS_H
#define FINDDUPLICATEDLARDIGITS_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include  "StoreGate/StoreGateSvc.h"
#include "LArIdentifier/LArOnlineID.h"
#include <string>
#include <map>

#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"

#include <bitset>

class FindDuplicatedLArDigits : public AthAlgorithm
{
 public:
  FindDuplicatedLArDigits(const std::string & name, ISvcLocator * pSvcLocator);
  ~FindDuplicatedLArDigits();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 private:

  std::string m_contKey;
  std::bitset<200000> m_bitpattern;
  unsigned m_nProblemEvent;
  int m_nDigits;
  const LArOnlineID *m_onlineHelper;
  
  const LArEM_ID    *m_emId;
  const LArHEC_ID   *m_hecId;
  const LArFCAL_ID  *m_fcalId;
};

#endif
