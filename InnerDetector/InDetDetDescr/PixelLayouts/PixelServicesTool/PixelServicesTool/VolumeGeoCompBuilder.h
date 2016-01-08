/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGeoModelUtils_VolumeGeoCompBuilder_H
#define InDetGeoModelUtils_VolumeGeoCompBuilder_H

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

/* namespace InDet{ */
/*   class GeoSimpleObject; */
/* } */

namespace InDetDD {
  class ServiceVolume;
 
  class VolumeGeoCompBuilder
  {
  public:
    VolumeGeoCompBuilder( const Zone & zone, const std::vector<const ServiceVolume * > & services);
    VolumeGeoCompBuilder(const std::vector<const ServiceVolume * > & services);
    VolumeGeoCompBuilder( const Zone & zone, const std::vector<const ServiceVolume * > & services,
			  const std::vector<const ServiceVolume * > & servEnv, const std::vector<const ServiceVolume * > & servChild );
    ~VolumeGeoCompBuilder();

    void setRegion(const std::string & region, double zcenter);
    void setMaterialManager(InDetMaterialManager * matManager) {m_matManager = matManager; }
    const std::vector<const ServiceVolume *> & services();
    const std::vector<const ServiceVolume * > & servicesEnv() ;
    const std::vector<const ServiceVolume * > & servicesChild() ;

    void buildAndPlace(const std::string & region, GeoPhysVol * parent, double zcenter = 0);
    void buildAndPlace(const std::string & region, GeoFullPhysVol * parent, double zcenter = 0);
    void buildAndPlaceEnvelope(const std::string & region, GeoFullPhysVol * parent, int iParent, int iElement, double zcenter = 0);
    void buildAndPlaceEnvelope(const std::string & region, GeoPhysVol * parent, int iParent, int iElement, double zcenter = 0);
    void addServices(const Zone & zone, const std::vector<const ServiceVolume * > & services);
    GeoVPhysVol* build(int iElement);
    int numCopies(int iElement);
    GeoTransform * getPlacement(int iElement, int iCopy,  bool forceRotation=false );
    GeoTransform * getPlacementEnvelope(int iElement, int iCopy,  int iMothElement);

    MsgStream& msg (MSG::Level lvl) const { return m_msg.get() << lvl; }
    bool msgLvl (MSG::Level lvl) const { return m_msg.get().level() <= lvl; }

    bool isEnvelopeOrChild(int iElement);
    int  getEnvelopeNum(int iElement);
    int  getParentNum(int iElement);
    bool isChildService(int iElt,int iChld);
    double getZcenter(int iElt);

    void resetServices();
    double getServiceNumber() { return 0;  /*m_svcObjects.size();*/ } 
/*     InDet::GeoSimpleObject* getServiceObject(int i) { return m_svcObjects[i]; } */

  private:
    //const GeoShape * getShape(const ServiceVolume & param, double & volume);
    
    mutable Athena::MsgStreamMember m_msg;
    std::string m_region;
    double m_zcenter;
    const std::vector<const ServiceVolume *>* m_services;
    const std::vector<const ServiceVolume *>* m_servEnvelope;
    const std::vector<const ServiceVolume *>* m_servChild;
    VolumeSplitter m_splitter;
    InDetMaterialManager * m_matManager;

/*     std::vector<InDet::GeoSimpleObject* >m_svcObjects; */

  };
 
}
#endif // InDetGeoModelUtils_VolumeBuidler_H
