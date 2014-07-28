/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/TriggerMap.h"

MuonFeatureDetails::AlgoId AlgoMap(const std::string& name)
{
    if      (name == "muFast_900GeV")  return MuonFeatureDetails::GEV900ID;
    else if (name == "muFast_Muon")    return MuonFeatureDetails::MUONID;
    else if (name.find("muFast_OutBCID") != std::string::npos ) return MuonFeatureDetails::HALOID;
    else if (name == "muFast_Cosmic")  return MuonFeatureDetails::COSMICID;
    
    return MuonFeatureDetails::NULLID;
}
