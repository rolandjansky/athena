/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//--------------------------------------------------
//      Author: Dengfeng Zhang
//      dengfeng.zhang@cern.ch
//--------------------------------------------------        
//
// This script will loop over all aant files, which are generated in digitization jobs 
// of different  types of particles, beam energies and physics lists:
//
//   Get the total energy, energy in Cental Barrel Module, Barrel Module 0 
//   and Extended Barrel Modules.
//   Get the longitudinal distribution-the mean energy as a function of the depth.
//
//--------------------------------------------------        
#include <iostream>
#include <string>
#include <vector>

#include "HistToolKit.h"

using namespace std;

// Select() functions return the energy collected by one PMT/several PMTs
// only when the energy>0 and fitting time!=0, we use the energy collected by this PMT.

// return the energy collected by one PMT
float Select(float* Efit, float* Tfit, int m) 
{
  float sum = 0 ;
  //only when the energy>0 and fitting time!=0, we use the energy collected by this PMT
  if(Efit[m]>=0&&Tfit[m]!=0)
    sum += Efit[m] ;
  return sum ;
}
// return the energy collected by two PMTs
float Select(float* Efit, float* Tfit, int m, int n)
{
  float sum = 0 ;
  if(Efit[m]>=0&&Tfit[m]!=0)
    sum += Efit[m] ;
  if(Efit[n]>=0&&Tfit[n]!=0)
    sum += Efit[n] ;
  return sum ;
}
// return the energy collected by four PMTs
float Select(float* Efit, float* Tfit, int a, int b, int c, int d)
{
  float sum = 0 ;
  if(Efit[a]>=0&&Tfit[a]!=0)
    sum += Efit[a] ;
  if(Efit[b]>=0&&Tfit[b]!=0)
    sum += Efit[b] ;
  if(Efit[c]>=0&&Tfit[c]!=0)
    sum += Efit[c] ;
  if(Efit[d]>=0&&Tfit[d]!=0)
    sum += Efit[d] ;
  return sum ;
}
// return the energy collected by six PMTs
float Select(float* Efit, float* Tfit, int a, int b, int c, int d, int e, int f)
{
  float sum = 0 ;
  if(Efit[a]>=0&&Tfit[a]!=0)
    sum += Efit[a] ;
  if(Efit[b]>=0&&Tfit[b]!=0)
    sum += Efit[b] ;
  if(Efit[c]>=0&&Tfit[c]!=0)
    sum += Efit[c] ;
  if(Efit[d]>=0&&Tfit[d]!=0)
    sum += Efit[d] ;
  if(Efit[e]>=0&&Tfit[e]!=0)
    sum += Efit[e] ;
  if(Efit[f]>=0&&Tfit[f]!=0)
    sum += Efit[f] ;
  return sum ;
}

int GetEnergy()
{
  double p = (5640.0-40.0)/307.0/10.0;   // period size of Tile in cm
  double halfLength = 280.0 ; // half length of barrel module in cm
  double lambdaIron = 131.9/7.87 ; //radiation length of iron
  // radiation length of the tile(mixture of iron and scintillantor)
  double lambda = 20.5559 ;
  double offset = 2.0; // offset in cm
  // the z coordiante of impact point in lambda
  double enterPos = halfLength/lambda + offset/lambdaIron ;
  // Only use the first 13 B cells here, barrel module has 18 B cells in total.
  int nBCell = 13 ;
  // Edge if B cells, unit is lambda, size = nBCell+1 .
  // The uper edge of this cell is next cell's lower edge.
  // this array is used create histograms for longitudinal profile.
  float xBLow[14] ;
  // the z coordiante of the edge of each B cell in cm.
  // Center of ATLAS is origin of coordinates.
  double zBLow[18] {136.0*p, 116.*p, 97.5*p, 79.5*p, 63*p, 46.5*p, 31*p, 15.58*p, 0, 0, -15.58*p, -31*p, -46.5*p, -63*p, -79.5*p, -97.5*p, -116.*p, -136.0*p};

  // Lower enge of first B Cell - B9
  xBLow[0] = offset/lambdaIron;
  int j=0 ;
  // Calculate lower and uper edges of B Cells
  for(int i=0 ; i<nBCell ; i++)
  {
    xBLow[i+1] = abs(enterPos-zBLow[i+j]/lambda) ; // // the z coordiante of the edge of each B cell in lambda
    if(i==8) j=1 ;
  }
  for(int i=0 ; i<nBCell+1 ; i++)
    cout<<xBLow[i]<<endl ;
  // If use total 18 B cells, set the uper edge of the last B Cell - B-9
  //xBLow[nBCell] = 2*halfLength/lambda + offset/lambdaIron;

  // Check the output dir holding output files does exist in current working dir,
  // otherwise create it.
  if(gSystem->AccessPathName("results"))
    gSystem->Exec("mkdir results") ;
  // Check the output dir holding plots does exist in current working dir,
  // otherwise create it.
  if (gSystem->AccessPathName("plots"))
    gSystem->Exec("mkdir plots") ;

  // list of beam energies in MeV
  string Energies[] = {"20000", "50000", "100000",  "180000"} ;
  // particle names
  string Particles[] = {"pi", "pr"} ;
  // Geant4 physics lists
  string PhysicsLists[] = {"FTFP_BERT", "FTFP_BERT_ATL_VALIDATION", "QGSP_BERT", "QGSP_BIC"} ; 

  // // main directory of input MC files
  string InPath = "/rdata2/dengfeng/QT/20.3.7.4/OutputFlagZ2795/" ;

  // loop  over particle types
  for(int i=0 ; i<sizeof(Particles)/sizeof(string) ; i++)
  {
    // loop  over beam energies
    for(int j=0 ; j<sizeof(Energies)/sizeof(string) ; j++)
    {
    // loop  over physics lists
      for(int k=0 ; k<sizeof(PhysicsLists)/sizeof(string) ; k++)
      {
        // Define the input aant file
        TFile* inputFile = new TFile((InPath+"/TileTB90-E"+Energies[j]+"-"+Particles[i]+"-"+PhysicsLists[k]+"/tiletb90-E"+Energies[j]+"-"+Particles[i]+"_"+PhysicsLists[k]+".aant.root").c_str()) ; 
        // Check the input aant file does exists, otherwise continue.
        if (!inputFile)
        {
          cout<< "File: "<<InPath+"/Tile_90-E"+Energies[j]+"-"+Particles[i]+"-"+PhysicsLists[k]+"/tile.tb90-E"+Energies[j]+"-"+Particles[i]+"_"+PhysicsLists[k]+".aant.root"<< " doesn't exist!"<<endl ;
          continue ;
        }
        cout<< "In File: "<<inputFile->GetName()<<endl ;
        // get the input tree-h1000
        TTree* h1000 = (TTree*) inputFile->Get("h1000") ; 
        // check if get tree-h1000 successfully, if not, continue.
        if (!h1000)
        {
          cout<< "Tree: h1000 doesn't exist!"<<endl ;
          continue ;
        }

        // barrel module:
        // each side(positive and negative) of the barrel module is connected to 48 PMTs, 
        // but only 45 are used.(the 32ed, 33rd and 44th are not used),
        // each A,B, C and D cell is connected to two reading out PMTs, except cell-D0.
        // More details, see the map of PMTs and cells in http://zenis.dnp.fmph.uniba.sk/tile.html

        // energies collected by PMTs connected to negative side of Barrel Module 0
        float EfitC0[48] ;
        // energies collected by PMTs connected to negative side of Central Barrel Module
        float EfitC1[48] ;
        // energies collected by PMTs connected to positive side of Barrel Module 0
        float EfitA0[48] ;
        // energies collected by PMTs connected to positive side of Central Barrel Module
        float EfitA1[48] ;

        // fit time
        float TfitC0[48] ; 
        float TfitC1[48] ; 
        float TfitA0[48] ; 
        float TfitA1[48] ; 

        // extended barrel module:
        // 28 of 48 PMTs are connected to extended barrel modules,
        // Cells-E1,E2,E3,E4 are not simulated.
        // More details, see the map of PMTs to cells in http://zenis.dnp.fmph.uniba.sk/tile.html

        // energies collected by PMTs connected to Extended Barrel on positive side
        float EfitD0[48] ;
        // energies collected by PMTs connected to Extended Barrel on negative side
        float EfitE0[48] ;

        // fit time
        float TfitD0[48] ; 
        float TfitE0[48] ; 

        // set branch address
        h1000->SetBranchAddress("EfitC0", EfitC0) ;
        h1000->SetBranchAddress("EfitC1", EfitC1) ;
        h1000->SetBranchAddress("EfitA0", EfitA0) ;
        h1000->SetBranchAddress("EfitA1", EfitA1) ;
        h1000->SetBranchAddress("EfitD0", EfitD0) ;
        h1000->SetBranchAddress("EfitE0", EfitE0) ;
        h1000->SetBranchAddress("TfitC0", TfitC0) ;
        h1000->SetBranchAddress("TfitC1", TfitC1) ;
        h1000->SetBranchAddress("TfitA0", TfitA0) ;
        h1000->SetBranchAddress("TfitA1", TfitA1) ;
        h1000->SetBranchAddress("TfitD0", TfitD0) ;
        h1000->SetBranchAddress("TfitE0", TfitE0) ;

        // total energy (deposited in2 barrel modules and 2 extended barrel modules)
        // Fill this histogram, then fit it by a Gaussian to get the cut range of 
        // total energy
        TH1F* AllRecoE = bookTH1F("AllRecoE", "Measured Energy", "Reconstructed Energy [GeV]", "Events", 100, 0, std::stoi(Energies[j])/1000.*1.5) ;
        // total energy distribution passing total energy cut
        TH1F* RecoE = bookTH1F("RecoE", "Measured Energy", "Reconstructed Energy [GeV]", "Events", 100, 0, std::stoi(Energies[j])/1000.*1.5) ;
        // distribution of energy deposited in barrel module 0 passing total energy cut
        TH1F* RecoEModule0 = bookTH1F("RecoEModule0", "Deposit Energy in Module0", "Reconstructed Energy [GeV]", "Events", 100, 0,std::stoi( Energies[j])/1000.*1.5) ;
        // distribution of energy deposited in central barrel passing total energy cut
        TH1F* RecoECentralModule = bookTH1F("RecoECentralModule", "Deposit Energy in Central Module", "Reconstructed Energy [GeV]", "Events", 100, 0, std::stoi(Energies[j])/1000.*1.5) ;
        // distribution of energy deposited in 2 extended barrel modules passing total energy cut
        TH1F* RecoEEModule = bookTH1F("RecoEEModule", "Deposit Energy in Extended Module", "Reconstructed Energy [GeV]", "Events", 100, 0, std::stoi(Energies[j])/1000.*1.5) ;
        // longitudinal profile dsitribution passing total energy cut
        TH1F* LongitudinalProfile = bookTH1F("LongitudinalProfile", "Longitudinal Profile(Mean Deposit Energy in B, C and A Cells)", "x[#lambda]", "dE/dx[GeV/#lambda]", nBCell, xBLow) ;

        cout<<"Entries: "<<h1000->GetEntries()<<endl ;
        // loop over ann events
        for(int n=0 ; n<h1000->GetEntries() ; n++)
        {
          h1000->GetEntry(n) ;
          float totalE = 0. ; // total energy
          for(int m=0 ; m<48 ; m++)
          {
            if(m==30||m==31||m==43) // these 3 PMTs are not used in Barrel Module
            {
              totalE += 0. ;
            }
            else
            {
              // add energies in two barrel modules
              totalE += Select(EfitC0, TfitC0, m) ; // energy in PMT with index m of negative of Barrel Module 0
              totalE += Select(EfitC1, TfitC1, m) ; // energy in PMT with index m of positive of Barrel Module 0
              totalE += Select(EfitA0, TfitA0, m) ; // energy in PMT with index m of negative of Central Barrel Module
              totalE += Select(EfitA1, TfitA1, m) ; // energy in PMT with index m of positive of Central Barrel Module
            }
            if(m==18||m==19||(m>=24&&m<=29)||m==33||m==34||(m>=42&&m<=47)||m==12||m==13||m==0||m==1) // PMTs with these 10 index are not used
            {
              totalE += 0. ;
            }
            else
            {
              // add energies in two extended barrel modules
              totalE += Select(EfitD0, TfitD0, m) ; // energy in PMT with index m of positive of Extended Barrel Module
              totalE += Select(EfitE0, TfitE0, m) ; // energy in PMT with index m of negative of Extended Barrel Module
            }
          }
          AllRecoE->Fill(totalE/1000.) ; // convert the energy to GeV, fill the total energy to AllRecoE
        }

        // define the gaus function
        TF1* func = new TF1("func","gaus",AllRecoE->GetMean()-2*AllRecoE->GetRMS(),AllRecoE->GetMean()+2*AllRecoE->GetRMS()) ;
        AllRecoE->Fit("func","R") ;
        // select events with total energy within 3 sigmas around peak.
        // Get the total energy cut:
        // low cut of total energy
        float CoreUp = func->GetParameter(1)+3*func->GetParameter(2) ; 
        // high cut of total energy
        float CoreDown = func->GetParameter(1)-3*func->GetParameter(2) ;

        //// number of events passing the total energy cut
        int AccEntries = 0 ;
        for(int n=0 ; n<h1000->GetEntries() ; n++)
        {
          h1000->GetEntry(n) ;
          float totalE = 0. ; // total energy in all 4 modules
          float EModule0 = 0. ; // energy in Barrel Module 0
          float ECModule = 0. ; // energy on Central Barrel Module
          float EEModule = 0. ;  // energy in Extended Barrel Modules
          float EModule0ABCCell[18] ; // energy distribution as a function of the depth in Barrel Module 0. The depth is taken to be the size of the B sub-cell
          float ECModuleABCCell[18] ; // energy distribution as a function of the depth in Central Barrel Module.
          for(int m=0 ; m<18 ; m++)
          {
            EModule0ABCCell[m]=0. ;
            ECModuleABCCell[m]=0. ;
          }

          // get energies in B, C and A cells
          // the map of PMTs to cells is in http://zenis.dnp.fmph.uniba.sk/tile.html
          // get energy in in cells(B9-B1, C8-C1, A10-A1)
          // energy in B9 and A10 of negative side of Central Barrel Modules
          ECModuleABCCell[0] = Select(EfitC1, TfitC1, 44, 45, 46, 47) ; 
          // energy in B9 and A10 of negative side of Barrel Module 0
          EModule0ABCCell[0] += Select(EfitC0, TfitC0, 44, 45, 46, 47) ;
          // energy in C8, B8 and A9 negative side of Central Barrel Modules
          ECModuleABCCell[1] += Select(EfitC1, TfitC1, 41, 40, 37, 38) ;
          // energy in C8, B8 and A9 negative side of Barrel Module 0
          EModule0ABCCell[1] += Select(EfitC0, TfitC0, 41, 40, 37, 38) ;
          // energy in C7, B7 and A8 negative side of Central Barrel Modules
          ECModuleABCCell[2] += Select(EfitC1, TfitC1, 34, 35, 33, 36) ;
          // energy in C7, B7 and A8 negative side of Barrel Module 0
          EModule0ABCCell[2] += Select(EfitC0, TfitC0, 34, 35, 33, 36)  ;
          // energy in C6, B6 and A7 negative side of Central Barrel Modules
          ECModuleABCCell[3] += Select(EfitC1, TfitC1, 28, 29, 27, 30) ;
          // energy in C6, B6 and A7 negative side of Barrel Module 0
          EModule0ABCCell[3] += Select(EfitC0, TfitC0, 28, 29, 27, 30) ;
          // energy in C5, B5 and A6 and A5 negative side of Central Barrel Modules
          ECModuleABCCell[4] += Select(EfitC1, TfitC1, 21, 22, 23, 24, 19, 20) ;
          // energy in C5, B5 and A6 and A5 negative side of Barrel Module 0
          EModule0ABCCell[4] += Select(EfitC0, TfitC0, 21, 22, 23, 24, 19, 20) ;
          // energy in C4, B4 and A4 negative side of Central Barrel Modules
          ECModuleABCCell[5] += Select(EfitC1, TfitC1, 16, 17, 15, 18) ;
          // energy in C4, B4 and A4 negative side of Barrel Module 0
          EModule0ABCCell[5] += Select(EfitC0, TfitC0, 16, 17, 15, 18) ;
          // energy in C3, B3 and A3 negative side of Central Barrel Modules
          ECModuleABCCell[6] +=  Select(EfitC1, TfitC1, 11, 12, 9, 10) ;
          // energy in C3, B3 and A3 negative side of Barrel Module 0
          EModule0ABCCell[6] += Select(EfitC0, TfitC0, 11, 12, 9, 10) ;
          // energy in C2, B2 and A2 negative side of Central Barrel Modules
          ECModuleABCCell[7] += Select(EfitC1, TfitC1, 6, 7, 5, 8) ;
          // energy in C2, B2 and A2 negative side of Barrel Module 0
          EModule0ABCCell[7] += Select(EfitC0, TfitC0, 6, 7, 5, 8) ;
          // energy in C1, B1 and A1 negative side of Central Barrel Modules
          ECModuleABCCell[8] += Select(EfitC1, TfitC1, 2, 3, 1, 4) ;
          // energy in C1, B1 and A1 negative side of Barrel Module 0
          EModule0ABCCell[8] += Select(EfitC0, TfitC0, 2, 3, 1, 4) ;

          // get the energy in cells (B-1-B-9, A-1-A-10 and C-1-C-8)
          // positive side of Module 0 and Central Barrel Module
          ECModuleABCCell[9] += Select(EfitA1, TfitA1, 2, 3, 1, 4) ;
          EModule0ABCCell[9] += Select(EfitA0, TfitA0, 2, 3, 1, 4) ;
          ECModuleABCCell[10] += Select(EfitA1, TfitA1, 6, 7, 5, 8) ;
          EModule0ABCCell[10] += Select(EfitA0, TfitA0, 6, 7, 5, 8) ;
          ECModuleABCCell[11] += Select(EfitA1, TfitA1, 11, 12, 9, 10) ;
          EModule0ABCCell[11] += Select(EfitA0, TfitA0, 11, 12, 9, 10) ;
          ECModuleABCCell[12] += Select(EfitA1, TfitA1, 16, 17, 15, 18) ;
          EModule0ABCCell[12] += Select(EfitA0, TfitA0, 16, 17, 15, 18) ;
          ECModuleABCCell[13] += Select(EfitA1, TfitA1, 21, 22, 23, 24, 19, 20) ;
          EModule0ABCCell[13] += Select(EfitA0, TfitA0, 21, 22, 23, 24, 19, 20) ;
          ECModuleABCCell[14] += Select(EfitA1, TfitA1, 28, 29, 27, 30) ;
          EModule0ABCCell[14] += Select(EfitA0, TfitA0, 28, 29, 27, 30) ;
          ECModuleABCCell[15] += Select(EfitA1, TfitA1, 34, 35, 33, 36) ;
          EModule0ABCCell[15] += Select(EfitA0, TfitA0, 34, 35, 33, 36) ;
          ECModuleABCCell[16] += Select(EfitA1, TfitA1, 40, 41, 37, 38) ;
          EModule0ABCCell[16] += Select(EfitA0, TfitA0, 40, 41, 37, 38) ;
          ECModuleABCCell[17] += Select(EfitA1, TfitA1, 44, 45, 46, 47) ;
          EModule0ABCCell[17] += Select(EfitA0, TfitA0, 44, 45, 46, 47) ;

          float All = 0. ; // total energy
          for(int m=0 ; m<18 ; m++)
          {
            All += ECModuleABCCell[m] ;
            All += EModule0ABCCell[m] ;
          }

           // calculate total energy and energy in Barrrel Module 0 and Cental Barrel
          for(int m=0 ; m<48 ; m++)
          {
            // these 3 PMTs are not used in Barrel Module
            if(m==30||m==31||m==43)
            {
              totalE += 0. ;
              ECModule += 0. ;
              EModule0 += 0. ;
            }
            else
            {
              // add total energy
              totalE += Select(EfitC0, TfitC0, m) ;
              totalE += Select(EfitC1, TfitC1, m) ;
              totalE += Select(EfitA0, TfitA0, m) ;
              totalE += Select(EfitA1, TfitA1, m) ;
              // add energy in Barrrel Module 0 and Cental Barrel
              EModule0 += Select(EfitC0, TfitC0, m) ;
              EModule0 += Select(EfitA0, TfitA0, m) ;
              ECModule += Select(EfitC1, TfitC1, m) ;
              ECModule += Select(EfitA1, TfitA1, m) ;
            }
            // these 20 PMTs are not used in Barrel Module
            if(m==18||m==19||(m>=24&&j<=29)||m==33||m==34||(m>=42&&m<=47)||m==12||m==13||m==0||m==1)
            {
              totalE += 0. ;
              EEModule += 0. ;
            }
            else
            {
              // calculate total energy and energy in Extended Barrel Modules
              // add total energy
              totalE += Select(EfitD0, TfitD0, m) ;
              totalE += Select(EfitE0, TfitE0, m) ;
              // add energy and energy in Extended Barrel Modules
              EEModule += Select(EfitD0, TfitD0, m) ;
              EEModule += Select(EfitE0, TfitE0, m) ;
            }
          }

          // select events with total energy within 3 sigmas around peak
          if(totalE/1000<CoreDown||totalE/1000>CoreUp)
            continue ;
          // fill the energy
          RecoEModule0->Fill(EModule0/1000.) ;
          RecoEEModule->Fill(EEModule/1000.) ;
          RecoECentralModule->Fill(ECModule/1000.) ;
          RecoE->Fill(totalE/1000.) ;

          for(int m=0 ; m<15 ; m++)
          {
            // fill the energy/length(in lambda unit) in each B cell length
            // The Extended Barrel modules are not used in the measurement 
            // of the longitudinal shower profile. Instead, the energy measured 
            // in the Barrel Module 0 is multiplied by a factor of two.
            LongitudinalProfile->Fill(LongitudinalProfile->GetBinCenter(m+1), ((ECModuleABCCell[m]+2*EModule0ABCCell[m])/1000.)/LongitudinalProfile->GetBinWidth(m+1)) ;
          }
          // count the number of events passing total energy cut
          AccEntries++ ;
        }
        cout<<"AccEntries: "<<AccEntries<<endl ;
        // scale the profile dsitribution by number of total accepted events
        // to get the mean distribution
        LongitudinalProfile->Scale(1./AccEntries) ;

        for(int m=0 ; m<18 ; m++)
        {
          // at the tail the distribution a flat, the energy is at level of noise
          // so truncate the tail
          if(LongitudinalProfile->GetBinContent(m+1)<0.05)
          {
            LongitudinalProfile->SetBinContent(m+1, 0.) ;
            LongitudinalProfile->SetBinError(m+1, 0.) ;
          }
        }

        // all outputs of each type of particles are placed in its own dir
        // if this dir doesn't exist, create it.
        string Sub_OutPath = "./results/"+Particles[i]+"/" ;
        if(gSystem->AccessPathName(Sub_OutPath.c_str()))
        {
          cout<<Sub_OutPath<<" doesn't exist! Making......"<<endl ;
          gSystem->Exec(("mkdir "+Sub_OutPath).c_str()) ;
        }

        string PhysicsList = PhysicsLists[k] ;
        if(PhysicsLists[k]=="FTFP_BERT_ATL_VALIDATION") 
          PhysicsList = "FTFP_BERT_ATL" ;
        // create output file 
        TFile* outputFile = new TFile((Sub_OutPath+"/tiletb90-E"+Energies[j]+"-"+Particles[i]+"_"+PhysicsList+".root").c_str(),"RECREATE") ;
        cout<<"outputFile: "<<outputFile->GetName()<<endl ;
        // write histograms
        //AllRecoE->Write() ;
        RecoE->Write() ;
        RecoEModule0->Write() ;
        RecoEEModule->Write() ;
        RecoECentralModule->Write() ;
        LongitudinalProfile->Write() ;
        outputFile->Close() ;
      }
      // Done looping over physics lists
    }
    // Done looping over beam energies
  }
  // Done looping over particles
  return 0 ;
}
