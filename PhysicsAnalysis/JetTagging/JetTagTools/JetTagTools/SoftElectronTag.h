/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_SOFTELECTRONTAG_H
#define JETTAGTOOLS_SOFTELECTRONTAG_H
/**
  @class SoftElectronTag
   top-Algorithm which performs b-tagging based on soft electron identification
   In reference mode the significance is filled into a histogram.
   In analysis mode a ref histo is read in and a likelihood according to
   the significance of each track calculated.
   @author derue@lpnhe.in2p3.fr
   @author2 Anna.Kaczmarska@ifj.edu.pl
*/


/********************************************************************

NAME:     SoftElectronTag.h
PACKAGE:  offline/PhysicsAnalysis/JetTagging/JetTagTools

AUTHORS:  F. Derue, A. Kaczmarska, M.Wolter
CREATED:  Jan 2005

PURPOSE:  b-tagging based on soft electron identification
          In reference mode the significance is filled into a histogram.
          In analysis mode a ref histo is read in and a likelihood according to
          the significance of each track calculated.

          Reconstruction and identification of non-isolated electrons is obtained through the algorithm located in egammaRec: egammaAODBuilder. Identification is based on information from shower shapes in the electromagnetic calorimeter (EMShower) or from the inner detector. Variables such defined are used to identifiy objects through a cut-based analysis (egammaSoftElectronCutIDTool) or through a ratio of likelihood (egammaEPiLogLikelihoodSETool). In the latter case weights for the electron and background hypothesis are obtained.
These variables are efficient in distinguishing electron tracks from non-electron tracks or electron tracks from gamma-conversions and Dalitz decays.
In order to construct a b-tagging algorithm we combine them with 
additional variables to exploit the specific features of b-jets.
First we take advantage of the fact that the electron is coming from 
a b quark and thus can have a significant transverse impact parameter d_0. 
In addition, because of the B hadron's high mass, electrons from direct bottom decays tend to be more boosted in a plane transverse to the jet axis, i.e. have a larger p_T^{rel} than the backgrounds. p_T^{rel} is defined as the electron momentum in the plane orthogonal to the jet axis. 
The combination of all variables is performed with a ratio of likelihood. 

For each track inside a jet, passing a serie of quality criteria, a discriminating function is build according to this ratio of likelihood. 
For each jet, the track with the highest value is chosen 
and this single track will now be used to estimate the discriminating function 
for the jet.

COMMENTS: evolved from LifetimeTag

UPDATE: 2006-02-10 AK, MW more track quality cuts added
        FD    - 06.02.2008 - migration to COOL
********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTools/ITagTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <vector>

//namespace xAOD  { class TrackParticle; }
namespace Reco { class ITrackToVertex; }
namespace Trk  { 
  class VxCandidate; 
  class ITrackToVertexIPEstimator;
}
//class Jet;
class ElectronContainer;
class PhotonContainer;
class egamma;
class TH1;

namespace Analysis
{
  class SoftElectronInfo;
  class HistoHelperRoot;
  class NewLikelihoodTool;
  class CalibrationBroker;
  class TrackSelector;
  class SVForIPTool;
  
  class SoftElectronTag : public AthAlgTool , virtual public ITagTool
  {
  public:

    /** @brief Default constructor*/
    SoftElectronTag(const std::string&,const std::string&,const IInterface*);
    
    /** @brief Destructor*/
    virtual ~SoftElectronTag();

    /** @brief initialize method*/
    StatusCode initialize();
    /** @brief finalize method*/
    StatusCode finalize();
        
    /** @brief Booking of histograms in reference mode */
    StatusCode InitReferenceMode(); 
    
    /** @brief Retrieve histograms in reference mode */
    StatusCode InitAnalysisMode(); 
    
    /** @brief look for the best electron candidate */
    void LookForBestElectron(xAOD::Jet& jetToTag);
    /** @brief retrieve info (pTrel,d0,weights) from best electron */
    void RetrieveInfoBestElectron(xAOD::Jet& jetToTag);

    /** @brief jet tagging part done only in reference mode*/
    void JetTagReferenceMode(xAOD::Jet& jetToTag);

    /** @brief jet tagging part done only in analysis mode*/
    void JetTagAnalysisMode(xAOD::Jet& jetToTag);
    
    /** @brief Set the primary vertex. TODO: This is temporary ! 
	The primary vertex should be part of the JetTag IParticle 
	interface implementation. The trouble with
	ElementLink and persistency has to be solved for that. Revisit ... */
    void setOrigin(const Trk::VxCandidate* priVtx);
    
    /** @brief main method to tag jets */
    void tagJet(xAOD::Jet& jetToTag);
    
    /** @brief finalize the histos: Normalize other histos 
	and fill the integratedNegativeIP histos.
	This cannot be done in the finalize() of this algtool because 
	the histo service writes the histos BEFORE AlgTool::finalize() methods 
	are called.
	However, it is closed AFTER Algorithm::finalize() methods are called ...
	In the calling Algorithm::finalize() method the following method will be called.
    */
    void finalizeHistos();
    
    /** @brief  Useful tools */
    bool IsHardEle(xAOD::Jet& jetToTag); 
    bool isBMeson(const int) const;
    bool isBBaryon(const int) const;
    bool isDMeson(const int) const;
    bool isDBaryon(const int) const;
    std::string longToStr(const long) const;
    
  private:

    /** @brief TrackToVertex tool */
    ToolHandle< Reco::ITrackToVertex > m_trackToVertexTool;
    /** @brief Track selection cuts for SoftElectronTag */
    ToolHandle< TrackSelector > m_trackSelectorTool;
    /** @brief Tool for the estimation of the IPs to the Vertex */
    ToolHandle< Trk::ITrackToVertexIPEstimator > m_trackToVertexIPEstimator;
    /** @brief Tool for IP */
    ToolHandle< SVForIPTool > m_SVForIPTool;
    /** @brief option to unbias IP estimation (remove track from vertex) */
    bool m_unbiasIPEstimation;
    /** @brief Pointer to the likelihood tool. */
    ToolHandle< NewLikelihoodTool > m_likelihoodTool;
    /** @brief tool to manipulate histograms*/
    HistoHelperRoot* m_histoHelper;
    /** @brief Pointer to the calibration tool: */
    ToolHandle<CalibrationBroker> m_calibrationTool;

    /** @brief tagger this tool refers to */
    std::string m_taggerName; 
    /** @brief likelihood hypotheses: usually B or U*/
    std::vector<std::string> m_hypotheses; 
    std::vector<std::string> m_histograms;    
    
    /** usually track parameters transported to the primary vertex 
	are used. for comparison study it is also possible to 
	use track parameters at the perigee: */
    //bool m_usePerigeeParameters; 

    /** @brief Storage for the primary vertex. 
	Can be removed when JetTag provides origin(). 
	this pointer does not need to be deleted in the destructor (because it
	points to something in storegate) */
    const Trk::VxCandidate* m_priVtx = 0;
    
    std::string m_author;
    int m_eleCounter;
    double m_thrTRT;
    SoftElectronInfo* m_softeInfo;
    const egamma *m_bestSoftE;

    bool m_bestSoftIsPhoton;
    // infos for best track
    double m_d0wrtPvx;
    double m_d0ErrwrtPvx;
    double m_signOfIP;
    double m_ptrel;
    double m_Db;
    double m_Du;
   /** @brief electron isemse in jet*/
    double m_isemse;
    /** @brief maximum track probability in jet*/
    double m_totalproMax;
    /** @brief vector of track probabilities*/
    std::vector<double> m_totalproVector; 

    /** @brief Track selection cuts for LifetimeTagging */
    /** @brief minimum pt[GeV] of the track*/
    //double m_minpt;     
    /** @brief max. transverse impact parameter: |d0|<d0Max */
    //double m_d0Max;     
    /** @brief max z0*/
    //double m_z0Max;     
    /** @brief maximum pseudo-rapidity */
    //double m_etaMax;    
    /** @brief at least n hits in Blayer */
    //int m_nHitBLayer;   
    /** @brief at least n hits in pixels */
    //int m_nHitPix;      
    /** @brief at least n hits in pixels+SCT */
    //int m_nHitSi;       
    /** @brief at least n hits in TRT */
    //int m_nHitTrt;      
    /** @brief at least n transition hits in TRT */
    //int m_nHitTrtHighE; 
    /** @brief ax. fit chi2 */
    //double m_fitChi2;   
    
    /** @brief Preselection routine for the tracks in the jet. */
    bool m_preselect(const xAOD::TrackParticle* track);
    
    /** @brief just print some info at the beginning */
    void UseElectrons(xAOD::Jet& jetToTag);
    void UsePhotons(xAOD::Jet& jetToTag);
    double CombineWeights(xAOD::Jet& jetToTag, const egamma *eTemp, int choice);
    std::vector<double> m_Weight;
   
    //
    bool m_writeInfoPlus;
    bool m_usePhoton;
    /** @brief Name of the original electron collection*/
    std::string m_originalElCollectionName;
    /** @brief Original electron collection*/
    const ElectronContainer* m_originalElCollection;
    /** @brief Name of the original photon collection*/
    std::string m_originalPhCollectionName;
    /** @brief Original photon collection*/
    const PhotonContainer* m_originalPhCollection;
    
    /** @brief for reference mode: */
    std::string m_runModus;
    std::string m_referenceType;
    std::string m_TPContainerName, m_TPTruthContainerName; 
    /** @brief Delta R for purification*/
    double m_purificationDeltaR;
    /** @brief Delta R for isolation wrt Z/W*/
    double m_elecIsolDeltaR;
    /** @brief min pT for jet*/
    double m_jetPtMinRef;
    /** @brief max eta for jet*/
    double m_jetEtaMaxRef;
    /** @brief boolean to use transverse impact parameter (NB: already applied 
        in the weight for electrons in egammaRec, so by default set to false) */
    bool m_useTransverseIP;
    /** @brief boolean to use cut based selection (isemse)
        for electrons in egammaRec, so by default set to false) */
    bool m_useCutBased;
    /** @brief the collection of jets*/
    std::vector<std::string> m_jetCollectionList;
    
    //GP: use Tool for getting the secondary vertex information
    std::string m_secVxFinderNameForV0Removal;
    std::string m_secVxFinderNameForIPSign;
    
    bool m_doForcedCalib;
    std::string m_ForcedCalibName;
    
  }; // End class
  
  inline void SoftElectronTag::setOrigin(const Trk::VxCandidate* priVtx) { m_priVtx=priVtx; } 
} // End namespace

#endif
