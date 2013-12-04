#include "GaudiKernel/DeclareFactoryEntries.h"
#include "BarcodeServices/LegacyBarcodeSvc.h"
#include "BarcodeServices/GenericBarcodeSvc.h"
#include "BarcodeServices/ValidationBarcodeSvc.h"
#include "BarcodeServices/GlobalBarcodeSvc.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( Barcode, LegacyBarcodeSvc  )
DECLARE_NAMESPACE_SERVICE_FACTORY( Barcode, GenericBarcodeSvc )
DECLARE_NAMESPACE_SERVICE_FACTORY( Barcode, ValidationBarcodeSvc )
DECLARE_NAMESPACE_SERVICE_FACTORY( Barcode, GlobalBarcodeSvc )

DECLARE_FACTORY_ENTRIES( BarcodeServices ) {
  DECLARE_NAMESPACE_SERVICE( Barcode, LegacyBarcodeSvc  )
  DECLARE_NAMESPACE_SERVICE( Barcode, GenericBarcodeSvc )
  DECLARE_NAMESPACE_SERVICE( Barcode, ValidationBarcodeSvc )
  DECLARE_NAMESPACE_SERVICE( Barcode, GlobalBarcodeSvc )
}

