/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCMPRIMEGEOMODELXML_BCMPRIMEDETECTORFACTORY_H
#define BCMPRIMEGEOMODELXML_BCMPRIMEDETECTORFACTORY_H

#include <BCMPrimeReadoutGeometry/BCMPrimeDetectorManager.h>
#include <InDetGeoModelUtils/InDetDDAthenaComps.h>
#include <InDetGeoModelUtils/InDetDetectorFactoryBase.h>


namespace InDetDD
{

class BCMPrimeOptions;

//
//    Main routine to build the GeoModel geometry, and handle the GeometryManager and
//    DetectorManager.
//
class BCMPrimeDetectorFactory : public DetectorFactoryBase
{
public:

  BCMPrimeDetectorFactory(AthenaComps *athenaComps,
                          const BCMPrimeOptions &options);

  /** Creation of geometry */
  virtual void create(GeoPhysVol *world) override final;

  /** Access to the results */
  virtual BCMPrimeDetectorManager * getDetectorManager() const override final;

private:

  // Copy and assignments operations illegal and so are made private
  BCMPrimeDetectorFactory(BCMPrimeDetectorFactory &right);
  BCMPrimeDetectorFactory & operator=(BCMPrimeDetectorFactory &right);

  BCMPrimeDetectorManager *m_detectorManager{};
  const BCMPrimeOptions &m_options;
};

} // namespace InDetDD

#endif
