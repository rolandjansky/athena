/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4DEADSDTOOL_H
#define LARG4DEADSDTOOL_H

#include "LArG4Code/LArG4SDTool.h"
#include <string>
#include <vector>

#include "StoreGate/WriteHandle.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

class LArG4CalibSD;

class LArG4DeadSDTool : public LArG4SDTool
{
 public:
  // Constructor
  LArG4DeadSDTool(const std::string& type, const std::string& name, const IInterface *parent);
    
  // Destructor
  virtual ~LArG4DeadSDTool() {}

  // Method in which all the SDs are created and assigned to the relevant volumes
  StatusCode initializeSD() override final;

  // Calls down to all the SDs to get them to pack their hits into a central collection
  StatusCode Gather() override final;

  /** Beginning of an athena event.  This is where collection initialization should happen.
    If we are using a WriteHandle, then this could be empty. */
//  StatusCode SetupEvent() override final;
    
 private:
  // Do we add the escaped energy processing?  This is only in "mode 1" (Tile+LAr), not in "DeadLAr" mode
  bool m_do_eep;

  // The actual hit container - here because the base class is for both calib and standard SD tools
  SG::WriteHandle<CaloCalibrationHitContainer> m_HitColl;

  // The volumes per SD, and the corresponding SDs
  std::vector<std::string> m_barCryVolumes;
  std::vector<std::string> m_barCryLArVolumes;
  std::vector<std::string> m_barCryMixVolumes;
  std::vector<std::string> m_DMVolumes;
  std::vector<std::string> m_barPresVolumes;
  std::vector<std::string> m_barVolumes;
  std::vector<std::string> m_ECCryVolumes;
  std::vector<std::string> m_ECCryLArVolumes;
  std::vector<std::string> m_ECCryMixVolumes;
  std::vector<std::string> m_ECSupportVolumes;
  std::vector<std::string> m_HECWheelVolumes;
  LArG4CalibSD* m_barCrySD;
  LArG4CalibSD* m_barCryLArSD;
  LArG4CalibSD* m_barCryMixSD;
  LArG4CalibSD* m_DMSD;
  LArG4CalibSD* m_barPresSD;
  LArG4CalibSD* m_barSD;
  LArG4CalibSD* m_ECCrySD;
  LArG4CalibSD* m_ECCryLArSD;
  LArG4CalibSD* m_ECCryMixSD;
  LArG4CalibSD* m_ECSupportSD;
  LArG4CalibSD* m_HECWheelSD;
  LArG4CalibSD* m_uninstSD;

};

#endif
