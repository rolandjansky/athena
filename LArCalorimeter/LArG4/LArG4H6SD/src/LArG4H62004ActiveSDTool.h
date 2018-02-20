/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H62004ACTIVESDTOOL_H
#define LARG4H62004ACTIVESDTOOL_H

#include "LArG4Code/LArG4SDTool.h"
#include <string>
#include <vector>

#include "StoreGate/WriteHandle.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "LArG4Code/ILArCalibCalculatorSvc.h"

class LArG4H62004CalibSD;

/// DEPRECATED AND WILL BE REMOVED.
/// Please see LArG4::H62004ActiveSDTool instead.
///
class LArG4H62004ActiveSDTool : public LArG4SDTool
{
 public:
  // Constructor
  LArG4H62004ActiveSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  virtual ~LArG4H62004ActiveSDTool() {}

  // Method in which all the SDs are created and assigned to the relevant volumes
  StatusCode initializeSD() override final;

  // Calls down to all the SDs to get them to pack their hits into a central collection
  StatusCode Gather() override final;

 private:
  StatusCode initializeCalculators() override final;

  // The actual hit container - here because the base class is for both calib and standard SD tools
  SG::WriteHandle<CaloCalibrationHitContainer> m_HitColl;
  ServiceHandle<ILArCalibCalculatorSvc> m_emepiwcalc;
  ServiceHandle<ILArCalibCalculatorSvc> m_heccalc;
  ServiceHandle<ILArCalibCalculatorSvc> m_fcal1calc;
  ServiceHandle<ILArCalibCalculatorSvc> m_fcal2calc;
  ServiceHandle<ILArCalibCalculatorSvc> m_fcalcoldcalc;

  LArG4H62004CalibSD* m_emecSD;
  LArG4H62004CalibSD* m_hecSD;
  LArG4H62004CalibSD* m_fcal1SD;
  LArG4H62004CalibSD* m_fcal2SD;
  LArG4H62004CalibSD* m_fcalColdSD;
  std::vector<std::string> m_emecVolumes;
  std::vector<std::string> m_hecVolumes;
  std::vector<std::string> m_fcal1Volumes;
  std::vector<std::string> m_fcal2Volumes;
  std::vector<std::string> m_fcalColdVolumes;
};

#endif
