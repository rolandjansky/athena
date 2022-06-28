// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETMONITORING_TRIGL1JETFEXMONITORTOOL_H
#define TRIGJETMONITORING_TRIGL1JETFEXMONITORTOOL_H

#include "TrigJetMonitoring/ITrigJetMonitorTool.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include <string>

/////////////////////////////////////////////////////////////////////////////

// Template paramter must provide a name T::JetContainer
template<typename T>
class TrigL1FexJetMonitorTool : virtual public AthAlgTool,
				virtual public ITrigJetMonitorTool {
  
public:
  
  typedef typename T::JetContainer JetContainer;
    
  TrigL1FexJetMonitorTool(const std::string&,
			  const std::string&,
			  const IInterface*);

  virtual StatusCode
  queryInterface( const InterfaceID& riid, void** ppvIf ) override;

  
  virtual StatusCode initialize() override;
  
  virtual StatusCode
  getData(const EventContext& ctx,
	  std::vector<JetData>& jetData) const override;

   virtual StatusCode
   getMatchData(const EventContext& ctx,
		MatchToEnum, 
		std::vector<JetMatchData>& jetData) const override;

private:

  // initialisation using an initialise list  ie {...}
  // does not work - sommrthing to do with the template.
  // intstead use declareProperty in the constructor.
  SG::ReadHandleKey<JetContainer> m_l1jetContainerkey;
  
  // Variables to add matched histograms
  Gaudi::Property<bool> m_doMatching {
    this, "do_matching", false,
    "Plotting response histograms for L1 jets matched to online/offline jets"};
  
  Gaudi::Property<std::string> m_offlineJetContainerToMatch {
    this, "offlineJetsToMatch", "",
    "Name of matched offline jet collection"};
  
  Gaudi::Property<std::string> m_HLTJetContainerToMatch {
    this, "HLTJetsToMatch", "", "Name of matched HLT jet collection"};

  SG::ReadDecorHandleKey<JetContainer> m_offmatchedKey{
    this, "offmatched", "offmatched", "SG key for input matched decoration"};

  SG::ReadDecorHandleKey<JetContainer> m_offptdiffKey{
    this, "offptdiff", "offptdiff",
    "SG key for input matched ptdiff decoration"};
  
  SG::ReadDecorHandleKey<JetContainer> m_offenergydiffKey{
    this, "offenergydiff", "offenergydiff",
    "SG key for input matched energydiff decoration"};
  
  SG::ReadDecorHandleKey<JetContainer> m_offmassdiffKey{
    this, "offmassdiff", "offmassdiff",
    "SG key for input matched massdiff decoration"};
  
  SG::ReadDecorHandleKey<JetContainer> m_offptrespKey{
    this, "offptresp", "offptresp",
    "SG key for input matched ptresp decoration"};
  
  SG::ReadDecorHandleKey<JetContainer> m_offenergyrespKey{
    this, "offenergyresp", "offenergyresp",
    "SG key for input matched energyresp decoration"};
  
  SG::ReadDecorHandleKey<JetContainer> m_offmassrespKey{
    this, "offmassresp", "offmassresp",
    "SG key for input matched massresp decoration"};
  
  SG::ReadDecorHandleKey<JetContainer> m_offptrefKey{
    this, "offptref", "offptref",
    "SG key for input matched ptref decoration"};
  
  SG::ReadDecorHandleKey<JetContainer> m_offetarefKey{
    this, "offetaref", "offetaref",
    "SG key for input matched etaref decoration"};

  SG::ReadDecorHandleKey<JetContainer> m_hltmatchedKey{
    this, "hltmatched", "hltmatched", "SG key for input matched decoration"};
  
  SG::ReadDecorHandleKey<JetContainer> m_hltptdiffKey{
    this, "hltptdiff", "hltptdiff",
    "SG key for input matched ptdiff decoration"};
  
  SG::ReadDecorHandleKey<JetContainer> m_hltenergydiffKey{
    this, "hltenergydiff", "hltenergydiff",
    "SG key for input matched energydiff decoration"};
  
  SG::ReadDecorHandleKey<JetContainer> m_hltmassdiffKey{
    this, "hltmassdiff", "hltmassdiff",
    "SG key for input matched massdiff decoration"};
  
  SG::ReadDecorHandleKey<JetContainer> m_hltptrespKey{
    this, "hltptresp", "hltptresp",
    "SG key for input matched ptresp decoration"};
  
  SG::ReadDecorHandleKey<JetContainer> m_hltenergyrespKey{
    this, "hltenergyresp", "hltenergyresp",
    "SG key for input matched energyresp decoration"};
  
  SG::ReadDecorHandleKey<JetContainer> m_hltmassrespKey{
    this, "hltmassresp", "hltmassresp",
    "SG key for input matched massresp decoration"};
  
  SG::ReadDecorHandleKey<JetContainer> m_hltptrefKey{
    this, "hltptref", "hltptref",
    "SG key for input matched ptref decoration"};
  
  SG::ReadDecorHandleKey<JetContainer> m_hltetarefKey{
    this, "hltetaref", "hltetaref",
    "SG key for input matched etaref decoration"};

};

#endif

