#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MagFieldUtils/MagFieldTestbedAlg.h"
#include "MagFieldUtils/SolenoidTest.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( MagField , MagFieldTestbedAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( MagField , SolenoidTest )

DECLARE_FACTORY_ENTRIES( MagFieldUtils ) {
  DECLARE_NAMESPACE_ALGORITHM( MagField ,  MagFieldTestbedAlg )
  DECLARE_NAMESPACE_ALGORITHM( MagField ,  SolenoidTest )
}   
