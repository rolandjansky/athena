/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_ApplyRawTimes_H
#define MuonCalib_ApplyRawTimes_H


//Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"

class RegionSelectionSvc;

namespace MuonCalib {
class IIdToFixedIdTool;

class ApplyRawTimes : public AthAlgTool, virtual public CalibSegmentPreparationTool {
 public:
//=========================constructor==========================================
  ApplyRawTimes(const std::string &t, const std::string &n, const IInterface *p);
  inline ~ApplyRawTimes() {}
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
  //! pointer to regino selectin service
  ServiceHandle<RegionSelectionSvc> m_reg_sel_svc;
		
};

}//namespace MuonCalib
#endif
