/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoDetModuleEnv_H
#define InDet_GeoDetModuleEnv_H

#include "PixelGeoComponent/GeoComponent.h"
#include "PixelGeoComponent/GeoComponentHelper.h"

class GeoNameTag;


namespace InDet {
  
  class GeoSensor;
  class GeoSimpleObject;

  class GeoDetModuleEnv : public GeoComponent {
  public:

    GeoDetModuleEnv( GeoFullPhysVol* vol);
    GeoDetModuleEnv(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag, int sideId=-9999);

    virtual ~GeoDetModuleEnv();

    // Interface extension: access to sensors
    virtual const std::vector<GeoSensor*> sensors(int iSide=0) const { iSide++; return m_sensors; }

    /// Access to all components (sensors and services/support volumes, if any)
    virtual const std::vector<GeoComponent*> components() const;

    // Polymorphic access to sensors
    virtual int numActiveComponents(int iSide) const { iSide++; return m_sensors.size();}

    // Polymorphic access to sensors
    virtual const GeoSensor& activeComponent( int i, int iSide=0) const { iSide++; return *m_sensors.at(i);}

    /// Polymorphic access to snsor services
      virtual const std::vector<GeoSimpleObject* > serviceComponent(int iSide=0 ) const { iSide++; return m_sensorServices; }

    // mayb some indication to whether the division in sectors is in eta or in phi?
    virtual void placeSensor( GeoSensor& sensor, const HepGeom::Transform3D& transform=HepGeom::Transform3D(), int number=0, int sensorPos=-9999);
    virtual void placeSensorService( GeoSimpleObject& service, const HepGeom::Transform3D& transform, int number);

    GeoDetModuleEnv* place(const HepGeom::Transform3D& transform, int number, int sideId);

    //    int getModuleType(int iSensor=0) const;
    const GeoFullPhysVol* getSensorFullPhysVolume(int iSensor=0) const;
    MinMaxHelper getSensorZminmax(const HepGeom::Transform3D& trf) const;

    int getSideId() const { return m_sideId; }
    int getSensorPos(int iSensor=0) const;

    /*    GeoFullPhysVol *getSensorGeoFullPhysVol<GeoFullPhysVol>(int iSensor){ return m_sensors[iSensor]->physVolume(); };*/

  private:

    int m_geoId;
    int m_sideId;
    std::vector<GeoSensor*> m_sensors;
    std::vector<GeoSimpleObject*> m_sensorServices;

  };

}

#endif
