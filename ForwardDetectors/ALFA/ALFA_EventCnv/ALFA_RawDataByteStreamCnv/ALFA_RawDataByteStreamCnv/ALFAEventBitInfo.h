/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFAEVENTCNV_ALFAEVENTBITINFO_H
#define ALFAEVENTCNV_ALFAEVENTBITINFO_H

 namespace ALFAEventBitInfo {
   enum   ALFAFLAGREASON {
     ROB_ERROR = 1 		///< ROB status error
     ,CORRUPTION = 2 		///< data corrupted
     ,COLL_NOT_FOUND = 3 		///< collection not found
   };
 }
#endif
