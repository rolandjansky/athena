/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_TrackAnalysisH
#define MuonCalib_TrackAnalysisH

#include <iostream>
#include <vector>
#include <math.h>  
#include <fstream>      
#include <string> 
#include <iomanip>  
#include <sstream> 

// For the Extra Tree Analysis //
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedTrack.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedSegment.h"
#include "MuonCalibExtraTreeEvent/EventHandler.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibEvent_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibSelector.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibCaloHit.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrackSummary.h"
#include "MuonCalibExtraTreeEvent/MuonCalibSelector.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTriggerInfo.h"
// #include "MuonCalibExtraNtuple/NTReaderBase_E.h"
// MuonCalib //
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MdtCalibFitters/QuasianalyticLineReconstruction.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MuonCalibPattern.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibTruthCollection.h"

#include "MuonCalibStandAloneExtraTools/HistogramManager.h"
#include "MuonCalibStandAloneExtraTools/SegmentAnalysis.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

//using namespace std;

namespace MuonCalib{

class MdtDqaTrackAnalysis{

 private:
  int m_TrkAuthor;
  HistogramManager *m_histoManager;
  bool m_verbose;
  // pointer to region selection service
  RegionSelectionSvc *p_reg_sel_svc;
  int number_IDhits(MuonCalibSelector::TrackVec::const_iterator, int);
  int cutflow(MuonCalibSelector::TrackVec::const_iterator, double , int , int , int , int );
  int MDTtiming(const std::vector<const MuonCalibExtendedSegment*> &segments);
  //void MDTtiming(const std::vector<MuonCalibSegment*>&);

 public:
  MdtDqaTrackAnalysis(RegionSelectionSvc*,int, HistogramManager*, bool);
  void handleEvent(const MuonCalibEvent&, int , const std::vector<MuonCalibSegment*>&, unsigned int);

};

}  //namespace MuonCalib

#endif

