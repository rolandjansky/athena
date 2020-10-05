/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_COOLCORALCLIENT_H
#define TRT_COOLCORALCLIENT_H

#include <map>                          // for map
#include <string>                       // for string
#include <utility>                      // for pair
#include <vector>                       // for vector
#include "CoolCoralClient.h"            // for COOLCORALClient

class TRT_COOLCORALClient : private COOLCORALClient {

 public:

  enum DetectorType {
    Barrel = 81,
    EndcapA = 98, // to be determined
    EndcapC = 99 // to be determined
  };

  TRT_COOLCORALClient(const std::string& id1 );
  ~TRT_COOLCORALClient();

  void GetHVLineFromPad( const DetectorType&, int, int, int, std::vector<std::string>& );
  void CreateHVLinePadMap(const DetectorType&, std::map<int,std::string>&  );
  int HashThisPad( const DetectorType&, int, int, int );

  void get_BarrelHVLinePadMap(std::map<std::string,std::string>&);
  void get_COOLChanMap( const std::string&, std::map<int,std::string>& );
  void get_RunTagAndTime( int, std::pair<std::string,int>& );
  void get_EnabledRODs( int, std::vector<int>& );

};

#endif // TRT_COOLCORALCLIENT_H
