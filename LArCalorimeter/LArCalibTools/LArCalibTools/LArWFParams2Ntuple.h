//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** This algorithm produces a column-wise NTuple
    out of a LArWFParamsContainer. Only the finalize
    method is used, initalize and execute are empty.
    The key of the container is given by the jobOption 
    'ContainerKey'. 
   * @author M. Fanti
   * 27. 2. 2004


*/

#ifndef LARWFPARAMS2NTUPLE_H
#define LARWFPARAMS2NTUPLE_H

#include "LArCalibTools/LArCond2NtupleBase.h"

class LArWFParams2Ntuple : public LArCond2NtupleBase
{
 public:
  LArWFParams2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArWFParams2Ntuple();

  //standard algorithm methods
  StatusCode  initialize();
  StatusCode execute() {return StatusCode::SUCCESS;}
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

 private:

  // bool m_useContainers , m_useComplete ,
  bool  m_dumpCaliPulseParams , m_dumpDetCellParams , m_dumpPhysCaliTdiff , m_dumpTdrift, m_dumpOFCBin ;
  std::string m_caliPulseParamsKey, m_detCellParamsKey, m_physCaliTDiffKey, m_tdriftKey, m_ofcBinKey;

};

#endif
