/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCALIBDEFINETYPES_H
#define CALOCALIBDEFINETYPES_H

/** A header file to define the CLID for a few types put into Storegate by CaloCalibHitRec/CaloCalibClusterTruthMapMakerAlgorithm */

#include "AthenaKernel/CLASS_DEF.h"

#include "Identifier/Identifier.h"

//EDM Classes
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "xAODTruth/TruthParticle.h"

//C++ classes
#include <map>
#include <vector>

/* Here are the definitions. We use CLASS_DEF2 for these types, which have a comma in the name */
CLASS_DEF2(std::map<Identifier,std::vector<const CaloCalibrationHit*> >,11518007,1)
CLASS_DEF2(std::map<unsigned int,const xAOD::TruthParticle* >,256282227,1)
#endif
