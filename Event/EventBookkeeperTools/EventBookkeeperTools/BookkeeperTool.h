/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOKKEEPERTOOL_H
#define BOOKKEEPERTOOL_H

/** @file BookkeeperTool.h
 *  @brief This file contains the class definition for the BookkeeperTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: BookkeeperTool.h 663679 2015-04-29 08:31:54Z krasznaa $
 **/

#include "AthenaBaseComps/AthAlgTool.h"
#include "AsgTools/AsgMetadataTool.h"
#include "AthenaKernel/IMetaDataTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/ICutFlowSvc.h"

#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"
#include "xAODCutFlow/CutBookkeeperAuxContainer.h"

#include <string>

/** @class BookkeeperTool
 *  @brief This class provides an example for reading with a ISelectorTool to veto events on AttributeList.
 **/


class BookkeeperTool : public AthAlgTool, public virtual ::IMetaDataTool
{
   //ASG_TOOL_CLASS0(BookkeeperTool)
public: // Constructor and Destructor
   /// Standard Service Constructor
   BookkeeperTool(const std::string& type, 
                  const std::string& name,
                  const IInterface*  parent);
   /// Destructor
   virtual ~BookkeeperTool();

public:
   virtual StatusCode metaDataStop(const SG::SourceID&);
   virtual StatusCode beginInputFile(const SG::SourceID& sid = "Serial");
   virtual StatusCode endInputFile(const SG::SourceID& sid = "Serial");
   virtual StatusCode initialize();
   virtual StatusCode finalize();
protected:
   ServiceHandle<StoreGateSvc> inputMetaStore() const;
   ServiceHandle<StoreGateSvc> outputMetaStore() const;

private:
  
  /// Helper class to update a container with information from another one
  StatusCode updateContainer( xAOD::CutBookkeeperContainer* contToUpdate,
                              const xAOD::CutBookkeeperContainer* otherCont );

  StatusCode initOutputContainer(const std::string& sgkey);

  StatusCode buildAthenaInterface(const std::string& inputName,
                                  const std::string& outputName,
                                  const SG::SourceID& sid);

  /// Fill Cutflow information
  StatusCode addCutFlow();
 
  /// Pointer to cut flow svc 
  ServiceHandle<StoreGateSvc> m_inputMetaStore;
  ServiceHandle<StoreGateSvc> m_outputMetaStore;

  /// The name of the output CutBookkeeperContainer
  std::string m_outputCollName;
  
  /// The name of the input CutBookkeeperContainer
  std::string  m_inputCollName;

  /// The name of the CutFlowSvc CutBookkeeperContainer
  std::string m_cutflowCollName;

  bool m_cutflowTaken;
  bool m_markIncomplete;

  /// List of source ids which have reached end file
  std::set<SG::SourceID> m_fullreads;
  std::set<SG::SourceID> m_read;
  std::set<SG::SourceID> m_written;

};

inline ServiceHandle<StoreGateSvc> BookkeeperTool::inputMetaStore() const
{
  return m_inputMetaStore;
}

inline ServiceHandle<StoreGateSvc> BookkeeperTool::outputMetaStore() const
{
  return m_outputMetaStore;
}

#endif

