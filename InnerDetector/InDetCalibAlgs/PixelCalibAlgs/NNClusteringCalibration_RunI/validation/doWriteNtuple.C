/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

void doWriteNtuple(int nParticles, bool useTrackEstimate )
{
  
  gSystem->Load("../libTJetNet.so");
  
  gROOT->ProcessLine(".L RIOs.C+g");
  gROOT->ProcessLine(".L writeNtuple.C+g");


  std::string argStr;

  std::ifstream ifs;//
  if(!useTrackEstimate){ ifs.open("files.txt");}else{ifs.open("filesOnTrack.txt");}
 

  std::getline(ifs,argStr);
  
  // split by ','
  std::vector<std::string> fileList;
  for (size_t i=0, n=0; i <= argStr.length(); i=n+1)
    {
      n = argStr.find_first_of(',',i);
      if (n == string::npos)
        n = argStr.length();
      string tmp = argStr.substr(i,n-i);
      fileList.push_back(tmp);
    }
  
  // with or without tracks
  TString w_wo;
  
  if(!useTrackEstimate){    w_wo = "_withoutTracks";  }else{ w_wo = "_withTracks";  }
  
  for(int num =0; num<fileList.size(); num++){
    
    TString fName = fileList.at(num);

    cout << "Processing file: " << endl;
    cout << fName << endl;
    TFile *f = TFile::Open(fName);
    TTree* tt = (TTree*)gDirectory->Get("Validation/NNinput");
    
    writeNtuple* l = new writeNtuple(tt); 
    
    TString    pathWithoutTracks  = "/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetJune2011/withoutTracks/";
    TString    pathWithTracks     = "/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtupleApril2011/withTracks/";
    
    l->m_pathWeightsWithoutTracks = pathWithoutTracks;
    l->m_pathWeightsWithTracks    = pathWithTracks;
    
    //  for (int nParticles = 1; nParticles < 4; nParticles++ ){
    
    TString outName;
    outName += "NNTrkValidationNtuple";
    outName += "_np";
    outName += nParticles;
    outName += w_wo;
    outName += "_";
    outName += num;
    outName += ".root";
    
    
    l->Loop(nParticles, useTrackEstimate, outName );
    
    //  }
    
  } 
  gROOT->ProcessLine(".q");
  
}
