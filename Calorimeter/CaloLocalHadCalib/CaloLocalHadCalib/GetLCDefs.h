/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOLOCALHADCALIB_GETLCDEFS_H
#define CALOLOCALHADCALIB_GETLCDEFS_H

/**
 * @namespace GetLCDefs
 * @version \$Id: GetLCDefs.h 1 2010-03-04 08:56:11 menke $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 8-March-2010
 * @brief namespace for enums releavant in the extraction of LC constants
 *
 * This names space collects enums and other definitions relevant for the
 * extraction of local hadron calibration constants. */

namespace GetLCDefs {

  enum NormalizationTypes {
    LIN = 0,LOG,CONST,NCLUS,Unknown
  };

  enum ClassificationTypes {
    NONE = 0,PARTICLEID_EM=1, PARTICLEID_HAD=2
  };

}
#endif // CALOLOCALHADCALIB_GETLCDEFS_H
