/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _IPIXRESULTSDBLISTENER_
#define _IPIXRESULTSDBLISTENER_

#include <string>
#include "PixelCoralClientUtils/PixelMap.h"

namespace CAN {
  class AverageResult_t;
}

class IPixResultsDbListener {
  public:
    virtual ~IPixResultsDbListener() {}
    virtual void newValue(const std::string &var_name, const std::string &conn_name, bool value) = 0;
    virtual void newValue(const std::string &var_name, const std::string &conn_name, unsigned int value) = 0;
    virtual void newValue(const std::string &var_name, const std::string &conn_name, float value) = 0 ;
    virtual void newValue(const std::string &var_name, const std::string &conn_name, const CAN::AverageResult_t &value) = 0 ;
    virtual void newValue(const std::string &var_name, const std::string &conn_name, const PixelMap_t &value) = 0 ;
};

#endif // _IPIXRESULTSDBLISTENER_
