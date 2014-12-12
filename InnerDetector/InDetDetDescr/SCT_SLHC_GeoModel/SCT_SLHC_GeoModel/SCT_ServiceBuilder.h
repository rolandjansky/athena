/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_SERVICE_BUILDER_H
#define SCT_SLHC_GeoModel_SCT_SERVICE_BUILDER_H

namespace InDetDD {
  class Zone;
  class VolumeBuilder;
}

namespace InDetDDSLHC {

class SCT_GeometryManager;

class  SCT_ServiceBuilder {
  
public:
  SCT_ServiceBuilder(InDetDD::Zone * topZone, const SCT_GeometryManager *);
  InDetDD::VolumeBuilder * getServMatBuilder() const {return m_servMatBuilder;}
  //disable copy, assign
  SCT_ServiceBuilder & operator=(const SCT_ServiceBuilder&) = delete;
  SCT_ServiceBuilder( const SCT_ServiceBuilder &) = delete;
  
  ~SCT_ServiceBuilder();
private:
  InDetDD::VolumeBuilder * m_servMatBuilder;
};

}

#endif  //SCT_SLHC_GeoModel_SCT_SERVICE_BUILDER_H
