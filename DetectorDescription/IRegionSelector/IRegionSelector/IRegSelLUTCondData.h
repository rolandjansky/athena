/* emacs: this is -*- c++ -*- */
/**
 **   @file    IRegSelLUTCondData.h        
 **                   
 **   @author  sutt
 **   @date    Wed  25 Feb 2020 15:25:00 CET
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
