/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ForwardScrutinizer_h
#define ForwardScrutinizer_h 1

#include "GeoModelKernel/GeoVDetectorElement.h"
#ifndef BUILDVP1LIGHT
    #include "Identifier/Identifier.h"
#endif

class ForwardScrutinizer final : public GeoVDetectorElement
{
 public:
  ForwardScrutinizer(const GeoVFullPhysVol *fullPhysVol);
  virtual ~ForwardScrutinizer() override final;

   #if defined BUILDVP1LIGHT
     int identify() const;
   #else
     Identifier identify() const;
   #endif

 private:
  ForwardScrutinizer(const ForwardScrutinizer &right);
  const ForwardScrutinizer & operator=(const ForwardScrutinizer &right);
};

#endif
