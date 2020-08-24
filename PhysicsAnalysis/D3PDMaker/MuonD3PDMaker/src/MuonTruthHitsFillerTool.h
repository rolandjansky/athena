/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonTruthHitsFillerTool.h
 * @author scott snyder <snyder@bnl.gov>, from code by Niels van Eldik.
 * @date Aug, 2013
 * @brief Fill truth hit information for muons.
 */

#ifndef MUOND3PDMAKER_MUONTRUTHHITSFILLERTOOL_H
#define MUOND3PDMAKER_MUONTRUTHHITSFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "D3PDMakerUtils/BlockFillerToolMulti.h"
#include "TrackRecord/TrackRecord.h"
#include "McParticleEvent/TruthParticle.h"
#include "xAODTruth/TruthParticle.h"

#include <vector>

namespace D3PD {

/** 
 * @brief Fill truth hit information for muons.
 */
class MuonTruthHitsFillerTool
  : public BlockFillerTool<Types<TrackRecord, TruthParticle, xAOD::TruthParticle> >
{
public:
  typedef BlockFillerTool<Types<TrackRecord, TruthParticle, xAOD::TruthParticle> > Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MuonTruthHitsFillerTool (const std::string& type,
                           const std::string& name,
                           const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   */
  virtual StatusCode fill (const TrackRecord& p);


  /**
   * @brief Fill one block --- type-safe version.
   */
  virtual StatusCode fill (const TruthParticle& p);


  /**
   * @brief Fill one block --- type-safe version.
   */
  virtual StatusCode fill (const xAOD::TruthParticle& p);


private:
  StatusCode fillHitCounts (int barcode);

  int* m_nprecLayers;
  int* m_nphiLayers;
  int* m_ntrigEtaLayers;
  std::vector<int*> m_nprecHitsPerChamberLayer;
  std::vector<int*> m_nphiHitsPerChamberLayer;
  std::vector<int*> m_ntrigEtaHitsPerChamberLayer;

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  std::vector<std::string> m_PRD_TruthNames;
};


} // namespace D3PD


#endif // not MUOND3PDMAKER_MUONTRUTHHITSFILLERTOOL_H
