/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDQAUtils/MuonStrToIntName.h"
using namespace std;


//Function converting from ATHENA identifier std::str StationName, std::string ChamberType 
//to the corresponding ATHENA identifier int StationName
int convertStrToIntName(std::string ChamberName, std::string ChamberType) {

  //Input values
  std::string m_Name_str = ChamberName;

  //add type
  //"MDT","CSC","TGC" go in the same loop while
  //"RPC" go to a different loop due to the same
  //naming with "MDT"
  std::string m_Type=ChamberType;


  //Set string variables
  int m_Name_int;



  if (m_Type=="MDT" || m_Type=="CSC" || m_Type=="TGC"){
    //Switch from ATHENA int name to Hardware string name 
    if (m_Name_str=="BIL") m_Name_int = 0; 
    else if (m_Name_str=="BIS") m_Name_int = 1; 
    else if (m_Name_str=="BML") m_Name_int = 2; 
    else if (m_Name_str=="BMS") m_Name_int = 3;  
    else if (m_Name_str=="BOL") m_Name_int = 4; 
    else if (m_Name_str=="BOS") m_Name_int = 5; 
    else if (m_Name_str=="BEE") m_Name_int = 6; 
    else if (m_Name_str=="BIR") m_Name_int = 7; 
    else if (m_Name_str=="BMF") m_Name_int = 8; 
    else if (m_Name_str=="BOF") m_Name_int = 9; 
    else if (m_Name_str=="BOG") m_Name_int = 10;
    else if (m_Name_str=="BME") m_Name_int = 53;
    else if (m_Name_str=="EIC") m_Name_int = 12;
    else if (m_Name_str=="EIL") m_Name_int = 13;
    else if (m_Name_str=="EEL") m_Name_int = 14;
    else if (m_Name_str=="EES") m_Name_int = 15;
    else if (m_Name_str=="EMC") m_Name_int = 16;
    else if (m_Name_str=="EML") m_Name_int = 17;
    else if (m_Name_str=="EMS") m_Name_int = 18;
    else if (m_Name_str=="EOC") m_Name_int = 19;
    else if (m_Name_str=="EOL") m_Name_int = 20;
    else if (m_Name_str=="EOS") m_Name_int = 21;
    else if (m_Name_str=="T1C") m_Name_int = 22;
    else if (m_Name_str=="T1L") m_Name_int = 23;
    else if (m_Name_str=="T1S") m_Name_int = 24;
    else if (m_Name_str=="T2C") m_Name_int = 25;
    else if (m_Name_str=="T2L") m_Name_int = 26;
    else if (m_Name_str=="T2S") m_Name_int = 27;
    else if (m_Name_str=="T3C") m_Name_int = 28;
    else if (m_Name_str=="T3L") m_Name_int = 29;
    else if (m_Name_str=="T3S") m_Name_int = 30;
    else if (m_Name_str=="CI1") m_Name_int = 31;
    else if (m_Name_str=="CI2") m_Name_int = 32;
    else if (m_Name_str=="CI3") m_Name_int = 33;
    else if (m_Name_str=="CI4") m_Name_int = 34;
    else if (m_Name_str=="FIL") m_Name_int = 35;
    else if (m_Name_str=="FIS") m_Name_int = 36;
    else if (m_Name_str=="FML") m_Name_int = 37;
    else if (m_Name_str=="FMS") m_Name_int = 38;
    else if (m_Name_str=="FOL") m_Name_int = 39;
    else if (m_Name_str=="FOS") m_Name_int = 40;
    else if (m_Name_str=="T1F") m_Name_int = 41;
    else if (m_Name_str=="T1E") m_Name_int = 42;
    else if (m_Name_str=="T2F") m_Name_int = 43;
    else if (m_Name_str=="T2E") m_Name_int = 44;
    else if (m_Name_str=="T3F") m_Name_int = 45;
    else if (m_Name_str=="T3E") m_Name_int = 46;
    else if (m_Name_str=="T4F") m_Name_int = 47;
    else if (m_Name_str=="T4E") m_Name_int = 48;
    else if (m_Name_str=="EIS") m_Name_int = 49;
    else if (m_Name_str=="CSS") m_Name_int = 50;
    else if (m_Name_str=="CSL") m_Name_int = 51;
    else if (m_Name_str=="BIM") m_Name_int = 52;
    else if (m_Name_str=="BMG") m_Name_int = 54;
    else m_Name_int = -1;
 
  } else if (m_Type=="RPC") {
    if (m_Name_str=="BML") m_Name_int = 2; 
    else if (m_Name_str=="BMS") m_Name_int = 3;  
    else if (m_Name_str=="BOL") m_Name_int = 4; 
    else if (m_Name_str=="BOS") m_Name_int = 5; 		  
    else if (m_Name_str=="BMF") m_Name_int = 8; 
    else if (m_Name_str=="BOF") m_Name_int = 9; 
    else if (m_Name_str=="BOG") m_Name_int = 10;		  
    else m_Name_int = -1;
  } else {m_Name_int = -1;}





  //Return int StationName

  return m_Name_int;

}
