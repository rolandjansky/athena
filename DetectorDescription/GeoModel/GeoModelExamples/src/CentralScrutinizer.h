/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CentralScrutinizer_h
#define CentralScrutinizer_h 1

#include "GeoModelKernel/GeoVDetectorElement.h"
#ifndef BUILDVP1LIGHT
    #include "Identifier/Identifier.h"
#endif

class CentralScrutinizer final : public GeoVDetectorElement
{
 public:
  CentralScrutinizer(const GeoVFullPhysVol *fullPhysVol);
  virtual ~CentralScrutinizer() override final;

  #if defined BUILDVP1LIGHT
  	int identify() const;
  #else
  	Identifier identify() const;
  #endif

 private:
  CentralScrutinizer(const CentralScrutinizer &right);
  const CentralScrutinizer & operator=(const CentralScrutinizer &right);
};

// Class CentralScrutinizer 
#endif
