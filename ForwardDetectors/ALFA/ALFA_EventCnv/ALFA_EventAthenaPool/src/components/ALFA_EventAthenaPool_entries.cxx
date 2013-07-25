#include "src/ALFA_HitCollectionCnv.h"
#include "src/ALFA_DigitCollectionCnv.h"
#include "src/ALFA_ODHitCollectionCnv.h"
#include "src/ALFA_ODDigitCollectionCnv.h"
#include "src/ALFA_LocRecEvCollectionCnv.h"
#include "src/ALFA_LocRecODEvCollectionCnv.h"
#include "src/ALFA_RawDataContainerCnv.h"
#include "src/ALFA_RawDataContainerCnv_charge.h"
#include "src/ALFA_GloRecEvCollectionCnv.h"
#include "src/ALFA_CLinkEventCnv.h"

DECLARE_ALGORITHM_FACTORY( ALFA_HitCollectionCnv )
DECLARE_ALGORITHM_FACTORY( ALFA_DigitCollectionCnv )
DECLARE_ALGORITHM_FACTORY( ALFA_ODHitCollectionCnv )
DECLARE_ALGORITHM_FACTORY( ALFA_ODDigitCollectionCnv )
DECLARE_ALGORITHM_FACTORY( ALFA_RawDataContainerCnv )
DECLARE_ALGORITHM_FACTORY( ALFA_RawDataContainerCnv_charge )
DECLARE_ALGORITHM_FACTORY( ALFA_LocRecEvCollectionCnv )
DECLARE_ALGORITHM_FACTORY( ALFA_LocRecODEvCollectionCnv )
DECLARE_ALGORITHM_FACTORY( ALFA_GloRecEvCollectionCnv )
DECLARE_ALGORITHM_FACTORY( ALFA_CLinkEventCnv )

DECLARE_FACTORY_ENTRIES(ALFA_EventAthenaPool) {

DECLARE_ALGORITHM( ALFA_HitCollectionCnv )
DECLARE_ALGORITHM( ALFA_DigitCollectionCnv )
DECLARE_ALGORITHM( ALFA_ODHitCollectionCnv )
DECLARE_ALGORITHM( ALFA_ODDigitCollectionCnv )
DECLARE_ALGORITHM( ALFA_RawDataContainerCnv )
DECLARE_ALGORITHM( ALFA_RawDataContainerCnv_charge )
DECLARE_ALGORITHM( ALFA_LocRecEvCollectionCnv )
DECLARE_ALGORITHM( ALFA_LocRecODEvCollectionCnv )
DECLARE_ALGORITHM( ALFA_GloRecEvCollectionCnv )
DECLARE_ALGORITHM( ALFA_CLinkEventCnv )

}

