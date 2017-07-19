/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
@page MuonEventTPCnv_page MuonEventTPCnv
@author Edward.Moyse@cern.ch

@section MuonEventTPCnv_MuonEventTPCnvIntro Introduction

This package handles the transient / persistent separation for the persistification of Muon EDM classes

Summary of current convertors (as defined in selection.xml)
  - PRDs
   - General
     - MuonPRD_Container_p2 - templated class which contains typedefs, e.g. MdtPrepDataContainer_p2
     - MuonPRD_Collection_p2 - non-templated class to hold generic collection info, such as id, hash and length.
   - Technology specific 
     - For all of the following, the PRD convertor handles the position, error, and specifics of the class. The Container convertor is responsible for the collections AND for filling re-calculatable information, such as hashes etc
       - MdtPrepDataContainerCnv_p2 / MdtPrepDataContainer_p2 
       - CscPrepDataCnv_p2 / CscPrepData_p2 
       - CscPrepDataContainerCnv_p2 / CscPrepDataContainer_p2 
       - RpcPrepDataCnv_p3 / RpcPrepData_p3 
       - RpcPrepDataContainerCnv_p3 / RpcPrepDataContainer_p3 
       - TgcPrepDataCnv_p2 / TgcPrepData_p2 
       - TgcPrepDataContainerCnv_p2 / TgcPrepDataContainer_p2 

Summary of old  convertors (as defined in OLD_selection.xml):
  - *TODO*
  - PRDs
   - MdtPrepDataContainer_tlp1 - old 'top' level convertor

The ARA convertors are defined in ARA_selection.xml
  
@section MuonEventTPCnv_MuonEventTPCnvLinks Further links
@see https://twiki.cern.ch/twiki/bin/view/Atlas/MuonEventDataModel
@see https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation
@see https://twiki.cern.ch/twiki/bin/view/Atlas/WriteReadDataViaPool



*/
