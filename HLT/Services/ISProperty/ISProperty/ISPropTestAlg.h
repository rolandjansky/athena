/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISPropTestAlg.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISPROPTESTALG_H
#define ISPROPTESTALG_H

// Gaudi includes
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

//Need to derive from ISPropertyMgr
#include "ISProperty/ISPropertyMgr.h"

//Some example services and toold we want to interact with
#include "ISProperty/IISPropertySvc.h"
#include "ISProperty/IISPropTestAlgTool.h"

/** @class ISPropTestAlg
 
    @brief Test alg for ISPropertySvc
    
    @author  Sebastian Boeser <sboeser@hep.ucl.ac.uk>
*/  

class ISPropTestAlg : public Algorithm ,
                     public ISPropertyMgr
{
  public:

     /** Standard Athena-Algorithm Constructor */
     ISPropTestAlg(const std::string& name, ISvcLocator* pSvcLocator);

     /** standard Athena-Algorithm method */
     StatusCode          initialize();
     /** standard Athena-Algorithm method */
     StatusCode          execute();
     /** standard Athena-Algorithm method */
     StatusCode          finalize();

     // update handler is called if property changes
     void updateHandler(Property& p);

  private:
    
    /** member variables for algorithm properties: */
    int  m_myInt;
    float  m_Delay;
    

    /** event counter - this property will not be declared as a jobOption */
    LongProperty  m_Events;    //!< Number of events seen so far

    /** Service handle to an example service which we can publish properties from */
    ServiceHandle<IISPropertySvc> m_AnyService;
    /** Service handle to an example tool which we can publish properties from */
    ToolHandle<IISPropTestAlgTool> m_AnyTool;

    /** class member version of retrieving MsgStream */
    mutable MsgStream                 log;



}; 

#endif 
