/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTDATA_CORRCONTAINER_H
#define MUONALIGNMENTDATA_CORRCONTAINER_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
#include "MuonAlignmentData/ALinePar.h"
#include "MuonAlignmentData/BLinePar.h"
#include "MuonAlignmentData/CscInternalAlignmentPar.h"
#include "MuonAlignmentData/MdtAsBuiltPar.h"
#include "MuonIdHelpers/MdtIdHelper.h"

typedef std::map<Identifier, ALinePar> ALineMapContainer;
CLASS_DEF(ALineMapContainer, 1324994866, 1)
CLASS_DEF(CondCont<ALineMapContainer>, 1201194425, 0)

typedef std::map<Identifier, BLinePar> BLineMapContainer;
CLASS_DEF(BLineMapContainer, 1124991377, 1)
CLASS_DEF(CondCont<BLineMapContainer>, 1332301344, 0)

typedef std::map<Identifier, CscInternalAlignmentPar> CscInternalAlignmentMapContainer;
CLASS_DEF(CscInternalAlignmentMapContainer, 1285567354, 1)
CLASS_DEF(CondCont<CscInternalAlignmentMapContainer>, 1227105862, 0)

typedef std::map<Identifier, MdtAsBuiltPar> MdtAsBuiltMapContainer;
CLASS_DEF(MdtAsBuiltMapContainer, 1198729422, 1)
CLASS_DEF(CondCont<MdtAsBuiltMapContainer>, 1076645826, 0)

typedef ALineMapContainer::iterator iALineMap;
typedef BLineMapContainer::iterator iBLineMap;
typedef CscInternalAlignmentMapContainer::iterator iCscInternalAlignmentMap;
typedef MdtAsBuiltMapContainer::iterator iMdtAsBuiltMap;

#endif  // MUONALIGNMENTDATA_ALINEPARCONTAINER_H
