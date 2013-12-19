/**
 * @file RegistrationServices_entries.cxx
 *
 * @brief Declaration of component entry points
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Kristo Karr <kkarr@hep.anl.gov>
 *
 * $Id: RegistrationServices_entries.cxx,v 1.16 2009-04-28 19:04:07 cranshaw Exp $
 *
 */

#include <stdint.h>
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../OutputConditionsAlg.h"
#include "../InputCollectionMetadataCopy.h"
#include "../RegistrationStream.h"
#include "../RegistrationStreamDefMeta.h"
#include "../RegistrationStreamLCGTool.h"
#include "../RegistrationStreamTagTool.h"

DECLARE_ALGORITHM_FACTORY(RegistrationStream)
DECLARE_ALGORITHM_FACTORY(RegistrationStreamDefMeta)
DECLARE_ALGORITHM_FACTORY(OutputConditionsAlg)
DECLARE_TOOL_FACTORY(InputCollectionMetadataCopy)
DECLARE_TOOL_FACTORY(RegistrationStreamLCGTool)
DECLARE_TOOL_FACTORY(RegistrationStreamTagTool)

DECLARE_FACTORY_ENTRIES(RegistrationServices) {
  DECLARE_ALGORITHM(RegistrationStream)
  DECLARE_ALGORITHM(RegistrationStreamDefMeta)
  DECLARE_ALGORITHM(OutputConditionsAlg)
  DECLARE_TOOL(RegistrationStreamLCGTool)
  DECLARE_TOOL(RegistrationStreamTagTool)
}
