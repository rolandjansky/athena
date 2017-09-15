/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SCT_RawDataByteStreamCnv_page SCT_RawDataByteStreamCnv Package
@author:

- SCT: Kondo.Gnanvo@cern.ch, Maria.Jose.Costa@cern.ch nbarlow@mail.cern.ch

@section SCT_RawDataByteStreamCnv_SCT_RawDataByteStreamCnvIntro Introduction

This package is meant to provide the SCT ByteStream converter 
tools for a full Atlas run, cosmics and combined test beam (CTB) data. 
These tools are automatically called when an algorithm asks for the data of the SCT. 
They get as an input the ByteStream and produce as an output the corresponding RDOs.

There are also the tools that allow to make the inverse process, i.e. write
the ByteStream from the RDOs objects.

All these tools have to decode (or encode) the data and deal with the cabling
map. For this purpose they use the cabling tool implemented in the 
InDetCabling package.

@section SCT_RawDataByteStreamCnv_SCT_RawDataByteStreamCnvOverview Tools Overview
  The SCT_RawDataByteStreamCnv package contains the following classes:
    -  <b>SCT:</b> 
     - SCTRawContByteStreamCnv
     - SCTRawContByteStreamTool
     - SCTRawDataProvider
     - SCTRawDataProviderTool
     - SCT_RodDecoder
     - SCT_RodEncoder

 1) SCTRawContByteStreamCnv:

    This is the top level converter for converting RDO->ByteStream, 
    implementing methods from the Gaudi Converter base class designed to interpret data
    between the transient/persistent data stores. For objects without interlinks, a minimal implementation requires two methods
    (see: http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/Gaudi/Gaudi_v9/GUG/Output/GDG_Converters.html#1010951 ) :
    - These methods are declared as:

        - createObj(IOpaqueAddress*,  DataObject*&) , persistent (i.e. bytestream) -> transient (i.e. Raw Data Object)
        - createRep(DataObject*, IOpaqueAddress*& ) , transient -> persistent 
    
    The second method uses an XXXRawContByteStreamTool in its implementation. ('Coll' and 'Cont'
    in the names refer to collections and containers respectively). I'm not convinced the first method is called! Rather, the method outlined 
    in (2) below is called to convert a bytestream to RDO.
    The IOpaqueAddress pointer is cast to a ByteStreamAddress pointer in the implementation. The second argument in these methods contains 
    a reference to a pointer which will return the result. The data object pointer is set or retrieved as pObj=StoreGateSvc::asStorable(m_container)
    or StoreGateSvc::fromStorable(pObj, container) respectively.
     
    - For each Collection, we need to find the corresponding ROBIDs,
    and then create ByteStreamAddress objects and record them in SG. 
    For that purpose, we use the SCT_Cabling package tools to map the Collection ID to ROBID. 
    See http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/InnerDetector/InDetDetDescr/SCT_Cabling/

 2) SCTRawDataProvider:

    This is an Algorithm for converting BS to RDO.
    It gets a vector of ROBFragments from the ROBDataProviderSvc, creates an SCT_RDO_Container and stores it in StoreGate,
    then uses the SCTRawDataProviderTool AlgTool (which in turn uses SCT_RodDecoder) to fill Collections (one per link) and
    put them in this Container.  Since the IDC migration, the SCT_RDO_Container owns these collections, so StoreGate no longer
    has to do the bookkeeping and cleanup, and there is no longer any need to write out empty collections.

*/
