/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H62004EMECSDTOOL_H
#define LARG4H62004EMECSDTOOL_H

#include "LArG4Code/LArG4SDTool.h"
#include "LArG4Code/ILArCalculatorSvc.h"

#include <string>
#include <vector>

#include "StoreGate/WriteHandle.h"
#include "LArSimEvent/LArHitContainer.h"

class LArG4SimpleSD;

/// DEPRECATED AND WILL BE REMOVED.
/// Please see LArG4::H62004EMECSDTool instead.
///
class LArG4H62004EMECSDTool : public LArG4SDTool
{
public:
  // Constructor
  LArG4H62004EMECSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  virtual ~LArG4H62004EMECSDTool() {}

  // Method in which all the SDs are created and assigned to the relevant volumes
  StatusCode initializeSD() override final;

  // Calls down to all the SDs to get them to pack their hits into a central collection
  StatusCode Gather() override final;

  /** Beginning of an athena event.  This is where collection initialization should happen.
      If we are using a WriteHandle, then this could be empty. */
  //  StatusCode SetupEvent() override final;

private:
  StatusCode initializeCalculators() override final;

  // The actual hit container - here because the base class is for both calib and standard SD tools
  SG::WriteHandle<LArHitContainer> m_HitColl;
  ServiceHandle<ILArCalculatorSvc> m_calculator;

  LArG4SimpleSD* m_emecSD;

};

#endif
