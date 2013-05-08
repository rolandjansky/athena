/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FINDFACTORS_H
#define FINDFACTORS_H

///////////////////////////////////////////////////////
//                                                   //
// Name:   TauTools/FindFactors.h                    //
// Author: Michel Trottier-McDonald                  //
//                                                   //
// Created: June 2009                                //
//                                                   //
// Description:                                      //
// Athena Algorithm to derive the calibration factors//
//                                                   //
///////////////////////////////////////////////////////
#include "TauTools/TauCalibrationTools.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include "TFile.h"


//*********************************
// Class declaration
//*********************************

class FindFactors: public AthAlgorithm
{
public:

  FindFactors(const std::string& name, ISvcLocator *pSvcLocator);
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  TauCalibrationTools tools;

  std::string m_TruthParticleContainerName;
  std::string m_TauJetContainerName;
  std::string m_ROOTFileName;
  
  float m_DeltaRMatchCut;
    
  TFile* rootfile;
  
};

#endif
