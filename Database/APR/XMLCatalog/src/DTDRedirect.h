/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DTDREDIRECT_H
#define DTDREDIRECT_H 

#include "xercesc/sax/EntityResolver.hpp"
#include "xercesc/sax/InputSource.hpp"

//using namespace xercesc;

class DTDRedirect : public xercesc::EntityResolver
{
  public:
  DTDRedirect() {}
  ~DTDRedirect() {}
  xercesc::InputSource* resolveEntity
    (
             const   XMLCh* const    publicId
             , const XMLCh* const    systemId
    );

};

#endif // DTDREDIRECT_H
