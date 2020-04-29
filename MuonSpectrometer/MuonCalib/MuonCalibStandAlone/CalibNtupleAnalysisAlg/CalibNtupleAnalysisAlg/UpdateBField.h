/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_UpdateBField_h
#define MuonCalib_UpdateBField_h

#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

namespace MuonCalib {

  class UpdateBField: public AthAlgTool, virtual public CalibSegmentPreparationTool {
  public:
//===========================constructor - destructor===========================
    UpdateBField(const std::string &t, const std::string &n, const IInterface *p);
    ~UpdateBField()=default;
//===========================public member functions============================
    StatusCode initialize();
    void prepareSegments(const MuonCalibEvent *& /*event*/, std::map<NtupleStationId, MuonCalibSegment *> &segments);
  private:
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj"};
  }; //class

} //namespace MuonCalib
#endif
