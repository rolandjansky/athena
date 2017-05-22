/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBECInnerModuleTool__H
#define TBECInnerModuleTool__H

#include "LArG4Code/LArG4SDTool.h"
#include <string>
#include <vector>

#include "StoreGate/WriteHandle.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArG4Code/ILArCalculatorSvc.h"

class LArG4SimpleSD;

/// @class TBECInnerModuleTool
///
/// This implementation has issues in multi-threading and so cannot
/// be used in an MT job. Migration discussion ongoing in ATLASSIM-2606.
///
class TBECInnerModuleTool : public LArG4SDTool
{
 public:
  // Constructor
  TBECInnerModuleTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  virtual ~TBECInnerModuleTool() {}

  virtual StatusCode initializeCalculators() override final;

  // Method in which all the SDs are created and assigned to the relevant volumes
  StatusCode initializeSD() override final;

  // Calls down to all the SDs to get them to pack their hits into a central collection
  StatusCode Gather() override final;

  /** Beginning of an athena event.  This is where collection initialization should happen.
    If we are using a WriteHandle, then this could be empty. */
//  StatusCode SetupEvent() override final;

 private:
  // The actual hit container - here because the base class is for both calib and standard SD tools
  SG::WriteHandle<LArHitContainer> m_HitColl_gapadj;
  SG::WriteHandle<LArHitContainer> m_HitColl_gapold;
  SG::WriteHandle<LArHitContainer> m_HitColl_gap_e;
  SG::WriteHandle<LArHitContainer> m_HitColl_gap_s;
  SG::WriteHandle<LArHitContainer> m_HitColl_gap_se;
  SG::WriteHandle<LArHitContainer> m_HitColl_chcoll;
  SG::WriteHandle<LArHitContainer> m_HitColl_ropt;

  ServiceHandle<ILArCalculatorSvc> m_emecinnergadjcalc;// LArG4::EMEC_ECOR_GADJ
  ServiceHandle<ILArCalculatorSvc> m_emecinnergadjoldcalc;// LArG4::EMEC_ECOR_GADJ_OLD
  ServiceHandle<ILArCalculatorSvc> m_emecinnergadjecalc;// LArG4::EMEC_ECOR_GADJ_E
  ServiceHandle<ILArCalculatorSvc> m_emecinnergadjscalc;// LArG4::EMEC_ECOR_GADJ_S
  ServiceHandle<ILArCalculatorSvc> m_emecinnergadjsecalc;// LArG4::EMEC_ECOR_GADJ_SE
  ServiceHandle<ILArCalculatorSvc> m_emecinnerchclcalc;// LArG4::EMEC_ECOR_CHCL
  ServiceHandle<ILArCalculatorSvc> m_emecinnercalc;// LArG4::EMEC_ECOR_ROPT

  // List of volumes for each SD and the corresponding SDs
  LArG4SimpleSD* m_gapadjSD;
  LArG4SimpleSD* m_gapoldSD;
  LArG4SimpleSD* m_gap_eSD;
  LArG4SimpleSD* m_gap_sSD;
  LArG4SimpleSD* m_gap_seSD;
  LArG4SimpleSD* m_chcollSD;
  LArG4SimpleSD* m_roptSD;
};

#endif
