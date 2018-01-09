/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_TRIGHISTO1DCONTAINER_H
#define TRIGINDETEVENT_TRIGHISTO1DCONTAINER_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "TrigInDetEvent/TrigHisto1D.h"
#include "AthenaKernel/BaseInfo.h"

/** @class TrigHistoContainer
 * @author W. H. Bell <W.Bell@cern.ch>
 * 
 * @brief Container of TrigHisto1D classes.
 *
 */

class TrigHisto1DContainer : public DataVector<TrigHisto1D> { }; 

// obtained using clid -m TrigHisto1DContainer
CLASS_DEF( TrigHisto1DContainer , 1218315511 , 1 )

SG_BASE(TrigHisto1DContainer, DataVector<TrigHisto1D>);

#endif 
