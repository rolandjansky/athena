/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HITRT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HITRT_H
#define HITRT_H

// Gaudi includes
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/DataHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "AthenaKernel/IOVSvcDefs.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/StatusCode.h"
//#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// EDM include(s):
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"


#include "TFile.h"
#include "TH1F.h"

#include <string>
using std::string;
using std::vector;

class AtlasDetectorID;
class Identifier;
class StoreGateSvc;
class ICoolHistSvc;

class ITRT_StrawStatusSummarySvc;
/** @class HITRT

    This algorithm is used to calculate event centrality bin
    based on calibration histograms of quantities monotonically
    corellated with centrality. It creates HITRTData object
    and saves it in SG.
    @author  Andrzej Olszewski <Andrzej.Olszewski@ifj.edu.pl>
*/  

class HITRT : public Algorithm
  {
  public:
    friend class HICentralityValidation;

    /** Standard Athena-Algorithm Constructor */
    HITRT(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~HITRT();
    
    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();

    StatusCode FillTRTNumber( vector<int> &m_OccupancyPerPart ); 
  private:

    //Main vector with occupancy have to be stored
    vector<int> m_OccupancyPerPart;
    /** indicates if we are processing Data */
    bool    m_isData;
    /** global trigger efficiency correction */
    //float   m_effCorr;
    /** indicates if initialization was succesfull */
    bool    m_initialized;


    StoreGateSvc* detStore;
    StoreGateSvc* evtStore;

    /** class member version of retrieving StoreGate */
    StoreGateSvc*  m_sgSvc;
    //StoreGateSvc*  p_detstore;
    // ICoolHistSvc* p_coolhistsvc;
    //std::string m_histfolder; // COOL folder to access
    //std::string m_histname; // histogram name
    //int m_channel; 

    /** name of the file with calibration histograms: */
    //string m_HICentralityCalibrationsFileName;
    /*
    int* m_countRDOhitsInEvent; ///< !!! Hits in event 
    int* m_countdeadstraws;///< !!! Noisy hits(straws) in event
    int* m_countBarrelhitsInEvent; ///< !!! Hits in event Barrel
    int* m_countEndCaphitsInEvent; ///< !!! Hits in event EndCap
    int* m_countEndCapAhitsInEvent; ///< !!! Hits in event EndCap A
    int* m_countEndCapChitsInEvent; ///< !!! Hits in event EndCap A
    */
  protected:
    ServiceHandle<ITRT_StrawStatusSummarySvc> m_sumSvc;

  }; 

#endif 
