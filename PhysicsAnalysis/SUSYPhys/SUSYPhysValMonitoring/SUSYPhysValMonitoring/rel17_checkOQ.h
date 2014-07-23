/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef egammaOQ_h
#define egammaOQ_h

//#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TH2I.h"
#include "TNamed.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

class egammaOQ : public TNamed {
 
public:

   egammaOQ( string name = "egammaOQ");
   ~egammaOQ();

   int initialize(string path = ".");

   vector<double> provideCellCenterS2(double myEta, double myPhi, bool verbose=false);
   int checkOQRegion(double myEta, double myPhi, double deltaEta, double deltaPhi, int candidate, bool verbose=false);
   int checkOQCoreRegion(double myEta, double myPhi, double deltaEta, double deltaPhi, int candidate, bool verbose=false);
   int checkOQ(TH2I *histo, double myEta, double myPhi);
   int checkOQCluster(int runnumber, double myEta, double myPhi, int candidate, bool verbose=false);
   int checkOQClusterElectron(int runnumber, double myEta, double myPhi, bool verbose=false);
   int checkOQClusterPhoton(int runnumber, double myEta, double myPhi, bool verbose=false);
   int checkOQCluster(double myEta, double myPhi, int NetaCells, int NphiCells, int candidate, bool verbose=false);
   int checkOQPointTile(double myEta, double myPhi);
   bool checkHistoBound(TH2I* histo, double myEta, double myPhi);
   int checkOQMap(TH2I* histo, double myEta, double myPhi, double deltaEta, double deltaPhi, double granularity);
   int LoadOQMaps(int runnumber);
   int GetMapNumber(int runnumber);

private:
   
   map< int, TH2I* > map_thehEMPS;
   map< int, TH2I* > map_thehEMS1;
   map< int, TH2I* > map_thehEMS2;
   map< int, TH2I* > map_thehEMS3;
   map< int, TH2I* > map_thehTile;
   map< int, TH2I* > map_thehEMS1onlyDeadFeb;
   map< int, TH2I* > map_thehEMS2onlyDeadFeb;
   map< int, TH2I* > map_thehEMPSOnlyHV;
   map< int, TH2I* > map_thehEMS1S2S3OnlyHV;
   map< int, TH2I* > map_thehEMPSonlyDeadFeb;
   

   TH2I *thehEMPS;
   TH2I *thehEMS1;
   TH2I *thehEMS2;
   TH2I *thehEMS3;
   TH2I *thehTile;
   TH2I *thehEMS1onlyDeadFeb;
   TH2I *thehEMS2onlyDeadFeb;
   TH2I *thehEMPSOnlyHV;
   TH2I *thehEMS1S2S3OnlyHV;
   TH2I *thehEMPSonlyDeadFeb;
   int OQMapInMemory;
   TFile* OQMapFile;

#ifndef SUSY_ATHENA
   ClassDef(egammaOQ,1) 
#endif

};

#endif
