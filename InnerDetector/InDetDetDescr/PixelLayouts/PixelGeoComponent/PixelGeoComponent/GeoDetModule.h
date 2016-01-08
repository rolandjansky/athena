/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoDetModule_H
#define InDet_GeoDetModule_H

#include "PixelGeoComponent/GeoComponent.h"
#include "PixelGeoComponent/GeoComponentHelper.h"

class GeoNameTag;


namespace InDet {
  
  class GeoSensor;
  class GeoSimpleObject;
  class GeoDetModuleEnv;

  class GeoDetModule : public GeoComponent {
  public:

    GeoDetModule( GeoFullPhysVol* vol);
    //    GeoDetModule( GeoPhysVol* vol);
    GeoDetModule(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag, int etaModule);

    virtual ~GeoDetModule()=0;

    /// Interface extension: access to sensors
    virtual const std::vector<GeoSensor*> sensors(int iSide) const =0;   //const {return m_sensors;}
    
    /// Access to all components (sensors and services/support volumes, if any)
      virtual const std::vector<GeoComponent*> components() const =0;
      
    /// Polymorphic access to sensors
    virtual int numActiveComponents(int iSide) const = 0;   //{return m_sensors.size();}
	
    // Polymorphic access to sensors
    virtual const GeoSensor& activeComponent( int i, int iSide) const = 0; //{return *m_sensors.at(i);}

    // Polymorphic access to snsor services
    virtual const std::vector<GeoSimpleObject* > serviceComponent(int iSide=0 ) const =0;

    // Number of sides
    virtual int getNumSides() const =0;

    /// mayb some indication to whether the division in sectors is in eta or in phi?
    virtual void placeSensor( GeoSensor& sensor, const HepGeom::Transform3D& transform, int number, int sector) =0;
    virtual void placeSensorService( GeoSimpleObject& layer, const HepGeom::Transform3D& transform,  int number, int sector) =0;
    virtual void placeAuxService( GeoSimpleObject& service, const HepGeom::Transform3D& transform) =0;
    virtual void placeSensorEnvelope( GeoDetModuleEnv& sensorEnv, const HepGeom::Transform3D& transform, int sector, int sideId) =0;

    virtual InDet::GeoDetModule* place(const HepGeom::Transform3D& transform, int number, int etaModule)=0;

    virtual int getModuleType() const=0;
    virtual const GeoFullPhysVol* getSensorFullPhysVolume(int iSensor, int iSide) const=0;
    virtual MinMaxHelper getSensorZminmax(const HepGeom::Transform3D& trf) const=0;

    int getEtaModuleId() { return m_etaModuleId; };
    virtual int getSensorPos(int iSensor, int iSide) const=0;

    /*    virtual int getModuleType() const =0;*/
    /*    int getPhiModule() const { return m_phiModule; }*/
/*     void setEtaModuleId(int eta)  { m_etaModule = eta; } */
/*     void setPhiModule(int phi) { m_phiModule = phi; } */
/*     void initEtaPhiModule() { m_etaModule=-9999; m_phiModule=-9999; } */

    void setModuleType(int iModule) { m_moduleType = iModule; };

  protected:

    int m_moduleType;
    int m_etaModuleId;

  };

  inline GeoDetModule::~GeoDetModule() { }

}

#endif
