///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthAnalysisHelper.h 
// Header file for class AthAnalysisHelper
// Methods that are useful for athena-based analysis will go here
// Author: W.Buttinger<will@cern.ch>
/////////////////////////////////////////////////////////////////// 

#ifndef ATHANALYSISBASECOMPS_ATHANALYSISHELPER_H
#define ATHANALYSISBASECOMPS_ATHANALYSISHELPER_H 1

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "IOVDbDataModel/IOVMetaDataContainer.h"

class AthAnalysisHelper { //thought about being a namespace but went for static methods instead, in case I want private data members in future

public:
   
   ///retrieve metadata from the input metadata storegate. Use checkMetaSG.py to see the 'folder' and 'key' values available
   template<typename T> static StatusCode retrieveMetadata(const std::string& folder, const std::string& key, T& out) {
      ServiceHandle<StoreGateSvc> inputMetaStore("StoreGateSvc/InputMetaDataStore", "AthAnalysisHelper");
      if(inputMetaStore.retrieve().isFailure()) return StatusCode::FAILURE; //must remember to release
      StatusCode result = retrieveMetadata(folder,key,out,inputMetaStore);
      if(inputMetaStore.release().isFailure()) return StatusCode::FAILURE;
      return result;
   }
   
   
   
   ///implemenation where you pass it a particular store instead
   template<typename T> static StatusCode retrieveMetadata(const std::string& folder, const std::string& key, T& out, ServiceHandle<StoreGateSvc> inputMetaStore) {
      const IOVMetaDataContainer* cont = 0;
      if( inputMetaStore->retrieve(cont,folder).isFailure()) return StatusCode::FAILURE;
   
      //payload is a collection of condattrlistcollections
      //only look a the first one, assuming it exists, and within that only look at the first channel;
      if(cont->payloadContainer()->size()>0 && cont->payloadContainer()->at(0)->size()>0) {
         //just try to retrieve the requested key from the attributelist - we will let it throw the AttributeListException if it fails
         //if the typeName is std::string, we will try to use the gaudi parsers to parse it
         //otherwise we try to do a straight assignment 
         const coral::Attribute& attr = cont->payloadContainer()->at(0)->attributeList(cont->payloadContainer()->at(0)->chanNum(0))[key];
         if(attr.specification().typeName()=="string") {
            if(Gaudi::Parsers::parse(out,attr.data<std::string>()).isFailure()) return StatusCode::FAILURE;
         } else { //do a straight conversion, and just hope its ok (FIXME: should probably do a check of typeid(T) vs typeName)
            out = attr.data<T>();
         }
   
         return StatusCode::SUCCESS;
      }
      return StatusCode::FAILURE;
   }






}; //AthAnalysisHelper class


#endif