/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBSEXTRACTION_TRIGBSTOXAODTOOL_H
#define TRIGBSEXTRACTION_TRIGBSTOXAODTOOL_H
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigBSExtraction/ITrigBStoxAODTool.h"
#include "TrigSerializeCnvSvc/ITrigSerConvHelper.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigNavigation/Navigation.h"


// conversion tool includes

#include "xAODTauCnv/ITauJetCnvTool.h"

#include "xAODTrigMuonCnv/ICombinedMuonFeatureContainerCnvTool.h"
#include "xAODTrigMuonCnv/IIsoMuonFeatureContainerCnvTool.h"
#include "xAODTrigMuonCnv/ITrigMuonEFInfoToMuonCnvTool.h"

#include "xAODJetCnv/IJetCnvTool.h"

// not configurable algtools
// #include "xAODTrigRingerCnv/ITrigRingerRingsCnvTool.h"
// #include "xAODTrigRingerCnv/ITrigRNNOutputCnvTool.h"

#include "xAODTrigCaloCnv/ITrigCaloClusterCnvTool.h"
#include "xAODTrigCaloCnv/ITrigEMClusterCnvTool.h"

//not in devval yet
//#include "xAODBTaggingCnv/ITrigBjetCnvTool.h"

#include "xAODTrigBphysCnv/ITrigEFBphysContainerCnvTool.h"
#include "xAODTrigBphysCnv/ITrigL2BphysContainerCnvTool.h"

//not in devval yet
//#include "xAODTrigEgammaCnv/ITrigElectronCnvTool.h"
//#include "xAODTrigEgammaCnv/ITrigPhotonCnvTool.h"

#include "xAODTrigMissingETCnv/ITrigMissingETCnvTool.h"

#include "xAODTrigMinBiasCnv/ITrigSpacePointCountsCnvTool.h"
#include "xAODTrigMinBiasCnv/ITrigT2MbtsBitsCnvTool.h"
#include "xAODTrigMinBiasCnv/ITrigTrackCountsCnvTool.h"
#include "xAODTrigMinBiasCnv/ITrigVertexCountsCnvTool.h"

//tracking collections
#include "xAODTrackingCnv/IRecTrackParticleContainerCnvTool.h"
#include "xAODTrackingCnv/ITrackCollectionCnvTool.h"

//egamma
#include "xAODEgammaCnv/IElectronCnvTool.h"
#include "xAODEgammaCnv/IPhotonCnvTool.h"


/**
 * @brief Tool used by TrigBSExtraction to convert to xAOD
 */

namespace BStoXAODHelper{
  class IHelper;
}

class TrigBStoxAODTool : public AthAlgTool, public virtual ITrigBStoxAODTool {
public:
  TrigBStoxAODTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TrigBStoxAODTool();   
  virtual StatusCode initialize();
  StatusCode convert(HLT::Navigation* nav); 
  StatusCode rewireNavigation(HLT::Navigation* nav);

 private:  
  
  StatusCode classLabel2CLIDLabel(const std::vector<std::string>& property,
			      std::vector<std::pair<CLID,
			      std::string> >& decoded );
  
  std::string m_l2ResultKey;  //!< key of HLTResult for L2
  std::string m_efResultKey;  //!< key of HLTResult for EF
  std::string m_hltResultKey;  //!< key of HLTResult for HLT
  std::vector<std::string> m_containersToConvert;

  std::vector<std::pair<CLID,std::string> > m_clid_labels;

  ToolHandle<xAODMaker::ITauJetCnvTool> m_tauJetTool;

  // xAODTrigMuonCnv
  ToolHandle<xAODMaker::ICombinedMuonFeatureContainerCnvTool> m_combMuonTool;
  ToolHandle<xAODMaker::IIsoMuonFeatureContainerCnvTool> m_isoMuonTool;
  ToolHandle<ITrigMuonEFInfoToMuonCnvTool> m_trigMuonTool;

  // xAODJetCnv
  ToolHandle<xAODMaker::IJetCnvTool> m_jetCnvTool;

  // xAODTrigRingerCnv (not configurable algtool)
  // ToolHandle<xAODMaker::ITrigRingerRingsCnvTool> m_ringerRingsTool;
  // ToolHandle<xAODMaker::ITrigRNNOutputCnvTool> m_trigRNNTool;

  // xAODTrigCaloCnv
  ToolHandle<xAODMaker::ITrigCaloClusterCnvTool> m_caloClusterTool;
  ToolHandle<xAODMaker::ITrigEMClusterCnvTool> m_emClusterTool;

  // xAODBTaggingCnv (not in devval yet)
  // ToolHandle<xAODMaker::ITrigBjetCnvTool> m_bjetTool;

  // xAODTrigBphysCnv
  ToolHandle<xAODMaker::ITrigEFBphysContainerCnvTool> m_efBphysTool;
  ToolHandle<xAODMaker::ITrigL2BphysContainerCnvTool> m_l2BphysTool;

  // xAODTrigEgammaCnv (not in devval yet)
  // ToolHandle<xAODMaker::ITrigElectronCnvTool> m_trigElectronTool;
  // ToolHandle<xAODMaker::ITrigPhotonCnvTool> m_trigPhotonTool;

  // xAODTrigMissingETCnv
  ToolHandle<xAODMaker::ITrigMissingETCnvTool> m_trigMetTool;

  // xAODTrigMinBiasCnv
  ToolHandle<xAODMaker::ITrigSpacePointCountsCnvTool> m_trigSpacePtsTool;
  ToolHandle<xAODMaker::ITrigT2MbtsBitsCnvTool> m_trigMbtsBitsTool;
  ToolHandle<xAODMaker::ITrigTrackCountsCnvTool> m_trigTrackCtsTool;
  ToolHandle<xAODMaker::ITrigVertexCountsCnvTool> m_trigVtxCtsTool;

  // xAODTrackingCnv
  ToolHandle<xAODMaker::ITrackCollectionCnvTool> m_trackCollectionTool;
  ToolHandle<xAODMaker::IRecTrackParticleContainerCnvTool> m_recTrackParticleContTool;

  // xAODEgammaCnv
  ToolHandle<xAODMaker::IElectronCnvTool> m_electronTool;
  ToolHandle<xAODMaker::IPhotonCnvTool> m_photonTool;

  std::map<CLID,BStoXAODHelper::IHelper*> m_helpers; //collection clid -> helper
};

#endif // TRIGBSEXTRACTION_TRIGBSTOXAODTOOL_H
