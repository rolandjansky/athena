// $Id: DataModelTestDataWrite_entries.cxx,v 1.2 2006-04-07 18:34:15 ssnyder Exp $
/**
 * @file  src/components/DataModelTestDataWrite_entries.cxx
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Gaudi algorithm factory declarations.
 */

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../DMTestWrite.h"
#include "../AuxDataTestWrite.h"
#include "../xAODTestWrite.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, DMTestWrite)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, AuxDataTestWrite)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestWrite)

DECLARE_FACTORY_ENTRIES(DataModelTestDataWrite) {
  DECLARE_NAMESPACE_ALGORITHM(DMTest, DMTestWrite);
  DECLARE_NAMESPACE_ALGORITHM(DMTest, AuxDataTestWrite);
  DECLARE_NAMESPACE_ALGORITHM(DMTest, xAODTestWrite);
}
