/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArDetectorFactory.h
 *
 * @brief Declaration of LArDetectorFactory class
 *
 * $Id: LArDetectorFactory.h,v 1.4 2009-02-10 16:43:17 tsulaia Exp $
 */

#ifndef LArGeo_LArDetectorFactory_h
#define LArGeo_LArDetectorFactory_h
#include "LArReadoutGeometry/LArDetectorManager.h"
#include "GeoModelKernel/GeoVDetectorFactory.h"

namespace LArGeo {

  /** @class LArGeo::LArDetectorFactory
      @brief LArDetectorFactory builds GeoModel description of LAr calorimeter by calling
      relevant 'Construction' classes (Barrel, Endcap). It also builds readout geometry
      description using LArReadoutGeometry objects.
   */
  class LArDetectorFactory : public GeoVDetectorFactory  {

  public:
  
    // Constructor:
    LArDetectorFactory(int testbeam, bool fullGeo);
  
    // Destructor:
    virtual ~LArDetectorFactory();
  
    // Creation of geometry:
    virtual void create(GeoPhysVol* world);
  
    // Access to the results:
    virtual const LArDetectorManager* getDetectorManager() const;
  

    // Set parameters:

    void setBarrelSagging(bool flag)        {_barrelSagging  = flag;}
    void setBarrelCellVisLimit(int maxCell) {_barrelVisLimit = maxCell;}
    void setFCALVisLimit(int maxCell)       {_fcalVisLimit   = maxCell;}
    void setBuildBarrel(bool flag)          {_buildBarrel = flag;}
    void setBuildEndcap(bool flag)          {_buildEndcap = flag;}


  private:  
  
    // Illegal operations:
    const LArDetectorFactory & operator=(const LArDetectorFactory &right);
    LArDetectorFactory(const LArDetectorFactory &right);
  
    // The manager:
    LArDetectorManager*       m_detectorManager;
    bool                      _barrelSagging;
    int                       _barrelVisLimit;
    int                       _fcalVisLimit;

    bool                      _buildBarrel;
    bool                      _buildEndcap;

    int                       _testbeam;

    bool                       m_fullGeo;  // true->FULL, false->RECO
  };

} // namespace LArGeo

#endif 

