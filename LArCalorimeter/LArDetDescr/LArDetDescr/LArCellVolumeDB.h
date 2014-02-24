/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDETDESCR_LARCELLVOLUMEDB_H 
#define LARDETDESCR_LARCELLVOLUMEDB_H 

#include <vector>
#include "Identifier/Identifier.h"

/** c-struct containing the LArCell volume information : 

    thanks to the phi symmetry, in em and hec there is 1
    volume per eta bin ( see class LArCellVolumes.h , where this
    class is used )

 * @author Hong Ma
 * @version  0-0-1 , Feb 25, 2004 

*/


// persistent c-struct
struct LArCellVolumeDB_t{
  Identifier    db_channelID; // offline comp_id 
  float         db_volume   ; // in mm*3 
};

typedef std::vector<LArCellVolumeDB_t> LArCellVolumeDB;

#ifndef __CINT__
#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(LArCellVolumeDB, 34482670 ,1)
#endif

#endif
