#include "InDetAlignGenAlgs/SiDistWriteAlg.h"
#include "InDetAlignGenAlgs/InDetAlignCog.h"
#include "InDetAlignGenAlgs/AddTRTMomConstr.h"
#include "InDetAlignGenAlgs/CreateMisalignAlg.h"
#include "InDetAlignGenAlgs/InDetAlignWrt.h"


DECLARE_ALGORITHM_FACTORY( SiDistWriteAlg )
DECLARE_ALGORITHM_FACTORY( InDetAlignCog )
DECLARE_ALGORITHM_FACTORY( AddTRTMomConstr )
DECLARE_ALGORITHM_FACTORY( InDetAlignWrt )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDetAlignment, CreateMisalignAlg )

