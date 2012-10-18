/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "APReweightND.h"
#include "APWeightEntry.h"
#include "APEvtWeight.h"
#include "APWeightSum.h"
#include "TH2D.h"
#include "TStyle.h"
#include "THnSparse.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "TRandom3.h"

using namespace std;

int main(int argc, char **argv) {

  //============= Initialize ===================================================
  gROOT->ProcessLine("#include <vector>");
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetPadTopMargin(0.09);
  gStyle->SetPadLeftMargin(0.12);
  gStyle->SetTitleYOffset(1.55);
  // you may want to set your own random seed here
  TRandom3 rndm(0 );

  //Define variables for logging and output:
  vector<int> configuration(16, 0);             // vector for dilepton events: each index holds on set of trigger bits
  int nPassDilepTrig = 0;                       // the number of events passing the asymmetric dilepton trigger
  double SumOfWeights = 0.;                     // the sum of weights for each ensemble.


  // open the output file and create the output tree
  TFile *OUT = new TFile( "ensembles.root", "RECREATE" );
  TTree *tOUT = new TTree( "ensembles","ensembles" );
  tOUT -> Branch( "passes", &nPassDilepTrig );
  tOUT -> Branch( "weightSum", &SumOfWeights );
  for( unsigned int i = 0; i < configuration.size(); ++i ) {
    char name[100];
    sprintf( name, "config_%i", i );
    tOUT -> Branch( name, &configuration.at(i) );
  }

  

  int nToys = (argc > 1 ) ? atoi(argv[1]) : 1.e5;         // set / read the values for the number of Toy Events
  int nLeptons = (argc > 2 ) ? atoi(argv[2]) : 2;         // set the lepton multipilcity
  int nEnsemble = (argc > 3 ) ? atoi(argv[3]) : 1;        // set the number of ensembles to be run
  
  
  
  //Load the trigger maps for the asymmetric dilepton trigger and setup the APReweightND instances
  TFile *fTrig = new TFile( "../Common/AsymmetricTriggerMaps.root" );
  //absolute efficiency of first leg
  THnSparseD* leg1Pass = (THnSparseD*)fTrig -> Get( "ths_leg1_abs_num" );
  THnSparseD* leg1All = (THnSparseD*)fTrig -> Get( "ths_leg1_abs_den" );
  //absolute efficiency of second leg
  THnSparseD* leg2Pass = (THnSparseD*)fTrig -> Get( "ths_leg2_abs_num" );
  THnSparseD* leg2All = (THnSparseD*)fTrig -> Get( "ths_leg2_abs_den" );
  //efficiency of second leg under the condition that the first leg did fire
  THnSparseD* leg2PassCondPass = (THnSparseD*)fTrig -> Get( "ths_leg2_condPass_num" );
  THnSparseD* leg2AllCondPass = (THnSparseD*)fTrig -> Get( "ths_leg2_condPass_den" );
  //efficiency of the second leg under the condition that the first leg did not fire
  THnSparseD* leg2PassCondFail = (THnSparseD*)fTrig -> Get( "ths_leg2_condFail_num" );
  THnSparseD* leg2AllCondFail = (THnSparseD*)fTrig -> Get( "ths_leg2_condFail_den" );

  APReweightND *rwleg1 = new APReweightND( leg1All, leg1Pass, true );
  APReweightND *rwleg2 = new APReweightND( leg2All, leg2Pass, true );
  APReweightND *rwleg2p = new APReweightND( leg2AllCondPass, leg2PassCondPass, true );
  APReweightND *rwleg2f = new APReweightND( leg2AllCondFail, leg2PassCondFail, true );


  //Loop over all ensembles requested
  for( int ENS = 0; ENS < nEnsemble; ++ENS ) {
    // for each ensemble, create a new APWeightSum
    APWeightSum *weightSum = new APWeightSum();
    std::cout << "PROCESSSING ENSEMBLE #" << ENS << endl;
    // reset saved trigger states for dilepton events
    for( unsigned int i = 0; i < configuration.size(); ++i ) {configuration.at(i) = 0;}
    // reset number of passing events and weight sum
    SumOfWeights = 0.;
    nPassDilepTrig = 0;
   
    // create tree holding the randomly created leptons; old data is deleted, only the data for the last ensemble is stored
    TFile *fData = new TFile( "pseudoData.root", "RECREATE" );
    TTree *tree = new TTree( "data", "data" );
    vector<float> *pt = new vector<float>;
    vector<float> *eta = new vector<float>;
    vector<float> *phi = new vector<float>;
    vector<float> *comb = new vector<float>;
    vector<float> *iso = new vector<float>;
    vector<int> *passLeg1 = new vector<int>;
    vector<int> *passLeg2 = new vector<int>;
    tree -> Branch( "pt", &pt );
    tree -> Branch( "eta", &eta );
    tree -> Branch( "phi", &phi );
    tree -> Branch( "comb", &comb );
    tree -> Branch( "iso", &iso );
    tree -> Branch( "leg1", &passLeg1 );
    tree -> Branch( "leg2", &passLeg2 );
    
    // loop over the number of toy events
    // in the analysis, this would be the event loop!
    for( int i = 0; i < nToys; ++i ) {
      // delete data from last event
      pt -> clear();
      eta -> clear();
      phi -> clear();
      comb -> clear();
      iso -> clear();
      passLeg1 -> clear();
      passLeg2 -> clear();
    
      //setup vector for lepton properties, and APEvtWeight
      vector<double*> props;
      // setup the APEvtWeight: For demonstration, we chose muons; can be kDiElectron/kDiJet/kDiTau as well!
      APEvtWeight weight(APEvtWeight::kDiMuon);

      // loop over all leptons in the event. We assume all leptons have the same flavour, i.e. muons in this example. Do this seperately for muons and electrons!
      for( int j = 0; j < nLeptons; ++j ) {
        pt -> push_back( rndm.Uniform(0., 100000.) );         // random pt
        eta -> push_back( rndm.Uniform( -2.4, 2.4 ) );        // random eta
        phi -> push_back( rndm.Uniform( -M_PI, M_PI ));       // random phi
        comb -> push_back( 1.0 );                             // combined muon
        iso -> push_back( 0.0 );                              // isolated muon
        double props_i[] = { (*pt)[j], (*eta)[j], (*phi)[j], (*comb)[j], (*iso)[j] };
      
        
        // add each lepton four times, each time with a different efficiency!
        // note the order! it is important! wrong order will result in a wrong weight!
        weight.AddWeightToEvt(rwleg1->GetWeight(props_i));      // absolute probability that lepton #i passes the first leg
        weight.AddWeightToEvt(rwleg2->GetWeight(props_i));      // absoltue probability that lepton #i passes the second leg
        weight.AddWeightToEvt(rwleg2f->GetWeight(props_i));     // probability that lepton #i passes the second leg given that it did NOT pass the first leg
        weight.AddWeightToEvt(rwleg2p->GetWeight(props_i));     // probability that lepton #i passes the second leg given that it did pass the first leg


        //In a standard analysis, you would be DONE here! The following is only for the ensemble test!
        // now throw a random number according to the efficiencies associated to this lepton to decide if it passes a trigger, or not
        props.push_back(props_i);
        int bins[5];
        int binsCond[5];
        for( int k = 0; k < 5; ++k ) { bins[k] = leg1Pass -> GetAxis(k) -> FindFixBin( props_i[k]); }
        for( int k = 0; k < 5; ++k ) { binsCond[k] = leg2PassCondPass -> GetAxis(k) -> FindFixBin( props_i[k]); }
        
        
        // calculate the efficiencies associated to the lepton
        double eff1 = (double)leg1Pass -> GetBinContent( bins )/(double)leg1All -> GetBinContent( bins );
        double eff2p = (double)leg2PassCondPass -> GetBinContent( binsCond )/(double)leg2AllCondPass -> GetBinContent( binsCond );
        double eff2f = (double)leg2PassCondFail -> GetBinContent( binsCond )/(double)leg2AllCondFail -> GetBinContent( binsCond );
        
        // print some efficiencies of wanted 
        //cout << "have " << eff1 << " and " << eff2p << " and " << eff2f << endl;
        // randomly get a boolean trigger decision:
        passLeg1 -> push_back( (rndm.Uniform(0., 1.) <= eff1) ? 1 : 0 );
        if( (*passLeg1)[j] == 0 ) passLeg2 -> push_back( (rndm.Uniform(0.,1.) <= eff2f) ? 1 : 0 );
        if( (*passLeg1)[j] == 1 ) passLeg2 -> push_back( (rndm.Uniform(0.,1.) <= eff2p) ? 1 : 0 );
      }

      // loop over leptons is done. now check how many passed the trigger in the 'trigger simulation'
      bool pass = false;
      // get the trigger-bit configuration; this is for logging for dilepton events only and can be ignored at this point!
      vector<int> bits;
      bits.push_back( (*passLeg2)[1] );
      bits.push_back( (*passLeg1)[1] );
      bits.push_back( (*passLeg2)[0] );
      bits.push_back( (*passLeg1)[0] );
      int config = 0;
      for( int j = 0; j < 4; ++j ) {
        config += bits.at(j)*pow( 2, j );
      }
      configuration.at(config) += 1; 

      // count how many different leptons have passed the trigger
      // check if one lepton issued the first leg;
      // if one lepton issued the first leg, check if a different second lepton issued the second leg
      // if yes, event passes the trigger; if not, event is rejected;
      for( int j = 0; j < nLeptons; ++j ) {
        if( (*passLeg1)[j] == 1 ) {
          for( int k = 0; k < nLeptons; ++k ) {
            if( k == j ) continue;
            if( (*passLeg2)[k] == 1 ) pass = true;
          }
        }
      }
      if( pass ) nPassDilepTrig += 1; 
      
      // finally add weight to sum of weights
      SumOfWeights += weight.GetWeight();
      weightSum->AddEvt( &weight );
      // fill event output tree
      tree -> Fill();
    }
    // cleanup and saving
    delete weightSum;
    tree -> Write();
    delete tree;
    fData -> Close();
    fTrig -> Close();
    delete pt;
    delete eta;
    delete phi;
    delete comb;
    delete iso;
    delete passLeg1;
    delete passLeg2;
    
    // state some general information:
    cout << "got sum of weights  :" << SumOfWeights << endl;
    cout << "events passing      : " << nPassDilepTrig << endl; 
    cout << "=========" << endl;
    // printout the fraction of the individual trigger states
    /*
    for( unsigned int i = 0; i < configuration.size(); ++i ) {
      cout << "\t config #" << i << "  have  " << (double)configuration.at(i)/(double)nToys << endl;
    }
    */
    tOUT -> Fill();
  }
  gDirectory = OUT;
  tOUT -> Write();
  delete tOUT;
  OUT -> Close();
  return 0;
}
