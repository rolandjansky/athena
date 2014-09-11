// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StoreClearedIncident.h,v 1.2 2008-09-10 04:07:14 ssnyder Exp $

/**
 * @file  StoreGate/StoreClearedIncident.h
 * @author scott snyder
 * @date Sep 2008
 * @brief Incident sent after a store is cleared.
 */


#ifndef STOREGATE_STORECLEAREDINCIDENT_H
#define STOREGATE_STORECLEAREDINCIDENT_H


#include "GaudiKernel/Incident.h"
class StoreGateSvc;


/**
 * @brief Incident sent after a store is cleared.
 */
class StoreClearedIncident
  : public Incident
{
public:
  /**
   * @brief Constructor.
   * @param sg       The store that was cleared.
   * @param source   the name of the service/algorithm firing
   * @param type     e.g. "StoreCleared"
   */
  StoreClearedIncident (StoreGateSvc* sg,
                        const std::string& source,
                        const std::string& type = "StoreCleared");

  /// Return the store that was cleared.
  StoreGateSvc* store() const;


private:
  /// The store that was cleared.
  StoreGateSvc* m_sg;
};


#endif // not STOREGATE_STORECLEAREDINCIDENT_H
