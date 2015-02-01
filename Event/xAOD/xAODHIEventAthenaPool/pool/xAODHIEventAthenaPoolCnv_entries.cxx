/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ExamplePackageCnv_entries.cxx
 * @brief Declares component instantiation factories for an example POOL converter 
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * $Id: ExamplePackageCnv_entries.cxx,v 1.3 2005-01-12 20:46:47 kkarr Exp $
 */

#include "xAODHIEventAthenaPoolCnv_entries.h"

DECLARE_FACTORY_ENTRIES(xAODHIEventAthenaPoolPoolCnv) {

/**
 * @file ExamplePackageCnv_entries_element.cxx
 * @brief Declares component instantiation factories for a converter
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * $Id: ExamplePackageCnv_entries_element.cxx,v 1.3 2005-01-12 20:47:00 kkarr Exp $
 */

   DECLARE_CONVERTER(xAODHIEventShapeAuxContainerCnv);

/**
 * @file ExamplePackageCnv_entries_element.cxx
 * @brief Declares component instantiation factories for a converter
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * $Id: ExamplePackageCnv_entries_element.cxx,v 1.3 2005-01-12 20:47:00 kkarr Exp $
 */

   DECLARE_CONVERTER(xAODHIEventShapeCnv);

/**
 * @file ExamplePackageCnv_entries_element.cxx
 * @brief Declares component instantiation factories for a converter
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * $Id: ExamplePackageCnv_entries_element.cxx,v 1.3 2005-01-12 20:47:00 kkarr Exp $
 */

   DECLARE_CONVERTER(xAODHIEventShapeContainerCnv);

}
