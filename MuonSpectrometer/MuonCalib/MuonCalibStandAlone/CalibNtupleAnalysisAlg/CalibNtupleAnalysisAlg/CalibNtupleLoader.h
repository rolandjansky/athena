/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_CalibNtupleLoader_h
#define MuonCalib_CalibNtupleLoader_h

//Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"

//c - c++
#include "string"

class RegionSelectionSvc;
class StoreGateSvc ;
class IIncidentSvc ;
class TChain;

namespace MuonCalib {

class NTReader;

//load events from CalibrationNtuple

class CalibNtupleLoader: public AthAlgTool, virtual public CalibSegmentPreparationTool {
 public:
//=========================constructor==========================================
  CalibNtupleLoader(const std::string &t, const std::string &n, const IInterface *p);
  inline ~CalibNtupleLoader() {}
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
  //!path to the file list - job option
  std::string m_filelist;
  //! type of ntuple - AUTO/NORMAL/REGION

  //! NOTE: Do not use auto if you are directly reading ntuples from
  //! castor or dcache. In this case all files have to be opened at initialisation.
  std::string m_ntuple_type;
  int m_ntuple_type_num;
  //! skip double events (event numbers) - job options
  bool m_skip_double_events;
  //!prev event number
  int m_prev_event_nr;
  //! prev run number
  unsigned int m_prev_run_nr ;

  //! first event - jobOption
  int m_first;
  //! last event - jobOption
  int m_last;
  //! ntuple reader
  NTReader *m_reader;
  //! pointer to region selection service
  ServiceHandle<RegionSelectionSvc> m_reg_sel_svc;
  //! ntuple chain
  TChain *m_chain;

  //! pointer to StoreGateSvc to manipulate EventInfo
  ServiceHandle<StoreGateSvc> m_sgSvc ;

  //! pointer to IncidentSvc to fire Incidents for IOV changes
  ServiceHandle<IIncidentSvc> m_incSvc ;
  inline StatusCode read_dictionary(std::istream &fl, int &tot_count);

};
	
} //namespace MuonCalib
#endif
