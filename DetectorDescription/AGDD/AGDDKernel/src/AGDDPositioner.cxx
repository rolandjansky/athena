/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDPositioner.h"
#include "AGDDKernel/AGDDPositionerStore.h"
#include "AGDDKernel/AGDDVolume.h"
#include "AGDDKernel/AGDDVolumeStore.h"

AGDDPositioner::AGDDPositioner(AGDDPositionerStore& ps,
                               AGDDVolumeStore& vs,
                               const std::string& n,
                               const GeoTrf::Transform3D& t) :
  m_isSensitiveDetector(false),
  m_volume(n),
  m_transform(t) {
	ps.RegisterPositioner(this);
	m_theVolume=vs.GetVolume(Volume());
}

std::string AGDDPositioner::Volume() {
	return m_volume;
}

const GeoTrf::Transform3D& AGDDPositioner::Transform() const {
	return m_transform;
} 

AGDDVolume* AGDDPositioner::GetVolume() {
	return m_theVolume;
} 

