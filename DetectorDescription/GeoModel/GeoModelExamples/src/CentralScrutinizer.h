/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CentralScrutinizer_h
#define CentralScrutinizer_h 1

#include "GeoModelKernel/GeoVDetectorElement.h"
#include "Identifier/Identifier.h"

class CentralScrutinizer : public GeoVDetectorElement
{
 public:
  CentralScrutinizer(const GeoVFullPhysVol *fullPhysVol);
  virtual ~CentralScrutinizer() override final;

  Identifier identify() const;

 private:
  CentralScrutinizer(const CentralScrutinizer &right);
  const CentralScrutinizer & operator=(const CentralScrutinizer &right);
};

// Class CentralScrutinizer 
#endif
