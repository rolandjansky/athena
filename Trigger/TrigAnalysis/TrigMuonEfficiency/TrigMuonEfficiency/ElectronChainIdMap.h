/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ElectronChainIdMap_h
#define ElectronChainIdMap_h
void ElectronTriggerMatching::createChainIdMapFromFile() {
  std::cerr << "ElectronTriggerMatching\t" << "inf> " << "Create ChainIdMap from TrigMuonEfficiency/ElectronChainIdMap.h.\n";
  std::cerr << std::endl;

  std::map<std::string, int> ChainIdMap;
  ChainIdMap.insert(std::pair<std::string, int>("EF_e20_medium",    10297));
  ChainIdMap.insert(std::pair<std::string, int>("EF_e22_medium",    10528));
  ChainIdMap.insert(std::pair<std::string, int>("EF_e22vh_medium1", 10049));
  ChainIdMap.insert(std::pair<std::string, int>("EF_e45_medium1",   10547));
  ChainIdMap.insert(std::pair<std::string, int>("EF_2e12_medium",   10361));
  ChainIdMap.insert(std::pair<std::string, int>("EF_2e12T_medium",  10718));
  ChainIdMap.insert(std::pair<std::string, int>("EF_e22vh_medium",  10528));
  ChainIdMap.insert(std::pair<std::string, int>("EF_e22vh_medium1", 10049));
  ChainIdMap.insert(std::pair<std::string, int>("EF_2e12Tvh_medium",10718));
  ChainIdMap.insert(std::pair<std::string, int>("EF_e24vhi_medium1",10178));
  ChainIdMap.insert(std::pair<std::string, int>("EF_e24vh_medium1", 10179));
  ChainIdMap.insert(std::pair<std::string, int>("EF_e45_medium1",   10547));
  ChainIdMap.insert(std::pair<std::string, int>("EF_e60_medium1",   10676));
  ChainIdMap.insert(std::pair<std::string, int>("EF_2e12Tvh_loose1",10719));
  ChainIdMap.insert(std::pair<std::string, int>("EF_e10_medium_mu6",10541));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(-1, ChainIdMap));

  std::map<std::string, int> ChainIdMap295;
  ChainIdMap295.insert(std::pair<std::string, int>("EF_e20_medium",  10297));
  ChainIdMap295.insert(std::pair<std::string, int>("EF_e22_medium",  10528));
  ChainIdMap295.insert(std::pair<std::string, int>("EF_e22_medium1", 10049));
  ChainIdMap295.insert(std::pair<std::string, int>("EF_e45_medium1", 10547));
  ChainIdMap295.insert(std::pair<std::string, int>("EF_2e12_medium", 10361));
  ChainIdMap295.insert(std::pair<std::string, int>("EF_2e12T_medium",10718));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(295, ChainIdMap295));

  std::map<std::string, int> ChainIdMap308;
  ChainIdMap308.insert(std::pair<std::string, int>("EF_e15_medium",  10003));
  ChainIdMap308.insert(std::pair<std::string, int>("EF_e20_medium",  10297));
  ChainIdMap308.insert(std::pair<std::string, int>("EF_e22_medium",  10528));
  ChainIdMap308.insert(std::pair<std::string, int>("EF_e22_medium1", 10049));
  ChainIdMap308.insert(std::pair<std::string, int>("EF_e45_medium1", 10547));
  ChainIdMap308.insert(std::pair<std::string, int>("EF_2e12_medium", 10361));
  ChainIdMap308.insert(std::pair<std::string, int>("EF_2e12T_medium",10718));
  ChainIdMap308.insert(std::pair<std::string, int>("EF_e10_medium_mu6",10541));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(308,  ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1190, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1187, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1176, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1172, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1170, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1168, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1164, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1163, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1161, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1141, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1137, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1132, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1127, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1123, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1122, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1101, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1096, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1094, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1087, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1085, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1082, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1078, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1053, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1052, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1051, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1048, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1044, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1043, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1039, ChainIdMap308));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1038, ChainIdMap308));

  std::map<std::string, int> ChainIdMap310;
  ChainIdMap310.insert(std::pair<std::string, int>("EF_e15vh_medium",  10003));
  ChainIdMap310.insert(std::pair<std::string, int>("EF_e22vh_medium",  10528));
  ChainIdMap310.insert(std::pair<std::string, int>("EF_e22vh_medium1", 10049));
  ChainIdMap310.insert(std::pair<std::string, int>("EF_e45_medium1",   10547));
  ChainIdMap310.insert(std::pair<std::string, int>("EF_e22vh_medium",  10528));
  ChainIdMap310.insert(std::pair<std::string, int>("EF_e22vh_medium1", 10049));
  ChainIdMap310.insert(std::pair<std::string, int>("EF_2e12Tvh_medium",10718));
  ChainIdMap310.insert(std::pair<std::string, int>("EF_e10_medium_mu6",10541));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(310,  ChainIdMap310));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1259, ChainIdMap310));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1255, ChainIdMap310));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1254, ChainIdMap310));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1237, ChainIdMap310));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1236, ChainIdMap310));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1232, ChainIdMap310));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1231, ChainIdMap310));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1228, ChainIdMap310));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1223, ChainIdMap310));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1216, ChainIdMap310));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1215, ChainIdMap310));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1213, ChainIdMap310));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1208, ChainIdMap310));
  
  std::map<std::string, int> ChainIdMap325;
  ChainIdMap325.insert(std::pair<std::string, int>("EF_e22vh_medium1",  10049));
  ChainIdMap325.insert(std::pair<std::string, int>("EF_e24vhi_medium1", 10178));
  ChainIdMap325.insert(std::pair<std::string, int>("EF_e24vh_medium1",  10179));
  ChainIdMap325.insert(std::pair<std::string, int>("EF_e45_medium1",    10547));
  ChainIdMap325.insert(std::pair<std::string, int>("EF_e60_medium1",    10676));
  ChainIdMap325.insert(std::pair<std::string, int>("EF_2e12Tvh_loose1", 10719));
  ChainIdMap325.insert(std::pair<std::string, int>("EF_2e12Tvh_loose1_L2StarB", 10426));
  ChainIdMap325.insert(std::pair<std::string, int>("EF_e24vh_medium1_e7_medium1", 10789));
  ChainIdMap325.insert(std::pair<std::string, int>("EF_e12Tvh_medium1_mu8", 10126));
  ChainIdMap325.insert(std::pair<std::string, int>("EF_e24vhi_loose1_mu8",  10543));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(325,  ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1369, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1370, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1372, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1373, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1374, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1375, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1388, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1389, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1393, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1396, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1398, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1399, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1404, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1407, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1409, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1413, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1428, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1430, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1433, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1439, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1441, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1443, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1445, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1446, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1453, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1459, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1487, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1505, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1511, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1514, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1518, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1522, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1529, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1532, ChainIdMap325));
  m_MapOfChainIdMap.insert(std::pair<int, std::map<std::string, int> >(1534, ChainIdMap325));

}
#endif
