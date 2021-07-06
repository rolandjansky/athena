/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#ifndef TBREC_CBNT_CALOH6_H
#define TBREC_CBNT_CALOH6_H

#include "GaudiKernel/ToolHandle.h"
#include "CBNT_TBRecBase.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloID.h"
// #include "LArDetDescr/LArDetDescrManager.h"

// NOISE
#include "CaloConditions/CaloNoise.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <fstream>
#include <string>
#include <vector>

class CBNT_CaloH6 : public CBNT_TBRecBase
{
 public:
  CBNT_CaloH6(const std::string & name, ISvcLocator * pSvcLocator);
  
  virtual ~CBNT_CaloH6();
  
  //standart algorithm methods
  virtual StatusCode CBNT_initialize() override;
  virtual StatusCode CBNT_execute() override;
  virtual StatusCode CBNT_finalize() override;
  virtual StatusCode CBNT_clear() override;
  
 private:
  // LArDetDescrManager* m_larMgr;

  // Identifier Pointers
  const LArEM_ID* m_emecID_help;
  const LArHEC_ID* m_hecID_help;
  const LArFCAL_ID* m_fcalID_help;

  SG::ReadCondHandleKey<CaloNoise> m_elecNoiseKey
    { this, "ElecNoiseKey", "electronicNoise", "SG key for electronic noise" };
  
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
