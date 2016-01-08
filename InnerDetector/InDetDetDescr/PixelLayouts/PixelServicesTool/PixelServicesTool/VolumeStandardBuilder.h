/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGeoModelUtils_VolumeStandardBuilder_H
#define InDetGeoModelUtils_VolumeStandardBuilder_H

#include "InDetGeoModelUtils/VolumeSplitterUtils.h"
#include "InDetGeoModelUtils/VolumeSplitter.h"
#include "AthenaKernel/MsgStreamMember.h"

#include <string>
#include <vector>

class InDetMaterialManager;
class GeoPhysVol;
class GeoFullPhysVol;
class GeoVPhysVol;
class GeoTransform;
class GeoShape;

namespace InDetDD {
  class ServiceVolume;
 
  class VolumeStandardBuilder
  {
  public:
    VolumeStandardBuilder( const Zone & zone, const std::vector<const ServiceVolume * > & services, int volIndex=-1);
    VolumeStandardBuilder(const std::vector<const ServiceVolume * > & services);
    VolumeStandardBuilder( const Zone & zone, const std::vector<const ServiceVolume * > & services,
			   const std::vector<const ServiceVolume * > & servEnv, const std::vector<const ServiceVolume * > & servChild );

    void setRegion(const std::string & region, double zcenter);
    void setMaterialManager(InDetMaterialManager * matManager) {m_matManager = matManager; }
    const std::vector<const ServiceVolume *> & services();
    const std::vector<const ServiceVolume * > & servicesEnv() ;
    const std::vector<const ServiceVolume * > & servicesChild() ;

    void buildAndPlace(const std::string & region, GeoPhysVol * parent, double zcenter = 0, std::vector<std::string> svcList = std::vector<std::string>());
    void buildAndPlace(const std::string & region, GeoFullPhysVol * parent, double zcenter = 0, std::vector<std::string> svcList = std::vector<std::string>());
    void buildAndPlaceEnvelope(const std::string & region, GeoFullPhysVol * parent, int iParent, int iElement, double zcenter = 0);
    void buildAndPlaceEnvelope(const std::string & region, GeoPhysVol * parent, int iParent, int iElement, double zcenter = 0);
    void addServices(const Zone & zone, const std::vector<const ServiceVolume * > & services);
    GeoVPhysVol* build(int iElement);
    int numCopies(int iElement);
    GeoTransform * getPlacement(int iElement, int iCopy);
    GeoTransform * getPlacementEnvelope(int iElement, int iCopy,  int iMothElement);

    double computeRmin(const std::string & region, std::vector<std::string> svcList = std::vector<std::string>());
    double computeRmax(const std::string & region, std::vector<std::string> svcList = std::vector<std::string>());
    double computeZmin(const std::string & region, std::vector<std::string> svcList = std::vector<std::string>());
    double computeZmax(const std::string & region, std::vector<std::string> svcList = std::vector<std::string>());

    MsgStream& msg (MSG::Level lvl) const { return m_msg.get() << lvl; }
    bool msgLvl (MSG::Level lvl) const { return m_msg.get().level() <= lvl; }

    bool isEnvelopeOrChild(int iElement);
    int  getEnvelopeNum(int iElement);
    int  getParentNum(int iElement);
    bool isChildService(int iElt,int iChld);
    double getZcenter(int iElt);

    double getServiceNumber() { return m_services->size(); } 

  private:
    //const GeoShape * getShape(const ServiceVolume & param, double & volume);
    
    mutable Athena::MsgStreamMember m_msg;
    std::string m_region;
    double m_zcenter;
    int m_volumeIndex;
    const std::vector<const ServiceVolume *>* m_services;
    const std::vector<const ServiceVolume *>* m_servEnvelope;
    const std::vector<const ServiceVolume *>* m_servChild;
    VolumeSplitter m_splitter;
    InDetMaterialManager * m_matManager;

  };
 
}
#endif // InDetGeoModelUtils_VolumeBuidler_H
