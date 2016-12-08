/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ExcluderConstruction
// 
// 
// July-2006 JP Archambault, Mohsen Khakzad

#ifndef LARGEOH62002ALGS_EXCLUDERCONSTRUCTION_H
#define LARGEOH62002ALGS_EXCLUDERCONSTRUCTION_H

class GeoPhysVol;

namespace LArGeo {
  
  class ExcluderConstruction 
  {
  public:
    
    ExcluderConstruction();
    virtual ~ExcluderConstruction();
    
    // Get the envelope containing this detector.
    virtual GeoPhysVol* GetEnvelope();
      
  private:
  };

}    // namespace LArGeo

#endif // LARGEOH62002ALGS_EXCLUDERCONSTRUCTION_H
