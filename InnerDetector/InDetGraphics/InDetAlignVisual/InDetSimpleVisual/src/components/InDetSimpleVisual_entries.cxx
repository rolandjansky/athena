#include "InDetSimpleVisual/GetDetectorPositions.h"
#include "InDetSimpleVisual/GetDetectorLocalFrames.h"
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_ALGORITHM_FACTORY( GetDetectorPositions )
DECLARE_ALGORITHM_FACTORY( GetDetectorLocalFrames )
  
DECLARE_FACTORY_ENTRIES( InDetSimpleVisual ) 
{
  DECLARE_ALGORITHM( GetDetectorPositions ) ;
  DECLARE_ALGORITHM( GetDetectorLocalFrames ) ;
}




