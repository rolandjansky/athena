/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: IdScanSpPoint.h
// 
// author: Nikos Konstantinidis
//         nk@hep.ucl.ac.uk
//		 
// 
// Description: - Internal IdScan space point
// 
// date: 24/07/2003
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#if ! defined( IDSCAN_IDSCANSPPOINT_H )
#define IDSCAN_IDSCANSPPOINT_H

#include <vector>
#include <iostream>

#include "IDScanHitFilter/tIdScanSpPoint.h"
#include "TrigInDetEvent/TrigSiSpacePoint.h"


typedef TrigSiSpacePoint ExternalSpacePoint;

typedef tIdScanSpPoint<ExternalSpacePoint>   IdScanSpPoint;


typedef std::vector< IdScanSpPoint* >  hitVec;
typedef torderInRho<IdScanSpPoint>     orderInRho;
typedef torderInLayer<IdScanSpPoint>   orderInLayer;
typedef torderInIndex<IdScanSpPoint>   orderInIndex;
typedef tequalIds<IdScanSpPoint>       equalIds;


#endif

