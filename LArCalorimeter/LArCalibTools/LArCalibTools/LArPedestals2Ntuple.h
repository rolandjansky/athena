//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** This algrithm produces a column-wise NTuple
    out of a LArPedestalDB. Only the finalize
    method is used, initalize and execute are empty.
    The key of the container is given by the jobOption 
    'ContainerKey'. 
   * @author R. Lafaye
   * 04. 3. 2004


*/

#ifndef LARPEDESTALS2NTUPLE_H
#define LARPEDESTALS2NTUPLE_H

#include "LArCalibTools/LArCond2NtupleBase.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArElecCalib/ILArPedestal.h"

class LArPedestals2Ntuple : public LArCond2NtupleBase
{
 public:
  LArPedestals2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArPedestals2Ntuple();

  //standard algorithm methods
  virtual StatusCode stop();
  StatusCode initialize();
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:

  SG::ReadCondHandleKey<ILArPedestal> m_pedKey;
};

#endif
