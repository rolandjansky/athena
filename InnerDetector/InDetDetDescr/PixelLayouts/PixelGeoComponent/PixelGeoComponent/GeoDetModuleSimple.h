/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoDetModuleSimple_H
#define InDet_GeoDetModuleSimple_H

#include "PixelGeoComponent/GeoComponent.h"
#include "PixelGeoComponent/GeoComponentHelper.h"

#include "PixelGeoComponent/GeoDetModule.h"

class GeoNameTag;


namespace InDet {
  
  class GeoSensor;
  class GeoSimpleObject;
  class GeoDetModuleEnv;

  class GeoDetModuleSimple : public GeoDetModule {
  public:

    GeoDetModuleSimple( GeoFullPhysVol* vol);
    //    GeoDetModuleSimple( GeoPhysVol* vol);
    GeoDetModuleSimple(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag, int etaModule);

    ~GeoDetModuleSimple();

    // Interface extension: access to sensors
    virtual const std::vector<GeoSensor*> sensors(int iSide=0) const { iSide++; return m_sensors;}

    // Access to all components (sensors and services/support volumes, if any)
    virtual const std::vector<GeoComponent*> components() const;
    
    // Polymorphic access to sensors
    virtual int numActiveComponents(int iSide) const;

    // Polymorphic access to sensors
    virtual const GeoSensor& activeComponent( int i, int iSide=0) const { iSide++; return *m_sensors.at(i);}

    // Polymorphic access to snsor services
    virtual const std::vector<GeoSimpleObject* > serviceComponent(int iSide=0 ) const { iSide++; return m_sensorServices; }

    // Number of sides
    virtual int getNumSides() const { return m_numSides; }

    // mayb some indication to whether the division in sectors is in eta or in phi?
    virtual void placeSensor( GeoSensor& sensor, const HepGeom::Transform3D& transform, int number, int sector);
    virtual void placeSensorService( GeoSimpleObject& layer, const HepGeom::Transform3D& transform, int number, int sector=0);
    virtual void placeAuxService( GeoSimpleObject& service, const HepGeom::Transform3D& transform);
    virtual void placeSensorEnvelope( GeoDetModuleEnv& sensorEnv, const HepGeom::Transform3D& transform, int sector, int sideId=0);

    InDet::GeoDetModuleSimple* place(const HepGeom::Transform3D& transform, int number, int etaModule);

    //    int getModuleType(int iSide=0) const;
    int getModuleType() const {  return m_moduleType; };
    const GeoFullPhysVol* getSensorFullPhysVolume(int iSensor, int iSide=0) const;
    MinMaxHelper getSensorZminmax(const HepGeom::Transform3D& trf) const;

    int getSensorPos(int iSensor, int iSide=0) const;

    /*    GeoFullPhysVol *getSensorGeoFullPhysVol<GeoFullPhysVol>(int iSensor){ return m_sensors[iSensor]->physVolume(); };*/

  private:

    std::vector<GeoSensor*> m_sensors;
    std::vector<GeoSimpleObject*> m_sensorServices;
    std::vector<GeoSimpleObject*> m_auxServices; 

    int m_numSides;

  };

}

#endif
