/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TrigJiveXML package
@author Nikos.Konstantinidis@cern.ch, Eric.Jansen@hef.ru.nl, Juergen.Thomas@cern.ch
@section IntroTrigJiveXML Introduction

This package contains the AlgToos that produce the XML fragments of Trigger datatypes for the XML files read by atlantis.
The AlgTools inherit from JiveXML::DataRetriever, and are the following:

   - JiveXML::EmTauROIRetriever : m_storeGate->retrieve(roi,"LVL1_ROI")
   - JiveXML::JetROIRetriever   : m_storeGate->retrieve(roi,"LVL1_ROI")
   - JiveXML::MuonROIRetriever : m_storeGate->retrieve(roi,"LVL1_ROI");

   - JiveXML::IdScanTrackRetriever : m_storeGate->retrieve(firstColl,lastColl); (retrieves all TrigInDetTracks)
   - JiveXML::TrigSiSpacePointRetriever : retrieves online pixel/SCT space points
      - Property: PixelSpacePoints : the StoreGate location of the Pixel space points container ( default TrigPixelSpacePoints )
      - Property: SCTSpacePoints : the StoreGate location of the SCT space points container ( default TrigSCTSpacePoints )

   - JiveXML::LVL1ResultRetriever : info retrieved via the TrigDec::TrigDecisionTool
   - JiveXML::LVL1JetElementRetriever : m_storeGate->retrieve(JEVector, m_sgKey)
      - Property: StoreGateKey : the StoreGate location of the DataVector<LVL1::JetElement> ( default LVL1JetElements )
   - JiveXML::LVL1TrigTowerRetriever : m_storeGate->retrieve(TTVector, m_sgKey)
      - Property: StoreGateKey : the StoreGate location of the DataVector<LVL1::TriggerTower> ( default LVL1TriggerTowers )


@htmlinclude used_packages.html

@include requirements

*/
