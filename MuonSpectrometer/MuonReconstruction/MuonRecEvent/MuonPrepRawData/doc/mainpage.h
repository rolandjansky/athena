/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage

\section introductionMuonPRD Introduction

The MuonPrepRawData package provides Muon Spectrometer specific implementations of the common TrkPrepRawData classes.

Several inherit from Muon::MuonCluster:

   - Muon::CscPrepData
   - Muon::RpcPrepData
   - Muon::TgcPrepData

One is for the 'raw' technology (more may be added in future):
   - Muon::CscStripPrepData
  
And one is for the drift tubes:
   - Muon::MdtPrepData
   
The MuonPrepRawData are stored in IdentifiableContainers (collections of collections), where the collections are typedefs to specific
instantiations of the Muon::MuonPrepDataCollection template, and where the containers are likewise typedefs to the Muon::MuonPrepDataContainer 
template

\section MuonPRDCollections Muon PRD Data Collections

This package also provides collections:
   - Muon::MuonPrepDataCollection
      - Muon::CscPrepDataCollection
      - Muon::CscStripPrepDataCollection
      - Muon::RpcPrepDataCollection
      - Muon::TgcPrepDataCollection
      - Muon::MdtPrepDataCollection

Which are stored inside the containers:
   - Muon::MuonPrepDataContainer
      - Muon::CscPrepDataContainer
      - Muon::CscStripPrepDataContainer
      - Muon::RpcPrepDataContainer
      - Muon::TgcPrepDataContainer
      - Muon::MdtPrepDataContainer
   
@htmlinclude used_packages.html

@include requirements

*/
