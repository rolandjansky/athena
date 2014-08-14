/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage
@author Nikos.Konstantinidis -at- cern.ch, Eric.Jansen -at- hef.ru.nl, Juergen.Thomas -at- cern.ch
@section IntroCaloJiveXML Introduction

This package contains the AlgToos that produce the XML fragments of AOD related datatypes for the XML files read by atlantis.
The AlgTools, all inheriting from JiveXML::DataRetriever, are the following:

   - JiveXML::AODCaloClusterRetriever : m_storeGate->retrieve(CaloClusterContainer*, m_sgKey). Property:
      - StoreGateKey : ( default "LArClusterEM37" ) the StoreGate location of the CaloClusterContainer.

   - JiveXML::AODJetRetriever : m_storeGate->retrieve(JetContainer*, m_sgKey). Property:
      - FavouriteJetCollection : ( default "Cone4TowerJets" ) the StoreGate location of the favourite JetContainer.
      - OtherJetCollections : (default: blank ) further StoreGate locations to be retrieved.

   - JiveXML::CompositeParticleRetriever : m_storeGate->retrieve(CompositeParticleContainer*, m_sgKey). Property:
      - StoreGateKey : ( default "AllObjects" ) the StoreGate location of the CompositeParticleContainer.

   - JiveXML::EventViewRetriever : m_storeGate->retrieve( EventViewContainer*, m_sgKey). Property:
      - StoreGateKey : ( default "FullReco" ) the StoreGate location of the EventViewContainer.

   - JiveXML::PhotonRetriever : m_storeGate->retrieve(PhotonContainer*, m_photon_key ). Property:
      - StoreGateKey : ( default "PhotonAODCollection" ) the StoreGate location of the PhotonContainer.

   - JiveXML::BJetRetriever : m_storeGate->retrieve(ParticleJetContainer*, m_bJet_key ). Property:
      - StoreGateKey : ( default "Cone4TowerJets" ) the StoreGate location of the ParticleJetContainer.

   - JiveXML::MuonRetriever : m_storeGate->retrieve(MuonContainer*, m_sgKey). Property:
      - StoreGateKey : ( default "StacoMuonCollection" ) the StoreGate location of the MuonContainer.

   - JiveXML::TauJetRetriever : m_storeGate->retrieve(Analysis::TauJetContainer, m_sgKey). Property:
      - StoreGateKey : ( default "TauJetCollection" ) the StoreGate location of the Analysis::TauJetContainer. 

   - JiveXML::ElectronRetriever : m_storeGate->retrieve(ElectronContainer*, m_sgKey). Property:
      - StoreGateKey : ( default "ElectronAODCollection" ) the StoreGate location of the ElectronContainer.

   - JiveXML::TrackParticleRetriever : m_storeGate->retrieveRec::TrackParticleContainer(, m_sgKey). Property:
      - StoreGateKey : ( default "TrackParticleCandidate" ) the StoreGate location of the Rec::TrackParticleContainer.


@htmlinclude used_packages.html

@include requirements

*/
