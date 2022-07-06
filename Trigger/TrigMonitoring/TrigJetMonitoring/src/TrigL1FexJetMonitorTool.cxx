// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL1FexJetMonitorTool.h"
#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AsgDataHandles/ReadDecorHandle.h"

#include "./JTMContainers.h"
#include "./L1MonitorAdaptors.h"
#include "./DataStructs.h"
#include <memory>

/////////////////////////////////////////////////////////////////////////////

template<typename T>
TrigL1FexJetMonitorTool<T>::TrigL1FexJetMonitorTool(const std::string& type,
						      const std::string& name,
						      const IInterface* parent) 
  : AthAlgTool( type, name, parent )
{
  declareProperty("l1container", m_l1jetContainerkey);

}


template<typename T>
StatusCode
TrigL1FexJetMonitorTool<T>::queryInterface( const InterfaceID& riid, void** ppvIf )
{
  if ( riid == ITrigJetMonitorTool::interfaceID() )  {
    *ppvIf = (ITrigJetMonitorTool*)this;
    addRef();
    return StatusCode::SUCCESS;
  }

  return AthAlgTool::queryInterface( riid, ppvIf );
}



template<typename T>
StatusCode TrigL1FexJetMonitorTool<T>::initialize()
{
  ATH_CHECK(m_l1jetContainerkey.initialize());

  if (m_doMatching) {
    m_offmatchedKey =
      m_l1jetContainerkey.key() + ".matched_" + m_offlineJetContainerToMatch;
    
    m_offptdiffKey =
      m_l1jetContainerkey.key() + ".ptdiff_" + m_offlineJetContainerToMatch;
    
    m_offenergydiffKey =
      m_l1jetContainerkey.key() + ".energydiff_" + m_offlineJetContainerToMatch;
    
    m_offmassdiffKey =
      m_l1jetContainerkey.key() + ".massdiff_" + m_offlineJetContainerToMatch;
    
    m_offptrespKey =
      m_l1jetContainerkey.key() + ".ptresp_" + m_offlineJetContainerToMatch;
    
    m_offenergyrespKey =
      m_l1jetContainerkey.key() + ".energyresp_" + m_offlineJetContainerToMatch;
    
    m_offmassrespKey =
      m_l1jetContainerkey.key() + ".massresp_" + m_offlineJetContainerToMatch;
    
    m_offptrefKey =
      m_l1jetContainerkey.key() + ".ptRef_" + m_offlineJetContainerToMatch;
    
    m_offetarefKey =
      m_l1jetContainerkey.key() + ".etaRef_" + m_offlineJetContainerToMatch;

    ATH_CHECK(m_offmatchedKey.initialize() );
    ATH_CHECK(m_offptdiffKey.initialize() );
    ATH_CHECK(m_offenergydiffKey.initialize() );
    ATH_CHECK(m_offmassdiffKey.initialize() );
    ATH_CHECK(m_offptrespKey.initialize() );
    ATH_CHECK(m_offenergyrespKey.initialize() );
    ATH_CHECK(m_offmassrespKey.initialize() );
    ATH_CHECK(m_offptrefKey.initialize() );
    ATH_CHECK(m_offetarefKey.initialize() );
    
    m_hltmatchedKey =
      m_l1jetContainerkey.key() + ".matched_" + m_HLTJetContainerToMatch;
    
    m_hltptdiffKey =
      m_l1jetContainerkey.key() + ".ptdiff_" + m_HLTJetContainerToMatch;
    
    m_hltenergydiffKey =
      m_l1jetContainerkey.key() + ".energydiff_" + m_HLTJetContainerToMatch;
    
    m_hltmassdiffKey =
      m_l1jetContainerkey.key() + ".massdiff_" + m_HLTJetContainerToMatch;
    
    m_hltptrespKey =
      m_l1jetContainerkey.key() + ".ptresp_" + m_HLTJetContainerToMatch;
    
    m_hltenergyrespKey =
      m_l1jetContainerkey.key() + ".energyresp_" + m_HLTJetContainerToMatch;
    
    m_hltmassrespKey =
      m_l1jetContainerkey.key() + ".massresp_" + m_HLTJetContainerToMatch;
    
    m_hltptrefKey =
      m_l1jetContainerkey.key() + ".ptRef_" + m_HLTJetContainerToMatch;
    
    m_hltetarefKey =
      m_l1jetContainerkey.key() + ".etaRef_" + m_HLTJetContainerToMatch;
    
    ATH_CHECK(m_hltmatchedKey.initialize());
    ATH_CHECK(m_hltptdiffKey.initialize());
    ATH_CHECK(m_hltenergydiffKey.initialize());
    ATH_CHECK(m_hltmassdiffKey.initialize());
    ATH_CHECK(m_hltptrespKey.initialize());
    ATH_CHECK(m_hltenergyrespKey.initialize());
    ATH_CHECK(m_hltmassrespKey.initialize());
    ATH_CHECK(m_hltptrefKey.initialize());
    ATH_CHECK(m_hltetarefKey.initialize());

  } else {
    // declare the keys unused if no matching is configured

    // offline container matching
    ATH_CHECK(m_offmatchedKey.initialize(false));
    ATH_CHECK(m_offptdiffKey.initialize(false));
    ATH_CHECK(m_offenergydiffKey.initialize(false));
    ATH_CHECK(m_offmassdiffKey.initialize(false));
    ATH_CHECK(m_offptrespKey.initialize(false));
    ATH_CHECK(m_offenergyrespKey.initialize(false));
    ATH_CHECK(m_offmassrespKey.initialize(false));
    ATH_CHECK(m_offptrefKey.initialize(false));
    ATH_CHECK(m_offetarefKey.initialize(false));

    // trigger container matching
    ATH_CHECK(m_hltmatchedKey.initialize(false));
    ATH_CHECK(m_hltptdiffKey.initialize(false));
    ATH_CHECK(m_hltenergydiffKey.initialize(false));
    ATH_CHECK(m_hltmassdiffKey.initialize(false));
    ATH_CHECK(m_hltptrespKey.initialize(false));
    ATH_CHECK(m_hltenergyrespKey.initialize(false));
    ATH_CHECK(m_hltmassrespKey.initialize(false));
    ATH_CHECK(m_hltptrefKey.initialize(false));
    ATH_CHECK(m_hltetarefKey.initialize(false));
  }
    
  return StatusCode::SUCCESS;
}



template<typename T>
StatusCode
TrigL1FexJetMonitorTool<T>::getData(const EventContext& ctx,
				      std::vector<JetData>& jetData
				      ) const{

  
  // Retrieve the L1 jet container
  SG::ReadHandle<JetContainer> jets(m_l1jetContainerkey, ctx);
  if( !jets.isValid() ){
    // the L1 containers should _always_ be present, although may be empty.
    ATH_MSG_WARNING("evtStore() does not contain the L1 jet collection with name "
		    << m_l1jetContainerkey);
    return StatusCode::SUCCESS;
  }
  
  // find variables associated with jets in the input container.
  for(const auto jet : *jets){
    /*
      Ensure that the the et is above a hardware threshold:
      From Sergi Rodriguez 23/02/2022
      in the bitwise simulation we also have an energy threshold, 
      which is taken from the trigger menu.
      If the Et is below the threshold the TOB word is set to 0
    */
    
    if (vetoJet(jet)) {continue;}  // see L1MonitoredAdaptors
    
    jetData.emplace_back(et(jet)*0.001,  // see L1MonitoredAdaptors
			 jet->eta(),
			 jet->phi(),
			 et_label(jet));
    
  }
  return StatusCode::SUCCESS;
}



template<typename T>
StatusCode
TrigL1FexJetMonitorTool<T>::getMatchData(const EventContext& ctx,
					   MatchToEnum matchTo,
					   std::vector<JetMatchData>& jetMatchData
					   ) const {
  if (!m_doMatching) {
    // otherwise will attempt to use uniniatialied Keys
    return StatusCode::SUCCESS;
  }

  std::unique_ptr<SG::ReadDecorHandle<JetContainer, char>>  matchedHandle;
  std::unique_ptr<SG::ReadDecorHandle<JetContainer, double>> ptdiffHandle;
  std::unique_ptr<SG::ReadDecorHandle<JetContainer, double>> energydiffHandle;
  std::unique_ptr<SG::ReadDecorHandle<JetContainer, double>> massdiffHandle;
  std::unique_ptr<SG::ReadDecorHandle<JetContainer, double>> ptrespHandle;
  std::unique_ptr<SG::ReadDecorHandle<JetContainer, double>> energyrespHandle;
  std::unique_ptr<SG::ReadDecorHandle<JetContainer, double>> massrespHandle;
  std::unique_ptr<SG::ReadDecorHandle<JetContainer, double>> ptrefHandle;
  std::unique_ptr<SG::ReadDecorHandle<JetContainer, double>> etarefHandle;

  if (matchTo == MatchToEnum::hlt) {
 
    matchedHandle.reset(new SG::ReadDecorHandle<JetContainer, char> (m_hltmatchedKey, ctx));
    ptdiffHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_hltptdiffKey, ctx));
    energydiffHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_hltenergydiffKey, ctx));
    massdiffHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_hltmassdiffKey, ctx));
    ptrespHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_hltptrespKey, ctx));
    energyrespHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_hltenergyrespKey, ctx));
    massrespHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_hltmassrespKey, ctx));
    ptrefHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_hltptrefKey, ctx));
    etarefHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_hltetarefKey, ctx));
  } else if (matchTo == MatchToEnum::offline) {
    matchedHandle.reset(new SG::ReadDecorHandle<JetContainer, char> (m_offmatchedKey, ctx));
    ptdiffHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_offptdiffKey, ctx));
    energydiffHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_offenergydiffKey, ctx));
    massdiffHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_offmassdiffKey, ctx));
    ptrespHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_offptrespKey, ctx));
    energyrespHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_offenergyrespKey, ctx));
    massrespHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_offmassrespKey, ctx));
    ptrefHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_offptrefKey, ctx));
    etarefHandle.reset(new SG::ReadDecorHandle<JetContainer, double> (m_offetarefKey, ctx));
  } else {
    ATH_MSG_ERROR ("unsupported MatchTo value");
    return StatusCode::FAILURE;
  }
  
  ATH_CHECK((*matchedHandle).initialize());
  ATH_CHECK((*ptdiffHandle).initialize());
  ATH_CHECK((*energydiffHandle).initialize());
  ATH_CHECK((*massdiffHandle).initialize());
  ATH_CHECK((*ptrespHandle).initialize());
  ATH_CHECK((*energyrespHandle).initialize());
  ATH_CHECK((*massrespHandle).initialize());
  ATH_CHECK((*ptrefHandle).initialize());
  ATH_CHECK((*etarefHandle).initialize());
 

  // Loop over L1 jets and fill pt, energy, mass differences and responses
  // between matched jets, plus reference pT and eta
  
  // Retrieve the L1 jet container
  SG::ReadHandle<JetContainer> jets(m_l1jetContainerkey, ctx);
  if(!jets.isValid()){
    // the L1 containers should _always_ be present, although may be empty.
    ATH_MSG_WARNING("evtStore() does not contain the L1 jet collection with name "
		 << m_l1jetContainerkey);
    return StatusCode::SUCCESS;
  }
  
  for(const auto jet : *jets){
    bool matched = (*matchedHandle)(*jet);
    
    if(matched){
      jetMatchData.emplace_back((*ptdiffHandle)(*jet),
				(*energydiffHandle)(*jet),
				(*massdiffHandle)(*jet),
				(*ptrespHandle)(*jet),
				(*energyrespHandle)(*jet),
				(*massrespHandle)(*jet),
				(*ptrefHandle)(*jet),
				(*etarefHandle)(*jet));
    } 
  }
  
  return StatusCode::SUCCESS;
}

template class TrigL1FexJetMonitorTool<JTM_JetRoIContainer>;
template class TrigL1FexJetMonitorTool<JTM_gFexJetRoIContainer>;
template class TrigL1FexJetMonitorTool<JTM_jFexLRJetRoIContainer>;
template class TrigL1FexJetMonitorTool<JTM_jFexSRJetRoIContainer>;

  
