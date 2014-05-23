#include "ToyConversion/FooBar.h"
#include "ToyConversion/ToyConversionSvc.h"
#include "ToyConversion/ToyConverter.h"

typedef ToyConverter<Foo> ToyCnvFoo;
typedef ToyConverter<Bar> ToyCnvBar;

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_FACTORY_ENTRIES( ToyConversion ) {
  DECLARE_SERVICE( ToyConversionSvc )
  DECLARE_CONVERTER( ToyCnvFoo )
  DECLARE_CONVERTER( ToyCnvBar )
}
DECLARE_SERVICE_FACTORY( ToyConversionSvc )
DECLARE_CONVERTER_FACTORY( ToyCnvFoo )
DECLARE_CONVERTER_FACTORY( ToyCnvBar )
