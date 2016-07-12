/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "CalibNtupleAnalysisAlg/UpdateBField.h"


namespace MuonCalib {

UpdateBField::UpdateBField(const std::string &t, const std::string &n, const IInterface *p): AthAlgTool(t, n, p),   
m_fieldServiceHandle("AtlasFieldSvc",n) {
  declareProperty("MagFieldSvc", m_fieldServiceHandle);
  declareInterface< CalibSegmentPreparationTool >(this);
}
	
StatusCode UpdateBField::initialize(void) {
  ATH_CHECK( m_fieldServiceHandle.retrieve() );
  return  StatusCode::SUCCESS;
}
	
void UpdateBField::prepareSegments(const MuonCalibEvent *& /*event*/, std::map<NtupleStationId, MuonCalibSegment *> &segments) {
  for(std::map<NtupleStationId, MuonCalibSegment *>::iterator it = segments.begin(); it!=segments.end(); it++) {
    for(unsigned int j=0; j<it->second->mdtHitsOnTrack(); j++) {
      MdtCalibHitBase *hit = it->second->mdtHOT()[j];
//    const MuonFixedId & id(hit->identify());
      Amg::Transform3D gToStation= it->second->localToGlobal().inverse();
      double XYZ[3], BGXYZ[3];
      XYZ[0] = hit->globalPosition().x();
      XYZ[1] = hit->globalPosition().y();
      XYZ[2] = hit->globalPosition().z();
      m_fieldServiceHandle->getField(XYZ,BGXYZ);
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
