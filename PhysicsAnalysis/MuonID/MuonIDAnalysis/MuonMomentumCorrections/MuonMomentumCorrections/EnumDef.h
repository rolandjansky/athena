/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ENUMDEF_H
#define ENUMDEF_H


namespace MCP {

    // This is for SagittaCorrection, there are two different types of error terms
    enum class TrackType{ CB, ID, ME }; 

    // This is for SagittaCorrection, there are two different types of error terms
    enum class SagittaCorrection{ Nominal, Datastat__1up, Residual__1up }; 

    // This for the calibration constants of the Scale and Resolution Corrections, s0/s1/r0/r1/r2
    enum class ScaleResCorrection{ Nominal, SystErr__1up, SystErr__1down }; 
    
    // Params 
    enum class ScaleSmearParam{ s0, s1, r0, r1, r2 }; 

    // Params 
    enum class ExpectedResParam{ r0, r1, r2, r2tan2 }; 

    // Data year
    enum class DataYear {Data16, Data17, Data18, Data22};

    // For HighPtSmearing
    enum class MST_Categories{Undefined, Zero, One, Two, Three, Four};
        
    // Data22 is not supported yet
    static constexpr std::array<MCP::DataYear, 4> dataYearList{MCP::DataYear::Data16, MCP::DataYear::Data17, MCP::DataYear::Data18, MCP::DataYear::Data22};

    // Need this defined as a simple enum to not change any interface class
    namespace DetectorType { enum { MS = 1, ID = 2, CB = 3 };}

    inline std::string toString(TrackType trkType) 
    {
        if(trkType == TrackType::CB) return "CB";
        if(trkType == TrackType::ID) return "ID";
        if(trkType == TrackType::ME) return "ME";

        return "";
    }

    inline std::string toString(ScaleSmearParam param) 
    {
        if(param == ScaleSmearParam::s0) return "s0";
        if(param == ScaleSmearParam::s1) return "s1";
        if(param == ScaleSmearParam::r0) return "r0";
        if(param == ScaleSmearParam::r1) return "r1";
        if(param == ScaleSmearParam::r2) return "r2";

        return "";
    }

    inline std::string toString(DataYear year) 
    {
        if(year == DataYear::Data16) return "Data16";
        if(year == DataYear::Data17) return "Data17";
        if(year == DataYear::Data18) return "Data18";
        if(year == DataYear::Data22) return "Data22";
        return "";
    }           

    // Need this defined as a simple enum to not change any interface class
    namespace SystVariation {
       enum { Default = 0, Down = -1, Up = 1 };
    }

};

#endif
