/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaNavZeeTPCounts_H
#define TrigEgammaNavZeeTPCounts_H

#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPBaseTool.h"

class TrigEgammaNavZeeTPCounts
: public TrigEgammaNavZeeTPBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaNavZeeTPCounts, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaNavZeeTPCounts( const std::string& myname );
  ~TrigEgammaNavZeeTPCounts() {};

  StatusCode childInitialize();
  StatusCode childExecute();
  StatusCode childFinalize();

private:
  /*! Total number of events */
  unsigned int m_eventCounter;
  /*! Number of Probes for L2 (really only need one probe counter) */
  std::map< std::string,unsigned int > m_nProbes;
  /*! Probes matching an L2 trigger object 
   *  L2Electron or L2Photon
   * */
  std::map< std::string,unsigned int > m_nProbesPassedL2;
  std::map< std::string,float > m_EffL2;
  std::map< std::string,unsigned int > m_nProbesPassedL1;
  std::map< std::string,float > m_EffL1;
  /*! EF Probes */
  /*! Probes mathing EF trigger object 
   *  xAOD::ElectronContainer egamma_Electrons
   *  xAOD::PhotonContainer egamma_Photons
   * */
  std::map< std::string,unsigned int > m_nProbesPassedEF;
  std::map< std::string,float > m_EffHLT;
  
  std::map< std::string,unsigned int > m_nProbesPassedL2Calo;
  std::map< std::string,float > m_EffL2Calo;
  
  std::map< std::string,unsigned int > m_nProbesPassedEFCalo;
  std::map< std::string,float > m_EffEFCalo;
};

#endif
