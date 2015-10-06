/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_ELMUFINDER_H
#define DITAUREC_ELMUFINDER_H

#include "DiTauToolBase.h"

#include "GaudiKernel/ToolHandle.h"

// #include "MuonSelectorTools/IMuonSelectionTool.h"
// #include "MuonSelectorTools/errorcheck.h"
#include "MuonSelectorTools/MuonSelectionTool.h"

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

 virtual StatusCode initialize();

 virtual StatusCode execute(DiTauCandidateData * data);

 virtual StatusCode eventFinalize(DiTauCandidateData *data);


 virtual void cleanup(DiTauCandidateData *) { }
 

private:
  std::string m_elContName;
  float m_elMinPt;
  float m_elMaxEta;
  std::string m_muContName;
  float m_muMinPt;
  float m_muMaxEta;
  int m_muQual;
  // ToolHandle<CP::IMuonSelectionTool> m_muSelectionTool;
  CP::MuonSelectionTool m_muSelectionTool;

};

#endif  /* ELMUFINDER_H */
