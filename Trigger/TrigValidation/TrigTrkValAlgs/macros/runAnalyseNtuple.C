/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
 AnalyseNtuple analyse("TrkParticleNtupl.root");
 analyse.setOffCollectionName("TrackParticleCandidate");
 analyse.setEFCollectionName("InDetTrigParticleCreationIOTRT_CosmicsN_EFID");
 analyse.setOfflineCuts(500., 400., 5);
 analyse.setEFCuts(500., 400., 5);
 
 try{
     analyse.initialize();
     analyse.execute();
     analyse.finalize();
 } 
 catch(AnalyseNtupleError::LoadTFileError loadTFileError){
    std::cerr<<"Cannot load TFile "<<loadTFileError.getFileName()<<std::endl;
   }
 catch(AnalyseNtupleError::LoadTTreeError loadTTreeError){
   std::cerr<<"Cannot load TTree "<<loadTTreeError.getTreeName()<<std::endl;
 }
} //end of main block
