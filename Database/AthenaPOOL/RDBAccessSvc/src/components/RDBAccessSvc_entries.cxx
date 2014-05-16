/**
 * @file RDBAccessSvc_entries.cxx
 *
 * @brief Required by Gaudi class factory
 *
 * @author Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
 *
 * $Id: RDBAccessSvc_entries.cxx,v 1.4 2005-07-22 15:56:45 obreshko Exp $
 */

#include "../RDBAccessSvc.h"
#include "../SourceCompAlg.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_SERVICE_FACTORY(RDBAccessSvc)
DECLARE_ALGORITHM_FACTORY(SourceCompAlg)

DECLARE_FACTORY_ENTRIES(RDBAccessSvc) {
    DECLARE_SERVICE   ( RDBAccessSvc );
    DECLARE_ALGORITHM ( SourceCompAlg );
}

