/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArDetectorFactory.h
 *
 * @class LArGeo::LArDetectorFactory
 *
 * @brief LArDetectorFactory builds GeoModel description of LAr calorimeter by calling
 * relevant 'Construction' classes (Barrel, Endcap). It also builds readout geometry
 * description using LArReadoutGeometry objects.
 *
 * $Id: LArDetectorFactory.h,v 1.4 2009-02-10 16:43:17 tsulaia Exp $
 */

#ifndef LARGEOALGSNV_LARDETECTORFACTORY_H
#define LARGEOALGSNV_LARDETECTORFACTORY_H
#include "LArReadoutGeometry/LArDetectorManager.h"
#include "GeoModelKernel/GeoVDetectorFactory.h"

class LArHVManager;

namespace LArGeo {

  class LArDetectorFactory : public GeoVDetectorFactory  {

  public:

    // Constructor:
    LArDetectorFactory(int testbeam, bool fullGeo, const LArHVManager* hvManager);

    // Destructor:
    virtual ~LArDetectorFactory();

    // Creation of geometry:
    virtual void create(GeoPhysVol* world);

    // Access to the results:
    virtual const LArDetectorManager* getDetectorManager() const;


    // Set parameters:

    void setBarrelSagging(bool flag)        {m_barrelSagging  = flag;}
    void setBarrelCellVisLimit(int maxCell) {m_barrelVisLimit = maxCell;}
    void setFCALVisLimit(int maxCell)       {m_fcalVisLimit   = maxCell;}
    void setBuildBarrel(bool flag)          {m_buildBarrel = flag;}
    void setBuildEndcap(bool flag)          {m_buildEndcap = flag;}
    void setEMECVariant(const std::string &inner, const std::string &outer)
    {
        m_EMECVariantInner = inner;
        m_EMECVariantOuter = outer;
    }
    void setActivateFT(bool flag)           { m_activateFT = flag; }

  private:

    // Illegal operations:
    const LArDetectorFactory & operator=(const LArDetectorFactory &right);
    LArDetectorFactory(const LArDetectorFactory &right);

    // The manager:
    LArDetectorManager*       m_detectorManager;
    const LArHVManager*       m_hvManager;
    bool                      m_barrelSagging;
    int                       m_barrelVisLimit;
    int                       m_fcalVisLimit;

    bool                      m_buildBarrel;
    bool                      m_buildEndcap;

    int                       m_testbeam;

    bool                       m_fullGeo;  // true->FULL, false->RECO
    std::string               m_EMECVariantInner;
    std::string               m_EMECVariantOuter;

    bool                      m_activateFT;
  };

} // namespace LArGeo

#endif

