/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H62004DEADSDTOOL_H
#define LARG4H62004DEADSDTOOL_H

#include "LArG4Code/LArG4SDTool.h"
#include <string>
#include <vector>

#include "StoreGate/WriteHandle.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "LArG4Code/ILArCalibCalculatorSvc.h"

class LArG4H62004CalibSD;

/// DEPRECATED AND WILL BE REMOVED.
/// Please see LArG4::H62004DeadSDTool instead.
///
class LArG4H62004DeadSDTool : public LArG4SDTool
{
 public:
  // Constructor
  LArG4H62004DeadSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  virtual ~LArG4H62004DeadSDTool() {}

  // Method in which all the SDs are created and assigned to the relevant volumes
  StatusCode initializeSD() override final;

  // Calls down to all the SDs to get them to pack their hits into a central collection
  StatusCode Gather() override final;

  /** Beginning of an athena event.  This is where collection initialization should happen.
    If we are using a WriteHandle, then this could be empty. */
//  StatusCode SetupEvent() override final;

 private:
  StatusCode initializeCalculators() override final;

  // Do we add the escaped energy processing?  This is only in "mode 1" (Tile+LAr), not in "DeadLAr" mode
  bool m_do_eep;

  // The actual hit container - here because the base class is for both calib and standard SD tools
  SG::WriteHandle<CaloCalibrationHitContainer> m_HitColl;

  ServiceHandle<ILArCalibCalculatorSvc> m_calculator;

  // The volumes per SD, and the corresponding SDs
  LArG4H62004CalibSD* m_deadSD;
  LArG4H62004CalibSD* m_uninstSD;

};

#endif
