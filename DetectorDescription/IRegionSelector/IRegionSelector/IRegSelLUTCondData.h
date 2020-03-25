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

#ifndef  IREGSELLUTCONDDATA_H
#define  IREGSELLUTCONDDATA_H

#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "IRegionSelector/IRegSelLUT.h"
#include "IRegionSelector/RegSelCondData.h"

typedef RegSelCondData<IRegSelLUT> IRegSelLUTCondData;

CLASS_DEF ( IRegSelLUTCondData, 156521008, 0 )
CONDCONT_DEF ( IRegSelLUTCondData, 23768744 );


#endif // IREGSELLUTCONDDATA_H
