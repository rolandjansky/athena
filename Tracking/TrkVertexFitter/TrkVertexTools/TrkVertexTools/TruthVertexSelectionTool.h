/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexTools_TruthVertexSelectionTool_H
#define TrkVertexTools_TruthVertexSelectionTool_H
 
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexSelectionTool.h"
#include "GeneratorObjects/McEventCollection.h"

 /**
  * @class Trk::TruthVertexSelectionTool 
  *
  * @author Kirill Prokofiev, March 2012
  * A test class, meant to be an example of 
  * correct implementation for the veretx
  * selector. Selects the real -= truth associated =-
  * vertex as primary and thus corrects the reconstructuon.
  * So far, the closet vertex to the truth position  is
  * selected as the best one.
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
    
  
  class TruthVertexSelectionTool : public AthAlgTool, virtual public IVertexSelectionTool
  {
  public:
    StatusCode initialize();
    StatusCode finalize();
    
    /**
     * constructor
     */
    
    TruthVertexSelectionTool(const std::string& t, const std::string& n, const IInterface*  p);
    
    /**
     * destructor
     */
     
    virtual ~TruthVertexSelectionTool();
    
    virtual unsigned int findVertexInContainer(const xAOD::VertexContainer * ) const;
    
             
  private:
    SG::ReadHandleKey<McEventCollection> m_mcEventCollectionKey { this, "mcContainerName", "GEN_AOD", "key to retrieve MC truth" };
 
  }; //end of class description
 }//end of namespace definition
 
 #endif
