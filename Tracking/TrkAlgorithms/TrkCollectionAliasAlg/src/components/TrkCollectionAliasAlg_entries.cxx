#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkCollectionAliasAlg/TrkCollectionAliasAlg.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk , TrkCollectionAliasAlg )

DECLARE_FACTORY_ENTRIES( TrkCollectionAliasAlg ) {
  DECLARE_NAMESPACE_ALGORITHM( Trk ,  TrkCollectionAliasAlg )
}   
