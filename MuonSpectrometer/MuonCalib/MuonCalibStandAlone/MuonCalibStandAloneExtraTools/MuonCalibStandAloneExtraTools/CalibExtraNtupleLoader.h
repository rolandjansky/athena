/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_CalibExtraNtupleLoader_h
#define MuonCalib_CalibExtraNtupleLoader_h

//Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"

//c - c++
#include "string"

class RegionSelectionSvc;
class TChain;

namespace MuonCalib {

class NTReader_E;

//load events from CalibrationNtuple

class CalibExtraNtupleLoader: public AthAlgTool, virtual public CalibSegmentPreparationTool {
 public:
//=========================constructor==========================================
  CalibExtraNtupleLoader(const std::string &t, const std::string &n, const IInterface *p);
  inline ~CalibExtraNtupleLoader() {}
//=========================public member functions==============================
  //initialize and finalize
  StatusCode initialize(void);
  inline StatusCode finalize(void) {
    return StatusCode::SUCCESS;
  }
  //load event
  void prepareSegments(const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> &segments);
 private:
//=========================private data=========================================
  //!path to the file list - job option
  std::string m_filelist;
  //! type of ntuple - AUTO/NORMAL/REGION
  //! NOTE: Do not use auto if you are directly reading ntuples from castor or dcache. 
  //! In this case all files have to be opened at initialisation.
  std::string m_ntuple_type;
  int m_ntuple_type_num;
  //! first event - jobOption
  int m_first;
  //! last event - jobOption
  int m_last;
  //! ntuple reader
  NTReader_E *m_reader;
  //! pointer to region selection service
  RegionSelectionSvc *p_reg_sel_svc;
  //! ntuple chain
  TChain *m_chain;
};
	
} //namespace MuonCalib

#endif
