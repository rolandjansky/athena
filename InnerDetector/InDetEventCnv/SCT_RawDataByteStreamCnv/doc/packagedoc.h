/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SCT_RawDataByteStreamCnv_page SCT_RawDataByteStreamCnv Package
@author: Kondo.Gnanvo@cern.ch, Maria.Jose.Costa@cern.ch, nbarlow@mail.cern.ch, Susumu.Oda@cern.ch

@section SCT_RawDataByteStreamCnv_SCT_RawDataByteStreamCnvIntro Introduction

This package is meant to provide the SCT ByteStream converter 
tools for a full ATLAS run, cosmics and combined test beam (CTB) data. 
The decoding tools have to be called when an algorithm asks for the RDOs of the SCT. 
They get as an input the ByteStream and produce as an output the corresponding RDOs.

There are also the tools that allow to make the inverse process, i.e. write
the ByteStream from the RDOs objects.
The encoding tools are automatically called when an algorithm asks for the ByteStream of the SCT with the RDO input. 
The encoding tools are used in ID HLT.

All these tools decode (or encode) the data and deal with the cabling map.
For this purpose they use the cabling tool implemented in the SCT_Cabling package.

@section SCT_RawDataByteStreamCnv_SCT_RawDataByteStreamCnvOverview Tools Overview
  The SCT_RawDataByteStreamCnv package contains the following classes:
    -  <b>SCT decoding</b> 
     - SCTRawDataProvider
     - SCTRawDataProviderTool
     - SCT_RodDecoder
     - SCTEventFlagWriter
    -  <b>SCT encoding</b> 
     - SCTRawContByteStreamCnv
     - SCTRawContByteStreamTool
     - SCT_RodEncoder

 1) SCTRawDataProvider:

    This is an AthReentrantAlgorithm for converting BS to RDO.
    It gets a vector of ROBFragments from the ROBDataProviderSvc, creates an SCT_RDO_Container and stores it in StoreGate,
    then uses the SCTRawDataProviderTool AthAlgTool (which in turn uses SCT_RodDecoder) to fill Collections (one per link) and
    put them in this Container. Since the IDC migration, the SCT_RDO_Container owns these collections, so StoreGate no longer
    has to do the bookkeeping and cleanup, and there is no longer any need to write out empty collections.

 2) SCTRawContByteStreamCnv:

    This is the top level converter for converting RDO->ByteStream, 
    implementing methods from the Gaudi Converter base class designed to interpret data
    between the transient/persistent data stores. For objects without interlinks, a minimal implementation requires two methods
    (see: http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/Gaudi/Gaudi_v9/GUG/Output/GDG_Converters.html ) :
    - These methods are declared as:

        - createObj(IOpaqueAddress*, DataObject*&) , persistent (i.e. bytestream) -> transient (i.e. Raw Data Object)
        - createRep(DataObject*, IOpaqueAddress*&) , transient -> persistent 
    
    The second method uses the SCTRawContByteStreamTool in its implementation. ('Coll' and 'Cont'
    in the names refer to collections and containers respectively). The first method is not used!
    Rather, the method outlined in (1) above is called to convert a bytestream to RDO.
    The IOpaqueAddress pointer is cast to a ByteStreamAddress pointer in the implementation. The first argument in the createRep method contains 
    a reference to a pointer which will return the result. The data object pointer is retrieved as StoreGateSvc::fromStorable(pObj, container).
     
    - For each Collection, we need to find the corresponding ROBIDs,
    and then create ByteStreamAddress objects and record them in SG. 
    For that purpose, we use the SCT_Cabling package tools to map the Collection ID to ROBID. 
    See https://gitlab.cern.ch/atlas/athena/blob/master/InnerDetector/InDetEventCnv/SCT_RawDataByteStreamCnv/src/SCTRawContByteStreamTool.cxx

*/
