/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StoreClearedIncident.cxx,v 1.2 2008-09-10 04:07:14 ssnyder Exp $
/**
 * @file  StoreGate/src/StoreClearedIncident.cxx
 * @author scott snyder
 * @date Sep 2008
 * @brief Incident sent after a store is cleared.
 */


#include "StoreGate/StoreClearedIncident.h"


/**
 * @brief Constructor.
 * @param sg       The store that was cleared.
 * @param source   the name of the service/algorithm firing
 * @param type     e.g. "StoreCleared"
 */
StoreClearedIncident::StoreClearedIncident (StoreGateSvc* sg,
                      const std::string& source,
                      const std::string& type /*= "StoreCleared"*/)
  : Incident (source, type),
    m_sg (sg)
{
}


/// Return the store that was cleared.
StoreGateSvc* StoreClearedIncident::store() const
{
  return m_sg;
}

