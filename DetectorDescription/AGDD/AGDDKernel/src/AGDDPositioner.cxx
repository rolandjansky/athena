/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDPositioner.h"
#include "AGDDKernel/AGDDPositionerStore.h"
#include "AGDDKernel/AGDDVolume.h"
#include "AGDDKernel/AGDDVolumeStore.h"

AGDDPositioner::AGDDPositioner(const std::string& n, GeoTrf::Transform3D t) :
  m_isSensitiveDetector(false),
  m_volume(n),
  m_transform(t) {
	AGDDPositionerStore::GetPositionerStore()->RegisterPositioner(this);
	m_theVolume=AGDDVolumeStore::GetVolumeStore()->GetVolume(Volume());
}

std::string AGDDPositioner::Volume() {
	return m_volume;
}

const GeoTrf::Transform3D& AGDDPositioner::Transform() {
	return m_transform;
} 

AGDDVolume* AGDDPositioner::GetVolume() {
	return m_theVolume;
} 

