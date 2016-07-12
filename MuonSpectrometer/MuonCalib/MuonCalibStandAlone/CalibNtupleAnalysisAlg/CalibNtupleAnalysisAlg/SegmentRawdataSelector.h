/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_SegmentRawdataSelector_h
#define MuonCalib_SegmentRawdataSelector_h

//Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"

//c - c++
#include "set"

#include "GeoPrimitives/GeoPrimitives.h"

class RegionSelectionSvc;
class StoreGateSvc;
class MdtIdHelper;


namespace MuonGM {
class MuonDetectorManager;
}

namespace MuonCalib {
class IIdToFixedIdTool;
class MuonCalibRawMdtHit;
class MuonCalibRawHitCollection;

class SegmentRawdataSelector : public AthAlgTool, virtual public CalibSegmentPreparationTool {
 public:
//=========================constructor==========================================
  SegmentRawdataSelector(const std::string &t, const std::string &n, const IInterface *p);
  inline ~SegmentRawdataSelector() {}
//=========================public member functions==============================
  //initialize and finalize
  StatusCode initialize(void);
  inline StatusCode finalize(void) {
    return StatusCode :: SUCCESS;
  }
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
  std::string m_MDT_ID_helper; // name of the MDT ID helper
  std::string m_idToFixedIdToolType; // type of the muon fixed id tool
  std::string m_idToFixedIdToolName; // name of the muon fixed id tool
  const MdtIdHelper *m_MdtIdHelper;  // pointer to the MDT ID helper
  const MuonGM::MuonDetectorManager *m_detMgr; // pointer to the muon detector manager
  const MuonCalib::IIdToFixedIdTool *m_id_tool;// identifier converter
  //store segments, because we have to delete them
  std::set<MuonCalibSegment *> m_segments;
/** get a the position of the raw hit - search for matching rpc-hits in order to get the x coordinate */
  inline const Amg::Vector3D get_raw_position(const MuonCalibRawMdtHit *hit, const MuonCalibRawHitCollection *coll, const Amg::Transform3D &local_to_global) const;
		
};

} //namespace MuonCalib
#endif
