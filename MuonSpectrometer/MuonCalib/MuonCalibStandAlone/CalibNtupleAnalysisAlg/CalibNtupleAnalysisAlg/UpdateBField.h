/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_UpdateBField_h
#define MuonCalib_UpdateBField_h

//Gaudi
#include "GaudiKernel/ServiceHandle.h"
//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

//BFieldAth
#include "MagFieldInterfaces/IMagFieldSvc.h"

namespace MuonCalib {

class UpdateBField: public AthAlgTool, virtual public CalibSegmentPreparationTool {
  public:
//===========================constructor - destructor===========================
    UpdateBField(const std::string &t, const std::string &n, const IInterface *p);
    inline ~UpdateBField() {}
//===========================public member functions============================
    //initialize and finalize
    StatusCode initialize(void);
    inline StatusCode finalize(void) {
      return StatusCode :: SUCCESS;
    }
    //update b-field
    void prepareSegments(const MuonCalibEvent *& /*event*/, std::map<NtupleStationId, MuonCalibSegment *> &segments);
  private:
//==========================private data========================================
    //handle of b-field service - job option
    ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle;
  }; //class

} //namespace MuonCalib
#endif
