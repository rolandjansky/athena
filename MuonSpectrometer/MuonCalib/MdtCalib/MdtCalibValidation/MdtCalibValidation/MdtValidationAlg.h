/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _MDTVALIDATIONALG_H
#define _MDTVALIDATIONALG_H

// c - c++ - system
#include "vector"
#include "map"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string> 
#include <stdio.h>
#include <sys/time.h>

//root
#include "TFile.h"
#include "TList.h"
#include "TH1.h"
#include "TH2.h"
#include "TList.h"
#include "TF1.h"

//athena
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" 

// AtlasCore //
#include "StoreGate/StoreGateSvc.h"


//MdtCalibData
#include "MdtCalibData/MdtTubeFitContainer.h"

class RegionSelectionSvc;

class MdtIdHelper;
namespace MuonGM {
  class MuonDetectorManager;
}

namespace MuonCalib {

  class NtupleStationId;
  class SamplePoint;
  class CalibDbConnection;
  class RtFullInfo;
  class CalibRtDbOperations;
  class CalibT0DbOperations;
  class CalibHeadOperations;
  
  class MdtValidationAlg : public AthAlgorithm {
    //===========================================
  public:
    /** Algorithm Constructor */
    MdtValidationAlg(const std::string& name, ISvcLocator* pSvcLocator);
    /** Algorithm destrucrtor*/
    ~MdtValidationAlg();
    /** Is called at the beginning of the analysis */
    StatusCode initialize();
    /** execute function NOTE: This will read all events at once*/
    StatusCode execute();   
    /** finalize functions */
    StatusCode finalize();

    void filltubeT0();
    void ValidateTubeT0 (const MdtTubeFitContainer::SingleTubeFit *fit, int & validflag);
 
    int CheckT0Parameters (const MdtTubeFitContainer::SingleTubeFit *fit);
    int CheckRtParameters (std::vector<SamplePoint> & points, const RtFullInfo * full_info);

    void SummaryReport(int success);
    ////  void ValidateTubeT0AllTimes(bool something_done);

    StatusCode ValidateChamberT0(NtupleStationId & id);

    ////  void ValidateChamberT0AllTimes(bool something_done);

    StatusCode ValidateRegionRt(const NtupleStationId & id);
    const char* NameSelectedChamber();
    bool ReadLimitsFile();
    bool ComputeLimits();
    bool Histos(NtupleStationId & id, const std::vector<int> & validflags = std::vector<int>(0));
    bool RootFile(bool good_tubes=false);

    //============================================
  private:
    // Methods
    //========
    /** Calibration DataBase handling methods */
    StatusCode ConnectDb();
    
    /** */
    double getLeftLimit(TH1F * histo);
    double getRightLimit(TH1F * histo);

    //std::vector <MdtOracleTubeTable *> refinelist(std::vector <MdtOracleTubeTable *> tubelist);

/** check if tube exists */
	inline bool exists(NtupleStationId & id, int ml, int ly, int tb);
        
    // Attributes
    //===========
 
    //! pointer to region selection service
    RegionSelectionSvc *p_reg_sel_svc;
    
    std::vector<MuonCalib::NtupleStationId> m_region_ids;
    
    MdtTubeFitContainer *  m_tube_chamber;
    
    // helpers //
    std::string m_detector_store; // name of the detector store
    std::string m_MDT_ID_helper; // name of the MDT ID helper
    
    StoreGateSvc *m_detStore; // pointer to the detector store
    const MdtIdHelper *m_MdtIdHelper; // pointer to the MDT ID helper
    
    const MuonGM::MuonDetectorManager *m_detMgr; // pointer to the muon
    // detector manager
  
    CalibDbConnection * m_db;
    CalibT0DbOperations * m_t0_op;
    CalibRtDbOperations * m_rt_op;
    CalibHeadOperations * m_head_ops;
    CalibDbConnection * m_data_connection;
    int m_fitter;
    bool m_writeToDbEnable ;
    int m_limitslevel;
    int m_makeHistos;
    int m_lastdate;
    std::string m_validationTask;

    std::string m_db_ConnectionString;
    std::string m_writer_ConnectionString;
    std::string m_db_WorkingSchema;
    std::string m_defaultRtFile;
    std::string m_reader_account, m_reader_password;
    std::string m_writer_account, m_writer_password;
    timeval m_t0,m_t1,m_result;
    
  
    /** limits for T0 acceptance */
    double m_minSlope;
    double m_maxSlope;
    double m_minT0;
    double m_maxT0;
    double m_minTmax;
    double m_maxTmax;
    double m_minTtot;
    double m_maxTtot;
    double m_minPP;
    double m_maxPP;

    /** limits for Rt acceptance */
    int	m_minSegs;
    double m_minDAngle;
   
    float m_summary[50] ;
    std::string m_limitsFile ;
    //! output file
    TFile* m_Histos;
    TList* m_HistosList;
    
    int m_headid,m_lowrun,m_uprun;
    std::string m_sitename;

    bool m_ComputeLimits;
    std::string m_chamber;
    int m_debug;
    int m_tube;
    int m_eta;
    int m_phi;
  };
 
}
#endif
