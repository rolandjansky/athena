/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4EMECSDTOOL_H
#define LARG4EMECSDTOOL_H

#include "LArG4Code/LArG4SDTool.h"
#include <string>
#include <vector>

#include "StoreGate/WriteHandle.h"
#include "LArSimEvent/LArHitContainer.h"

class LArG4SimpleSD;

/// @class LArG4EMECSDTool
/// @brief SD tool which manages EM endcap sensitive detectors.
///
/// NOTE: this design has some multi-threading issues which will need to be
/// addressed.
///
class LArG4EMECSDTool : public LArG4SDTool
{
 public:
  /// Constructor
  LArG4EMECSDTool(const std::string& type, const std::string& name, const IInterface *parent);
    
  /// Destructor
  virtual ~LArG4EMECSDTool() {}

  /// Method in which all the SDs are created and assigned to the relevant volumes
  StatusCode initializeSD() override final;

  /// Calls down to all the SDs to get them to pack their hits into a central collection
  StatusCode Gather() override final;

 private:
  /// The actual hit container - here because the base class is for both calib and standard SD tools
  SG::WriteHandle<LArHitContainer> m_HitColl;

  // List of volumes for each SD and the corresponding SD
  std::vector<std::string> m_posIWVolumes;
  std::vector<std::string> m_negIWVolumes;
  std::vector<std::string> m_posOWVolumes;
  std::vector<std::string> m_negOWVolumes;
  std::vector<std::string> m_presVolumes;
  std::vector<std::string> m_bobVolumes;
  LArG4SimpleSD* m_posIWSD;
  LArG4SimpleSD* m_negIWSD;
  LArG4SimpleSD* m_posOWSD;
  LArG4SimpleSD* m_negOWSD;
  LArG4SimpleSD* m_presSD;
  LArG4SimpleSD* m_bobSD;
};

#endif
