/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_ATHENAPOOLCNV_TP_EXT_H
#define ATHENAPOOLCNVSVC_ATHENAPOOLCNV_TP_EXT_H

#include "TPTools/AthenaConverterTLPExtension.h"
#include "GaudiKernel/IConverter.h"
class AthenaPoolTopLevelTPCnvBase;

#include <stdexcept>

class AthenaPoolCnvTPExtension : public AthenaConverterTLPExtension
{
public:
  AthenaPoolCnvTPExtension() {}

  virtual bool          registerExtendingCnv( AthenaConverterTLPExtension *cnv ) {
     return AthenaConverterTLPExtension::registerExtendingCnv( cnv );
  }

  virtual bool          registerExtendingCnv( IConverter *cnv ) {
     AthenaConverterTLPExtension *extending_converter = dynamic_cast<AthenaPoolCnvTPExtension*>(cnv);
     // if the converter passed is not an extending converter, we can't do anything
     if( !extending_converter )
        throw std::runtime_error("Attempt to register extending AthenaPool converter that does not inherit from AthenaPoolCnvTPExtension");
     return AthenaConverterTLPExtension::registerExtendingCnv( extending_converter );
  }

  void usingTPCnvForReading( AthenaPoolTopLevelTPCnvBase &baseTLPcnv ) {
     AthenaConverterTLPExtension::usingTPCnvForReading( (void*)&baseTLPcnv );
  }

};


#endif











