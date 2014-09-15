#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MdtCalibValidation/MdtValidationAlg.h"
 
using namespace MuonCalib; 

DECLARE_ALGORITHM_FACTORY ( MdtValidationAlg )
 
DECLARE_FACTORY_ENTRIES(MdtValidationAlg) {
  DECLARE_ALGORITHM(MdtValidationAlg)
}
