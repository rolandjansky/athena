/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_ELMUFINDER_H
#define DITAUREC_ELMUFINDER_H

#include "DiTauToolBase.h"

#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/ToolHandle.h"

// #include "MuonSelectorTools/IMuonSelectionTool.h"
// #include "MuonSelectorTools/errorcheck.h"
#include "MuonSelectorTools/MuonSelectionTool.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"

class ElMuFinder : public DiTauToolBase {
public:

 //-------------------------------------------------------------
 //! Constructor
 //-------------------------------------------------------------
 ElMuFinder(const std::string& type,
         const std::string& name,
         const IInterface * parent);

 //-------------------------------------------------------------
 //! Destructor
 //-------------------------------------------------------------
 virtual ~ElMuFinder();

 virtual StatusCode initialize() override;

 virtual StatusCode execute(DiTauCandidateData * data,
                            const EventContext& ctx) const override;

 virtual void cleanup(DiTauCandidateData *) override { }
 

private:
  SG::ReadHandleKey<xAOD::ElectronContainer> m_elContName
  { this, "ElectronContainer", "Electrons", "" };
  float m_elMinPt;
  float m_elMaxEta;
  SG::ReadHandleKey<xAOD::MuonContainer> m_muContName
  { this, "MuonContainer", "Muons", "" };
  float m_muMinPt;
  float m_muMaxEta;
  int m_muQual;

};

#endif  /* ELMUFINDER_H */
