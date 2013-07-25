#include "src/ALFA_HitCollectionCnv.h"
#include "src/ALFA_DigitCollectionCnv.h"
#include "src/ALFA_ODHitCollectionCnv.h"

DECLARE_ALGORITHM_FACTORY( ALFA_HitCollectionCnv )
DECLARE_ALGORITHM_FACTORY( ALFA_DigitCollectionCnv )
DECLARE_ALGORITHM_FACTORY( ALFA_ODHitCollectionCnv )

DECLARE_FACTORY_ENTRIES(ALFA_EventAthenaPool) {

DECLARE_ALGORITHM( ALFA_HitCollectionCnv )
DECLARE_ALGORITHM( ALFA_DigitCollectionCnv )
DECLARE_ALGORITHM( ALFA_ODHitCollectionCnv )

}

