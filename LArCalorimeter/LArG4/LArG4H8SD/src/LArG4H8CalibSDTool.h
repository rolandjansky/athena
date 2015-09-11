/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArG4H8CalibSDTool_H__
#define __LArG4H8CalibSDTool_H__

#include "LArG4Code/LArG4SDTool.h"

// Private member variables
#include "StoreGate/WriteHandle.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include <vector>
#include <string>

class LArG4CalibSD;

class LArG4H8CalibSDTool : public LArG4SDTool
{
public:
  LArG4H8CalibSDTool(const std::string& type, const std::string& name, const IInterface *parent);
  ~LArG4H8CalibSDTool() {}
    
  StatusCode initializeSD() override final;

  // Calls down to all the SDs to get them to pack their hits into a central collection
  StatusCode Gather() override final;
    
private:
  // The actual hit container - here because the base class is for both calib and standard SD tools
  SG::WriteHandle<CaloCalibrationHitContainer> m_HitColl;

  // Vectors of volume strings for the SDs
  std::vector<std::string> m_barCryVolumes;
  std::vector<std::string> m_bpInVolumes;
  std::vector<std::string> m_bpDeadVolumes;
  std::vector<std::string> m_bpCalibVolumes;
  std::vector<std::string> m_barInVolumes;
  std::vector<std::string> m_barDeadVolumes;

  // The actual SD pointers
  LArG4CalibSD* m_barCrySD;
  LArG4CalibSD* m_bpInSD;
  LArG4CalibSD* m_bpDeadSD;
  LArG4CalibSD* m_bpModSD;
  LArG4CalibSD* m_barInSD;
  LArG4CalibSD* m_barDeadSD;
  LArG4CalibSD* m_uninstSD;

};

#endif // __LArG4H8CalibSD_H__
