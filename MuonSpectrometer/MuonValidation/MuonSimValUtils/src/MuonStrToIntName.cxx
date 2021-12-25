/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonSimValUtils/MuonStrToIntName.h"

//Function converting from ATHENA identifier std::str StationName, std::string ChamberType 
//to the corresponding ATHENA identifier int StationName
int convertStrToIntName(const std::string& ChamberName, const std::string& ChamberType) {

  //Input values
  const std::string& Name_str = ChamberName;

  //add type
  //"MDT","CSC","TGC" go in the same loop while
  //"RPC" go to a different loop due to the same
  //naming with "MDT"
  const std::string& Type=ChamberType;


  //Set string variables
  int Name_int;



  if (Type=="MDT" || Type=="CSC" || Type=="TGC"){
    //Switch from ATHENA int name to Hardware string name 
    if (Name_str=="BIL") Name_int = 0; 
    else if (Name_str=="BIS") Name_int = 1; 
    else if (Name_str=="BML") Name_int = 2; 
    else if (Name_str=="BMS") Name_int = 3;  
    else if (Name_str=="BOL") Name_int = 4; 
    else if (Name_str=="BOS") Name_int = 5; 
    else if (Name_str=="BEE") Name_int = 6; 
    else if (Name_str=="BIR") Name_int = 7; 
    else if (Name_str=="BMF") Name_int = 8; 
    else if (Name_str=="BOF") Name_int = 9; 
    else if (Name_str=="BOG") Name_int = 10;
    else if (Name_str=="BOH") Name_int = 11;
    else if (Name_str=="EIC") Name_int = 12;
    else if (Name_str=="EIL") Name_int = 13;
    else if (Name_str=="EEL") Name_int = 14;
    else if (Name_str=="EES") Name_int = 15;
    else if (Name_str=="EMC") Name_int = 16;
    else if (Name_str=="EML") Name_int = 17;
    else if (Name_str=="EMS") Name_int = 18;
    else if (Name_str=="EOC") Name_int = 19;
    else if (Name_str=="EOL") Name_int = 20;
    else if (Name_str=="EOS") Name_int = 21;
    else if (Name_str=="T1C") Name_int = 22;
    else if (Name_str=="T1L") Name_int = 23;
    else if (Name_str=="T1S") Name_int = 24;
    else if (Name_str=="T2C") Name_int = 25;
    else if (Name_str=="T2L") Name_int = 26;
    else if (Name_str=="T2S") Name_int = 27;
    else if (Name_str=="T3C") Name_int = 28;
    else if (Name_str=="T3L") Name_int = 29;
    else if (Name_str=="T3S") Name_int = 30;
    else if (Name_str=="CI1") Name_int = 31;
    else if (Name_str=="CI2") Name_int = 32;
    else if (Name_str=="CI3") Name_int = 33;
    else if (Name_str=="CI4") Name_int = 34;
    else if (Name_str=="FIL") Name_int = 35;
    else if (Name_str=="FIS") Name_int = 36;
    else if (Name_str=="FML") Name_int = 37;
    else if (Name_str=="FMS") Name_int = 38;
    else if (Name_str=="FOL") Name_int = 39;
    else if (Name_str=="FOS") Name_int = 40;
    else if (Name_str=="T1F") Name_int = 41;
    else if (Name_str=="T1E") Name_int = 42;
    else if (Name_str=="T2F") Name_int = 43;
    else if (Name_str=="T2E") Name_int = 44;
    else if (Name_str=="T3F") Name_int = 45;
    else if (Name_str=="T3E") Name_int = 46;
    else if (Name_str=="T4F") Name_int = 47;
    else if (Name_str=="T4E") Name_int = 48;
    else if (Name_str=="EIS") Name_int = 49;
    else if (Name_str=="CSS") Name_int = 50;
    else if (Name_str=="CSL") Name_int = 51;
    else if (Name_str=="BIM") Name_int = 52;
    else Name_int = -1;
 
  } else if (Type=="RPC") {
    if (Name_str=="BML") Name_int = 2; 
    else if (Name_str=="BMS") Name_int = 3;  
    else if (Name_str=="BOL") Name_int = 4; 
    else if (Name_str=="BOS") Name_int = 5; 		  
    else if (Name_str=="BMF") Name_int = 8; 
    else if (Name_str=="BOF") Name_int = 9; 
    else if (Name_str=="BOG") Name_int = 10;		  
    else Name_int = -1;
  } else {Name_int = -1;}





  //Return int StationName

  return Name_int;

}
