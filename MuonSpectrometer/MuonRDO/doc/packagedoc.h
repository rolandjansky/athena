/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuonRDO_page 
@section MuonRDO_introductionMuonRDO Introduction

This package contains the Muon 'Raw Data Objects' or RDOs. These are the objects representing the Muon bytestream

There are four raw objects, representing the four Muon technologies. There are containers.
At the top we have four IdentifiableContainers:
    - CscRawDataContainer
    - MdtCsmContainer
    - RpcPadContainer
    - TgcRdoContainer

These contain 'collections':
    - CscRawDataCollection
    - MdtCsm
    - RpcPad
    - TgcRdo

(for the RPCs there is an additional level RpcFiredChannel)

Finally, there are the raw data objects:
    - CscRawData
    - MdtAmtHit
    - RpcCoinMatrix
    - TgcRawData

@author Ketevi A. Assamagan <ketevi@bnl.gov>
@author Edward.Moyse@cern.ch

@sa The official Muon EDM webpage is https://twiki.cern.ch/twiki/bin/view/Atlas/MuonEventDataModel

*/
