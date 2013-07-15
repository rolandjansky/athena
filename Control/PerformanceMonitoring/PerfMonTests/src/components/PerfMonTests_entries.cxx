#include "../PerfMonTestNoopAlg.h"
#include "../PerfMonTestLeakyAlg.h"
#include "../PerfMonTestBasicAlg.h"
#include "../PerfMonTestCpuCruncherAlg.h"
#include "../PerfMonTestMallocAlg.h"
#include "../PerfMonTestErroneousAlg.h"
#include "../PerfMonTestManyLeaksAlg.h"
#include "../PerfMonTestVectorAlg.h"
#include "../PerfMonTestPolyVectorAlg.h"
#include "../PerfMonTestPolyVectorAlgWithArenas.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
  
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PerfMonTest, NoopAlg  )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PerfMonTest, LeakyAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PerfMonTest, BasicAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PerfMonTest, CpuCruncherAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PerfMonTest, MallocAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PerfMonTest, ErroneousAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PerfMonTest, ManyLeaksAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PerfMonTest, VectorAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PerfMonTest, PolyVectorAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PerfMonTest, PolyVectorAlgWithArenas )

DECLARE_FACTORY_ENTRIES( PerfMonTests ) {
  DECLARE_NAMESPACE_ALGORITHM( PerfMonTest, NoopAlg  )
  DECLARE_NAMESPACE_ALGORITHM( PerfMonTest, LeakyAlg )
  DECLARE_NAMESPACE_ALGORITHM( PerfMonTest, BasicAlg )
  DECLARE_NAMESPACE_ALGORITHM( PerfMonTest, CpuCruncherAlg )
  DECLARE_NAMESPACE_ALGORITHM( PerfMonTest, MallocAlg )
  DECLARE_NAMESPACE_ALGORITHM( PerfMonTest, ErroneousAlg )
  DECLARE_NAMESPACE_ALGORITHM( PerfMonTest, ErroneousAlg )
  DECLARE_NAMESPACE_ALGORITHM( PerfMonTest, VectorAlg )
  DECLARE_NAMESPACE_ALGORITHM( PerfMonTest, PolyVectorAlg )
  DECLARE_NAMESPACE_ALGORITHM( PerfMonTest, PolyVectorAlgWithArenas )
}
