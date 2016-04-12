/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_SegmentAnalysisH
#define MuonCalib_SegmentAnalysisH

#include <iostream>
#include <vector>
#include <math.h>  
#include <fstream>      
#include <string> 
#include <iomanip>  
#include <sstream> 

// MuonCalib //
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MdtCalibFitters/QuasianalyticLineReconstruction.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"

#include "MuonCalibStandAloneExtraTools/HistogramManager.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

//using namespace std;

namespace MuonCalib{

class SegmentAnalysis{

 private:
  HistogramManager *m_histoManager;
  // pointer to region selection service
  RegionSelectionSvc *p_reg_sel_svc;
  bool m_verbose;
  int multiChambSeg( MuonCalibSegment *segment  );
  int segmentStation( MuonCalibSegment *segment  );
  int segmentEta( MuonCalibSegment *segment  );
  int segmentPhi( MuonCalibSegment *segment  );

  int m_SectorMin;
  int m_SectorMax;
 
  bool m_doHitResids;

 public:
  SegmentAnalysis(RegionSelectionSvc*,HistogramManager*, bool, bool);
  void handleEvent(const MuonCalibEvent&, int , const std::vector<MuonCalibSegment*>&, unsigned int position);
};

}

#endif
