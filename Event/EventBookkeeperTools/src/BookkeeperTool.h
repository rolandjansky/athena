/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOKKEEPERTOOL_H
#define BOOKKEEPERTOOL_H

/** @file BookkeeperTool.h
 *  @brief This class is an implementation of the AsgMetadataTool
 *  for the xAOD::CutBookkeeperContainer.
 *  @author Jack Cranshaw <cranshaw@anl.gov>
 *  $Id: $
 **/

//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AsgTools/AsgMetadataTool.h"
#include "AthenaKernel/ICutFlowSvc.h"
#ifndef XAOD_STANDALONE
#include "AthenaKernel/IMetaDataTool.h"
#endif // XAOD_STANDALONE
#include "GaudiKernel/ServiceHandle.h"

#include <xAODCutFlow/CutBookkeeperContainer.h>

class CutFlowSvc;

class BookkeeperTool : public asg::AsgMetadataTool
#ifndef XAOD_STANDALONE
                     , public virtual ::IMetaDataTool
#endif // XAOD_STANDALONE
{
   ASG_TOOL_CLASS0(BookkeeperTool)
public: // Constructor and Destructor
   /// Standard Service Constructor
   BookkeeperTool(const std::string& name = "BookkeeperTool");
   /// Destructor
   virtual ~BookkeeperTool();

public:
   virtual StatusCode metaDataStop() override;
   virtual StatusCode beginInputFile() override {return StatusCode::SUCCESS;}
   virtual StatusCode endInputFile() override {return StatusCode::SUCCESS;}
   virtual StatusCode beginInputFile(const SG::SourceID&) override;
   virtual StatusCode endInputFile(const SG::SourceID&) override;
   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;

private:
  
  /// Helper class to update a container with information from another one
  StatusCode updateContainer( xAOD::CutBookkeeperContainer* contToUpdate,
                              const xAOD::CutBookkeeperContainer* otherCont );

  StatusCode copyContainerToOutput(const std::string& outname);

  /// Fill Cutflow information
  StatusCode addCutFlow();
 
  /// Pointer to the public CutFlowSvc interface
  ServiceHandle<ICutFlowSvc> m_cutFlowSvc{ this, "CutFlowSvc", "CutFlowSvc/CutFlowSvc", "Pointer to the CutFlowSvc"};
  /// Direct pointer to the CutFlowSvc for "private" methods access
  const CutFlowSvc *m_cutFlowSvcPrivate;

  /// The name of the output CutBookkeeperContainer
  std::string m_outputCollName;
  
  /// The name of the input CutBookkeeperContainer
  std::string  m_inputCollName;

  bool m_cutflowTaken;

};

#endif


