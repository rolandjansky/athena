/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s).
#include "xAODTrigMuon/TrigMuonDefs.h"

// System include(s).
#include <cmath>

namespace xAOD{

namespace L2MuonParameters{


  ECRegions whichECRegion( const float eta, const float phi ){
      float absEta = fabs(eta);
      if( ( 1.3 <= absEta && absEta < 1.45) &&
	  ( (0                 <= fabs(phi) && fabs(phi) < M_PI/48. )     ||
	    (M_PI*11./48. <= fabs(phi) && fabs(phi) < M_PI*13./48. ) ||
	    (M_PI*23./48. <= fabs(phi) && fabs(phi) < M_PI*25./48. ) ||
	    (M_PI*35./48. <= fabs(phi) && fabs(phi) < M_PI*37./48. ) ||
	    (M_PI*47./48. <= fabs(phi) && fabs(phi) < M_PI )
	    )
	  ) return WeakBFieldA;
      
      else if( ( 1.5 <= absEta && absEta < 1.65 ) &&
	       ( (M_PI*3./32.  <= fabs(phi) && fabs(phi) < M_PI*5./32. ) ||
		 (M_PI*11./32. <= fabs(phi) && fabs(phi) < M_PI*13./32.) ||
		 (M_PI*19./32. <= fabs(phi) && fabs(phi) < M_PI*21./32.) ||
		 (M_PI*27./32. <= fabs(phi) && fabs(phi) < M_PI*29./32.)
		 )
	       ) return WeakBFieldB;
      
      else return Bulk;
    }

}

}

