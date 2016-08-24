/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventInfoMgt_entries.cxx
 *
 * @brief Declare a gaudi service in EventInfoMgr component
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: EventInfoMgt_entries.cxx,v 1.5 2007-01-03 09:44:09 dquarrie Exp $
 */
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TagInfoMgr.h"

DECLARE_SERVICE_FACTORY(TagInfoMgr)


DECLARE_FACTORY_ENTRIES(EventInfoMgt) {
  DECLARE_SERVICE(TagInfoMgr)

    

}
