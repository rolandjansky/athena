/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_SegmentRawdataSelector_h
#define MuonCalib_SegmentRawdataSelector_h

//Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"

#include "set"

#include "GeoPrimitives/GeoPrimitives.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

class RegionSelectionSvc;

namespace MuonCalib {
class IIdToFixedIdTool;
class MuonCalibRawMdtHit;
class MuonCalibRawHitCollection;

class SegmentRawdataSelector : public AthAlgTool, virtual public CalibSegmentPreparationTool {
 public:
//=========================constructor==========================================
  SegmentRawdataSelector(const std::string &t, const std::string &n, const IInterface *p);
  inline ~SegmentRawdataSelector()=default;
//=========================public member functions==============================
  //initialize and finalize
  StatusCode initialize();

  //load event
  void prepareSegments(const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> &segments);
 private:
//=========================private data=========================================
  //minimum and maximum number of segment hits
  int m_min_hits, m_max_hits;
  //adc cut on raw hits
  int m_adc_cut;
  //! pointer to region selection service
  ServiceHandle<RegionSelectionSvc> m_reg_sel_svc;
  // helpers //
  std::string m_idToFixedIdToolType; // type of the muon fixed id tool
  std::string m_idToFixedIdToolName; // name of the muon fixed id tool
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  // MuonDetectorManager from the conditions store
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};    

  const MuonCalib::IIdToFixedIdTool *m_id_tool;// identifier converter
  //store segments, because we have to delete them
  std::set<MuonCalibSegment *> m_segments;
/** get a the position of the raw hit - search for matching rpc-hits in order to get the x coordinate */
  inline const Amg::Vector3D get_raw_position(const MuonCalibRawMdtHit *hit, const MuonCalibRawHitCollection *coll, const Amg::Transform3D &local_to_global) const;
		
};

} //namespace MuonCalib
#endif
