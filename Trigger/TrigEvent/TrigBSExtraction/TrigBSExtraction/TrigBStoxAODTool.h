/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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


#include "xAODTrigCaloCnv/ITrigCaloClusterCnvTool.h"
#include "xAODTrigCaloCnv/ITrigEMClusterCnvTool.h"

#include "xAODBTaggingCnv/ITrigBjetCnvTool.h"

#include "xAODTrigBphysCnv/ITrigEFBphysContainerCnvTool.h"
#include "xAODTrigBphysCnv/ITrigL2BphysContainerCnvTool.h"

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

//CaloCluster
#include "xAODCaloEventCnv/ICaloClusterCnvTool.h"

//TrigPassBits
#include "xAODTriggerCnv/ITrigPassBitsCnvTool.h"



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
  StatusCode setTrigPassBits(HLT::Navigation* nav);

 private:  
  
  StatusCode classLabel2CLIDLabel(const std::vector<std::string>& property,
				  const std::vector<std::string>& newProperty,
				  std::vector<std::pair<CLID,std::string> >& decoded,
				  std::vector<std::pair<CLID,std::string> >& decodedNewNames);

  typedef std::multimap<CLID,BStoXAODHelper::IHelper*> MM_CLID_IHELPER;

  StatusCode findHelper( MM_CLID_IHELPER& helpers, CLID clid, std::string& label, MM_CLID_IHELPER::const_iterator& it );

  std::vector<std::string> m_containersToConvert;
  std::vector<std::string> m_newContainers;

  std::vector<std::pair<CLID,std::string> > m_clid_labels;
  std::vector<std::pair<CLID,std::string> > m_clid_newLabels; //Run-2 labels are not always the same as in Run 1

  ToolHandle<xAODMaker::ITauJetCnvTool> m_tauJetTool;

  // xAODTrigMuonCnv
  ToolHandle<xAODMaker::ICombinedMuonFeatureContainerCnvTool> m_combMuonTool;
  ToolHandle<xAODMaker::IIsoMuonFeatureContainerCnvTool> m_isoMuonTool;
  ToolHandle<ITrigMuonEFInfoToMuonCnvTool> m_trigMuonTool;

  // xAODJetCnv
  ToolHandle<xAODMaker::IJetCnvTool> m_jetCnvTool;

  // xAODTrigCaloCnv
  ToolHandle<xAODMaker::ITrigCaloClusterCnvTool> m_trigCaloClusterTool;
  ToolHandle<xAODMaker::ITrigEMClusterCnvTool> m_emClusterTool;

  // xAODBTaggingCnv
  ToolHandle<xAODMaker::ITrigBjetCnvTool> m_bjetTool;

  // xAODTrigBphysCnv
  ToolHandle<xAODMaker::ITrigEFBphysContainerCnvTool> m_efBphysTool;
  ToolHandle<xAODMaker::ITrigL2BphysContainerCnvTool> m_l2BphysTool;

  // xAODTrigMissingETCnv
  ToolHandle<xAODMaker::ITrigMissingETCnvTool> m_trigMetTool;

  // xAODTrigMinBiasCnv
  ToolHandle<xAODMaker::ITrigSpacePointCountsCnvTool> m_trigSpacePtsTool;
  ToolHandle<xAODMaker::ITrigT2MbtsBitsCnvTool> m_trigMbtsBitsTool;
  ToolHandle<xAODMaker::ITrigTrackCountsCnvTool> m_trigTrackCtsTool;
  ToolHandle<xAODMaker::ITrigVertexCountsCnvTool> m_trigVtxCtsTool;

  // xAODTrackingCnv
  ToolHandle<xAODMaker::ITrackCollectionCnvTool> m_trackCollectionTool{this, "TrackCollectionCnvTool","xAODMaker::TrackCollectionCnvTool/TrackCollectionCnvTool","Cnv of tracks"};
  ToolHandle<xAODMaker::IRecTrackParticleContainerCnvTool> m_recTrackParticleContTool{this, "TrackParticleContainerCnvTool","xAODMaker::RecTrackParticleContainerCnvTool/RecTrackParticleContainerCnvTool","Cnv of Rec::TrackParticles"};

  // xAODEgammaCnv
  ToolHandle<xAODMaker::IElectronCnvTool> m_electronTool;
  ToolHandle<xAODMaker::IPhotonCnvTool> m_photonTool;

  // xAODCaloEventCnv
  ToolHandle<xAODMaker::ICaloClusterCnvTool> m_caloClusterTool;

  // xAODTriggerCnv
  ToolHandle<xAODMaker::ITrigPassBitsCnvTool> m_trigPassBitsTool;

  std::multimap<CLID,BStoXAODHelper::IHelper*> m_helpers; //collection clid -> helper

  // to disentangle AOD egamma electrons/photons to different xAOD types
  CLID m_CLID_xAODPhotonContainer;
  CLID m_CLID_xAODElectronContainer;

};

#endif // TRIGBSEXTRACTION_TRIGBSTOXAODTOOL_H
