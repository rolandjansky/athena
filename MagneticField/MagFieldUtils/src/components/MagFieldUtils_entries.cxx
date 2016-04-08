#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MagFieldUtils/MagFieldTestbedAlg.h"
#include "MagFieldUtils/SolenoidTest.h"
#include "MagFieldUtils/IdentityManipulator.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( MagField , MagFieldTestbedAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( MagField , SolenoidTest )
DECLARE_NAMESPACE_TOOL_FACTORY( MagField , IdentityManipulator )

DECLARE_FACTORY_ENTRIES( MagFieldUtils ) {
  DECLARE_NAMESPACE_ALGORITHM( MagField ,  MagFieldTestbedAlg )
  DECLARE_NAMESPACE_ALGORITHM( MagField ,  SolenoidTest )
  DECLARE_NAMESPACE_TOOL( MagField ,  IdentityManipulator )
}   
