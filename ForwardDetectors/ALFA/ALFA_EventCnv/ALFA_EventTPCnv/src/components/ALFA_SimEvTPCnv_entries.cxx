#include "ALFA_EventTPCnv/ALFA_Hit_p1.h"
#include "ALFA_EventTPCnv/ALFA_HitCnv_p1.h"
#include "ALFA_EventTPCnv/ALFA_HitCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_HitCollectionCnv_p1.h"

#include "ALFA_EventTPCnv/ALFA_Digit_p1.h"
#include "ALFA_EventTPCnv/ALFA_DigitCnv_p1.h"
#include "ALFA_EventTPCnv/ALFA_DigitCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_DigitCollectionCnv_p1.h"

#include "ALFA_EventTPCnv/ALFA_ODHit_p1.h"
#include "ALFA_EventTPCnv/ALFA_ODHitCnv_p1.h"
#include "ALFA_EventTPCnv/ALFA_ODHitCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_ODHitCollectionCnv_p1.h"

#include "ALFA_EventTPCnv/ALFA_ODDigit_p1.h"
#include "ALFA_EventTPCnv/ALFA_ODDigitCnv_p1.h"
#include "ALFA_EventTPCnv/ALFA_ODDigitCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_ODDigitCollectionCnv_p1.h"

#include "ALFA_EventTPCnv/ALFA_RawData_p1.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCnv_p1.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCollectionCnv_p1.h"
#include "ALFA_EventTPCnv/ALFA_RawDataContainer_p1.h"
#include "ALFA_EventTPCnv/ALFA_RawDataContainerCnv_p1.h"

#include "ALFA_EventTPCnv/ALFA_RawData_charge_p1.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCnv_charge_p1.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCollection_charge_p1.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCollectionCnv_charge_p1.h"
#include "ALFA_EventTPCnv/ALFA_RawDataContainer_charge_p1.h"
#include "ALFA_EventTPCnv/ALFA_RawDataContainerCnv_charge_p1.h"


#include "ALFA_EventTPCnv/ALFA_LocRecEvent_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecEventCnv_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecEvCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecEvCollectionCnv_p1.h"

#include "ALFA_EventTPCnv/ALFA_LocRecODEvent_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecODEventCnv_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecODEvCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecODEvCollectionCnv_p1.h"

#include "ALFA_EventTPCnv/ALFA_LocRecCorrEvent_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrEventCnv_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrEvCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrEvCollectionCnv_p1.h"

#include "ALFA_EventTPCnv/ALFA_LocRecCorrODEvent_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrODEventCnv_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrODEvCollection_p1.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrODEvCollectionCnv_p1.h"



DECLARE_COMPONENT( ALFA_Hit_p1 )
DECLARE_COMPONENT( ALFA_HitCnv_p1 )
DECLARE_COMPONENT( ALFA_HitCollection_p1 )
DECLARE_COMPONENT( ALFA_HitCollectionCnv_p1 )

DECLARE_COMPONENT( ALFA_Digit_p1 )
DECLARE_COMPONENT( ALFA_DigitCnv_p1 )
DECLARE_COMPONENT( ALFA_DigitCollection_p1 )
DECLARE_COMPONENT( ALFA_DigitCollectionCnv_p1 )

DECLARE_COMPONENT( ALFA_ODHit_p1 )
DECLARE_COMPONENT( ALFA_ODHitCnv_p1 )
DECLARE_COMPONENT( ALFA_ODHitCollection_p1 )
DECLARE_COMPONENT( ALFA_ODHitCollectionCnv_p1 )

DECLARE_COMPONENT( ALFA_ODDigit_p1 )
DECLARE_COMPONENT( ALFA_ODDigitCnv_p1 )
DECLARE_COMPONENT( ALFA_ODDigitCollection_p1 )
DECLARE_COMPONENT( ALFA_ODDigitCollectionCnv_p1 )

DECLARE_COMPONENT( ALFA_RawData_p1 )
DECLARE_COMPONENT( ALFA_RawDataCnv_p1 )
DECLARE_COMPONENT( ALFA_RawDataCollection_p1 )
DECLARE_COMPONENT( ALFA_RawDataCollectionCnv_p1 )
DECLARE_COMPONENT( ALFA_RawDataContainer_p1 )
DECLARE_COMPONENT( ALFA_RawDataContainerCnv_p1 )

DECLARE_COMPONENT( ALFA_RawData_charge_p1 )
DECLARE_COMPONENT( ALFA_RawDataCnv_charge_p1 )
DECLARE_COMPONENT( ALFA_RawDataCollection_charge_p1 )
DECLARE_COMPONENT( ALFA_RawDataCollectionCnv_charge_p1 )
DECLARE_COMPONENT( ALFA_RawDataContainer_charge_p1 )
DECLARE_COMPONENT( ALFA_RawDataContainerCnv_charge_p1 )

DECLARE_COMPONENT( ALFA_LocRecEvent_p1 )
DECLARE_COMPONENT( ALFA_LocRecEventCnv_p1 )
DECLARE_COMPONENT( ALFA_LocRecEvCollection_p1 )
DECLARE_COMPONENT( ALFA_LocRecEvCollectionCnv_p1 )

DECLARE_COMPONENT( ALFA_LocRecODEvent_p1 )
DECLARE_COMPONENT( ALFA_LocRecODEventCnv_p1 )
DECLARE_COMPONENT( ALFA_LocRecODEvCollection_p1 )
DECLARE_COMPONENT( ALFA_LocRecODEvCollectionCnv_p1 )

DECLARE_COMPONENT( ALFA_LocRecCorrEvent_p1 )
DECLARE_COMPONENT( ALFA_LocRecCorrEventCnv_p1 )
DECLARE_COMPONENT( ALFA_LocRecCorrEvCollection_p1 )
DECLARE_COMPONENT( ALFA_LocRecCorrEvCollectionCnv_p1 )

DECLARE_COMPONENT( ALFA_LocRecCorrODEvent_p1 )
DECLARE_COMPONENT( ALFA_LocRecCorrODEventCnv_p1 )
DECLARE_COMPONENT( ALFA_LocRecCorrODEvCollection_p1 )
DECLARE_COMPONENT( ALFA_LocRecCorrODEvCollectionCnv_p1 )

