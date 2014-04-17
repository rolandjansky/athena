/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage
@author Nikos.Konstantinidis -at- cern.ch, Eric.Jansen -at- hef.ru.nl, Juergen.Thomas -at- cern.ch
@section IntroMuonJiveXML Introduction

This package contains the AlgToos that produce the XML fragments of Muon related datatypes for the XML files read by atlantis.
The AlgTools, all inheriting from JiveXML::DataRetriever, are the following:

   - JiveXML::CscPrepDataRetriever : retrieves the CSC strips. Property:
      - StoreGateKey ( default "CSC_Clusters" ) the StoreGate location for the CSC clusters container (through which the digits are accessed).

   - JiveXML::MdtPrepDataRetriever : retrieves the MDT drift circles. Property:
      - StoreGateKey ( default "MDT_DriftCircles" ) the StoreGate location for the MDT drift circles container.

   - JiveXML::MuidTrackRetriever : not used any more. Muid tracks are stored in storegate as a Trk::Track collection and are collected through the TrackRetriever in InDetJiveXML.

   - JiveXML::RpcPrepDataRetriever : retrieves the RPC hits. Property:
      - StoreGateKey : ( default "RPC_Measurements" ) the StoreGate location of the RPC hits container.

   - JiveXML::CSCClusterRetriever : retrieves CSC clusters. Property:
      - StoreGateKey ( default "CSC_Clusters" ) the StoreGate location for the CSC clusters container.

   - JiveXML::MooreRetriever : not used any more. Moore tracks are stored in storegate as a Trk::Track collection and are collected through the TrackRetriever in InDetJiveXML.

   - JiveXML::TgcPrepDataRetriever : retrieves the TGC hits. Property:
      - StoreGateKey : ( default "TGC_Measurements" ) the StoreGate location of the TGC hits container.


@htmlinclude used_packages.html

@include requirements

*/
