/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimValUtils/MuonStrToIntName.h"

//Function converting from ATHENA identifier int StationName, std::string ChamberType 
//to the corresponding ATHENA identifier std::string StationName
std::string convertIntToStrName(int ChamberNameInt, const std::string& ChamberType) {

  //Input values
  int Name_int = ChamberNameInt;

  //add type
  //"MDT","CSC","TGC" go in the same loop while
  //"RPC" go to a different loop due to the same
  //naming with "MDT"
  const std::string& Type=ChamberType;


  //Set string variables
  std::string Name_str;


  if (Type=="MDT" || Type=="CSC" || Type=="TGC"){
    //Switch from ATHENA int name to Hardware string name
    if (Name_int == 0) Name_str="BIL";
    else if (Name_int == 1) Name_str="BIS";
    else if (Name_int == 2) Name_str="BML";
    else if (Name_int == 3) Name_str="BMS";
    else if (Name_int == 4) Name_str="BOL";
    else if (Name_int == 5) Name_str="BOS";
    else if (Name_int == 6) Name_str="BEE";
    else if (Name_int == 7) Name_str="BIR";
    else if (Name_int == 8) Name_str="BMF";
    else if (Name_int == 9) Name_str="BOF";
    else if (Name_int == 10) Name_str="BOG";
    else if (Name_int == 11) Name_str="BOH";
    else if (Name_int == 12) Name_str="EIC";
    else if (Name_int == 13) Name_str="EIL";
    else if (Name_int == 14) Name_str="EEL";
    else if (Name_int == 15) Name_str="EES";
    else if (Name_int == 16) Name_str="EMC";
    else if (Name_int == 17) Name_str="EML";
    else if (Name_int == 18) Name_str="EMS";
    else if (Name_int == 19) Name_str="EOC";
    else if (Name_int == 20) Name_str="EOL";
    else if (Name_int == 21) Name_str="EOS";
    else if (Name_int == 22) Name_str="T1C";
    else if (Name_int == 23) Name_str="T1L";
    else if (Name_int == 24) Name_str="T1S";
    else if (Name_int == 25) Name_str="T2C";
    else if (Name_int == 26) Name_str="T2L";
    else if (Name_int == 27) Name_str="T2S";
    else if (Name_int == 28) Name_str="T3C";
    else if (Name_int == 29) Name_str="T3L";
    else if (Name_int == 30) Name_str="T3S";
    else if (Name_int == 31) Name_str="CI1";
    else if (Name_int == 32) Name_str="CI2";
    else if (Name_int == 33) Name_str="CI3";
    else if (Name_int == 34) Name_str="CI4";
    else if (Name_int == 35) Name_str="FIL";
    else if (Name_int == 36) Name_str="FIS";
    else if (Name_int == 37) Name_str="FML";
    else if (Name_int == 38) Name_str="FMS";
    else if (Name_int == 39) Name_str="FOL";
    else if (Name_int == 40) Name_str="FOS";
    else if (Name_int == 41) Name_str="T1F";
    else if (Name_int == 42) Name_str="T1E";
    else if (Name_int == 43) Name_str="T2F";
    else if (Name_int == 44) Name_str="T2E";
    else if (Name_int == 45) Name_str="T3F";
    else if (Name_int == 46) Name_str="T3E";
    else if (Name_int == 47) Name_str="T4F";
    else if (Name_int == 48) Name_str="T4E";
    else if (Name_int == 49) Name_str="EIS";
    else if (Name_int == 50) Name_str="CSS";
    else if (Name_int == 51) Name_str="CSL";
    else if (Name_int == 52) Name_str="BIM";
    else Name_str = "XXX";
 
  } else if (Type=="RPC") {
    if (Name_int == 2) Name_str="BML";
    else if (Name_int == 3) Name_str="BMS";
    else if (Name_int == 4) Name_str="BOL";
    else if (Name_int == 5) Name_str="BOS";
    else if (Name_int == 8) Name_str="BMF";
    else if (Name_int == 9) Name_str="BOF";
    else if (Name_int == 10) Name_str="BOG";
    else Name_str = "XXX";
  } else {Name_str = "XXX";}

  //Return int StationName

  return Name_str;

}
