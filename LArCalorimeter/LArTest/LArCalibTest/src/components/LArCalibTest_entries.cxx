#include "LArCalibTest/LArCablingChecker.h"
#include "LArCalibTest/LArFakeCellFactor.h"
#include "LArCalibTest/LArReadHadDMCoeffFile.h"
#include "LArCalibTest/LArReadHadDMCoeffFile2.h"
#include "LArCalibTest/DumpCaloBadChannels.h"
#include "GaudiKernel/DeclareFactoryEntries.h"



DECLARE_ALGORITHM_FACTORY( LArCablingChecker )
DECLARE_ALGORITHM_FACTORY( LArReadHadDMCoeffFile )
DECLARE_ALGORITHM_FACTORY( LArReadHadDMCoeffFile2 )
DECLARE_ALGORITHM_FACTORY( LArFakeCellFactor )
DECLARE_ALGORITHM_FACTORY( DumpCaloBadChannels )


DECLARE_FACTORY_ENTRIES(LArCalibTest) 
{
  DECLARE_ALGORITHM( LArCablingChecker )
  DECLARE_ALGORITHM( LArReadHadDMCoeffFile );
  DECLARE_ALGORITHM( LArReadHadDMCoeffFile2 );
  DECLARE_ALGORITHM( LArFakeCellFactor )
  DECLARE_ALGORITHM( DumpCaloBadChannels )
}
