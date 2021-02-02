//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


/* 
    
    This algorithm produces a column-wise NTuple out of a LArOFCContainer. 
    Only the finalize method is used, initalize and execute are empty. 
    The key of the container is given by the jobOption  'ContainerKey'.

   * @author M. Delmastro
   * 30. 3. 2005

*/

#ifndef LAROFC2NTUPLE_H
#define LAROFC2NTUPLE_H
#include "LArCalibTools/LArCond2NtupleBase.h"
#include "LArElecCalib/ILArOFCTool.h"

class LArOFC2Ntuple : public LArCond2NtupleBase {
 public:

  LArOFC2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArOFC2Ntuple();

  //standard algorithm methods
  StatusCode initialize();
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

 private:
   
  unsigned m_nSamples;// , m_nPhases ;
   std::string m_contKey;
   std::string m_ntName;
   std::string m_ntFile;   
   bool m_isMC;
   ToolHandle<ILArOFCTool> m_OFCTool;
};

#endif
