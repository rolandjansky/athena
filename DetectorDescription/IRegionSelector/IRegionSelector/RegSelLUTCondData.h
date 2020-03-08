/* emacs: this is -*- c++ -*- */
/**
 **   @file    RegSelLUTCondData.h        
 **                   
 **   @author  sutt
 **   @date    Tue  4 Feb 2020 15:25:00 CET
 **
 **   $Id: RegSelLUTCondData.h, v0.0   Tue  4 Feb 2020 15:25:00 CET sutt $
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/

#ifndef  REGSELLUTCONDDATA_H
#define  REGSELLUTCONDDATA_H

#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "RegSelLUT/RegSelSiLUT.h"
#include "IRegionSelector/RegSelCondData.h"

typedef RegSelCondData<RegSelSiLUT> RegSelLUTCondData;

CLASS_DEF ( RegSelLUTCondData, 244180796, 0 )
CONDCONT_DEF ( RegSelLUTCondData, 30547718 );

#endif // REGSELLUTCONDDATA_H
