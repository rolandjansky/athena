/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexTools_DummyVertexSelectionTool_H
#define TrkVertexTools_DummyVertexSelectionTool_H
 
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexSelectionTool.h"

#include "GaudiKernel/RndmGenerators.h"
#include "AthenaKernel/IAthRNGSvc.h"

 /**
  * @class Trk::DummyVertexSelectionTool 
  *
  * @author Kirill Prokofiev, January 2012
  * A test class, meant to be an example of 
  * correct implementation for the veretx
  * selector.
  *
  * ---------------------------------------------------
  * Changes:
  *
  * David Shope <david.richard.shope@cern.ch> (2016-06-01)
  *
  *   EDM Migration to xAOD - replace VxContainer with xAOD::VertexContainer
  *
  */
 
 namespace Trk
 {
    
  
  class DummyVertexSelectionTool : public extends<AthAlgTool, IVertexSelectionTool>
  {
  public:
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    
    /**
     * constructor
     */
    
    DummyVertexSelectionTool(const std::string& t, const std::string& n, const IInterface*  p);
    
    /**
     * destructor
     */
     
    virtual ~DummyVertexSelectionTool();
    
    virtual unsigned int findVertexInContainer(const xAOD::VertexContainer * ) const override;
    
             
  private:
  
  /** Pointer to the random number generator service */
    ServiceHandle<IAthRNGSvc>     m_randomSvc 		     //!< Random Svc
    { this, "RndmSvc", "AthRNGSvc", ""};
    std::string  		  m_randomEngineName;	     //!< Name of the random number stream
  }; //end of class description
 }//end of namespace definition
 
 #endif
