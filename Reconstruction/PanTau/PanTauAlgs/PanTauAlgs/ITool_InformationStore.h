/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_ITOOL_INFORMATIONSTORE_H
#define PANTAUALGS_ITOOL_INFORMATIONSTORE_H
// Gaudi
#include "AsgTools/IAsgTool.h"

#include <string>

namespace PanTau {


  /** @class ITool_InformationStore
      @brief Interface for Tool_InformationStore
      @author Christian Limbach (limbach@physik.uni-bonn.de)
  */


  class ITool_InformationStore : virtual public asg::IAsgTool {
    ASG_TOOL_INTERFACE(ITool_InformationStore)


      public:

    virtual bool isInitialized() = 0;

    typedef std::map<std::string, std::string>                  MapString;
    typedef std::map<std::string, std::vector<std::string> >    MapVecString;
    typedef std::map<std::string, int>                          MapInt;
    typedef std::map<std::string, double>                       MapDouble;
    typedef std::map<std::string, std::vector<double> >         MapVecDouble;

#ifdef XAOD_ANALYSIS
    virtual inline void setMapString( MapString&v ) = 0 ;
    virtual inline void setMapVecString( MapVecString&v ) = 0 ;
    virtual inline void setMapInt( MapInt &v ) = 0 ;
    virtual inline void setMapDouble( MapDouble &v ) = 0 ;
    virtual inline void setMapVecDouble( MapVecDouble &v ) = 0 ;
#endif

    virtual StatusCode getInfo_Int(const std::string& varName, int& value) const = 0;
    virtual StatusCode getInfo_Double(const std::string& varName, double& value) const = 0;
    virtual StatusCode getInfo_VecDouble(const std::string& varName, std::vector<double>& value) const = 0;
    virtual StatusCode getInfo_String(const std::string& varName, std::string& value) const = 0;
    virtual StatusCode getInfo_VecString(const std::string& varName, std::vector<std::string>& value) const = 0;
  };
    
}
#endif //PANTAUALGS_ITOOL_INFORMATIONSTORE_H

