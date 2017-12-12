/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_heptoptaggerimpl_header
#define jetsubstructureutils_heptoptaggerimpl_header

#include "fastjet/PseudoJet.hh"

#include "JetSubStructureUtils/IPseudoJetCalibrationTool.h"

#include <string>
#include <vector>
#include <ostream>


class HEPTopTaggerImpl 
{
  public:
    // ==========================================
    // result storange container
    // ==========================================
    class HTTResult{
      public:
        HTTResult();
        void reset();
        void calcBWWIndices( double mW=80400.0 );
        void calcMassRatioVariables( double mTop=172300.0, double mW=80400.0 );

        void setTaggingFlags( bool isValid, bool isTagged, bool tagMass, bool tagMassRatios, bool tagPt );
        void setTopPseudoJets( fastjet::PseudoJet topPJ, std::vector<fastjet::PseudoJet> childrenPJVec, std::vector<fastjet::PseudoJet> constituentsPJVec);
        void setOptimalR( double optimalR );

        bool                            isValidCandidate();
        bool                            isTagged();
        bool                            getTagMass();
        bool                            getTagMassRatios();
        bool                            getTagPt();
        fastjet::PseudoJet              getTopCandidate();
        std::vector<fastjet::PseudoJet> getTopChildren();
        std::vector<fastjet::PseudoJet> getTopConstituents();
        double                          getOptimalR();
        
        void                            getChildIndices( int &bIdx, int &W1Idx, int &W2Idx );
        double                          getBestMassRatio();
        double                          getAshapeVariable();
        double                          getAtanM13M12();
        void                            getMasses( double &m123, double &m12, double &m13, double &m23 );
        double                          getAbsDeltaMTop();

      private:
        // calculated and set by HEPTopTaggerImpl
        bool                            m_isValidCandidate; // if false, no other members are set!
        bool                            m_isTagged;         // is HTT tagged
        bool                            m_tagMass;
        bool                            m_tagMassRatios;
        bool                            m_tagPt;
        fastjet::PseudoJet              m_topCandidate;     
        std::vector<fastjet::PseudoJet> m_topChildren;      // 3 top children, pT ordered
        std::vector<fastjet::PseudoJet> m_topConstituents;
        double                          m_optimalR;

        // calculated internally
        int                             m_bIdx;             // m_topChildren b candidate index
        int                             m_W1Idx, m_W2Idx;   // m_topChildren W1/W2 candidates index
        double                          m_bestMassRatio;    // best mass ratio
        double                          m_AshapeVariable;   // A-shape cut variable
        double                          m_atanM13M12;
        double                          m_m12;
        double                          m_m13;
        double                          m_m23;
        double                          m_m123;
        double                          m_absDeltaMTop;
    };

    // ==========================================
    // public enums
    // ==========================================
    enum MsgLevel { QUIET, ERROR, INFO, DEBUG, VERBOSE };
    enum TopCandSortingMode { SortByMass, SortByPt, SortByAshapeVariable };

    // ==========================================
    // public member functions
    // ==========================================
    HEPTopTaggerImpl( const std::string name);
    ~HEPTopTaggerImpl() = default;

    // returns 0 if successfull 
    int initialize( );

    // return best (by pt or mass ordering) HTT candidate for jet, 
    // optionally vector of all candidates can be stored as well
    HTTResult runTagger( 
        const fastjet::PseudoJet &jet, 
        std::vector<HTTResult>* allCandidates=0 ) const;
    HTTResult runTaggerFixedR( 
        const fastjet::PseudoJet& jet, 
        std::vector<HTTResult>* allCandidates=0 ) const;
    HTTResult runTaggerOptimalR( 
        const fastjet::PseudoJet& jet, 
        std::vector<HTTResult>* allCandidates=0 ) const;

    void printConfig( void ) const;

    // internal messages, I know it's not the nicest way
    void setMsgLevel( MsgLevel msgLevel ){ m_msgLevel = msgLevel; }
   
    // configuration parameter setters
    void setMinFatjetPt(        double pt );
    void setMinFatjetM(         double m ); 
    void setMaxFatjetAbsEta(    double absEta );

    void setTopCandSortingMode( TopCandSortingMode mode );
    void setAcutBeforeSorting(  bool flag=true );
    void setTwoStepFilter(      bool flag=true );
    void setOptimalRTagger(     bool flag=true );
    void setStoreUntagged(      bool flag=true ); 

    void setMassDropMaxSjMass(  double mdMaxSjMass );
    void setMassDropThreshold(  double mdThreshold );
    void setMinSubjetPt(        double minSjPt );
    void setMaxSubjetAbsEta(    double maxSjAbsEta );
    void setRadiusFilter(       double rFilt );
    void setNJetsFilter(        int nFilt   );
    void setTaggerJetAlgorithm(    int jetAlgo );

    void setOptimalRThreshold(  double optRTreshold );
    void setMinOptimalR(        double minOptR ); 
    void setStepOptimalR(       double stepOptR );

    void setTagMassCutMin(      double tagMMin );
    void setTagMassCutMax(      double tagMMax );
    void setTagAtan1312CutMin(  double tagAtanMin );
    void setTagAtan1312CutMax(  double tagAtanMax );
    void setTagM23M123CutMin(   double  tagM23M123Min );
    void setTagRelMassWidth(    double tagRelMWidht );
    void setTagPtCutMin(        double tagPtMin );

    void setTopMass(            double topMass);
    void setWMass(              double WMass);

    void setCalibrateSubjets(   bool flag=true );
    void setCutCalibratedSubjetPt( bool flag=true );

    void setPseudoJetCalTool(   IPseudoJetCalibrationTool* pjCalTool );

  private:
    // ==========================================
    // member variables
    // ==========================================
    std::string m_name;
    static bool m_firstCall;

    // large-R jet kinematics
    double m_minFatjetPt;     // cuts on input jet before even considereing running HTT, min pT [MeV]
    double m_minFatjetM;      // min m [MeV]
    double m_maxFatjetAbsEta; // max eta

    // HTT configuration
    TopCandSortingMode m_topCandSortingMode;// which criteria to select best top candidate
    bool m_AcutBeforeSorting;               // only sort top candidates passing mass ratio "A-shape" cuts
    bool m_twoStepFilter;                   // apply two step filter: use 3 leading-pT constituents as sole triplett 
    bool m_optimalRTagger;                  // run tagger on reclusterd fatjet with various R
    bool m_storeUntagged;                   // store HTT result(s) also if candidate fails mass or mass ratio cuts

    double m_massDropMaxSjMass;             // stop splitting substructure object when mass below this value [MeV]
    double m_massDropThreshold;             // mass drop criterion
    double m_minSubjetPt;                   // minimal pt for a top-subjet to be used [MeV]
    double m_maxSubjetAbsEta;               // maximal abs(eta) for a top-subjet to be used
    double m_radiusFilter;                  // maximal radius parameter in filtering step
    int    m_nJetsFilter;                   // number of kept filter subjets
    int    m_taggerJetAlgorithm;            // algorithm used INSIDE HEPTopTagger - 0: kt, 1: CA. 2: AntiKt

    double m_optimalRThreshold;            // optimal R threshold
    double m_minOptimalR;                   // minimal fatjet R for optimal R tagger
    double m_stepOptimalR;                  // fatjet R stepsize for optimal R tagger

    double m_tagMassCutMin;                 // minimal top pt [MeV] for HTT tag
    double m_tagMassCutMax;                 // maximal top pt [MeV] for HTT tag
    double m_tagAtan1312CutMin;             // minimal atan(m13/m12) for HTT tag
    double m_tagAtan1312CutMax;             // maximal atan(m13/m12) for HTT tag
    double m_tagM23M123CutMin;              // minimal m23/m123 for HTT tag
    double m_tagRelMassWidth;               // width of W boson mass window, 0.3 -> +-15%
    double m_tagPtCutMin;                   // minimal top candidate pT [MeV] for HTT tag

    double m_topMass;                       // true top mass [MeV]
    double m_WMass;                         // mass of the W boson [MeV]

    // misc
    bool m_calibrateSubjets;                // calibrate subjets?
    bool m_cutCalibratedSubjetPt;           // set to True to apply the minimal_subjet_pt cut on the calibrated subjet
    // ...                                  // false -> Rmerge: (smallest) R that would yield the same subjet in inclusive clustering

    // jet calibration
    IPseudoJetCalibrationTool* m_pseudoJetCalTool; // pseudo jet calibration tool

    // ==========================================
    // internal member functions
    // ==========================================
    // misc
    void printBanner( void ) const;
    void findHardSubst( 
        const fastjet::PseudoJet        &inJet, 
        std::vector<fastjet::PseudoJet> &outTopParts, 
        const fastjet::ClusterSequence  &inClSeq) const;
    void unclusterFatjets(
        const std::vector<fastjet::PseudoJet> &inBigFatjets, 
        std::vector<fastjet::PseudoJet>       &outSmallFatjets, 
        const fastjet::ClusterSequence        &inClSeq, 
        const double smallR) const;

    std::vector<fastjet::PseudoJet> inclCalSjClustering( fastjet::ClusterSequence *cs ) const ;
    std::vector<fastjet::PseudoJet> exclCalSjClustering( fastjet::ClusterSequence *cs, const int njets ) const;
    void sortTopCandidates( std::vector<HEPTopTaggerImpl::HTTResult>* allCandidates ) const;

    void filterPseudoJetsPtMin(  std::vector<fastjet::PseudoJet>& pjVec, double ptCut ) const;
    void filterPseudoJetsAbsEta( std::vector<fastjet::PseudoJet>& pjVec, double etaCut ) const;
    
    // HTT tagging criteria
    bool httMassCut( double m ) const;
    bool httPtCut( double pt ) const;
    bool httMassRatiosCut(  const std::vector<fastjet::PseudoJet> &topChildren ) const;

    void MSG_ERROR( std::string msg ) const;
    void MSG_INFO(  std::string msg ) const;
    void MSG_DEBUG( std::string msg ) const;
    void MSG_VERBOSE( std::string msg ) const;
    MsgLevel    m_msgLevel;
    std::string m_msgName;

};

#endif
