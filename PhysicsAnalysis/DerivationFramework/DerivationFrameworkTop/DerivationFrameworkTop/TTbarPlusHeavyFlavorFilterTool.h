/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file DerivationFrameworkTop/TTbarPlusHeavyFlavorFilterTool.h
 * @author Georges Aad
 * @date Apr. 2015
 * @brief tool to compute filter flag for ttbar+HF
 * @brief same as GeneratorFilters/TTbarPlusHeavyFlavorFilter.h but working on xAOD truth objects and not Gen objects
*/


#ifndef DerivationFrameworkTop_TTbarPlusHeavyFlavorFilterTool_H
#define DerivationFrameworkTop_TTbarPlusHeavyFlavorFilterTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODTruth/TruthEventContainer.h"


namespace DerivationFramework{

static const InterfaceID IID_TTbarPlusHeavyFlavorFilterTool("TTbarPlusHeavyFlavorFilterTool", 1, 0);

class TTbarPlusHeavyFlavorFilterTool: public AthAlgTool {
public:
  TTbarPlusHeavyFlavorFilterTool(const std::string& t, const std::string& n, const IInterface* p);
  virtual ~TTbarPlusHeavyFlavorFilterTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  int filterFlag() const;

  static const InterfaceID& interfaceID() { return IID_TTbarPlusHeavyFlavorFilterTool; }


private:


  /// properties

  std::string m_mcName;

  bool m_usePileUp;
  bool m_useFinalStateHadrons;

  double m_bPtMinCut;
  double m_bEtaMaxCut;
  double m_cPtMinCut;
  double m_cEtaMaxCut;

  int m_bMultiCut;
  int m_cMultiCut;

  bool m_excludeBFromTop;
  bool m_excludeCFromTop;


  bool passBSelection(const xAOD::TruthParticle* part) const;
  bool passCSelection(const xAOD::TruthParticle* part) const;

  int hadronType(int pdgid) const;
  bool isBHadron(const xAOD::TruthParticle* part) const;
  bool isCHadron(const xAOD::TruthParticle* part) const;

  bool isInitialHadron(const xAOD::TruthParticle* part) const;
  bool isFinalHadron(const xAOD::TruthParticle* part) const;

  bool isQuarkFromHadron(const xAOD::TruthParticle* part) const;
  bool isCHadronFromB(const xAOD::TruthParticle* part) const;

  /// init_part needed to detect looping graphs (sherpa) and to switch on using barcode to resolve it without affecting pythia8
  /// up to know only seen at parton level
  bool isLooping(const xAOD::TruthParticle* part, std::set<const xAOD::TruthParticle*> init_part = std::set<const xAOD::TruthParticle*>()) const;

  const xAOD::TruthParticle* findInitial(const xAOD::TruthParticle* part, bool looping) const;

  bool isFromTop(const xAOD::TruthParticle* part, bool looping) const;
  bool isDirectlyFromTop(const xAOD::TruthParticle* part, bool looping) const;
  bool isDirectlyFromWTop(const xAOD::TruthParticle* part, bool looping) const;




};


} /// namespace

#endif /// DerivationFrameworkTop_TTbarPlusHeavyFlavorFilterTool_H
