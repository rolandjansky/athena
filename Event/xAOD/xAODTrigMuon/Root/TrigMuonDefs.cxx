#include "xAODTrigMuon/TrigMuonDefs.h"
//#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <math.h>
#include <iostream>


namespace xAOD{

namespace L2MuonParameters{


  ECRegions whichECRegion( const float eta, const float phi ){
      float absEta = fabs(eta);
      //ATH_MSG_INFO("DEBUG:Esumi message"); 
      std::cout<<"Esumi message"<<std::endl;
      if( ( 1.3 <= absEta && absEta < 1.45) &&
	  ( (0                 <= fabs(phi) && fabs(phi) < CLHEP::pi/48. )     ||
	    (CLHEP::pi*11./48. <= fabs(phi) && fabs(phi) < CLHEP::pi*13./48. ) ||
	    (CLHEP::pi*23./48. <= fabs(phi) && fabs(phi) < CLHEP::pi*25./48. ) ||
	    (CLHEP::pi*35./48. <= fabs(phi) && fabs(phi) < CLHEP::pi*37./48. ) ||
	    (CLHEP::pi*47./48. <= fabs(phi) && fabs(phi) < CLHEP::pi )
	    )
	  ) return WeakBFieldA;
      
      else if( ( 1.5 <= absEta && absEta < 1.65 ) &&
	       ( (CLHEP::pi*3./32.  <= fabs(phi) && fabs(phi) < CLHEP::pi*5./32. ) ||
		 (CLHEP::pi*11./32. <= fabs(phi) && fabs(phi) < CLHEP::pi*13./32.) ||
		 (CLHEP::pi*19./32. <= fabs(phi) && fabs(phi) < CLHEP::pi*21./32.) ||
		 (CLHEP::pi*27./32. <= fabs(phi) && fabs(phi) < CLHEP::pi*29./32.)
		 )
	       ) return WeakBFieldB;
      
      else return Bulk;
    }

}

}

