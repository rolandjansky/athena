/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include <cstdlib>

//Function converting from ATHENA identifier int StationName,int StationEta, int StationPhi, std::string ChamberType 
//to the (hopefully) corresponding Hardware chamber name.Works only for Barrel MDTs (for the moment) 
std::string convertChamberName(int ChamberName , int ChamberEta , int ChamberPhi , std::string ChamberType) {

  //Input values
  int Name_int = ChamberName;
  int Eta_int = ChamberEta;
  int Phi_int = ChamberPhi;
  //add type
  //"MDT","CSC","TGC" go in the same loop while
  //"RPC" go to a different loop due to the same
  //naming with "MDT"
  std::string Type=ChamberType;


  //Set string variables
  std::string Name_string;
  std::string Eta_string;
  std::string Phi_string;

  //Vector setting the chamber eta, phi range
  //Elements:
  // limit eta , forbiden eta , min phi , max phi , allowed phi1 , allowed phi2
  std::vector<int> limits;


  if (Type=="MDT" || Type=="CSC" || Type=="TGC"){
    //Switch from ATHENA int name to Hardware string name 
    switch (Name_int) 
      {
	// identifier :   harware name	  : limit eta	       : forbiden eta	    : min phi		 :  max phi	      :   allowed    phi1     :	allowed phi2      :  long/short
      case 0  : Name_string = "BIL"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(5); limits.push_back(7); limits.push_back(-100); limits.push_back(1); break;
      case 1  : Name_string = "BIS"; limits.push_back(8); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 2  : Name_string = "BML"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 3  : Name_string = "BMS"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(5); limits.push_back(8); limits.push_back(-100); limits.push_back(-1); break;
      case 4  : Name_string = "BOL"; limits.push_back(7); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 5  : Name_string = "BOS"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(5); limits.push_back(8); limits.push_back(-100); limits.push_back(-1); break;
      case 6  : Name_string = "BEE"; limits.push_back(2); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 7  : Name_string = "BIR"; limits.push_back(6); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(8); limits.push_back(1); break;
      case 8  : Name_string = "BMF"; limits.push_back(3); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(7); limits.push_back(-1); break;
      case 9  : Name_string = "BOF"; limits.push_back(4); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(7); limits.push_back(-1); break;
      case 10 : Name_string = "BOG"; limits.push_back(4); limits.push_back(-100); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(7); limits.push_back(-1); break;
      case 11 : Name_string = "BOH_DOES_NOT_EXIST!"; limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 12 : Name_string = "EIC_DOES_NOT_EXIST!"; limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 13 : Name_string = "EIL"; limits.push_back(5); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 14 : Name_string = "EEL"; limits.push_back(2); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 15 : Name_string = "EES"; limits.push_back(2); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break; 
      case 16 : Name_string = "EMC_DOES_NOT_EXIST!"; limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 17 : Name_string = "EML"; limits.push_back(5); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 18 : Name_string = "EMS"; limits.push_back(5); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 19 : Name_string = "EOC_DOES_NOT_EXIST!"; limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 20 : Name_string = "EOL"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 21 : Name_string = "EOS"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;

      // TGC part implemented by Masaya Ishino (Masaya.Ishino@cern.ch)         
	    // TGC start ; harware name  ; limit eta          ; forbiden eta       ; min phi            ; max phi             ; allowed phi1          ; allowed phi2          ; long/short
      case 41 : Name_string = "T1F"; limits.push_back(1); limits.push_back(0); limits.push_back(1); limits.push_back(24); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 42 : Name_string = "T1E"; limits.push_back(4); limits.push_back(0); limits.push_back(1); limits.push_back(48); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 43 : Name_string = "T2F"; limits.push_back(1); limits.push_back(0); limits.push_back(1); limits.push_back(24); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 44 : Name_string = "T2E"; limits.push_back(5); limits.push_back(0); limits.push_back(1); limits.push_back(48); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 45 : Name_string = "T3F"; limits.push_back(1); limits.push_back(0); limits.push_back(1); limits.push_back(24); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 46 : Name_string = "T3E"; limits.push_back(5); limits.push_back(0); limits.push_back(1); limits.push_back(48); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 47 : Name_string = "T4F"; limits.push_back(1); limits.push_back(0); limits.push_back(1); limits.push_back(24); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 48 : Name_string = "T4E"; limits.push_back(1); limits.push_back(0); limits.push_back(1); limits.push_back(21); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      // TGC end

      case 49 : Name_string = "EIS"; limits.push_back(2); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 50 : Name_string = "CSS"; limits.push_back(1); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 51 : Name_string = "CSL"; limits.push_back(1); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 52 : Name_string = "BIM"; limits.push_back(5); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(8); limits.push_back(1); break;
      case 53 : Name_string = "BME"; limits.push_back(1); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(7); limits.push_back(7); limits.push_back(-1); break;
      case 54 : Name_string = "BMG"; limits.push_back(3); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(7); limits.push_back(-1); break;
      default : Name_string = "WrongName"; limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100);
      }
  } else if (Type=="RPC") {
    switch (Name_int) 
      { 
      case 2  : Name_string = "BML"; limits.push_back(7); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 3  : Name_string = "BMS"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(5); limits.push_back(8); limits.push_back(-100); limits.push_back(-1); break; 
      case 4  : Name_string = "BOL"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 5  : Name_string = "BOS"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(5); limits.push_back(8); limits.push_back(-100); limits.push_back(-1); break;  
      case 8  : Name_string = "BMF"; limits.push_back(3); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(7); limits.push_back(-1); break;
      case 9  : Name_string = "BOF"; limits.push_back(4); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(7); limits.push_back(-1); break;
      case 10 : Name_string = "BOG"; limits.push_back(4); limits.push_back(-100); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(7); limits.push_back(-1); break;	 	    
      default : Name_string = "WrongName"; limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); 
      }
  } else {Name_string = "WrongName";}

  //Convert ATHENA int eta to correct side and eta Hardware index
  if (Eta_int>=-limits[0] && Eta_int<=limits[0] && Eta_int!=limits[1]){
    if(Eta_int>0){
      if (Name_int==9){//exception of the BOG,BOF alternating eta
	Eta_int=(2*Eta_int)-1;
      }
      if (Name_int==10){//exception of the BOG,BOF alternating eta
	Eta_int=2*Eta_int;
      }
      if (Name_int==54){//exception of the BMG
	Eta_int=2*Eta_int;
      }
      std::stringstream eta_inttostring;
      std::string eta_str;
      eta_inttostring << Eta_int;
      eta_inttostring >> eta_str; 
      Eta_string += eta_str;
      Eta_string += "A"; 
    }
 
    if (Eta_int<0){
      if (Name_int==9){//exception of the BOG,BOF alternating eta
	Eta_int=(2*Eta_int)+1;
      }
      if (Name_int==10){//exception of the BOG,BOF alternating eta
	Eta_int=2*Eta_int;
      }
      if (Name_int==54){//exception of the BMG alternating eta
	Eta_int=2*Eta_int;
      }
      std::stringstream eta_inttostring;
      std::string eta_str;
      eta_inttostring << std::abs(Eta_int);
      eta_inttostring >> eta_str; 
      Eta_string += eta_str;
      Eta_string += "C";
 
    }
	
    if (Eta_int==0) {
      std::stringstream eta_inttostring;
      std::string eta_str;
      eta_inttostring << Eta_int;
      eta_inttostring >> eta_str; 
      Eta_string += eta_str;
      Eta_string += "B";	
    } 
  } else {Eta_string="WrongEta";}

  //Convert ATHENA int phi to correct phi Hardware index (taking into account Long/Short case)
  // check correct phi_index
  if ((Phi_int>=limits[2] && Phi_int<=limits[3]) || Phi_int==limits[4] || Phi_int==limits[5]) { 
    if (Type=="TGC") {
      std::stringstream phi_inttostring;
      std::string phi_str;
      phi_inttostring << Phi_int;
      phi_inttostring >> phi_str;
      Phi_string = phi_str ;
    } else { // for Type ==TGC
      if (limits[6]==1) {//Long chamber
        Phi_int = (2*Phi_int)-1;
        std::stringstream phi_inttostring;
        std::string phi_str;
        phi_inttostring << Phi_int;
        phi_inttostring >> phi_str; 		
        if (Phi_int<10) {
          Phi_string = "0"+phi_str;		
        } else {
          Phi_string = phi_str;
        }
      }
      if (limits[6]==-1) {//Short chamber
        Phi_int = 2*Phi_int;
        std::stringstream phi_inttostring;
        std::string phi_str;
        phi_inttostring << Phi_int;
        phi_inttostring >> phi_str; 		
        if (Phi_int<10){
          Phi_string = "0"+phi_str;		
        } else {Phi_string = phi_str;}						
      }
    } // end of else ; MDT / RPC
  } else {
    Phi_string="WrongPhi";
  }

  //Return full Hardware name 
  Name_string += Eta_string;
  Name_string += Phi_string;
  limits.clear();
  return Name_string;
}
