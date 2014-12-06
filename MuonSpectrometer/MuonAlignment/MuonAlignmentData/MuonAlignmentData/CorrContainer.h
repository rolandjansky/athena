/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTDATA_CORRCONTAINER_H
#define MUONALIGNMENTDATA_CORRCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonAlignmentData/ALinePar.h"
#include "MuonAlignmentData/BLinePar.h"
#include "MuonAlignmentData/CscInternalAlignmentPar.h"
#include "MuonAlignmentData/MdtAsBuiltParams.h"
 
typedef std::map<Identifier,ALinePar*> ALineMapContainer;
CLASS_DEF(ALineMapContainer,1324994866,1)

typedef std::map<Identifier,BLinePar*> BLineMapContainer;
CLASS_DEF(BLineMapContainer,1124991377,1)

typedef std::map<Identifier,CscInternalAlignmentPar*> CscInternalAlignmentMapContainer;
CLASS_DEF(CscInternalAlignmentMapContainer,1285567354,1)

typedef std::map<Identifier,MdtAsBuiltParams*> MdtAsBuiltParMapContainer;

typedef std::map<Identifier,ALinePar*>::iterator iALineMap;
typedef std::map<Identifier,BLinePar*>::iterator iBLineMap;
typedef std::map<Identifier,CscInternalAlignmentPar*>::iterator iCscInternalAlignmentMap;
typedef std::map<Identifier,MdtAsBuiltParams*>::iterator        iMdtAsBuiltParMapContainer;


#endif // MUONALIGNMENTDATA_ALINEPARCONTAINER_H
