/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_Numerology.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_TRTNUMEROLOGY_H
#define INDETREADOUTGEOMETRY_TRTNUMEROLOGY_H

namespace InDetDD {
  
  /** @class TRT_Numerology

      Helper class to organize the straw elements on 
      TRT readout elements.

    */
  class TRT_Numerology {
  
    public:
    
      // Constructor:
      TRT_Numerology();
    
      // Destructor:
      ~TRT_Numerology();
    
      // Modifiers:
      void setNBarrelLayers(unsigned int module, unsigned int nLayers);
      void setNEndcapLayers(unsigned int wheel,  unsigned int nLayers);
    
      void setNBarrelRings (unsigned int ring );
      void setNBarrelPhi   (unsigned int phi  );
      void setNEndcapWheels(unsigned int wheel);
      void setNEndcapPhi   (unsigned int phi  );

      // Accessors:
      inline unsigned int getNBarrelLayers(unsigned int iMod  ) const; 
      inline unsigned int getNEndcapLayers(unsigned int iWheel) const; 

      inline unsigned int getNBarrelRings () const;
      inline unsigned int getNBarrelPhi   () const;
      inline unsigned int getNEndcapWheels() const;
      inline unsigned int getNEndcapPhi   () const;
    
    private:
    
      unsigned int m_nlayerBarrel[3];
      unsigned int m_nlayerEndcap[18];

      unsigned int m_nringBarrel;
      unsigned int m_nphiBarrel;
      unsigned int m_nwheelEndcap;
      unsigned int m_nphiEndcap;
  };
}
#include "TRT_Numerology.icc"
#endif
