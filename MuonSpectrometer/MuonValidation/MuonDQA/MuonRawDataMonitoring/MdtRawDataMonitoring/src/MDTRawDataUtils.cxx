/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
//Utils for the main MdtRawDataValAlg.cxx
//Part of MdtRawDataValAlg.h
//Authors
//   see MdtRawDataValAlg.cxx
//   Split off of MdtRawDataValAlg.cxx Nov 2009
///////////////////////////////////////////////////////////////////////////

#include "MdtRawDataMonitoring/MdtRawDataValAlg.h"
#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include <TError.h>
#include <string>
#include <TBox.h>
#include <TList.h>
#include <TLine.h>
#include <TColor.h>
#include <TString.h>
#include <TRegexp.h>


using std::string;

StatusCode MdtRawDataValAlg::binMdtGlobal( TH2* &h, char ecap ) {

  //Set x-axis labels
  int LowerEta=0;
  int UpperEta=0;
  if(ecap=='B') {
    LowerEta = -8;
    UpperEta = 8;
  }
  else {
    LowerEta = -6;
    UpperEta = 6;
  }
  TString eta_s;
  for(int ieta = LowerEta; ieta!= UpperEta+1; ++ieta){
    if(ieta==0 && ecap=='E') continue;
    eta_s = ecap;
    (ieta<0)? eta_s+="C":((ieta>0)?eta_s+="A":eta_s+="B");
    eta_s+=returnString(std::abs(ieta));
    h->Fill(eta_s,1,0);
  }	    	  
  
  //Set y-axis labels
  TString phi_s;
  for(int iphi=1; iphi<=16; iphi++) {
    if(iphi<10)  phi_s = "0"+returnString(iphi);
    else phi_s = returnString(iphi);
    if(ecap == 'B' && (iphi == 11 || iphi == 15)){
      h->Fill(eta_s,"I,"+phi_s+",R",0);
      h->Fill(eta_s,"I,"+phi_s+",M",0);
      h->Fill(eta_s,"M,"+phi_s,0);
      h->Fill(eta_s,"O,"+phi_s,0);
    }
    else{
      h->Fill(eta_s,"I,"+phi_s,0);
      h->Fill(eta_s,"M,"+phi_s,0);
      h->Fill(eta_s,"O,"+phi_s,0);
      if(iphi%2==0 && ecap=='B') h->Fill(eta_s,"E,"+phi_s,0);
      else if(ecap=='E') h->Fill(eta_s,"E,"+phi_s,0);
    }
  }

  h->LabelsDeflate("X");
  h->LabelsDeflate("Y");
  h->LabelsOption("a", "Y");
  h->GetYaxis()->SetLabelSize(0.025);
  h->Reset();

  //Place Bounding Lines around bins representing actual detector
  if( ecap=='B' ){
    //Left Side
    putBox(h, 0, 0, 6, 16);   putLine(h,6,0,6,16); putLine(h,2,16,6,16);                             
    putBox(h, 0, 16, 2, 18);  putLine(h,2,16,2,18); putLine(h,0,18,2,18);//1
    putBox(h, 0, 20, 2, 22);  putLine(h,2,20,2,22); putLine(h,0,20,2,20); putLine(h,0,22,2,22);//3
    putBox(h, 0, 24, 2, 26);  putLine(h,2,24,2,26); putLine(h,0,24,2,24); putLine(h,0,26,2,26);//5
    putBox(h, 0, 28, 2, 30);  putLine(h,2,28,2,30); putLine(h,0,28,2,28); putLine(h,0,30,2,30);//7
    putBox(h, 0, 32, 2, 34);  putLine(h,2,32,2,34); putLine(h,0,32,2,32); putLine(h,0,34,2,34);//9
    putBox(h, 0, 36, 3, 38);  putLine(h,3,36,3,38); putLine(h,0,36,3,36); putLine(h,2,38,3,38);//11M
    putBox(h, 0, 38, 2, 40);  putLine(h,2,38,2,40); putLine(h,0,40,2,40);//11R
    putBox(h, 0, 42, 2, 44);  putLine(h,0,42,2,42); putLine(h,2,42,2,44); putLine(h,0,44,2,44);//13
    putBox(h, 0, 46, 3, 48);  putLine(h,3,46,3,48); putLine(h,0,46,3,46); putLine(h,2,48,3,48);//15M
    putBox(h, 0, 48, 2, 50);  putLine(h,2,48,2,50); putLine(h,0,50,2,50);//15R
    putBox(h, 0, 52, 2, 106); putLine(h,0,52,2,52); putLine(h,0,106,2,106); //putLine(h,2,48,2,102);  
    putBox(h, 0, 108, 1, 110); putLine(h,1,108,1,110); putLine(h,0,108,1,108); putLine(h,0,110,1,110);
    putBox(h, 0, 112, 2, 116); putLine(h,2,112,2,116); putLine(h,0,112,2,112); 

    putLine(h,2,52,2,106);

    //Mid Section
    putBox(h, 8, 0, 9, 106); putLine(h,8,0,8,106); putLine(h,8,106,9,106); putLine(h,9,0,9,106);
    putBox(h, 8, 108, 9, 110); putLine(h,8,108,9,108); putLine(h,8,110,9,110); putLine(h,8,108,8,110); putLine(h,9,108,9,110);
    putBox(h, 8, 112, 9, 116); putLine(h,8,112,9,112); putLine(h,8,112,8,116); putLine(h,9,112,9,116);

    //Right Side
    putBox(h, 11, 0, 17, 16);     putLine(h,11,0,11,16); putLine(h,11,16,15,16);                         
    putBox(h, 15, 16, 17, 18);	  putLine(h,15,16,15,18); putLine(h,15,18,17,18);//1			  
    putBox(h, 15, 20, 17, 22);	  putLine(h,15,20,15,22); putLine(h,15,20,17,20); putLine(h,15,22,17,22); //3
    putBox(h, 15, 24, 17, 26);	  putLine(h,15,24,15,26); putLine(h,15,24,17,24); putLine(h,15,26,17,26); //5
    putBox(h, 15, 28, 17, 30);	  putLine(h,15,28,15,30); putLine(h,15,28,17,28); putLine(h,15,30,17,30); //7
    putBox(h, 15, 32, 17, 34);    putLine(h,15,32,15,34); putLine(h,15,32,17,32); putLine(h,15,34,17,34); //9
    putBox(h, 14, 36, 17, 38);    putLine(h,14,36,14,38); putLine(h,14,36,17,36); putLine(h,14,38,15,38); //11M
    putBox(h, 15, 38, 17, 40);    putLine(h,15,38,15,40); putLine(h,15,40,17,40);//11R
    putBox(h, 15, 42, 17, 44);    putLine(h,15,42,15,44); putLine(h,15,42,17,42); putLine(h,15,44,17,44); //13 
    putBox(h, 14, 46, 17, 48);    putLine(h,14,46,14,48); putLine(h,14,46,17,46); putLine(h,14,48,15,48); //15M
    putBox(h, 15, 48, 17, 50);    putLine(h,15,48,15,50); putLine(h,15,50,17,50);//15R
    putBox(h, 15, 52, 17, 106);	  putLine(h,15,52,17,52); putLine(h,15,106,17,106);	 //putLine(h,2,48,2,102); 
    putBox(h, 16, 108, 17, 110);   putLine(h,16,108,16,110); putLine(h,16,108,17,108); putLine(h,16,110,17,110);
    putBox(h, 15, 112, 17, 116);   putLine(h,15,112,15,116); putLine(h,15,112,17,112); 			 

    putLine(h,15,52,15,106);

    //Draw TickMarks
    for(int i = 0; i < 59; i +=1){
      TLine* l = new TLine(0,i,0.55,i);
      //      l->SetLineColor(kRed);
      h->GetListOfFunctions()->Add(l);
    }
    for(int i = 0; i != 18; ++i){
      TLine* l = new TLine(i,0,i,0.55/17*58);
      //      l->SetLineColor(kRed);
      h->GetListOfFunctions()->Add(l);
    }
    TLine* lx = new TLine(0,0,0,58);
    TLine* ly = new TLine(0,0,17,0);
    lx->SetLineColor(kBlack);
    ly->SetLineColor(kBlack);
    h->GetListOfFunctions()->Add(lx);
    h->GetListOfFunctions()->Add(ly);

  }

  if(ecap=='E'){
    
//     //Bottom: OLD (without new EE chambers)
//     putBox(h, 0, 0, 12, 8); //phi 1-4
//     putBox(h, 0, 8, 5, 10); //phi 5
//     putBox(h, 7, 8, 12, 10); putLine(h, 5, 8, 7, 8); putLine(h, 5, 10, 7, 10); putLine(h, 5, 8, 5, 10); putLine(h, 7, 8, 7, 10);//phi 5
//     putBox(h, 0, 10, 12, 20); // phi 6-10
//     putBox(h, 0, 20, 6, 22); // phi 11 
//     putBox(h, 8, 20, 12, 22); putLine(h, 6, 20, 8, 20); putLine(h, 6, 22, 8, 22); putLine(h, 6, 20, 6, 22); putLine(h, 8, 20, 8, 22); //phi 11
//     putBox(h, 0, 22, 12, 24); // phi 12
//     putBox(h, 0, 24, 4, 26); // phi 13
//     putBox(h, 8, 14, 12, 26); putLine(h, 4, 24, 8, 24); putLine(h, 4, 26, 8, 26); putLine(h, 4, 24, 4, 26); putLine(h, 8, 24, 8, 26); //phi 13
//     putBox(h, 0, 26, 12, 28); //phi 14
//     putBox(h, 0, 28, 4, 30); //phi 15
//     putBox(h, 6, 28, 12, 30); putLine(h, 4, 28, 6, 28); putLine(h, 4, 30, 6, 30); putLine(h, 4, 28, 4, 30); putLine(h, 6, 28, 6, 30);//phi 15
//     putBox(h, 0, 30, 12, 32); //phi 16

    //Bottom: NEW
    putBox(h, 0, 0, 4, 32); putBox(h, 4, 8, 5, 10); 
    putLine(h, 4, 0, 4, 8); putLine(h, 4, 8, 5, 8); putLine(h, 5, 8, 5, 10); putLine(h, 4, 10, 5, 10); putLine(h, 4, 10, 4, 32); //Left side
    putBox(h, 8, 0, 12, 32);  putBox(h, 7, 8, 8, 10); 
    putLine (h, 8, 0, 8, 8); putLine(h, 7, 8, 8, 8); putLine(h, 7, 8, 7, 10); putLine(h, 7, 10, 8, 10);  putLine(h, 8, 10, 8, 32);


    //Left
    putBox(h, 0, 32, 1, 34); putLine(h, 1, 32, 4, 32); putLine(h, 1, 32, 1, 34); //phi 1                          
    putBox(h, 0, 34, 4, 36); putLine(h, 1, 34, 4, 34); putLine(h, 4, 34, 4, 36); putLine(h, 2, 36, 4, 36); //phi 2
    putBox(h, 0, 36, 2, 38); putLine(h, 2, 36, 2, 38); //phi 3
    putBox(h, 0, 38, 4, 40); putLine(h, 2, 38, 4, 38); putLine(h, 4, 38, 4, 40); putLine(h, 2, 40, 4, 40);//phi 4
    putBox(h, 0, 40, 2, 42); putLine(h, 2, 40, 2, 42);//phi 5
    putBox(h, 0, 42, 4, 44); putLine(h, 2, 42, 4, 42); putLine(h, 4, 42, 4, 44); putLine(h, 2, 44, 4, 44);//phi 6
    putBox(h, 0, 44, 2, 46); putLine(h, 2, 44, 2, 46);//phi 7
    putBox(h, 0, 46, 4, 48); putLine(h, 2, 46, 4, 46); putLine(h, 4, 46, 4, 48); putLine(h, 1, 48, 4, 48);//phi 8
    putBox(h, 0, 48, 1, 50); putLine(h, 1, 48, 1, 50);//phi 9
    putBox(h, 0, 50, 4, 52); putLine(h, 1, 50, 4, 50); putLine(h, 4, 50, 4, 52); putLine(h, 2, 52, 4, 52);//phi 10
    putBox(h, 0, 52, 2, 54); putLine(h, 2, 52, 2, 54);//phi 11
    putBox(h, 0, 54, 4, 56); putLine(h, 2, 54, 4, 54); putLine(h, 4, 54, 4, 56); putLine(h, 2, 56, 4, 56);//phi 12
    putBox(h, 0, 56, 2, 58); putLine(h, 2, 56, 2, 58);//phi 13
    putBox(h, 0, 58, 4, 60); putLine(h, 2, 58, 4, 58); putLine(h, 4, 58, 4, 60); putLine(h, 2, 60, 4, 60);//phi 14
    putBox(h, 0, 60, 2, 62); putLine(h, 2, 60, 2, 62);//phi 15
    putBox(h, 0, 62, 4, 64); putLine(h, 2, 62, 4, 62); putLine(h, 4, 62, 4, 64); putLine(h, 1, 64, 4, 64);//phi 16

    putBox(h, 0, 64, 1, 96); putLine(h, 1, 64, 1, 96); putLine(h, 0, 96, 1, 96);

    //Right
    putBox(h, 11, 32, 12, 34); putLine(h, 8, 32, 11, 32); putLine(h, 11, 32, 11, 34); //phi 1                 
    putBox(h, 8, 34, 12, 36);  putLine(h, 8, 34, 11, 34); putLine(h, 8, 34, 8, 36); putLine(h, 10, 36, 8, 36); //phi 2
    putBox(h, 10, 36, 12, 38); putLine(h, 10, 36, 10, 38); //phi 3						
    putBox(h, 8, 38, 12, 40);  putLine(h, 10, 38, 8, 38); putLine(h, 8, 38, 8, 40); putLine(h, 10, 40, 8, 40); //phi 4
    putBox(h, 10, 40, 12, 42); putLine(h, 10, 40, 10, 42);//phi 5	       			       
    putBox(h, 8, 42, 12, 44);  putLine(h, 10, 42, 8, 42); putLine(h, 8, 42, 8, 44); putLine(h, 10, 44, 8, 44); //phi 6
    putBox(h, 10, 44, 12, 46); putLine(h, 10, 44, 10, 46);//phi 7	       			       
    putBox(h, 8, 46, 12, 48);  putLine(h, 10, 46, 8, 46); putLine(h, 8, 46, 8, 48); putLine(h, 11, 48, 8, 48); //phi 8
    putBox(h, 11, 48, 12, 50); putLine(h, 11, 48, 11, 50);//phi 9					       
    putBox(h, 8, 50, 12, 52);  putLine(h, 11, 50, 8, 50); putLine(h, 8, 50, 8, 52); putLine(h, 10, 52, 8, 52); //phi 10
    putBox(h, 10, 52, 12, 54); putLine(h, 10, 52, 10, 54);//phi 11					       
    putBox(h, 8, 54, 12, 56);  putLine(h, 10, 54, 8, 54); putLine(h, 8, 54, 8, 56); putLine(h, 10, 56, 8, 56); //phi 12
    putBox(h, 10, 56, 12, 58); putLine(h, 10, 56, 10, 58);//phi 13					       
    putBox(h, 8, 58, 12, 60);  putLine(h, 10, 58, 8, 58); putLine(h, 8, 58, 8, 60); putLine(h, 10, 60, 8, 60); //phi 14
    putBox(h, 10, 60, 12, 62); putLine(h, 10, 60, 10, 62);//phi 15					       
    putBox(h, 8, 62, 12, 64);  putLine(h, 10, 62, 8, 62); putLine(h, 8, 62, 8, 64); putLine(h, 11, 64, 8, 64); //phi 16
			       	  														       
    putBox(h, 11, 64, 12, 96); putLine(h, 11, 64, 11, 96); putLine(h, 12, 96, 11, 96);				       

    //Draw TickMarks
    for(int i = 0; i < 65; i ++){
      TLine* l = new TLine(0,i,0.55*12/17,i);
      //      l->SetLineColor(kRed);
      h->GetListOfFunctions()->Add(l);
    }
    for(int i = 0; i != 13; ++i){
      TLine* l = new TLine(i,0,i,0.55/17*64);
      //      l->SetLineColor(kRed);
      h->GetListOfFunctions()->Add(l);
    }

    putLine(h, 0, 0, 0, 64);
    putLine(h, 0, 0, 12, 0);

  }


  return StatusCode::SUCCESS;
} 

StatusCode  MdtRawDataValAlg::binMdtRegional( TH2* &h, string &xAxis){
  
  ///SET Ghost Entries
  int LowerEta=-0;
  int UpperEta=0;
  if (xAxis.substr(0,2) == "BO" || xAxis.substr(0,2) == "BI") {//Barrel Hit Mutltiplicities
    LowerEta = -8;
    UpperEta = 8;
  }
  else if(xAxis.substr(0,2) == "BM" || xAxis.substr(0,2) == "EO"){
    LowerEta = -6;
    UpperEta = 6;
  }
  else if (xAxis.substr(0,2) == "EM" || xAxis.substr(0,2) == "EI"){
    LowerEta = -5;
    UpperEta = 5;
  }
  else {
    LowerEta = -2;
    UpperEta = 2;
  }
  
  //Set x-axis labels
  TString eta_s;
  for(int ieta = LowerEta; ieta!= UpperEta+1; ++ieta){
    if(xAxis.substr(2,1)=="A" && ieta<0) continue; //A side goes from 1-...
    if(xAxis.substr(2,1)=="C" && ieta==0) break; //C side goes from ...-1
    if(ieta==0 && xAxis.substr(0,2)=="BO") eta_s = "BOB";
    else if(ieta==0) continue;
    else eta_s = xAxis.substr(0,3);
    eta_s+=returnString(std::abs(ieta));
    h->Fill(eta_s,1,0);
  }

  bool barrelExtra = (xAxis=="BEA"||xAxis=="BEC");
  //Set y-axis labels
  TString phi_s;
  for(int iphi=1; iphi<=16; iphi++) {
    if(iphi<10) phi_s = "0"+returnString(iphi);
    else phi_s = returnString(iphi);
    if(!barrelExtra) {
      //Also uncomment lines in MdtRawDataValAlg.cxx in fillMDTSummaryHistograms() that actually fill these values
      if(xAxis.substr(0,2) == "BI" && (iphi == 11 || iphi ==15) ) {
	h->Fill(eta_s,phi_s+",1,R",0);
	h->Fill(eta_s,phi_s+",2,R",0);
	h->Fill(eta_s,phi_s+",1,M",0);
	h->Fill(eta_s,phi_s+",2,M",0);
      }
      else {
	h->Fill(eta_s,phi_s+",1",0);
	h->Fill(eta_s,phi_s+",2",0);
      }
    }
    else if(iphi%2==0) h->Fill(eta_s,phi_s+",1",0);
  }

  if(xAxis == "BEA" || xAxis == "BEC" || xAxis == "EMA" || xAxis == "EMC" || xAxis == "EOA" || xAxis == "EOC" ){
	  h->GetXaxis()->FindBin(".");
  }
  
  h->LabelsDeflate("X");
  h->LabelsDeflate("Y");
  h->Reset();

  //Make gray boxes
  if(xAxis=="BOA"){
    putBox(h, 0, 0, 1, 44);  putBox(h, 7, 0, 9, 44);
    putBox(h, 0, 48, 1, 52); putBox(h, 8, 48, 9, 52);
    putBox(h, 0, 56, 1, 64); putBox(h, 7, 56, 9, 64);

    putLine(h, 1, 0, 1, 44); putLine(h, 7, 0, 7, 44); putLine(h, 0, 44, 1, 44); putLine(h, 7, 44, 9, 44);
    putLine(h, 1, 48, 1, 52); putLine(h, 8, 48, 8, 52); putLine(h, 0, 48, 1, 48); putLine(h, 8, 48, 9, 48); putLine(h, 0, 52, 1, 52); putLine(h, 8, 52, 9, 52);
    putLine(h, 1, 56, 1, 64); putLine(h, 7, 56, 7, 64); putLine(h, 0, 56, 1, 56); putLine(h, 7, 56, 9, 56 );

  }
  if(xAxis=="BOC"){
    putBox(h, 0, 0, 2, 44);
    putBox(h, 0, 48, 1, 52);
    putBox(h, 0, 56, 2, 64);

    putLine(h, 2, 0, 2, 44); putLine(h, 0, 44, 2, 44);
    putLine(h, 1, 48, 1, 52); putLine(h, 0, 48, 1, 48); putLine(h, 0, 52, 1, 52);
    putLine(h, 2, 56, 2, 64); putLine(h, 0, 56, 2, 56);
  }
  if(xAxis=="BIA") {
    putBox(h, 6, 0, 8, 4); putLine(h, 6, 0, 8, 0); putLine(h, 6, 0, 6, 4); putLine(h, 6, 4, 8, 4); //phi 1
    putBox(h, 7, 6, 8, 8); putLine(h, 7, 6, 8, 6); putLine(h, 7, 6, 7, 8); //phi 2
    putBox(h, 6, 8, 8, 12); putLine(h, 6, 8, 7, 8); putLine(h, 6, 8, 6, 12); putLine(h, 6, 12, 8, 12); //phi 3
    putBox(h, 7, 14, 8, 16); putLine(h, 7, 14, 8, 14); putLine(h, 7, 14, 7, 16); //phi 4
    putBox(h, 6, 16, 8, 20); putLine(h, 6, 16, 7, 16); putLine(h, 6, 16, 6, 20); putLine(h, 6, 20, 8, 20); //phi 5
    putBox(h, 7, 22, 8, 24); putLine(h, 7, 22, 8, 22); putLine(h, 7, 22, 7, 24); //phi 6
    putBox(h, 6, 24, 8, 28); putLine(h, 6, 24, 7, 24); putLine(h, 6, 24, 6, 28); putLine(h, 6, 28, 8, 28); //phi 7
    putBox(h, 7, 30, 8, 32); putLine(h, 7, 30, 8, 30); putLine(h, 7, 30, 7, 32); //phi 8
    putBox(h, 6, 32, 8, 36); putLine(h, 6, 32, 7, 32); putLine(h, 6, 32, 6, 36); putLine(h, 6, 36, 8, 36); //phi 9
    putBox(h, 7, 38, 8, 40); putLine(h, 7, 38, 8, 38); putLine(h, 7, 38, 7, 40); //phi 10
    putBox(h, 6, 40, 8, 44); putLine(h, 6, 40, 7, 40); putLine(h, 6, 40, 6, 44); //putLine(h, 6, 40, 5, 44); //phi 11
    putBox(h, 5, 44, 8, 48); putLine(h, 5, 44, 6, 44); putLine(h, 5, 44, 5, 48); putLine(h, 5, 48, 8, 48); //phi 11 M
    putBox(h, 7, 50, 8, 52); putLine(h, 7, 50, 8, 50); putLine(h, 7, 50, 7, 52); //phi 12
    putBox(h, 6, 52, 8, 56); putLine(h, 6, 52, 7, 52); putLine(h, 6, 52, 6, 56); putLine(h, 6, 56, 8, 56); //phi 13
    putBox(h, 7, 58, 8, 60); putLine(h, 7, 58, 8, 58); putLine(h, 7, 58, 7, 60); //phi 14
    putBox(h, 6, 60, 8, 64); putLine(h, 6, 60, 7, 60); putLine(h, 6, 60, 6, 64); //putLine(h, 6, 60, 5, 64); //phi 15
    putBox(h, 5, 64, 8, 68); putLine(h, 5, 64, 6, 64); putLine(h, 5, 64, 5, 68); putLine(h, 5, 68, 8, 68); //phi 15 M
    putBox(h, 7, 70, 8, 72); putLine(h, 7, 70, 8, 70); putLine(h, 7, 70, 7, 72); putLine(h, 7, 72, 8, 72);// phi 16
  } 
  if(xAxis=="BIC") {
    putBox(h, 2, 0, 0, 4); putLine(h, 2, 0, 0, 0); putLine(h, 2, 0, 2, 4); putLine(h, 2, 4, 0, 4); //phi 1
    putBox(h, 1, 6, 0, 8); putLine(h, 1, 6, 0, 6); putLine(h, 1, 6, 1, 8); //phi 2
    putBox(h, 2, 8, 0, 12); putLine(h, 2, 8, 1, 8); putLine(h, 2, 8, 2, 12); putLine(h, 2, 12, 0, 12); //phi 3
    putBox(h, 1, 14, 0, 16); putLine(h, 1, 14, 0, 14); putLine(h, 1, 14, 1, 16); //phi 4
    putBox(h, 2, 16, 0, 20); putLine(h, 2, 16, 1, 16); putLine(h, 2, 16, 2, 20); putLine(h, 2, 20, 0, 20); //phi 5
    putBox(h, 1, 22, 0, 24); putLine(h, 1, 22, 0, 22); putLine(h, 1, 22, 1, 24); //phi 6
    putBox(h, 2, 24, 0, 28); putLine(h, 2, 24, 1, 24); putLine(h, 2, 24, 2, 28); putLine(h, 2, 28, 0, 28); //phi 7
    putBox(h, 1, 30, 0, 32); putLine(h, 1, 30, 0, 30); putLine(h, 1, 30, 1, 32); //phi 8
    putBox(h, 2, 32, 0, 36); putLine(h, 2, 32, 1, 32); putLine(h, 2, 32, 2, 36); putLine(h, 2, 36, 0, 36); //phi 9
    putBox(h, 1, 38, 0, 40); putLine(h, 1, 38, 0, 38); putLine(h, 1, 38, 1, 40); //phi 10
    putBox(h, 2, 40, 0, 44); putLine(h, 2, 40, 1, 40); putLine(h, 2, 40, 2, 44); //putLine(h, 2, 40, 3, 44); //phi 11
    putBox(h, 3, 44, 0, 48); putLine(h, 3, 44, 2, 44); putLine(h, 3, 44, 3, 48); putLine(h, 3, 48, 0, 48); //phi 11 M
    putBox(h, 1, 50, 0, 52); putLine(h, 1, 50, 0, 50); putLine(h, 1, 50, 1, 52); //phi 12
    putBox(h, 2, 52, 0, 56); putLine(h, 2, 52, 1, 52); putLine(h, 2, 52, 2, 56); putLine(h, 2, 56, 0, 56); //phi 13
    putBox(h, 1, 58, 0, 60); putLine(h, 1, 58, 0, 58); putLine(h, 1, 58, 1, 60); //phi 14
    putBox(h, 2, 60, 0, 64); putLine(h, 2, 60, 1, 60); putLine(h, 2, 60, 2, 64); //putLine(h, 2, 60, 3, 64); //phi 15
    putBox(h, 3, 64, 0, 68); putLine(h, 3, 64, 2, 64); putLine(h, 3, 64, 3, 68); putLine(h, 3, 68, 0, 68); //phi 15 M
    putBox(h, 1, 70, 0, 72); putLine(h, 1, 70, 0, 70); putLine(h, 1, 70, 1, 72); putLine(h, 1, 72, 0, 72);// phi 16
  } 

  if(xAxis=="EIA"){
    putBox(h, 2, 4, 5, 8);  putLine(h, 2, 4, 5, 4); putLine(h, 2, 4, 2, 8); putLine(h, 2, 8, 4, 8); // phi 2
    putBox(h, 4, 8, 5, 12); putLine(h, 4, 8, 4, 12); //phi 3
    putBox(h, 2, 12, 5, 16);  putLine(h, 2, 12, 4, 12); putLine(h, 2, 12, 2, 16); putLine(h, 2, 16, 4, 16); // phi 4
    putBox(h, 4, 16, 5, 20); putLine(h, 4, 16, 4, 20); //phi 5
    putBox(h, 2, 20, 5, 24);  putLine(h, 2, 20, 4, 20); putLine(h, 2, 20, 2, 24); putLine(h, 2, 24, 4, 24); // phi 6
    putBox(h, 4, 24, 5, 28); putLine(h, 4, 24, 4, 28); //phi 7
    putBox(h, 2, 28, 5, 32);  putLine(h, 2, 28, 4, 28); putLine(h, 2, 28, 2, 32); putLine(h, 2, 32, 5, 32); // phi 8
    putBox(h, 2, 36, 5, 40);  putLine(h, 2, 36, 5, 36); putLine(h, 2, 36, 2, 40); putLine(h, 2, 40, 4, 40); // phi 10
    putBox(h, 4, 40, 5, 44); putLine(h, 4, 40, 4, 44); //phi 11
    putBox(h, 2, 44, 5, 48);  putLine(h, 2, 44, 4, 44); putLine(h, 2, 44, 2, 48); putLine(h, 2, 48, 4, 48); // phi 12
    putBox(h, 4, 48, 5, 52); putLine(h, 4, 48, 4, 52); //phi 13
    putBox(h, 2, 52, 5, 56);  putLine(h, 2, 52, 4, 52); putLine(h, 2, 52, 2, 56); putLine(h, 2, 56, 4, 56); // phi 14
    putBox(h, 4, 56, 5, 60); putLine(h, 4, 56, 4, 60); //phi 15
    putBox(h, 2, 60, 5, 64);  putLine(h, 2, 60, 4, 60); putLine(h, 2, 60, 2, 64); putLine(h, 2, 64, 5, 64); // phi 16
  }
  if(xAxis=="EIC"){
    putBox(h, 3, 4, 0, 8);  putLine(h, 3, 4, 0, 4); putLine(h, 3, 4, 3, 8); putLine(h, 3, 8, 1, 8); // phi 2
    putBox(h, 1, 8, 0, 12); putLine(h, 1, 8, 1, 12); //phi 3
    putBox(h, 3, 12, 0, 16);  putLine(h, 3, 12, 1, 12); putLine(h, 3, 12, 3, 16); putLine(h, 3, 16, 1, 16); // phi 4
    putBox(h, 1, 16, 0, 20); putLine(h, 1, 16, 1, 20); //phi 5
    putBox(h, 3, 20, 0, 24);  putLine(h, 3, 20, 1, 20); putLine(h, 3, 20, 3, 24); putLine(h, 3, 24, 1, 24); // phi 6
    putBox(h, 1, 24, 0, 28); putLine(h, 1, 24, 1, 28); //phi 7
    putBox(h, 3, 28, 0, 32);  putLine(h, 3, 28, 1, 28); putLine(h, 3, 28, 3, 32); putLine(h, 3, 32, 0, 32); // phi 8
    putBox(h, 3, 36, 0, 40);  putLine(h, 3, 36, 0, 36); putLine(h, 3, 36, 3, 40); putLine(h, 3, 40, 1, 40); // phi 10
    putBox(h, 1, 40, 0, 44); putLine(h, 1, 40, 1, 44); //phi 11
    putBox(h, 3, 44, 0, 48);  putLine(h, 3, 44, 1, 44); putLine(h, 3, 44, 3, 48); putLine(h, 3, 48, 1, 48); // phi 12
    putBox(h, 1, 48, 0, 52); putLine(h, 1, 48, 1, 52); //phi 13
    putBox(h, 3, 52, 0, 56);  putLine(h, 3, 52, 1, 52); putLine(h, 3, 52, 3, 56); putLine(h, 3, 56, 1, 56); // phi 14
    putBox(h, 1, 56, 0, 60); putLine(h, 1, 56, 1, 60); //phi 15
    putBox(h, 3, 60, 0, 64);  putLine(h, 3, 60, 1, 60); putLine(h, 3, 60, 3, 64); putLine(h, 3, 64, 0, 64); // phi 16
  }

  if(xAxis=="EEA"){
	putBox(h, 1, 16, 2, 20);
	putLine(h, 1, 16, 2, 16);
	putLine(h, 1, 16, 1, 20);
	putLine(h, 1, 20, 2, 20);
  }

  if(xAxis=="EEC"){
    putBox(h, 0, 16, 1, 20);
    putLine(h, 0, 16, 1, 16);
    putLine(h, 1, 16, 1, 20);
    putLine(h, 0, 20, 1, 20);
  }

  if(xAxis=="BEA" || xAxis == "BEC"){
	putBox(h, 2, 0, 3, 16);
	putLine(h, 2, 0, 2, 16);
  }

  if(xAxis == "EMA" || xAxis == "EMC"){
		putBox(h, 5, 0, 6, 64);
		putLine(h, 5, 0, 5, 64);
  }
  
  if( xAxis == "EOA" || xAxis == "EOC"){
		putBox(h, 6, 0, 7, 64);
		putLine(h, 6, 0, 6, 64);
  }
  
  if(xAxis.substr(0,2) == "EE" || xAxis.substr(0,2) == "BI" || xAxis.substr(0,2) == "EI" || xAxis.substr(0,2) == "BM" || xAxis.substr(0,2) == "BO"){
    //Draw TickMarks
    for(int i = 0; i != h->GetNbinsY()+1; i++){
      TLine* l = new TLine(0,i,0.55*h->GetNbinsX()/17,i);
      h->GetListOfFunctions()->Add(l);
    }
    for(int i = 0; i != h->GetNbinsX()+1; ++i){
      TLine* l = new TLine(i,0,i,0.55/17*h->GetNbinsY());
      h->GetListOfFunctions()->Add(l);
    }
    
    TLine* lx = new TLine(0,0,h->GetNbinsX(),0);
    TLine* ly = new TLine(0,0,0,h->GetNbinsY());
    lx->SetLineColor(kBlack);
    ly->SetLineColor(kBlack);
    h->GetListOfFunctions()->Add(lx);
    h->GetListOfFunctions()->Add(ly);    
  }

  return StatusCode::SUCCESS;
}

StatusCode  MdtRawDataValAlg::binMdtGlobal_byLayer( TH2* nHits_In, TH2* nHits_Mid, TH2* nHits_Out){
  for(int iPhi = 1; iPhi != 17; ++iPhi){
    TString phiString = "";
    if(iPhi<10) phiString = "0"+MdtRawDataValAlg::returnString(iPhi);
    else phiString = MdtRawDataValAlg::returnString(iPhi);
    TString phiString_ml1 = phiString+",1";
    TString phiString_ml2 = phiString+",2";
    if(iPhi==11||iPhi==15){
      TString phiString_ml1_BIR = phiString+",1,R";
      TString phiString_ml2_BIR = phiString+",2,R";	
      nHits_In->Fill("EIC5",phiString_ml1_BIR,0.);
      nHits_In->Fill("EIC5",phiString_ml2_BIR,0.);
      TString phiString_ml1_BIM = phiString+",1,M";
      TString phiString_ml2_BIM = phiString+",2,M";	
      nHits_In->Fill("EIC5",phiString_ml1_BIM,0.);
      nHits_In->Fill("EIC5",phiString_ml2_BIM,0.);
    }	
    else{
      nHits_In->Fill("EIC5",phiString_ml1,0.);
      nHits_In->Fill("EIC5",phiString_ml2,0.);
    }
    nHits_Mid->Fill("EMC5",phiString_ml1,0.);
    nHits_Mid->Fill("EMC5",phiString_ml2,0.);
    nHits_Out->Fill("EOC6",phiString_ml1,0.);
    nHits_Out->Fill("EOC6",phiString_ml2,0.);
  }
  for(int iEta = 6; iEta!=-2; --iEta){
    TString etaIn = "EIC"+MdtRawDataValAlg::returnString(iEta);
    TString etaMid = "EMC"+MdtRawDataValAlg::returnString(iEta);
    TString etaOut = "EOC"+MdtRawDataValAlg::returnString(iEta);      
    if(iEta>0){
      if(iEta<6){
	nHits_In->Fill(etaIn,"01,1",0);
	nHits_Mid->Fill(etaMid,"01,1",0);
      }
      nHits_Out->Fill(etaOut,"01,1",0);
    }
    else {
      nHits_In->Fill("","01,1",0);
      nHits_Mid->Fill("","01,1",0);
      nHits_Out->Fill("","01,1",0);
    }
  }
  //Add BIR11/15 separately at a higher eta station
    
  //BEE, EE chambers on inner plots
  for(int iEta = -4; iEta!=1; ++iEta){
    TString etaIn = "";
    if( iEta<-2){
      etaIn = "EEC"+MdtRawDataValAlg::returnString(std::abs(iEta+2));
      nHits_In->Fill(etaIn,"01,1",0);
    }
    else if(iEta<0){
      etaIn = "BEC"+MdtRawDataValAlg::returnString(std::abs(iEta));
      nHits_In->Fill(etaIn,"01,1",0);
    }
    else nHits_In->Fill("  ","01,1",0);
  }

  for(int iEta = -8; iEta!=11; ++iEta){
    TString etaIn = "";
    TString etaMid = "";
    TString etaOut = "";
    if(iEta<0){
      etaIn = "BIC"+MdtRawDataValAlg::returnString(std::abs(iEta));
      etaOut = "BOC"+MdtRawDataValAlg::returnString(std::abs(iEta));
      if(iEta>-7){
	etaMid = "BMC"+MdtRawDataValAlg::returnString(std::abs(iEta));
      }
    }
    else if(iEta==0){
      // 	etaIn = "BIB"+MdtRawDataValAlg::returnString(iEta);
      // 	etaMid = "BMB"+MdtRawDataValAlg::returnString(iEta);
      etaOut = "BOB"+MdtRawDataValAlg::returnString(iEta);
    }
    else if(iEta<9){
      etaIn = "BIA"+MdtRawDataValAlg::returnString(iEta);
      etaOut = "BOA"+MdtRawDataValAlg::returnString(iEta);
      if(iEta<7){
	etaMid = "BMA"+MdtRawDataValAlg::returnString(iEta);
      }
    }
    if(iEta<9){
      nHits_In->Fill(etaIn,"01,1",0);
      nHits_Mid->Fill(etaMid,"01,1",0);
      nHits_Out->Fill(etaOut,"01,1",0);
    }
    else {
      nHits_In->Fill(" ","01,1",0);
      nHits_Mid->Fill(" ","01,1",0);
      nHits_Out->Fill(" ","01,1",0);
    }
  }

  //BEE, EE chambers on inner plots
  for(int iEta = 1; iEta!=6; ++iEta){
    TString etaIn = "";
    if( iEta<3){
      etaIn = "BEA"+MdtRawDataValAlg::returnString(std::abs(iEta));
      nHits_In->Fill(etaIn,"01,1",0);
    }
    else if(iEta<5){
      etaIn = "EEA"+MdtRawDataValAlg::returnString(std::abs(iEta-2));
      nHits_In->Fill(etaIn,"01,1",0);
    }
    else nHits_In->Fill("   ","01,1",0);
  }

  for(int iEta = 1; iEta!=7; ++iEta){
    TString etaIn = "EIA"+MdtRawDataValAlg::returnString(iEta);
    TString etaMid = "EMA"+MdtRawDataValAlg::returnString(iEta);
    TString etaOut = "EOA"+MdtRawDataValAlg::returnString(iEta);      
    if(iEta<6){
      nHits_In->Fill(etaIn,"01,1",0);
      nHits_Mid->Fill(etaMid,"01,1",0);
    }
    nHits_Out->Fill(etaOut,"01,1",0);
  }

  nHits_In->LabelsDeflate("X");
  nHits_In->LabelsDeflate("Y");
  nHits_In->LabelsOption("v","x");
  nHits_In->Reset();
  nHits_Mid->LabelsDeflate("X");
  nHits_Mid->LabelsDeflate("Y");
  nHits_Mid->LabelsOption("v","x");
  nHits_Mid->Reset();
  nHits_Out->LabelsDeflate("X");
  nHits_Out->LabelsDeflate("Y");
  nHits_Out->LabelsOption("v","x");
  nHits_Out->Reset();

  return StatusCode::SUCCESS;

}

void MdtRawDataValAlg::TubeID_to_ID_L_ML(int & tubeID, const std::string & hardware_name, int & derived_tube, int & derived_layer, int & derived_ML, int totalTubes) {
  derived_tube = 1;
  derived_layer = 1;
  derived_ML = 1;

  // Get Number of X
  int derived_NumTubePerLayer = 1;
  int derived_NumTubePerML = 1;
  int derived_NumLayer = 1;
  int derived_NumLayerPerML = 1;
  int derived_NumML = 1;
  if( hardware_name.substr(0,4) == "BIS8" /*&& hardware_name.substr(5,2) == "12"*/ ) {
    derived_NumLayerPerML = 3;
    derived_NumML = 1;
  }
  else if( /*hardware_name.substr(0,4) == "BIS8" ||*/ hardware_name.substr(0,3) == "BEE" ) {
    derived_NumLayerPerML = 4;
    derived_NumML = 1;    
  }
  else if( hardware_name.substr(0,2) == "BI" || hardware_name.substr(0,2) == "EI" /*|| (hardware_name.substr(0,4) == "EEL1" && hardware_name.substr(5,2) == "05")*/ ) {
    derived_NumLayerPerML = 4;
    derived_NumML = 2;
  }
  else {
    derived_NumLayerPerML = 3;
    derived_NumML = 2;
  }

  derived_NumLayer = derived_NumLayerPerML * derived_NumML;
  derived_NumTubePerML = totalTubes / derived_NumML;
  derived_NumTubePerLayer = totalTubes / derived_NumLayer;

  // Corrections for derived_NumTubePerLayer
  if( hardware_name.substr(0,4) == "BMS4" || hardware_name.substr(0,4) == "BMS6" )
    derived_NumTubePerLayer = 48;
  if((hardware_name.substr(5,2) == "11" || hardware_name.substr(5,2) == "15")) {
    if( hardware_name.substr(0,4) == "BIR1" )
      derived_NumTubePerLayer = 30;
    if( hardware_name.substr(0,4) == "BIR2" )
      derived_NumTubePerLayer = 30;
    if( hardware_name.substr(0,4) == "BIR3" )
      derived_NumTubePerLayer = 36;
    if( hardware_name.substr(0,4) == "BIR4" )
      derived_NumTubePerLayer = 30;
    if( hardware_name.substr(0,4) == "BIR5" )
      derived_NumTubePerLayer = 24;
  }
  
  // Now get X
  derived_ML = (tubeID-1)/derived_NumTubePerML + 1;
  derived_layer = (tubeID-1)/derived_NumTubePerLayer + 1;
  derived_tube = tubeID - (derived_layer - 1) * derived_NumTubePerLayer;
  if(derived_ML==2) derived_layer -= derived_NumLayerPerML;
}

//Correct for CutOuts
void MdtRawDataValAlg::ChamberTubeNumberCorrection(int & tubeNum, const std::string & hardware_name, int tubePos, int numLayers) {
  //numLayers should be mdt_layer-1 so numLayers = 0 implies layer 1 ML 1 or mdt_layer==1
  if(hardware_name.substr(0,4)=="BMS4" || hardware_name.substr(0,4)=="BMS6"){//layer 1-4 tubeId 41-48 cut out
    if( numLayers <= 2 ) tubeNum = tubePos + numLayers * 48;
  }
  if(hardware_name.substr(0,3)=="BIR" && numLayers <= 3 ){
    if(hardware_name.substr(5,2)=="11"||hardware_name.substr(5,2)=="15") {
      if(hardware_name.substr(3,1)=="1") tubeNum = tubePos + 6 + numLayers*30;//layer 1-4 tube id 1-6 cut out
      if(hardware_name.substr(3,1)=="2") tubeNum = tubePos + numLayers*30;//layer 1-4 tube id 28-30 cut out
      if(hardware_name.substr(3,1)=="4") tubeNum = tubePos + 3 + numLayers*30;//layer 1-4 tube id 1-3 cut out
      if(hardware_name.substr(3,1)=="5") tubeNum = tubePos + numLayers*24;//layer 1-4 tube id 22-24 cut out
    }
  }
  if( hardware_name.substr(0,3)=="BIR" && hardware_name.substr(3,1)=="3") tubeNum = tubePos + numLayers*36; //cut out on both ML
  if( hardware_name == "EEL1A05" || hardware_name == "EEL1C05" ) tubeNum = tubePos + numLayers*48; //mdtIdHelper gives wrong #tubes/layer (incidentally also wrong #layers)
}

//Correct for F@#!ing mdtIdHelper
void MdtRawDataValAlg::CorrectTubeMax(const std::string & hardware_name, int & numTubes) {
  if( hardware_name == "EEL1A05" || hardware_name == "EEL1C05" ) numTubes = 48;
}

//Correct for F@#!ing mdtIdHelper
void MdtRawDataValAlg::CorrectLayerMax(const std::string & hardware_name, int & numLayers) {
  if( hardware_name == "EEL1A05" || hardware_name == "EEL1C05" ) numLayers = 3;
}

StatusCode MdtRawDataValAlg::bookMDTHisto_overview( TH1*& h, TString h_title, TString xtit, TString ytit, int nbin, float xi, float xf, MonGroup& mgrp) {
  h = new TH1F(h_title, h_title, nbin, xi, xf);  
  h->SetFillColor(42);
  h->SetTitleSize(0.3, "y");
  h->GetXaxis()->SetTitle(xtit);
  h->GetYaxis()->SetTitle(ytit);
  ATH_MSG_DEBUG("INSIDE bookMDTHisto: " << h << " " << h_title );
  StatusCode sc = mgrp.regHist( h );
  if(sc.isFailure()) ATH_MSG_WARNING("couldn't register " << h_title << " hist to MonGroup" );
  return sc;
}
    
StatusCode MdtRawDataValAlg::bookMDTHisto_chambers( TH1F_LW*& h, TString h_title, TString xtit, TString ytit, int nbin, float xi, float xf, MonGroup& mgrp) {
  h = TH1F_LW::create(h_title, h_title, nbin, xi, xf);  
  h->GetXaxis()->SetTitle(xtit);
  h->GetYaxis()->SetTitle(ytit);
  ATH_MSG_VERBOSE("INSIDE bookMDTHisto: " << h << " " << h_title );
  StatusCode sc = mgrp.regHist( h );
  if(sc.isFailure()) ATH_MSG_WARNING("couldn't register " << h_title << " hist to MonGroup" );  
  return sc;
}

StatusCode MdtRawDataValAlg::bookMDTHisto_overview_2D( TH2*& h, TString h_title, TString xtit, TString ytit, int nbin1, float xi1, float xf1, int nbin2, float xi2, float xf2, MonGroup& mgrp) {
  h = new TH2F(h_title, h_title, nbin1, xi1, xf1, nbin2, xi2, xf2);  
  h->SetOption("COLZ");  
  h->SetMarkerColor(1);  
  h->SetMarkerStyle(21);   
  h->SetMarkerSize(0.2);
  h->GetXaxis()->SetTitle(xtit);
  h->GetYaxis()->SetTitle(ytit);
  ATH_MSG_DEBUG("INSIDE bookMDTHisto: " << h << " " << h_title );
  StatusCode sc = mgrp.regHist( h );
  if(sc.isFailure()) ATH_MSG_WARNING("couldn't register " << h_title << " hist to MonGroup" );
  return sc;
}

StatusCode MdtRawDataValAlg::bookMDTHisto_chambers_2D( TH2F_LW*& h, TString h_title, TString xtit, TString ytit, int nbin1, float xi1, float xf1, int nbin2, float xi2, float xf2, MonGroup& mgrp) {
  h = TH2F_LW::create(h_title, h_title, nbin1, xi1, xf1, nbin2, xi2, xf2);  
  h->SetOption("COLZ");  
  //  h->SetFillColor(42);
  h->SetMarkerColor(1);  
  h->SetMarkerStyle(21);   
  h->SetMarkerSize(0.2);
  h->GetXaxis()->SetTitle(xtit);
  h->GetYaxis()->SetTitle(ytit);
  ATH_MSG_VERBOSE("INSIDE bookMDTHisto: " << h << " " << h_title );
  StatusCode sc = mgrp.regHist( h );
  if(sc.isFailure()) ATH_MSG_WARNING("couldn't register " << h_title << " hist to MonGroup" ); 
  return sc;
}

StatusCode MdtRawDataValAlg::bookMDTHisto_OccVsLB( TH2*& h, TString h_title, TString xtit, TString ytit, int nbin1, float xi1, float xf1, int nbin2, float xi2, float xf2, MonGroup& mgrp){
  h = new TH2F(h_title, h_title, nbin1, xi1, xf1, nbin2, xi2, xf2);  
  h->SetOption("COLZ");  
  h->SetMarkerColor(1);  
  h->SetMarkerStyle(21);   
  h->SetMarkerSize(0.2);
  h->GetXaxis()->SetTitle(xtit);
  h->GetYaxis()->SetTitle(ytit);
  ATH_MSG_DEBUG("INSIDE bookMDTHisto_OccVsLB: " << h << " " << h_title );
  StatusCode sc = mgrp.regHist( h );
  if(sc.isFailure()) ATH_MSG_WARNING("couldn't register " << h_title << " hist to MonGroup" );
  return sc;
}

StatusCode MdtRawDataValAlg::fillMDTMaskedTubes(IdentifierHash idHash, const std::string &hardware_name, TH1F_LW*& h){
  //Loop Over all mdt identifiers
  if(!m_masked_tubes) {
    ATH_MSG_WARNING("Could Not Fill masked tubes noise map not set!");
    return StatusCode::SUCCESS;
  }
  
  std::set<Identifier> noisyTubes = m_masked_tubes->getNoiseList(idHash);
  for(std::set<Identifier>::const_iterator itr = noisyTubes.begin(); itr != noisyTubes.end(); ++itr){
    Identifier digcoll_id = *itr;
    int mdtlayer = m_idHelperSvc->mdtIdHelper().tubeLayer(digcoll_id);
    if (m_idHelperSvc->mdtIdHelper().multilayer(digcoll_id)==2) {
      if ( hardware_name.at(1) == 'I' && hardware_name.at(3) != '8' )
	mdtlayer += 4;
      else 
	mdtlayer += 3;
    }	  
    int mdttube= m_idHelperSvc->mdtIdHelper().tube(digcoll_id) + (mdtlayer-1) * m_idHelperSvc->mdtIdHelper().tubeMax(digcoll_id);
    ChamberTubeNumberCorrection(mdttube, hardware_name, m_idHelperSvc->mdtIdHelper().tube(digcoll_id), mdtlayer-1);
    h->Fill(mdttube, 1);
  }
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// Private function to select mdt chambersId of the spectrometer

void MdtRawDataValAlg::mdtchamberId() 
{
  ATH_MSG_DEBUG("in MDT ChambersIDvector" );  

  std::vector<Identifier>::const_iterator  idfirst = m_idHelperSvc->mdtIdHelper().module_begin();
  std::vector<Identifier>::const_iterator  idlast =  m_idHelperSvc->mdtIdHelper().module_end();

  IdContext mdtModuleContext = m_idHelperSvc->mdtIdHelper().module_context();
  Identifier Id;
  IdentifierHash Idhash;
  for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)
    {    
      Id=*i;
      int gethash_code = m_idHelperSvc->mdtIdHelper().get_hash(Id, Idhash, &mdtModuleContext); 
      m_chambersId.push_back(Id);
      m_chambersIdHash.push_back(Idhash);

      std::string extid = m_idHelperSvc->mdtIdHelper().show_to_string(Id);
      ATH_MSG_DEBUG("Adding the chamber Identifier: " << extid );
      if (gethash_code == 0) {
	ATH_MSG_DEBUG(" its hash Id is " << Idhash );
      }
      else{
	ATH_MSG_DEBUG("  hash Id NOT computed "<< Idhash );
      }
    } 

  return;
}  

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
int MdtRawDataValAlg::mezzmdt(Identifier digcoll_id) { //int mezz_chamber, int mezz_eta, int mezz_ml, int mezz_tube, int max_tube) {
  int TotmezzTubes = 8;
  if( m_idHelperSvc->mdtIdHelper().tubeLayerMax(digcoll_id) == 4 ) 
    TotmezzTubes = 6;
  int Imezz = (int)((m_idHelperSvc->mdtIdHelper().tube(digcoll_id)-1)/TotmezzTubes) + (int)((m_idHelperSvc->mdtIdHelper().multilayer(digcoll_id)-1)*((m_idHelperSvc->mdtIdHelper().tubeMax(digcoll_id))/TotmezzTubes));

  return Imezz;  
}

// Get the Maximum # of tubes in the chamber
// the 'if' statements are for chambers with ML1 != ML2
// except for BIS8 -- mdtIdHelper gets the # layers wrong in this instance
int MdtRawDataValAlg::GetTubeMax( const Identifier & digcoll_id, const std::string & hardware_name ) {
  int numtubes = m_idHelperSvc->mdtIdHelper().tubeMax(digcoll_id);
  int numlayers = m_idHelperSvc->mdtIdHelper().tubeLayerMax(digcoll_id);
  int numML = m_idHelperSvc->mdtIdHelper().numberOfMultilayers(digcoll_id);
  int tubeMax = numtubes * numlayers * numML;

  if( hardware_name.substr(0,4) == "BIS8" ) // Why does mdtIdHelper get this one wrong?
    tubeMax = 16*3;
  if( hardware_name.substr(0,4) == "BIR5" )
    //     tubeMax = 21*4 + 24*4;
    tubeMax = 24*4 + 24*4;
  if( hardware_name.substr(0,4) == "BIR2" || hardware_name.substr(0,4) == "BIR4" )
    //     tubeMax = 27*4 + 30*4;
    tubeMax = 30*4 + 30*4;
  if( hardware_name.substr(0,4) == "BIR3" )
    tubeMax = 36*4 + 36*4;
  if( hardware_name.substr(0,4) == "BIR1" )
    //     tubeMax = 24*4 + 30*4;
    tubeMax = 30*4 + 30*4;
  if( hardware_name.substr(0,4) == "BMS4" || hardware_name.substr(0,4) == "BMS6" )
    //     tubeMax = 40*3 + 48*3;
    tubeMax = 48*3 + 48*3;
  if( hardware_name == "EEL1A05" || hardware_name == "EEL1C05" )
    tubeMax = 48*3 + 48*3;
  if( hardware_name.substr(0,3) == "BME")
	  tubeMax = 546;
  
  
  std::map<string,float>::iterator iter_tubesperchamber = m_tubesperchamber_map.find(hardware_name);
  if ( iter_tubesperchamber == m_tubesperchamber_map.end() ) { 
      m_tubesperchamber_map.insert( make_pair( hardware_name, tubeMax ) );
      ATH_MSG_DEBUG("Chamber " << hardware_name << " has " << tubeMax << " tubes.");
  } 
  else {
      ATH_MSG_WARNING("GetTubeMax: computing tubes per chamber twice for this chamber!");
  }     
  
  return tubeMax;
  
  
  
} 

bool MdtRawDataValAlg::AinB( int A, std::vector<int> & B ) {
  for(unsigned int i=0; i<B.size(); ++i) {
    if( B.at(i) == A )
      return true;
  }
  return false;
}

StatusCode MdtRawDataValAlg::GetTimingInfo() {
  m_time = -1;

  SG::ReadHandle<xAOD::EventInfo> evt(m_eventInfo);
  m_time = (1000000000L*(uint64_t)evt->timeStamp()+evt->timeStampNSOffset()) * 0.000000001;
   
  // protection against simulated cosmics when the trigger_info() of the event_info is not filled and returns a null pointer. 
  //trigtype = eventInfo->level1TriggerType();

  return StatusCode::SUCCESS;
} 

void MdtRawDataValAlg::StoreTriggerType(int type) {
  m_trigtype = type;
  if( m_trigtype == L1_BARREL )
    m_trig_BARREL = true;
  if( m_trigtype == L1_ENDCAP )
    m_trig_ENDCAP = true;
}

void MdtRawDataValAlg::setIsATLASReady(){
  bool filterresult(true);
  if (! m_DQFilterTools.empty()) {
    ToolHandleArray<IDQFilterTool>::const_iterator ifilter(m_DQFilterTools.begin()), filterend(m_DQFilterTools.end());
    for (; filterresult && (ifilter != filterend);
	 ++ifilter) {
      filterresult = (filterresult && (*ifilter)->accept());
      }
  }
  m_atlas_ready = filterresult;
}

StatusCode MdtRawDataValAlg::fillLumiBlock(){

  m_lumiblock = -1;

  SG::ReadHandle<xAOD::EventInfo> evt(m_eventInfo);

  m_lumiblock = evt->lumiBlock();

  return StatusCode::SUCCESS;

}
StatusCode MdtRawDataValAlg::GetEventNum(){

  m_eventNum = -1;

  SG::ReadHandle<xAOD::EventInfo> evt(m_eventInfo);

  m_eventNum = evt->eventNumber();

  return StatusCode::SUCCESS;

}

void MdtRawDataValAlg::putBox(TH2* h, float x1, float y1, float x2, float y2){
    TBox* box = new TBox(x1, y1*0.5, x2, y2*0.5);
//     box->SetFillColor(kGray+1);
//     box->SetLineColor(kGray+1);
//     box->SetFillStyle(3013);
     //    box->SetFillStyle(3004);

    box->SetFillColor(kGray);
    box->SetLineColor(kGray);

    //    box->SetLineStyle(3);

    (h->GetListOfFunctions())->Add(box);    
    //    //    delete box; // will cause crash or not save box // is deleted along with h
}

void MdtRawDataValAlg::putLine(TH2* h, float x1, float y1, float x2, float y2, Color_t c){
    TLine* line = new TLine(x1, y1*0.5, x2, y2*0.5);
    line->SetLineColor(c);

    (h->GetListOfFunctions())->Add(line);    
    //    //    delete line; // will cause crash or not save box // is deleted along with h
}

std::string MdtRawDataValAlg::getChamberName(const Muon::MdtPrepData* hit){
  return getChamberName( hit->identify() );
}

std::string MdtRawDataValAlg::getChamberName(Identifier id){
  if(m_hist_hash_list) {
    IdentifierHash idHash;
    m_idHelperSvc->mdtIdHelper().get_module_hash(id, idHash);
    if( idHash < m_hist_hash_list->size() ) {
      MDTChamber* chamber = (*m_hist_hash_list)[idHash];
      if(chamber) return chamber->getName();
      else return convertChamberName(m_idHelperSvc->mdtIdHelper().stationName(id),m_idHelperSvc->mdtIdHelper().stationEta(id),m_idHelperSvc->mdtIdHelper().stationPhi(id),"MDT");    
    }
  }
  return convertChamberName(m_idHelperSvc->mdtIdHelper().stationName(id),m_idHelperSvc->mdtIdHelper().stationEta(id),m_idHelperSvc->mdtIdHelper().stationPhi(id),"MDT");
}

StatusCode MdtRawDataValAlg::getChamber(IdentifierHash id, MDTChamber* &chamber){
  if( id >= m_hist_hash_list->size() ) return StatusCode::FAILURE;

  chamber = (*m_hist_hash_list)[id];
  if( chamber == 0 ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

void MdtRawDataValAlg::clear_hist_map(bool reallocate){
  if(m_hist_hash_list) {
    std::vector<MDTChamber*>::const_iterator itr = m_hist_hash_list->begin();
    std::vector<MDTChamber*>::const_iterator itr_end = m_hist_hash_list->end();
    for( ; itr != itr_end; ++itr){
      delete (*itr);
    }
    delete m_hist_hash_list;
    m_hist_hash_list = 0;
  }
  //  if(reallocate) m_hist_map = new std::map<IdentifierHash, MDTChamber*>;
  if(reallocate) {
    m_hist_hash_list = new std::vector<MDTChamber*>;
    //reserve enough space for all needed chambers
    unsigned int nChambers = 1200;
    m_hist_hash_list->reserve(nChambers);
    //Ensure size is 1200 and that all reservec quantities are 0!
    for(unsigned int i = 0; i != 1200; ++i) m_hist_hash_list->push_back(0);
  }
}

int MdtRawDataValAlg::get_bin_for_LB_hist(int region, int layer, int phi, int eta, bool isBIM){
  if(region == 0 || region == 1){ //Barrel

    if(layer == 0){ //Inner
      if(eta < 6){
	if(phi < 11 && !isBIM) return 18*(eta-1) + phi;
	else if((phi == 10 && isBIM) || (phi < 15 && !isBIM)) return 18*(eta-1) + phi + 1;
	else return 18*(eta-1) + phi + 2;
      }
      else if(eta == 6) return 90 + phi;
      else if(eta == 7) return 106 + (phi/2);
      else if(eta == 8) return 114 + (phi/2);
    }

    else if(layer == 1){ //Middle
      // 95 = 1 + 16 + 16 + 16 + 16 + 16 + 14   total number of phi sectors (+1)
      // in the last eta-sector (6) there is no phi-sector 13; ie there are no chambers BML6A13 and BML6C13, so there are only 14 phi sectos
      // we move the bin of phi=14 directly above phi=12 so there is no white line in the histogram
      if(eta == 6 && phi > 11 ) return 16*(eta-1) + phi - 1;
      else return 16*(eta-1) + phi;

    }
    else if(layer == 2 && region == 0){ // Outer, side A (must be separated due to presence of eta=0 chambers)
      if(eta == 0 && phi == 11) return 0;
      else if(eta == 0 && phi == 13) return 1;
      else if(eta < 7) return 16*(eta-1) + phi + 2;
      else if(eta == 7 && phi == 11) return 98;
      else if(eta == 7 && phi == 13) return 99;
      else if(eta == 8 && phi == 11) return 100;
      else if(eta == 8 && phi == 13) return 101;
    }
    else if(layer == 3 && region == 0){ // Extra: put with outer
      return 102 + 8*(eta-1) + (phi-1)/2;
    }
    else if(layer == 2 && region == 1){ // Outer, side C
      if(eta < 7) return 16*(eta-1) + phi ;
      else if(eta == 7 && phi == 11) return 96;
      else if(eta == 7 && phi == 13) return 97;
      else if(eta == 8 && phi == 11) return 98;
      else if(eta == 8 && phi == 13) return 99;
    }
    else if(layer == 3 && region == 1){ // Extra: put with outer
      return 100 + 8*(eta-1) + (phi-1)/2;
    }

  }

  else{ //Endcap
    if(layer == 0){ //Inner
      if(eta < 3) return 16*(eta-1) + phi;
      else if(eta == 3) return 32 + phi/2;
      else if(eta == 4) return 40 + phi/2;
      else if(eta == 5) return 48 + phi/8;
    }
    else if(layer == 1){ //Middle
      return 16*(eta-1) + phi;
    }
    else if(layer == 2){ //Outer
      return 16*(eta-1) + phi;
    }
    else if(layer == 3 && region == 2){ //Extra A: put with outer. Asymmetry between A and C
      if(eta == 1 && phi == 2) return 96;
      if(eta == 1 && phi == 4) return 97;
      if(eta == 1 && phi == 10) return 98; 
      if(eta == 1 && phi == 12) return 99;
      if(eta == 2 && phi == 2) return 100;
      if(eta == 2 && phi == 10) return 101;
      if(eta == 2 && phi == 12) return 102;
    }
    else if (layer == 3 && region == 3){ //Extra C: put with outer. 
      if(eta == 1 || phi < 5) return 80 + 16*eta + phi;
      else return 79 + 16*eta + phi; //(missing eta = 2, phi = 5)
    }

  }

  return -1;
  
}

StatusCode MdtRawDataValAlg::binMdtOccVsLB(TH2* &h, int region, int layer){
  if(region == 0 || region == 1){//Barrel
    if(layer == 0){ //Inner
      //Add Labels
      h->SetBins(834,1,2502,122,0,122);
      h->GetYaxis()->SetBinLabel(1,"BI1");
      h->GetYaxis()->SetBinLabel(19,"BI2");
      h->GetYaxis()->SetBinLabel(37,"BI3");
      h->GetYaxis()->SetBinLabel(55,"BI4");
      h->GetYaxis()->SetBinLabel(73,"BI5");
      h->GetYaxis()->SetBinLabel(91,"BI6");
      h->GetYaxis()->SetBinLabel(107,"BI7");
      h->GetYaxis()->SetBinLabel(115,"BI8");
      //Add lines
      h->GetYaxis()->SetTickLength(0);
      for(int i = 0; i < 6; i++){
	TLine* l = new TLine(1,18*i,50,18*i);
	h->GetListOfFunctions()->Add(l);
      }
      TLine* l2 = new TLine(1,106,50,106);
      h->GetListOfFunctions()->Add(l2);
      TLine* l3 = new TLine(1,114,50,114);
      h->GetListOfFunctions()->Add(l3);
      for(int i = 1; i < 122; i++){
	if(i < 90 && (i%18==11 || i%18==16)){
	  TLine* l = new TLine(1,i,10,i);
	  h->GetListOfFunctions()->Add(l);
	}
	else {
	  TLine* l = new TLine(1,i,20,i);
	  h->GetListOfFunctions()->Add(l);
	}
      }
    }

    else if(layer == 1){ //Middle
      h->SetBins(834,1,2502,95,0,95);   // 95 = 1 + 16 + 16 + 16 + 16 + 16 + 14   total number of phi sectors (+1)
                                        // in the last eta-sector (6) there is no phi-sector 13; ie there arent chambers BML6A13 and BML6C13
                                        // so there are only 14 phi sectors
      //Add Labels
      h->GetYaxis()->SetBinLabel(1,"BM1");
      h->GetYaxis()->SetBinLabel(17,"BM2");
      h->GetYaxis()->SetBinLabel(33,"BM3");
      h->GetYaxis()->SetBinLabel(49,"BM4");
      h->GetYaxis()->SetBinLabel(65,"BM5");
      h->GetYaxis()->SetBinLabel(81,"BM6");
      //Add lines
      h->GetYaxis()->SetTickLength(0);
      for(int i = 1; i < 95; i++){
	TLine* l = new TLine(1,i,20,i);
	h->GetListOfFunctions()->Add(l);
      }
      TLine* l1 = new TLine(1,16,50,16); h->GetListOfFunctions()->Add(l1);
      TLine* l2 = new TLine(1,32,50,32); h->GetListOfFunctions()->Add(l2);
      TLine* l3 = new TLine(1,48,50,48); h->GetListOfFunctions()->Add(l3);
      TLine* l4 = new TLine(1,64,50,64); h->GetListOfFunctions()->Add(l4);
      TLine* l5 = new TLine(1,80,50,80); h->GetListOfFunctions()->Add(l5);
    }

    else if(layer == 2 && region == 0){ //Outer, side A 
      h->SetBins(834,1,2502,118,0,118);
      //Add labels
      h->GetYaxis()->SetBinLabel(1,"BO0");
      h->GetYaxis()->SetBinLabel(4,"BO1");
      h->GetYaxis()->SetBinLabel(19,"BO2");
      h->GetYaxis()->SetBinLabel(35,"BO3");
      h->GetYaxis()->SetBinLabel(51,"BO4");
      h->GetYaxis()->SetBinLabel(67,"BO5");
      h->GetYaxis()->SetBinLabel(83,"BO6");
      h->GetYaxis()->SetBinLabel(99,"BO7,8");
      // h->GetYaxis()->SetBinLabel(101,"BO8");
      h->GetYaxis()->SetBinLabel(103,"BE1");
      h->GetYaxis()->SetBinLabel(111,"BE2");
      //Add lines
      h->GetYaxis()->SetTickLength(0);
      for(int i = 1; i < 118; i++){
	TLine* l = new TLine(1,i,20,i);
	h->GetListOfFunctions()->Add(l);
      }
      TLine* l1 = new TLine(1,2,50,2); h->GetListOfFunctions()->Add(l1);
      TLine* l2 = new TLine(1,18,50,18); h->GetListOfFunctions()->Add(l2);
      TLine* l3 = new TLine(1,34,50,34); h->GetListOfFunctions()->Add(l3);
      TLine* l4 = new TLine(1,50,50,50); h->GetListOfFunctions()->Add(l4);
      TLine* l5 = new TLine(1,66,50,66); h->GetListOfFunctions()->Add(l5);
      TLine* l6 = new TLine(1,82,50,82); h->GetListOfFunctions()->Add(l6);
      TLine* l7 = new TLine(1,98,50,98); h->GetListOfFunctions()->Add(l7);
      TLine* l8 = new TLine(1,100,50,100); h->GetListOfFunctions()->Add(l8);
      TLine* l9 = new TLine(1,102,50,102); h->GetListOfFunctions()->Add(l9);
      TLine* l10 = new TLine(1,110,50,110); h->GetListOfFunctions()->Add(l10);
    }

    else if(layer == 2 && region == 1){ //Outer, side C (no eta = 0)
      h->SetBins(834,1,2502,116,0,116);
      //Add labels
      h->GetYaxis()->SetBinLabel(1,"BO1");
      h->GetYaxis()->SetBinLabel(17,"BO2");
      h->GetYaxis()->SetBinLabel(33,"BO3");
      h->GetYaxis()->SetBinLabel(49,"BO4");
      h->GetYaxis()->SetBinLabel(65,"BO5");
      h->GetYaxis()->SetBinLabel(81,"BO6");
      h->GetYaxis()->SetBinLabel(97,"BO7,8");
      //      h->GetYaxis()->SetBinLabel(101,"BO8");
      h->GetYaxis()->SetBinLabel(101,"BE1");
      h->GetYaxis()->SetBinLabel(109,"BE2");
      //Add lines
      h->GetYaxis()->SetTickLength(0);
      for(int i = 1; i < 116; i++){
	TLine* l = new TLine(1,i,20,i);
	h->GetListOfFunctions()->Add(l);
      }
      TLine* l1 = new TLine(1,16,50,16); h->GetListOfFunctions()->Add(l1);
      TLine* l2 = new TLine(1,32,50,32); h->GetListOfFunctions()->Add(l2);
      TLine* l3 = new TLine(1,48,50,48); h->GetListOfFunctions()->Add(l3);
      TLine* l4 = new TLine(1,64,50,64); h->GetListOfFunctions()->Add(l4);
      TLine* l5 = new TLine(1,80,50,80); h->GetListOfFunctions()->Add(l5);
      TLine* l6 = new TLine(1,96,50,96); h->GetListOfFunctions()->Add(l6);
      TLine* l7 = new TLine(1,98,50,98); h->GetListOfFunctions()->Add(l7);
      TLine* l8 = new TLine(1,100,50,100); h->GetListOfFunctions()->Add(l8);
      TLine* l9 = new TLine(1,108,50,108); h->GetListOfFunctions()->Add(l9);
    }
  }

  else{ //Endcap
    if(layer == 0){ //Inner
      h->SetBins(834,1,2502,50,0,50);
      //Add labels
      h->GetYaxis()->SetBinLabel(1,"EI1");
      h->GetYaxis()->SetBinLabel(17,"EI2");
      h->GetYaxis()->SetBinLabel(33,"EI3");
      h->GetYaxis()->SetBinLabel(41,"EI4");
      h->GetYaxis()->SetBinLabel(49,"EI5");
      //Add lines
      h->GetYaxis()->SetTickLength(0);
      for(int i = 1; i < 50; i++){
	TLine* l = new TLine(1,i,20,i);
	h->GetListOfFunctions()->Add(l);
      }
      TLine* l1 = new TLine(1,16,50,16); h->GetListOfFunctions()->Add(l1);
      TLine* l2 = new TLine(1,32,50,32); h->GetListOfFunctions()->Add(l2);
      TLine* l3 = new TLine(1,40,50,40); h->GetListOfFunctions()->Add(l3);
      TLine* l4 = new TLine(1,48,50,48); h->GetListOfFunctions()->Add(l4);
    }

    else if(layer == 1){ //Middle
      h->SetBins(834,1,2502,80,0,80);
      //Add labels 
      h->GetYaxis()->SetBinLabel(1,"EM1");
      h->GetYaxis()->SetBinLabel(17,"EM2");
      h->GetYaxis()->SetBinLabel(33,"EM3");
      h->GetYaxis()->SetBinLabel(49,"EM4");
      h->GetYaxis()->SetBinLabel(65,"EM5");
      //Add lines
      h->GetYaxis()->SetTickLength(0);
      for(int i = 1; i < 80; i++){
	TLine* l = new TLine(1,i,20,i);
	h->GetListOfFunctions()->Add(l);
      }
      TLine* l1 = new TLine(1,16,50,16); h->GetListOfFunctions()->Add(l1);
      TLine* l2 = new TLine(1,32,50,32); h->GetListOfFunctions()->Add(l2);
      TLine* l3 = new TLine(1,48,50,48); h->GetListOfFunctions()->Add(l3);
      TLine* l4 = new TLine(1,64,50,64); h->GetListOfFunctions()->Add(l4);
    }

    else if(layer == 2){ //Outer + extra

      if(region == 2){ //side A
	h->SetBins(834,1,2502,103,0,103);
	//Add labels
	h->GetYaxis()->SetBinLabel(1,"EO1");
	h->GetYaxis()->SetBinLabel(17,"EO2");
	h->GetYaxis()->SetBinLabel(33,"EO3");
	h->GetYaxis()->SetBinLabel(49,"EO4");
	h->GetYaxis()->SetBinLabel(65,"EO5");
	h->GetYaxis()->SetBinLabel(81,"EO6");
	h->GetYaxis()->SetBinLabel(97,"EE1,2");
	//Add lines
	h->GetYaxis()->SetTickLength(0);
	for(int i = 1; i < 103; i++){
	  TLine* l = new TLine(1,i,20,i);
	  h->GetListOfFunctions()->Add(l);
	}
	TLine* l1 = new TLine(1,16,50,16); h->GetListOfFunctions()->Add(l1);
	TLine* l2 = new TLine(1,32,50,32); h->GetListOfFunctions()->Add(l2);
	TLine* l3 = new TLine(1,48,50,48); h->GetListOfFunctions()->Add(l3);
	TLine* l4 = new TLine(1,64,50,64); h->GetListOfFunctions()->Add(l4);
	TLine* l5 = new TLine(1,80,50,80); h->GetListOfFunctions()->Add(l5);
	TLine* l6 = new TLine(1,96,50,96); h->GetListOfFunctions()->Add(l6);
	TLine* l7 = new TLine(1,100,50,100); h->GetListOfFunctions()->Add(l7);
      }
      else if(region == 3){ //side C
	h->SetBins(834,1,2502,127,0,127);
	//Add labels
	h->GetYaxis()->SetBinLabel(1,"EO1");
	h->GetYaxis()->SetBinLabel(17,"EO2");
	h->GetYaxis()->SetBinLabel(33,"EO3");
	h->GetYaxis()->SetBinLabel(49,"EO4");
	h->GetYaxis()->SetBinLabel(65,"EO5");
	h->GetYaxis()->SetBinLabel(81,"EO6");
	h->GetYaxis()->SetBinLabel(97,"EE1");
	h->GetYaxis()->SetBinLabel(113,"EE2");      
	//Add lines
	h->GetYaxis()->SetTickLength(0);
	for(int i = 1; i < 127; i++){
	  TLine* l = new TLine(1,i,20,i);
	  h->GetListOfFunctions()->Add(l);
	}
	TLine* l1 = new TLine(1,16,50,16); h->GetListOfFunctions()->Add(l1);
	TLine* l2 = new TLine(1,32,50,32); h->GetListOfFunctions()->Add(l2);
	TLine* l3 = new TLine(1,48,50,48); h->GetListOfFunctions()->Add(l3);
	TLine* l4 = new TLine(1,64,50,64); h->GetListOfFunctions()->Add(l4);
	TLine* l5 = new TLine(1,80,50,80); h->GetListOfFunctions()->Add(l5);
	TLine* l6 = new TLine(1,96,50,96); h->GetListOfFunctions()->Add(l6);
	TLine* l7 = new TLine(1,112,50,112); h->GetListOfFunctions()->Add(l7);
      }

    }
  }

  return StatusCode::SUCCESS;

}

StatusCode MdtRawDataValAlg::binMdtOccVsLB_Crate(TH2* &h, int region, int crate){
  if(region == 0 || region == 1){//Barrel
    if(crate == 0){ //Crate BA01 or BC01
      //Add Labels
      h->SetBins(834,1,2502,73,0,73);
      h->GetYaxis()->SetBinLabel(1,"BIL");
      h->GetYaxis()->SetBinLabel(13,"BIS");
      h->GetYaxis()->SetBinLabel(25,"BME");
      h->GetYaxis()->SetBinLabel(26,"BML");
      h->GetYaxis()->SetBinLabel(38,"BMS");
      h->GetYaxis()->SetBinLabel(50,"BOL");
      h->GetYaxis()->SetBinLabel(62,"BOS");
      //Add lines
      h->GetYaxis()->SetTickLength(0);
      for(int i = 0; i < 73; i++){
	TLine* l = new TLine(1,i,20,i);
	h->GetListOfFunctions()->Add(l);
      }
      TLine* l2 = new TLine(1,0,50,0); h->GetListOfFunctions()->Add(l2);
      TLine* l3 = new TLine(1,12,50,12);  h->GetListOfFunctions()->Add(l3);
      TLine* l4 = new TLine(1,24,50,24);  h->GetListOfFunctions()->Add(l4);
      TLine* l5 = new TLine(1,25,50,25);  h->GetListOfFunctions()->Add(l5);
      TLine* l6 = new TLine(1,37,50,37);  h->GetListOfFunctions()->Add(l6);
      TLine* l8 = new TLine(1,49,50,49);  h->GetListOfFunctions()->Add(l8);
      TLine* l9 = new TLine(1,61,50,61);  h->GetListOfFunctions()->Add(l9);
    }

    else if(crate == 1){ //BA02, BC02
      h->SetBins(834,1,2502,73,0,73);
      //Add Labels
      h->GetYaxis()->SetBinLabel(1,"BIL");
      h->GetYaxis()->SetBinLabel(13,"BIS");
      h->GetYaxis()->SetBinLabel(25,"BME");
      h->GetYaxis()->SetBinLabel(37,"BMS");
      h->GetYaxis()->SetBinLabel(49,"BOL");
      h->GetYaxis()->SetBinLabel(61,"BOS");
      //Add lines
      h->GetYaxis()->SetTickLength(0);
      for(int i = 1; i < 73; i++){
	TLine* l = new TLine(1,i,20,i);
	h->GetListOfFunctions()->Add(l);
      }
      TLine* l1 = new TLine(1,12,50,12); h->GetListOfFunctions()->Add(l1);
      TLine* l2 = new TLine(1,24,50,24); h->GetListOfFunctions()->Add(l2);
      TLine* l3 = new TLine(1,36,50,36); h->GetListOfFunctions()->Add(l3);
      TLine* l4 = new TLine(1,48,50,48); h->GetListOfFunctions()->Add(l4);
      TLine* l6 = new TLine(1,60,50,60); h->GetListOfFunctions()->Add(l6);

    }
    else if(crate == 2){ //BA03, BC03
     //Add Labels
      h->SetBins(834,1,2502,80,0,80);
      h->GetYaxis()->SetBinLabel(1,"BIL");
      h->GetYaxis()->SetBinLabel(7,"BIM");
      h->GetYaxis()->SetBinLabel(12,"BIR");
      h->GetYaxis()->SetBinLabel(18,"BIS");
      h->GetYaxis()->SetBinLabel(30,"BMF");
      h->GetYaxis()->SetBinLabel(33,"BMG");
      h->GetYaxis()->SetBinLabel(36,"BML");
      h->GetYaxis()->SetBinLabel(48,"BMS");
      h->GetYaxis()->SetBinLabel(54,"BOF");
      h->GetYaxis()->SetBinLabel(58,"BOG");
      if(region == 0){
        h->GetYaxis()->SetBinLabel(63,"BOL");
        h->GetYaxis()->SetBinLabel(75,"BOS");
      } else if (region == 1){
        h->GetYaxis()->SetBinLabel(62,"BOL");
        h->GetYaxis()->SetBinLabel(74,"BOS");    	  
      }
      //Add lines
      h->GetYaxis()->SetTickLength(0);
      for(int i = 0; i < 80; i++){
	     TLine* l = new TLine(1,i,20,i);
	     h->GetListOfFunctions()->Add(l);
      } ////change line positions!
      TLine* l2 = new TLine(1,6,50,6); h->GetListOfFunctions()->Add(l2);
      TLine* l3 = new TLine(1,11,50,11);  h->GetListOfFunctions()->Add(l3);
      TLine* l3b = new TLine(1,17,50,17);  h->GetListOfFunctions()->Add(l3b);
      //TLine* l4 = new TLine(1,26,50,26);  h->GetListOfFunctions()->Add(l4); //removed this line because it doesnt correspond to anything
      TLine* l5 = new TLine(1,29,50,29);  h->GetListOfFunctions()->Add(l5);
      TLine* l6 = new TLine(1,32,50,32);  h->GetListOfFunctions()->Add(l6);
      TLine* l7 = new TLine(1,35,50,35);  h->GetListOfFunctions()->Add(l7);
      TLine* l8 = new TLine(1,47,50,47);  h->GetListOfFunctions()->Add(l8);
      TLine* l9 = new TLine(1,53,50,53);  h->GetListOfFunctions()->Add(l9);
      TLine* l9b = new TLine(1,57,50,57);  h->GetListOfFunctions()->Add(l9b);
      if(region == 0) {
    	  TLine* l10 = new TLine(1,62,50,62);  h->GetListOfFunctions()->Add(l10);
          TLine* l11 = new TLine(1,74,50,74);  h->GetListOfFunctions()->Add(l11);
      } else if (region == 1){
          TLine* l10 = new TLine(1,61,50,61);  h->GetListOfFunctions()->Add(l10);
          TLine* l11 = new TLine(1,73,50,73);  h->GetListOfFunctions()->Add(l11);
      }
    }

    else if(crate == 3){ //BA04, BC04 
     //Add Labels
      h->SetBins(834,1,2502,79,0,79);
      h->GetYaxis()->SetBinLabel(1,"BIL");
      h->GetYaxis()->SetBinLabel(7,"BIM");
      h->GetYaxis()->SetBinLabel(12,"BIR");
      h->GetYaxis()->SetBinLabel(18,"BIS");
      h->GetYaxis()->SetBinLabel(30,"BMF");
      h->GetYaxis()->SetBinLabel(33,"BMG");
      h->GetYaxis()->SetBinLabel(36,"BML");
      h->GetYaxis()->SetBinLabel(47,"BMS");
      h->GetYaxis()->SetBinLabel(53,"BOF");
      h->GetYaxis()->SetBinLabel(57,"BOG");
      if (region == 0){
          h->GetYaxis()->SetBinLabel(62,"BOL");
          h->GetYaxis()->SetBinLabel(75,"BOS");
      } else if (region == 1){
          h->GetYaxis()->SetBinLabel(61,"BOL");
          h->GetYaxis()->SetBinLabel(74,"BOS");
      }
      
      //Add lines
      h->GetYaxis()->SetTickLength(0);
      for(int i = 0; i < 79; i++){
	TLine* l = new TLine(1,i,20,i);
	h->GetListOfFunctions()->Add(l);
      }
      TLine* l2 = new TLine(1,6,50,6); h->GetListOfFunctions()->Add(l2);
      TLine* l3 = new TLine(1,11,50,11);  h->GetListOfFunctions()->Add(l3);
      TLine* l3b = new TLine(1,17,50,17);  h->GetListOfFunctions()->Add(l3b);
      //TLine* l4 = new TLine(1,26,50,26);  h->GetListOfFunctions()->Add(l4);//removed this line because it doesnt correspond to anything
      TLine* l5 = new TLine(1,29,50,29);  h->GetListOfFunctions()->Add(l5);
      TLine* l6 = new TLine(1,32,50,32);  h->GetListOfFunctions()->Add(l6);
      TLine* l7 = new TLine(1,35,50,35);  h->GetListOfFunctions()->Add(l7);
      TLine* l8 = new TLine(1,46,50,46);  h->GetListOfFunctions()->Add(l8);
      TLine* l9 = new TLine(1,52,50,52);  h->GetListOfFunctions()->Add(l9);
      TLine* l9b = new TLine(1,56,50,56);  h->GetListOfFunctions()->Add(l9b);
      if(region == 0){
    	  TLine* l10 = new TLine(1,61,50,61);  h->GetListOfFunctions()->Add(l10);
          TLine* l11 = new TLine(1,74,50,74);  h->GetListOfFunctions()->Add(l11);
      } else if (region == 1 ){
    	  TLine* l10 = new TLine(1,60,50,60);  h->GetListOfFunctions()->Add(l10);
          TLine* l11 = new TLine(1,73,50,73);  h->GetListOfFunctions()->Add(l11);
      }
    }
  }

  else{ //Endcap
    if(crate == 0 || crate == 2){ //EA01, EC01 and EA03, EC03 are the same
      h->SetBins(834,1,2502,73,0,73);
      //Add labels
      h->GetYaxis()->SetBinLabel(1,"BEE");
      h->GetYaxis()->SetBinLabel(5,"BIS");
      h->GetYaxis()->SetBinLabel(9,"EEL");
      h->GetYaxis()->SetBinLabel(13,"EES");
      h->GetYaxis()->SetBinLabel(17,"EIL");
      h->GetYaxis()->SetBinLabel(26,"EIS");
      h->GetYaxis()->SetBinLabel(30,"EML");
      h->GetYaxis()->SetBinLabel(40,"EMS");
      h->GetYaxis()->SetBinLabel(50,"EOL");
      h->GetYaxis()->SetBinLabel(62,"EOS");
      //Add lines
      h->GetYaxis()->SetTickLength(0);
      for(int i = 1; i < 73; i++){
	TLine* l = new TLine(1,i,20,i);
	h->GetListOfFunctions()->Add(l);
      }
      TLine* l2 = new TLine(1,4,50,4); h->GetListOfFunctions()->Add(l2);
      TLine* l3 = new TLine(1,8,50,8);  h->GetListOfFunctions()->Add(l3);
      TLine* l4 = new TLine(1,12,50,12);  h->GetListOfFunctions()->Add(l4);
      TLine* l5 = new TLine(1,16,50,16);  h->GetListOfFunctions()->Add(l5);
      TLine* l6 = new TLine(1,25,50,25);  h->GetListOfFunctions()->Add(l6);
      TLine* l7 = new TLine(1,29,50,29);  h->GetListOfFunctions()->Add(l7);
      TLine* l8 = new TLine(1,39,50,39);  h->GetListOfFunctions()->Add(l8);
      TLine* l9 = new TLine(1,49,50,49);  h->GetListOfFunctions()->Add(l9);
      TLine* l10 = new TLine(1,61,50,61);  h->GetListOfFunctions()->Add(l10);
    }
    else if(crate == 1){ //EA02, EC02
      h->SetBins(834,1,2502,71,0,71);
      //Add labels
      h->GetYaxis()->SetBinLabel(1,"BEE");
      h->GetYaxis()->SetBinLabel(5,"BIS");
      h->GetYaxis()->SetBinLabel(9,"EEL");
      h->GetYaxis()->SetBinLabel(12,"EES");
      h->GetYaxis()->SetBinLabel(16,"EIL");
      h->GetYaxis()->SetBinLabel(24,"EIS");
      h->GetYaxis()->SetBinLabel(28,"EML");
      h->GetYaxis()->SetBinLabel(38,"EMS");
      h->GetYaxis()->SetBinLabel(48,"EOL");
      h->GetYaxis()->SetBinLabel(60,"EOS");
      //Add lines
      h->GetYaxis()->SetTickLength(0);
      for(int i = 1; i < 71; i++){
	TLine* l = new TLine(1,i,20,i);
	h->GetListOfFunctions()->Add(l);
      }
      TLine* l2 = new TLine(1,4,50,4); h->GetListOfFunctions()->Add(l2);
      TLine* l3 = new TLine(1,8,50,8);  h->GetListOfFunctions()->Add(l3);
      TLine* l4 = new TLine(1,11,50,11);  h->GetListOfFunctions()->Add(l4);
      TLine* l5 = new TLine(1,15,50,15);  h->GetListOfFunctions()->Add(l5);
      TLine* l6 = new TLine(1,23,50,23);  h->GetListOfFunctions()->Add(l6);
      TLine* l7 = new TLine(1,27,50,27);  h->GetListOfFunctions()->Add(l7);
      TLine* l8 = new TLine(1,37,50,37);  h->GetListOfFunctions()->Add(l8);
      TLine* l9 = new TLine(1,47,50,47);  h->GetListOfFunctions()->Add(l9);
      TLine* l10 = new TLine(1,59,50,59);  h->GetListOfFunctions()->Add(l10);
    }
    else if(crate == 3){ //EA04, EC04
      h->SetBins(834,1,2502,72,0,72);
      //Add labels
      h->GetYaxis()->SetBinLabel(1,"BEE");
      h->GetYaxis()->SetBinLabel(5,"BIS");
      h->GetYaxis()->SetBinLabel(9,"EEL");
      h->GetYaxis()->SetBinLabel(13,"EES");
      h->GetYaxis()->SetBinLabel(17,"EIL");
      h->GetYaxis()->SetBinLabel(25,"EIS");
      h->GetYaxis()->SetBinLabel(29,"EML");
      h->GetYaxis()->SetBinLabel(39,"EMS");
      h->GetYaxis()->SetBinLabel(49,"EOL");
      h->GetYaxis()->SetBinLabel(61,"EOS");
      //Add lines
      h->GetYaxis()->SetTickLength(0);
      for(int i = 1; i < 72; i++){
	TLine* l = new TLine(1,i,20,i);
	h->GetListOfFunctions()->Add(l);
      }
      TLine* l2 = new TLine(1,4,50,4); h->GetListOfFunctions()->Add(l2);
      TLine* l3 = new TLine(1,8,50,8);  h->GetListOfFunctions()->Add(l3);
      TLine* l4 = new TLine(1,12,50,12);  h->GetListOfFunctions()->Add(l4);
      TLine* l5 = new TLine(1,16,50,16);  h->GetListOfFunctions()->Add(l5);
      TLine* l6 = new TLine(1,24,50,24);  h->GetListOfFunctions()->Add(l6);
      TLine* l7 = new TLine(1,28,50,28);  h->GetListOfFunctions()->Add(l7);
      TLine* l8 = new TLine(1,38,50,38);  h->GetListOfFunctions()->Add(l8);
      TLine* l9 = new TLine(1,48,50,48);  h->GetListOfFunctions()->Add(l9);
      TLine* l10 = new TLine(1,60,50,60);  h->GetListOfFunctions()->Add(l10);
    }
  }

  return StatusCode::SUCCESS;

}


int MdtRawDataValAlg::get_bin_for_LB_crate_hist(int region, int crate, int phi, int eta, std::string chamber){
  int binNum = 999;

  if(region == 0 || region == 1){ //Barrel
    if(crate == 1){ //BA01, BC01
	if(chamber.substr(0,3)=="BIL") binNum = 2*eta + (phi-1)/2 - 1; 
	else if(chamber.substr(0,3)=="BIS") binNum = 12 + 2*eta + (phi-2)/2 - 1;
	else if(chamber.substr(0,3)=="BME") binNum = 25;	
	else if(chamber.substr(0,3)=="BML") binNum = 25 + 2*eta + (phi-1)/2 - 1;
	else if(chamber.substr(0,3)=="BMS") binNum = 37 + 2*eta + (phi-2)/2 - 1;
	else if(chamber.substr(0,3)=="BOL") binNum = 49 + 2*eta + (phi-1)/2 - 1;
	else if(chamber.substr(0,3)=="BOS") binNum = 61 + 2*eta + (phi-2)/2 - 1;
	else ATH_MSG_INFO("chamber " << chamber << " didn't exist in crate Bx01");
  	return binNum - 1;
    } else if(crate == 2){//BA02, BC02

	if(chamber.substr(0,3)=="BIL") binNum = 2*eta + (phi-5)/2 - 1; 
	else if(chamber.substr(0,3)=="BIS") binNum = 12 + 2*eta + (phi-6)/2 - 1;
	else if(chamber.substr(0,3)=="BML") binNum = 24 + 2*eta + (phi-5)/2 - 1;
	else if(chamber.substr(0,3)=="BMS") binNum = 36 + 2*eta + (phi-6)/2 - 1;
	else if(chamber.substr(0,3)=="BOL") binNum = 48+ 2*eta + (phi-5)/2 - 1;
	else if(chamber.substr(0,3)=="BOS") binNum = 60 + 2*eta + (phi-6)/2 - 1;
	else ATH_MSG_INFO("chamber " << chamber << " didn't exist in crate Bx02");
	return binNum - 1;
    } else if(crate == 3){//BA03,BC03
	if(chamber.substr(0,3)=="BIL") binNum = eta;
	else if(chamber.substr(0,3)=="BIM") binNum = 6 + eta;
	else if(chamber.substr(0,3)=="BIR") binNum = 11 + eta;
	else if(chamber.substr(0,3)=="BIS") binNum = 17 + 2*eta + (phi-10)/2-1;
	else if(chamber.substr(0,3)=="BMF") binNum = 29 + (eta+1)/2;
	else if(chamber.substr(0,3)=="BMG") binNum = 32 + eta/2;
	else if(chamber.substr(0,3)=="BML") binNum = 35 + 2*eta + (phi-9)/2-1;
	else if(chamber.substr(0,3)=="BMS") binNum = 47 + eta;
	else if(chamber.substr(0,3)=="BOF") binNum = 53 + (eta+1)/2;
	else if(chamber.substr(0,4)=="BOG0" && region == 0 ) binNum = 58;
	else if(chamber.substr(0,3)=="BOG") binNum = 58 + eta/2 - region;
	else if(chamber.substr(0,3)=="BOL") binNum = 62 + 2*eta + (phi-9)/2-1 - region;
	else if(chamber.substr(0,3)=="BOS") binNum = 74 + eta - region;
	else ATH_MSG_INFO("chamber " << chamber << " didn't exist in crate Bx03");
	return binNum - 1;
    } else if(crate == 4){//BA04, BC04

	if(chamber.substr(0,3)=="BIL") binNum = eta;
	else if(chamber.substr(0,3)=="BIM") binNum = 6 + eta;
	else if(chamber.substr(0,3)=="BIR") binNum = 11 + eta;
	else if(chamber.substr(0,3)=="BIS") binNum = 17 + 2*eta + (phi-14)/2-1;
	else if(chamber.substr(0,3)=="BMF") binNum = 29 + (eta+1)/2;
	else if(chamber.substr(0,3)=="BMG") binNum = 32 + eta/2;
	else if(chamber.substr(0,3)=="BML" && eta < 6) binNum = 35 + 2*eta + (phi-13)/2-1;
	else if(chamber.substr(0,7)=="BML6A15" || chamber.substr(0,7)=="BML6C15" ) binNum = 46;
	else if(chamber.substr(0,3)=="BMS") binNum = 46 + eta;
	else if(chamber.substr(0,3)=="BOF") binNum = 52 + (eta+1)/2;
	else if(chamber.substr(0,4)=="BOG0" && region == 0) binNum = 57;
	else if(chamber.substr(0,3)=="BOG") binNum = 57 + eta/2 - region;
	else if(chamber.substr(0,3)=="BOL") binNum = 61 + 2*eta + (phi-13)/2-1 - region;
	else if(chamber.substr(0,3)=="BOS") binNum = 74 + eta - region;
	else ATH_MSG_INFO("chamber " << chamber << " didn't exist in crate Bx04");
	return binNum - 1;
    }
  } else if(region == 2 || region == 3){
	if(crate == 1){ //EA01, EC01
	if(chamber.substr(0,3)=="BEE") binNum = 2*eta + (phi-2)/2-1;
	else if(chamber.substr(0,3)=="BIS") binNum = 4 + 2*(eta-6) + (phi-2)/2-1;
	else if(chamber.substr(0,3)=="EEL") binNum = 8 + 2*eta + (phi-1)/2-1;	
	else if(chamber.substr(0,3)=="EES") binNum = 12 + 2*eta + (phi-2)/2-1;
	else if(chamber.substr(0,3)=="EIL" && eta < 5) binNum = 16 + 2*eta + (phi-1)/2-1;
	else if(chamber.substr(0,3)=="EIL" && eta == 5) binNum = 25;
	else if(chamber.substr(0,3)=="EIS") binNum = 25 + 2*eta + (phi-2)/2-1;
	else if(chamber.substr(0,3)=="EML") binNum = 29 + 2*eta + (phi-1)/2-1;
	else if(chamber.substr(0,3)=="EMS") binNum = 39 + 2*eta + (phi-2)/2-1;
	else if(chamber.substr(0,3)=="EOL") binNum = 49 + 2*eta + (phi-1)/2-1;
	else if(chamber.substr(0,3)=="EOS") binNum = 61 + 2*eta + (phi-2)/2-1;
	else ATH_MSG_INFO("chamber " << chamber << " didn't exist in crate Ex01");	
	return binNum - 1;
    } else if(crate == 2){//EA02, EC02
	if(chamber.substr(0,3)=="BEE") binNum = 2*eta + (phi-6)/2-1;
	else if(chamber.substr(0,3)=="BIS") binNum = 4 + 2*(eta-6) + (phi-6)/2-1;
	else if(chamber.substr(0,4)=="EEL1") binNum = 9 + (phi-3)/2 - 1;
	else if(chamber.substr(0,4)=="EEL2") binNum = 11;
	else if(chamber.substr(0,3)=="EES") binNum = 11 + 2*eta + (phi-6)/2-1;
	else if(chamber.substr(0,3)=="EIL") binNum = 15 + 2*eta + (phi-5)/2-1;
	else if(chamber.substr(0,3)=="EIS") binNum = 23 + 2*eta + (phi-6)/2-1;
	else if(chamber.substr(0,3)=="EML") binNum = 27 + 2*eta + (phi-5)/2-1;
	else if(chamber.substr(0,3)=="EMS") binNum = 37 + 2*eta + (phi-6)/2-1;
	else if(chamber.substr(0,3)=="EOL") binNum = 47 + 2*eta + (phi-5)/2-1;
	else if(chamber.substr(0,3)=="EOS") binNum = 59 + 2*eta + (phi-6)/2-1;
	else ATH_MSG_INFO("chamber " << chamber << " didn't exist in crate Ex02");
	return binNum - 1;
    } else if(crate == 3){//EA03, EC03

	if(chamber.substr(0,3)=="BEE") binNum = 2*eta + (phi-10)/2-1;
	else if(chamber.substr(0,3)=="BIS") binNum = 4 + 2*(eta-6) + (phi-10)/2-1;
	else if(chamber.substr(0,3)=="EEL") binNum = 8 + 2*eta + (phi-9)/2-1;
	else if(chamber.substr(0,3)=="EES") binNum = 12 + 2*eta + (phi-10)/2-1;
	else if(chamber.substr(0,3)=="EIL" && eta < 5) binNum = 16 + 2*eta + (phi-9)/2-1;
	else if(chamber.substr(0,4)=="EIL5") binNum = 25;
	else if(chamber.substr(0,3)=="EIS") binNum = 25 + 2*eta + (phi-10)/2-1;
	else if(chamber.substr(0,3)=="EML") binNum = 29 + 2*eta + (phi-9)/2-1;
	else if(chamber.substr(0,3)=="EMS") binNum = 39 + 2*eta + (phi-10)/2-1;
	else if(chamber.substr(0,3)=="EOL") binNum = 49 + 2*eta + (phi-9)/2-1;
	else if(chamber.substr(0,3)=="EOS") binNum = 61 + 2*eta + (phi-10)/2-1;
	else ATH_MSG_INFO("chamber " << chamber << " didn't exist in crate Ex03");	
	return binNum - 1;
    } else if(crate == 4){//EA04, EC04
	if(chamber.substr(0,3)=="BEE") binNum = 2*eta + (phi-14)/2-1;
	else if(chamber.substr(0,3)=="BIS") binNum = 4 + 2*(eta-6) + (phi-14)/2-1;
	else if(chamber.substr(0,3)=="EEL") binNum = 8 + 2*eta + (phi-13)/2-1;
	else if(chamber.substr(0,3)=="EES") binNum = 12 + 2*eta + (phi-14)/2-1;
	else if(chamber.substr(0,3)=="EIL") binNum = 16 + 2*eta + (phi-13)/2-1;
	else if(chamber.substr(0,3)=="EIS") binNum = 24 + 2*eta + (phi-14)/2-1;
	else if(chamber.substr(0,3)=="EML") binNum = 28 + 2*eta + (phi-13)/2-1;
	else if(chamber.substr(0,3)=="EMS") binNum = 38 + 2*eta + (phi-14)/2-1;
	else if(chamber.substr(0,3)=="EOL") binNum = 48 + 2*eta + (phi-13)/2-1;
	else if(chamber.substr(0,3)=="EOS") binNum = 60 + 2*eta + (phi-14)/2-1;
	else ATH_MSG_INFO("chamber " << chamber << " didn't exist in crate Ex04");	
	return binNum - 1;
    }
  }  return -1;
  
}
