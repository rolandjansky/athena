/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTruth/versions/TruthMetaData_v1.h"

namespace xAOD {

   TruthMetaData_v1::TruthMetaData_v1(): SG::AuxElement() {}

   //Arguments for MACRO are as follows: AUXSTORE_OBJECT/PRIMITIVE_SETTER_AND_GETTER(CL, TYPE, NAME, SETTER)
   AUXSTORE_OBJECT_SETTER_AND_GETTER(TruthMetaData_v1, std::vector < std::string >, weightNames, setWeightNames )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TruthMetaData_v1, uint32_t, mcChannelNumber, setMcChannelNumber )
   AUXSTORE_OBJECT_SETTER_AND_GETTER(TruthMetaData_v1, std::string, lhefGenerator, setLhefGenerator )
   AUXSTORE_OBJECT_SETTER_AND_GETTER(TruthMetaData_v1, std::string, generators, setGenerators )
   AUXSTORE_OBJECT_SETTER_AND_GETTER(TruthMetaData_v1, std::string, evgenProcess, setEvgenProcess )
   AUXSTORE_OBJECT_SETTER_AND_GETTER(TruthMetaData_v1, std::string, evgenTune, setEvgenTune )
   AUXSTORE_OBJECT_SETTER_AND_GETTER(TruthMetaData_v1, std::string, hardPDF, setHardPDF )
   AUXSTORE_OBJECT_SETTER_AND_GETTER(TruthMetaData_v1, std::string, softPDF, setSoftPDF )
}
 // 
