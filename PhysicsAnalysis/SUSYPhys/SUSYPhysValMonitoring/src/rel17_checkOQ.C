/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************************************
//
// ROOT macro to apply object-level quality cuts
//

/*

EXAMPLE of how to use:
root -l
    .L checkOQ.C++;
    egammaOQ myOQ;
    myOQ.initialize();
    myOQ.checkOQClusterPhoton(152209, 0.6, 1.2);
    myOQ.checkOQClusterElectron(152209, -1.4, 2.2);
IF I GET 3 AS OUTPUT I HAVE TO REJECT THE CLUSTER

IMPORTANT NOTE: It is recommended to use the eta and phi of the cluster!

The output is:
-  If you are outside the maps region (|eta|>3.2): 0 
-  If there are no problems: 1
-  If there is a non nominal HV problems 2
-  If there is a major problem (dead feb, dead HV) 3

By default (so if you do not modify anything):
 - isolated bad cells in S2 kill the candidate if they are within a 3x3 region around the cluster barycenter
 - isolated bad cells in S1 kill the candidate if they are within a x*y region around the cluster barycenter, corresponding to the 8 central strips (photons only!)
 - dead FEBs kill the candidate if they overlap with a 3x7 (barrel) or 5x5 (endcap) region around the cluster barycenter in S1, S2
**********************************************************************************/

#include "SUSYPhysValMonitoring/rel17_checkOQ.h"

#include <cmath>
using std::abs;

#include <TMath.h>

//ClassImp(egammaOQ)

egammaOQ::egammaOQ( string name )
{
   TNamed::SetName( name.c_str() );

   thehEMPS = NULL;
   thehEMS1 = NULL;
   thehEMS2 = NULL;
   thehEMS3 = NULL;
   thehTile = NULL;
   thehEMS1onlyDeadFeb = NULL;
   thehEMS2onlyDeadFeb = NULL;
   thehEMPSOnlyHV = NULL;
   thehEMS1S2S3OnlyHV = NULL;
   thehEMPSonlyDeadFeb = NULL;
   OQMapFile = NULL;

   OQMapInMemory = 0;

   map_thehEMPS.clear();
   map_thehEMS1.clear();
   map_thehEMS2.clear();
   map_thehEMS3.clear();
   map_thehTile.clear();
   map_thehEMS1onlyDeadFeb.clear();
   map_thehEMS2onlyDeadFeb.clear();
   map_thehEMPSOnlyHV.clear();
   map_thehEMS1S2S3OnlyHV.clear();
   map_thehEMPSonlyDeadFeb.clear();
}

egammaOQ::~egammaOQ()
{
   map_thehEMPS.clear();
   map_thehEMS1.clear();
   map_thehEMS2.clear();
   map_thehEMS3.clear();
   map_thehTile.clear();
   map_thehEMS1onlyDeadFeb.clear();
   map_thehEMS2onlyDeadFeb.clear();
   map_thehEMPSOnlyHV.clear();
   map_thehEMS1S2S3OnlyHV.clear();
   map_thehEMPSonlyDeadFeb.clear();  
}

// load all maps to memory
int egammaOQ::initialize()
{
   vector<int> mapnumber;
   /*   mapnumber.push_back( 152166 );
   mapnumber.push_back( 155228 );
   mapnumber.push_back( 155760 );
   mapnumber.push_back( 158115 );
   mapnumber.push_back( 158643 );
   mapnumber.push_back( 158707 );
   mapnumber.push_back( 159040 );*/
   mapnumber.push_back( 161730 );
   mapnumber.push_back( 165589 );
   mapnumber.push_back( 166142 );
   mapnumber.push_back( 166497 );
   mapnumber.push_back( 166658 );
   mapnumber.push_back( 167521 ); 
   mapnumber.push_back( 169136 ); 

   //
   // open the file with the new maps
   //
   for( vector<int>::iterator it = mapnumber.begin(); it != mapnumber.end(); ++it){
      char filename[255];
      sprintf(filename, "./ObjectQualityMaps_run%d.root", *it);
      std::cout << "checkOQ: opening file " << filename << std::endl;
      OQMapFile = TFile::Open(filename,"READ");
      if (OQMapFile==0) {
         std::cout << "checkOQ: could not open file " << filename << std::endl;
         exit(-1);
      }
      
      //
      // read the maps into the egammaOQ histograms and 
      // move the histograms to memory (SetDirectory(0))
      // so that we can close the file afterwards
      //
      OQMapFile->GetObject("hEMPS",thehEMPS);
      thehEMPS->SetName("thehEMPS");
      thehEMPS->SetDirectory(0);
      map_thehEMPS[*it] = thehEMPS;
      
      OQMapFile->GetObject("hEMS1",thehEMS1);
      thehEMS1->SetName("thehEMS1");
      thehEMS1->SetDirectory(0);
      map_thehEMS1[*it] = thehEMS1;

      OQMapFile->GetObject("hEMS2",thehEMS2);
      thehEMS2->SetName("thehEMS2");
      thehEMS2->SetDirectory(0);
      map_thehEMS2[*it] = thehEMS2;

      OQMapFile->GetObject("hEMS3",thehEMS3);
      thehEMS3->SetName("thehEMS3");
      thehEMS3->SetDirectory(0);
      map_thehEMS3[*it] = thehEMS3;

      OQMapFile->GetObject("hEMS1onlyDeadFeb",thehEMS1onlyDeadFeb);
      thehEMS1onlyDeadFeb->SetName("thehEMS1onlyDeadFeb");
      thehEMS1onlyDeadFeb->SetDirectory(0);
      map_thehEMS1onlyDeadFeb[*it] = thehEMS1onlyDeadFeb;

      OQMapFile->GetObject("hEMS2onlyDeadFeb",thehEMS2onlyDeadFeb);
      thehEMS2onlyDeadFeb->SetName("thehEMS2onlyDeadFeb");
      thehEMS2onlyDeadFeb->SetDirectory(0);
      map_thehEMS2onlyDeadFeb[*it] = thehEMS2onlyDeadFeb;

      OQMapFile->GetObject("hEMPSOnlyHV",thehEMPSOnlyHV);
      thehEMPSOnlyHV->SetName("thehEMPSOnlyHV");
      thehEMPSOnlyHV->SetDirectory(0);
      map_thehEMPSOnlyHV[*it] = thehEMPSOnlyHV;

      OQMapFile->GetObject("hEMS1S2S3OnlyHV",thehEMS1S2S3OnlyHV);
      thehEMS1S2S3OnlyHV->SetName("thehEMS1S2S3OnlyHV");
      thehEMS1S2S3OnlyHV->SetDirectory(0);
      map_thehEMS1S2S3OnlyHV[*it] = thehEMS1S2S3OnlyHV;

      OQMapFile->GetObject("hEMPSonlyDeadFeb",thehEMPSonlyDeadFeb);
      thehEMPSonlyDeadFeb->SetName("thehEMPSonlyDeadFeb");
      thehEMPSonlyDeadFeb->SetDirectory(0);
      map_thehEMPSonlyDeadFeb[*it] = thehEMPSonlyDeadFeb;

      //
      // close file since we have copied the maps to memory
      //
      OQMapFile->Close();
   }

   return 0;
}



//
// Get map number corresponding to a certain run
//
int egammaOQ::GetMapNumber(int runnumber) {
  
  
  int mapnumber = 0;
  /*  if (runnumber <  155228) mapnumber=152166;
  else if (runnumber<155760) mapnumber=155228;
  else if (runnumber<158115) mapnumber=155760;
  else if (runnumber<158643) mapnumber=158115;
  else if (runnumber<158707) mapnumber=158643;
  else if (runnumber<159040) mapnumber=158707;
  else if (runnumber<161730) mapnumber=159040;*/

  if (runnumber<165589) mapnumber=161730;
  else if (runnumber<166142) mapnumber=165589;
  else if (runnumber<166497) mapnumber=166142;
  else if (runnumber<166658) mapnumber=166497;
  else if (runnumber == 166658) mapnumber=166658;
  else if (runnumber<167521) mapnumber=166497;
  else if (runnumber<169136) mapnumber=167521;
  else if (runnumber >= 169136) mapnumber=169136;
  else{
    cout << "Run Number not found: " << runnumber << endl;
    exit(-1);
  }
  
  return(mapnumber);
}


//
// Load maps for a certain run
//
int egammaOQ::LoadOQMaps(int runnumber) {

  //
  // Get map number corresponding to this run
  // 
  int mapnumber = GetMapNumber(runnumber);

  //
  // if maps for this run is already in memory, don't do anything
  //
  if (mapnumber == OQMapInMemory) return 1;

  //
  // otherwise...
  // 
  OQMapInMemory = mapnumber;

//   if( thehEMPS != NULL ) delete thehEMPS;
//   if( thehEMS1 != NULL ) delete thehEMS1;
//   if( thehEMS2 != NULL ) delete thehEMS2;
//   if( thehEMS3 != NULL ) delete thehEMS3;
//   if( thehEMS1onlyDeadFeb != NULL ) delete thehEMS1onlyDeadFeb;
//   if( thehEMS2onlyDeadFeb != NULL ) delete thehEMS2onlyDeadFeb;
//   if( thehEMPSOnlyHV != NULL ) delete thehEMPSOnlyHV;
//   if( thehEMS1S2S3OnlyHV != NULL ) delete thehEMS1S2S3OnlyHV;
//   if( thehEMPSonlyDeadFeb != NULL ) delete thehEMPSonlyDeadFeb;

  if( map_thehEMPS.find( mapnumber ) == map_thehEMPS.end() ){
     std::cout << "Map Number \"" << mapnumber << "\" not loaded into memory!" << endl;
     exit(-1);
  }

  thehEMPS = map_thehEMPS.find(mapnumber)->second;
  thehEMS1 = map_thehEMS1.find(mapnumber)->second;
  thehEMS2 = map_thehEMS2.find(mapnumber)->second;
  thehEMS3 = map_thehEMS3.find(mapnumber)->second;
  thehEMS1onlyDeadFeb = map_thehEMS1onlyDeadFeb.find(mapnumber)->second;
  thehEMS2onlyDeadFeb = map_thehEMS2onlyDeadFeb.find(mapnumber)->second;
  thehEMPSOnlyHV = map_thehEMPSOnlyHV.find(mapnumber)->second;
  thehEMS1S2S3OnlyHV = map_thehEMS1S2S3OnlyHV.find(mapnumber)->second;
  thehEMPSonlyDeadFeb = map_thehEMPSonlyDeadFeb.find(mapnumber)->second;
    
  return(1);
}


//
// Main functions:
// check quality of cluster in run=runnumber, with barycenter (myEta,myPhi),
// either candidate="photon" or "electron"
//

int egammaOQ::checkOQClusterElectron(int runnumber, double myEta, double myPhi, bool verbose) {

  int particletype=1;
  int result=egammaOQ::checkOQCluster(runnumber, myEta, myPhi, particletype, verbose);

  return result;
}

int egammaOQ::checkOQClusterPhoton(int runnumber, double myEta, double myPhi, bool verbose) {

  int particletype=2;
  int result=egammaOQ::checkOQCluster(runnumber, myEta, myPhi, particletype, verbose);

  return result;
}




int egammaOQ::checkOQCluster(int runnumber, double myEta, double myPhi, int candidate, bool verbose) {

  //
  // check that user has selected either photon or electron as candidate type
  //
  if ( (candidate!=1) && (candidate!=2) ) {
    std::cout << "Wrong candidate type selected: " << candidate << std::endl;
    std::cout << "Please choose either photon (=2) or electron(=1)" << std::endl;
    return 3;
  }

  //
  // load object quality maps
  //
  int retcode = LoadOQMaps(runnumber);
  if (retcode==0) {
    // if failed to load oq map, return cluster is BAD (so user notices the error!)
    return 3;
  }

  //
  // define fiducial region around cluster barycenter for dead FEB quality cuts
  //
  int netacells=0;
  int nphicells=0;
  const double etabarrel=1.37; //WHICH EXACT VALUE SHOULD WE USE HERE??
  bool isinbarrel = (abs(myEta) < etabarrel);
  // for the moment the fiducial region are identical for photons and electrons, but
  // it may change in the future...
  if (candidate==1) {  //CASE OF ELECTRON
    if (isinbarrel) {
      netacells = 3;
      nphicells = 7;
    } else {
      netacells = 5;
      nphicells = 5;
    }
  }
  else if (candidate==2) {  // CASE OF PHOTON
    if (isinbarrel) {
      netacells = 3;
      nphicells = 7;
    } else {
      netacells = 5;
      nphicells = 5;
    }
  }
  //
  // check quality of cluster
  //
  return checkOQCluster(myEta, myPhi, netacells, nphicells, candidate, verbose);
}


//
// helper function: check cluster defined by barycenter (myEta, myPhi) and cluster size (NetaCells, NphiCells)
// calls checkOQClusterUtil with different parameters to perform checks for bad FEB and isolated dead cells
//
int egammaOQ::checkOQCluster(double myEta, double myPhi, int NetaCells, int NphiCells, int candidate, bool verbose) {
  // Example of a function checking if there is a problem in the cluster (myEta,myPhi) of size NetaCells times NphiCells
  // in the middle sampling, with cell size of 0.025 in eta.
  // It could not be 100% ok if the cluster includes the last cells before the crack region (where cell size is different). 
  // It returns : 1 if the cluster is ok;
  //              2 if it has non nominal HV (but not deadHV or readout problems)
  //              3 if the cluster is bad ( any deadHV or readout problems);

  if (verbose) std::cout << "CHECKING THE " << NetaCells << "x" << NphiCells << " CLUSTER CENTERED IN ===> " << myEta << " " << myPhi << std::endl;   
  vector<double> cellCenter = provideCellCenterS2(myEta,myPhi,verbose);

  double etaCell = cellCenter[0];
  double phiCell = cellCenter[1];

  //
  // Check if core is ok
  //
  if (verbose) std::cout << "checking the core..." << endl;
  int clusCore =  checkOQCoreRegion(etaCell, phiCell, 3*0.025/2., 3*0.025/2., candidate, verbose);

  //
  // If core is bad, return bad flag
  //
  if(clusCore >2) {
    if (verbose) std::cout << "     The problem is in the CORE of the cluster" << std::endl;
    return clusCore;
  }
  
  //
  // If core OK (or non nominal HV, but no dead HV/FEB) then check the whole cluster
  //
  if (verbose) std::cout << "checking the whole cluster..." << endl;
  int clusFull =  checkOQRegion(etaCell, phiCell, NetaCells*0.025/2., NphiCells*0.025/2., candidate, verbose);
  return(clusFull);

}


// checks if there is any problem (dead HV/FEB) in the region myEta+/-deltaEta VS myPhy+/-deltaPhi
// where: myEta and myPhi are the coordinates of the cluster 
//        deltaEta and deltaPhi are half the size of the region in eta and phi respectively 
// It returns : 1 if the region is ok;
//              2 if in the region there is non nominal HV (but not deadHV or readout problems)
//              3 if in the region there is  a deadHV and/or a problem of readout.
int egammaOQ::checkOQRegion(double myEta, double myPhi, double deltaEta, double deltaPhi, int candidate, bool verbose) {
  
  if (candidate==1) {}; //SWITCH ELECTRON/PHOTON, not used at the moment  

  int cond[5];
  
  
  //Check PS HV
  cond[0] = checkOQMap(thehEMPSOnlyHV, myEta, myPhi, deltaEta, deltaPhi, 0.025);
  
  //Check S1S2S3HV
  cond[1] = checkOQMap(thehEMS1S2S3OnlyHV, myEta, myPhi, deltaEta, deltaPhi, 0.025);

  //Check sampling 1 for dead FEBs
  cond[2] = checkOQMap(thehEMS1onlyDeadFeb, myEta, myPhi, deltaEta, deltaPhi, 0.025/8.0);

  //Check sampling 2 for dead FEBs
  cond[3] = checkOQMap(thehEMS2onlyDeadFeb, myEta, myPhi, deltaEta, deltaPhi, 0.025);  
  
  //Check PS dead FEBs
  cond[4] = -99;//checkOQMap(thehEMPSonlyDeadFeb, myEta, myPhi, deltaEta, deltaPhi, 0.025);
  
  //Return the worst condition flag.
  
  int flagmax=0;
  for (int i=0; i<5 ; ++i) {
    if (cond[i] != -99 &&  cond[i]>=flagmax) flagmax=cond[i];
  }
  
  
  if (verbose) {
    if(flagmax == 0) {
      std::cout<< "Your cluster is outside the calo region you are checking" << std::endl;
    }
    else if(flagmax==1) {
      std::cout << "  There are no problems in the region analyzed" << std::endl;
    }
    else if(flagmax==2) {
      std::cout << "  There is a non nominal HV in the region analyzed" << std::endl;
    }
    else if(flagmax==3) {
      std::cout << "  There is readout and/or HV problem in the region analyzed" << std::endl;
    }
  }

  return(flagmax);
}


// Checks if there is any problem in the region myEta+/-deltaEta VS myPhy+/-deltaPhi
// where: myEta and myPhi are the coordinates of the cluster 
//        deltaEta and deltaPhi are half the size of the region in eta and phi respectively 
// It returns : 1 if the region is ok;
//              2 if in the region there is non nominal HV (but not deadHV or readout problems)
//              3 if in the region there is a deadHV and/or a problem of readout or a bad isolated cell

//
// Perhaps we don't need to provide deltaEta and deltaPhi as argument (we know the core is 3x3..)?
//
int egammaOQ::checkOQCoreRegion(double myEta, double myPhi, double deltaEta, double deltaPhi, int candidate, bool verbose) {

  int cond[2];

  // addition for photons (we rely on strip variables for purity measurement)
  // Check the sampling 1 for  ISOLATED BAD CELLS IN 8x1 STRIPS (0.025/2. * 0.1/2.)
  if (candidate==1) //CASE OF ELECTRON
    cond[0] = 1;
  else if (candidate==2) //CASE OF PHOTON
    cond[0] = checkOQMap(thehEMS1, myEta, myPhi, 0.025/2.0, 0.1/2.0, 0.025/8.0);
  
  //
  // Check the sampling 2 (HV group, FEB, single bad cells)
  // The HV group/FEB checks are not really needed since they are applied
  // also in checkOQRegion... one could just check against bad cells...
  //
  cond[1] = checkOQMap(thehEMS2, myEta, myPhi, deltaEta, deltaPhi, 0.025);

  //Return the worst condition flag.
  int flagmax=0;
  for (int i=0; i<2 ; ++i) {
    if (cond[i] != -99 &&  cond[i]>=flagmax) flagmax=cond[i];
  }


  if (verbose) {
    if(flagmax==0) {
      std::cout<< "Your cluster is outside the calo region you are checking" << std::endl;
    }
    else if(flagmax==1) {
      std::cout << "  There are no problems in the region analyzed" << std::endl;
    }
    else if(flagmax==2) {
      std::cout << "  There is a non nominal HV in the region analyzed" << std::endl;
    }
    else if(flagmax==3) {
      std::cout << "  There is readout and/or HV problem in the region analyzed" << std::endl;
    }
  }
  
  return(flagmax);
}


//
// Return the value of the histogram "histo" for the point (myEta,myPhi) 
//
int egammaOQ::checkOQ( TH2I *histo, double myEta, double myPhi){
  
  int theValue= -99; 
  float epsilon = 0.0001;
  bool physReg = checkHistoBound(histo, myEta, myPhi);
  
  if (physReg) {
    int binNum = histo->FindBin(myEta+epsilon, myPhi+epsilon);
    theValue = (int)histo->GetBinContent(binNum);
  }
  
  return(theValue);
}


//
// Provide the center of the cell in S2 where myEta,myPhi is contained.
// Not working if the cluster is outside the (-3.2,3.2) region in eta:
// in this case it gives back the original values.
//
vector<double> egammaOQ::provideCellCenterS2(double myEta, double myPhi, bool verbose) {
  vector<double> cellCenter;
  bool physReg = checkHistoBound(thehEMS2, myEta, myPhi);

  if (physReg) {
    int xbin = thehEMS2->GetXaxis()->FindBin(myEta);
    cellCenter.push_back(thehEMS2->GetXaxis()->GetBinCenter(xbin));
    int ybin = thehEMS2->GetYaxis()->FindBin(myPhi);
    cellCenter.push_back(thehEMS2->GetYaxis()->GetBinCenter(ybin));
  }else{
    if (verbose) {
      std::cout << "WARNING: I AM NOT IN THE (-3.2,3.2) REGION IN ETA" << std::endl;
      std::cout << "         I WILL GIVE BACK THE ORIGINAL VALUES " << std::endl;
    }
    cellCenter.push_back(myEta);
    cellCenter.push_back(myPhi);
  }

  return cellCenter;
}


//
// Check if the point is inside the limits of the histogram
//
bool egammaOQ::checkHistoBound(TH2I* histo, double myEta, double myPhi) {

  bool physReg = true;
  double xmin = histo->GetXaxis()->GetXmin();
  double xmax = histo->GetXaxis()->GetXmax();
  if(myEta<xmin || myEta>xmax) {
    physReg=false;
  }

  if(physReg){
    double ymin = histo->GetYaxis()->GetXmin();
    double ymax = histo->GetYaxis()->GetXmax();
    if(myPhi<ymin || myPhi>ymax) {
      physReg=false;
    }
  }

  return(physReg);
}



//
// Scan the map in the requested region and recover the highest flag. As soon as the flag is 3 it stops.
//
int egammaOQ::checkOQMap(TH2I* histo, double myEta, double myPhi, double deltaEta, double deltaPhi, double granularity) {

  double theEta;
  double thePhi;
  int flagmax=-99;
  int value = -99;
  float epsilon = 0.0001;

  double etaMin = myEta-deltaEta;
  double phiMin = myPhi-deltaPhi;
  //CHOOSE THE ONE WHICH IS MORE APPROPRIATE WRT THE HISTOGRAM EDGES
  //const double pi = TMath::Pi();
  const double pi = 3.1416;
  if (phiMin<-pi) phiMin +=  2.*pi; //crossing the lower bound in Phi
  
  int phiSteps = int((2*deltaPhi+epsilon)/0.025);
  int etaSteps = int((2*deltaEta+epsilon)/granularity);
 
  theEta=etaMin+granularity/2.;
  for(int i=0; i<etaSteps; i++){
    thePhi=phiMin+0.025/2.;
    for (int k =0; k<phiSteps; k++){
      if(thePhi>pi) thePhi -= 2.*pi;  //crossing upper bound in Phi
       value = checkOQ(histo,theEta,thePhi);
       if (value> flagmax){
	flagmax = value;
      }
     if (flagmax==3) break; //as soon as it is bad, return
      thePhi = thePhi + 0.025;
    }
    theEta = theEta + granularity;
  }
  

  return(flagmax);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int egammaOQ::checkOQPointTile(double myEta, double myPhi) {
  // PRELIMINARY !
  // Example of a function checking if there is a problem in the point (myEta,myPhi) for the Tile
  // It returns : 1 if the point is ok;
  //              4 if the point is BAD

  int flagmax = checkOQ(thehTile,myEta,myPhi); // Check the Tile

  if (flagmax == 4 ) std::cout<< " BAD TILE POINT" << std::endl;
  return(flagmax);
}