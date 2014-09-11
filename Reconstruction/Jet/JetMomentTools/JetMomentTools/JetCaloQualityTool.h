// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**  
     @class JetCaloQualityTool
     Calculates calorimeter based variables for jet quality 
     @author Nikola Makovec
     @author P-A Delsart 
     @date (first implementation) October , 2009
     @date (run 2 implementation) February , 2014
*/

#ifndef JETREC_JETCALOQUALITYTOOL_H
#define JETREC_JETCALOQUALITYTOOL_H


#include "JetRec/JetModifierBase.h"

#include "JetUtils/JetCaloCalculations.h"

#include <vector>
#include <string>


class JetCaloQualityTool: public JetModifierBase {
  ASG_TOOL_CLASS0(JetCaloQualityTool);

public:
  JetCaloQualityTool(const std::string & name);

  virtual int modifyJet(xAOD::Jet& ) const ;
  
  virtual StatusCode initialize();

 private:

  int m_LArQualityCut;
  int m_TileQualityCut;

  bool m_doFracSamplingMax;
  
  bool m_doN90;
  bool m_doLArQuality;
  bool m_doTileQuality;
  bool m_doTiming;
  bool m_doHECQuality;
  bool m_doNegativeE;
  bool m_doAverageLArQF;
  bool m_timingOnlyPosE;
  bool m_computeFromCluster;
  bool m_doJetCentroid;


  std::vector <double> m_timingCellTimeCuts;
  std::vector <double> m_timingClusterTimeCuts;

  mutable jet::JetCaloCalculations m_jetCalculations;

			     
};
#endif 

