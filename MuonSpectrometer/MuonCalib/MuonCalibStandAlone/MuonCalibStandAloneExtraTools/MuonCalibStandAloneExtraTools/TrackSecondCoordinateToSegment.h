/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_TrackSecondCoordinateToSegment_h
#define MuonCalib_TrackSecondCoordinateToSegment_h

//Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"

//root
class TFile;
class TH1F;
class TH2F;
namespace MuonCalib {

class MuonCalibSegment;
class MuonCalibExtendedTrack;

class TrackSecondCoordinateToSegment : public AthAlgTool, virtual public CalibSegmentPreparationTool {
 public:
//=========================constructor==========================================
  TrackSecondCoordinateToSegment(const std::string &t, const std::string &n, const IInterface *p);
  inline ~TrackSecondCoordinateToSegment() {}
//=========================public member functions==============================
  //initialize and finalize
  StatusCode initialize(void);
  StatusCode finalize(void);
  //load event
  void prepareSegments(const MuonCalibEvent *& /*event*/, std::map<NtupleStationId, MuonCalibSegment *> &segments);
 private:
//=========================private data=========================================
  //control histrograms - plot changes of x coordinate
  bool m_control_histos;
  TFile *m_root_file;
  TH1F *m_delta_x;
  TH1F *m_delta_sx;
  TH1F *m_delta_hit_x;
  inline void apply_2n_coordinate(const  MuonCalibExtendedTrack *track, MuonCalibSegment *segment) const;
  inline void updatePosition(Amg::Vector3D & position, const Amg::Vector3D & seg_pos, const Amg::Vector3D &seg_dir) const;
  //  inline void updatePosition(HepGeom::Point3D<double> &position, const HepGeom::Point3D<double> &seg_pos, const HepGeom::Vector3D<double> &seg_dir) const;
};
	
} //namespace MuonCalib	

#endif
