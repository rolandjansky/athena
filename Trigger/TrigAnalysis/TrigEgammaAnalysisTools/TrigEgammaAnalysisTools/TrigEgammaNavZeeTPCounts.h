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
  std::map< std::string,unsigned int > m_nProbesL2;
  /*! Probes matching an L2 trigger object 
   *  L2Electron or L2Photon
   * */
  std::map< std::string,unsigned int > m_nProbesPassedL2;
  /*! EF Probes */
  std::map< std::string,unsigned int > m_nProbesEF;
  /*! Probes mathing EF trigger object 
   *  xAOD::ElectronContainer egamma_Electrons
   *  xAOD::PhotonContainer egamma_Photons
   * */
  std::map< std::string,unsigned int > m_nProbesPassedEF;
};

#endif
