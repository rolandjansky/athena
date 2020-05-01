/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CalibNtupleAnalysisAlg/UpdateBField.h"
#include <TString.h> // for Form

namespace MuonCalib {

UpdateBField::UpdateBField(const std::string &t, const std::string &n, const IInterface *p) :
  AthAlgTool(t, n, p) {
  declareInterface< CalibSegmentPreparationTool >(this);
}
	
StatusCode UpdateBField::initialize() {
  ATH_CHECK(AthAlgTool::initialize());
  ATH_CHECK(m_fieldCondObjInputKey.initialize());
  return  StatusCode::SUCCESS;
}
	
void UpdateBField::prepareSegments(const MuonCalibEvent *& /*event*/, std::map<NtupleStationId, MuonCalibSegment *> &segments) {
  /// magnetic field
  MagField::AtlasFieldCache fieldCache;
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, Gaudi::Hive::currentContext()};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (!fieldCondObj) {
    throw std::runtime_error(Form("File: %s, Line: %d\nUpdateBField::prepareSegments() - Failed to retrieve AtlasFieldCacheCondObj with key %s", __FILE__, __LINE__, (m_fieldCondObjInputKey.key()).c_str()));
  }
  fieldCondObj->getInitializedCache(fieldCache);

  for(std::map<NtupleStationId, MuonCalibSegment *>::iterator it = segments.begin(); it!=segments.end(); it++) {
    for(unsigned int j=0; j<it->second->mdtHitsOnTrack(); j++) {
      MdtCalibHitBase *hit = it->second->mdtHOT()[j];
      Amg::Transform3D gToStation= it->second->localToGlobal().inverse();
      double XYZ[3], BGXYZ[3];
      XYZ[0] = hit->globalPosition().x();
      XYZ[1] = hit->globalPosition().y();
      XYZ[2] = hit->globalPosition().z();
      fieldCache.getField(XYZ,BGXYZ);
      Amg::Vector3D B_global(BGXYZ[0], BGXYZ[1], BGXYZ[2]);
      Amg::Vector3D B_loc(gToStation.linear()*B_global);
      hit->setBFieldPara(B_loc.x());
      Amg::Vector3D dir(0.0,
                        it->second->direction().y(),
                        it->second->direction().z());
      hit->setBFieldPerp(B_loc. dot(dir.unit()));
    }
  }
}  //end UpdateBField::prepareSegments

}//namespace MuonCalib
