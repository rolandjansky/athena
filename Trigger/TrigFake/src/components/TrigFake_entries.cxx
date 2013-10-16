#include "TrigFake/FakeLvl1RoIatFixedEtaPhi.h"
#include "TrigFake/FakeLvl1MultipleRoIsatFixedEtaPhi.h"
//#include "TrigFake/FakeLvl1ConversionTestBeam.h"
#include "TrigFake/FakeLvl1RoIfromKine.h"
#include "TrigFake/ZVertexFromKine.h"
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_TOOL_FACTORY( FakeLvl1RoIfromKine )
  //DECLARE_ALGORITHM_FACTORY( FakeLvl1ConversionTestBeam )
DECLARE_TOOL_FACTORY( FakeLvl1RoIatFixedEtaPhi )
DECLARE_TOOL_FACTORY( FakeLvl1MultipleRoIsatFixedEtaPhi )
DECLARE_ALGORITHM_FACTORY( ZVertexFromKine )

DECLARE_FACTORY_ENTRIES(TrigFake) {
    DECLARE_TOOL( FakeLvl1RoIfromKine )
      //DECLARE_ALGORITHM( FakeLvl1ConversionTestBeam )
    DECLARE_TOOL( FakeLvl1RoIatFixedEtaPhi )
    DECLARE_TOOL( FakeLvl1MultipleRoIsatFixedEtaPhi )
    DECLARE_ALGORITHM( ZVertexFromKine )
}
