/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArDetectorFactoryLite.h
 *
 * @class LArGeo::LArDetectorFactoryLite
 *
 * @brief LArDetectorFactoryLite is invoked by the LArDetectorTool when the GeoModel 
 * description of LAr calorimeter is built from the SQLite database
 * relevant 'Construction' classes (Barrel, Endcap). It also builds readout geometry
 *
 */

#ifndef LARGEOALGSNV_LARDETECTORFACTORYLITE_H
#define LARGEOALGSNV_LARDETECTORFACTORYLITE_H

#include "LArReadoutGeometry/LArDetectorManager.h"
#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "AthenaBaseComps/AthMessaging.h"

class StoreGateSvc;
class IRDBAccessSvc;
class LArHVManager;

namespace GeoModelIO {
  class ReadGeoModel;
}

namespace LArGeo {

  class LArDetectorFactoryLite : public GeoVDetectorFactory, public AthMessaging {

  public:

    LArDetectorFactoryLite(StoreGateSvc* detStore
			   , IRDBAccessSvc* paramSvc
			   , GeoModelIO::ReadGeoModel* sqliteReader
			   , const LArHVManager* hvManager);
    LArDetectorFactoryLite() = delete;
    LArDetectorFactoryLite(const LArDetectorFactoryLite &right) = delete;
    const LArDetectorFactoryLite & operator=(const LArDetectorFactoryLite &right) = delete;

    virtual ~LArDetectorFactoryLite();

    // Build the parts of the description that cannot be read from the SQLite database
    virtual void create(GeoPhysVol* world) override;

    // Access to the results:
    virtual const LArDetectorManager* getDetectorManager() const override {return m_detectorManager;}

    void setBarrelSagging(bool flag) {m_barrelSagging  = flag;}
    void setTestBeam(int flag)       {m_testBeam  = flag;}

  private:

    LArDetectorManager*       m_detectorManager;
    StoreGateSvc*             m_detStore;
    IRDBAccessSvc*            m_paramSvc;
    GeoModelIO::ReadGeoModel* m_sqliteReader;
    const LArHVManager*       m_hvManager;

    bool m_barrelSagging;
    int  m_testBeam;
  };

} // namespace LArGeo

#endif

