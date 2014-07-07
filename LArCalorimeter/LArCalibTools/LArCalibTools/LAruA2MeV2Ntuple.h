//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** This algrithm produces a column-wise NTuple
    out of a LAruA2MeVDB. Only the finalize
    method is used, initalize and execute are empty.
    The key of the object is given by the jobOption 
    'ContainerKey'. 
   * @author Walter Lampl
   * 11. 8. 2005 

*/

#ifndef LARUA2MEV2NTUPLE_H
#define LARUA2MEV2NTUPLE_H
#include "LArCalibTools/LArCond2NtupleBase.h"

class LAruA2MeV2Ntuple : public LArCond2NtupleBase
{
 public:
  LAruA2MeV2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LAruA2MeV2Ntuple();

  //standard algorithm methods
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:

  std::string m_uA2MeVKey, m_DAC2uAKey;
 
};

#endif
