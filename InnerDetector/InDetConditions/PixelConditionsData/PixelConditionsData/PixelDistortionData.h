/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsData/PixelDistortionData.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date Octorber, 2019
 * @brief Hold pixel distortion data produced by PixelDistortionAlg.
 */

#ifndef PIXELDISTORTIONDATA_H
#define PIXELDISTORTIONDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <map>

#include "GeoPrimitives/GeoPrimitives.h"
#include "AthenaKernel/CondCont.h"

class PixelDistortionData {
  public:
    void setVersion(int version);
    void setDistortionMap(std::unordered_map<uint32_t,std::vector<float>> distortionMap);

    int getVersion() const;
    std::vector<float> getDistortionMap(uint32_t id) const;

    Amg::Vector2D correction(uint32_t hashID, const Amg::Vector2D & locpos, const Amg::Vector3D & direction) const;
    Amg::Vector2D correctReconstruction(uint32_t hashID, const Amg::Vector2D & locpos, const Amg::Vector3D & direction) const;
    Amg::Vector2D correctSimulation(uint32_t hashID, const Amg::Vector2D & locpos, const Amg::Vector3D & direction) const;
 
    void clear();

  private:
    int m_version;
    std::unordered_map<uint32_t,std::vector<float>> m_distortionMap;
};

inline void PixelDistortionData::setVersion(int version) { m_version=version; }
inline void PixelDistortionData::setDistortionMap(std::unordered_map<uint32_t,std::vector<float>> distortionMap) { m_distortionMap=distortionMap; }

inline int PixelDistortionData::getVersion() const { return m_version; }

CLASS_DEF( PixelDistortionData , 127567278 , 1 )

CONDCONT_DEF( PixelDistortionData, 127567679 );

#endif
