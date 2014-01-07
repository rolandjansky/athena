/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
string fname;
cout << "Provide ROOT file to process: "<< endl;
cin >> fname;
int nL1A;
cout << "Provide LVL1 Accept: "<< endl ;
cin >> nL1A;

TFile *file = TFile::Open(fname.c_str());

int nEvents = NEvents->GetEntries();
double occCut = 10./(nEvents*nL1A);
double occCutMax = 1.E-5; // max occupancy cut;
if (occCut<occCutMax) occCut = occCutMax; 

cout << "Occupancy cut for " << nEvents << " events and " 
<< nL1A << " accepted L1 = " << occCut << endl;

double occRaw=0, occMasked=0.;
int nMasked=0;

for (int i=2; i<=40; i++ ) {
  double occBin = pow(10.,occupancy->GetBinCenter(i));
  occRaw+=(occupancy->GetBinContent(i)*occBin);
  if ( occBin<occCut ) occMasked+=(occupancy->GetBinContent(i)*occBin); 
  else nMasked+=occupancy->GetBinContent(i);
}
occRaw/=occupancy->GetEntries();
occMasked/=occupancy->GetEntries();

cout << "Noise occupancy before/after masking " << nMasked << " pixels:"
<< occRaw << "/" << occMasked << endl;

cout << "Preparing noiseOccupancyCuts.txt file..." << endl;

ofstream cutFile("noiseOccupancyCuts.txt");
cutFile << "Disk1ACut  " << occCut << endl;
cutFile << "Disk2ACut  " << occCut << endl;
cutFile << "Disk3ACut  " << occCut << endl;
cutFile << "Disk1CCut  " << occCut << endl;
cutFile << "Disk2CCut  " << occCut << endl;
cutFile << "Disk3CCut  " << occCut << endl;
cutFile << "B-layerCut " << occCut << endl;
cutFile << "Layer1Cut  " << occCut << endl;
cutFile << "Layer2Cut  " << occCut << endl;
cutFile << "gangedPixelMultiplier  " << 1. << endl;
cutFile << "longPixelMultiplier    " << 1. << endl;
cutFile << "occupancyPerBC         " << "true" << endl;
cutFile << "nBCReadout             " << nL1A << endl;
cutFile.close();
cout << "...done!" << endl;
}
