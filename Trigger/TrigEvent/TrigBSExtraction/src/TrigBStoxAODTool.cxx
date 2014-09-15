// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBSExtraction/TrigBStoxAODTool.h"

#include "TrigSteeringEvent/HLTResult.h"
#include "TrigNavigation/NavigationCore.h"

#include "TrigNavigation/Holder.h"
#include "GaudiKernel/ClassID.h"

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

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

// #include "CaloEvent/CaloClusterContainer.h"
// #include "xAODCaloEvent/CaloClusterContainer.h"
// #include "xAODCaloEvent/CaloClusterAuxContainer.h"

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

    virtual CLID AODclid() = 0;
    virtual CLID xAODclid() = 0;

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
  template<typename AOD,typename xAOD,typename xAODAux, typename CnvTool>
  struct DefaultHelper : public ToolHolder<CnvTool> {
    DefaultHelper(const ToolHandle<CnvTool>& tool) : ToolHolder<CnvTool>(tool){;}

    CLID AODclid(){return ClassID_traits<AOD>::ID();}    
    CLID xAODclid(){return ClassID_traits<xAOD>::ID();}

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

    CLID AODclid(){return ClassID_traits<TrigMuonEFInfoContainer>::ID();}
    CLID xAODclid(){return ClassID_traits<xAOD::MuonContainer>::ID();}

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

//   struct CaluClusterHelper : public ToolHolder<ICaloClusterCnvTool> {
//     CaluClusterHelper(const std::string& toolname) : ToolHolder(toolname){;}
//     virtual StatusCode help(const std::string& label){
//       std::string aodName = std::string("HLT_")+label);
//       std::string xaodName = std::string("xAOD_")+label);

//       // Retrieve the AOD clusters:
//       const CaloClusterContainer* aod = 0;
//       CHECK( this->m_sg->retrieve( aod, aodName ) );
      
//       // Create new Cell-Link container:
//       const std::string ccclc_name = xaodName + "_links";
//       CaloClusterCellLinkContainer* ccclc = new CaloClusterCellLinkContainer();
      
//       // Create the xAOD container and its auxiliary store:
//       xAOD::CaloClusterContainer* xaod = new xAOD::CaloClusterContainer();
//       xAOD::CaloClusterAuxContainer* aux = new xAOD::CaloClusterAuxContainer();
//       xaod->setStore( aux );
//       // Create the xAOD objects:
//       CaloClusterContainer::const_iterator itr = aod->begin();
//       CaloClusterContainer::const_iterator end = aod->end();
//       for(unsigned i = 0 ; itr != end; ++itr, ++i ) {
// 	// Create the xAOD object:
// 	xAOD::CaloCluster* cluster = new xAOD::CaloCluster();
// 	xaod->push_back( cluster );
	
// 	// Leave the conversion up to the tool:
// 	CHECK( this->m_tool->convert( *itr, cluster, ccclc ) );
//       } // end loop over clusters
      
//       // Record all the new containers:
//       CHECK( this->m_sg->record( ccclc, ccclc_name ) );
//       CHECK( this->m_sg->record( xaod, xaodName ) );
//       CHECK( this->m_sg->record( aux, xaodName + "Aux." ) );
//     } // end loop over cluster containers    

//     return StatusCode::SUCCESS;
//   }
// }
  
}
  
TrigBStoxAODTool::TrigBStoxAODTool(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent),
    m_trigmettool("xAODMaker::TrigMissingETCnvTool/TrigMetTool",this),
    m_trigmuontool("TrigMuonEFInfoToMuonCnvTool/TrigMuonTool",this),
    //m_photontool("xAODMaker::PhotonCnvTool/PhotonCnvTool",this),
    m_jettool("xAODMaker::JetCnvTool/JetTool",this)
{
  declareInterface<ITrigBStoxAODTool>( this );
  declareProperty("L2ResultKey",     m_l2ResultKey = "HLTResult_L2");
  declareProperty("EFResultKey",     m_efResultKey = "HLTResult_EF");
  declareProperty("HLTResultKey",     m_hltResultKey = "HLTResult_HLT");
  declareProperty("ContainersToConvert",m_containersToConvert);
  declareProperty("TrigMuonTool", m_trigmuontool);
  declareProperty("TrigMetTool", m_trigmettool);
  declareProperty("JetTool",m_jettool);
  // declareProperty("TrigPhotonTool",m_trigphotontool);
  // declareProperty("ConvertibleContainers", m_convertible);
}

TrigBStoxAODTool::~TrigBStoxAODTool() {
  std::map<CLID,BStoXAODHelper::IHelper*>::iterator it;
  for(it = m_helpers.begin();it!=m_helpers.end();++it){
    delete it->second;
  }
}

StatusCode TrigBStoxAODTool::initialize(){
  m_helpers[1162448536] = new BStoXAODHelper::DefaultHelper<JetCollection,xAOD::JetContainer,xAOD::JetAuxContainer,xAODMaker::IJetCnvTool>(m_jettool);
  m_helpers[1271945976] = new BStoXAODHelper::DefaultHelper<TrigMissingETContainer,xAOD::TrigMissingETContainer,xAOD::TrigMissingETAuxContainer,xAODMaker::ITrigMissingETCnvTool>(m_trigmettool);
  //m_helpers[1296898266] = new BStoXAODHelper::DefaultHelper<TrigPhotonContainer,xAOD::TrigPhotonContainer,xAOD::TrigPhotonAuxContainer,xAODMaker::ITrigPhotonCnvTool>("xAODMaker::TrigPhotonCnvTool/PhotonCnvTool");
  m_helpers[1173853172] = new BStoXAODHelper::MuonHelper(m_trigmuontool);
  
  std::map<CLID,BStoXAODHelper::IHelper*>::iterator it;
  for(it = m_helpers.begin();it!=m_helpers.end();++it){
    CHECK(it->second->initialize(this->msg(),*(evtStore())));
  }

  ATH_MSG_DEBUG("containers to convert: " << m_containersToConvert);
  CHECK(classLabel2CLIDLabel(m_containersToConvert,m_clid_labels));
  ATH_MSG_DEBUG("extracted class IDs " << m_clid_labels);

  return StatusCode::SUCCESS;
}

StatusCode TrigBStoxAODTool::convert() {
    std::vector<std::pair<CLID,std::string> >::const_iterator clidlabel;
    for(clidlabel =m_clid_labels.begin();clidlabel!=m_clid_labels.end();++clidlabel){
      std::map<CLID,BStoXAODHelper::IHelper*>::iterator hit = m_helpers.find(clidlabel->first);
      if(hit!=m_helpers.end()){
	ATH_MSG_DEBUG("attempting convertion of clid " << clidlabel->first << " for label " << clidlabel->second);
	CHECK(hit->second->help(clidlabel->second));//get aod container and convert
	ATH_MSG_DEBUG("converted clid " << clidlabel->first << " for label " << clidlabel->second);
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
      auto it = m_helpers.find(featacchelp.getCLID());
      if(it != m_helpers.end()){
	ATH_MSG_VERBOSE("need to rewire this CLID from " << it->second->AODclid() << " to " << it->second->xAODclid());

	HLTNavDetails::IHolder* oldholder = nav->m_featuresByIndex[it->second->AODclid()][featacchelp.getIndex().subTypeIndex()];
	HLTNavDetails::IHolder* newholder = nav->m_featuresByLabel[it->second->xAODclid()][oldholder->label()];

	if(!oldholder || !newholder){
	  ATH_MSG_WARNING("either old holder " << oldholder << " or new holder " << newholder << " not found for AOD clid: " << it->second->AODclid() << " and xAOD clid " <<  it->second->xAODclid());
	    continue;
	  }
	
	ATH_MSG_VERBOSE("old holder: " << *oldholder);
	ATH_MSG_VERBOSE("new holder: " << *newholder);
	ATH_MSG_VERBOSE("changing CLID: " << featacchelp.getCLID() << " to " << it->second->xAODclid()); 
	ATH_MSG_VERBOSE("changing STI: " << featacchelp.getIndex().subTypeIndex() << " to " << newholder->subTypeIndex()); 
	
	featacchelp.setCLIDandIndex(it->second->xAODclid(),newholder->subTypeIndex());
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
