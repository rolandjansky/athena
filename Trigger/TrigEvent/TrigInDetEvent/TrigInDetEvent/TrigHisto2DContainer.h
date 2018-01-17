/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_TRIGHISTO2DCONTAINER_H
#define TRIGINDETEVENT_TRIGHISTO2DCONTAINER_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "TrigInDetEvent/TrigHisto2D.h"
#include "AthenaKernel/BaseInfo.h"

/** @class TrigHistoContainer
 * @author W. H. Bell <W.Bell@cern.ch>
 * 
 * @brief Container of TrigHisto2D classes.
 *
 */

class TrigHisto2DContainer : public DataVector<TrigHisto2D> { }; 

// obtained using clid -m TrigHisto2DContainer
CLASS_DEF( TrigHisto2DContainer , 1261809248 , 1 )

SG_BASE(TrigHisto2DContainer, DataVector<TrigHisto2D>);

#endif 
