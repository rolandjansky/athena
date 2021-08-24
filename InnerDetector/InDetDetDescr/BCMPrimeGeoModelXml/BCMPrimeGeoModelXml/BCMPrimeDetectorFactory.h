/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCMPRIMEGEOMODELXML_BCMPRIMEDETECTORFACTORY_H 
#define BCMPRIMEGEOMODELXML_BCMPRIMEDETECTORFACTORY_H 

#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h" 
#include "BCMPrimeReadoutGeometry/BCMPrimeDetectorManager.h"

namespace InDetDD {class AthenaComps;}
class GeoPhysVol;

namespace InDetDDSLHC {
  class BCMPrimeOptions;

  //
  //    Main routine to build the GeoModel geometry, and handle the GeometryManager and 
  //    DetectorManager.
  // 
  class BCMPrimeDetectorFactory : public InDetDD::DetectorFactoryBase {   
  public: 

    BCMPrimeDetectorFactory(InDetDD::AthenaComps *athenaComps, 
                            BCMPrimeOptions &options);
    virtual ~BCMPrimeDetectorFactory(); 

    /** Creation of geometry */
    virtual void create(GeoPhysVol *world);
  
    /** Access to the results */
    virtual InDetDD::BCMPrimeDetectorManager * getDetectorManager() const;

  private: 

    // Copy and assignments operations illegal and so are made private
    BCMPrimeDetectorFactory(BCMPrimeDetectorFactory &right); 
    BCMPrimeDetectorFactory & operator=(BCMPrimeDetectorFactory &right); 

    InDetDD::BCMPrimeDetectorManager *m_detectorManager;
    InDetDD::AthenaComps *m_athenaComps;
    BCMPrimeOptions *m_options;
  }; 
}  
#endif 
