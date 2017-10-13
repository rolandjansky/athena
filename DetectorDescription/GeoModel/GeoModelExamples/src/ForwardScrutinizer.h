/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ForwardScrutinizer_h
#define ForwardScrutinizer_h 1

#include "GeoModelKernel/GeoVDetectorElement.h"
#include "Identifier/Identifier.h"

class ForwardScrutinizer : public GeoVDetectorElement
{
 public:
  ForwardScrutinizer(const GeoVFullPhysVol *fullPhysVol);
  virtual ~ForwardScrutinizer() override final;

  Identifier identify() const;

 private:
  ForwardScrutinizer(const ForwardScrutinizer &right);
  const ForwardScrutinizer & operator=(const ForwardScrutinizer &right);
};

#endif
