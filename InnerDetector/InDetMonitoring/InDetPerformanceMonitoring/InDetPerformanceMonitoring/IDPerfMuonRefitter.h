/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDPerfMuonRefitterTER_H
#define IDPerfMuonRefitterTER_H

//==============================================================================
// Include files...
//==============================================================================

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ToolHandle.h"
#include "egammaInterfaces/IegammaTrkRefitterTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODMuon/MuonContainer.h"

class IDPerfMuonRefitter : public AthAlgorithm
{
 public:
  // Constructors & destructors

  IDPerfMuonRefitter(const std::string& name, ISvcLocator* pSvcLocator);

  ~IDPerfMuonRefitter();

  // Overriden class functions.
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 protected:

 private:

  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_TrackRefitter1;

  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_TrackRefitter2;


  std::string m_outputTracksName;


  int m_N_Muons;
  int m_N_MuonsRefit;
  int m_N_MuonRefitFailures;

  SG::ReadHandleKey<xAOD::MuonContainer> m_muonContainerKey
  { this, "MuonContainerKey", "Muons", "" };

};
//==============================================================================

#endif
