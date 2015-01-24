/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HICentrality.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HICENTRALITY_H
#define HICENTRALITY_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TFile.h"
#include "TH1F.h"

#include <string>

class ICoolHistSvc;

/** @class HICentrality

    This algorithm is used to calculate event centrality bin
    based on calibration histograms of quantities monotonically
    corellated with centrality. It creates HICentralityData object
    and saves it in SG.
    @author  Andrzej Olszewski <Andrzej.Olszewski@ifj.edu.pl>
*/  

class HICentrality : public AthAlgorithm
  {
  public:
    friend class HICentralityValidation;

    /** Standard Athena-Algorithm Constructor */
    HICentrality(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~HICentrality();
    
    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();
    
    StatusCode          HijingParsVect(std::vector<float> &hijing_event_params);
    StatusCode          CaloCellEnergy(std::vector<float> &energy_by_detector);
    StatusCode          NumberOfSiClusters(std::vector<float> &npix_clusters, 
                                           std::vector<float> &nsct_clusters);
    
  private:

    StatusCode callBack( IOVSVC_CALLBACK_ARGS );

    bool binsearch (TH1F*, float, unsigned short &);
    unsigned short corrbin (unsigned short, float);

    /** pointer to file with calibration histograms */
    TFile  *m_calibfile;
    /** user settable percentage schema 
	for centrality bin definition */
    std::vector<unsigned short> m_CentralityPercSchema;
    /** indicates if we are processing Data */
    bool    m_isData;
    /** global trigger efficiency correction */
    float   m_effCorr;
    /** indicates if initialization was succesfull */
    bool    m_initialized;

    /** pointers to calibration histograms */
    TH1   *m_b_perch;
    TH1   *m_nw_perch;
    TH1   *m_ncoll_perch;
    TH1   *m_calocell_energy_perch;
    TH1   *m_number_of_siclusters_perch;    

    ServiceHandle<ICoolHistSvc> m_coolhistsvc;
    std::string m_histfolder; // COOL folder to access
    std::string m_histname; // histogram name
    int m_channel; 

    /** name of the file with calibration histograms: */
    std::string m_HICentralityCalibrationsFileName;

  }; 

#endif 
