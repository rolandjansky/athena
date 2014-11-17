/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//######################################################
//#                                                    #
//# File   : CBNT_Calo_H6.h                            #
//# Package: TBRec                                     #
//#                                                    #
//# Author : Marco Bieri <mbieri@sfu.ca>               #
//# Adopted for CBNTAA Pavol Strizenec                 #
//#                                                    #
//######################################################

#ifndef CBNT_CALOH6_H
#define CBNT_CALOH6_H

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ToolHandle.h"
#include "CBNT_TBRecBase.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloID.h"
// #include "LArDetDescr/LArDetDescrManager.h"

// NOISE
class ICaloNoiseTool;

#include <fstream>
#include <string>
#include <vector>

class StoreGateSvc;

class CBNT_CaloH6 : public CBNT_TBRecBase
{
 public:
  CBNT_CaloH6(const std::string & name, ISvcLocator * pSvcLocator);
  
  ~CBNT_CaloH6();
  
  //standart algorithm methods
  virtual StatusCode CBNT_initialize();
  virtual StatusCode CBNT_execute();
  virtual StatusCode CBNT_finalize();
  virtual StatusCode CBNT_clear();
  
 private:
  // LArDetDescrManager* m_larMgr;

  // Identifier Pointers
  const LArEM_ID* m_emecID_help;
  const LArHEC_ID* m_hecID_help;
  const LArFCAL_ID* m_fcalID_help;

  // noise tool pointer
  ToolHandle<ICaloNoiseTool> m_noiseTool;
  
  float m_noiseSigma;
  
  // picking detector and number of channels
  std::string m_det_type;
  int m_numchan;

  /*-------------------Ntuple--------------------------------*/
  
  std::vector<float> *m_energy;
  std::vector<float> *m_NoiseRMS;
  std::vector<float> *m_ieta;
  std::vector<float> *m_iphi;
  std::vector<float> *m_eta;
  std::vector<float> *m_phi;
  std::vector<float> *m_sampling;
  std::vector<float> *m_region;

};                                                                      
#endif
