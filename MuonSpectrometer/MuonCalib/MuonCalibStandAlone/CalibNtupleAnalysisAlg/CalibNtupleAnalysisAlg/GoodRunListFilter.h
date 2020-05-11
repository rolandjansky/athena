/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib__GoodRunListFilter_h
#define MuonCalib__GoodRunListFilter_h

//Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h" 
#include "AthenaBaseComps/AthAlgTool.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"

//GoodRunsLists
#include "GoodRunsLists/IGoodRunsListSelectorTool.h"

namespace MuonCalib {

class GoodRunListFilter: public AthAlgTool, virtual public CalibSegmentPreparationTool {
  public:
//=========================constructor==========================================
    GoodRunListFilter(const std::string &t, const std::string &n, const IInterface *p);
    ~GoodRunListFilter()=default;
//=========================public member functions==============================
    StatusCode initialize(void);
    //load event
    void prepareSegments(const MuonCalibEvent *& /*event*/, std::map<NtupleStationId, MuonCalibSegment *> &segments);
  private:
    ToolHandle<IGoodRunsListSelectorTool> m_good_runlist_tool;
  };

} //namespace MuonCalib
#endif
