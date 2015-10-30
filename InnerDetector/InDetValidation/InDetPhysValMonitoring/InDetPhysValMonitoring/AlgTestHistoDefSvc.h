/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AlgTestHistoDefSvc_h
#define AlgTestHistoDefSvc_h
/**
 * @file AlgTestHistoDefSvc.h
 * Header file for Algorithm to test histogram definition svc
 * @author Shaun Roe
 * @date 5 September, 2015
 **/
//STL
#include <string>

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

//package includes
#include "IHistogramDefinitionSvc.h"

class ISvcLocator;
class StatusCode;

/**
 *  AlgTestHistoDefSvc exercises the routines of the histogram definition service
 **/
class AlgTestHistoDefSvc:public AthAlgorithm{
  public:
    AlgTestHistoDefSvc( const std::string & name, ISvcLocator * pSvcLocator);
    ~AlgTestHistoDefSvc();
    // Standard Gaudi functions
    StatusCode initialize(); //!< Gaudi initialiser
    StatusCode execute();    //!< Gaudi executer
    StatusCode finalize();   //!< Gaudi finaliser
    
  private:
    ServiceHandle<IHistogramDefinitionSvc> m_histoDefSvc;
};
#endif
