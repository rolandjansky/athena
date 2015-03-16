/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file TauDetailsContainer.h
///
///Declaration of tau details container transient class
///
///Package: Reconstruction/tauEvent
///
///@author Thomas Burgess <tburgess-at-cern.ch>
///@author Lukasz Janyst
///
/// Created 2007-01-16.
///
/// $Id: TauDetailsContainer.h,v 1.4 2009-03-09 20:28:47 tburgess Exp $

#ifndef TAU_DETAILS_CONTAINER_ANALYSIS_H
#define TAU_DETAILS_CONTAINER_ANALYSIS_H

#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "tauEvent/TauDetails.h"

namespace Analysis
{
    //-------------------------------------------------------------------------
    // this must be new class for persistency reasons
    //-------------------------------------------------------------------------
    class TauDetailsContainer: public DataVector<TauDetails> 
    {
    };
}

CLASS_DEF( Analysis::TauDetailsContainer, 563909202, 1 )

#endif // TAU_DETAILS_CONTAINER_ANALYSIS
