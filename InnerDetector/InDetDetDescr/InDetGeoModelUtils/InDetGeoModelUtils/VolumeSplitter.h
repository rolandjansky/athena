/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGeoModelUtils_VolumeSplitter_H
#define InDetGeoModelUtils_VolumeSplitter_H

#include "InDetGeoModelUtils/VolumeSplitterUtils.h"

#include <string>
#include <vector>

namespace InDetDD {
  class ServiceVolume;
  
  class VolumeSplitter
  {
  public:
    VolumeSplitter();
    ~VolumeSplitter();
    const std::vector<const ServiceVolume *> & splitAll(const Zone & zone,  const std::vector<const ServiceVolume *>);
    void split(const Zone & zone, const ServiceVolume & origVolume);
    const std::vector<const ServiceVolume *> & getVolumes() const {return m_volumes;}
    // Take ownership of volumes. If called user must delete the volumes. 
    void takeOwnership() {m_ownVolumes = false;}
  private:
    Ray makeRay(const ServiceVolume & origVolume);
    void makeVolumes(const Zone * zone, const ServiceVolume & origVolume);
    void splitVolume(const Zone * zone, const ServiceVolume & vol);
    void adjustR(const ServiceVolume & param, ServiceVolume * paramNew);

    bool m_ownVolumes;
    double m_epsilon;
    std::vector<const ServiceVolume *> m_volumes;
  };
 
}
#endif // InDetGeoModelUtils_VolumeSplitter_H
