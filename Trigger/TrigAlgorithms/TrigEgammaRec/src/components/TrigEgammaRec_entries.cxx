#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigEgammaRec/TrigEgammaRec.h"

//Algoritmos
DECLARE_ALGORITHM_FACTORY(TrigEgammaRec)
//Libreria
DECLARE_FACTORY_ENTRIES(TrigEgammaRec)
{
//Repites algoritmos
  DECLARE_ALGORITHM(TrigEgammaRec);
}

