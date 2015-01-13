//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/* 
    
    This algorithm produces a column-wise NTuple out of a LArShapeContainer. 
    Only the finalize method is used, initalize and execute are empty. 
    The key of the container is given by the jobOption  'ContainerKey'.

   * @author M. Delmastro
   * 19. 6. 2008

*/

#ifndef LARShape2NTUPLE_H
#define LARShape2NTUPLE_H
#include "LArCalibTools/LArCond2NtupleBase.h"

class LArShape2Ntuple : public LArCond2NtupleBase {
 public:

  LArShape2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArShape2Ntuple();

  //standard algorithm methods
  StatusCode initialize();
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
  

 private:
   std::string m_contKey;
   std::string m_ntName;
   std::string m_ntFile;
   bool m_isComplete;
   
};

#endif
