// $Id: DataModelTestDataRead_entries.cxx,v 1.2 2006-04-07 18:30:38 ssnyder Exp $
/**
 * @file  src/components/DataModelTestDataRead_entries.cxx
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Gaudi algorithm factory declarations.
 */

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../DMTestRead.h"
#include "../AuxDataTestRead.h"
#include "../AuxDataTestDecor.h"
#include "../AuxDataTestClearDecor.h"
#include "../AuxDataTestTypelessRead.h"
#include "../xAODTestRead.h"
#include "../xAODTestDecor.h"
#include "../xAODTestClearDecor.h"
#include "../xAODTestTypelessRead.h"
#include "../xAODTestShallowCopy.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, DMTestRead)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, AuxDataTestRead)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, AuxDataTestDecor)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, AuxDataTestClearDecor)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, AuxDataTestTypelessRead)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestRead)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestDecor)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestClearDecor)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestTypelessRead)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DMTest, xAODTestShallowCopy)

DECLARE_FACTORY_ENTRIES(DataModelTestDataRead) {
  DECLARE_NAMESPACE_ALGORITHM(DMTest, DMTestRead);
  DECLARE_NAMESPACE_ALGORITHM(DMTest, AuxDataTestRead);
  DECLARE_NAMESPACE_ALGORITHM(DMTest, AuxDataTestDecor);
  DECLARE_NAMESPACE_ALGORITHM(DMTest, AuxDataTestClearDecor);
  DECLARE_NAMESPACE_ALGORITHM(DMTest, AuxDataTestTypelessRead);
  DECLARE_NAMESPACE_ALGORITHM(DMTest, xAODTestRead);
  DECLARE_NAMESPACE_ALGORITHM(DMTest, xAODTestDecor);
  DECLARE_NAMESPACE_ALGORITHM(DMTest, xAODTestClearDecor);
  DECLARE_NAMESPACE_ALGORITHM(DMTest, xAODTestTypelessRead);
  DECLARE_NAMESPACE_ALGORITHM(DMTest, xAODTestShallowCopy);
}
