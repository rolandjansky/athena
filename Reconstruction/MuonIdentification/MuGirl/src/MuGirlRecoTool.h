/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_MUGIRLRECOTOOL_H
#define MUGIRL_MUGIRLRECOTOOL_H

// basic includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"


// local includes
#include "MuGirlInterfaces/IMuGirlRecoTool.h"
#include "MuGirlInterfaces/CandidateSummary.h"

#include "xAODCaloEvent/CaloClusterContainer.h"

// ID helper tool
#include "MuonIdHelpers/MuonIdHelperTool.h"

// Hough transform
#include "MuonLayerHough/MuonLayerHough.h"


// Forward declarations
class ICaloMuonLikelihoodTool;


namespace Muon {
  class IMuGirlParticleCreatorTool;
  class MuonLayerHoughTool;
}


namespace NTuple { class Tuple; }

// new magnetic field service
namespace MagField { class IMagFieldSvc; }

// tracking classes
namespace Trk
{
  class IParticleCaloExtensionTool;
}


// namespace for the MuGirl related reconstruction classes
namespace MuGirlNS {

  class ICandidateTool;
  class IPerformanceTruthTool;
  class IGlobalFitTool;
  class IStauTool;
  class IANNSelectionTool;
  class IMuGirlParticleCreatorTool;
  class SegmentManager;

  /**
   *
   *   @short implementation of the MuGirl reconstruction tool
   *
   * Tool to steer the MuGirl reconstruction. It can be called from an algorithm
   * (e.g. TrigMuGirl or MuGirl) and from another tool (e.g. MuGirlTagTool). The
   * configuration is delivered by the python fragments MuGirlRecoConfig.py. 
   *
   * Supported processing:
   *  The MuGirl and TrigMuGirl algorithms will call the tool through the execute
   *  interface. The seed will be retrieced from StoreGate. The output is put into
   *  StoreGate.
   *  The MuGirlTagTool will call the tool through the MuGirlReco interface which 
   *  accepts InDetCandidate collections. The output is attached to the MuGirlTag.
   *
   *  creation date: 06/02/2014
   *
   *  @author Shlomit Tarem <tarem@cern.ch>, Alessandro Di Mattia <dimattia@cern.ch>
   *
   *
   */

  class MuGirlRecoTool: virtual public IMuGirlRecoTool, public AthAlgTool
  {
    public:
    MuGirlRecoTool(const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

    virtual ~MuGirlRecoTool(void);

    virtual StatusCode initialize();

    /**
     * Interface implementations.
     */  
    StatusCode execute();
    StatusCode MuGirlReco( const InDetCandidateCollection& InDetSeeds);


    private:
    typedef std::vector<MuonHough::MuonLayerHough::Maximum*> HoughData;

    /**
     * Fills the Hough maxima data structure.
     */ 
    StatusCode processHoughData();

    /**
     * Associates the Hough maxima to the MuGirl candidate/
     */
    void associateMaxima(HoughData hough_data[16][4][2]);

    /**
     * Returns the number of maximum stored into the Hough data structure.
     */
    int  hough_data_size() const;

    /**
     * Perform the processing from the ID tracks //vxCandidateContainer not used, so removed
     */
    bool RunFromID(CandidateSummaryList& summaryList);
    
    /**
     * Removes the maxima stored into the Hough data structure.
     */ 
    void clear_hough_data();

    /**
     *debug
     **/
    void listTrackParticles(const InDetCandidateCollection& InDetSeeds );

    /**
     * Print through the athena log the tool configuration.
     */
    void print_configuration() const;  

    /**
     *  Structure to handle the eta, phi extrapolation of the ID at the CALO layers (obsolete in the new schema)
     */   

    void doHoughTransformForNtuple(const xAOD::TrackParticle* pTrackParticle, MuGirlNS::CandidateSummary& summary);

    void doSAFit(const Trk::Track* RefittedTrack, MuGirlNS::CandidateSummary& summary);

    struct CaloParticle
    {
        MuonCombined::InDetCandidate* pInDetCandidate;
        const xAOD::TrackParticle*    pTrackParticle;
        const Trk::TrackParameters*   pTrackParameters;
        double eta;
        double phi;
        bool isolated;

        CaloParticle() :
                pInDetCandidate(NULL),pTrackParticle(NULL), pTrackParameters(NULL), eta(0.0), phi(0.0), isolated(true)
        {}

        CaloParticle(MuonCombined::InDetCandidate* InDetSeed, const xAOD::TrackParticle* pTrackParticle, const Trk::TrackParameters* parameters, double eta, double phi) :
                pInDetCandidate(InDetSeed), pTrackParticle(pTrackParticle), pTrackParameters(parameters), eta(eta), phi(phi), isolated(true)
        {}
    };
    typedef std::vector<CaloParticle*> CaloParticleList;

    /**
     * Checks the candidate topology and apply the neural network cut to accept it as a muon.
     * Fills the result is filled into the CandidateSummary. 
     */
    bool generateMuGirl(MuGirlNS::CandidateSummary& summary);


    /**
     * Computes the muon likelihood using the Calo Energy Cluster.
     */  
    void calculateLHR(CaloParticle* pParticle);


    // Variables
    CaloParticleList                               m_caloParticles;   /**< The list of CaloParticles structures */
    std::string                                    m_applyLHR;        /**< ???? */
    HoughData                                      m_hough_data[16][4][2]; /**< Structure holding the Hough maxima per [Sector][Distance][Region] */


    // Pointers to input/output containers, collections and helpers
    MuGirlNS::SegmentManager*           m_pSegmentManager;           /**< The MuGirl segment manager */
    //const xAOD::TrackParticleContainer* m_pInDetParticleContainer;   /**< Input: DataVector of Inner Detector Track Particles */
    const xAOD::CaloClusterContainer*   m_pClusCollection;           /**< Input: Datavector of Calo Energy Clusters */
    NTuple::Tuple*                      m_pCandNTuple;               /**< Pointer to the MuGirl ntuple */



    // Tool Configuration

    // Steering Properties
    BooleanProperty m_doLHR;              /**< compute the likelihood for the calorimeter deposit along the track to be muon like */
    BooleanProperty m_doANNSelection;     /**< performs the selection of muon candidate trhough the neural network (uses LHR parameter) */
    BooleanProperty m_doGlobalFit;        /**< performs the global track fit after MuGirl confirms the muon candidate */
    BooleanProperty m_doParticleCreator;  /**< fills the ParticleCreator container with the Muon Candidate summary */
    BooleanProperty m_doStau;             /**< Stau: performs the reconstruction for Slow Partciles */
    BooleanProperty m_doRH;               /**< Stau: performs the reconstruction for R-Hadrons */
    BooleanProperty m_doSAFit;            /**< Stau: MS SA fit for stau when starting from trigger */
    BooleanProperty m_doMuonFeature;      /**< Stau: Starts from a trigger muon feature */
    BooleanProperty m_doMuonBetaRefit;    /**< Stau: Starts from the hit obtained by the beatRefit algo that are in pLowBetaMuonContainer */
    BooleanProperty m_doHoughTransform;   /**< retrive the Hough Maxima from the Hough Pattern Tool for further processing */
    BooleanProperty m_doNTuple;           /**< outputs the CandidateNtuple */
    BooleanProperty m_doTruth;            /**< include the Truth association infos into the CandidateNtuple */
 
    // Configuration properties
    IntegerProperty m_nIDhits;            /**< minimum number of hits of an ID track for being extrapolated to Calo (not used anymore) */
    FloatProperty   m_ptLowerLimit;       /**< minimum pT of an ID track for being extrapolated To the Muon System */
    FloatProperty   m_stauPtCut;          /**< Stau: minimum pT of ID track to initiate the processing for the Slow Particles */
    FloatProperty   m_mfPtCut;            /**< Stau: minimum pT of muon seed track to be processed when MuGirl is seeded by the MuonFeature (=m_stauPtCut) */
    FloatProperty   m_roadDeltaPhi;       /**< Phi road half-width opened around the extrapolated track to collect the muon hits (moved into CandidateTool) */
    FloatProperty   m_barrelNNCut;        /**< threshold for the barrel Neural Network decision (currently not used) */
    FloatProperty   m_endcapNNCut;        /**< threshold for the endcap Neural Network decision (currently not used) */
    DoubleProperty  m_idR;                /**< r cone to define the ID track as isolated for LHR calculation */
    DoubleProperty  m_lhr;                /**< likelihood for the calorimeter deposit along the track to be muon like or 0.8 in trigger */
    DoubleProperty  m_eOverP;             /**< e/P threshold, but not used anywhere */
    DoubleProperty  m_defaultLHR;         /**< set default value to be used for LHR if LHR cannot be calculated */

    StringProperty  m_ntupleName;                /**< root directory where to put the ntuple */
    StringProperty  m_ntupleTitle;               /**< the title of the MuGirl output NTuple */
    StringProperty  m_CaloCollection;            /**< storegate-key name of the container of the CaloCluster (used to compute the LHR variable) */


    // Tool and Service Handles
    ToolHandle<MuGirlNS::ICandidateTool>                m_pCandidate;              /**< The Tool building the MuGirl candidate */
    ToolHandle<MuGirlNS::IPerformanceTruthTool>         m_pTruthTool;              /**< The MuGirl Performance Truth Tool */
    ToolHandle<MuGirlNS::IGlobalFitTool>                m_pGlobalFitTool;          /**< The MuGirl Global Fit Tool (muon combined)*/
    ToolHandle<MuGirlNS::IANNSelectionTool>             m_pANNSelectionTool;       /**< The MuGirl Neural Netowork Tool */
    ToolHandle<MuGirlNS::IMuGirlParticleCreatorTool>    m_pParticleCreatorTool;    /**< The MuGirl Partcile Creator Tool */
    ToolHandle<MuGirlNS::IStauTool>                     m_pStauTool;               /**< The Stau tool */

    ToolHandle<ICaloMuonLikelihoodTool>                 m_pMuLHR;                  /**< The Tool for computing the muon Calo likelihood */
    ToolHandle<Muon::MuonLayerHoughTool>                m_pMuonLayerHoughTool;     /**< Tool performing the Hough transform */ 
    ToolHandle<Muon::MuonIdHelperTool>                  m_MuonIdHelperTool;        /**< Muon Id Helper Tool */
    ToolHandle <Trk::IParticleCaloExtensionTool>        m_caloExtensionTool; //!< Tool to make the step-wise extrapolation

    ServiceHandle<MagField::IMagFieldSvc>               m_magFieldSvc;             /**< Magnetic Field Service */

  }; // class MuGirlRecoTool


} // namespace MuGirlNS

#endif // MUGIRL_MUGIRLRECOTOOL_H
