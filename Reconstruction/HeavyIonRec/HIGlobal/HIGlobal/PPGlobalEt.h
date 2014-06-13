/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PPGlobalEt.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PPGLOBALET_H
#define PPGLOBALET_H

#include "HIGlobal/HICaloUtil.h"
// Gaudi includes
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include <string>

class AtlasDetectorID;
class Identifier;
class StoreGateSvc;

class CaloCellContainer;

// No NameSpace selected 
/** @class PPGlobalEt
 *   @author  Adam Trzupek <Adam.Trzupek@ifj.edu.pl>
 *   @author  Dominik Derendarz <Dominik.Derendarz@ifj.edu.pl>
 *   @date 26 June 2009.
 *   @brief This calss is the algorithm for global Et reconstruction in case of pp data analysis, 
 *   @brief any varialble ending with _pp are using for pp analysis. 
 */  

class PPGlobalEt : public Algorithm
{
    public:

     /** Standard Athena-Algorithm Constructor */
     PPGlobalEt(const std::string& name, ISvcLocator* pSvcLocator);
     /** Default Destructor */
     ~PPGlobalEt();
     /** Function which retrieve cell Et from CaloCellContainer */ 
     float GetEt_pp(const CaloCellContainer * cCell);   
     /** Function which retrieve cell Et from CaloCellContainer and multiplicate it by proper calibration factor depending on cell's eta */
     float* GetEtvsEta_pp(const CaloCellContainer * cCell,HICaloUtil *m_correction_pp); 


     /** standard Athena-Algorithm method */
     StatusCode          initialize();
     /** standard Athena-Algorithm method */
     StatusCode          execute();
     /** standard Athena-Algorithm method */
     StatusCode          finalize();

    private:
    
    /** class member version of retrieving MsgStream */
    mutable MsgStream                 m_log;

    /** class member version of retrieving StoreGate */
    StoreGateSvc*  m_sgSvc;

    /** member variables for algorithm properties: */
    float *m_et_vs_eta_pp;
     
    HICaloUtil *m_correction_pp;
    
};

#endif