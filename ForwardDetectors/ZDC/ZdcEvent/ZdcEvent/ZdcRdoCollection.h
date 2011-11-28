/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCRDOCOLLECTION_H
#define ZDCRDOCOLLECTION_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "ZdcEvent/ZdcRdo.h"

/** @class ZdcRdoCollection
 * @author W. H. Bell <W.Bell@cern.ch>
 * 
 * @brief A collection of ZdcRdo from the readout of all ZDC modules.
 *
 */
class ZdcRdoCollection : public DataVector<ZdcRdo> { }; 

// obtained using clid -m ZdcRdoCollection
CLASS_DEF( ZdcRdoCollection , 1218480446 , 1 )

#endif
