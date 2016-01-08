/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoDetModuleDouble_H
#define InDet_GeoDetModuleDouble_H

#include "PixelGeoComponent/GeoComponent.h"
#include "PixelGeoComponent/GeoComponentHelper.h"

#include "PixelGeoComponent/GeoDetModule.h"
#include "PixelGeoComponent/GeoDetModuleDouble.h"

class GeoNameTag;


namespace InDet {
  
  class GeoSensor;
  class GeoSimpleObject;
  class GeoDetModuleEnv;

  class GeoDetModuleDouble : public GeoDetModule {
  public:

    GeoDetModuleDouble( GeoFullPhysVol* vol);
    //    GeoDetModuleDouble( GeoPhysVol* vol);
    GeoDetModuleDouble(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag, int etaModule);

    ~GeoDetModuleDouble();

    /// Interface extension: access to sensors
    virtual const std::vector<GeoSensor*> sensors(int iSide) const;

    /// Access to all components (sensors and services/support volumes, if any)
    virtual const std::vector<GeoComponent*> components() const;

    /// Polymorphic access to sensors
    virtual int numActiveComponents(int iSide) const;

    // Polymorphic access to sensors
    virtual const GeoSensor& activeComponent( int i, int iSide) const;

    // Polymorphic access to snsor services
    virtual const std::vector<GeoSimpleObject* > serviceComponent(int iSide) const;

    // Number of sides
    virtual int getNumSides() const { return m_numSides; }

    // mayb some indication to whether the division in sectors is in eta or in phi?
    virtual void placeSensor( GeoSensor& sensor, const HepGeom::Transform3D& transform, int number, int sector);
    virtual void placeSensorService( GeoSimpleObject& layer, const HepGeom::Transform3D& transform, int number, int sector);
    virtual void placeAuxService( GeoSimpleObject& service, const HepGeom::Transform3D& transform);

    virtual void placeSensorEnvelope( GeoDetModuleEnv& sensorEnv, const HepGeom::Transform3D& transform, int sector, int sideId);

    InDet::GeoDetModuleDouble* place(const HepGeom::Transform3D& transform, int number, int etaModule);

    //    int getModuleType(int iSide) const;
    int getModuleType() const {  return m_moduleType; };
    const GeoFullPhysVol* getSensorFullPhysVolume(int iSensor, int iSide) const;
    MinMaxHelper getSensorZminmax(const HepGeom::Transform3D& trf) const;
    const GeoDetModuleEnv& getModuleEnvelope(int iSide) const;

    int getEnvelopeSideId(int iSide) const;
    int getSensorPos(int iSensor, int iSide) const;

    /*    GeoFullPhysVol *getSensorGeoFullPhysVol<GeoFullPhysVol>(int iSensor){ return m_sensors[iSensor]->physVolume(); };*/

  private:

    GeoDetModuleEnv* m_innerEnvelope;
    GeoDetModuleEnv* m_outerEnvelope;
    std::vector<GeoSimpleObject*> m_auxServices;

    int m_numSides;

  };

}

#endif
