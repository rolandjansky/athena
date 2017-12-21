#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigEgammaRec/TrigEgammaRec.h"
#include "TrigEgammaRec/TrigTopoEgammaBuilder.h"

//Algoritmos
DECLARE_ALGORITHM_FACTORY(TrigEgammaRec)
DECLARE_ALGORITHM_FACTORY(TrigTopoEgammaBuilder)

//Libreria
DECLARE_FACTORY_ENTRIES(TrigEgammaRec)
{
//Repites algoritmos
  DECLARE_ALGORITHM(TrigEgammaRec);
  DECLARE_ALGORITHM(TrigTopoEgammaBuilder);
}
