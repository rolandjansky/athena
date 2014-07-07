//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** This algrithm produces a column-wise NTuple
    out of a LArMphysOverMcalDB. Only the finalize
    method is used, initalize and execute are empty.
    The key of the container is given by the jobOption 
    'ContainerKey'. 
   * @author R. Lafaye
   * 04. 3. 2004


*/

#ifndef LARMPHYSOVERMCAL2NTUPLE_H
#define LARMPHYSOVERMCAL2NTUPLE_H

#include "LArCalibTools/LArCond2NtupleBase.h"


class LArMphysOverMcal2Ntuple : public LArCond2NtupleBase
{
 public:
  LArMphysOverMcal2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArMphysOverMcal2Ntuple();

  //standard algorithm methods
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:

  std::string m_contKey;
  bool m_isMC;
};

#endif
