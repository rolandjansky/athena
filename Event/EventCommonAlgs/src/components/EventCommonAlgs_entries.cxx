#include "EventCommonAlgs/INav4MomDumper.h"
#include "../I4MomDumper.h"
//#include "../INav4MomSymLinkBuilder.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
  
DECLARE_ALGORITHM_FACTORY( I4MomDumper  )
DECLARE_ALGORITHM_FACTORY( INav4MomDumper  )
//DECLARE_ALGORITHM_FACTORY( INav4MomSymLinkBuilder  )

DECLARE_FACTORY_ENTRIES( EventCommonAlgs ) {
  DECLARE_ALGORITHM( I4MomDumper )
  DECLARE_ALGORITHM( INav4MomDumper )
    //DECLARE_ALGORITHM( INav4MomSymLinkBuilder )
}
