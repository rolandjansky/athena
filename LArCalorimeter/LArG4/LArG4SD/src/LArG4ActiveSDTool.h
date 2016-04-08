/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4ACTIVESDTOOL_H
#define LARG4ACTIVESDTOOL_H

#include "LArG4Code/LArG4SDTool.h"
#include <string>
#include <vector>

#include "StoreGate/WriteHandle.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

class LArG4CalibSD;

class LArG4ActiveSDTool : public LArG4SDTool
{
 public:
  // Constructor
  LArG4ActiveSDTool(const std::string& type, const std::string& name, const IInterface *parent);
    
  // Destructor
  virtual ~LArG4ActiveSDTool() {}

  // Method in which all the SDs are created and assigned to the relevant volumes
  StatusCode initializeSD() override final;

  // Calls down to all the SDs to get them to pack their hits into a central collection
  StatusCode Gather() override final;

  /** Beginning of an athena event.  This is where collection initialization should happen.
    If we are using a WriteHandle, then this could be empty. */
//  StatusCode SetupEvent() override final;

 private:
  // The actual hit container - here because the base class is for both calib and standard SD tools
  SG::WriteHandle<CaloCalibrationHitContainer> m_HitColl;

  std::vector<std::string> m_stacVolumes;
  std::vector<std::string> m_presBarVolumes;
  std::vector<std::string> m_posIWVolumes;
  std::vector<std::string> m_negIWVolumes;
  std::vector<std::string> m_posOWVolumes;
  std::vector<std::string> m_negOWVolumes;
  std::vector<std::string> m_presECVolumes;
  std::vector<std::string> m_bobVolumes;
  std::vector<std::string> m_fcal1Volumes;
  std::vector<std::string> m_fcal2Volumes;
  std::vector<std::string> m_fcal3Volumes;
  std::vector<std::string> m_sliceVolumes;
  std::vector<std::string> m_miniVolumes;

  LArG4CalibSD* m_stacSD;
  LArG4CalibSD* m_presBarSD;
  LArG4CalibSD* m_posIWSD;
  LArG4CalibSD* m_negIWSD;
  LArG4CalibSD* m_posOWSD;
  LArG4CalibSD* m_negOWSD;
  LArG4CalibSD* m_presECSD;
  LArG4CalibSD* m_bobSD;
  LArG4CalibSD* m_fcal1SD;
  LArG4CalibSD* m_fcal2SD;
  LArG4CalibSD* m_fcal3SD;
  LArG4CalibSD* m_sliceSD;
  LArG4CalibSD* m_miniSD;
};

#endif
