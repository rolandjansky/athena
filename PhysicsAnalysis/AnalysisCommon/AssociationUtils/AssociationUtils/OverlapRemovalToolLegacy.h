#ifndef ASSOCIATIONUTILS_OVERLAPREMOVALTOOL_H
#define ASSOCIATIONUTILS_OVERLAPREMOVALTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"

// EDM includes
#include "xAODBase/IParticle.h"

// Local includes
#include "AssociationUtils/IOverlapRemovalTool.h"
#include "AssociationUtils/OverlapRemovalDefs.h"

/// Overlap removal tool
///
/// This tool implements the harmonized object overlap removal
/// recommendations from the harmonization study group 5, given in
/// https://cds.cern.ch/record/1700874
///
/// This tool is deprecated and will no longer be supported. Please consider
/// using the new OverlapRemovalTool in the ORUtils namespace.
///
/// @author Steve Farrell <steven.farrell@cern.ch>
///
class OverlapRemovalTool : public virtual IOverlapRemovalTool,
                           public asg::AsgTool
{

    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS(OverlapRemovalTool, IOverlapRemovalTool)

  public:

    /// Constructor for standalone usage
    OverlapRemovalTool(const std::string& name);
    /// Destructor
    ~OverlapRemovalTool();

    /// @name Methods implementing the asg::IAsgTool interface
    /// @{

    /// Initialize the tool
    virtual StatusCode initialize() override;

    /// @}

    /// @name Methods implementing the IOverlapRemovalTool interface
    /// @{

    /// Top-level method for performing full overlap-removal.
    /// The individual OR methods will be called in the recommended order,
    /// and the considered objects will be decorated with the output result.
    /// Use this method form when the electron and muon containers are
    /// sufficiently loose for the tau-lep overlap removal.
    virtual StatusCode removeOverlaps(const xAOD::ElectronContainer* electrons,
                                      const xAOD::MuonContainer* muons,
                                      const xAOD::JetContainer* jets,
                                      const xAOD::TauJetContainer* taus = 0,
                                      const xAOD::PhotonContainer* photons = 0) override;

    /// Top-level method for performing full overlap-removal.
    /// The individual OR methods will be called in the recommended order,
    /// and the considered objects will be decorated with the output result.
    /// Use this method form when you're using view-containers or subset
    /// containers in order to provide the loose electrons and muons for the
    /// tau-lep overlap removal.
    virtual StatusCode removeOverlaps(const xAOD::ElectronContainer* electrons,
                                      const xAOD::MuonContainer* muons,
                                      const xAOD::JetContainer* jets,
                                      const xAOD::TauJetContainer* taus,
                                      const xAOD::ElectronContainer* looseElectrons,
                                      const xAOD::MuonContainer* looseMuons,
                                      const xAOD::PhotonContainer* photons = 0) override;

    /// Remove overlapping electrons and jets
    /// This method will decorate both the electrons and jets according to
    /// both the e-jet and jet-e overlap removal prescriptions.
    virtual StatusCode removeEleJetOverlap(const xAOD::ElectronContainer& electrons,
                                           const xAOD::JetContainer& jets) override;

    /// Remove overlapping muons and jets
    virtual StatusCode removeMuonJetOverlap(const xAOD::MuonContainer& muons,
                                            const xAOD::JetContainer& jets) override;

    /// Remove overlapping electrons and muons
    /// TODO: make it possible to veto event based on this.
    virtual StatusCode removeEleMuonOverlap(const xAOD::ElectronContainer& electrons,
                                            const xAOD::MuonContainer& muons) override;

    /// Remove jets overlapping with taus
    virtual StatusCode removeTauJetOverlap(const xAOD::TauJetContainer& taus,
                                           const xAOD::JetContainer& jets) override;

    /// Remove overlapping taus and electrons
    virtual StatusCode removeTauEleOverlap(const xAOD::TauJetContainer& taus,
                                           const xAOD::ElectronContainer& electrons) override;

    /// Remove overlapping taus and muons
    virtual StatusCode removeTauMuonOverlap(const xAOD::TauJetContainer& taus,
                                            const xAOD::MuonContainer& muons) override;

    /// Remove overlapping photons and electrons
    virtual StatusCode removePhotonEleOverlap(const xAOD::PhotonContainer& photons,
                                              const xAOD::ElectronContainer& electrons) override;

    /// Remove overlapping photons and muons
    virtual StatusCode removePhotonMuonOverlap(const xAOD::PhotonContainer& photons,
                                               const xAOD::MuonContainer& muons) override;

    /// Remove overlapping photons
    virtual StatusCode removePhotonPhotonOverlap(const xAOD::PhotonContainer& photons) override;

    /// Remove overlapping photons and jets
    virtual StatusCode removePhotonJetOverlap(const xAOD::PhotonContainer& photons,
                                              const xAOD::JetContainer& jets) override;

    /// @}

  protected:

    // I might use these next methods for the loose lepton ID

    /// Loose electron ID for tau-electron OR
    StatusCode isLooseEleForTauEleOR(const xAOD::Electron* electron, bool& pass);
    /// Loose muon ID for tau-muon OR
    StatusCode isLooseMuonForTauMuonOR(const xAOD::Muon* muon, bool& pass);

    /// Determine if jet is flagged as a b-jet
    /// TODO: Use StatusCode?
    bool isBJet(const xAOD::Jet* jet);

    /// Generic dR-based overlap check between one object and a container.
    bool objectOverlaps(const xAOD::IParticle* obj,
                        const xAOD::IParticleContainer& container,
                        double dR);

    /// Determine if objects overlap by a simple dR comparison
    bool objectsOverlap(const xAOD::IParticle* p1, const xAOD::IParticle* p2,
                        double dRMax, double dRMin = 0);

    /// Decorate p1 with an overlap object link to p2
    StatusCode addOverlapObjectLink(const xAOD::IParticle* p1,
                                    const xAOD::IParticle* p2);

    /// Recommended calculation of overlap distance parameter, (delta R)^2.
    /// dR^2 = (y1-y2)^2 + (phi1-phi2)^2
    /// Note this is calculated with the rapidity rather than the
    /// pseudorapidity. TLorentzVector::DeltaR uses the latter.
    double deltaR2(const xAOD::IParticle* p1, const xAOD::IParticle* p2);
    /// deltaR = sqrt( deltaR2 )
    double deltaR(const xAOD::IParticle* p1, const xAOD::IParticle* p2);

    /// Check if object is flagged as input for OR
    bool isInputObject(const xAOD::IParticle* obj);

    /// Check if object has been rejected by decoration
    bool isRejectedObject(const xAOD::IParticle* obj);

    /// Check if object is surviving OR thus far
    bool isSurvivingObject(const xAOD::IParticle* obj)
    { return isInputObject(obj) && !isRejectedObject(obj); }

    /// Set output decoration on object, pass or fail
    void setOverlapDecoration(const xAOD::IParticle* obj, bool result);

    /// Shorthand way to set an object as pass
    void setObjectPass(const xAOD::IParticle* obj)
    { setOverlapDecoration(obj, false); }

    /// Shorthand way to set an object as fail
    void setObjectFail(const xAOD::IParticle* obj)
    { setOverlapDecoration(obj, true); }

    /// Check if output decoration has been applied to a container.
    /// Returns false if the container is empty.
    /// Output logic independent.
    bool isDecorated(const xAOD::IParticleContainer& container);

    /// Helper method for ensuring proper initialization of output decoration.
    /// This implementation takes advantage of the default initialization of
    /// the decoration value. So, 'overlaps' is by default false. This method
    /// will not change the decoration value if it exists.
    void initializeDecorations(const xAOD::IParticleContainer& container);

    /// Helper method for setting all objects as passing
    void resetDecorations(const xAOD::IParticleContainer& container);

    /// Following methods can be used to decorate an electron with an appropriate flag if it
    /// shares a track with a muon. It enables to rerun the full OR on skimmed/slimmed xAODs
    /// which do not contain the TrackParticleLinks anymore. The flag-writing and flag-running
    /// modes can be enabled via setProperty methods.

    /// Check if TrackParticleLinks are available in the input.
    /// Mainly needed if flag-running mode should be set automatically.
    bool checkTrackParticles(const xAOD::ElectronContainer& container);
    /// Check if input electron container is decorated with the flag for flag-running mode.
    bool checkSharesTrackDecoration(const xAOD::ElectronContainer& container);
    /// Check if electron shares track with a muon based on decoration.
    bool sharesTrack(const xAOD::IParticle* obj);
    /// Set output decoration on electron indicating if it shares a track with a muon.
    StatusCode setSharesTrackDecoration(const xAOD::IParticle* obj, bool sharesTrack);
    /// Set output decoration on all electrons also those not qualified for OR. Default is false.
    void initializeSharesTrackDecoration(const xAOD::ElectronContainer& container);   //need wrapper?

  private:

    //
    // Configurable properties
    //

    /// Input object decoration which specifies which objects to look at
    std::string m_inputLabel;
    /// Output object decoration which specifies overlapping objects
    std::string m_overlapLabel;

    /// Input jet decoration which labels a bjet
    std::string m_bJetLabel;

    /// Flag to toggle overlap object links
    bool m_linkOverlapObjects;

    /// electron-jet overlap cone (removes electron)
    float m_electronJetDR;
    /// jet-electron overlap cone (removes jet)
    float m_jetElectronDR;
    /// muon-jet overlap cone
    float m_muonJetDR;
    /// tau-jet overlap cone
    float m_tauJetDR;
    /// tau-electron overlap cone
    float m_tauElectronDR;
    /// tau-muon overlap cone
    float m_tauMuonDR;
    /// photon-electron overlap cone
    float m_photonElectronDR;
    /// photon-muon overlap cone
    float m_photonMuonDR;
    /// photon-photon overlap cone
    float m_photonPhotonDR;
    /// photon-jet overlap cone
    float m_photonJetDR;

    /// Electron ID selection for tau-ele OR
    std::string m_tauEleOverlapID;
    /// Muon pt threshold for tau-mu OR
    float m_tauMuOverlapMuPt;
    /// Tau pt criteria divider for tau-mu OR
    float m_tauMuOverlapTauPt;

    /// Flag to decorate electron if it shares
    /// a track with a muon
    bool m_writeSharedTrackFlag;
    /// Flag to enable flag-running mode
    bool m_useSharedTrackFlag;

    //
    // Accessors and decorators
    //

    /// Input label accessor
    ort::inputAccessor_t* m_inputAccessor;
    /// Output decorator
    ort::outputDecorator_t* m_outputDecorator;
    /// B-jet label accessor
    ort::inputAccessor_t* m_bJetAccessor;
    /// Object link decorator
    ort::objLinkDecorator_t* m_overlapLinkDecorator;
    /// Output decorator for flag-running mode of ele-muon overlap removal
    ort::sharedTrackDecorator_t* m_sharedTrackDecorator;
    /// Input accessor for flag-running mode of ele-muon overlap removal
    ort::sharedTrackAccessor_t* m_sharedTrackAccessor;

}
#if not defined(__GCCXML__) and not defined(__ROOTCLING__) and not defined(__MAKECINT__) and not defined(R__DICTIONARY_FILENAME)
__attribute__((deprecated))
#endif
; // class OverlapRemovalTool

#endif
