/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCRDO_H
#define ZDCRDO_H

#include "CLIDSvc/CLASS_DEF.h"

/** @class ZdcRdo
 * @author W. H. Bell <W.Bell@cern.ch>
 * 
 * A class to store the RAW readout decoded from bytestream or filled from 
 * monte carlo.   
 */
class ZdcRdo {
public:
  ZdcRdo();

};

// obtained using clid -m ZdcRdo
CLASS_DEF( ZdcRdo , 164548392 , 1 )

#endif
