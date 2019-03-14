/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOKKEEPERTOOL_H
#define BOOKKEEPERTOOL_H

/** @file BookkeeperTool.h
 *  @brief This class is an implementation of the GenericMetadataTool
 *  for the xAOD::CutBookkeeperContainer.
 *  @author Jack Cranshaw <cranshaw@anl.gov>
 *  $Id: $
 **/

#include "AthenaKernel/GenericMetadataTool.h"

#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"
#include "xAODCutFlow/CutBookkeeperAuxContainer.h"

#include <string>


class BookkeeperTool : public GenericMetadataTool <xAOD::CutBookkeeperContainer, xAOD::CutBookkeeperAuxContainer> 
{
public: 
   // Constructor and Destructor
   /// Standard Service Constructor
   BookkeeperTool(const std::string& type, 
                  const std::string& name,
                  const IInterface*  parent);
   /// Destructor
   virtual ~BookkeeperTool();

private:
  
  /// Virtual method to update a container with information from another one
  virtual StatusCode updateContainer(xAOD::CutBookkeeperContainer* contToUpdate,
                               const xAOD::CutBookkeeperContainer* otherCont );

};

#endif

