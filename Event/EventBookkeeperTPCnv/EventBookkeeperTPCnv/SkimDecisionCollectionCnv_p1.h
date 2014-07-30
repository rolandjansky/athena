/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SKIMDECISIONCOLLECTIONCNV_P1_H
#define SKIMDECISIONCOLLECTIONCNV_P1_H

///////////////////////////////////////////////////////////////////////////
//  Header file for class SkimDecisionCollectionCnv_p1
//  Author: David Cote, September 2008. <david.cote@cern.ch>
///////////////////////////////////////////////////////////////////////////

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "EventBookkeeperMetaData/SkimDecisionCollection.h"
#include "EventBookkeeperTPCnv/SkimDecisionCollection_p1.h"
#include "SkimDecisionCnv_p1.h"


typedef T_AthenaPoolTPCnvVector< SkimDecisionCollection, SkimDecisionCollection_p1, SkimDecisionCnv_p1 >  SkimDecisionCollectionCnv_p1;

template<>
class T_TPCnv<SkimDecisionCollection, SkimDecisionCollection_p1>
  : public SkimDecisionCollectionCnv_p1
{
public:
};

#endif  //SKIMDECISIONCOLLECTIONCNV_P1_H
