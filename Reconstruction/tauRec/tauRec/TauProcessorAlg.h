/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPROCESSORALG_H
#define TAUREC_TAUPROCESSORALG_H

#include "tauRecTools/ITauToolBase.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"
#include "CaloInterface/ICaloCellMakerTool.h"

#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToolHandle.h"

/**
 * @brief       Main class for tau candidate processing.
 */

class CaloCell_ID;

class TauProcessorAlg: public AthReentrantAlgorithm
{
 public:
  //-----------------------------------------------------------------
  // Contructor and destructor
  //-----------------------------------------------------------------
  TauProcessorAlg( const std::string &name, ISvcLocator *pSvcLocator );
  ~TauProcessorAlg();

  //-----------------------------------------------------------------
  // Gaudi algorithm hooks
  //-----------------------------------------------------------------
  virtual StatusCode initialize();
  virtual StatusCode execute(const EventContext& ctx) const;

 private:
       
  Gaudi::Property<double> m_maxEta {this, "MaxEta", 2.5, "maximum eta for jet seed"};
  Gaudi::Property<double> m_minPt {this, "MinPt", 10 * Gaudi::Units::GeV, "minimum pT for jet seed"};

  const ToolHandleArray<ITauToolBase> m_tools {this, "Tools", {}, "Tools processing taus"};
  const ToolHandle<ICaloCellMakerTool> m_cellMakerTool {this, "CellMakerTool", "", "Tool to sort the CaloCellContainer"};

  SG::ReadHandleKey<xAOD::JetContainer> m_jetInputContainer{this,"Key_jetInputContainer","AntiKt4LCTopoJets","input jet key"};
	
  SG::WriteHandleKey<xAOD::TauJetContainer> m_tauOutputContainer{this,"Key_tauOutputContainer","tmp_TauJets","output tau data key"};
  SG::WriteHandleKey<xAOD::TauTrackContainer> m_tauTrackOutputContainer{this,"Key_tauTrackOutputContainer","TauTracks","output tau tracks data key"};
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_tauShotClusOutputContainer{this,"Key_tauShotClusOutputContainer", "TauShotClusters", "tau shot clusters out key"};
  SG::WriteHandleKey<xAOD::PFOContainer> m_tauShotPFOOutputContainer{this,"Key_tauShotPFOOutputContainer", "TauShotParticleFlowObjects", "tau pfo out key"};
  SG::WriteHandleKey<CaloCellContainer> m_tauPi0CellOutputContainer{this,"Key_tauPi0CellOutputContainer","TauCommonPi0Cells","output calo cell key"};
    
  const CaloCell_ID* m_cellID;
    
  // These are needed to read ESD and AOD in AthenaMT for P->T conversion of ID tracks.
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> m_trtDetEleContKey{this, "TRTDetEleContKey", "TRT_DetElementContainer", "Key of TRT_DetElementContainer"}; 

};

#endif // TAUREC_TAUPROCESSORALG_H
