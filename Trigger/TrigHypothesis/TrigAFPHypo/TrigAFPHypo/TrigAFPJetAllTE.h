/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGAFPHYPO_TRIGAFPJETALLTE_H
#define TRIGAFPHYPO_TRIGAFPJETALLTE_H

#include <string>

#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigAFPHypo/AFPProtonTransportTool.h"

class TrigAFPJetAllTE: public HLT::AllTEAlgo {
public:
  TrigAFPJetAllTE(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigAFPJetAllTE();

  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE,
                            unsigned int outputTE);

  HLT::ErrorCode hltEndEvent();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
private:
  bool m_decision;
  bool m_useCachedResult;

  AFPProtonTransportTool* m_transportBeam1;
  AFPProtonTransportTool* m_transportBeam2;

  std::string m_protonTransportParamFileName1;
  std::string m_protonTransportParamFileName2;

  float m_maxProtonDist;
  float m_maxProtonDiff_x;
  float m_maxProtonDiff_y;

  float m_beamEnergy;       ///< energy of one beam i.e. half of centre-of-mass energy
  float m_totalEnergy;      ///< beams centre-of-mass energy 2*#m_beamEnergy

  float m_protonPosShift_x;
  float m_protonPosShift_y;

  float m_alignmentCorrection_nearA;
  float m_alignmentCorrection_nearC;

  const float m_GeV = 0.001;

  //Monitored variables
  int m_jetsN;
  double m_dijetMass;
  double m_dijetEta;
  double m_dijetRapidity;

  int m_sideA_tracksN;
  double m_sideA_minDist;
  double m_sideA_trkX;
  double m_sideA_trkY;
  double m_sideA_predictX;
  double m_sideA_predictY;
  double m_sideA_diffX;
  double m_sideA_diffY;

  int m_sideC_tracksN;
  double m_sideC_minDist;
  double m_sideC_trkX;
  double m_sideC_trkY;
  double m_sideC_predictX;
  double m_sideC_predictY;
  double m_sideC_diffX;
  double m_sideC_diffY;
};

#endif
