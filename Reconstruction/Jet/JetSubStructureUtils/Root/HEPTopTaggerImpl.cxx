/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/HEPTopTaggerImpl.h"

#include <fastjet/PseudoJetStructureBase.hh>
#include <fastjet/SharedPtr.hh>
#include "fastjet/JetDefinition.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/tools/Filter.hh"

#include "TString.h"

#include <algorithm>

#if defined(ROOTCORE) || defined(ASGTOOL_ATHENA)
  #include <AsgTools/MessageCheck.h>
  using namespace asg::msgUserCode;
#else
  #define ANA_MSG_VERBOSE( xmsg ) { std::cout << "VERBOSE: " << xmsg << std::endl; }
  #define ANA_MSG_ERROR(   xmsg ) { std::cout << "ERROR:   " << xmsg << std::endl; }
  #define ANA_MSG_DEBUG(   xmsg ) { std::cout << "DEBUG:   " << xmsg << std::endl; }
  #define ANA_MSG_INFO(    xmsg ) { std::cout << "INFO:    " << xmsg << std::endl; }
#endif

// ====================================================================================
// HEPTopTaggerImpl::HTTResult
// ====================================================================================
HEPTopTaggerImpl::HTTResult::HTTResult() { 
  reset(); 
}
// ========================================
void HEPTopTaggerImpl::HTTResult::reset() {
  m_isValidCandidate = false;
  m_isTagged         = false;
  m_tagMass          = false;
  m_tagPt            = false;
  m_tagMassRatios    = false;
  m_topCandidate.reset_momentum(0,0,0,0);
  m_topChildren.clear();
  m_topConstituents.clear();
  m_optimalR    = -9999;

  m_bIdx  = -1;
  m_W1Idx = -1;
  m_W2Idx = -1;   
  m_bestMassRatio  = -9999;
  m_AshapeVariable = -9999;
  m_atanM13M12  = -9999;
  m_m12         = -9999;
  m_m13         = -9999;
  m_m23         = -9999;
  m_m123        = -9999;
  m_absDeltaMTop = -9999;
}
// ========================================
void HEPTopTaggerImpl::HTTResult::calcBWWIndices( double mW){
  if(m_topConstituents.size() != 3) return;
  double dmWm12 = abs( (m_topChildren[0]+m_topChildren[1]).m() - mW );
  double dmWm13 = abs( (m_topChildren[0]+m_topChildren[2]).m() - mW );
  double dmWm23 = abs( (m_topChildren[1]+m_topChildren[2]).m() - mW );
  double dmMin = std::min( dmWm12, std::min( dmWm13, dmWm23 ) );
  if( dmMin == dmWm12 ){
    m_bIdx=2; m_W1Idx=0; m_W2Idx=1;
  }
  else if( dmMin == dmWm13 ){
    m_bIdx=1; m_W1Idx=0; m_W2Idx=2;
  }
  else if( dmMin == dmWm23 ){
    m_bIdx=0; m_W1Idx=1; m_W2Idx=2;
  }        
}
// ========================================
void HEPTopTaggerImpl::HTTResult::calcMassRatioVariables( double mTop, double mW )
{
  m_m12  = (m_topChildren[0] + m_topChildren[1]).m();
  m_m13  = (m_topChildren[0] + m_topChildren[2]).m();
  m_m23  = (m_topChildren[1] + m_topChildren[2]).m();
  m_m123 = (m_topChildren[0] + m_topChildren[1] + m_topChildren[2]).m();

  double fw12 = std::fabs( (m_m12/m_m123) / (mW/mTop) - 1);
  double fw13 = std::fabs( (m_m13/m_m123) / (mW/mTop) - 1);
  double fw23 = std::fabs( (m_m23/m_m123) / (mW/mTop) - 1);

  m_bestMassRatio  = std::min(fw12, std::min(fw13, fw23));  
  m_atanM13M12     = std::atan(m_m13/m_m12);

  // this corresponds to the actual variables cut on by the HEPTopTagger
  // only exactly the same as m_bestMassRatio under the assumption that m123^2 = m12^2 + m13^2 + m23^2
  fw12 = std::fabs( sqrt( (1 - (m_m23/m_m123)*(m_m23/m_m123)) / ( 1 + (m_m12/m_m13)*(m_m12/m_m13) ) ) / (mW/mTop) -1 );
  fw13 = std::fabs( sqrt( (1 - (m_m23/m_m123)*(m_m23/m_m123)) / ( 1 + (m_m13/m_m12)*(m_m13/m_m12) ) ) / (mW/mTop) -1 );

  m_AshapeVariable = std::min( fw12, std::min( fw13, fw23 ) );
  m_absDeltaMTop   = std::abs( m_topCandidate.m() - mTop );
}
// ========================================
void  HEPTopTaggerImpl::HTTResult::setTaggingFlags( bool isValid, bool isTagged, bool tagMass, bool tagMassRatios, bool tagPt ){
  m_isValidCandidate = isValid;
  m_isTagged         = isTagged;
  m_tagMass          = tagMass;
  m_tagMassRatios    = tagMassRatios;
  m_tagPt            = tagPt;
}
void HEPTopTaggerImpl::HTTResult::setTopPseudoJets(  fastjet::PseudoJet topPJ, std::vector<fastjet::PseudoJet> childrenPJVec, std::vector<fastjet::PseudoJet> constituentsPJVec){
  m_topCandidate    = topPJ;
  m_topChildren     = childrenPJVec;
  m_topConstituents = constituentsPJVec;
}
void HEPTopTaggerImpl::HTTResult::setOptimalR( double optimalR ){
  m_optimalR = optimalR;
}
// ========================================
bool HEPTopTaggerImpl::HTTResult::isValidCandidate(){
  return m_isValidCandidate;
}
bool HEPTopTaggerImpl::HTTResult::isTagged(){ 
  return m_isTagged;
}
bool HEPTopTaggerImpl::HTTResult::getTagMass(){
  return m_tagMass;
}
bool HEPTopTaggerImpl::HTTResult::getTagMassRatios(){
  return m_tagMassRatios;
}
bool HEPTopTaggerImpl::HTTResult::getTagPt(){
  return m_tagPt;
}
fastjet::PseudoJet HEPTopTaggerImpl::HTTResult::getTopCandidate(){
  return m_topCandidate;
}
std::vector<fastjet::PseudoJet> HEPTopTaggerImpl::HTTResult::getTopChildren(){
  return m_topChildren;
}
std::vector<fastjet::PseudoJet> HEPTopTaggerImpl::HTTResult::getTopConstituents(){
  return m_topConstituents;
}
double HEPTopTaggerImpl::HTTResult::getOptimalR(){
  return m_optimalR;
}
void HEPTopTaggerImpl::HTTResult::getChildIndices( int &bIdx, int &W1Idx, int &W2Idx ){
  bIdx  = m_bIdx;
  W1Idx = m_W1Idx;
  W2Idx = m_W2Idx;
}
double HEPTopTaggerImpl::HTTResult::getBestMassRatio(){ 
  return m_bestMassRatio;
}
double HEPTopTaggerImpl::HTTResult::getAshapeVariable(){
  return m_AshapeVariable;
}
double HEPTopTaggerImpl::HTTResult::getAtanM13M12(){
  return m_atanM13M12;
}
void HEPTopTaggerImpl::HTTResult::getMasses( double &m123, double &m12, double &m13, double &m23 ){
  m123 = m_m123;
  m12  = m_m12;
  m13  = m_m13;
  m23  = m_m23;
}
double HEPTopTaggerImpl::HTTResult::getAbsDeltaMTop(){
  return m_absDeltaMTop;
}
// ====================================================================================
// HEPTopTaggerImpl
// ====================================================================================
bool HEPTopTaggerImpl::m_firstCall = true;
// ===========================
HEPTopTaggerImpl::HEPTopTaggerImpl( const std::string name) :
  m_name( name ),

  m_minFatjetPt(     180000.0 ),      // cuts on input jet before even considereing running HTT, min pT [MeV]
  m_minFatjetM(           0.0 ),      // min m [MeV]
  m_maxFatjetAbsEta(      2.5 ),      // max eta

  // HTT configuration
  m_topCandSortingMode( SortByMass ), // which criteria to select best top candidate
  m_AcutBeforeSorting(  false ),       // only sort top candidates passing mass ratio "A-shape" cuts
  m_twoStepFilter(      false ),       // apply two step filter: use 3 leading-pT constituents as sole triplett 
  m_optimalRTagger(     false ),       // run tagger on reclusterd fatjet with various R
  m_storeUntagged(      false ),       // store HTT result(s) also if candidate fails mass or mass ratio cuts

  m_massDropMaxSjMass(50000.0 ),       // stop splitting substructure object when mass below this value [MeV]
  m_massDropThreshold(    0.8 ),       // mass drop criterion
  m_minSubjetPt(      20000.0 ),       // minimal pt for a top-subjet to be used [MeV]
  m_maxSubjetAbsEta(      2.0 ),       // maximal abs(eta) for a top-subjet to be used
  m_radiusFilter(         0.25),       // maximal radius parameter in filtering step
  m_nJetsFilter(          5   ),       // number of kept filter subjets
  m_taggerJetAlgorithm(      1   ),       // algorithm used INSIDE HEPTopTagger - 0: kt, 1: CA. 2: AntiKt

  m_optimalRThreshold(   0.2 ),        // optimal R threshold
  m_minOptimalR(         0.5 ),        // minimal fatjet R for optimal R tagger
  m_stepOptimalR(        0.1 ),        // fatjet R stepsize for optimal R tagger

  m_tagMassCutMin(   140000.0 ),       // minimal top pt [MeV] for HTT tag
  m_tagMassCutMax(   210000.0 ),       // maximal top pt [MeV] for HTT tag
  m_tagAtan1312CutMin(    0.2 ),       // minimal atan(m13/m12) for HTT tag
  m_tagAtan1312CutMax(    1.3 ),       // maximal atan(m13/m12) for HTT tag
  m_tagM23M123CutMin(     0.35),       // minimal m23/m123 for HTT tag
  m_tagRelMassWidth(      0.3 ),       // width of W boson mass window, 0.3 -> +-15%
  m_tagPtCutMin(     180000.0 ),       // minimal top candidate pT [MeV] for HTT tag

  m_topMass(         172300.0 ),       // true top mass [MeV]
  m_WMass(            80400.0 ),       // mass of the W boson [MeV]

  // misc
  m_calibrateSubjets(        false ),   // calibrate subjets?
  m_cutCalibratedSubjetPt(   true ),   // set to True to apply the m_minSubjetPt cut on the calibrated subjet
  // ...                               // false -> Rmerge: (smallest) R that would yield the same subjet in inclusive clustering
  // jet calibration
  m_pseudoJetCalTool(NULL),

  // message level
  m_msgLevel( HEPTopTaggerImpl::ERROR ),
  m_msgName( name )
{
  m_msgName.resize(30, ' ');
  this->printBanner();
}
// ====================================================================================
// 
// ====================================================================================
int HEPTopTaggerImpl::initialize( void )
{
  int status = 0;
  if( m_calibrateSubjets && !m_pseudoJetCalTool ){
    MSG_ERROR("Requested to calibrate subjets but no IPseudoJetCalibrationTool provided");
    status = 1;
  }
  return status;
}
// ====================================================================================
// 
// ====================================================================================
HEPTopTaggerImpl::HTTResult HEPTopTaggerImpl::runTagger( 
    const fastjet::PseudoJet &jet, 
    std::vector<HEPTopTaggerImpl::HTTResult>* allCandidates ) const
{
  if(m_optimalRTagger){
    return this->runTaggerOptimalR( jet, allCandidates );
  }
  else
    return this->runTaggerFixedR( jet, allCandidates );
}
// ====================================================================================
// 
// ====================================================================================
HEPTopTaggerImpl::HTTResult HEPTopTaggerImpl::runTaggerFixedR( 
    const fastjet::PseudoJet &jet, 
    std::vector<HEPTopTaggerImpl::HTTResult>* allCandidates ) const
{
  MSG_DEBUG("==================================================");
  MSG_DEBUG(Form("runTaggerFixedR on jet (pT,eta,phi,m)=(%.2f,%.2f,%.2f,%.2f)", jet.pt(),jet.eta(),jet.phi(),jet.m()));
  MSG_DEBUG("==================================================");
  bool storeAllCandidates = (allCandidates != 0);
  if(!allCandidates) allCandidates = new std::vector<HEPTopTaggerImpl::HTTResult>();
  HEPTopTaggerImpl::HTTResult bestCandidate;
  HEPTopTaggerImpl::HTTResult tempCandidate;

  bool   sortTopCandByPt       = ( m_topCandSortingMode == SortByPt);
  bool   sortTopCandByAshape   = ( m_topCandSortingMode == SortByAshapeVariable);
  double bestTopCandPt     = -9999999; // allow chosing max pt candidate
  double bestTopCandDeltaM =  9999999; // allow chosing min delta m candidate
  double bestTopCandAshape =  9999999; // allow choosing min 1D Ashape variable
  int nHTTCandidates = 0;

  // try to get associated cluster sequence
  const fastjet::ClusterSequence* jetAssocCs = jet.associated_cluster_sequence();
  if(! jetAssocCs){
    MSG_ERROR("HTT failed: No availaible ClusterSequence!");
    return tempCandidate;
  }

  // ===========================
  // Step 1: Fatjet preselection (use calibrated fatjet)
  // ===========================
  MSG_VERBOSE("========== Step 1: fastjet selection ==========");
  double jetR = jetAssocCs->jet_def().R();
  fastjet::PseudoJet cutJet(jet);
  if( cutJet.pt() < m_minFatjetPt ||
      cutJet.m()  < m_minFatjetM ||
      abs(cutJet.eta()) > m_maxFatjetAbsEta ){
    MSG_VERBOSE("HTT reject: Jet failed kinematic requirements.");
    return tempCandidate;
  }

  // ===========================
  // Step 2: Find hard substructure
  // ===========================
  MSG_VERBOSE("========== Step 2: mass drop ==========");
  std::vector<fastjet::PseudoJet> jetParts;
  // no difference using calibrated or uncalibrated jet!
  this->findHardSubst( jet, jetParts, *jetAssocCs);
  unsigned jetParts_size = jetParts.size();
  if( jetParts_size < 3){
    MSG_VERBOSE("HTT reject: Jet has <3 hard substructre objects.");
    return tempCandidate;
  }
  // Sort subjets-after-unclustering by pT.
  // Necessary so that two-step-filtering can use the leading-three.
  // FIXME: need to sort calibrated jets
  //        does it affect anything other than order if I calibrate jetParts here?
  //m_pseudoJetCalTool->CalibratePseudoJets( jetParts, std::vector<double>( jetParts.size(), -1) );
  //        if so, write sort_by_calib_pt function and copy jetParts calib
  //        this indeed leads to slightly different results!
  jetParts=fastjet::sorted_by_pt(jetParts);

  for( unsigned ii=0; ii<jetParts_size; ii++)
    MSG_VERBOSE(Form("hard top part %u pt=%f, m=%f", ii, jetParts[ii].pt(),jetParts[ii].m()) );

  // loop over triplets
  for (     unsigned rr = 0;      rr < jetParts_size; rr++) {
    for (   unsigned ll = rr + 1; ll < jetParts_size; ll++) {
      for ( unsigned kk = ll + 1; kk < jetParts_size; kk++) {
        MSG_VERBOSE(Form("========== Triplett loop (rr,ll,kk) = (%u,%u,%u) ==========", rr, ll, kk));
        bool isBestCandidate   = false;
        tempCandidate.reset();

        // two-step filtering 
        // This means that we only look at the triplet formed by the
        // three leading-in-pT subjets-after-unclustering.
        // FIXME: this is taken from the original HTT, here UNCALIBRATED JETS are sorted
        //        calibration might (?!) potentially change the jet pt ordering!
        if( m_twoStepFilter && ( rr!=1 || ll!=1 || kk!=2 ) ){
          isBestCandidate = true;
          if (!storeAllCandidates) continue;
        }

        // ===========================
        // Step 3: Filtering of the substructure objects
        // ===========================
        MSG_VERBOSE("========== Step 3: filtering 1 ==========");
        //filtering, radius = std::min(m_radiusFilter, dR(subjet_i, subjet_j)) )
        double filteringR = std::min(m_radiusFilter, 0.5*sqrt(std::min(jetParts[kk].squared_distance(jetParts[ll]), 
                std::min(jetParts[rr].squared_distance(jetParts[ll]), jetParts[kk].squared_distance(jetParts[rr])))));
        MSG_VERBOSE( Form( "filteringR = %f", filteringR ) ); 

        // define triplett constituents  
        std::vector<fastjet::PseudoJet> triplettConstits;
        triplettConstits.reserve(30); // guessed max size
        jetAssocCs->add_constituents( jetParts[rr], triplettConstits );
        jetAssocCs->add_constituents( jetParts[ll], triplettConstits );
        jetAssocCs->add_constituents( jetParts[kk], triplettConstits );

        // m_taggerJetAlgorithm: 0 = kt, 1 = C/A, 2 = antikt
        fastjet::JetDefinition filteringJetDef(fastjet::JetAlgorithm(m_taggerJetAlgorithm), filteringR);

        // NOTE: There was some GhostedAreaSpec functionality in the orig BoostedJetTaggers::HEPTopTagger code.
        //       It was not used in the latest verision of the code
        //       No one I asked new who put it there and why
        //       And it was very ugly programming, so I took it out completely
        //       If someone comes along and wants this functionality, he or she can add it :D
        fastjet::ClusterSequence filteringCs(triplettConstits,  filteringJetDef);
        // extract top subjets
        std::vector <fastjet::PseudoJet> filteredSubjets = inclCalSjClustering(&filteringCs);
        // print block for debugging
        for(unsigned ii = 0; ii<filteredSubjets.size() ; ii++) 
          MSG_VERBOSE( Form("inclusive subjet: %u with pT=%f, eta=%f", ii, filteredSubjets[ii].pt(), filteredSubjets[ii].eta()) );

        // ===========================
        // Step 4: Keep m_nJetsFilter hardest subjets and test with top-quark mass
        // ===========================
        MSG_VERBOSE("========== Step 4: filtering 2 ==========");
        fastjet::PseudoJet topCandidate(0.0,0.0,0.0,0.0);
        std::vector <fastjet::PseudoJet> topCandConstits;
        topCandConstits.resize(0);
        for(unsigned ii = 0; ii<std::min((unsigned)m_nJetsFilter, (unsigned)filteredSubjets.size()); ii++) {
          MSG_VERBOSE( Form(" filteredSubjets[%u] pt=%f", ii, filteredSubjets[ii].pt()) );
          filteringCs.add_constituents( filteredSubjets[ii], topCandConstits );
          // NOTE: there was some custom indexing in the origina BoostedJetTaggers::HEPTopTagger
          //       It was ugly programming and I didn't see any purpose in it so I removed it
          topCandidate += filteredSubjets[ii];
        }

        double mTopCand  = topCandidate.m();
        double pTTopCand = topCandidate.pt();
        bool httMassTag  = httMassCut( mTopCand );
        bool httPtTag    = httPtCut( pTTopCand );
        MSG_VERBOSE( Form("topCandidate.m()=%f, pT=%f", mTopCand, pTTopCand) );

        // mass window and pT cut
        if( !m_storeUntagged && !httMassTag ) continue;
        // NOTE: before pt cut was not treated as part of tag
        //       but in strange way: candidates failing the pT cut were "invalid"
        //       but could still be considered in mass sorting. Doesn't make much
        //       sense to me...
        //       alternative: remove from tag and only consider pT passed candidates for ordering
        if( !m_storeUntagged && !httPtTag ) continue;

        // ===========================
        // Step 5: form exactly 3 top candidate subjets
        // ===========================
        MSG_VERBOSE("========== Step 5: build ==3 children ==========");
        fastjet::JetDefinition reclustering(fastjet::JetAlgorithm(m_taggerJetAlgorithm), 3.14/2);
        fastjet::ClusterSequence* csTopChildren = new fastjet::ClusterSequence(topCandConstits,reclustering);
        if (topCandConstits.size() < 3) {
          MSG_VERBOSE( Form("Only %lu constituents for exclusive subjets...", topCandConstits.size()) );
          continue;
        }
        std::vector <fastjet::PseudoJet> topCandChildren = exclCalSjClustering(csTopChildren, 3);
        if( topCandChildren.size() < 3 ){
          MSG_VERBOSE( Form("Only %lu valid top children found...", topCandChildren.size()));
          continue;
        }
        for( unsigned ii=0; ii<topCandChildren.size(); ii++)
          MSG_VERBOSE( Form("top child i=%u pT=%f", ii, topCandChildren[ii].pt()) );

        //catch exceptions as occasionally exclusive filtering fails and fastjet::Error is thrown in delete_self_when_unused()
        // NOTE: taken from original BoostedJetTaggers impl. don't know what it's for
        try {
          csTopChildren->delete_self_when_unused(); // Because we'll need this sequence when dealing with subjets
        } catch (fastjet::Error) {
          continue;
        }

        // ===========================
        // Step 6: Mass plane cut (A-cut)
        // ===========================
        MSG_VERBOSE("========== Step 6: A cut ==========");
        bool httMassRatiosTag = httMassRatiosCut( topCandChildren );
        bool isTagged         = (httMassRatiosTag && httMassTag && httPtTag);
        // FIXME: maybe replace AcutBeforeSorting by TagBeforeSorting?
        // if not cand satisfies AShapeCuts, still want to store best
        bool considerForTopCandSorting = (!m_AcutBeforeSorting || httMassRatiosTag || !bestCandidate.getTagMassRatios() );
        bool firstValidAcutTopCand     = m_AcutBeforeSorting && !bestCandidate.getTagMassRatios() && httMassRatiosTag;

        // not a cut--> consider all
        // a cut -> consider only those tagged, unless no tagged is found

        tempCandidate.setTaggingFlags( true, isTagged, httMassTag, httMassRatiosTag, httPtTag );
        tempCandidate.setTopPseudoJets( topCandidate, topCandChildren, topCandConstits);
        tempCandidate.setOptimalR(jetR);

        tempCandidate.calcBWWIndices( m_WMass );
        tempCandidate.calcMassRatioVariables( m_topMass, m_WMass );

        if( !m_twoStepFilter && considerForTopCandSorting ){

          if( sortTopCandByPt ){ // sort in pt
            if( pTTopCand > bestTopCandPt || firstValidAcutTopCand){
              MSG_VERBOSE( Form("found new leading pt top candidate w/ pt=%f", pTTopCand));
              isBestCandidate = true;
              bestTopCandPt = pTTopCand;
            }
          }
          else if( sortTopCandByAshape ){
            if( tempCandidate.getAshapeVariable() < bestTopCandAshape || firstValidAcutTopCand){
              MSG_VERBOSE( Form("found new Ashape cut ordered top candidate w/ f_A=%f", tempCandidate.getAshapeVariable()));
              isBestCandidate = true;
              bestTopCandAshape = tempCandidate.getAshapeVariable();
            }
          }
          else{ // sort in deltaM
            double deltaM = std::abs(m_topMass-mTopCand);
            if( deltaM < bestTopCandDeltaM || firstValidAcutTopCand){
              MSG_VERBOSE( Form("found new mass ordered top candidate w/ m=%f",mTopCand));
              isBestCandidate = true;
              bestTopCandDeltaM = deltaM;
            }
          }
        }

        if( !m_storeUntagged && !tempCandidate.isTagged() ){
          MSG_VERBOSE(      " reset bc not tagged ");
          MSG_VERBOSE( Form("       mass tag       =%i", tempCandidate.getTagMass()));
          MSG_VERBOSE( Form("       pt tag         =%i", tempCandidate.getTagPt()));
          MSG_VERBOSE( Form("       mass ratios tag=%i", tempCandidate.getTagMassRatios()));
          tempCandidate.reset();
        }
        if( isBestCandidate ){
          bestCandidate = tempCandidate;
          MSG_VERBOSE("new best cand");
        }
        if( storeAllCandidates && (tempCandidate.isTagged() || m_storeUntagged) )
          allCandidates->push_back( tempCandidate );
        nHTTCandidates += 1;

      } // end kk 
    } // end ll
  } // end rr

  MSG_DEBUG("==== HEPTopTagger fixed R Result ====");
  if( !bestCandidate.isValidCandidate() ){
    MSG_DEBUG(" --> No valid candidate found. Happens if: ");
    MSG_DEBUG("       jet fails kinematic cuts ");
    MSG_DEBUG("       or if untagged candidates are not stored and not tag found");
    MSG_DEBUG("       or filtering gave no 3 acceptable candidate children");
  }
  else{
    fastjet::PseudoJet bestTopCandidateFJ = bestCandidate.getTopCandidate();
    MSG_DEBUG(Form(" --> top candidate pt =%f", bestTopCandidateFJ.pt()));
    MSG_DEBUG(Form(" --> top candidate  m =%f", bestTopCandidateFJ.m()));
    MSG_DEBUG(Form(" --> is tagged        =%i", bestCandidate.isTagged()));
    MSG_DEBUG(Form("       mass tag       =%i", bestCandidate.getTagMass()));
    MSG_DEBUG(Form("       pt tag         =%i", bestCandidate.getTagPt()));
    MSG_DEBUG(Form("       mass ratios tag=%i", bestCandidate.getTagMassRatios()));
  }
  if( !storeAllCandidates ) delete allCandidates;
  else sortTopCandidates( allCandidates );
  return bestCandidate;
}
// ====================================================================================
// 
// ====================================================================================
HEPTopTaggerImpl::HTTResult HEPTopTaggerImpl::runTaggerOptimalR( 
    const fastjet::PseudoJet& jet, 
    std::vector<HEPTopTaggerImpl::HTTResult>* allRCandidates ) const
{
  // recluster fatjet with decreasing R until a 2
  MSG_DEBUG("==================================================");
  MSG_DEBUG(Form("runTaggerOptimalR on jet (pT,eta,phi,m)=(%.2f,%.2f,%.2f,%.2f)", jet.pt(),jet.eta(),jet.phi(),jet.m()));
  MSG_DEBUG("==================================================");
  std::vector<fastjet::PseudoJet> bigFatjets;
  std::vector<fastjet::PseudoJet> smallFatjets;
  bigFatjets.push_back( jet );

  // try to get associated cluster sequence
  const fastjet::ClusterSequence* jetAssocCs = jet.associated_cluster_sequence();
  if(! jetAssocCs){
    MSG_ERROR("HTT failed: No availaible ClusterSequence!");
    return HEPTopTaggerImpl::HTTResult();
  }
  double jetR = jetAssocCs->jet_def().R();

  int maxR  = int(jetR * 10);
  int minR  = int(m_minOptimalR * 10);
  int stepR = int(m_stepOptimalR * 10);

  bool storeAllCandidates    = (allRCandidates!=0);
  bool sortTopCandByPt       = ( m_topCandSortingMode == SortByPt);
  bool sortTopCandByAshape   = ( m_topCandSortingMode == SortByAshapeVariable);
  if( !allRCandidates ) allRCandidates = new std::vector< HEPTopTaggerImpl::HTTResult >;
  int iRopt   = -1;

  for( int R=maxR, iR=0; R>=minR; R-=stepR, iR++ ) {
    unclusterFatjets(bigFatjets, smallFatjets, *jetAssocCs, R / 10.);
    MSG_DEBUG( Form("R = %i(%i) -> number of small fatjets = %lu", R, iR, smallFatjets.size()) );

    double bestTopCandPt     = -9999999; // allow chosing max pt candidate
    double bestTopCandDeltaM =  9999999; // allow chosing min delta m candidate
    double bestTopCandAshape =  9999999; // allow choosing min 1D Ashape variable

    allRCandidates->push_back( HEPTopTaggerImpl::HTTResult() );

    for( unsigned i = 0; i < smallFatjets.size(); i++ ) {
      HEPTopTaggerImpl::HTTResult tempCandidate = this->runTaggerFixedR( smallFatjets[i] );
      tempCandidate.setOptimalR( R/10. );
      if( !tempCandidate.isValidCandidate() ) continue;
      bool considerForTopCandSorting = (!m_AcutBeforeSorting || tempCandidate.getTagMassRatios() || !allRCandidates->at(iR).getTagMassRatios() );
      if( !considerForTopCandSorting) continue;
      bool firstValidAcutTopCand = m_AcutBeforeSorting && tempCandidate.getTagMassRatios() && !allRCandidates->at(iR).getTagMassRatios();


      if( sortTopCandByPt ){ // sort in pt
        double pt = tempCandidate.getTopCandidate().pt();
        if( pt > bestTopCandPt || firstValidAcutTopCand){ 
          bestTopCandPt          = pt;
          allRCandidates->at(iR) = tempCandidate;
        }
      }
      else if( sortTopCandByAshape ){ // sort by Ashape variable
        if( tempCandidate.getAshapeVariable() < bestTopCandAshape || firstValidAcutTopCand ){
          bestTopCandAshape = tempCandidate.getAshapeVariable();
          allRCandidates->at(iR) = tempCandidate;
        }
      }
      else{ // sort in mass
        double deltaM = std::abs(m_topMass-tempCandidate.getTopCandidate().m());
        if( deltaM < bestTopCandDeltaM || firstValidAcutTopCand){
          bestTopCandDeltaM  = deltaM;
          allRCandidates->at(iR)        = tempCandidate;
        }
      }
    }// end loop over small fatjets

    // find optimal R
    if( iRopt == -1 && R < maxR ) {                 
      // If the new mass is OUTSIDE the window ..
      if ( !allRCandidates->at(iR).isValidCandidate() ||
          allRCandidates->at(iR).getTopCandidate().m() < (1-m_optimalRThreshold)*allRCandidates->at(0).getTopCandidate().m()){
        // .. set Ropt to the previous mass 
        iRopt = iR-1;
        if( !storeAllCandidates ) break;
      }
    }
    bigFatjets = smallFatjets;
    smallFatjets.clear();
  }//End of loop over R

  // if we did not find Ropt in the loop, pick the last value
  if( iRopt == -1 ){
    if( allRCandidates->at(0).isValidCandidate()) 
      iRopt = allRCandidates->size()-1;
    else // no valid candidate at all
      iRopt = 0;
  }

  HEPTopTaggerImpl::HTTResult bestCandidate = allRCandidates->at(iRopt);
  MSG_DEBUG("==== HEPTopTagger optimal R Result ====");
  if( !bestCandidate.isValidCandidate() ){
    MSG_DEBUG(" --> No valid candidate found. Happens if: ");
    MSG_DEBUG("       jet fails kinematic cuts ");
    MSG_DEBUG("       or if untagged candidates are not stored and not tag found");
    MSG_DEBUG("       or filtering gave no 3 acceptable candidate children");
  }
  else{
    fastjet::PseudoJet bestTopCandidateFJ = bestCandidate.getTopCandidate();
    MSG_DEBUG(Form(" --> top candidate pt =%f", bestTopCandidateFJ.pt()));
    MSG_DEBUG(Form(" --> top candidate  m =%f", bestTopCandidateFJ.m()));
    MSG_DEBUG(Form(" --> optimal R        =%f", bestCandidate.getOptimalR()));
    MSG_DEBUG(Form(" --> is tagged        =%i", bestCandidate.isTagged()));
    MSG_DEBUG(Form("       mass tag       =%i", bestCandidate.getTagMass()));
    MSG_DEBUG(Form("       pt tag         =%i", bestCandidate.getTagPt()));
    MSG_DEBUG(Form("       mass ratios tag=%i", bestCandidate.getTagMassRatios()));
  }
  if( !storeAllCandidates ) delete allRCandidates;
  return bestCandidate;
}
// ====================================================================================
// 
// ====================================================================================
void HEPTopTaggerImpl::printConfig( void ) const
{
  MSG_INFO("#--------------------------------------------------------------------------");
  MSG_INFO("#                           HTT Configuration                              ");
  MSG_INFO(Form("# fatjet min pt      = %f", m_minFatjetPt                              ));
  MSG_INFO(Form("# fatjet min m       = %f", m_minFatjetM                               ));
  MSG_INFO(Form("# fatjet max abs eta = %f\n", m_maxFatjetAbsEta                        ));

  MSG_INFO(Form("# topCandSortingMode = %i", m_topCandSortingMode                       ));

  MSG_INFO(Form("# AcutBeforeSorting  = %i", m_AcutBeforeSorting                        ));
  MSG_INFO(Form("# twoStepFilter      = %i", m_twoStepFilter                            ));
  MSG_INFO(Form("# optimalRTagger     = %i", m_optimalRTagger                           ));
  MSG_INFO(Form("# storeUntagged      = %i\n", m_storeUntagged                          ));

  MSG_INFO(Form("# massDropMaxSjMass  = %f", m_massDropMaxSjMass                        ));
  MSG_INFO(Form("# massDropThreshold  = %f", m_massDropThreshold                        ));
  MSG_INFO(Form("# minSubjetPt        = %f", m_minSubjetPt                              ));
  MSG_INFO(Form("# maxSubjetAbsEta    = %f", m_maxSubjetAbsEta                          ));
  MSG_INFO(Form("# radiusFilter       = %f", m_radiusFilter                             ));
  MSG_INFO(Form("# nJetsFilter        = %i", m_nJetsFilter                              ));
  MSG_INFO(Form("# taggerJetAlgorithm = %i\n", m_taggerJetAlgorithm                     ));

  MSG_INFO(Form("# optimalRThreshold  = %f", m_optimalRThreshold                        ));
  MSG_INFO(Form("# minOptimalR        = %f", m_minOptimalR                              ));
  MSG_INFO(Form("# stepOptimalR       = %f\n", m_stepOptimalR                           ));

  MSG_INFO(Form("# tagMassCutMin      = %f", m_tagMassCutMin                            ));
  MSG_INFO(Form("# tagMassCutMax      = %f", m_tagMassCutMax                            ));
  MSG_INFO(Form("# tagAtan1312CutMin  = %f", m_tagAtan1312CutMin                        ));
  MSG_INFO(Form("# tagAtan1312CutMax  = %f", m_tagAtan1312CutMax                        ));
  MSG_INFO(Form("# tagM23M123CutMin   = %f", m_tagM23M123CutMin                         ));
  MSG_INFO(Form("# tagRelMassWidth    = %f", m_tagRelMassWidth                          ));
  MSG_INFO(Form("# tagPtCutMin        = %f\n", m_tagPtCutMin                            ));

  MSG_INFO(Form("# topMass            = %f", m_topMass                                  ));
  MSG_INFO(Form("# WMass              = %f\n", m_WMass                                  ));

  MSG_INFO(Form("# calibrateSubjets       = %i", m_calibrateSubjets                     ));
  MSG_INFO(Form("# cutCalibratedSubjetPt  = %i", m_cutCalibratedSubjetPt                ));

  //    if( m_pseudoJetCalTool ){    
  //MSG_INFO(Form("# PseudoJet cal tool = %s", m_pseudoJetCalTool->Name().c_str()         ));
  //    }
  MSG_INFO("#--------------------------------------------------------------------------");
}
// ====================================================================================
// 
// ====================================================================================
void HEPTopTaggerImpl::setMinFatjetPt( double pt ){
  m_minFatjetPt = pt;
}
void HEPTopTaggerImpl::setMinFatjetM( double m ){
  m_minFatjetM = m;
}
void HEPTopTaggerImpl::setMaxFatjetAbsEta( double absEta ){
  m_maxFatjetAbsEta = absEta;
}
void HEPTopTaggerImpl::setTopCandSortingMode( TopCandSortingMode mode ){
  m_topCandSortingMode = mode;
}
void HEPTopTaggerImpl::setAcutBeforeSorting( bool flag ){
  m_AcutBeforeSorting = flag;
}
void HEPTopTaggerImpl::setTwoStepFilter( bool flag ){
  m_twoStepFilter = flag;
}
void HEPTopTaggerImpl::setOptimalRTagger( bool flag ){
  m_optimalRTagger = flag;
}
void HEPTopTaggerImpl::setStoreUntagged( bool flag ){ 
  m_storeUntagged = flag;
}
void HEPTopTaggerImpl::setMassDropMaxSjMass( double mdMaxSjMass ){
  m_massDropMaxSjMass = mdMaxSjMass;
}
void HEPTopTaggerImpl::setMassDropThreshold( double mdThreshold ){
  m_massDropThreshold = mdThreshold;
}
void HEPTopTaggerImpl::setMinSubjetPt( double minSjPt ){
  m_minSubjetPt = minSjPt;
}
void HEPTopTaggerImpl::setMaxSubjetAbsEta( double maxSjAbsEta ){
  m_maxSubjetAbsEta = maxSjAbsEta;
}
void HEPTopTaggerImpl::setRadiusFilter( double rFilt ){
  m_radiusFilter = rFilt;
}
void HEPTopTaggerImpl::setNJetsFilter( int nFilt   ){
  m_nJetsFilter = nFilt;
}
void HEPTopTaggerImpl::setTaggerJetAlgorithm( int jetAlgo ){
  m_taggerJetAlgorithm = jetAlgo;
}
void HEPTopTaggerImpl::setOptimalRThreshold( double optRTreshold ){
  m_optimalRThreshold = optRTreshold;
}
void HEPTopTaggerImpl::setMinOptimalR( double minOptR ){
  m_minOptimalR = minOptR;
}
void HEPTopTaggerImpl::setStepOptimalR( double stepOptR ){
  m_stepOptimalR = stepOptR;
}
void HEPTopTaggerImpl::setTagMassCutMin( double tagMMin ){
  m_tagMassCutMin = tagMMin;
}
void HEPTopTaggerImpl::setTagMassCutMax( double tagMMax ){
  m_tagMassCutMax = tagMMax;
}
void HEPTopTaggerImpl::setTagAtan1312CutMin( double tagAtanMin ){
  m_tagAtan1312CutMin = tagAtanMin;
}
void HEPTopTaggerImpl::setTagAtan1312CutMax( double tagAtanMax ){
  m_tagAtan1312CutMax = tagAtanMax;
}
void HEPTopTaggerImpl::setTagM23M123CutMin( double  tagM23M123Min ){
  m_tagM23M123CutMin = tagM23M123Min;
}
void HEPTopTaggerImpl::setTagRelMassWidth( double tagRelMWidht ){
  m_tagRelMassWidth = tagRelMWidht;
}
void HEPTopTaggerImpl::setTagPtCutMin( double tagPtMin ){
  m_tagPtCutMin = tagPtMin;
}
void HEPTopTaggerImpl::setTopMass( double topMass){
  m_topMass = topMass;
}
void HEPTopTaggerImpl::setWMass( double WMass){
  m_WMass = WMass;
}
void HEPTopTaggerImpl::setCalibrateSubjets( bool flag ){
  m_calibrateSubjets = flag;
}
void HEPTopTaggerImpl::setCutCalibratedSubjetPt( bool flag ){
  m_cutCalibratedSubjetPt = flag;
}
void HEPTopTaggerImpl::setPseudoJetCalTool( IPseudoJetCalibrationTool* pjCalTool ){
  m_pseudoJetCalTool = pjCalTool;
}
// ====================================================================================
// 
// ====================================================================================
void HEPTopTaggerImpl::printBanner( void ) const
{
  if( !m_firstCall ) return;
  m_firstCall = false;
  MSG_INFO("#--------------------------------------------------------------------------");
  MSG_INFO("#                           HEPTopTagger ?.?                               ");
  MSG_INFO("#                                                                          ");
  MSG_INFO("# Please cite JHEP 1506 (2015) 203 [arXiv:1503.05921 [hep-ph]]             ");
  MSG_INFO("# and JHEP 1010 (2010) 078 [arXiv:1006.2833 [hep-ph]].                     ");
  MSG_INFO("# This code is provided without warranty.                                  ");
  MSG_INFO("#--------------------------------------------------------------------------");
}
// ====================================================================================
// 
// ====================================================================================
void HEPTopTaggerImpl::findHardSubst( 
    const fastjet::PseudoJet        &inJet, 
    std::vector<fastjet::PseudoJet> &outTopParts,
    const fastjet::ClusterSequence  &inClSeq) const
{
  fastjet::PseudoJet parent1(0, 0, 0, 0), parent2(0, 0, 0, 0);
  if (inJet.m() < m_massDropMaxSjMass || !inClSeq.has_parents(inJet, parent1, parent2)) {
    outTopParts.push_back(inJet);
  } else {
    if (parent1.m() < parent2.m()) 
      std::swap(parent1, parent2);   
    findHardSubst(parent1, outTopParts, inClSeq);
    if (parent1.m() < m_massDropThreshold * inJet.m()){
      findHardSubst(parent2, outTopParts, inClSeq);   
    }
  }

}

// ====================================================================================
// 
// ====================================================================================
void HEPTopTaggerImpl::unclusterFatjets(
    const std::vector<fastjet::PseudoJet> & inBigFatjets, 
    std::vector<fastjet::PseudoJet> & outSmallFatjets, 
    const fastjet::ClusterSequence & inClSeq, 
    const double smallR) const
{
  for (unsigned i=0; i < inBigFatjets.size(); i++) {
    fastjet::PseudoJet thisJet = inBigFatjets[i];
    fastjet::PseudoJet parent1(0, 0, 0, 0), parent2(0, 0, 0, 0);
    bool test = inClSeq.has_parents(thisJet, parent1, parent2);
    double dR = 100;

    if(test) dR = std::sqrt(parent1.squared_distance(parent2));

    if (!test || dR<smallR) {
      outSmallFatjets.push_back(thisJet);
    } else {
      std::vector<fastjet::PseudoJet> parents;
      parents.push_back(parent1);
      parents.push_back(parent2);
      unclusterFatjets(parents, outSmallFatjets, inClSeq, smallR);
    }
  }
}
// ====================================================================================
// 
// ====================================================================================
std::vector<fastjet::PseudoJet> HEPTopTaggerImpl::inclCalSjClustering( fastjet::ClusterSequence *cs ) const 
{
  MSG_DEBUG( "inclCalSjClustering" );
  // Find the Subjets
  double ptmin = m_minSubjetPt;
  if (m_cutCalibratedSubjetPt) ptmin = m_minSubjetPt*0.75; 
  std::vector<fastjet::PseudoJet> out_jetVec = fastjet::sorted_by_pt(cs->inclusive_jets( ptmin ));
  // Remove high eta jets
  this->filterPseudoJetsAbsEta( out_jetVec, m_maxSubjetAbsEta );
  //Calibrate the subjets
  if (m_calibrateSubjets) {
    m_pseudoJetCalTool->calibratePseudoJets( out_jetVec, cs->jet_def().R() );
  }
  if (m_cutCalibratedSubjetPt) 
    this->filterPseudoJetsPtMin( out_jetVec, m_minSubjetPt );
  return fastjet::sorted_by_pt(out_jetVec); // calibration might change pT order
}
// ====================================================================================
// 
// ====================================================================================
std::vector<fastjet::PseudoJet> HEPTopTaggerImpl::exclCalSjClustering( fastjet::ClusterSequence *cs, const int njets ) const
{
  MSG_DEBUG( "exclCalSjClustering" );
  // Find the Subjets
  std::vector<fastjet::PseudoJet> out_jetVec = fastjet::sorted_by_pt(cs->exclusive_jets( njets ));
  // Remove low pT and high eta jets
  double ptmin = m_minSubjetPt;
  if (m_cutCalibratedSubjetPt) ptmin = m_minSubjetPt*0.75;
  this->filterPseudoJetsPtMin( out_jetVec, ptmin );
  this->filterPseudoJetsAbsEta( out_jetVec, m_maxSubjetAbsEta );
  //Calibrate the subjets, for exclusively clustered jets, there's no R-parameter
  if (m_calibrateSubjets) {
    m_pseudoJetCalTool->calibratePseudoJets( out_jetVec, 0 );
  }
  if (m_cutCalibratedSubjetPt)
    this->filterPseudoJetsPtMin( out_jetVec, m_minSubjetPt );
  return fastjet::sorted_by_pt(out_jetVec); // calibration might change pT order
}
// ====================================================================================
//
// ====================================================================================
void HEPTopTaggerImpl::filterPseudoJetsPtMin(  std::vector<fastjet::PseudoJet>& pjVec, double ptCut ) const
{
  std::vector<fastjet::PseudoJet> tmp;
  tmp.reserve(pjVec.size());
  for(fastjet::PseudoJet& pj: pjVec ) if( pj.perp() > ptCut ) tmp.push_back(pj);
  tmp.swap(pjVec);
}
// ====================================================================================
//
// ====================================================================================
void HEPTopTaggerImpl::filterPseudoJetsAbsEta( std::vector<fastjet::PseudoJet>& pjVec, double etaCut ) const
{
  std::vector<fastjet::PseudoJet> tmp; 
  tmp.reserve(pjVec.size());
  for(fastjet::PseudoJet& pj: pjVec ) if( fabs(pj.eta()) <= etaCut ) tmp.push_back(pj);
  tmp.swap(pjVec);
}
// ====================================================================================
//
// ====================================================================================
bool HEPTopTaggerImpl::httMassCut( double mTopCand ) const{
  return ( m_tagMassCutMin < mTopCand && mTopCand < m_tagMassCutMax );
}
bool HEPTopTaggerImpl::httPtCut( double pTTopCand ) const{
  return( m_tagPtCutMin < pTTopCand );
}
// ====================================================================================
//
// ====================================================================================
bool HEPTopTaggerImpl::httMassRatiosCut(  const std::vector<fastjet::PseudoJet> &topChildren ) const
{
  // HTT mass ratio cut
  // basic principle is there should be one pair mij close to the W mass
  // to reduce scale dependence look at the ratio mij/m123 ~ mW/mt
  // there are some asymmetries between the pairs (e.g. different box cuts) 
  // which are possibly motivated by the expected pT ordering of the top children

  bool is_passed = false;
  double m12  = (topChildren[0] + topChildren[1]).m();
  double m13  = (topChildren[0] + topChildren[2]).m();
  double m23  = (topChildren[1] + topChildren[2]).m();
  double m123 = (topChildren[0] + topChildren[1] + topChildren[2]).m();

  double atan1312 = std::atan(m13/m12);

  double rmin = (1-m_tagRelMassWidth/2. )*m_WMass/m_topMass;
  double rmax = (1+m_tagRelMassWidth/2. )*m_WMass/m_topMass;

  if (
      ( m_tagAtan1312CutMin < atan1312 && atan1312 < m_tagAtan1312CutMax
        && (m23/m123 > rmin && rmax > m23/m123))
      ||
      (((m23/m123) * (m23/m123) < 1 - rmin * rmin * (1 + (m13/m12) * (m13/m12)))
       &&
       ((m23/m123) * (m23/m123) > 1 - rmax * rmax * (1 + (m13/m12) * (m13/m12)))
       && 
       (m23/m123 > m_tagM23M123CutMin))
      ||
      (((m23/m123) * (m23/m123) < 1 - rmin * rmin * (1 + (m12/m13) * (m12/m13)))
       &&
       ((m23/m123) * (m23/m123) > 1 - rmax * rmax * (1 + (m12/m13) * (m12/m13)))
       && 
       (m23/m123 > m_tagM23M123CutMin))
     ) { 
    is_passed = true;
  }
  return is_passed;
}
// ====================================================================================
//
// ====================================================================================
void HEPTopTaggerImpl::sortTopCandidates( std::vector<HEPTopTaggerImpl::HTTResult>* allCandidates ) const
{
  MSG_DEBUG( Form("sortTopCandidates %lu candidates", allCandidates->size() ));
  if( allCandidates->size() == 0) return;
  bool   sortTopCandByPt       = ( m_topCandSortingMode == SortByPt);
  bool   sortTopCandByAshape   = ( m_topCandSortingMode == SortByAshapeVariable);

  if( sortTopCandByPt ){
    if( m_AcutBeforeSorting ){
      std::sort( allCandidates->begin(), allCandidates->end(), [](HEPTopTaggerImpl::HTTResult a, HEPTopTaggerImpl::HTTResult b){
          return (
              ( a.getTagMassRatios() &&  b.getTagMassRatios() && a.getTopCandidate().pt() > b.getTopCandidate().pt()) ||
              (!a.getTagMassRatios() && !b.getTagMassRatios() && a.getTopCandidate().pt() > b.getTopCandidate().pt()) ||
              ( a.getTagMassRatios() && !b.getTagMassRatios())
              );
          });
    }
    else{
      std::sort( allCandidates->begin(), allCandidates->end(), [](HEPTopTaggerImpl::HTTResult a, HEPTopTaggerImpl::HTTResult b){
          return a.getTopCandidate().pt() > b.getTopCandidate().pt();
          });
    }
  }
  else if( sortTopCandByAshape ){
    std::sort( allCandidates->begin(), allCandidates->end(), [](HEPTopTaggerImpl::HTTResult a, HEPTopTaggerImpl::HTTResult b){
        return a.getAshapeVariable() < b.getAshapeVariable();
        });
  }
  else{
    if( m_AcutBeforeSorting ){
      std::sort( allCandidates->begin(), allCandidates->end(), [](HEPTopTaggerImpl::HTTResult a, HEPTopTaggerImpl::HTTResult b){
          return (
              ( a.getTagMassRatios() &&  b.getTagMassRatios() && a.getAbsDeltaMTop() < b.getAbsDeltaMTop() ) ||
              (!a.getTagMassRatios() && !b.getTagMassRatios() && a.getAbsDeltaMTop() < b.getAbsDeltaMTop() ) ||
              ( a.getTagMassRatios() && !b.getTagMassRatios())
              );
          });
    }
    else{
      std::sort( allCandidates->begin(), allCandidates->end(), [](HEPTopTaggerImpl::HTTResult a, HEPTopTaggerImpl::HTTResult b){
          return a.getAbsDeltaMTop() < b.getAbsDeltaMTop();
          });
    }
  }
}
// ====================================================================================
//
// ====================================================================================
void HEPTopTaggerImpl::MSG_ERROR( std::string xmsg ) const{
  if( HEPTopTaggerImpl::ERROR <= m_msgLevel )
    ANA_MSG_ERROR( m_msgName << ": " << xmsg );
}
void HEPTopTaggerImpl::MSG_INFO( std::string xmsg ) const{
  if( HEPTopTaggerImpl::INFO <= m_msgLevel )
    ANA_MSG_INFO( m_msgName << ": " << xmsg );
}
void HEPTopTaggerImpl::MSG_DEBUG( std::string xmsg ) const{
  if( HEPTopTaggerImpl::DEBUG <= m_msgLevel )
    ANA_MSG_DEBUG( m_msgName << ": " << xmsg);
}
void HEPTopTaggerImpl::MSG_VERBOSE( std::string xmsg ) const{
  if( HEPTopTaggerImpl::VERBOSE <= m_msgLevel )
    ANA_MSG_VERBOSE( m_msgName << ": " << xmsg);
}
