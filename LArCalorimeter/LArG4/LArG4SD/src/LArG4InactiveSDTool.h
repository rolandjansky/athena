/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4INACTIVESDTOOL_H
#define LARG4INACTIVESDTOOL_H

#include "LArG4Code/LArG4SDTool.h"
#include <string>
#include <vector>

#include "StoreGate/WriteHandle.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

class LArG4CalibSD;

class LArG4InactiveSDTool : public LArG4SDTool
{
 public:
  // Constructor
  LArG4InactiveSDTool(const std::string& type, const std::string& name, const IInterface *parent);
    
  // Destructor
  virtual ~LArG4InactiveSDTool() {}

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

  // The list of volumes and the corresponding SDs
  std::vector<std::string> m_barPreVolumes;
  std::vector<std::string> m_barVolumes;
  std::vector<std::string> m_ECPosInVolumes;
  std::vector<std::string> m_ECPosOutVolumes;
  std::vector<std::string> m_ECNegInVolumes;
  std::vector<std::string> m_ECNegOutVolumes;
  std::vector<std::string> m_HECWheelVolumes;
  std::vector<std::string> m_fcal1Volumes;
  std::vector<std::string> m_fcal2Volumes;
  std::vector<std::string> m_fcal3Volumes;
  std::vector<std::string> m_miniMomVolumes;
  std::vector<std::string> m_miniVolumes;
  std::vector<std::string> m_miniLayVolumes;
  LArG4CalibSD* m_barPreSD;
  LArG4CalibSD* m_barSD;
  LArG4CalibSD* m_ECPosInSD;
  LArG4CalibSD* m_ECPosOutSD;
  LArG4CalibSD* m_ECNegInSD;
  LArG4CalibSD* m_ECNegOutSD;
  LArG4CalibSD* m_HECWheelSD;
  LArG4CalibSD* m_fcal1SD;
  LArG4CalibSD* m_fcal2SD;
  LArG4CalibSD* m_fcal3SD;
  LArG4CalibSD* m_miniMomSD;
  LArG4CalibSD* m_miniSD;
  LArG4CalibSD* m_miniLaySD;

};

#endif
