// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBSExtraction/TrigBStoxAODTool.h"

#include "TrigSteeringEvent/HLTResult.h"
#include "TrigNavigation/NavigationCore.h"

#include "TrigNavigation/Holder.h"
#include "GaudiKernel/ClassID.h"

#include "TrigStorageDefinitions/EDM_TypeInfo.h"

#include "TrigSerializeCnvSvc/TrigStreamAddress.h"
#include "Particle/TrackParticleContainer.h"

#include "TrigNavigation/TriggerElement.h"

#include "JetEvent/JetCollection.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "xAODTrigMissingET/TrigMissingETAuxContainer.h"

// #include "TrigParticle/TrigPhotonContainer.h"
// #include "xAODTrigEgamma/TrigPhotonContainer.h"
// #include "xAODTrigEgamma/TrigPhotonAuxContainer.h"

#include "TrigNavigation/Holder.h"

#include "TrigCaloEvent/TrigCaloClusterContainer.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/IsoMuonFeatureContainer.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"


#include "xAODTrigMuon/L2IsoMuonContainer.h"
#include "xAODTrigMuon/L2IsoMuonAuxContainer.h"

#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuonAuxContainer.h"

#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"

#include "xAODTrigCalo/TrigCaloClusterContainer.h"
#include "xAODTrigCalo/TrigCaloClusterAuxContainer.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"

#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticle/TrigL2BphysContainer.h"
#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"

#include "TrigInDetEvent/TrigSpacePointCountsCollection.h"
#include "TrigInDetEvent/TrigTrackCountsCollection.h"
#include "TrigInDetEvent/TrigVertexCountsCollection.h"
#include "TrigCaloEvent/TrigT2MbtsBitsContainer.h"


#include "xAODTrigMinBias/TrigSpacePointCountsContainer.h"
#include "xAODTrigMinBias/TrigSpacePointCountsAuxContainer.h"

#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h"

#include "xAODTrigMinBias/TrigTrackCountsContainer.h"
#include "xAODTrigMinBias/TrigTrackCountsAuxContainer.h"

#include "xAODTrigMinBias/TrigVertexCountsContainer.h"
#include "xAODTrigMinBias/TrigVertexCountsAuxContainer.h"

#include "TrkTrack/TrackCollection.h"
#include "Particle/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include "tauEvent/TauJetContainer.h"
#include "xAODTau/TauJetContainer.h"

#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "egammaEvent/egammaContainer.h"

// #include "CaloEvent/CaloClusterContainer.h"
// #include "xAODCaloEvent/CaloClusterContainer.h"
// #include "xAODCaloEvent/CaloClusterAuxContainer.h"

template<typename element,typename list,int index> struct get_strictly_feat{
  static const bool result = HLT::TypeInformation::at<list,index>::type::list_of_features::template has<element>::result;
};

template<class T, class EDMLIST = TypeInfo_EDM> struct known{
  typedef typename master_search<typename EDMLIST::map,
				 get_strictly_feat,T>::result::search_result search_result;
  static const bool value = !boost::is_same<HLT::TypeInformation::ERROR_THE_FOLLOWING_TYPE_IS_NOT_KNOWN_TO_THE_EDM<T>,search_result>::value;
};


template<typename T, bool = known<T>::value> struct getCLID;
template<typename T> struct getCLID<T,true>{static int ID(){return ClassID_traits<T>::ID();}};
template<typename T> struct getCLID<T,false>{static int ID(){return -1;}};


namespace BStoXAODHelper{



  //Helper class providing interface for conversion and some convenient access to SG and Msg Services
  class IHelper{
  public:
    virtual ~IHelper(){;}
    virtual StatusCode initialize(MsgStream& m, StoreGateSvc& s){
      m_msg = &m; 
      m_sg = &s;
      return StatusCode::SUCCESS;
    };
    
    virtual StatusCode help(const std::string& label) = 0;

    virtual CLID AODContainerClid() = 0;
    virtual CLID xAODContainerClid() = 0;
    virtual CLID AODElementClid() = 0;
    virtual CLID xAODElementClid() = 0;

    MsgStream& msg(MSG::Level l){return *m_msg << l;}
    bool msgLvl (const MSG::Level l){
      if(m_msg->level() <= l){
	*m_msg << l; return true;
      } else {
	return false;
      };      
    }

    template<typename T>
    std::string format(const std::string& label){
      return HLTNavDetails::formatSGkey(this->m_AODprefix,ClassID_traits<T>::typeName(),label);
    }
  protected:
    MsgStream* m_msg;
    StoreGateSvc* m_sg;
    const std::string m_AODprefix = "HLT";
  };
    
  //class holding a tool, since this is what all helpers will need to do.
  template<typename T>
  class ToolHolder : public IHelper {
  protected:
    const ToolHandle<T>& m_tool;
    ToolHandle<T>& tool(){return m_tool;}
  public:
    ToolHolder(const ToolHandle<T>& tool) : m_tool(tool){;}
    virtual ~ToolHolder(){;}
    virtual StatusCode initialize(MsgStream& m, StoreGateSvc& s){
      CHECK(IHelper::initialize(m,s));
      CHECK(m_tool.retrieve());
      return StatusCode::SUCCESS;
    }
  };
  
  //this is the most vanilla case
  template<typename AOD,typename xAOD, typename CnvTool>
  struct DefaultHelper : public ToolHolder<CnvTool> {
    typedef typename Container2Aux<xAOD>::type xAODAux;
    
    DefaultHelper(const ToolHandle<CnvTool>& tool) : ToolHolder<CnvTool>(tool){;}

    CLID AODContainerClid(){return getCLID<AOD>::ID();}    
    CLID xAODContainerClid(){return getCLID<xAOD>::ID();}

    CLID AODElementClid(){return getCLID<typename Container2Object<AOD>::type>::ID();}    
    CLID xAODElementClid(){return getCLID<typename Container2Object<xAOD>::type>::ID();}

    virtual StatusCode help(const std::string& label){
      typedef IHelper IH;
      std::string fmtkey_AOD = IH::template format<AOD>(label);
      std::string fmtkey_xAOD = IH::template format<xAOD>(label);
      std::string fmtkey_xAODAux = IH::template format<xAODAux>(label+"Aux.");

      const AOD* aod = this->m_sg->template tryConstRetrieve<AOD>(fmtkey_AOD);
      if(!aod){
	ATH_MSG_WARNING("key: " <<  fmtkey_AOD << " not found for xAOD conversion");
	return StatusCode::SUCCESS;
      }
      
      ATH_MSG_DEBUG("attempting to convert " << fmtkey_AOD << " of size " << aod->size() << " to " << fmtkey_xAOD);

      xAOD* xaod = this->m_sg->template tryRetrieve<xAOD>(fmtkey_xAOD);
      if(!xaod){
	ATH_MSG_WARNING("key: " <<  fmtkey_xAOD << " not found for xAOD conversion");
	return StatusCode::SUCCESS;
      }

      CHECK( this->m_tool->convert(aod,xaod));      
      ATH_MSG_DEBUG("AOD container has size: " << aod->size());
      ATH_MSG_DEBUG("xAOD container has size: " << xaod->size());
      if(aod->size() != xaod->size()){
	ATH_MSG_ERROR("conversion resulted in differently sized containers");
	return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }    
  };
  
  struct MuonHelper : public ToolHolder<ITrigMuonEFInfoToMuonCnvTool> {
    MuonHelper(const ToolHandle<ITrigMuonEFInfoToMuonCnvTool>& tool) : ToolHolder(tool){;}

    CLID AODContainerClid(){return getCLID<TrigMuonEFInfoContainer>::ID();}
    CLID xAODContainerClid(){return getCLID<xAOD::MuonContainer>::ID();}

    CLID AODElementClid(){return getCLID<Container2Object<TrigMuonEFInfoContainer>::type>::ID();}
    CLID xAODElementClid(){return getCLID<Container2Object<xAOD::MuonContainer>::type >::ID();}

    virtual StatusCode help(const std::string& label){
      xAOD::MuonContainer* xaodMuon = this->m_sg->tryRetrieve<xAOD::MuonContainer>(format<xAOD::MuonContainer>(label));
      if(!xaodMuon){
	ATH_MSG_WARNING("label: " << format<xAOD::MuonContainer>(label) << " not found for xAOD conversion");
	return StatusCode::SUCCESS;
      }

      xAOD::TrackParticleContainer* xaodCombTrackParticles = this->m_sg->tryRetrieve<xAOD::TrackParticleContainer>(format<xAOD::TrackParticleContainer>(label+"_CombTrackParticles"));
      if(!xaodCombTrackParticles){
	ATH_MSG_WARNING("label: " << format<xAOD::TrackParticleContainer>(label+"_CombTrackParticles") << " not found for xAOD conversion");
	return StatusCode::SUCCESS;
      }

      xAOD::TrackParticleContainer* xaodExtrapTrackParticles = this->m_sg->tryRetrieve<xAOD::TrackParticleContainer>(format<xAOD::TrackParticleContainer>(label+"_ExtrapTrackParticles"));
      if(!xaodCombTrackParticles){
	ATH_MSG_WARNING("label: " << format<xAOD::TrackParticleContainer>(label+"_ExtrapTrackParticles") << " not found for xAOD conversion");
	return StatusCode::SUCCESS;
      }

      const TrigMuonEFInfoContainer* aod = 
	this->m_sg->tryConstRetrieve<TrigMuonEFInfoContainer>(format<TrigMuonEFInfoContainer>(label));
      if(!aod){
	ATH_MSG_WARNING("label: " << format<TrigMuonEFInfoContainer>(label) << " not found for xAOD conversion");
	return StatusCode::SUCCESS;
      }

      CHECK(this->m_tool->convertTrigMuonEFInfoContainer( *aod, *xaodMuon, xaodCombTrackParticles, xaodExtrapTrackParticles));

      ATH_MSG_DEBUG("Muon AOD container has size: " << aod->size());
      ATH_MSG_DEBUG("Muon xAOD container has size: " << xaodMuon->size());
      
      return StatusCode::SUCCESS;
    }
  };

}
  
TrigBStoxAODTool::TrigBStoxAODTool(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent),
    // needs newer pkg tag
    m_tauJetTool(      "xAODMaker::TauJetCnvTool/TauJetCnvTool",this),
    m_combMuonTool(    "xAODMaker::CombinedMuonFeatureContainerCnvTool/CombinedMuonFeatureContainerCnvTool",this),
    m_isoMuonTool(     "xAODMaker::IsoMuonFeatureContainerCnvTool/IsoMuonFeatureContainerCnvTool",this),
    m_trigMuonTool(    "TrigMuonEFInfoToMuonCnvTool/TrigMuonEFInfoToMuonCnvTool",this),
    m_jetCnvTool(      "xAODMaker::JetCnvTool/JetCnvTool",this),
    // not configurable algtool
    // m_ringerRingsTool( "xAODMaker::TrigRingerRingsCnvTool/TrigRingerRingsCnvTool",this),
    // m_trigRNNTool(     "xAODMaker::TrigRNNOutputCnvTool/TrigRNNOutputCnvTool",this),
    m_caloClusterTool( "xAODMaker::TrigCaloClusterCnvTool/TrigCaloClusterCnvTool",this),
    m_emClusterTool(   "xAODMaker::TrigEMClusterCnvTool/TrigEMClusterCnvTool",this),
    // not in devval yet
    // m_bjetTool(        "xAODMaker::TrigBjetCnvTool/TrigBjetCnvTool",this),
    m_efBphysTool(     "xAODMaker::TrigEFBphysContainerCnvTool/TrigEFBphysContainerCnvTool",this),
    m_l2BphysTool(     "xAODMaker::TrigL2BphysContainerCnvTool/TrigL2BphysContainerCnvTool",this),
    // not in devval yet 
    // m_trigElectronTool("xAODMaker::TrigElectronCnvTool/TrigElectronCnvTool",this),
    // m_trigPhotonTool(  "xAODMaker::TrigPhotonCnvTool/TrigPhotonCnvTool",this),
    m_trigMetTool(     "xAODMaker::TrigMissingETCnvTool/TrigMissingETCnvTool",this),
    m_trigSpacePtsTool("xAODMaker::TrigSpacePointCountsCnvTool/TrigSpacePointCountsCnvTool",this),
    m_trigMbtsBitsTool("xAODMaker::TrigT2MbtsBitsCnvTool/TrigT2MbtsBitsCnvTool",this),
    m_trigTrackCtsTool("xAODMaker::TrigTrackCountsCnvTool/TrigTrackCountsCnvTool",this),
    m_trigVtxCtsTool(  "xAODMaker::TrigVertexCountsCnvTool/TrigVertexCountsCnvTool",this),
    m_trackCollectionTool(      "xAODMaker::TrackCollectionCnvTool/TrackCollectionCnvTool", this ),
    m_recTrackParticleContTool( "xAODMaker::RecTrackParticleContainerCnvTool/RecTrackParticleContainerCnvTool", this ),
    m_electronTool( "xAODMaker::ElectronCnvTool/ElectronCnvTool", this ),
    m_photonTool( "xAODMaker::PhotonCnvTool/PhotonCnvTool", this )
{
  declareInterface<ITrigBStoxAODTool>( this );
  declareProperty("L2ResultKey",     m_l2ResultKey = "HLTResult_L2");
  declareProperty("EFResultKey",     m_efResultKey = "HLTResult_EF");
  declareProperty("HLTResultKey",     m_hltResultKey = "HLTResult_HLT");
  declareProperty("ContainersToConvert",m_containersToConvert);

  declareProperty("tauJetTool", m_tauJetTool);
  declareProperty("combMuonTool", m_combMuonTool);
  declareProperty("isoMuonTool", m_isoMuonTool);
  declareProperty("trigMuonTool", m_trigMuonTool);
  declareProperty("jetCnvTool", m_jetCnvTool);

  // not configurable algtools
  // declareProperty("ringerRingsTool", m_ringerRingsTool);
  // declareProperty("trigRNNTool", m_trigRNNTool);

  declareProperty("caloClusterTool", m_caloClusterTool);
  declareProperty("emClusterTool", m_emClusterTool);
  // not in devval yet
  // declareProperty("bjetTool", m_bjetTool);
  declareProperty("efBphysTool", m_efBphysTool);
  declareProperty("l2BphysTool", m_l2BphysTool);
  // not in devval yet
  // declareProperty("trigElectronTool", m_trigElectronTool);
  // declareProperty("trigPhotonTool", m_trigPhotonTool);
  declareProperty("trigMetTool", m_trigMetTool);
  declareProperty("trigSpacePtsTool", m_trigSpacePtsTool);
  declareProperty("trigMbtsBitsTool", m_trigMbtsBitsTool);
  declareProperty("trigTrackCtsTool", m_trigTrackCtsTool);
  declareProperty("trigVtxCtsTool", m_trigVtxCtsTool);
  declareProperty("trackCollectionTool", m_trackCollectionTool);
  declareProperty("recTrackParticleContTool", m_recTrackParticleContTool);
  declareProperty("electronTool", m_electronTool);
  declareProperty("photonTool", m_photonTool);
}

TrigBStoxAODTool::~TrigBStoxAODTool() {
  std::map<CLID,BStoXAODHelper::IHelper*>::iterator it;
  for(it = m_helpers.begin();it!=m_helpers.end();++it){
    delete it->second;
  }
}

StatusCode TrigBStoxAODTool::initialize(){
  // m_helpers[ClassID_traits<IsoMuonFeatureContainer>::ID()] = 
  //   new BStoXAODHelper::DefaultHelper<
  //     IsoMuonFeatureContainer,xAOD::L2IsoMuonContainer,xAODMaker::IIsoMuonFeatureContainerCnvTool>(m_isoMuonTool);

  m_helpers[ClassID_traits<CombinedMuonFeatureContainer>::ID()] = 
    new BStoXAODHelper::DefaultHelper<
      CombinedMuonFeatureContainer,xAOD::L2CombinedMuonContainer,xAODMaker::ICombinedMuonFeatureContainerCnvTool>(m_combMuonTool);

  m_helpers[ClassID_traits<TrigMuonEFInfoContainer>::ID()] = new BStoXAODHelper::MuonHelper(m_trigMuonTool);

  m_helpers[ClassID_traits<TrigCaloClusterContainer>::ID()] = 
    new BStoXAODHelper::DefaultHelper<
      TrigCaloClusterContainer,xAOD::TrigCaloClusterContainer,xAODMaker::ITrigCaloClusterCnvTool>(m_caloClusterTool);

  m_helpers[ClassID_traits<TrigEMClusterContainer>::ID()] = 
    new BStoXAODHelper::DefaultHelper<
      TrigEMClusterContainer,xAOD::TrigEMClusterContainer,xAODMaker::ITrigEMClusterCnvTool>(m_emClusterTool);

  m_helpers[ClassID_traits<TrigEFBphysContainer>::ID()] = 
    new BStoXAODHelper::DefaultHelper<
      TrigEFBphysContainer,xAOD::TrigBphysContainer,xAODMaker::ITrigEFBphysContainerCnvTool>(m_efBphysTool);

  m_helpers[ClassID_traits<TrigL2BphysContainer>::ID()] = 
    new BStoXAODHelper::DefaultHelper<
      TrigL2BphysContainer,xAOD::TrigBphysContainer,xAODMaker::ITrigL2BphysContainerCnvTool>(m_l2BphysTool);

  m_helpers[ClassID_traits<JetCollection>::ID()] = 
    new BStoXAODHelper::DefaultHelper<JetCollection,xAOD::JetContainer,xAODMaker::IJetCnvTool>(m_jetCnvTool);

  m_helpers[ClassID_traits<TrigMissingETContainer>::ID()] = 
    new BStoXAODHelper::DefaultHelper<
      TrigMissingETContainer,xAOD::TrigMissingETContainer,xAODMaker::ITrigMissingETCnvTool>(m_trigMetTool);

  m_helpers[ClassID_traits<TrigSpacePointCountsCollection>::ID()] = 
    new BStoXAODHelper::DefaultHelper<
      TrigSpacePointCountsCollection,xAOD::TrigSpacePointCountsContainer,xAODMaker::ITrigSpacePointCountsCnvTool>(m_trigSpacePtsTool);

  m_helpers[ClassID_traits<TrigT2MbtsBitsContainer>::ID()] = 
    new BStoXAODHelper::DefaultHelper<
      TrigT2MbtsBitsContainer,xAOD::TrigT2MbtsBitsContainer,xAODMaker::ITrigT2MbtsBitsCnvTool>(m_trigMbtsBitsTool);

  m_helpers[getCLID<TrigTrackCountsCollection>::ID()] = 
    new BStoXAODHelper::DefaultHelper<
      TrigTrackCountsCollection,xAOD::TrigTrackCountsContainer,xAODMaker::ITrigTrackCountsCnvTool>(m_trigTrackCtsTool);

  m_helpers[ClassID_traits<TrigVertexCountsCollection>::ID()] = 
    new BStoXAODHelper::DefaultHelper<
      TrigVertexCountsCollection,xAOD::TrigVertexCountsContainer,xAODMaker::ITrigVertexCountsCnvTool>(m_trigVtxCtsTool);

  m_helpers[ClassID_traits<TrackCollection>::ID()] =
    new BStoXAODHelper::DefaultHelper<
  TrackCollection,xAOD::TrackParticleContainer,xAODMaker::ITrackCollectionCnvTool>(m_trackCollectionTool);
  
  m_helpers[ClassID_traits<Rec::TrackParticleContainer>::ID()] =
    new BStoXAODHelper::DefaultHelper<
  Rec::TrackParticleContainer,xAOD::TrackParticleContainer,xAODMaker::IRecTrackParticleContainerCnvTool>(m_recTrackParticleContTool);
  
  m_helpers[ClassID_traits<Analysis::TauJetContainer>::ID()] =
    new BStoXAODHelper::DefaultHelper<
  Analysis::TauJetContainer,xAOD::TauJetContainer,xAODMaker::ITauJetCnvTool>(m_tauJetTool);

  m_helpers[ClassID_traits<egammaContainer>::ID()] =
    new BStoXAODHelper::DefaultHelper<
  egammaContainer,xAOD::PhotonContainer,xAODMaker::IPhotonCnvTool>(m_photonTool);
  
  m_helpers[ClassID_traits<egammaContainer>::ID()] =
    new BStoXAODHelper::DefaultHelper<
  egammaContainer,xAOD::ElectronContainer,xAODMaker::IElectronCnvTool>(m_electronTool);
  
  
  std::map<CLID,BStoXAODHelper::IHelper*>::iterator it;
  for(it = m_helpers.begin();it!=m_helpers.end();++it){
    CHECK(it->second->initialize(this->msg(),*(evtStore())));
  }

  ATH_MSG_DEBUG("containers to convert: " << m_containersToConvert);
  CHECK(classLabel2CLIDLabel(m_containersToConvert,m_clid_labels));
  ATH_MSG_DEBUG("extracted class IDs " << m_clid_labels);

  return StatusCode::SUCCESS;
}

StatusCode TrigBStoxAODTool::convert(HLT::Navigation* nav) {
    std::vector<std::pair<CLID,std::string> >::const_iterator clidlabel;
    for(clidlabel =m_clid_labels.begin();clidlabel!=m_clid_labels.end();++clidlabel){

      HLTNavDetails::IHolder* holder = nav->getHolder(clidlabel->first,clidlabel->second);
      
      if(!holder){
	ATH_MSG_WARNING("couldn't find holder for " << clidlabel->first << " " << clidlabel->second);
	continue;
      }

      ATH_MSG_VERBOSE("container corresponding to feature CLID " << clidlabel->first << " has CLID " << holder->containerClid());
      
      std::map<CLID,BStoXAODHelper::IHelper*>::iterator hit = m_helpers.find(holder->containerClid());
      if(hit!=m_helpers.end()){
	ATH_MSG_DEBUG("attempting convertion of clid " << holder->containerClid() << " for label " << clidlabel->second);
	CHECK(hit->second->help(clidlabel->second));//get aod container and convert
	ATH_MSG_DEBUG("converted clid " << holder->containerClid() << " for label " << clidlabel->second);
      }
      else{
	ATH_MSG_DEBUG("couldn't find converters for clid: " << clidlabel->first);
      }
    }
    return StatusCode::SUCCESS;
}

StatusCode TrigBStoxAODTool::rewireNavigation(HLT::Navigation* nav) {
  //we need to loop over all features of all TEs
  //find the ones pointing to a AOD container that has been converted
  //and change the holder they use to access the feature, i.e. changes CLID and subTypeIndex

  std::vector<HLT::TriggerElement*>& allTEs = nav->getAllTEs();
  //nav->getAll(allTEs,false); //false: also get inactive TEs
  ATH_MSG_DEBUG("rewiring Navigation for xAOD. working on " << allTEs.size() << " TEs");
  for(auto te : allTEs){
    std::vector<HLT::TriggerElement::FeatureAccessHelper>& feats = te->getFeatureAccessHelpers();
    for(auto& featacchelp : feats){
      ATH_MSG_VERBOSE("this feature has CLID: " << featacchelp.getCLID());

      HLTNavDetails::IHolder* oldholder = nav->getHolder(featacchelp.getCLID(),featacchelp.getIndex().subTypeIndex());

      if(!oldholder){
	ATH_MSG_WARNING("could not find old holder for CLID " << featacchelp.getCLID() << " and subTypeIndex: " << featacchelp.getIndex());
	continue;
      }
      
      auto it = m_helpers.find(oldholder->containerClid());
      if(it != m_helpers.end()){
       	ATH_MSG_VERBOSE("need to rewire this CLID from " << oldholder->typeClid() << "/"<< oldholder->containerClid() << " to " << it->second->xAODContainerClid());

	bool iselement = (oldholder->typeClid()==it->second->AODElementClid());
	bool iscontainer = (oldholder->typeClid()==it->second->AODContainerClid());
	
	ATH_MSG_VERBOSE("old is element feature " << iselement << " or container feature: " <<  iscontainer);
	
	if(!(iselement || iscontainer)){
	  ATH_MSG_WARNING("not clear what kind of feature we're dealing with. Skipping. Old holder is:" << *oldholder);
	}
	
	CLID newTypeClid = CLID_NULL;
	if(iselement)   newTypeClid = it->second->xAODElementClid();
	if(iscontainer) newTypeClid = it->second->xAODContainerClid();

	HLTNavDetails::IHolder* newholder = nav->m_holderstorage.getHolder<HLTNavDetails::IHolder>(newTypeClid,oldholder->label());
	
       	if(!newholder){
	  ATH_MSG_WARNING("could not find new holder for xAOD clid " <<  newTypeClid << " and label " << oldholder->label());
	  continue;
	}
	
       	ATH_MSG_VERBOSE("old holder: " << *oldholder);
       	ATH_MSG_VERBOSE("new holder: " << *newholder);
       	ATH_MSG_VERBOSE("changing CLID: " << featacchelp.getCLID() << " to " << newTypeClid); 
       	ATH_MSG_VERBOSE("changing STI: " << featacchelp.getIndex().subTypeIndex() << " to " << newholder->subTypeIndex()); 
	
       	featacchelp.setCLIDandIndex(newTypeClid,newholder->subTypeIndex());
       	ATH_MSG_VERBOSE("clid is now: " << featacchelp.getCLID() << " sti: " << featacchelp.getIndex().subTypeIndex());
	ATH_MSG_VERBOSE("TE is: " << *te);
      } // end if(helper was found) 
    }//end feature loop
  }//end TE loop


  return StatusCode::SUCCESS;
}



//blatantly stolen from navigation code
StatusCode TrigBStoxAODTool::classLabel2CLIDLabel(const std::vector<std::string>& property,
			    std::vector<std::pair<CLID,
			    std::string> >& decoded ) {
  // translate Class names into CLID numbers
  IClassIDSvc* clidSvc;
  if( service("ClassIDSvc", clidSvc).isFailure() ) {
    ATH_MSG_FATAL("Unable to get pointer to CLIDSvc Service");
    return StatusCode::FAILURE;
  }
  
  std::vector<std::string>::const_iterator it;
  for ( it = property.begin(); it != property.end(); ++it ) {
    CLID clid;
    std::string label;
    std::string type;
    
    if ( it->find("#") != std::string::npos ) {
	type = it->substr(0, it->find("#") );
	label = it->substr(it->find("#")+1 );
      } else {
	type = *it;
	label = "";
    }
    
    if ( clidSvc->getIDOfTypeName(type, clid).isFailure() ) {
      ATH_MSG_FATAL("Unable to get CLID for class: " << *it
		    << " check property");
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG("Recognized CLID : " << type << " and label: " << label);
    decoded.push_back(std::make_pair(clid, label));
  }
  return StatusCode::SUCCESS;
}
