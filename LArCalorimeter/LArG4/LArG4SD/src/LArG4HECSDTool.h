/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4HECSDTOOL_H
#define LARG4HECSDTOOL_H

#include "LArG4Code/LArG4SDTool.h"
#include <string>
#include <vector>

#include "StoreGate/WriteHandle.h"
#include "LArSimEvent/LArHitContainer.h"

class LArG4SimpleSD;

class LArG4HECSDTool : public LArG4SDTool
{
 public:
  // Constructor
  LArG4HECSDTool(const std::string& type, const std::string& name, const IInterface *parent);
    
  // Destructor
  virtual ~LArG4HECSDTool() {}

  // Method in which all the SDs are created and assigned to the relevant volumes
  StatusCode initializeSD() override final;

  // Calls down to all the SDs to get them to pack their hits into a central collection
  StatusCode Gather() override final;
    
  /** Beginning of an athena event.  This is where collection initialization should happen.
    If we are using a WriteHandle, then this could be empty. */
//  StatusCode SetupEvent() override final;

 private:
  // The actual hit container - here because the base class is for both calib and standard SD tools
  SG::WriteHandle<LArHitContainer> m_HitColl;

  std::vector<std::string> m_sliceVolumes;
  std::vector<std::string> m_localVolumes;
  std::vector<std::string> m_wheelVolumes;
  LArG4SimpleSD* m_sliceSD;
  LArG4SimpleSD* m_localSD;
  LArG4SimpleSD* m_wheelSD;

};

#endif
