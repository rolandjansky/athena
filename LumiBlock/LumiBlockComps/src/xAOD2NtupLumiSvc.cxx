/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAOD2NtupLumiSvc.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TKey.h"
#include "TTree.h"
#include "TFile.h"

#include "xAODLuminosity/LumiBlockRangeContainer.h"
#include "xAODLuminosity/LumiBlockRangeAuxContainer.h"
#include "xAODRootAccess/TEvent.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"

// Constructor
xAOD2NtupLumiSvc::xAOD2NtupLumiSvc(const std::string& name, ISvcLocator *svcLoc) :
  AthService(name, svcLoc)
{

}



StatusCode xAOD2NtupLumiSvc::initialize() {

  ServiceHandle<IIncidentSvc> isvc("IncidentSvc",name());
  isvc->addListener( this, "MetaDataStop", 0, true);

  return StatusCode::SUCCESS;

}

void xAOD2NtupLumiSvc::handle(const Incident& inc) {
  if (inc.type() == "MetaDataStop") {
      finish().ignore();
  }
}

StatusCode xAOD2NtupLumiSvc::finish(){

   ServiceHandle<StoreGateSvc> metastore("StoreGateSvc/MetaDataStore",name());
   
   //see if we have any luminosity to propagate to ntuples
   std::vector<std::string> keys;
   metastore->keys<xAOD::LumiBlockRangeContainer>(keys);
   //if there is no lumi, then we are done
   if(!keys.size()) return StatusCode::SUCCESS;
   


   //use xAOD::RootAccess to create a metadata file
   //put all lumiblock types into output so that hadd merging the ntuples is 'safe'
   xAOD::TEvent evt;
   TFile* outFile = new TFile("metadata.out.root","RECREATE");
   if(evt.writeTo( outFile ).isFailure()) return StatusCode::FAILURE;
   std::vector<std::string> allKeys = {"LumiBlocks","IncompleteLumiBlocks","SuspectLumiBlocks"};
   for(auto& key : allKeys) {
    bool foundKey(false);for(uint i=0;i<keys.size();i++) {if(keys[i]==key) {foundKey=true;break;}}
    if(foundKey) {
      auto lbs = metastore->retrieveUniquePrivateCopy<xAOD::LumiBlockRangeContainer>(key); //returns unique_ptr
      auto lbsAux = metastore->retrieveUniquePrivateCopy<xAOD::LumiBlockRangeAuxContainer>(key+"Aux.");  //returns unique_ptr
      if( evt.recordMeta( lbs.release() , key ).isFailure() ) return StatusCode::FAILURE; //technically we wont need to store the interface container, but doesnt harm us
      if( evt.recordMeta( lbsAux.release() , key+"Aux.").isFailure() ) return StatusCode::FAILURE;
    } else {
      //record a dummy interface and aux container
      xAOD::LumiBlockRangeContainer* dummy = new xAOD::LumiBlockRangeContainer;
      xAOD::LumiBlockRangeAuxContainer* dummyAux = new xAOD::LumiBlockRangeAuxContainer;
      dummy->setStore(dummyAux);
      if( evt.recordMeta( dummy , key).isFailure() ) return StatusCode::FAILURE;
      if( evt.recordMeta( dummyAux , key+"Aux.").isFailure() ) return StatusCode::FAILURE;
    }
   }
   
   if( evt.finishWritingTo( outFile ).isFailure() ) return StatusCode::FAILURE;;
   
   TTree* metatree = dynamic_cast<TTree*>(outFile->Get("MetaData"))->CloneTree(); 
   metatree->SetDirectory(0);

   //copy this MetaData tree to every output stream that contains a Tree
   ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
   std::vector<std::string> histsvcTrees = histSvc->getTrees(); 
   std::set<TString> doneStreams;
   for(unsigned int i=0;i<histsvcTrees.size();i++) {
      //strip the tree name (all after last /)
      TString ss(histsvcTrees[i].c_str());
      int secondSlash = TString(ss(1,ss.Length())).First('/');
      TString ssPath = ss(0,secondSlash+1);
      if(doneStreams.find(ssPath)!=doneStreams.end()) continue;
      TString sss = ssPath;sss += "/MetaData";
      histSvc->regTree(sss.Data(),metatree->CloneTree());
      doneStreams.insert(ssPath);
   }
   
   outFile->Close();
   delete outFile;
   //now delete the actual file
   std::remove("metadata.out.root");

   return StatusCode::SUCCESS;
}

