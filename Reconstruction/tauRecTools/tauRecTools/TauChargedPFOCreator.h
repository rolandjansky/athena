/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUCHARGEDPFOCREATOR_H
#define TAUREC_TAUCHARGEDPFOCREATOR_H

#include "tauRecTools/TauRecToolBase.h"
#include "xAODTau/TauDefs.h"

#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODPFlow/PFOContainer.h"


/**
 * @brief Implementation of tool to correct associate charged PFOs to Tau candidate
 * 
 *  If one redoes tracking in ABR mode, then the charged PFO and links will be incorrect
 *  This tool mimics as best as possible in ABR what is done in TauPi0ClusterScaler
 *
 *  The Charged PFOs are meant for use by PanTau
 *
 * @author J. Griffiths
 *                                                                              
 */


class TauChargedPFOCreator : public TauRecToolBase {
public:

  TauChargedPFOCreator(const std::string& name="TauChargedPFOCreator") ;
  ASG_TOOL_CLASS2(TauChargedPFOCreator, TauRecToolBase, ITauToolBase)
  ~TauChargedPFOCreator();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute(xAOD::TauJet& pTau);

  virtual void print() const { }
  virtual StatusCode eventInitialize();
  virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }
  virtual void cleanup(xAOD::TauJet* ) { }


private:
    
  std::string m_chargedPFOContainerName;

  int m_iTrackFlag;

  xAOD::PFOContainer* m_chargedPFOContainer;
  xAOD::PFOAuxContainer* m_chargedPFOAuxStore;

};

#endif
