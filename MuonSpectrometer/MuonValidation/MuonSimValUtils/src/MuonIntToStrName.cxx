/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimValUtils/MuonStrToIntName.h"
using namespace std;


//Function converting from ATHENA identifier int StationName, std::string ChamberType 
//to the corresponding ATHENA identifier std::string StationName
std::string convertIntToStrName(int ChamberNameInt, std::string ChamberType) {

  //Input values
  int m_Name_int = ChamberNameInt;

  //add type
  //"MDT","CSC","TGC" go in the same loop while
  //"RPC" go to a different loop due to the same
  //naming with "MDT"
  std::string m_Type=ChamberType;


  //Set string variables
  std::string m_Name_str;


  if (m_Type=="MDT" || m_Type=="CSC" || m_Type=="TGC"){
    //Switch from ATHENA int name to Hardware string name
    if (m_Name_int == 0) m_Name_str="BIL";
    else if (m_Name_int == 1) m_Name_str="BIS";
    else if (m_Name_int == 2) m_Name_str="BML";
    else if (m_Name_int == 3) m_Name_str="BMS";
    else if (m_Name_int == 4) m_Name_str="BOL";
    else if (m_Name_int == 5) m_Name_str="BOS";
    else if (m_Name_int == 6) m_Name_str="BEE";
    else if (m_Name_int == 7) m_Name_str="BIR";
    else if (m_Name_int == 8) m_Name_str="BMF";
    else if (m_Name_int == 9) m_Name_str="BOF";
    else if (m_Name_int == 10) m_Name_str="BOG";
    else if (m_Name_int == 11) m_Name_str="BOH";
    else if (m_Name_int == 12) m_Name_str="EIC";
    else if (m_Name_int == 13) m_Name_str="EIL";
    else if (m_Name_int == 14) m_Name_str="EEL";
    else if (m_Name_int == 15) m_Name_str="EES";
    else if (m_Name_int == 16) m_Name_str="EMC";
    else if (m_Name_int == 17) m_Name_str="EML";
    else if (m_Name_int == 18) m_Name_str="EMS";
    else if (m_Name_int == 19) m_Name_str="EOC";
    else if (m_Name_int == 20) m_Name_str="EOL";
    else if (m_Name_int == 21) m_Name_str="EOS";
    else if (m_Name_int == 22) m_Name_str="T1C";
    else if (m_Name_int == 23) m_Name_str="T1L";
    else if (m_Name_int == 24) m_Name_str="T1S";
    else if (m_Name_int == 25) m_Name_str="T2C";
    else if (m_Name_int == 26) m_Name_str="T2L";
    else if (m_Name_int == 27) m_Name_str="T2S";
    else if (m_Name_int == 28) m_Name_str="T3C";
    else if (m_Name_int == 29) m_Name_str="T3L";
    else if (m_Name_int == 30) m_Name_str="T3S";
    else if (m_Name_int == 31) m_Name_str="CI1";
    else if (m_Name_int == 32) m_Name_str="CI2";
    else if (m_Name_int == 33) m_Name_str="CI3";
    else if (m_Name_int == 34) m_Name_str="CI4";
    else if (m_Name_int == 35) m_Name_str="FIL";
    else if (m_Name_int == 36) m_Name_str="FIS";
    else if (m_Name_int == 37) m_Name_str="FML";
    else if (m_Name_int == 38) m_Name_str="FMS";
    else if (m_Name_int == 39) m_Name_str="FOL";
    else if (m_Name_int == 40) m_Name_str="FOS";
    else if (m_Name_int == 41) m_Name_str="T1F";
    else if (m_Name_int == 42) m_Name_str="T1E";
    else if (m_Name_int == 43) m_Name_str="T2F";
    else if (m_Name_int == 44) m_Name_str="T2E";
    else if (m_Name_int == 45) m_Name_str="T3F";
    else if (m_Name_int == 46) m_Name_str="T3E";
    else if (m_Name_int == 47) m_Name_str="T4F";
    else if (m_Name_int == 48) m_Name_str="T4E";
    else if (m_Name_int == 49) m_Name_str="EIS";
    else if (m_Name_int == 50) m_Name_str="CSS";
    else if (m_Name_int == 51) m_Name_str="CSL";
    else if (m_Name_int == 52) m_Name_str="BIM";
    else m_Name_str = "XXX";
 
  } else if (m_Type=="RPC") {
    if (m_Name_int == 2) m_Name_str="BML";
    else if (m_Name_int == 3) m_Name_str="BMS";
    else if (m_Name_int == 4) m_Name_str="BOL";
    else if (m_Name_int == 5) m_Name_str="BOS";
    else if (m_Name_int == 8) m_Name_str="BMF";
    else if (m_Name_int == 9) m_Name_str="BOF";
    else if (m_Name_int == 10) m_Name_str="BOG";
    else m_Name_str = "XXX";
  } else {m_Name_str = "XXX";}

  //Return int StationName

  return m_Name_str;

}
