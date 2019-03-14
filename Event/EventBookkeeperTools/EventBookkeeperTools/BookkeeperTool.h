/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOKKEEPERTOOL_H
#define BOOKKEEPERTOOL_H

/** @file BookkeeperTool.h
 *  @brief This class is an implementation of the GenericMetadataTool
 *  for the xAOD::CutBookkeeperContainer.
 *  @author Jack Cranshaw <cranshaw@anl.gov>
 *  $Id: $
 **/

//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AsgTools/AsgMetadataTool.h"
#ifdef ASGTOOL_ATHENA
#include "AthenaKernel/IMetaDataTool.h"
#endif // ASGTOOL_ATHENA
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/ICutFlowSvc.h"

#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"
#include "xAODCutFlow/CutBookkeeperAuxContainer.h"

#include <string>


class BookkeeperTool : public asg::AsgMetadataTool
#ifdef ASGTOOL_ATHENA
                     , public virtual ::IMetaDataTool
#endif // ASGTOOL_ATHENA
{
   ASG_TOOL_CLASS0(BookkeeperTool)
public: // Constructor and Destructor
   /// Standard Service Constructor
   BookkeeperTool(const std::string& name = "BookkeeperTool");
   /// Destructor
   virtual ~BookkeeperTool();

public:
   //void handle(const Incident& incident);
   virtual StatusCode metaDataStop();
   virtual StatusCode beginInputFile() override {return StatusCode::SUCCESS;}
   virtual StatusCode endInputFile() override {return StatusCode::SUCCESS;}
   virtual StatusCode beginInputFile(const SG::SourceID&) override;
   virtual StatusCode endInputFile(const SG::SourceID&) override;
   virtual StatusCode initialize();
   virtual StatusCode finalize();

private:
  
  /// Helper class to update a container with information from another one
  StatusCode updateContainer( xAOD::CutBookkeeperContainer* contToUpdate,
                              const xAOD::CutBookkeeperContainer* otherCont );

  StatusCode copyContainerToOutput(const std::string& outname);

  /// Fill Cutflow information
  StatusCode addCutFlow();
 
  /// Pointer to cut flow svc 
  //ServiceHandle<ICutFlowSvc> m_cutflowsvc;

  /// The name of the output CutBookkeeperContainer
  std::string m_outputCollName;
  
  /// The name of the input CutBookkeeperContainer
  std::string  m_inputCollName;

  /// The name of the CutFlowSvc CutBookkeeperContainer
  std::string m_cutflowCollName;

  bool m_cutflowTaken;

};

#endif


