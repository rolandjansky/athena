// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETMONITORING_TRIGJFEXMONITORTOOL_H
#define TRIGJETMONITORING_TRIGJFEXMONITORTOOL_H

#include "./ITrigJetMonitorTool.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"

#include "AthenaBaseComps/AthAlgTool.h"


template<typename JetContainer>
class TrigJFexMonitorTool : virtual public AthAlgTool,
				virtual public ITrigJetMonitorTool {
  
public:
  TrigJFexMonitorTool(const std::string&,
			  const std::string&,
			  const IInterface*);
  
  // to allow access to the ITrigJetMonitorTool interface
   StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );
  // to resolve possible conflicts with IProperty::interfaceID()
  static const InterfaceID& interfaceID() {
    return ITrigJetMonitorTool::interfaceID();}
  
  StatusCode initialize();
  StatusCode finalize();
  
  virtual StatusCode getData(const EventContext& ctx,
			     std::vector<JetData>& jetData) const override;

  virtual std::string groupName() const override {return m_groupName;}
  
private:
  
  // Name of the L1 jet collection to be monitored
  SG::ReadHandleKey<JetContainer> m_l1jetContainerkey;
  
  // Variables to add matched histograms
  Gaudi::Property<bool> m_isMatched {
    this, "IsMatched", true,
    "Plotting response histograms for L1 jets matched to online/offline jets"};
  
  Gaudi::Property<std::string> m_matchedOfflineJetContainer {
    this, "MatchedOfflineJets", "",
    "Name of matched offline jet collection"};
  
  Gaudi::Property<std::string> m_matchedHLTJetContainer {
    this, "MatchedHLTJets", "", "Name of matched HLT jet collection"};

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

  std::string m_groupName{"uninitialised"};



};

#include "./TrigJFexMonitorTool.icc"
#endif // !TRIGJETMONITORING_TRIGL1JETFEXMONITORTOOL_H
