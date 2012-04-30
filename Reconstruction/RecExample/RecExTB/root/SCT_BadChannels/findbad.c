/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//macro to find SCT dead and noisy channels
//outputs a list in terms of offline identifiers
//
//Based on orignal code of Sergio Gonzalez Sevilla
//by Paul Bell 29/10/04 
//paul.bell@cern.ch
//

#include <iostream>
#include <strstream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cstring>
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TProfile.h"
#include "TF1.h"
//
//Requirements: Need to find bad chips and bad strips.
//Due to beam profile, looking at link averages not so useful.
//Look at one chip at a time. Then bad chips will stand out
//and bad strips within chips will stand out
//
//
void findbad(int run)
//
{
  char filename[500], path[500], ntuple[100], title[200];
  char maskfile_onl[200], maskfile_offl[200];
  const int NPLANES = 16;
  const int NCHIPS = 6;
  const int MAX     = 10000;
  int strips, rdo_nhits;
  float rdo_strips[MAX],rdo_layer[MAX],rdo_phi[MAX],rdo_side[MAX];
  int ievent, plane, total;
  int noisy_channel[NPLANES][768];
  int dead_channel[NPLANES][768];
  bool draw = true;

  gROOT->Reset();

  sprintf(path,"rfio:/castor/cern.ch/user/i/idctb/Validation/ntuple/");  
  sprintf(ntuple, "ntuple_run%d.root", run);
  sprintf(filename,"%s%s", path, ntuple);  
  TFile *file = TFile::Open(filename,"");

  TTree *tree = (TTree*)file->Get("TB/tree");
  
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("sct_rdo_*",1);
  tree->SetBranchAddress("Event",&ievent);
  tree->SetBranchAddress("sct_rdo_nhits",&rdo_nhits);
  tree->SetBranchAddress("sct_rdo_strips",rdo_strips);
  tree->SetBranchAddress("sct_rdo_layer",rdo_layer);
  tree->SetBranchAddress("sct_rdo_phi",rdo_phi);
  tree->SetBranchAddress("sct_rdo_side",rdo_side);
  
  int entries = tree->GetEntries();
  cout << entries << " events\n" << endl;
  
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetOptFit(111111);
  gStyle->SetOptStat("nemruo");

  // build hitmaps
  int hits[16][768]={0};  
  int module, layer, phi, side, strip;
  TH1F *hitmap[NPLANES];
  for ( layer=0; layer<4; layer++){
    for ( phi=0; phi<2; phi++){
      for (side=0; side<2; side++){

	plane = 2*(2*layer + phi) + side;

	sprintf(title,"hitmap%d",plane);
	hitmap[plane]= new TH1F(title,title,768,-0.5,767.5);
	sprintf(title, "SCT hitmap plane %d [%d:%d:%d]", plane, layer, phi, side);
	cout << title << endl;
	
	hitmap[plane]->SetTitle(title); 
	hitmap[plane]->SetStats(kFALSE); 
	hitmap[plane]->SetXTitle("Strip");   
	if (phi==0) hitmap[plane]->SetFillColor(kRed); 
	else hitmap[plane]->SetFillColor(kBlue);
      }
    }
  }
  int count=0;
  for (Int_t i=0; i<entries; i++) {
    tree->GetEntry(i);
    
    if(i%(entries/4)==0) cout << i << endl;
     
    if(!(rdo_nhits>0)) count++;

    for(int j=0; j<rdo_nhits; j++) {       
      layer  = rdo_layer[j];
      phi    = rdo_phi[j];
      side   = rdo_side[j]; 
      plane  = 2*(2*layer + phi) + side;
      strip  = rdo_strips[j];	 
      if(layer<4 && phi<2 && strip<768){
	hits[plane][strip]++;
      }
    }     
  }
  
  cout << "no sct rdos " << count << endl;
  //zeroth bin is for underflow
  for(int i=0; i<16; i++){
    for(int j=0; j<768; j++){
      hitmap[i]->SetBinContent(j+1, hits[i][j]);
    }
  }
   
  if(draw){
    c1 = new TCanvas("c1", "SCT hitmaps");
    c1->SetBorderMode(0);
    c1->Divide(4,4);  
    for(int i=0; i<NPLANES; i++){
      c1->cd(i+1);
      //gPad->SetLogy();
      hitmap[i]->Draw();
    }
    c1->Update();     
  }

  //reset flags
  for(int plane=0; plane<NPLANES; plane++){
    for(int chip=0;chip<NCHIPS;chip++){
      for(int j=0; j<128; j++){
	int channel = (chip*128)+j;
	noisy_channel[plane][channel]=0;
	dead_channel[plane][channel]=0;
      }
    }
  }
  ///////////////////////////////////////////////////////////
  // get mean hit rates by chip

  float mean[NPLANES][NCHIPS]={0};
  int content;

  for(int plane=0; plane<NPLANES; plane++){
     
     for(int chip=0;chip<NCHIPS;chip++){
   
       for(int j=0; j<128; j++){
	 
	 int channel = (chip*128)+j;
	 content = hitmap[plane]->GetBinContent(channel+1);
	 mean[plane][chip] += content;
	 //if (plane==5) cout << content << endl;
       }

       mean[plane][chip] = mean[plane][chip]/128.0;

       //if (plane==5) cout << "mean[" << plane << "][" << chip << "]=" << mean[plane][chip] << endl;
     }
  }

  /////////////////////////////////////////////////////
  // calculate sd of hits by chip

  float sqd_diff,sum_sqd_diff[NPLANES][NCHIPS]={0};
  float variance[NPLANES][NCHIPS]={0}, sd[NPLANES][NCHIPS]={0};

  for(int plane=0; plane<NPLANES; plane++){
     
     for(int chip=0;chip<NCHIPS;chip++){

       for(int j=0; j<128; j++){
	 
	 int channel = (chip*128)+j;

	 content = hitmap[plane]->GetBinContent(channel+1);

	 sqd_diff = (mean[plane][chip]-content)*(mean[plane][chip]-content);
	 //cout << content << " " << mean[plane][chip] << endl;
	 sum_sqd_diff[plane][chip]=sum_sqd_diff[plane][chip]+sqd_diff;

       }

       variance[plane][chip] = sum_sqd_diff[plane][chip]/128.0;
       
       sd[plane][chip] = sqrt(variance[plane][chip]);
 
       //if(plane==13) cout << variance[plane][chip] << endl;
     
     }
  }

  
  /////////////////////////////////////////////////
  //flag noisy or dead chips
  //also find max occ of ok chips
  float maxavhits[NPLANES];

  for(int plane=0; plane<NPLANES; plane++){
     
    maxavhits[plane]=0.0;
    for(int chip=0;chip<NCHIPS;chip++){
       
       //whole chips - if quarter of chip noisy then it gets away
       if(mean[plane][chip]>0.25*entries){
	 for(int ch =0;ch<128;ch++){
	   int cch = (chip*128)+ch;
	   noisy_channel[plane][cch]=1;
	 }    
       }else{
	 if(mean[plane][chip]>maxavhits[plane]){
	   maxavhits[plane]=mean[plane][chip];
	 }
       }  

       //check for whole chips dead
       if(sd[plane][chip]==0){
	 for(int ch =0;ch<128;ch++){
	   int cch = (chip*128)+ch;
	   dead_channel[plane][cch]=1;
	 }    
       }

     }
    //    cout << "max " <<maxavhits[plane] << endl;    
  }

  //////////////////////////////////////////////////////
  //now check for noisy channels within chips
  
  for(int plane=0; plane<NPLANES; plane++){
     
     for(int chip=0;chip<NCHIPS;chip++){
       
       for(int j=0; j<128; j++){
	 
	 int channel = (chip*128)+j;
	 
	 //within chip - 
	 //assume 5 sd and also that its 2* greater than good max chip av
	 //and gt 5 (poisson)

	 if( float((hitmap[plane]->GetBinContent(channel+1)))- mean[plane][chip] > (5*sd[plane][chip]) && hitmap[plane]->GetBinContent(channel+1)>2.*maxavhits[plane] &&   hitmap[plane]->GetBinContent(channel+1)> 5){
	   cout << "NOISY CANDIDATE Plane " << plane << " channel " << channel << " Hits:  " << float(hitmap[plane]->GetBinContent(channel+1)) <<" Chip Mean: "<< mean[plane][chip] <<" Chip SD: "<< sd[plane][chip] << " max av " << maxavhits[plane] << endl;
	   noisy_channel[plane][channel] = 1;
	 }
	 
	 //abs
	 if( float(abs(hitmap[plane]->GetBinContent(channel+1))) > 0.25*entries){
	   noisy_channel[plane][channel] = 1;
	 }
	 
       }
     }
   }

  //sprintf(maskfile_onl,"/afs/cern.ch/user/i/idctb/SCT_BadChannels/archive/softmask_onl_%d.txt",run);
  sprintf(maskfile_offl,"/afs/cern.ch/user/i/idctb/SCT_BadChannels/archive/softmask_onl_%d.txt",run);
  
  //ofstream outfile_onl;
  ofstream outfile_offl;
  
  // outfile_onl.open(maskfile_onl);
  outfile_offl.open(maskfile_offl);

   int layer, phi, side, rob, link, strip_onl;
  
   cout << "------------------------------" << endl;
   cout << "Noisy Channel Summary by Plane" << endl;
   cout << "------------------------------" << endl;
   
   //flag all noisy
   for(int plane=0; plane<NPLANES; plane++){    
     total = 0;
     
     for(int strip_offl=0; strip_offl<768; strip_offl++) {	
       if( noisy_channel[plane][strip_offl] ){

	 /*This is nasty.
	   Work out again what layer, phi and side are (assuming mapping fixed!!)
	   Also work out what the online idendifiers are!
	   store two files - softmask_onl, softmask_offl
	   Will use the former to upload to db
	 */
	 if(plane==0){
	   layer=0;
	   phi=0;
	   side=0;
	   rob=0;
	   link=11;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==1){
	   layer=0;
	   phi=0;
	   side=1;
	   rob=0;
	   link=10;
	   strip_onl = strip_offl;
	 }
	 if(plane==2){
	   layer=0;
	   phi=1;
	   side=0;
	   rob=0;
	   link=3;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==3){
	   layer=0;
	   phi=1;
	   side=1;
	   rob=0;
	   link=2;
	   strip_onl = strip_offl;
	 }
	 if(plane==4){
	   layer=1;
	   phi=0;
	   side=0;
	   rob=0;
	   link=1;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==5){
	   layer=1;
	   phi=0;
	   side=1;
	   rob=0;
	   link=0;
	   strip_onl = strip_offl;
	 }
	 if(plane==6){
	   layer=1;
	   phi=1;
	   side=0;
	   rob=0;
	   link=9;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==7){
	   layer=1;
	   phi=1;
	   side=1;
	   rob=0;
	   link=8;
	   strip_onl = strip_offl;
	 }
	 if(plane==8){
	   layer=2;
	   phi=0;
	   side=0;
	   rob=0;
	   link=13;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==9){
	   layer=2;
	   phi=0;
	   side=1;
	   rob=0;
	   link=12;
	   strip_onl = strip_offl;
	 }
	 if(plane==10){
	   layer=2;
	   phi=1;
	   side=0;
	   rob=0;
	   link=19;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==11){
	   layer=2;
	   phi=1;
	   side=1;
	   rob=0;
	   link=18;
	   strip_onl = strip_offl;
	 }
	 if(plane==12){
	   layer=3;
	   phi=0;
	   side=0;
	   rob=0;
	   link=23;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==13){
	   layer=3;
	   phi=0;
	   side=1;
	   rob=0;
	   link=22;
	   strip_onl = strip_offl;
	 }
	 if(plane==14){
	   layer=3;
	   phi=1;
	   side=0;
	   rob=0;
	   link=21;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==15){
	   layer=3;
	   phi=1;
	   side=1;
	   rob=0;
	   link=20;
	   strip_onl = strip_offl;
	 }

	 total++;
	 //2 is the status for noisy
	 outfile_offl << layer <<" " << phi <<" "<< side <<" "<< strip_offl << " " << 2 << endl;
	 //outfile_onl << rob <<" " << link <<" "<< strip_onl << " " << 2 << endl;
	 
	 //	 cout << " ROB " << rob 
	 //   << " Link "<< link 
	 //   << " strip on line " << strip_onl 
	 //   << " layer " << layer 
	 //   << " phi "   << phi 
	 //   << " side "  << side 
	 //   << " strip off line " << strip_offl 
	 //   << endl;

	 //	 cout << " Plane " << plane 
	 //   << " strip off line " << strip_offl 
	 //   << endl;
       }
     }    
     cout << "plane " << plane <<  " : total noisy channels = " << total << endl;
   }

   //flag all dead

   cout << "------------------------------" << endl;
   cout << "Dead Channel Summary by Plane" << endl;
   cout << "------------------------------" << endl;
   

   for(int plane=0; plane<NPLANES; plane++){    
     total = 0;
     
     for(int strip_offl=0; strip_offl<768; strip_offl++) {	
       if( dead_channel[plane][strip_offl] ){

	 /*This is nasty.
	   Work out again what layer, phi and side are (assuming mapping fixed!!)
	   Also work out what the online idendifiers are!
	   store two files - softmask_onl, softmask_offl
	   Will use the former to upload to db
	   ARE THE STRIPS INVERTED OR NOT?
	 */
	 if(plane==0){
	   layer=0;
	   phi=0;
	   side=0;
	   rob=0;
	   link=11;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==1){
	   layer=0;
	   phi=0;
	   side=1;
	   rob=0;
	   link=10;
	   strip_onl = strip_offl;
	 }
	 if(plane==2){
	   layer=0;
	   phi=1;
	   side=0;
	   rob=0;
	   link=3;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==3){
	   layer=0;
	   phi=1;
	   side=1;
	   rob=0;
	   link=2;
	   strip_onl = strip_offl;
	 }
	 if(plane==4){
	   layer=1;
	   phi=0;
	   side=0;
	   rob=0;
	   link=1;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==5){
	   layer=1;
	   phi=0;
	   side=1;
	   rob=0;
	   link=0;
	   strip_onl = strip_offl;
	 }
	 if(plane==6){
	   layer=1;
	   phi=1;
	   side=0;
	   rob=0;
	   link=9;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==7){
	   layer=1;
	   phi=1;
	   side=1;
	   rob=0;
	   link=8;
	   strip_onl = strip_offl;
	 }
	 if(plane==8){
	   layer=2;
	   phi=0;
	   side=0;
	   rob=0;
	   link=13;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==9){
	   layer=2;
	   phi=0;
	   side=1;
	   rob=0;
	   link=12;
	   strip_onl = strip_offl;
	 }
	 if(plane==10){
	   layer=2;
	   phi=1;
	   side=0;
	   rob=0;
	   link=19;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==11){
	   layer=2;
	   phi=1;
	   side=1;
	   rob=0;
	   link=18;
	   strip_onl = strip_offl;
	 }
	 if(plane==12){
	   layer=3;
	   phi=0;
	   side=0;
	   rob=0;
	   link=23;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==13){
	   layer=3;
	   phi=0;
	   side=1;
	   rob=0;
	   link=22;
	   strip_onl = strip_offl;
	 }
	 if(plane==14){
	   layer=3;
	   phi=1;
	   side=0;
	   rob=0;
	   link=21;
	   strip_onl = 767-strip_offl;
	 }
	 if(plane==15){
	   layer=3;
	   phi=1;
	   side=1;
	   rob=0;
	   link=20;
	   strip_onl = strip_offl;
	 }

	 total++;
	 //3 is the status for dead
	 outfile_offl << layer <<" " << phi <<" "<< side <<" "<< strip_offl << " " << 3 << endl;
	 //outfile_onl << rob <<" " << link <<" "<< strip_onl << " " << 3 << endl;
	 
	 //	 cout << " ROB " << rob 
	 //   << " Link "<< link 
	 //   << " strip on line " << strip_onl 
	 //   << " layer " << layer 
	 //   << " phi "   << phi 
	 //   << " side "  << side 
	 //   << " strip off line " << strip_offl 
	 //   << endl;

	 //cout << " Plane " << plane 
	 //   << " strip off line " << strip_offl 
	 //   << endl;
       }
     }    
     cout << "plane " << plane <<  " : total dead channels = " << total << endl;
   }



   //   outfile_onl.close();
   outfile_offl.close();
}
