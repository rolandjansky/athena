/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include <cstdlib>
using namespace std;


//Function converting from ATHENA identifier int StationName,int StationEta, int StationPhi, std::string ChamberType 
//to the (hopefully) corresponding Hardware chamber name.Works only for Barrel MDTs (for the moment) 
std::string convertChamberName(int ChamberName , int ChamberEta , int ChamberPhi , std::string ChamberType) {

  //Input values
  int m_Name_int = ChamberName;
  int m_Eta_int = ChamberEta;
  int m_Phi_int = ChamberPhi;
  //add type
  //"MDT","CSC","TGC" go in the same loop while
  //"RPC" go to a different loop due to the same
  //naming with "MDT"
  std::string m_Type=ChamberType;


  //Set string variables
  std::string m_Name_string;
  std::string m_Eta_string;
  std::string m_Phi_string;

  //Vector setting the chamber eta, phi range
  //Elements:
  // limit eta , forbiden eta , min phi , max phi , allowed phi1 , allowed phi2
  std::vector<int> limits;


  if (m_Type=="MDT" || m_Type=="CSC" || m_Type=="TGC"){
    //Switch from ATHENA int name to Hardware string name 
    switch (m_Name_int) 
      {
	// identifier :   harware name	  : limit eta	       : forbiden eta	    : min phi		 :  max phi	      :   allowed    phi1     :	allowed phi2      :  long/short
      case 0  : m_Name_string = "BIL"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(5); limits.push_back(7); limits.push_back(-100); limits.push_back(1); break;
      case 1  : m_Name_string = "BIS"; limits.push_back(8); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 2  : m_Name_string = "BML"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 3  : m_Name_string = "BMS"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(5); limits.push_back(8); limits.push_back(-100); limits.push_back(-1); break;
      case 4  : m_Name_string = "BOL"; limits.push_back(7); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 5  : m_Name_string = "BOS"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(5); limits.push_back(8); limits.push_back(-100); limits.push_back(-1); break;
      case 6  : m_Name_string = "BEE"; limits.push_back(2); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 7  : m_Name_string = "BIR"; limits.push_back(6); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(8); limits.push_back(1); break;
      case 8  : m_Name_string = "BMF"; limits.push_back(3); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(7); limits.push_back(-1); break;
      case 9  : m_Name_string = "BOF"; limits.push_back(4); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(7); limits.push_back(-1); break;
      case 10 : m_Name_string = "BOG"; limits.push_back(4); limits.push_back(-100); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(7); limits.push_back(-1); break;
      case 11 : m_Name_string = "BOH_DOES_NOT_EXIST!"; limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 12 : m_Name_string = "EIC_DOES_NOT_EXIST!"; limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 13 : m_Name_string = "EIL"; limits.push_back(5); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 14 : m_Name_string = "EEL"; limits.push_back(2); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 15 : m_Name_string = "EES"; limits.push_back(2); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break; 
      case 16 : m_Name_string = "EMC_DOES_NOT_EXIST!"; limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 17 : m_Name_string = "EML"; limits.push_back(5); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 18 : m_Name_string = "EMS"; limits.push_back(5); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 19 : m_Name_string = "EOC_DOES_NOT_EXIST!"; limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 20 : m_Name_string = "EOL"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 21 : m_Name_string = "EOS"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
//       case 22 : m_Name_string = "T1C"; break;
//       case 23 : m_Name_string = "T1L"; break;
//       case 24 : m_Name_string = "T1S"; break;
//       case 25 : m_Name_string = "T2C"; break;
//       case 26 : m_Name_string = "T2L"; break;
//       case 27 : m_Name_string = "T2S"; break;
//       case 28 : m_Name_string = "T3C"; break;
//       case 29 : m_Name_string = "T3L"; break;
//       case 30 : m_Name_string = "T3S"; break;
//       case 31 : m_Name_string = "CI1"; break;
//       case 32 : m_Name_string = "CI2"; break;
//       case 33 : m_Name_string = "CI3"; break;
//       case 34 : m_Name_string = "CI4"; break;
//       case 35 : m_Name_string = "FIL"; break;
//       case 36 : m_Name_string = "FIS"; break;
//       case 37 : m_Name_string = "FML"; break;
//       case 38 : m_Name_string = "FMS"; break;
//       case 39 : m_Name_string = "FOL"; break;
//       case 40 : m_Name_string = "FOS"; break;

        // TGC part implemented by Masaya Ishino (Masaya.Ishino@cern.ch)         
	// TGC start ;    harware name	  : limit eta	       : forbiden eta	    : min phi		 :  max phi	      :   allowed    phi1     :	allowed phi2      :  long/short
      case 41 : m_Name_string = "T1F"; limits.push_back(1); limits.push_back(0); limits.push_back(1); limits.push_back(24); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 42 : m_Name_string = "T1E"; limits.push_back(4); limits.push_back(0); limits.push_back(1); limits.push_back(48); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 43 : m_Name_string = "T2F"; limits.push_back(1); limits.push_back(0); limits.push_back(1); limits.push_back(24); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 44 : m_Name_string = "T2E"; limits.push_back(5); limits.push_back(0); limits.push_back(1); limits.push_back(48); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 45 : m_Name_string = "T3F"; limits.push_back(1); limits.push_back(0); limits.push_back(1); limits.push_back(24); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 46 : m_Name_string = "T3E"; limits.push_back(5); limits.push_back(0); limits.push_back(1); limits.push_back(48); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 47 : m_Name_string = "T4F"; limits.push_back(1); limits.push_back(0); limits.push_back(1); limits.push_back(24); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 48 : m_Name_string = "T4E"; limits.push_back(1); limits.push_back(0); limits.push_back(1); limits.push_back(21); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
	// TGC end   ;    harware name	  : limit eta	       : forbiden eta	    : min phi		 :  max phi	      :   allowed    phi1     :	allowed phi2      :  long/short

      case 49 : m_Name_string = "EIS"; limits.push_back(2); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 50 : m_Name_string = "CSS"; limits.push_back(1); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(-1); break;
      case 51 : m_Name_string = "CSL"; limits.push_back(1); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 52 : m_Name_string = "BIM"; limits.push_back(5); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(8); limits.push_back(1); break;
      case 53 : m_Name_string = "BME"; limits.push_back(1); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(7); limits.push_back(7); limits.push_back(-1); break;
      case 54 : m_Name_string = "BMG"; limits.push_back(3); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(7); limits.push_back(-1); break;
      default : m_Name_string = "WrongName"; limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100);
      }
  } else if (m_Type=="RPC") {
    switch (m_Name_int) 
      { 
      case 2  : m_Name_string = "BML"; limits.push_back(7); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 3  : m_Name_string = "BMS"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(5); limits.push_back(8); limits.push_back(-100); limits.push_back(-1); break; 
      case 4  : m_Name_string = "BOL"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(8); limits.push_back(-100); limits.push_back(-100); limits.push_back(1); break;
      case 5  : m_Name_string = "BOS"; limits.push_back(6); limits.push_back(0); limits.push_back(1); limits.push_back(5); limits.push_back(8); limits.push_back(-100); limits.push_back(-1); break;  
      case 8  : m_Name_string = "BMF"; limits.push_back(3); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(7); limits.push_back(-1); break;
      case 9  : m_Name_string = "BOF"; limits.push_back(4); limits.push_back(0); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(7); limits.push_back(-1); break;
      case 10 : m_Name_string = "BOG"; limits.push_back(4); limits.push_back(-100); limits.push_back(100); limits.push_back(-100); limits.push_back(6); limits.push_back(7); limits.push_back(-1); break;	 	    
      default : m_Name_string = "WrongName"; limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); limits.push_back(-100); 
      }
  } else {m_Name_string = "WrongName";}

  //        cout << "limits[0] limits[1] limits[2] limits[3] limits[4] limits[5] " << limits.at(0) << " " << limits.at(1) << " " <<
  //        limits.at(2) << " " << limits.at(3) << " " << limits.at(4) << " " << limits.at(5) << endl;

  //Convert ATHENA int eta to correct side and eta Hardware index
  if (m_Eta_int>=-limits[0] && m_Eta_int<=limits[0] && m_Eta_int!=limits[1]){
    if(m_Eta_int>0){
      if (m_Name_int==9){//exception of the BOG,BOF alternating eta
	m_Eta_int=(2*m_Eta_int)-1;
      }
      if (m_Name_int==10){//exception of the BOG,BOF alternating eta
	m_Eta_int=2*m_Eta_int;
      }
      if (m_Name_int==54){//exception of the BMG
	m_Eta_int=2*m_Eta_int;
      }
      std::stringstream eta_inttostring;
      std::string eta_str;
      eta_inttostring << m_Eta_int;
      eta_inttostring >> eta_str; 
      m_Eta_string += eta_str;
      m_Eta_string += "A"; 
    }
 
    if (m_Eta_int<0){
      if (m_Name_int==9){//exception of the BOG,BOF alternating eta
	m_Eta_int=(2*m_Eta_int)+1;
      }
      if (m_Name_int==10){//exception of the BOG,BOF alternating eta
	m_Eta_int=2*m_Eta_int;
      }
      if (m_Name_int==54){//exception of the BMG alternating eta
	m_Eta_int=2*m_Eta_int;
      }
      std::stringstream eta_inttostring;
      std::string eta_str;
      eta_inttostring << abs(m_Eta_int);
      eta_inttostring >> eta_str; 
      m_Eta_string += eta_str;
      m_Eta_string += "C";
 
    }
	
    if (m_Eta_int==0) {
      std::stringstream eta_inttostring;
      std::string eta_str;
      eta_inttostring << m_Eta_int;
      eta_inttostring >> eta_str; 
      m_Eta_string += eta_str;
      m_Eta_string += "B";	
    } 
  } else {m_Eta_string="WrongEta";}

  //Convert ATHENA int phi to correct phi Hardware index (taking into account Long/Short case) 
  if ((m_Phi_int>=limits[2] && m_Phi_int<=limits[3]) || m_Phi_int==limits[4] || m_Phi_int==limits[5]) // correct phi_index
    { 
      if (m_Type=="TGC") 
	{
	  std::stringstream phi_inttostring;
	  std::string phi_str;
	  phi_inttostring << m_Phi_int;
	  phi_inttostring >> phi_str;
	  m_Phi_string = phi_str ;
	} // for m_Type ==TGC
      else 
	{
	  if (limits[6]==1){//Long chamber
	    m_Phi_int = (2*m_Phi_int)-1;
	    std::stringstream phi_inttostring;
	    std::string phi_str;
	    phi_inttostring << m_Phi_int;
	    phi_inttostring >> phi_str; 		
	    if (m_Phi_int<10){
	      m_Phi_string = "0"+phi_str;		
	    } else {m_Phi_string = phi_str;}
	  }

	  if (limits[6]==-1){//Short chamber
	    m_Phi_int = 2*m_Phi_int;
	    std::stringstream phi_inttostring;
	    std::string phi_str;
	    phi_inttostring << m_Phi_int;
	    phi_inttostring >> phi_str; 		
	    if (m_Phi_int<10){
	      m_Phi_string = "0"+phi_str;		
	    } else {m_Phi_string = phi_str;}						
	  }	
	} // end of else ; MDT / RPC
 
    } else {m_Phi_string="WrongPhi";}

  //Return full Hardware name 
  m_Name_string += m_Eta_string;
  m_Name_string += m_Phi_string;
  limits.clear();
  return m_Name_string;

}



// //Function overload:
// //Converting from Hardware chamber name to the ATHENA identifier int StationName,int StationEta, int StationPhi 
//  convertChamberName(std::string HarwareChamberName , std::string HarwareChamberType , std::string ChamberType) {
// 
// 
// }


