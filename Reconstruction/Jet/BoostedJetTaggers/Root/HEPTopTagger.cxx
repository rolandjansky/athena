/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "BoostedJetTaggers/HEPTopTagger.h"
#include "xAODJet/JetAuxContainer.h"
#include "JetEDM/IConstituentUserInfo.h"
#include <fastjet/PseudoJetStructureBase.hh>
#include <fastjet/SharedPtr.hh>

#include "fastjet/JetDefinition.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/tools/Filter.hh"

// Adapt ASG_CHECK macro :
#define INT_CHECK( EXP )                                       \
   do {                                                        \
      const StatusCode sc__ = EXP;                             \
      if( ! sc__.isSuccess() ) {                               \
         ATH_MSG_ERROR( "Failed to call \"" << #EXP << "\"" ); \
         return 1;                                          \
      }                                                        \
   } while( 0 )

namespace HTTxAOD {



  typedef fastjet::JetDefinition::DefaultRecombiner DefRecomb;

  ///******************************************************
  class FlavourRecombiner : public DefRecomb {
  public:
    FlavourRecombiner(fastjet::RecombinationScheme recomb_scheme = fastjet::E_scheme);

    virtual std::string description() const;

    /// get rid of default -ve user index (flavour!) for any ghosts that are around...
    virtual void preprocess(fastjet::PseudoJet&) const;

    /// recombine pa and pb and put result into pab
    virtual void recombine(const fastjet::PseudoJet&,
                           const fastjet::PseudoJet&,
                           fastjet::PseudoJet&) const;
  };



  class HTTPseudoJetStructure : public fastjet::PseudoJetStructureBase {
  public :
    HTTPseudoJetStructure( std::vector<fastjet::PseudoJet> & v) {m_constit.swap(v);}
    bool has_constituents () const {return true;}
    std::vector< fastjet::PseudoJet > 	constituents (const fastjet::PseudoJet &) const {
      return m_constit;
    }
    std::vector< fastjet::PseudoJet > m_constit;
  };

  // ***************************************
  // Flavour recomibing class
  // ***************************************
  // ------------------------
  // used to combine PseudoJets via JetDefinition::RecombinationScheme and set user index
  FlavourRecombiner::FlavourRecombiner(fastjet::RecombinationScheme recomb_scheme) :
    DefRecomb(recomb_scheme) {}

  std::string FlavourRecombiner::description() const {
    return DefRecomb::description() + " (with user index addition)";
  }

  void FlavourRecombiner::preprocess(fastjet::PseudoJet& p) const {
    if (p.user_index() < 0) p.set_user_index(0);
  }

  void FlavourRecombiner::recombine(const fastjet::PseudoJet& pa,
                                    const fastjet::PseudoJet& pb,
                                    fastjet::PseudoJet& pab) const {
    DefRecomb::recombine(pa,pb,pab);
    pab.set_user_index(pa.user_index() + pb.user_index());
  }


  // ************************************************************
  // Helper to filter a PseudoJet vector according to a filter operator
  template<class FILTER>
  void filterPseudoJets( std::vector<fastjet::PseudoJet>& vec, const FILTER &f){
    std::vector<fastjet::PseudoJet> tmp; tmp.reserve(vec.size());
    for(fastjet::PseudoJet& pj:vec ) if( f(pj) ) tmp.push_back(pj);
    tmp.swap(vec);
  }

  // a filter operator implementing a pt cut
  // instantiation is : PtMinPJFilter filter{cutvalue}; (c++11)
  struct PtMinPJFilter {
    bool operator()(const fastjet::PseudoJet &pj) const {return pj.perp()>ptcut;}
    const double ptcut;
  };

  // a filter operator implementing a absolute eta cut
  struct AbsEtaPJFilter {
    bool operator()(const fastjet::PseudoJet &pj) const {return fabs(pj.eta())<etacut;}
    const double etacut;
  };



  // ************************************************************
  // HEPTopTagger implementation
  // ************************************************************
  HEPTopTagger::HEPTopTagger(const std::string& name) :
    asg::AsgTool(name)
    , m_jetFromPJTool("")
    , m_jetCalibTools()
    , m_jetCalibToolsCA15()
    , m_preDefSettings("")
    , m_topContName("")
    , m_topCandLink("HTTCandidate")
    , m_groomedFatJetLink("SetMe")
    , m_CutCalibratedSubjetPt(true)             // set to True to apply the minimal_subjet_pt cut on the calibrated subjet
    , m_ExclusiveSubjets_Rparam(true)           // true  -> Rmax:   max. distance of constituent from jet axis)
    // false -> Rmerge: (smallest) R that would yield the same subjet in inclusive clustering
    , m_CorrectSubjetJES(true)                  // do subjet calibrations
    , m_SortTopCandInPt(false)                  // sort candidates by pT instead of closeness to 172.3 GeV
    , m_AcutBeforeSorting(false)                // A-cut applied before picking candidate
    , m_ScaleFactor_JES(1)                      // scale subjet 4-momenta by this factor

    , m_TaggerAlgorithm(1)                      // algorithm used INSIDE HEPTopTagger - 0: kt, 1: CA. 2: AntiKt
    // Step 1: Fatjet selection
    // , m_MinFatjetPt(180000.)                    // minimal fatjet pT (for ALL fatjets) [MeV]
    , m_MinFatjetM(0.)                          // minimal fatjet mass [MeV]
    , m_MaxFatjetEta(2.5)                       // maximal fatjet eta
    // Step 2: Cuts to find hard substructure
    , m_MassCutoff(50000.)                      // stop splitting substructure object when mass below this value [MeV]
    , m_MassDrop(0.8)                           // mass drop criterion
    // Step 3: Maximum filtering radius
    , m_RadiusFilter(0.3)                       // maximal radius parameter in filtering step
    // Step 3+4: Cuts on reclustering
    , m_MinSubjetPt(20000.)                     // minimal pt for a top-subjet to be used [MeV]
    , m_MaxSubjetEta(2.0)                       // maximal abs(eta) for a top-subjet to be used
    // Step 4: Number of kept subjets and allowed top mass range
    , m_NJetsFilter(5)                          // number of kept filter subjets
    , m_lowerMassCut(140000.)                   // minimal top pt [MeV]
    , m_upperMassCut(210000.)                   // maximal top pt [MeV]
    // Step 6: Mass plane cut values
    , m_TopMass(172300.)                        // true top mass [MeV]
    , m_WMass(80400.)                           // mass of the W boson [MeV]
    , m_LowerXBound(0.2)                        // minimal atan(m13/m12)
    , m_UpperXBound(1.3)                        // maximal atan(m13/m12)
    , m_LowerYBound(0.35)                       // minimal m23/m123
    , m_RelMassWidth(0.3)                       // width of W boson mass window, 0.3 -> +-15%
    // Final: Cut on top candidate pt
    , m_MinCandPt(180000.)                      // minimal top candidate pT [MeV],
    // Fat-jet grooming parameters
    , m_FilterNSubJets(5)			// number of subjets to cluster in filtering
    , m_FilterR(0.3)				// jet R used in filtering
    , m_groomedContName("HTT_CA15GroomedJet")		// name of the output Container to store groomed jets
  {

    declareProperty("JetFromPseudoJetTool", m_jetFromPJTool);

    declareProperty("CalibrateSubJets", m_CorrectSubjetJES);
    declareProperty("CalibrationTools", m_jetCalibTools);
    declareProperty("CalibrationToolsCA15", m_jetCalibToolsCA15);

    declareProperty("TaggerAlgorithm", m_TaggerAlgorithm);
    declareProperty("MassDrop", m_MassDrop);

    declareProperty("SortCandInPt", m_SortTopCandInPt);
    declareProperty("AcutBeforeSorting", m_AcutBeforeSorting);
    declareProperty("ExclusiveSubjetsRparam", m_ExclusiveSubjets_Rparam);

    declareProperty("TopMass", m_TopMass);
    declareProperty("WMass",   m_WMass);

    declareProperty("PreDefSettings", m_preDefSettings);


    declareProperty("TopContName", m_topContName);
    //    declareProperty("", m_topSubjetContName);
    declareProperty("TopCandidateLink", m_topCandLink);

    // there's no JetTool yet to filter on mass/eta. Do it here :
    declareProperty("MinFatJetMass", m_MinFatjetM);
    declareProperty("MaxFatJetEta", m_MaxFatjetEta);


    declareProperty("MassCutoff", m_MassCutoff  );
    declareProperty("RadiusFilter", m_RadiusFilter);
    declareProperty("MinSubjetPt", m_MinSubjetPt);
    declareProperty("MaxSubjetEta", m_MaxSubjetEta);
    declareProperty("NJetsFilter", m_NJetsFilter);
    declareProperty("LowerMassCut", m_lowerMassCut);
    declareProperty("UpperMassCut", m_upperMassCut);

    declareProperty("LowerXBound" , m_LowerXBound );
    declareProperty("UpperXBound" , m_UpperXBound );
    declareProperty("LowerYBound" , m_LowerYBound );
    declareProperty("RelMassWidth", m_RelMassWidth);
    declareProperty("MinCandPt"   , m_MinCandPt   );

    declareProperty("CutCalibratedSubjetPt", m_CutCalibratedSubjetPt);

    declareProperty("FilterNSubJets", m_FilterNSubJets);
    declareProperty("FilterR",        m_FilterR);
    declareProperty("FiltContainerName", m_groomedContName);
  }

  HEPTopTagger::~HEPTopTagger() {}

  StatusCode HEPTopTagger::initialize(){

    ATH_CHECK(m_jetFromPJTool.retrieve());

    if(m_preDefSettings=="normal") UseNormalTagger();
    if(m_preDefSettings=="def") UseDefTagger();
    if(m_preDefSettings=="tight") UseTightTagger();
    if(m_preDefSettings=="loose") UseLooseTagger();
    if(m_preDefSettings=="sloose") UseSuperLooseTagger();
    if(m_preDefSettings=="Acut") UseAcutTagger();
    if(m_preDefSettings=="Ptord") UsePtordTagger();
    if(m_preDefSettings=="defR03") UseDefR03Tagger();
    if(m_preDefSettings=="defR02") UseDefR02Tagger();
    if(m_preDefSettings=="defNf6") UseDefNf6Tagger();
    if(m_preDefSettings.find("sjcalib") != std::string::npos) UseSJCalibXTagger();
    if(m_preDefSettings.find("sj2Dcalib") != std::string::npos) UseSJ2DCalibXTagger();
    if(m_preDefSettings.find("sj3Dcalib") != std::string::npos) UseSJ3DCalibXTagger();
    if(m_preDefSettings.find("sjNDcalib") != std::string::npos) UseSJNDCalibXTagger();
    if(m_preDefSettings=="noAShape") UseNoAShapeTagger();

    ATH_MSG_DEBUG("top mass = "<< m_TopMass);
    ATH_MSG_DEBUG("W   mass = "<< m_WMass);

    m_topCandSubJetLink    = m_topCandLink+"SubJets";
    m_topgroomedFatJetLink = m_topCandLink+"groomedFatJet";
    m_groomedFatJetLink    = "groomedFatJet";

    return StatusCode::SUCCESS;
  }


  int HEPTopTagger::modify(xAOD::JetContainer& jets) const {

    xAOD::JetContainer* topCandidate = new xAOD::JetContainer();
    xAOD::JetAuxContainer* topaux = new xAOD::JetAuxContainer();
    topCandidate->setStore(topaux);
    INT_CHECK( evtStore()->record(topCandidate, m_topContName) );
    INT_CHECK( evtStore()->record(topaux, m_topContName+"Aux.") );

    xAOD::JetContainer* topSubJets = new xAOD::JetContainer();
    xAOD::JetAuxContainer* topsubaux = new xAOD::JetAuxContainer();
    topSubJets->setStore(topsubaux);
    INT_CHECK( evtStore()->record(topSubJets, m_topContName+"SubJets") );
    INT_CHECK( evtStore()->record(topsubaux,  m_topContName+"SubJetsAux.") );

    // keep creating groomed jet container for each HTT configuration
    xAOD::JetContainer* groomedFatJets = 0;
    xAOD::JetAuxContainer* groomedFatAux;
    if(getCalibratorCA15() != NULL || (m_preDefSettings.find("sj") != std::string::npos && m_preDefSettings.find("calib") != std::string::npos)) {
      groomedFatJets   = new xAOD::JetContainer();
      groomedFatAux    = new xAOD::JetAuxContainer();
      groomedFatJets->setStore(groomedFatAux);
      std::string extraName = "";
      if(m_preDefSettings.find("sj") != std::string::npos && m_preDefSettings.find("calib") != std::string::npos)
	extraName = m_preDefSettings;
      INT_CHECK( evtStore()->record(groomedFatJets, m_groomedContName+extraName) );
      INT_CHECK( evtStore()->record(groomedFatAux,  m_groomedContName+extraName+"Aux.") );
    }

    int res = findTopCandidates(jets, *topCandidate, *topSubJets, *groomedFatJets);
    ATH_MSG_DEBUG(" END HTT  topCandidate size = "<<topCandidate->size() );
    return res;
  }


  void HEPTopTagger::notTagged(xAOD::Jet *j) const {
    static  ElementLink<xAOD::IParticleContainer> el;
    j->setAttribute(m_topCandLink, el);
  }

  int HEPTopTagger::findTopCandidates(xAOD::JetContainer & input,
                                      xAOD::JetContainer & topCandCont,
                                      xAOD::JetContainer & topSubJetCont,
				      xAOD::JetContainer & groomedFatJetsCont
				      ) const {

    xAOD::JetInput::Type jtype= xAOD::JetInput::LCTopo;
    const IJetCalibrationTool* calTool = getCalibratorCA15();
    for( xAOD::Jet *j : input) {
      // calibrate CA15 before threshold cuts
      if(calTool){
        ATH_MSG_DEBUG("Calibration tool given, calibrating input fat jets." );
        j->setAttribute<xAOD::JetFourMom_t>("JetPileupScaleMomentum", j->jetP4()); // This is to workaound JetCalibTools issue
        StatusCode sc= calTool->applyCalibration(*j ) ; if(sc.isFailure()) continue;
      }

      // a dirty trick to check if this is the first JetModifier. Skip otherwise, to avoid overwritng of the groomed jet collection
      xAOD::Jet * groomed_jet = nullptr;
      if(calTool || (m_preDefSettings.find("sj") != std::string::npos && m_preDefSettings.find("calib") != std::string::npos)) {
	groomFatJet(j, groomed_jet, groomedFatJetsCont, m_FilterR, m_FilterNSubJets, 0.0);
/*	groomFatJet(j, groomed_jet, groomedFatJetsCont, 0.3, 5, 0.0);*/
	groomFatJet(j, groomed_jet, groomedFatJetsCont, 0.25, 5, 0.0);
	groomFatJet(j, groomed_jet, groomedFatJetsCont, 0.25, 6, 0.0);
	groomFatJet(j, groomed_jet, groomedFatJetsCont, 0.2, 5, 0.0);
	groomFatJet(j, groomed_jet, groomedFatJetsCont, 0.2, 6, 0.0);
	groomFatJet(j, groomed_jet, groomedFatJetsCont, 0.3, 0, 0.05);
	groomFatJet(j, groomed_jet, groomedFatJetsCont, 0.25, 0, 0.05);
	groomFatJet(j, groomed_jet, groomedFatJetsCont, 0.2, 0, 0.05);
      }

      // there's no JetTool yet to filter on mass/eta. Do it here :
      if( fabs(j->eta()) > m_MaxFatjetEta ) {notTagged(j); continue;}
      if( j->m() < m_MinFatjetM ) {notTagged(j); continue;}

      ATH_MSG_DEBUG("Runing HTT for jet at "<< j->index() << "  "<<j->pt() );
      HTTResult cand = runTagger(*(j->getPseudoJet()) );

      if( ! cand.validCandidate) {notTagged(j); continue;}
      if( ! cand.isTagged) {notTagged(j); continue;} // for now exclude untagged jets
      ATH_MSG_DEBUG(" HTT Found a candidate ! Saving it." );
      ATH_MSG_DEBUG(" Top pt, mass "<< cand.top.pt() << " , "<< cand.top.m() );

      // Add PJ constituents to the top constituent
      // We add a custom PseudoJetStructure containing the constituents
      cand.top.set_structure_shared_ptr( fastjet::SharedPtr<fastjet::PseudoJetStructureBase>(new HTTPseudoJetStructure(cand.topConstituents) ) );

      // This allows the m_jetFromPJTool to create top Jet and fill topCandCont :
      //static vector<std::string> nullvecstr;
      std::vector<std::string> tmpv;
      xAOD::Jet * top = m_jetFromPJTool->add( cand.top , topCandCont, jtype, tmpv );
      ATH_MSG_DEBUG(" Top pt, mass "<< cand.top.pt() << " , "<< cand.top.m() );

      // Create top subjets and fill topSubJetCont :
      std::vector<const xAOD::Jet *> subjets; subjets.reserve(3); // will use as an association.
      for(fastjet::PseudoJet &pjsub : cand.topSubJets)
        subjets.push_back(m_jetFromPJTool->add( pjsub , topSubJetCont, jtype, tmpv) );

      // do associations
      ATH_MSG_DEBUG("associating fatjet to top");
      j->setAssociatedObject(m_topCandLink, top);
      ATH_MSG_DEBUG("associating subjets to top");
      top->setAssociatedObjects(m_topCandSubJetLink, subjets);
      if(calTool)
        top->setAssociatedObject(m_topgroomedFatJetLink, groomed_jet); // groomed jets associations
      // ATH_MSG_DEBUG("associating  top to fatjet  "<< &input);
      // ElementLink<xAOD::IParticleContainer> el(input, j->index() , 0);
      // ATH_MSG_DEBUG(" created EL");
      // top->setAttribute("Parent", el);
      // //top->setAssociatedObject("Parent", j);


      // calculate & associate moments
      BWWIndex res = extractBWMij( cand.topSubJets);
      double m123=(cand.topSubJets[0]+cand.topSubJets[1]+cand.topSubJets[2]).m();

      float atan1312= atan(res.m13/res.m12);

      top->setAttribute("M12",  res.m12);
      top->setAttribute("M13",  res.m13);
      top->setAttribute("M23",  res.m23);
      top->setAttribute("M123", m123);
      top->setAttribute("Atan1312", atan1312);

      top->setAttribute("n_top_cands",  cand.n_top_cands);
      top->setAttribute("HTTagged",  cand.isTagged);

      top->setAttribute("ptSub1", cand.topSubJets[0].pt());
      top->setAttribute("ptSub2", cand.topSubJets[1].pt());
      top->setAttribute("ptSub3", cand.topSubJets[2].pt());

      // also get dr values
      top->setAttribute( "DrW1W2", cand.topSubJets[res.w1].delta_R(cand.topSubJets[res.w2] ) );
      top->setAttribute( "DrW1B", cand.topSubJets[res.w1].delta_R(cand.topSubJets[res.b] ) );
      top->setAttribute( "DrW2B", cand.topSubJets[res.w2].delta_R(cand.topSubJets[res.b] ) );
      top->setAttribute( "DrMaxW1W2B", r_max_3jets(cand.topSubJets[0],cand.topSubJets[1],cand.topSubJets[2]) );

      top->setAttribute( "CosThetaW1W2",  // m12 is closest to mW
                         check_cos_theta(cand.top, cand.topSubJets[res.w1],cand.topSubJets[res.w2]) );
      top->setAttribute( "CosTheta12",// pt-ordering
                         check_cos_theta(cand.top,cand.topSubJets[1],cand.topSubJets[2]) );


    }

    return 0;
  }




  HEPTopTagger::HTTResult HEPTopTagger::runTagger(const fastjet::PseudoJet &jet) const {


    std::vector<fastjet::PseudoJet> top_parts;
    const fastjet::ClusterSequence* cs = jet.associated_cluster_sequence ();

    static HTTResult noCandidate;
    noCandidate.validCandidate = false;

    if(! cs){
      ATH_MSG_ERROR(" No availaible ClusterSequence !");
      return noCandidate;
    }

    // ------------------------------
    // Step 2: Find hard substructure
    // ------------------------------
    // input: jet, output: top_parts

    FindHardSubst(jet, top_parts, *cs);

    // now we have the hard substructure of the fat jet
    size_t parts_size = top_parts.size();

    // these events are not interesting
    if(top_parts.size() < 3){return noCandidate;}


    // ------------------------------------------
    // Start loop over the hard substructure jets
    // ------------------------------------------

    // For a good miss-tag efficiency I need to apply all 3 conditions
    // (1) topmass (2) W mass (3) costheta
    // at the same time to the top_parts to single out the right 3 of them.


    // Default sorting in the HTT:
    // mass closest to top mass (m_TopMass)
    // Alternative: ptsorted - take the candidate with maximum pT
    double dummy	  = m_SortTopCandInPt ?  0 : 1000000000000.0;
    double toppt	  = m_SortTopCandInPt ?  0 : -1;
    double deltatop = m_SortTopCandInPt ? -1 : 10000000000.0;

    FlavourRecombiner fl_rec;


    //double fatjet_rap = jet.rapidity();
    //double fatjet_r = cs->jet_def().R()*1.3;

    HTTResult bestCandidate;
    int count_top = 0;
    bestCandidate.validCandidate = false;
    for(unsigned rr=0; rr<parts_size; rr++) {
      for(unsigned ll=rr+1; ll<parts_size; ll++) {
        for(unsigned kk=ll+1; kk<parts_size; kk++) {
          ATH_MSG_DEBUG( "Hard structure Loop :"<< rr <<" "<< ll << "  "<< kk );
          // ---------------------------------------------
          // Step 3: Filtering of the substructure objects
          // ---------------------------------------------

          // define top_constituents
          std::vector<fastjet::PseudoJet> top_constits;
          top_constits.reserve(30); // guessed
          cs->add_constituents(top_parts[rr],top_constits);
          cs->add_constituents(top_parts[ll],top_constits);
          cs->add_constituents(top_parts[kk],top_constits);

          // define radius for filtering ( std::min(m_RadiusFilter, dR(subjet_i, subjet_j)) )
          double filt_top_R = std::min(m_RadiusFilter,
                                  0.5*sqrt(std::min(top_parts[kk].squared_distance(top_parts[ll]),
                                               std::min(top_parts[rr].squared_distance(top_parts[ll]),
                                                   top_parts[kk].squared_distance(top_parts[rr])))));

          ATH_MSG_DEBUG( "filt_top_R = " << filt_top_R );

          // perform filtering
          // m_TaggerAlgorithm: 0 = kt, 1 = C/A, 2 = antikt
          fastjet::JetDefinition filtering_top_def(fastjet::JetAlgorithm(m_TaggerAlgorithm),
                                                   filt_top_R);
          filtering_top_def.set_recombiner(&fl_rec);



          // this area def is used for bosed cluster sequences
          // This is the faster method, but not the one used as it's less precise.
          //fastjet::GhostedAreaSpec ghost_area_spec = fastjet::GhostedAreaSpec(6.0, 1, 0.01, 1.0, 0.1, 1e-100);
          fastjet::GhostedAreaSpec ghost_area_spec = fastjet::GhostedAreaSpec(6.0, 1, 0.01, 0., 0.1, 1e-100);
          //fastjet::GhostedAreaSpec ghost_area_spec = fastjet::GhostedAreaSpec(fatjet_rap-fatjet_r,fatjet_rap+fatjet_r , 1, 0.01, 1.0, 0.1, 1e-100);
          //fastjet::GhostedAreaSpec ghost_area_spec = fastjet::GhostedAreaSpec(fatjet_rap-fatjet_r,fatjet_rap+fatjet_r , 1, 0.01, .0, 0.1, 1e-100);
          std::vector<int> vseed = {32,5434,342342};
          ghost_area_spec.set_random_status(vseed);
          // ghost_area_spec.set_fj2_placement(true);
          fastjet::AreaDefinition areaDefinition = fastjet::AreaDefinition(ghost_area_spec);


          //fastjet::ClusterSequenceArea cstopfilt(top_constits,  filtering_top_def, areaDefinition);
          // Area commented out : not sure it's relevant for small subjets+ very cpu intensive
          fastjet::ClusterSequence cstopfilt(top_constits,  filtering_top_def);


          // extract top subjets
          std::vector <fastjet::PseudoJet> filt_top_subjets = inclCsFunction(&cstopfilt);

          // print block for debugging
          if(filt_top_subjets.size() > 0) {
            ATH_MSG_DEBUG("inclusive subjets:" );
            for(unsigned ii = 0; ii<filt_top_subjets.size() ; ii++) {
              ATH_MSG_DEBUG( "subjet : "<<ii << " with pT: " << filt_top_subjets[ii].perp() << " and eta: " << filt_top_subjets[ii].eta() );
            }
          }

          // --------------------------------------------------------------
          // Step 4: Keep five hardest subjets and test with top-quark mass
          // --------------------------------------------------------------
          ATH_MSG_DEBUG("Step 4" );

          fastjet::PseudoJet topcandidate(0.0,0.0,0.0,0.0);
          topcandidate.set_user_index(0);
          std::vector <fastjet::PseudoJet> topcand_constits;
          topcand_constits.resize(0);

          for(unsigned ii = 0; ii<std::min((unsigned)m_NJetsFilter, (unsigned)filt_top_subjets.size()) ; ii++) {
            cstopfilt.add_constituents(filt_top_subjets[ii],topcand_constits);
            fl_rec.plus_equal(topcandidate, filt_top_subjets[ii]);
          }

          ATH_MSG_DEBUG( "topcandidate.m() = " << topcandidate.m() << "   pt="<< topcandidate.pt());

          // test topcandidate to lie between m_LowerMassCut and m_UpperMassCut
          if(!inMassRange(topcandidate.m())) continue;
          ATH_MSG_DEBUG("top candidate in mass window!" );

          /////////////////////// passed for top mass criteria////////////////////////


          // -------------------------------
          // Step 5: Form exactly three jets
          // -------------------------------
          ATH_MSG_DEBUG("Step 5" );

          fastjet::JetDefinition reclustering(fastjet::JetAlgorithm(m_TaggerAlgorithm), 3.14/2);
          //fastjet::ClusterSequenceArea * cssubtop = new fastjet::ClusterSequenceArea(topcand_constits,reclustering,areaDefinition);
          fastjet::ClusterSequence * cssubtop = new fastjet::ClusterSequence(topcand_constits,reclustering);

          if (topcand_constits.size() < 3) {
            ATH_MSG_DEBUG("Only " << topcand_constits.size() << " constituents for exclusive subjets..." );
            continue;
          }

          std::vector <fastjet::PseudoJet> top_subs = exclCsFunction(cssubtop, 3);
          //catch exceptions as occasionally exclusive filtering fails and fastjet::Error is thrown in delete_self_when_unused()
          try {
            cssubtop->delete_self_when_unused(); // Because we'll need this sequence when dealing with subjets
          } catch (fastjet::Error) {
            continue;
          }
          if (top_subs.size() < 3) {
            ATH_MSG_DEBUG("Only " << topcand_constits.size() << " exclusive subjets..." );
            continue;
          }


          // not saving all candidates ... to do ?
          // candjets.push_back(top_subs[0]); //
          // candjets.push_back(top_subs[1]); //
          // candjets.push_back(top_subs[2]); //

          // // Write out all proto-candidate masses and pTs
          // if (topcandidate.m()>0){
          //   all_cand_masses.push_back( topcandidate.m() );
          //   all_cand_pts.push_back( topcandidate.perp() );

          //   all_cand_m12s.push_back(  (top_subs[0]+top_subs[1]).m() );
          //   all_cand_m13s.push_back(  (top_subs[0]+top_subs[2]).m() );
          //   all_cand_m23s.push_back(  (top_subs[1]+top_subs[2]).m() );
          //   all_cand_m123s.push_back( (top_subs[0]+top_subs[1]+top_subs[2]).m() );
          //}

          // ------------------------------
          // Step 6: Mass plane cut (A-cut)
          // ------------------------------
          ATH_MSG_DEBUG("Step 6" );

          // Calculate already here so we can optionally do it BEFORE the ordering
          // mass plane cut
          double rmin = (1-m_RelMassWidth/2. )*m_WMass/m_TopMass;
          double rmax = (1+m_RelMassWidth/2. )*m_WMass/m_TopMass;
          /////////////////////// check for W mass criteria////////////////////////
          // these are the 2d massplane cuts (A-cut)
          bool tmp_masscut_passed = check_mass_criteria(rmin,rmax,top_subs);

          count_top+=1;
          // Optional massplane BEFORE sorting
          if ( m_AcutBeforeSorting && (!tmp_masscut_passed) ) continue;

          // Default sorting in the HTT:
          // mass closest to top quark mass m_TopMass
          // Alternative: pt sorted - take the candidate with maximum pT
          bool isBestCandidate = false;
          if (m_SortTopCandInPt) {
            // transfer infos of the positively identified top to the outer world toppt = topcandidate.perp();
            toppt = topcandidate.pt();
            if(toppt > dummy) {
              isBestCandidate  = true ;
              ATH_MSG_DEBUG( "We have a good Pt candidate!" );
              dummy = toppt;
            }
          }else {
            deltatop = std::abs(m_TopMass - topcandidate.m());
            if(deltatop < dummy) {
              isBestCandidate  = true ;
              ATH_MSG_DEBUG("We have a good DeltaM candidate! masscut passed "<< tmp_masscut_passed << "  m="<<  topcandidate.m() << "  deltatop="<< deltatop );
              dummy = deltatop;
            }
          }

          //isBestCandidate = isBestCandidate&& (topcandidate.perp() > m_MinCandPt)  ;
          if(isBestCandidate){
            bestCandidate.top = topcandidate;
            bestCandidate.topSubJets = top_subs;
            bestCandidate.topConstituents = topcand_constits;
            bestCandidate.isTagged = (inMassRange( topcandidate.m() ) && tmp_masscut_passed);
            bestCandidate.validCandidate = tmp_masscut_passed && (topcandidate.perp() > m_MinCandPt);
            //   '= tmp_masscut_passed && pt cut' above is to reproduce run1 feature :
            //    unless m_AcutBeforeSorting is true, the best candidate is rejected if tmp_masscut_passed is false (and thus no valid candidate are found),
            //    A consequence is if m_AcutBeforeSorting is false (default), all candidate have validCandidate == isTagged


            // Calculate maximum of pairwise substrct distances as well as the
            // max 3-object distance
            double a=sqrt( top_parts[rr].squared_distance( top_parts[ll]));
            double b=sqrt( top_parts[rr].squared_distance( top_parts[kk]));
            double c=sqrt( top_parts[kk].squared_distance( top_parts[ll]));

            bestCandidate.drmaxpairsubstruct = std::max(a, std::max(b, c));
            bestCandidate.drmax3substruct = r_max_3jets( top_parts[rr], top_parts[ll], top_parts[kk] );
	    bestCandidate.n_top_cands = count_top;

          } // end if(isBestCandidate)

        } // end kk
      } // end ll
    }// end rr

    if(!bestCandidate.validCandidate) {
      ATH_MSG_DEBUG("no top candidate found !  " );
    }

    return bestCandidate;
  }



  void HEPTopTagger::FindHardSubst(const fastjet::PseudoJet & this_jet, std::vector<fastjet::PseudoJet> & t_parts, const fastjet::ClusterSequence &clSeq) const
  {
    fastjet::PseudoJet parent1(0,0,0,0), parent2(0,0,0,0);
    //if (this_jet.m() < _max_subjet_mass || !clSeq.has_parents(this_jet, parent1, parent2)) original HTT
    if (!clSeq.has_parents(this_jet, parent1, parent2))
      {
        t_parts.push_back(this_jet);
      }
    else
      {
        if (parent1.m() < parent2.m()) std::swap(parent1, parent2);

        // original HTT :
        // FindHardSubst(parent1,t_parts);
        // if (parent1.m() < _mass_drop_threshold * this_jet.m())
        //   {
        //     FindHardSubst(parent2,t_parts);
        //   }

        // the _max_subjet_mass cut above is probably replaced by the m_MassDrop tests below :

        if (parent1.m() < m_MassDrop * this_jet.m()) {

          if((parent1.m() > m_MassCutoff) && (parent2.m() > m_MassCutoff)) {
            FindHardSubst(parent1,t_parts, clSeq);
            FindHardSubst(parent2,t_parts, clSeq);
          } else if((parent1.m() > m_MassCutoff) && (parent2.m() <m_MassCutoff)) {
            t_parts.push_back(parent2);
            FindHardSubst(parent1,t_parts, clSeq);
          } else {
            t_parts.push_back(parent1);
            t_parts.push_back(parent2);
            return;
          }
        } else { // then parent1.m()>m_MassDrop* jet.m()
          FindHardSubst(parent1,t_parts,clSeq);
        }

      }
  }


  // ------------------------------------------
  // Reclusters the subjets and calibrates them
  // ------------------------------------------
  std::vector<fastjet::PseudoJet> HEPTopTagger::inclCsFunction(fastjet::ClusterSequence *cs) const {
    ATH_MSG_DEBUG( "In inclCsFunction" );

    //Find the Subjets
    std::vector<fastjet::PseudoJet> out_v;
    if (m_CutCalibratedSubjetPt) {
      out_v = fastjet::sorted_by_pt(cs->inclusive_jets( 0.75*m_MinSubjetPt ));
    }
    else {
      out_v = fastjet::sorted_by_pt(cs->inclusive_jets( m_MinSubjetPt ));
    }

    // Remove high eta jets
    filterPseudoJets( out_v, AbsEtaPJFilter{m_MaxSubjetEta} );

    //Calibrate the subjets
    if (m_CorrectSubjetJES) {
      // build a temporary JetContainer
      xAOD::JetContainer subjetCont;
      // SET an AUX STORE !
      fillJetContainer( subjetCont, out_v );

      double R_para = cs->jet_def().R();

      ATH_MSG_DEBUG( "filtering. Incl: R_param = " << R_para << "  subjet size="<< subjetCont.size());
      const IJetCalibrationTool* calTool = getCalibrator(R_para);
      // loop over subjet, calibrate them
      for (size_t i=0; i<subjetCont.size(); i++) {
        xAOD::Jet *j =  subjetCont[i];
        StatusCode sc= calTool->applyCalibration(*j ) ; if(sc.isFailure()) continue;
        out_v[i].reset_momentum( j->px(),j->py(),j->pz(),j->e() );
        //out_v[i] *= m_ScaleFactor_JES; // can be 1.
        applyInSituJESRescale(out_v[i]);
      }
      ATH_MSG_DEBUG( " Calibrated."  );
      if (m_CutCalibratedSubjetPt) {
        filterPseudoJets( out_v, PtMinPJFilter{m_MinSubjetPt} );
      }
      ATH_MSG_DEBUG( " Filtered."  );

      // We must delete the aux store
      delete subjetCont.getStore();

    } // end if(m_correctsubjetjes)
    return out_v;
  }



  // -----------------------------------------------------------
  // Reclusters the subjets to exactly njets and calibrates them
  // -----------------------------------------------------------
  std::vector<fastjet::PseudoJet> HEPTopTagger::exclCsFunction(fastjet::ClusterSequence *cs,
                                                          const int njets ) const {

    //Find the Subjets
    std::vector<fastjet::PseudoJet> out_v = fastjet::sorted_by_pt(cs->exclusive_jets( njets ));

    // Remove low pT jets
    double ptmin = m_MinSubjetPt;
    if (m_CutCalibratedSubjetPt) ptmin = m_MinSubjetPt*0.75;
    filterPseudoJets(out_v, PtMinPJFilter{ptmin});

    // Remove high eta jets
    filterPseudoJets( out_v, AbsEtaPJFilter{m_MaxSubjetEta} );

    //Calibrate the subjets
    if (m_CorrectSubjetJES) {
      // build a temporary JetContainer
      xAOD::JetContainer subjetCont;
      // SET an AUX STORE !
      fillJetContainer( subjetCont, out_v );

      for (size_t i=0; i<out_v.size(); i++) {
        fastjet::PseudoJet &pj = out_v[i];
        ATH_MSG_DEBUG("Calibrating jet " << i << " with pT = " << pj.perp() << " GeV" );

        double R_param = findTopSubJetsRparam(cs , pj );
        xAOD::Jet *j =  subjetCont[i];
        StatusCode sc= getCalibrator(R_param)->applyCalibration(* j ); if(sc.isFailure()) continue;

        out_v[i].reset_momentum( j->px(),j->py(),j->pz(),j->e() );
        //out_v[i] *= m_ScaleFactor_JES; // can be 1.
        applyInSituJESRescale(out_v[i]);
      }

      if (m_CutCalibratedSubjetPt)
        filterPseudoJets(out_v, PtMinPJFilter{m_MinSubjetPt});

      // We must delete the aux store
      delete subjetCont.getStore();
    }

    return out_v;
  }

  // ----------------
  // Helper functions
  // ----------------
  bool HEPTopTagger::inMassRange (double mass) const {
    return m_lowerMassCut <= mass && mass <= m_upperMassCut;
  }

  double HEPTopTagger::findTopSubJetsRparam(const fastjet::ClusterSequence * cs , fastjet::PseudoJet & pj) const {

    double R_param = -1;

    //see if the subjet consists of a single cluster
    // call the method of the cluster sequence
    std::vector<fastjet::PseudoJet> constituents = cs->constituents(pj);


    if (constituents.size() == 1) {
      //set radius parameter to a reasonable value
      R_param = 0.15;
    }
    else {
      if (m_ExclusiveSubjets_Rparam) {
        //Calculate the maximal deltaR between the jet axis and the constituents
        double R_max = -1;
        for (size_t i=0; i<constituents.size(); i++) {
          R_max = std::max(R_max, pj.squared_distance(constituents[i]));
        }
        R_param = std::sqrt(R_max);
      }
      else {
        //use the radius parameter that would have to be used with inclusive
        //clustering to obtain the same subjet
        //repeat the exclusive clustering, but for more than 3 jets
        //if the jet disappears use FastJet to get the corresponding radius
        int nj = 3;
        bool pj_stillhere = true;
        while (pj_stillhere) {
          nj += 1;
          std::vector<fastjet::PseudoJet> l_pj_ptsorted_nj = fastjet::sorted_by_pt(cs->exclusive_jets( nj ));

          //go through the list of returned jets and see if pj is in the list
          pj_stillhere = false;
          for (size_t i=0; i<l_pj_ptsorted_nj.size(); i++) {
            if ( pj.squared_distance(l_pj_ptsorted_nj[i]) < 0.00001)
              {
                pj_stillhere = true;
                break;
              }
            //if pj is in the list when clustering exclusively for nj jets
            //then pj_stillhere = True
          }
        }

        double dmerge = cs->exclusive_dmerge_max(nj-1);
        // dcut<dmerge => nj jets      # the jet is split
        // dcut=dmerge => nj-1 jets    # the jet is not yet split
        // 3.14/2*sqrt(dmerge) is the smallest radius parameter that would
        // have to be used in inclusive clustering to obtain the same jet

        double R_exclusive = 3.14/2;   // hardcoded in TopTagger.cpp
        R_param = R_exclusive*sqrt(dmerge);

      } // end not m_ExclusiveSubjets_Rparam

    } // end constituents.size() >1

    return R_param;
  }


  void HEPTopTagger::groomFatJet(xAOD::Jet *j, xAOD::Jet *&groomedJet, xAOD::JetContainer &groomedFatJetsCont, float Rjet, int NFiltJets, float fracPt) const {
    xAOD::JetInput::Type jtype= xAOD::JetInput::LCTopo;

    ATH_MSG_DEBUG("Start grooming" );
    // add filtering to fatjets
    const fastjet::PseudoJet &fj_jet = *(j->getPseudoJet()); // "atlas" jet -> "fastjet" jet
    fastjet::Filter filter_fj;
    if(NFiltJets !=0 && fracPt < 0.01){
      // give C/A and R=0.3 for filtering
      filter_fj = fastjet::Filter( fastjet::JetDefinition( fastjet::JetAlgorithm(m_TaggerAlgorithm) , Rjet ), fastjet::SelectorNHardest(NFiltJets) );
    }
    else if (NFiltJets ==0 && fracPt >= 0.01){
      // give C/A and R=0.3 for trimming
      filter_fj = fastjet::Filter( fastjet::JetDefinition( fastjet::JetAlgorithm(m_TaggerAlgorithm) , Rjet ), fastjet::SelectorPtFractionMin(fracPt) );
    }
    else{
      // print error message if you get here
      ATH_MSG_ERROR("NFiltJets and fracPt can not be both 0 or non-zero");
    }

    fastjet::PseudoJet groomed_fj_jet = filter_fj(fj_jet);
    std::vector<fastjet::PseudoJet> keptsubjets = groomed_fj_jet.pieces(); // get N PJ subjets from filtering procedure. These subjets we can calibrate.
    ATH_MSG_DEBUG("Finish Filtering fat PJ" );

    // build a temporary JetContainer
    xAOD::JetContainer groomedCont;

    // SET an AUX STORE !
    // convert PJ's into xAOD::Jet's
    fillJetContainer( groomedCont, keptsubjets );

    const IJetCalibrationTool* calToolRFilt = getCalibrator(Rjet); // get calibration tool for C/A R=0.3 jets

    // calibrate subjets resulting from filtering procedure
    fastjet::PseudoJet calib_groomed_fj_jet;
    for (size_t i=0; i<groomedCont.size(); i++) {
      xAOD::Jet *sj = groomedCont[i];
      StatusCode sc = calToolRFilt->applyCalibration(*sj ) ; if(sc.isFailure()) continue;
      keptsubjets[i].reset_momentum( sj->px(),sj->py(),sj->pz(),sj->e() );
      //keptsubjets[i] *= m_ScaleFactor_JES; // can be 1.
      applyInSituJESRescale( keptsubjets[i] );
      calib_groomed_fj_jet += keptsubjets[i]; // add them to get calibrated groomed fat jet
    }
    delete groomedCont.getStore();
    ATH_MSG_DEBUG("Finish calibration of subjets and construction of a calibrated filtered fat PJ");

    std::vector<std::string> tmpv;
    // fill groomed xAOD::Jet container with uncalibrated jets
    groomedJet =  m_jetFromPJTool->add( groomed_fj_jet , groomedFatJetsCont, jtype, tmpv ) ;
    ATH_MSG_DEBUG("Finish transformation uncalibrated filtered fat PJ --> uncalibrated filtered fat xAOD::Jet");

    // a trick to keep internal structure from groomed_fj_jet, but rescale 4-momentum to calib_groomed_fj_jet
    xAOD::JetFourMom_t calibP4;
    calibP4.SetPxPyPzE( calib_groomed_fj_jet.px(), calib_groomed_fj_jet.py(), calib_groomed_fj_jet.pz(), calib_groomed_fj_jet.E() );
    groomedJet->setJetP4(calibP4);
    ATH_MSG_DEBUG("Finish adjusting for calibration the 4-momentum of the uncalibrated groomed xAOD::Jet");

    std::string linkName = m_groomedFatJetLink;
    if(NFiltJets !=0 && fracPt < 0.01){
      linkName +=  "FiltR"+std::to_string(int(Rjet*100))+"N"+std::to_string(NFiltJets);
    }
    else{
      linkName +=  "TrimR"+std::to_string(int(Rjet*100))+"F"+std::to_string(int(fracPt*100));
    }

    if(m_preDefSettings.find("sj") != std::string::npos && m_preDefSettings.find("calib") != std::string::npos){
      linkName += m_preDefSettings;
    }

    j->setAssociatedObject(linkName, groomedJet);

    ATH_MSG_DEBUG("Finish linking and grooming in general");

  }



  void HEPTopTagger::fillJetContainer(xAOD::JetContainer& cont, std::vector<fastjet::PseudoJet> & pjVec) const {
    xAOD::JetAuxContainer* aux = new xAOD::JetAuxContainer();
    cont.setStore( aux );
    cont.reserve( pjVec.size() );
    std::vector<std::string> tmpv;

    for( fastjet::PseudoJet& pj : pjVec){
      //cont.push_back( m_jetFromPJTool->add( pj, cont, xAOD::JetInput::LCTopo ) );
      m_jetFromPJTool->add( pj, cont, xAOD::JetInput::LCTopo, tmpv ) ;
      xAOD::Jet & j = * (cont.back());
      j.setAttribute<xAOD::JetFourMom_t>("JetPileupScaleMomentum", j.jetP4()); // This is to workaound JetCalibTools issue
    }
  }

  const IJetCalibrationTool * HEPTopTagger::getCalibrator(double R ) const {
    size_t index = 0;
    if      (R <= 0.2)  index = 0;
    else if (R <= 0.25) index = 1;
    else if (R <= 0.3)  index = 2;
    else if (R <= 0.35) index = 3;
    else if (R <= 0.4)  index = 4;
    else if (R <= 0.45) index = 5;
    else if (R <= 0.5)  index = 6;
    else if (R <= 0.55) index = 7;
    else if (R <= 0.6)  index = 8;
    // Otherwise return the maximum available ('normal') radius
    else                index = 8;

    return m_jetCalibTools[index].operator->();
  }


  const IJetCalibrationTool * HEPTopTagger::getCalibratorCA15() const {
    size_t index = 0;
    if(m_jetCalibToolsCA15.size()>0)
      return m_jetCalibToolsCA15[index].operator->();
    else return NULL;
  }



  void HEPTopTagger::UseNormalTagger() {
    m_MassCutoff   = 50000.;
    m_RadiusFilter = 0.3;
    m_NJetsFilter  = 5;
    m_RelMassWidth = 0.3;
  }

  void HEPTopTagger::UseDefTagger() {
    ATH_MSG_DEBUG("Using def HTT");
    m_MassCutoff   = 50000.;
    m_RadiusFilter = 0.25;
    m_NJetsFilter  = 5;
    m_RelMassWidth = 0.3;
  }

  void HEPTopTagger::UsePtordTagger() {
    ATH_MSG_DEBUG("Using ptordered HTT");
    m_MassCutoff   = 50000.;
    m_RadiusFilter = 0.25;
    m_NJetsFilter  = 5;
    m_RelMassWidth = 0.3;
    m_SortTopCandInPt = true;
  }

    void HEPTopTagger::UseAcutTagger() {
    ATH_MSG_DEBUG("Using Acut first");
    m_MassCutoff   = 50000.;
    m_RadiusFilter = 0.25;
    m_NJetsFilter  = 5;
    m_RelMassWidth = 0.3;
    m_AcutBeforeSorting = true;
  }

  void HEPTopTagger::UseDefR02Tagger() {
    ATH_MSG_DEBUG("Using normal HTT");
    m_MassCutoff   = 50000.;
    m_RadiusFilter = 0.20;
    m_NJetsFilter  = 5;
    m_RelMassWidth = 0.3;
  }

  void HEPTopTagger::UseDefR03Tagger() {
    ATH_MSG_DEBUG("Using def R03 HTT");
    m_MassCutoff   = 50000.;
    m_RadiusFilter = 0.30;
    m_NJetsFilter  = 5;
    m_RelMassWidth = 0.3;
  }

  void HEPTopTagger::UseDefNf6Tagger() {
    ATH_MSG_DEBUG("Using def Nf6HTT");
    m_MassCutoff   = 50000.;
    m_RadiusFilter = 0.25;
    m_NJetsFilter  = 6;
    m_RelMassWidth = 0.3;
  }

  void HEPTopTagger::UseTightTagger() {
    m_MassCutoff   = 30000.;
    m_RadiusFilter = 0.2;
    m_NJetsFilter  = 4;
    m_RelMassWidth = 0.2;
  }
  void HEPTopTagger::UseLooseTagger() {
    m_MassCutoff   = 70000.;
    m_RadiusFilter = 0.5;
    m_NJetsFilter  = 7;
    m_RelMassWidth = 0.4;
  }

  void HEPTopTagger::UseSuperLooseTagger() {
    m_MassCutoff   = 70000.;
    m_RadiusFilter = 0.7;
    m_NJetsFilter  = 7;
    m_RelMassWidth = 0.4;
    m_LowerYBound  = 0.1;
    m_lowerMassCut = 20000.0;
    m_upperMassCut = 250000.0;
    m_RelMassWidth = 0.6;
  }

  void HEPTopTagger::UseSJCalibXTagger() {
    float tmp_sj_rescale = atoi(m_preDefSettings.substr(7).c_str()) / 1000.;
    ATH_MSG_DEBUG("Using def-like HTT with sub-jet calibration scaled by " << tmp_sj_rescale);
    m_MassCutoff   = 50000.;
    m_RadiusFilter = 0.25;
    m_NJetsFilter  = 5;
    m_RelMassWidth = 0.3;
    m_ScaleFactor_JES = tmp_sj_rescale;
  }

  void HEPTopTagger::UseSJ2DCalibXTagger() {

    ATH_MSG_DEBUG(m_preDefSettings.substr(9,4).c_str()<<" "<<m_preDefSettings.substr(14,4));
    m_ScaleFactor_JES_EtaPT.push_back( std::stoi(m_preDefSettings.substr(9,4)) / 1000.);
    m_ScaleFactor_JES_EtaPT.push_back( std::stoi(m_preDefSettings.substr(14,4)) / 1000.);
    m_ScaleFactor_JES_ptmin.push_back(0);
    m_ScaleFactor_JES_ptmin.push_back(100e3);
    m_ScaleFactor_JES_ptmax.push_back(100e3);
    m_ScaleFactor_JES_ptmax.push_back(1e6);

    ATH_MSG_DEBUG("Using def-like HTT with 2D sub-jet calibration scaled by: ");
    ATH_MSG_DEBUG("sj0 : "<<m_ScaleFactor_JES_EtaPT[0] << " for " << m_ScaleFactor_JES_ptmin[0]/1e3 << " < pT < " << m_ScaleFactor_JES_ptmax[0]/1e3 << " GeV");
    ATH_MSG_DEBUG("sj1 : "<<m_ScaleFactor_JES_EtaPT[1] << " for " << m_ScaleFactor_JES_ptmin[1]/1e3 << " < pT < " << m_ScaleFactor_JES_ptmax[1]/1e3 << " GeV");

    m_MassCutoff   = 50000.;
    m_RadiusFilter = 0.25;
    m_NJetsFilter  = 5;
    m_RelMassWidth = 0.3;
    m_ScaleFactor_JES = -1;
  }

  void HEPTopTagger::UseSJ3DCalibXTagger() {

    ATH_MSG_DEBUG(m_preDefSettings.substr(9,4).c_str()<<" "<<m_preDefSettings.substr(14,4)<<" "<<m_preDefSettings.substr(19,4));
    m_ScaleFactor_JES_EtaPT.push_back( std::stoi(m_preDefSettings.substr(9,4)) / 1000.);
    m_ScaleFactor_JES_EtaPT.push_back( std::stoi(m_preDefSettings.substr(14,4)) / 1000.);
    m_ScaleFactor_JES_EtaPT.push_back( std::stoi(m_preDefSettings.substr(19,4)) / 1000.);
    m_ScaleFactor_JES_ptmin.push_back(0);
    m_ScaleFactor_JES_ptmin.push_back(60e3);
    m_ScaleFactor_JES_ptmin.push_back(125e3);
    m_ScaleFactor_JES_ptmax.push_back(60e3);
    m_ScaleFactor_JES_ptmax.push_back(125e3);
    m_ScaleFactor_JES_ptmax.push_back(1e6);

    ATH_MSG_DEBUG("Using def-like HTT with 3D sub-jet calibration scaled by: ");
    ATH_MSG_DEBUG("sj0 : "<<m_ScaleFactor_JES_EtaPT[0] << " for " << m_ScaleFactor_JES_ptmin[0]/1e3 << " < pT < " << m_ScaleFactor_JES_ptmax[0]/1e3 << " GeV");
    ATH_MSG_DEBUG("sj1 : "<<m_ScaleFactor_JES_EtaPT[1] << " for " << m_ScaleFactor_JES_ptmin[1]/1e3 << " < pT < " << m_ScaleFactor_JES_ptmax[1]/1e3 << " GeV");
    ATH_MSG_DEBUG("sj2 : "<<m_ScaleFactor_JES_EtaPT[2] << " for " << m_ScaleFactor_JES_ptmin[2]/1e3 << " < pT < " << m_ScaleFactor_JES_ptmax[2]/1e3 << " GeV");

    m_MassCutoff   = 50000.;
    m_RadiusFilter = 0.25;
    m_NJetsFilter  = 5;
    m_RelMassWidth = 0.3;
    m_ScaleFactor_JES = -1;
  }

  void HEPTopTagger::UseSJNDCalibXTagger() {
    std::string sjJESConfig = m_preDefSettings.substr(9);
    int binningVersion = 0;
    // extract pT binning version
    if(sjJESConfig.find("v",0,3) != std::string::npos ){
      ATH_MSG_DEBUG("N-dimensional configuration has to have v[00-99] to define binning version. Assume v00...");
    }
    else {
        binningVersion = std::stoi(sjJESConfig.substr(1,2));
        sjJESConfig.erase(0,4);
    }
    // extra sjJES scales
    while ( sjJESConfig.size() != 0 ) {
        std::string tmpSjJESConfig = sjJESConfig.substr(0, 4);
        //either directly add the configuration of add automatically pre-defined configs (see HTTsjJES_calib_grids)
        m_ScaleFactor_JES_EtaPT.push_back( std::stoi(tmpSjJESConfig) / 1000. );
        sjJESConfig.erase(0, 4);
        // erase "_", if not the last element in the list
        if(sjJESConfig.size() != 0)
            sjJESConfig.erase(0, 1);
    }
    // set pT bins
    setSJNDCalibPtEtaBins(binningVersion);
    // information about configured HTT setup
    ATH_MSG_DEBUG("Using def-like HTT with 3D sub-jet calibration scaled by: ");
    for( size_t i = 0; i < m_ScaleFactor_JES_EtaPT.size(); i++ ){
      ATH_MSG_DEBUG(m_ScaleFactor_JES_EtaPT[i] << " for " << m_ScaleFactor_JES_ptmin[i]/1e3 << " < pT < " << m_ScaleFactor_JES_ptmax[i]/1e3 << " GeV ");
    }

    m_MassCutoff   = 50000.;
    m_RadiusFilter = 0.25;
    m_NJetsFilter  = 5;
    m_RelMassWidth = 0.3;
    m_ScaleFactor_JES = -1;
  }


  void HEPTopTagger::UseNoAShapeTagger() {
    ATH_MSG_DEBUG("Using def HTT w/o A-shape cuts");
    m_MassCutoff   = 50000.;
    m_RadiusFilter = 0.25;
    m_NJetsFilter  = 5;

    m_RelMassWidth = 2.;
    m_LowerYBound  = 0.;
    m_LowerXBound = 0.;
    m_UpperXBound = 1.5;
  }

  void HEPTopTagger::setSJNDCalibPtEtaBins(int ver) {
    int nBins = m_ScaleFactor_JES_EtaPT.size();
    if( nBins == 2 ) {
      if( ver == 0 ) {
        m_ScaleFactor_JES_ptmin.push_back(0);
        m_ScaleFactor_JES_ptmin.push_back(100e3);
        m_ScaleFactor_JES_ptmax.push_back(100e3);
        m_ScaleFactor_JES_ptmax.push_back(1e6);
      }
    } else if( nBins == 3 ) {
      if( ver == 0 ) {
        m_ScaleFactor_JES_ptmin.push_back(0);
        m_ScaleFactor_JES_ptmin.push_back(60e3);
        m_ScaleFactor_JES_ptmin.push_back(125e3);
        m_ScaleFactor_JES_ptmax.push_back(60e3);
        m_ScaleFactor_JES_ptmax.push_back(125e3);
        m_ScaleFactor_JES_ptmax.push_back(1e6);
      }
    } else if( nBins == 4 ) {
      if( ver == 0 ) {
        m_ScaleFactor_JES_ptmin.push_back(0);
        m_ScaleFactor_JES_ptmin.push_back(60e3);
        m_ScaleFactor_JES_ptmin.push_back(100e3);
        m_ScaleFactor_JES_ptmin.push_back(150e3);
        m_ScaleFactor_JES_ptmax.push_back(60e3);
        m_ScaleFactor_JES_ptmax.push_back(100e3);
        m_ScaleFactor_JES_ptmax.push_back(150e3);
        m_ScaleFactor_JES_ptmax.push_back(1e6);
      } else if( ver == 1 ) {
        m_ScaleFactor_JES_ptmin.push_back(0);
        m_ScaleFactor_JES_ptmin.push_back(60e3);
        m_ScaleFactor_JES_ptmin.push_back(125e3);
        m_ScaleFactor_JES_ptmin.push_back(175e3);
        m_ScaleFactor_JES_ptmax.push_back(60e3);
        m_ScaleFactor_JES_ptmax.push_back(125e3);
        m_ScaleFactor_JES_ptmax.push_back(175e3);
        m_ScaleFactor_JES_ptmax.push_back(1e6);
      } else if( ver == 2 ) {
        m_ScaleFactor_JES_ptmin.push_back(0);
        m_ScaleFactor_JES_ptmin.push_back(60e3);
        m_ScaleFactor_JES_ptmin.push_back(90e3);
        m_ScaleFactor_JES_ptmin.push_back(125e3);
        m_ScaleFactor_JES_ptmax.push_back(60e3);
        m_ScaleFactor_JES_ptmax.push_back(90e3);
        m_ScaleFactor_JES_ptmax.push_back(125e3);
        m_ScaleFactor_JES_ptmax.push_back(1e6);
      } else if( ver == 3 ) {
        m_ScaleFactor_JES_ptmin.push_back(0);
        m_ScaleFactor_JES_ptmin.push_back(50e3);
        m_ScaleFactor_JES_ptmin.push_back(100e3);
        m_ScaleFactor_JES_ptmin.push_back(150e3);
        m_ScaleFactor_JES_ptmax.push_back(50e3);
        m_ScaleFactor_JES_ptmax.push_back(100e3);
        m_ScaleFactor_JES_ptmax.push_back(150e3);
        m_ScaleFactor_JES_ptmax.push_back(1e6);
      }
    }
  }

  bool HEPTopTagger::check_mass_criteria(const double rmin, const double rmax, const std::vector<fastjet::PseudoJet> & top_subs) const {
    bool ispassed=false;
    double m12=(top_subs[0]+top_subs[1]).m();
    double m13=(top_subs[0]+top_subs[2]).m();
    double m23=(top_subs[1]+top_subs[2]).m();
    double m123=(top_subs[0]+top_subs[1]+top_subs[2]).m();

    if(
       (atan(m13/m12)>m_LowerXBound && m_UpperXBound> atan(m13/m12)
        && (m23/m123>rmin && rmax>m23/m123))
       ||
       (((m23/m123)*(m23/m123) < 1-rmin*rmin*(1+(m13/m12)*(m13/m12))) &&
        ((m23/m123)*(m23/m123) > 1-rmax*rmax*(1+(m13/m12)*(m13/m12))) &&
        (m23/m123 > m_LowerYBound))
       ||
       (((m23/m123)*(m23/m123) < 1-rmin*rmin*(1+(m12/m13)*(m12/m13))) &&
        ((m23/m123)*(m23/m123) > 1-rmax*rmax*(1+(m12/m13)*(m12/m13))) &&
        (m23/m123 > m_LowerYBound))
       ){
      ispassed=true;
    }
    return ispassed;
  }

  void HEPTopTagger::applyInSituJESRescale(fastjet::PseudoJet& pj) const {
    if(m_ScaleFactor_JES > 0)
	pj *= m_ScaleFactor_JES;
    else {
	for(unsigned int i = 0; i < m_ScaleFactor_JES_EtaPT.size(); i++) {
	  if( pj.pt() >= m_ScaleFactor_JES_ptmin[i] && pj.pt() < m_ScaleFactor_JES_ptmax[i] )
	    pj *= m_ScaleFactor_JES_EtaPT[i];
	}
    }
  }

  // ------------------------------------------------
  // Calculate cos theta of top and lower pt W subjet
  // ------------------------------------------------
  double HEPTopTagger::check_cos_theta(const fastjet::PseudoJet & jet, const fastjet::PseudoJet & subj1, const fastjet::PseudoJet & subj2) const {
    // the two jets of interest: top and lower-pt prong of W
    fastjet::PseudoJet W2;
    fastjet::PseudoJet top = jet;

    if(subj1.perp2() < subj2.perp2()) W2 = subj1;
    else W2 = subj2;

    // transform these jets into jets in the rest frame of the W
    W2.unboost(subj1+subj2);
    top.unboost(subj1+subj2);

    double csthet = (W2.px()*top.px() + W2.py()*top.py() + W2.pz()*top.pz())/sqrt(W2.modp2() * top.modp2());

    return(csthet);
  }

  double HEPTopTagger::r_max_3jets(const fastjet::PseudoJet & jet1,const fastjet::PseudoJet & jet2,const fastjet::PseudoJet & jet3) const {
    fastjet::PseudoJet jet12,jet13,jet23;
    jet12=jet1+jet2;
    jet13=jet1+jet3;
    jet23=jet2+jet3;

    double a=sqrt(jet1.squared_distance(jet2));
    double b=sqrt(jet2.squared_distance(jet3));
    double c=sqrt(jet3.squared_distance(jet1));
    double dR1=a,dR2=a;

    if(a<=b && a<=c){
      dR1=a;
      dR2=sqrt(jet12.squared_distance(jet3));
    };
    if(b<a && b<=c){
      dR1=b;
      dR2=sqrt(jet23.squared_distance(jet1));
    };
    if(c<a && c<b){
      dR1=c;
      dR2=sqrt(jet13.squared_distance(jet2));
    };
    return std::max(dR1,dR2);
  }


  HEPTopTagger::BWWIndex HEPTopTagger::extractBWMij(const std::vector<fastjet::PseudoJet>& top_subs) const {
    BWWIndex result;
    result.m12=(top_subs[0]+top_subs[1]).m();
    result.m13=(top_subs[0]+top_subs[2]).m();
    result.m23=(top_subs[1]+top_subs[2]).m();

    double dm12=std::abs(result.m12-m_WMass);
    double dm13=std::abs(result.m13-m_WMass);
    double dm23=std::abs(result.m23-m_WMass);
    double dm_min=std::min(dm12,std::min(dm13,dm23));
    if(dm_min==dm23){
      result.b=0;
      result.w1=1;
      result.w2=2;
    }
    else if(dm_min==dm13){
      result.b=1;
      result.w1=0;
      result.w2=2;
    }
    else if(dm_min==dm12){
      result.b=2;
      result.w1=0;
      result.w2=1;
    }
    return result;
  }


}
