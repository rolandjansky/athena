/* emacs: this is -*- c++ -*- */
/**
 **   @file    RegSelCaloLUTCondData.h        
 **                   
 **   @author  sutt
 **   @date    Tue  4 Feb 2020 15:25:00 CET
 **
 **   $Id: RegSelCaloLUTCondData.h, v0.0   Sat 21 March 2020 15:25:00 CET sutt $
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/

#ifndef  REGSELCALOLUTCONDDATA_H
#define  REGSELCALOLUTCONDDATA_H

#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "IRegionSelector/RegSelCondData.h"
#include "RegionSelector/RegSelectorHashMap.h"

typedef RegSelCondData<RegSelectorHashMap> RegSelCaloLUTCondData;

CLASS_DEF ( RegSelCaloLUTCondData, 232357481, 0 )
CONDCONT_DEF ( RegSelCaloLUTCondData, 198043569 );



#endif // REGSELCALOLUTCONDDATA_H
