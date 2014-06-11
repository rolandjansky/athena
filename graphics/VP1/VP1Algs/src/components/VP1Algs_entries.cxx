//    NOTE:
//    VP1EventProd has been moved to a separate package: VP1AlgsEventProd
//    It has been needed for VP1 Live to decouple VP1EventProd from openGl, which was needed by VP1Gui
//    22 May Riccardo-Maria BIANCHI <rbianchi@cern.ch>


#include "GaudiKernel/DeclareFactoryEntries.h"

#include "VP1Algs/VP1Alg.h"
//#include "VP1Algs/VP1EventProd.h"

DECLARE_ALGORITHM_FACTORY(VP1Alg)
//DECLARE_ALGORITHM_FACTORY(VP1EventProd)

DECLARE_FACTORY_ENTRIES(VP1Algs) {
  DECLARE_ALGORITHM( VP1Alg );
  //DECLARE_ALGORITHM( VP1EventProd );
}
