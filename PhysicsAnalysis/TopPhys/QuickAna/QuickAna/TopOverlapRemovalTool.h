/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__TOPOVERLAPREMOVALTOOL_H
#define QUICK_ANA__TOPOVERLAPREMOVALTOOL_H

#include <QuickAna/Global.h>

#include <AssociationUtils/OverlapRemovalTool.h>

namespace ana
{

// Put the tool in a namespace?

/// Overlap removal tool that supports the Run 1 top group recommendation +
/// some modified versions
///
/// The parent class implements the harmonized object overlap removal
/// recommendations from the harmonization study group 5, given in
/// https://cds.cern.ch/record/1700874
///
/// @author Xiaowen Lei <Xiaowen.Lei@cern.ch>
///
class TopOverlapRemovalTool : public OverlapRemovalTool
{

    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS(TopOverlapRemovalTool, IOverlapRemovalTool)

  public:

    /// Constructor for standalone usage
    TopOverlapRemovalTool(const std::string& name);
    /// Destructor
    ~TopOverlapRemovalTool();

    /// @name Methods implementing the IOverlapRemovalTool interface
    /// @{

    /// Remove overlapping electrons and jets
    /// This method will decorate both the electrons and jets according to
    /// both the e-jet and jet-e overlap removal prescriptions.
    ///   -- compared to the method in the parent, eta is used
    ///      (TODO: this will be studied)
    ///   -- also, remove **single** closest jet within dR of an electron
    ///       (TODO: I think this should be the default behavior. Talk to Steve.)
    /// A more appropriate function name : removeEleJetOverlapEtaPhi
    virtual StatusCode removeEleJetOverlap(const xAOD::ElectronContainer& electrons,
                                           const xAOD::JetContainer& jets);

    /// Remove muons overlapping jets
    /// A more appropriate function name : removeMuonsOverlappingJetEtaPhi
    virtual StatusCode removeMuonJetOverlap(const xAOD::MuonContainer& muons,
                                            const xAOD::JetContainer& jets);
                                            //bool use_vari_dR = false);

    /// @}


  private:

    /// Determine if objects overlap by a simple dR comparison
    ///   -- with dR calculated from dEta and dPhi
    bool objectsOverlapEtaPhi(const xAOD::IParticle* p1, const xAOD::IParticle* p2,
                                  double dRMax, double dRMin = 0);

    /// Run 1 calculation of overlap distance parameter, (delta R)^2.
    /// dR^2 = (eta1-eta2)^2 + (phi1-phi2)^2
    /// Note this is calculated with the pseudorapidity, same as
    /// TLorentzVector::DeltaR (DrEtaPhi).
    double dR2EtaPhi(const xAOD::IParticle* p1, const xAOD::IParticle* p2);
    /// deltaR = sqrt( deltaR2 )
    double dREtaPhi(const xAOD::IParticle* p1, const xAOD::IParticle* p2);

    /// electron-jet overlap cone (removes electron)
    float m_electronJetDREtaPhi;
    /// jet-electron overlap cone (removes jet)
    float m_jetElectronDREtaPhi;
    /// muon-jet overlap cone
    float m_muonJetDREtaPhi;

    /// Use variable dR for removing muons overlapping a jet
    bool m_muORJetVariDR;

}; // class TopOverlapRemovalTool

}

#endif
