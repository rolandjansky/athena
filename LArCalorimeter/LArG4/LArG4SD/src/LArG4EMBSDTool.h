/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4EMBSDTOOL_H
#define LARG4EMBSDTOOL_H

#include "LArG4Code/LArG4SDTool.h"
#include <string>
#include <vector>

#include "StoreGate/WriteHandle.h"
#include "LArSimEvent/LArHitContainer.h"

class LArG4SimpleSD;

/// @class LArG4EMBSDTool
/// @brief SD tool which manages EM barrel sensitive detectors.
///
/// NOTE: this design has some multi-threading issues which will need to be
/// addressed.
///
class LArG4EMBSDTool : public LArG4SDTool
{
 public:
  /// Constructor
  LArG4EMBSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  /// Destructor
  virtual ~LArG4EMBSDTool() {}

  /// Method in which all the SDs are created and assigned to the relevant volumes
  StatusCode initializeSD() override final;

  /// Calls down to all the SDs to get them to pack their hits into a central collection
  StatusCode Gather() override final;

  /** Beginning of an athena event.  This is where collection initialization should happen.
    If we are using a WriteHandle, then this could be empty. */
  //StatusCode SetupEvent() override final;

 private:
  /// The actual hit container - here because the base class is for both calib and standard SD tools
  SG::WriteHandle<LArHitContainer> m_HitColl;

  /// List of volumes for the stac SD
  std::vector<std::string> m_stacVolumes;
  /// List of volumes for the presampler SD
  std::vector<std::string> m_presVolumes;
  /// Pointer to the presampler SD
  LArG4SimpleSD* m_presSD;
  /// Pointer to the stac SD
  LArG4SimpleSD* m_stacSD;
};

#endif
