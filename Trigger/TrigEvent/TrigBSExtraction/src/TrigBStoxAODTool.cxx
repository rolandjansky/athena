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

#include "TrigNavigation/TriggerElement.h"

#include "JetEvent/JetCollection.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "xAODTrigMissingET/TrigMissingETAuxContainer.h"

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

#include "tauEvent/TauJetContainer.h"
#include "xAODTau/TauJetContainer.h"

#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "egammaEvent/egammaContainer.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "xAODTrigger/TrigPassBits.h"
#include "xAODTrigger/TrigPassBitsContainer.h"
#include "TrigSteeringEvent/TrigPassBitsCollection.h"

#include "xAODBTagging/BTaggingContainer.h"
#include "TrigParticle/TrigEFBjetContainer.h"

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
    
    virtual StatusCode help(const std::string& label, const std::string& newLabel) = 0;

    //we need this method, because for creating converted xAOD::TrigPassBits, one needs access to the corresponding xaod container
    //with physics objects. This leads to a partial duplication of the code for the muon helper
    virtual StatusCode help_fillTrigPassBits(const std::string& label, const std::string& newLabel, const TrigPassBits* tpb_aod, xAOD::TrigPassBits*& tpb_xaod) = 0;

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
  template<typename AOD,typename XAOD, typename CnvTool>
  struct DefaultHelper : public ToolHolder<CnvTool> {
    typedef typename Container2Aux<XAOD>::type xAODAux;
    
    DefaultHelper(const ToolHandle<CnvTool>& tool) : ToolHolder<CnvTool>(tool){;}

    CLID AODContainerClid(){return getCLID<AOD>::ID();}    
    CLID xAODContainerClid(){return getCLID<XAOD>::ID();}

    CLID AODElementClid(){return getCLID<typename Container2Object<AOD>::type>::ID();}    
    CLID xAODElementClid(){return getCLID<typename Container2Object<XAOD>::type>::ID();}

    virtual StatusCode help(const std::string& label, const std::string& newLabel){
      typedef IHelper IH;
      std::string fmtkey_AOD = IH::template format<AOD>(label);
      std::string fmtkey_xAOD = IH::template format<XAOD>(newLabel);
      std::string fmtkey_xAODAux = IH::template format<xAODAux>(newLabel+"Aux.");

      const AOD* aod = this->m_sg->template tryConstRetrieve<AOD>(fmtkey_AOD);
      if(!aod){
	ATH_MSG_WARNING("AOD key: " <<  fmtkey_AOD << " not found for xAOD conversion");
	return StatusCode::SUCCESS;
      }
      
      ATH_MSG_DEBUG("attempting to convert " << fmtkey_AOD << " of size " << aod->size() << " to " << fmtkey_xAOD);

      XAOD* xaod = this->m_sg->template tryRetrieve<XAOD>(fmtkey_xAOD);
      if(!xaod){
	ATH_MSG_WARNING("xAOD key: " <<  fmtkey_xAOD << " not found for xAOD conversion");
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

    virtual StatusCode help_fillTrigPassBits(const std::string& label, const std::string& newLabel, const TrigPassBits* tpb_aod, xAOD::TrigPassBits*& tpb_xaod){
      typedef IHelper IH;
      std::string fmtkey_AOD = IH::template format<AOD>(label);
      std::string fmtkey_xAOD = IH::template format<XAOD>(newLabel);
      std::string fmtkey_xAODAux = IH::template format<xAODAux>(newLabel+"Aux.");

      const AOD* aod = this->m_sg->template tryConstRetrieve<AOD>(fmtkey_AOD);
      if(!aod){
	ATH_MSG_WARNING("AOD key: " <<  fmtkey_AOD << " not found");
	return StatusCode::SUCCESS;
      }
      ATH_MSG_DEBUG( "AOD container " << fmtkey_AOD << " extracted of size " << aod->size() );
      // it was found out that in Run 1 data for some containers the size of the TrigPassBits object differs from
      // the size of the corresponding container. It was decided to not convert TrigPassBits->xAOD::TrigPassBits for such cases,
      // so xAOD::TrigPassBits will be nullptr
      if( aod->size() != tpb_aod->size() ) {
	ATH_MSG_WARNING( "sizes of the container " << fmtkey_AOD << " and the corresponding (?) trigpassbits object are different: " 
			 << aod->size() << " vs " << tpb_aod->size() << ", will not convert TrigPassBits for this container" );
	return StatusCode::SUCCESS;
      } 

      XAOD* xaod = this->m_sg->template tryRetrieve<XAOD>(fmtkey_xAOD);
      if(!xaod){
	ATH_MSG_WARNING("xAOD key: " <<  fmtkey_xAOD << " not found");
	return StatusCode::SUCCESS;
      }
      ATH_MSG_DEBUG("xAOD container " << fmtkey_xAOD << " extracted of size " << xaod->size());
      
      if(aod->size() != xaod->size()) {
	ATH_MSG_ERROR("containers before and after the conversion are of different sizes");
	return StatusCode::FAILURE;
      }
      
      ATH_MSG_DEBUG( "setting xaod passbits size to " << tpb_aod->size() );
      std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits(xaod);

      for(uint i=0; i<aod->size(); i++) {
 	//	if(i < tpb_aod->size() ){
	  ATH_MSG_DEBUG( "looking at object " << i+1 << " of " << aod->size() );
	  bool passed = HLT::isPassing( tpb_aod, aod->at(i), aod );
	  ATH_MSG_DEBUG( "aod ispassing = " << passed );
	  ATH_MSG_DEBUG( "xaod ispassing = " << xBits->isPassing( i ) );
	  if(passed)
	    xBits->markPassing( i );
	  ATH_MSG_DEBUG( "xaod ispassing (after update) = " << xBits->isPassing( i ) );
	  /*	} else {
	  ATH_MSG_DEBUG( "skipping object " << i+1 << " as it is outside of the array range of the corresponding aod TrigPassBits object" );
	  }*/
      }
      tpb_xaod = xBits.release();
      return StatusCode::SUCCESS;
    }	  
  };
  
  struct MuonHelper : public ToolHolder<ITrigMuonEFInfoToMuonCnvTool> {
    MuonHelper(const ToolHandle<ITrigMuonEFInfoToMuonCnvTool>& tool) : ToolHolder(tool){;}

    CLID AODContainerClid(){return getCLID<TrigMuonEFInfoContainer>::ID();}
    CLID xAODContainerClid(){return getCLID<xAOD::MuonContainer>::ID();}

    CLID AODElementClid(){return getCLID<Container2Object<TrigMuonEFInfoContainer>::type>::ID();}
    CLID xAODElementClid(){return getCLID<Container2Object<xAOD::MuonContainer>::type >::ID();}

    virtual StatusCode help(const std::string& label, const std::string& newLabel){
      xAOD::MuonContainer* xaodMuon = this->m_sg->tryRetrieve<xAOD::MuonContainer>(format<xAOD::MuonContainer>(newLabel));
      if(!xaodMuon){
	ATH_MSG_WARNING("muon label: " << format<xAOD::MuonContainer>(newLabel) << " not found for xAOD conversion");
	return StatusCode::SUCCESS;
      }

      xAOD::TrackParticleContainer* xaodCombTrackParticles = this->m_sg->tryRetrieve<xAOD::TrackParticleContainer>(format<xAOD::TrackParticleContainer>(newLabel+"_CombTrackParticles"));
      if(!xaodCombTrackParticles){
	ATH_MSG_WARNING("muon label: " << format<xAOD::TrackParticleContainer>(newLabel+"_CombTrackParticles") << " not found for xAOD conversion");
	return StatusCode::SUCCESS;
      }

      xAOD::TrackParticleContainer* xaodExtrapTrackParticles = this->m_sg->tryRetrieve<xAOD::TrackParticleContainer>(format<xAOD::TrackParticleContainer>(newLabel+"_ExtrapTrackParticles"));
      if(!xaodCombTrackParticles){
	ATH_MSG_WARNING("muon label: " << format<xAOD::TrackParticleContainer>(newLabel+"_ExtrapTrackParticles") << " not found for xAOD conversion");
	return StatusCode::SUCCESS;
      }

      const TrigMuonEFInfoContainer* aod = 
	this->m_sg->tryConstRetrieve<TrigMuonEFInfoContainer>(format<TrigMuonEFInfoContainer>(label));
      if(!aod){
	ATH_MSG_WARNING("muon label: " << format<TrigMuonEFInfoContainer>(label) << " not found for xAOD conversion");
	return StatusCode::SUCCESS;
      }

      CHECK(this->m_tool->convertTrigMuonEFInfoContainer( *aod, *xaodMuon, xaodCombTrackParticles, xaodExtrapTrackParticles));

      ATH_MSG_DEBUG("Muon AOD container has size: " << aod->size());
      ATH_MSG_DEBUG("Muon xAOD container has size: " << xaodMuon->size());
      
      return StatusCode::SUCCESS;
    }

    virtual StatusCode help_fillTrigPassBits(const std::string& label, const std::string& newLabel, const TrigPassBits* tpb_aod, xAOD::TrigPassBits*& tpb_xaod){

      const TrigMuonEFInfoContainer* aod = this->m_sg->template tryConstRetrieve<TrigMuonEFInfoContainer>(label);
      if(!aod){
	ATH_MSG_WARNING("AOD key: " <<  label << " not found");
	return StatusCode::SUCCESS;
      }
      ATH_MSG_DEBUG( "AOD muon container " << label << " extracted of size " << aod->size() );
      if( aod->size() != tpb_aod->size() ) {
	ATH_MSG_WARNING( "sizes of the muon container " << label << " and the corresponding (?) trigpassbits object are different: "
			 << aod->size() << " vs " << tpb_aod->size() << ", will not convert TrigPassBits for this container" );
      }
      xAOD::MuonContainer* xaodMuon = this->m_sg->template tryRetrieve<xAOD::MuonContainer>(format<xAOD::MuonContainer>(newLabel));
      if(!xaodMuon){
	ATH_MSG_WARNING("xAOD key: " <<  format<xAOD::MuonContainer>(newLabel) << " not found");
	return StatusCode::SUCCESS;
      }
      ATH_MSG_DEBUG("xAOD muon container " << format<xAOD::MuonContainer>(newLabel) << " extracted of size " << xaodMuon->size());
      
      if(aod->size() != xaodMuon->size()) {
	ATH_MSG_ERROR("containers before and after the conversion are of different sizes");
	return StatusCode::FAILURE;
      }
      
      ATH_MSG_DEBUG( "setting muon xaod passbits size to " << tpb_aod->size() );
      std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits(xaodMuon);

      for(uint i=0; i<aod->size(); i++) {
	//	if(i < tpb_aod->size() ){
	  ATH_MSG_DEBUG( "looking at muon object " << i+1 << " of " << aod->size() );
	  bool passed = HLT::isPassing( tpb_aod, aod->at(i), aod );
	  ATH_MSG_DEBUG( "muon aod ispassing = " << passed );
	  ATH_MSG_DEBUG( "muon xaod ispassing = " << xBits->isPassing( i ) );
	  if(passed)
	    xBits->markPassing( i );
	  ATH_MSG_DEBUG( "muon xaod ispassing (after update) = " << xBits->isPassing( i ) );
	  /*	} else {
	  ATH_MSG_DEBUG( "skipping object " << i+1 << " as it is outside of the array range of the corresponding aod TrigPassBits object" );
	  }*/
      }
      tpb_xaod = xBits.release();
      return StatusCode::SUCCESS;
    }
  };

} // namespace BStoXAODHelper
  
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
    m_trigCaloClusterTool( "xAODMaker::TrigCaloClusterCnvTool/TrigCaloClusterCnvTool",this),
    m_emClusterTool(   "xAODMaker::TrigEMClusterCnvTool/TrigEMClusterCnvTool",this),
    m_bjetTool(        "xAODMaker::TrigBjetCnvTool/TrigBjetCnvTool",this),
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
    m_photonTool( "xAODMaker::PhotonCnvTool/PhotonCnvTool", this ),
    m_caloClusterTool( "xAODMaker::CaloClusterCnvTool/CaloClusterCnvTool", this),
    m_trigPassBitsTool( "xAODMaker::TrigPassBitsCnvTool/TrigPassBitsCnvTool", this )
{
  declareInterface<ITrigBStoxAODTool>( this );
  declareProperty("L2ResultKey",     m_l2ResultKey = "HLTResult_L2");
  declareProperty("EFResultKey",     m_efResultKey = "HLTResult_EF");
  declareProperty("HLTResultKey",    m_hltResultKey = "HLTResult_HLT");
  declareProperty("ContainersToConvert",m_containersToConvert);
  declareProperty("NewContainers",   m_newContainers);

  declareProperty("tauJetTool", m_tauJetTool);
  declareProperty("combMuonTool", m_combMuonTool);
  declareProperty("isoMuonTool", m_isoMuonTool);
  declareProperty("trigMuonTool", m_trigMuonTool);
  declareProperty("jetCnvTool", m_jetCnvTool);

  // not configurable algtools
  // declareProperty("ringerRingsTool", m_ringerRingsTool);
  // declareProperty("trigRNNTool", m_trigRNNTool);

  declareProperty("trigCaloClusterTool", m_trigCaloClusterTool);
  declareProperty("emClusterTool", m_emClusterTool);
  declareProperty("bjetTool", m_bjetTool);
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
  declareProperty("caloClusterTool", m_caloClusterTool);
  declareProperty("trigPassBitsTool", m_trigPassBitsTool);

  m_CLID_xAODPhotonContainer = getCLID<xAOD::PhotonContainer>::ID();
  m_CLID_xAODElectronContainer = getCLID<xAOD::ElectronContainer>::ID();
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
  /*
  m_helpers[ClassID_traits<CombinedMuonFeatureContainer>::ID()] = 
    new BStoXAODHelper::DefaultHelper<
      CombinedMuonFeatureContainer,xAOD::L2CombinedMuonContainer,xAODMaker::ICombinedMuonFeatureContainerCnvTool>(m_combMuonTool);

  m_helpers[ClassID_traits<TrigMuonEFInfoContainer>::ID()] = new BStoXAODHelper::MuonHelper(m_trigMuonTool);

  m_helpers[ClassID_traits<TrigCaloClusterContainer>::ID()] = 
    new BStoXAODHelper::DefaultHelper<
      TrigCaloClusterContainer,xAOD::TrigCaloClusterContainer,xAODMaker::ITrigCaloClusterCnvTool>(m_trigCaloClusterTool);

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

  m_helpers[ClassID_traits<CaloClusterContainer>::ID()] =
    new BStoXAODHelper::DefaultHelper<
  CaloClusterContainer,xAOD::CaloClusterContainer,xAODMaker::ICaloClusterCnvTool>(m_caloClusterTool);

  m_helpers[ClassID_traits<egammaContainer>::ID()] =
    new BStoXAODHelper::DefaultHelper<
    egammaContainer,xAOD::ElectronContainer,xAODMaker::IElectronCnvTool>(m_electronTool);

  m_helpers[ClassID_traits<egammaContainer>::ID()] =
    new BStoXAODHelper::DefaultHelper<
  egammaContainer,xAOD::PhotonContainer,xAODMaker::IPhotonCnvTool>(m_photonTool);

  m_helpers[ClassID_traits<TrigPassBitsCollection>::ID()] =
    new BStoXAODHelper::DefaultHelper<
  TrigPassBitsCollection,xAOD::TrigPassBitsContainer,xAODMaker::ITrigPassBitsCnvTool>(m_trigPassBitsTool);

  m_helpers[ClassID_traits<TrigEFBjetContainer>::ID()] = 
    new BStoXAODHelper::DefaultHelper<
  TrigEFBjetContainer,xAOD::BTaggingContainer,xAODMaker::ITrigBjetCnvTool>(m_bjetTool);
  */

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    CombinedMuonFeatureContainer,
		    xAOD::L2CombinedMuonContainer,
		    xAODMaker::ICombinedMuonFeatureContainerCnvTool>* >
		    (ClassID_traits<CombinedMuonFeatureContainer>::ID(), 
		     new BStoXAODHelper::DefaultHelper<
		     CombinedMuonFeatureContainer,
		     xAOD::L2CombinedMuonContainer,
		     xAODMaker::ICombinedMuonFeatureContainerCnvTool>(m_combMuonTool)) );

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::MuonHelper*>
		    (ClassID_traits<TrigMuonEFInfoContainer>::ID(),
		     new BStoXAODHelper::MuonHelper(m_trigMuonTool)) );

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    TrigCaloClusterContainer,
		    xAOD::TrigCaloClusterContainer,
		    xAODMaker::ITrigCaloClusterCnvTool>* >
		    (ClassID_traits<TrigCaloClusterContainer>::ID(), 
		     new BStoXAODHelper::DefaultHelper<
		     TrigCaloClusterContainer,
		     xAOD::TrigCaloClusterContainer,
		     xAODMaker::ITrigCaloClusterCnvTool>(m_trigCaloClusterTool)) );

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    TrigEMClusterContainer,
		    xAOD::TrigEMClusterContainer,
		    xAODMaker::ITrigEMClusterCnvTool>* >
		    (ClassID_traits<TrigEMClusterContainer>::ID(), 
		     new BStoXAODHelper::DefaultHelper<
		     TrigEMClusterContainer,
		     xAOD::TrigEMClusterContainer,
		     xAODMaker::ITrigEMClusterCnvTool>(m_emClusterTool)) );

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    TrigEFBphysContainer,
		    xAOD::TrigBphysContainer,
		    xAODMaker::ITrigEFBphysContainerCnvTool>* >
		    (ClassID_traits<TrigEFBphysContainer>::ID(), 
		     new BStoXAODHelper::DefaultHelper<
		     TrigEFBphysContainer,
		     xAOD::TrigBphysContainer,
		     xAODMaker::ITrigEFBphysContainerCnvTool>(m_efBphysTool)) );

   m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		     TrigL2BphysContainer,xAOD::TrigBphysContainer,xAODMaker::ITrigL2BphysContainerCnvTool>* >
		     (ClassID_traits<TrigL2BphysContainer>::ID(), 
		      new BStoXAODHelper::DefaultHelper<
		      TrigL2BphysContainer,xAOD::TrigBphysContainer,xAODMaker::ITrigL2BphysContainerCnvTool>(m_l2BphysTool)) );

   m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		     JetCollection,xAOD::JetContainer,xAODMaker::IJetCnvTool>* >
		     (ClassID_traits<JetCollection>::ID(), 
		      new BStoXAODHelper::DefaultHelper<
		      JetCollection,xAOD::JetContainer,xAODMaker::IJetCnvTool>(m_jetCnvTool)) );

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    TrigMissingETContainer,xAOD::TrigMissingETContainer,xAODMaker::ITrigMissingETCnvTool>* >
		    (ClassID_traits<TrigMissingETContainer>::ID(), 
		     new BStoXAODHelper::DefaultHelper<
		     TrigMissingETContainer,xAOD::TrigMissingETContainer,xAODMaker::ITrigMissingETCnvTool>(m_trigMetTool)) );

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    TrigSpacePointCountsCollection,xAOD::TrigSpacePointCountsContainer,xAODMaker::ITrigSpacePointCountsCnvTool>* >
		    (ClassID_traits<TrigSpacePointCountsCollection>::ID(), 
		     new BStoXAODHelper::DefaultHelper<
		     TrigSpacePointCountsCollection,xAOD::TrigSpacePointCountsContainer,xAODMaker::ITrigSpacePointCountsCnvTool>(m_trigSpacePtsTool)) );

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    TrigT2MbtsBitsContainer,xAOD::TrigT2MbtsBitsContainer,xAODMaker::ITrigT2MbtsBitsCnvTool>* >
		    (ClassID_traits<TrigT2MbtsBitsContainer>::ID(), 
		     new BStoXAODHelper::DefaultHelper<
		     TrigT2MbtsBitsContainer,xAOD::TrigT2MbtsBitsContainer,xAODMaker::ITrigT2MbtsBitsCnvTool>(m_trigMbtsBitsTool)) );

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    TrigTrackCountsCollection,xAOD::TrigTrackCountsContainer,xAODMaker::ITrigTrackCountsCnvTool>* >
		    (getCLID<TrigTrackCountsCollection>::ID(), 
		     new BStoXAODHelper::DefaultHelper<
		     TrigTrackCountsCollection,xAOD::TrigTrackCountsContainer,xAODMaker::ITrigTrackCountsCnvTool>(m_trigTrackCtsTool)) );

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    TrigVertexCountsCollection,xAOD::TrigVertexCountsContainer,xAODMaker::ITrigVertexCountsCnvTool>* >
		    (ClassID_traits<TrigVertexCountsCollection>::ID(), 
		     new BStoXAODHelper::DefaultHelper<
		     TrigVertexCountsCollection,xAOD::TrigVertexCountsContainer,xAODMaker::ITrigVertexCountsCnvTool>(m_trigVtxCtsTool)) );

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    TrackCollection,xAOD::TrackParticleContainer,xAODMaker::ITrackCollectionCnvTool>* >
		    (ClassID_traits<TrackCollection>::ID(),
		     new BStoXAODHelper::DefaultHelper<
		     TrackCollection,xAOD::TrackParticleContainer,xAODMaker::ITrackCollectionCnvTool>(m_trackCollectionTool)) );
  
  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    Rec::TrackParticleContainer,xAOD::TrackParticleContainer,xAODMaker::IRecTrackParticleContainerCnvTool>* >
		    (ClassID_traits<Rec::TrackParticleContainer>::ID(),
		     new BStoXAODHelper::DefaultHelper<
		     Rec::TrackParticleContainer,xAOD::TrackParticleContainer,xAODMaker::IRecTrackParticleContainerCnvTool>(m_recTrackParticleContTool)) );
  
  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    Analysis::TauJetContainer,xAOD::TauJetContainer,xAODMaker::ITauJetCnvTool>* >
		    (ClassID_traits<Analysis::TauJetContainer>::ID(),
		     new BStoXAODHelper::DefaultHelper<
		     Analysis::TauJetContainer,xAOD::TauJetContainer,xAODMaker::ITauJetCnvTool>(m_tauJetTool)) );

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    CaloClusterContainer,xAOD::CaloClusterContainer,xAODMaker::ICaloClusterCnvTool>* >
		    (ClassID_traits<CaloClusterContainer>::ID(),
		     new BStoXAODHelper::DefaultHelper<
		     CaloClusterContainer,xAOD::CaloClusterContainer,xAODMaker::ICaloClusterCnvTool>(m_caloClusterTool)) );

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    egammaContainer,xAOD::ElectronContainer,xAODMaker::IElectronCnvTool>* >
		    (ClassID_traits<egammaContainer>::ID(),
		     new BStoXAODHelper::DefaultHelper<
		     egammaContainer,xAOD::ElectronContainer,xAODMaker::IElectronCnvTool>(m_electronTool)) );

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    egammaContainer,xAOD::PhotonContainer,xAODMaker::IPhotonCnvTool>* >
		    (ClassID_traits<egammaContainer>::ID(),
		     new BStoXAODHelper::DefaultHelper<
		     egammaContainer,xAOD::PhotonContainer,xAODMaker::IPhotonCnvTool>(m_photonTool)) );

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    TrigPassBitsCollection,xAOD::TrigPassBitsContainer,xAODMaker::ITrigPassBitsCnvTool>* >
		    (ClassID_traits<TrigPassBitsCollection>::ID(),
		     new BStoXAODHelper::DefaultHelper<
		     TrigPassBitsCollection,xAOD::TrigPassBitsContainer,xAODMaker::ITrigPassBitsCnvTool>(m_trigPassBitsTool)) );

  m_helpers.insert( std::pair<CLID,BStoXAODHelper::DefaultHelper<
		    TrigEFBjetContainer,xAOD::BTaggingContainer,xAODMaker::ITrigBjetCnvTool>* >
		    (ClassID_traits<TrigEFBjetContainer>::ID(), 
		     new BStoXAODHelper::DefaultHelper<
		     TrigEFBjetContainer,xAOD::BTaggingContainer,xAODMaker::ITrigBjetCnvTool>(m_bjetTool)) );

  
  std::multimap<CLID,BStoXAODHelper::IHelper*>::iterator it;
  for(it = m_helpers.begin();it!=m_helpers.end();++it){
    CHECK(it->second->initialize(this->msg(),*(evtStore())));
  }

  ATH_MSG_DEBUG("containers to convert: " << m_containersToConvert);
  ATH_MSG_DEBUG("containers will be converted to: " << m_newContainers);
  CHECK(classLabel2CLIDLabel(m_containersToConvert,m_newContainers,m_clid_labels,m_clid_newLabels));
  ATH_MSG_DEBUG("extracted class IDs (" << m_clid_labels.size() << " in total): " << m_clid_labels);

  if(m_clid_labels.size() != m_clid_newLabels.size()) {
    ATH_MSG_ERROR("number of new containers names (" << m_clid_newLabels.size() 
		  << ") is not the same as the number of old containers names ("
		  << m_clid_newLabels.size() << ")");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("CLID CLID, old label, new label:");
  for(uint i=0; i<m_clid_labels.size(); i++) {
    ATH_MSG_DEBUG(m_clid_labels[i].first << " " << m_clid_newLabels[i].first << ", "
		  << m_clid_labels[i].second << ", " << m_clid_newLabels[i].second );
    if(m_clid_labels[i].second != m_clid_newLabels[i].second) {
      ATH_MSG_VERBOSE("Run-1 " << m_clid_labels[i].first << " label " <<
		      m_clid_labels[i].second << " differs from the equivalent Run-2 label: "
		      << m_clid_newLabels[i].second);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigBStoxAODTool::convert(HLT::Navigation* nav) {
    std::vector<std::pair<CLID,std::string> >::const_iterator clidlabel;
    std::vector<std::pair<CLID,std::string> >::const_iterator clidNewLabel = m_clid_newLabels.begin();
    for(clidlabel =m_clid_labels.begin();clidlabel!=m_clid_labels.end();++clidlabel, ++clidNewLabel){

      HLTNavDetails::IHolder* holder = nav->getHolder(clidlabel->first,clidlabel->second);
      
      if(!holder){
	ATH_MSG_WARNING("couldn't find holder for " << clidlabel->first << " " << clidlabel->second);
	continue;
      }

      ATH_MSG_VERBOSE("container corresponding to feature CLID " << clidlabel->first << " has CLID " << holder->containerClid());
      
      std::multimap<CLID,BStoXAODHelper::IHelper*>::const_iterator hit;
      hit = m_helpers.end();//find(holder->containerClid());

      std::string clabel = clidlabel->second;
      CHECK(findHelper( m_helpers, holder->containerClid(), clabel, hit ));
      
      if(hit!=m_helpers.end()){
	ATH_MSG_DEBUG("attempting convertion of clid " << holder->containerClid() 
		      << " for label " << clidlabel->second << ", new label " << clidNewLabel->second);
	CHECK(hit->second->help(clidlabel->second,clidNewLabel->second));//get aod container and convert
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
      
      std::multimap<CLID,BStoXAODHelper::IHelper*>::const_iterator it = m_helpers.end();//find(oldholder->containerClid());
      std::string old_label = oldholder->label();
      CHECK(findHelper( m_helpers, oldholder->containerClid(), old_label, it));
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
						  const std::vector<std::string>& newProperty,
						  std::vector<std::pair<CLID,std::string> >& decoded,
						  std::vector<std::pair<CLID,std::string> >& decodedNewNames) {
  // translate Class names into CLID numbers
  IClassIDSvc* clidSvc;
  if( service("ClassIDSvc", clidSvc).isFailure() ) {
    ATH_MSG_FATAL("Unable to get pointer to CLIDSvc Service");
    return StatusCode::FAILURE;
  }
  if( property.size() != newProperty.size() ) {
    ATH_MSG_ERROR("vector sizes of new and old SG labels are different: "
		  << newProperty.size() << " vs " << property.size() );
    return StatusCode::FAILURE;
  }

  std::vector<std::string>::const_iterator it;
  std::vector<std::string>::const_iterator itNew = newProperty.begin();
  for ( it = property.begin(); it != property.end(); ++it, ++itNew ) {
    CLID clid;
    std::string label;
    std::string label_new;
    std::string type;
    
    if ( it->find("#") != std::string::npos ) {
	type = it->substr(0, it->find("#") );
	label = it->substr(it->find("#")+1 );
      } else {
	type = *it;
	label = "";
    }
    
    if ( itNew->find("#") != std::string::npos ) {
      label_new = itNew->substr(itNew->find("#")+1 );
    } else {
      label_new = "";
    }

    if ( clidSvc->getIDOfTypeName(type, clid).isFailure() ) {
      ATH_MSG_FATAL("Unable to get CLID for class: " << *it
		    << " check property");
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG("Recognized CLID : " << type << " and label: " << label << ", new label will be: " << label_new);
    decoded.push_back(std::make_pair(clid, label));
    decodedNewNames.push_back(std::make_pair(clid, label_new));
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigBStoxAODTool::setTrigPassBits(HLT::Navigation* nav) {
  //we need to loop over all features of all TEs
  //find the ones pointing to a AOD container that has been converted
  //extract the corresponding aod TrigPassBits container and set the same
  //bits in xAOD::TrigPassBits

  std::vector<HLT::TriggerElement*>& allTEs = nav->getAllTEs();

  ATH_MSG_DEBUG("setting xAOD TrigPassBits. Working on " << allTEs.size() << " TEs");

  for(auto te : allTEs){
    std::vector<HLT::TriggerElement::FeatureAccessHelper>& feats = te->getFeatureAccessHelpers();
    for(auto& featacchelp : feats){
      ATH_MSG_VERBOSE("this feature has CLID: " << featacchelp.getCLID());
      
      HLTNavDetails::IHolder* holder = nav->getHolder(featacchelp.getCLID(),featacchelp.getIndex().subTypeIndex());
      
      if(!holder){
	ATH_MSG_WARNING("could not find holder for CLID " << featacchelp.getCLID() << " and subTypeIndex: " << featacchelp.getIndex());
	continue;
      }

      if(holder->label() == "passbits") {
	ATH_MSG_VERBOSE( "skipping this feature, as this is the passbits object itself" );
	continue;
      }
      
      std::multimap<CLID,BStoXAODHelper::IHelper*>::const_iterator it = m_helpers.end();//find(holder->containerClid());
      std::string hlabel = holder->label();
      CHECK(findHelper( m_helpers, holder->containerClid(), hlabel, it ));
      if(it != m_helpers.end()){

	ATH_MSG_DEBUG( "need to extract TrigPassBits for this AOD object " << holder->typeClid() << "/"<< holder->containerClid() 
		       << ", label " << holder->label() << " corresponding to xAOD CLID " << it->second->xAODContainerClid() );

	bool iselement = (holder->typeClid()==it->second->AODElementClid());
	bool iscontainer = (holder->typeClid()==it->second->AODContainerClid());
	
	ATH_MSG_VERBOSE("is element feature " << iselement << " or container feature: " <<  iscontainer);
	
	if(!(iselement || iscontainer)){
	  ATH_MSG_WARNING("not clear what kind of feature we're dealing with. Skipping. The holder is:" << *holder);
	}
	
       	ATH_MSG_VERBOSE("holder: " << *holder);

	const TrigPassBitsCollection* bits_collection = evtStore()->template tryConstRetrieve<TrigPassBitsCollection>("HLT_TrigPassBitsCollection_passbits");
	if(bits_collection != nullptr) {
	  ATH_MSG_DEBUG( "aod bits collection extracted of size " << bits_collection->size() );
	  //dump this to compare with the results from release 17
	  //	  for(uint i=0; i<bits_collection->size(); i++ ) {
	  //	    std::cout << "size of the TrigPassBits object " << i << " is " << bits_collection->at(i)->size() << std::endl;
	  //	  }
	} else {
	  ATH_MSG_WARNING( "aod bits collection cannot be extracted" );
	  continue;
	}


	xAOD::TrigPassBitsContainer* xaod_bits_collection = evtStore()->template tryRetrieve<xAOD::TrigPassBitsContainer>("HLT_xAOD__TrigPassBitsContainer_passbits");
	if(xaod_bits_collection != nullptr)
	  ATH_MSG_DEBUG( "xaod bits container extracted of size " << xaod_bits_collection->size() );
	else {
	  ATH_MSG_WARNING( "xaod bits container cannot be extracted" );
	  continue;
	}

	const TrigPassBits* bits(0);
	if ( nav->getFeature(te, bits, "passbits" ) ){
	  ATH_MSG_DEBUG( "TrigPassBits extracted for the container " << holder->label() );
	  if(bits!=nullptr) {
	    ATH_MSG_DEBUG( "it's size is " << bits->size() );
	  } else {
	    ATH_MSG_DEBUG( "it is nullptr" );
	    continue;
	  }
	} else {
	  ATH_MSG_WARNING( "this TE has no associated TrigPassBits object" );
	  continue;
	}
	if(bits_collection != nullptr && xaod_bits_collection != nullptr && bits != nullptr) {
	  int bits_nr = -1;
	  for(uint i=0; i<bits_collection->size(); i++) {
	    if(bits == bits_collection->at(i)) {
	      bits_nr = i;
	      ATH_MSG_VERBOSE( "this is bits # " << bits_nr );
	      break;
	    }
	  }
	  if(bits_nr >= 0) {
	    xAOD::TrigPassBits* xaod_tpb = nullptr;
	    std::pair<CLID,std::string> pair = std::make_pair( featacchelp.getCLID(), holder->label() );
	    std::vector<std::pair<CLID,std::string> >::const_iterator it_new_label;
	    it_new_label = std::find(m_clid_newLabels.begin(), m_clid_newLabels.end(), pair);
	    std::string new_label = "";
	    if(it_new_label != m_clid_newLabels.end()) {
	      new_label = (*it_new_label).second;
	      ATH_MSG_DEBUG( "calling help_fillTrigPassBits for TE " << te << ", feature CLID " << featacchelp.getCLID() 
			     << ", aod container " << holder->label() << ", new container name: " << new_label );
	      CHECK( it->second->help_fillTrigPassBits( holder->label(), new_label,  bits_collection->at(bits_nr), xaod_tpb ) );
	    } else {
	      ATH_MSG_DEBUG( "new label was not found, presumably this container was not converted. Will not look for TrigPassBits" );
	    }
	    
	    if(xaod_tpb != nullptr) {
	      xaod_bits_collection->at(bits_nr) = xaod_tpb;
	      ATH_MSG_VERBOSE( "after the conversion, size of xAOD::TrigPassBits is " << xaod_bits_collection->at(bits_nr)->size() );
	    }

	  } else {
	    ATH_MSG_WARNING( "TrigPassBits object was not identified within container!" );
	  }
	}// if good aod and xaod passbits are available
      } // if helper was found
    }// feature loop
  }// TE loop


  return StatusCode::SUCCESS;
}

// the following function returns iterator pointing to the desired converter, based on the clid of the AOD type and label
StatusCode TrigBStoxAODTool::findHelper( MM_CLID_IHELPER& helpers, CLID clid, std::string& label, MM_CLID_IHELPER::const_iterator& it ) {
  auto myrange = helpers.equal_range(clid);
  it = myrange.first;
  //let's see how many there are converters corresponding to the same AOD input type
  int distance = std::distance(myrange.first, myrange.second);
  if (distance == 1) {
    //default case, one-to-one relation of AOD and xAOD types
    ATH_MSG_VERBOSE("helper to convert AOD CLID " << clid << " and label " << label << " found");
    return StatusCode::SUCCESS;
  } else if (distance == 2) {
    //this is for the case when there is single AOD type converted to two different xAOD types,
    //will treat this here separately. So far this only happens for egammaContainer->xAOD::ElectronContainer and
    //egammaContainer->xAOD::PhotonContainer
    for(;it!=myrange.second;++it) {
      CLID myHelperxAODCLID = (*it).second->xAODContainerClid();
      if((label == "egamma_Electrons" && myHelperxAODCLID == m_CLID_xAODElectronContainer) ||
	 (label == "egamma_Photons" && myHelperxAODCLID == m_CLID_xAODPhotonContainer)) {
	ATH_MSG_VERBOSE("helper to convert AOD CLID " << clid << " and label " << label << " found");
	return StatusCode::SUCCESS;
      }
    }
  } else if( distance == 0) {
    ATH_MSG_WARNING("there is no helper to convert AOD CLID " << clid << ", label " << label);
    it = helpers.end();
    return StatusCode::SUCCESS;
  } else {
    //this should not happen...
    ATH_MSG_ERROR("found " << distance << " helpers to convert AOD type " << clid << " and label " << label
		  << ", probably you should accomodate for this change in the code above" );
    return StatusCode::FAILURE;
  }

  //this point is not reached if everything is OK
  return StatusCode::SUCCESS;

} // MM_CLID_IHELPER::const_iterator findHelper( ... )
