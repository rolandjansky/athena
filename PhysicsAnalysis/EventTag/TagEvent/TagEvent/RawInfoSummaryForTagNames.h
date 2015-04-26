/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_GLOBALEVENTATTRIBUTENAMES_H
#define EVENTTAGALGS_GLOBALEVENTATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the RawSummary attributes */
namespace Raw { 

  static const unsigned int NAttr = 22;    // This is the number of the RawSummary TAG attributes
  static const unsigned int NFloats = 16;
}

static const AthenaAttributeSpecification RawSummaryAttributeSpecs[Raw::NAttr] = 
{
   AthenaAttributeSpecification("CellEnergySum","float",    GroupNames[Grp::RawSummary],UnitNames[Unt::Mev]),
   AthenaAttributeSpecification("CellEnergySumEMB","float", GroupNames[Grp::RawSummary],UnitNames[Unt::Mev]),
   AthenaAttributeSpecification("CellEnergySumEMEC","float",GroupNames[Grp::RawSummary],UnitNames[Unt::Mev]),
   AthenaAttributeSpecification("CellEnergySumHEC","float", GroupNames[Grp::RawSummary],UnitNames[Unt::Mev]),
   AthenaAttributeSpecification("CellEnergySumFCAL","float",GroupNames[Grp::RawSummary],UnitNames[Unt::Mev]),
   AthenaAttributeSpecification("CellEnergySumTile","float",GroupNames[Grp::RawSummary],UnitNames[Unt::Mev]),
   AthenaAttributeSpecification("ClusterEnergySum","float", GroupNames[Grp::RawSummary],UnitNames[Unt::Mev]),
   AthenaAttributeSpecification("TopoClusterEt1","float",    GroupNames[Grp::RawSummary],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("TopoClusterEta1","float",   GroupNames[Grp::RawSummary],UnitNames[Unt::Rap]),
   AthenaAttributeSpecification("TopoClusterPhi1","float",   GroupNames[Grp::RawSummary],UnitNames[Unt::Rad]),
   AthenaAttributeSpecification("CellMissingET","float",      GroupNames[Grp::RawSummary],UnitNames[Unt::Mev]),
   AthenaAttributeSpecification("CellMissingETPhi","float",   GroupNames[Grp::RawSummary],UnitNames[Unt::Rad]),
   AthenaAttributeSpecification("MBTStimeDiff","float", GroupNames[Grp::RawSummary],UnitNames[Unt::nanosec]),
   AthenaAttributeSpecification("LArECtimeDiff","float",GroupNames[Grp::RawSummary],UnitNames[Unt::nanosec]),
   AthenaAttributeSpecification("MBTStimeAvg","float", GroupNames[Grp::RawSummary],UnitNames[Unt::nanosec]),
   AthenaAttributeSpecification("LArECtimeAvg","float",GroupNames[Grp::RawSummary],UnitNames[Unt::nanosec]),
   AthenaAttributeSpecification("NSCTSPs","int",  GroupNames[Grp::RawSummary],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("NPixelSPs","int",  GroupNames[Grp::RawSummary],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("NTRTDCs","int",  GroupNames[Grp::RawSummary],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("NTRTHtDCs","int",GroupNames[Grp::RawSummary],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("NBCMHits","int", GroupNames[Grp::RawSummary],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("MBTSword","unsigned int",GroupNames[Grp::RawSummary],UnitNames[Unt::Bitmask])
};


#endif
