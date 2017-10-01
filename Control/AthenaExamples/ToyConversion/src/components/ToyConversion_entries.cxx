#include "ToyConversion/FooBar.h"
#include "ToyConversion/ToyConversionSvc.h"
#include "ToyConversion/ToyConverter.h"

typedef ToyConverter<Foo> ToyCnvFoo;
typedef ToyConverter<Bar> ToyCnvBar;

DECLARE_SERVICE_FACTORY( ToyConversionSvc )
DECLARE_CONVERTER_FACTORY( ToyCnvFoo )
DECLARE_CONVERTER_FACTORY( ToyCnvBar )

