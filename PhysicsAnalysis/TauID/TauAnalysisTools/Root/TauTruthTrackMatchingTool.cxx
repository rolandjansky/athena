/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TauAnalysisTools/TauTruthTrackMatchingTool.h>

using namespace TauAnalysisTools;

//=================================PUBLIC-PART==================================
//______________________________________________________________________________
TauTruthTrackMatchingTool::TauTruthTrackMatchingTool( const std::string& name )
  : AsgTool(name)
{
}

//______________________________________________________________________________
TauTruthTrackMatchingTool::~TauTruthTrackMatchingTool( )
{
}

//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::initialize()
{
  ATH_MSG_INFO( "Initialising TauTruthTrackMatchingTool" );
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::classifyTrack(const xAOD::TrackParticle& xTrackParticle)
{
  checkTrackIsTauInheritant(xTrackParticle);
  checkTrackSpuriousType(xTrackParticle);
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTruthTrackMatchingTool::classifyTracks(std::vector<const xAOD::TrackParticle*>& vTrackParticles)
{
  for (auto xTrackParticle : vTrackParticles)
  {
    checkTrackIsTauInheritant(*xTrackParticle);
    checkTrackSpuriousType(*xTrackParticle);
  }
  return StatusCode::SUCCESS;
}

//=================================PRIVATE-PART=================================
//______________________________________________________________________________
void TauTruthTrackMatchingTool::checkTrackSpuriousType(const xAOD::TrackParticle& xTrackParticle)
{
  const xAOD::TruthParticle* xTruthParticle = getTruthParticle(xTrackParticle);

  // if not truth track is available -> unclassified
  if (!xTruthParticle)
  {
    xTrackParticle.auxdecor<int>("SpuriousType") = TauAnalysisTools::UnclassifiedTrack;
    return;
  }

  int iBarcode = xTruthParticle->barcode();
  if (xTrackParticle.auxdata<float>("truthMatchProbability") >= 0.5)
  {
    if (iBarcode >= 10000 && iBarcode < 200000) xTrackParticle.auxdecor<int>("SpuriousType") = TauAnalysisTools::PileupTrack;
    else if (iBarcode >= 200000)                xTrackParticle.auxdecor<int>("SpuriousType") = TauAnalysisTools::ConversionTrack;
    else if (iBarcode > 0 && iBarcode < 10000)  xTrackParticle.auxdecor<int>("SpuriousType") = TauAnalysisTools::UnderlyingEventTrack;
    else if (iBarcode == 0)                     xTrackParticle.auxdecor<int>("SpuriousType") = TauAnalysisTools::FakeTrack;
  }
  else
  {
    if (iBarcode < 200000)                      xTrackParticle.auxdecor<int>("SpuriousType") = TauAnalysisTools::FakeTrack;
    else                                        xTrackParticle.auxdecor<int>("SpuriousType") = TauAnalysisTools::ConversionTrack;
  }
}


//______________________________________________________________________________
const xAOD::TruthParticle* TauTruthTrackMatchingTool::getTruthParticle(const xAOD::TrackParticle& xTrackParticle)
{
  auto xTruthParticleContainer = xTrackParticle.auxdata<ElementLink<xAOD::TruthParticleContainer>>("truthParticleLink");
  //check validity of truth particle element link
  if (xTruthParticleContainer.isValid())
    return (*xTruthParticleContainer);
  return NULL;
}

//______________________________________________________________________________
void TauTruthTrackMatchingTool::checkTrackIsTauInheritant(const xAOD::TrackParticle& xTrackParticle)
{
  xTrackParticle.auxdecor<bool>("IsHadronicTrack") = false;
  int iDepth = -1;
  const xAOD::TruthParticle* xTruthParticle = getTruthParticle(xTrackParticle);
  std::string sHistory = "";
  if (xTruthParticle)
  {
    sHistory = std::to_string(xTruthParticle->pdgId());
    if (checkTruthParent(*xTruthParticle, iDepth, sHistory))
    {
      xTrackParticle.auxdecor<bool>("IsHadronicTrack") = true;
    }
  }
  xTrackParticle.auxdecor<std::string>("DecayHistory") = sHistory;
  xTrackParticle.auxdecor<int>("IsHadronicTrackDecayDepth") = iDepth;
}

//______________________________________________________________________________
bool TauTruthTrackMatchingTool::checkTruthParent(const xAOD::TruthParticle& xTruthParticle, int& iDepth, std::string& sHistory)
{
  iDepth++;
  if (xTruthParticle.hasProdVtx())
  {
    const xAOD::TruthVertex* xVertex = xTruthParticle.prodVtx();
    if (xVertex->nIncomingParticles() != 1)
      ATH_MSG_VERBOSE("not exactly one incomming particles for vertex, number of incomming particles: "<<xVertex->nIncomingParticles());
    if (xVertex->nIncomingParticles() > 0)
    {
      const xAOD::TruthParticle* xTruthParticleParent = xVertex->incomingParticle(0);
      // store parent pdgID in history
      sHistory.insert(0, std::to_string(xTruthParticleParent->pdgId())+":");//xTruthParticleParent->pdgId());
      if (xTruthParticleParent->absPdgId() == 15)
      {
        return true;
      }
      else
      {
        return checkTruthParent(*xTruthParticleParent, iDepth, sHistory);
      }
    }
  }
  return false;
}

#ifdef DEVEL_TAUTRUTHTRACKMATCHINGTOOL


#include <TLorentzVector.h>
#include <iostream>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::vector;

// destructor
TruthTrackMatchingTool::~TruthTrackMatchingTool()
{
  if (m_myTauTrack)
  {
    delete m_myTauTrack;
    m_myTauTrack=NULL;
  }
  if (m_myTrack)
  {
    delete m_myTrack;
    m_myTrack=NULL;
  }
  if (m_myTau)
  {
    delete m_myTau;
    m_myTau =NULL;
  }
}

// *************** static members' initialization and static methods' implementation ******************* //
unsigned int TruthTrackMatchingTool::s_counters_tracktype[kTRACKNSPURIOUS_TYPES] = {0};

unsigned int TruthTrackMatchingTool::nTracksType(TrackSpuriousType_t tracktype)
{
  return s_counters_tracktype[tracktype];
}

bool TruthTrackMatchingTool::isGoodDaughter(int status, int barcode, int vtx_barcode)
{
  if ( status == 2 ) return true;
  if ( status < 200 && status > 190 ) return true;
  if ( (status%1000 == 1 || (status%1000 == 2 && status > 1000) || (status == 2 && vtx_barcode < -200000)) && barcode < 10000 ) return true;
  return false;
}

// *************** Main methods of the tool ******************* //

// ----- Initialization method ----- //
void TruthTrackMatchingTool::Initialize( vector<float> *_mc_pt,
    vector<float> *_mc_eta,
    vector<float> *_mc_phi,
    vector<float> *_mc_m,
    vector<float> *_mc_charge,
    vector<int>   *_mc_pdgId,
    vector<int>   *_mc_status,
    vector<int>   *_mc_barcode,
    vector<int>   *_mc_vx_barcode,
    vector<vector<int> > *_mc_child_index,
    vector<vector<int> > *_mc_parent_index,
    vector<float>	*_trk_pt,
    vector<float>	*_trk_eta,
    vector<float>	*_trk_phi,
    vector<float>	*_trk_mc_probability,
    vector<int>	*_trk_mc_barcode)
{
  m_myBlock.set_addresses(_mc_pt,
                          _mc_eta,
                          _mc_phi,
                          _mc_m,
                          _mc_charge,
                          _mc_pdgId,
                          _mc_status,
                          _mc_barcode,
                          _mc_vx_barcode,
                          _mc_child_index,
                          _mc_parent_index,
                          _trk_pt,
                          _trk_eta,
                          _trk_phi,
                          _trk_mc_probability,
                          _trk_mc_barcode
                         );
}

// ----- Classify method ----- //

// INPUT: a single tau track
void TruthTrackMatchingTool::Classify(TLorentzVector &TAU_TRACK)
{
  ClearTool();
  m_myTauTrack = MakeTauTrack(TAU_TRACK);
  m_myTrack    = MakeTrk(m_myTauTrack);

  TruthTrackMatching(m_myTrack);
}

// INPUT: a tau with its tau tracks
void TruthTrackMatchingTool::Classify(TLorentzVector &TAU, vector<TLorentzVector> &TAU_TRACKS)
{
  ClearTool();
  m_myTau = MakeTau(TAU, TAU_TRACKS);

  TruthTauMatching(m_myTau);
  TauTrkMatching(m_myTau);
  TauTruthTrackMatching(m_myTau);
}

void TruthTrackMatchingTool::UseDeltaRMatching(bool use_delta_r_matching, float delat_r_cut)
{
  if (use_delta_r_matching)
  {
    m_use_delta_r_matching = true;
    m_delta_r_cut          = delat_r_cut;
  }
}

// *************** Accessory methods of the tool ******************* //

TruthTrackMatchingTool::TauMatchStatus_t TruthTrackMatchingTool::getTauMatchStatus()
{
  if ( !(m_myTau) )   // if there is NOT a tau, return default value and print error message
  {
    cerr << "CAUTION! You don't have a tau to look at - returning default value (0)" << endl;
    return TruthTrackMatchingTool::kTAUMATCHSTATUS_UNCL;
  }
  return m_myTau->WhichTauMatchStatus();
}
TruthTrackMatchingTool::TauType_t TruthTrackMatchingTool::getTauType()
{
  if ( !(m_myTau) )   // if there is NOT a tau, return default value and print error message
  {
    cerr << "CAUTION! You don't have a tau to look at - returning default value (0)" << endl;
    return TruthTrackMatchingTool::kTAUTYPE_UNCL;
  }
  return m_myTau->WhichTauType();
}
TruthTrackMatchingTool::TrackMatchStatus_t  TruthTrackMatchingTool::getTrackMatchStatus()
{
  if ( !(m_myTrack) )   // if there is NOT a tau, return default value and print error message
  {
    cerr << "CAUTION! You don't have a track to look at - returning default value (0)" << endl;
    return TruthTrackMatchingTool::kTRACKMATCHSTATUS_UNCL;
  }
  return m_myTrack->WhichTrackMatchStatus();
}
TruthTrackMatchingTool::TrackSpuriousType_t TruthTrackMatchingTool::getTrackSpuriousType()
{
  if ( !(m_myTrack) )   // if there is NOT a tau, return default value and print error message
  {
    cerr << "CAUTION! You don't have a track to look at - returning default value (0)" << endl;
    return TruthTrackMatchingTool::kTRACKSPURIOUSTYPE_UNCL;
  }
  return m_myTrack->WhichSpuriousType();
}
int TruthTrackMatchingTool::getMCParticleMatchIndex()
{
  if ( !(m_myTrack) )   // if there is NOT a tau, return default value and print error message
  {
    cerr << "CAUTION! You don't have a track to look at - returning default value (-1)" << endl;
    return -1;
  }

  if (m_myTrack->WhichTrackMatchStatus() == TruthTrackMatchingTool::kTRACKMATCHSTATUS_TRUTH_MATCHED)
  {
    return m_myTrack->TruthTrackIndex();
  }
  else
  {
    cerr << "CAUTION! You are trying to get the match index of a track which is spurious! - returning default value (-1)" << endl;
    return -1;
  }
}


float TruthTrackMatchingTool::getDeltaRTruthPartner()
{
  return m_myTrack->DeltaRTruthPartner();
}

void TruthTrackMatchingTool::ClearTool()
{
  if (m_myTauTrack)
  {
    delete m_myTauTrack;
    m_myTauTrack=NULL;
  }
  if (m_myTrack)
  {
    delete m_myTrack;
    m_myTrack=NULL;
  }
  if(m_myTau)
  {
    delete m_myTau;
    m_myTau =NULL;
  }
}

// **************** "Output" methods ******************* //

void TruthTrackMatchingTool::SingleTrackOutput(Track* track)
{

  cout << "  Input TauTrack INFO: "                         << "\n"
       << "    pT: "  << track->Pt()  << " [GeV]"     << "\n"
       << "    eta: " << track->Eta()                 << "\n"
       << "    phi: " << track->Phi()                 << "\n"
       << "    ---------- "                           << "\n"
       << "    barcode: "     << track->Barcode()     << "\n"
       << "    probability: " << track->Probability() << endl;
  cout << " " << endl;

  switch(track->WhichTrackMatchStatus())
  {
  case 1 :
    cout << "  is TRUTH-MATCHED to 1 truth charged pion/kaon from tau decay"<< endl;
    cout << "   with features: " <<           "\n"
         << "        pT: "  << track->m_truthpartner->Pt()  << " [GeV]"  << "\n"
         << "        eta: " << track->m_truthpartner->Eta() << "\n"
         << "        phi: " << track->m_truthpartner->Phi() << "\n"
         << "        charge: " << track->m_truthpartner->Charge() << "\n"
         << "        ---------- "                         << "\n"
         << "        pdgId: "  << track->m_truthpartner->PdgId() << "\n"
         << "        barcode: "  << track->m_truthpartner->Barcode()   << "\n"
         << "        DeltaR(track): " << track->DeltaRTruthPartner()   << "\n"
         << endl;
    break;
  case 2 :
    cout << "  is SPURIOUS "<< endl;
    break;
  case 3 :
    cout << "  is TRUTH-MATCHED to MORE THAN 1 particle "<< endl;
    break;
  default :
    cout << "  is UNCLASSIFIED - Something terribly wrong happened! "<< endl;
  }

  if(track->WhichTrackMatchStatus() == kTRACKMATCHSTATUS_TRUTH_UNMATCHED)
  {
    switch(track->WhichSpuriousType())
    {
    case 1 :
      cout << "     is from CONVERSION "<< endl;
      break;
    case 2 :
      cout << "     is from PILEUP "<< endl;
      break;
    case 3 :
      cout << "     is from Underlying Event "<< endl;
      break;
    case 4 :
      cout << "     is a FAKE "<< endl;
      break;
    default :
      cout << "     is SPURIOUS_TYPE-UNCLASSIFIED - Check the mc_probability "<< endl;
    }
  }
}

void TruthTrackMatchingTool::PrintOutput()
{

  if ( !(m_myTau) && (m_myTrack) )  // if there is NOT a tau, look at the single track
  {

    SingleTrackOutput(m_myTrack);

  }
  else if (m_myTau)   	       // if there is a tau, look at the tau tracks
  {
    switch(m_myTau->WhichTauMatchStatus())
    {
    case 1 :
      cout << "  with features: "                           << "\n"
           << "    pT:  "  << m_myTau->Pt()  << " [GeV]"     << "\n"
           << "    eta: "  << m_myTau->Eta()		    << "\n"
           << "    phi: "  << m_myTau->Phi()		    << "\n"
           << "    M:   "  << m_myTau->M()		    << "\n"
           << "    ---------- "                              << endl;
      cout << "is TRUTH-MATCHED "<< endl;
      break;
    case 2 :
      cout << "is NOT TRUTH-MATCHED "<< endl;
    default :
      cout << "is UNCLASSIFIED "<< endl;
    }
    if(m_myTau->WhichTauMatchStatus() == kTAUMATCHSTATUS_TRUTH_MATCHED)
    {
      switch(m_myTau->WhichTauType())
      {
      case 1 :
        cout << "   to a 1-prong truth tau "<< endl;
        break;
      case 3 :
        cout << "   to a 3-prong truth tau "<< endl;
        break;
      case 5 :
        cout << "   to a > 3-prong truth tau "<< endl;
        break;
      default :
        cout << "   to a truth tau, but with unexpected # prongs (0, 2) - Something strange in the truth-particle definition? "<< endl;
      }
      cout << "   with features: " <<           "\n"
           << "        pT: "  << m_myTau->m_truthpartner->Pt()  << " [GeV]"  << "\n"
           << "        eta: " << m_myTau->m_truthpartner->Eta() << "\n"
           << "        phi: " << m_myTau->m_truthpartner->Phi() << "\n"
           << "        m: "   << m_myTau->m_truthpartner->M()   << "\n"
           << "        DeltaR(tau): "   <<  m_myTau->DeltaR(*(m_myTau->m_truthpartner)) << "\n"
           << "        # good charged daughters: "   <<  m_myTau->m_truthpartner->m_charged_daughters.size()        << "\n"
           << endl;
      for (unsigned int iDgt = 0; iDgt < m_myTau->m_truthpartner->m_charged_daughters.size(); iDgt++)
      {
        cout << "           Daughter:  " << iDgt  << "\n"
             << "              pT: "  << m_myTau->m_truthpartner->m_charged_daughters[iDgt]->Pt()  << " [GeV]"  << "\n"
             << "              eta: " << m_myTau->m_truthpartner->m_charged_daughters[iDgt]->Eta() << "\n"
             << "              phi: " << m_myTau->m_truthpartner->m_charged_daughters[iDgt]->Phi() << "\n"
             << "              ---------- "                         << "\n"
             << "              barcode: "     << m_myTau->m_truthpartner->m_charged_daughters[iDgt]->Barcode()     << "\n"
             << endl;
      }
      // print out tau track matchings
      int nTracks = m_myTau->m_tracks.size();
      for (int iTrack = 0; iTrack < nTracks; iTrack++)
      {
        Track *this_track = m_myTau->m_tracks[iTrack];
        cout << "TauTrack " << iTrack << " : " << endl;
        cout << "   " << endl;
        SingleTrackOutput(this_track);
      }
    }
  }
  cout << "*******************************************" << endl;
}


// *************** Core methods ***************** //

// 1) Starting from single track as input
Track* TruthTrackMatchingTool::MakeTauTrack(TLorentzVector &tauTrack)
{

  Track *this_tauTrack = new Track();   // deleted by the tool when calling ClearTool(), or when calling Tool destructor
  this_tauTrack->SetPtEtaPhiM(tauTrack.Pt()/1e3,
                              tauTrack.Eta(),
                              tauTrack.Phi(),
                              0.13957);    // set track mass to pi+/- mass [GeV]

  return this_tauTrack;
}

Track* TruthTrackMatchingTool::MakeTrk(Track *tauTrack)
{

  float tauTrack_pt	  = tauTrack->Pt();
  float tauTrack_eta  = tauTrack->Eta();
  float tauTrack_phi  = tauTrack->Phi();

  bool isTauTrackMatched = false;
  int  idxMatchingTrk = -1;

  // loop over trk_* container to find the matching track
  int nTrks = m_myBlock.trk_pt->size();
  for (int iTrk = 0; iTrk < nTrks ; iTrk++)
  {

    float trk_pt    = m_myBlock.trk_pt->at(iTrk)/1e3;
    float trk_eta   = m_myBlock.trk_eta->at(iTrk);
    float trk_phi   = m_myBlock.trk_phi->at(iTrk);
    if (fabs(trk_pt  - tauTrack_pt  ) < 0.01 &&
        fabs(trk_eta - tauTrack_eta ) < 0.01 &&
        fabs(trk_phi - tauTrack_phi ) < 0.01 )
    {

      isTauTrackMatched = true;
      idxMatchingTrk = iTrk;
      break;
    }
  }
  // check that you found a matching trk_ ... what happens if you don't find it?
  if (!isTauTrackMatched) cout << "Could not find a matching trk_ for this tau_track! - WTF?" << endl;

  // now that we have index of corresponding trk in container, return track
  Track *this_track = new Track();  // deleted by the tool when calling ClearTool(), or when calling Tool destructor

  this_track->SetPtEtaPhiM(m_myBlock.trk_pt ->at(idxMatchingTrk)/1e3,
                           m_myBlock.trk_eta->at(idxMatchingTrk),
                           m_myBlock.trk_phi->at(idxMatchingTrk),
                           0.13957); // set track mass to pi+/- mass [GeV]
  // set barcode and probability!
  this_track->SetBarcode    (m_myBlock.trk_mc_barcode    ->at(idxMatchingTrk) );
  this_track->SetProbability(m_myBlock.trk_mc_probability->at(idxMatchingTrk) );

  return this_track;
}

// 2) Starting from single tau and vector<tau tracks> as input
Tau* TruthTrackMatchingTool::MakeTau(TLorentzVector &tau, vector<TLorentzVector> &tau_tracks)
{

  Tau *this_tau = new Tau();  // deleted by the tool when calling ClearTool(), or when calling Tool destructor

  this_tau->SetPtEtaPhiM(tau.Pt()/1e3,
                         tau.Eta(),
                         tau.Phi(),
                         tau.M()/1e3);

  int nTauTracks = tau_tracks.size();
  for (int iTauTrack = 0; iTauTrack < nTauTracks; iTauTrack++)
  {

    Track *this_tautrack = new Track();  // deleted when calling Tau destructor
    this_tautrack->SetPtEtaPhiM(tau_tracks[iTauTrack].Pt()/1e3,
                                tau_tracks[iTauTrack].Eta(),
                                tau_tracks[iTauTrack].Phi(),
                                0.13957);    // set track mass to pi+/- mass [GeV]
    this_tau->m_tautracks.push_back(this_tautrack);
  }

  return this_tau;
}



// 1) VERSION 1 - pass as input the entire mc_ container to find the barcode-matching truth track for input track
void TruthTrackMatchingTool::TruthTrackMatching(Track *track)
{

  // loop over truth mc_ particles
  int nParticles = m_myBlock.mc_pdgId->size();

  int matchCounter = 0; // when you find a truth-match to THIS track, increment it. In principle it should become 1 for good tau tracks, 0 for spurious
  for (int iParticle = 0; iParticle < nParticles; iParticle++)
  {

    float mc_pt  =       m_myBlock.mc_pt->at(iParticle)/1e3;
    float mc_eta =       m_myBlock.mc_eta->at(iParticle);
    float mc_phi =       m_myBlock.mc_phi->at(iParticle);
    float mc_m   =       m_myBlock.mc_m->at(iParticle)/1e3;
    int mc_status = 	   m_myBlock.mc_status->at(iParticle);
    int mc_pdgId =       m_myBlock.mc_pdgId->at(iParticle);
    int mc_barcode =     m_myBlock.mc_barcode->at(iParticle);
    int mc_vtx_barcode = m_myBlock.mc_vx_barcode->at(iParticle);
    float mc_charge =    m_myBlock.mc_charge->at(iParticle);

    // check that this particle comes eventually from a truth tau
    bool ancestor_is_tau = false;
    int nParents = m_myBlock.mc_parent_index->at(iParticle).size();
    for (int iParent = 0; iParent < nParents; iParent++)
    {
      int parent_index =  m_myBlock.mc_parent_index->at(iParticle).at(iParent);
      // if mother is a tau, then particle is good!
      if (abs(m_myBlock.mc_pdgId->at(parent_index)) == 15)
      {
        ancestor_is_tau = true;
        break;
      }
      // now check particles whose mother is a rho+- (pdgID = 213), or an a1+- (pdgId = 20213)
      bool is_done1 = false;
      if  (abs(m_myBlock.mc_pdgId->at(parent_index)) == 213 || abs(m_myBlock.mc_pdgId->at(parent_index)) == 20213)
      {
        int nGrandParents = m_myBlock.mc_parent_index->at(parent_index).size();
        for (int iGrandParent = 0; iGrandParent < nGrandParents; iGrandParent++)
        {
          int grandparent_index = m_myBlock.mc_parent_index->at(parent_index).at(iGrandParent);
          // if  mother of  rho+- / a1+- is a tau, then particle is good!
          if (abs(m_myBlock.mc_pdgId->at(grandparent_index)) == 15 )
          {
            is_done1 = true;
            // cout << "Particle " << iParticle << "  with pdgId " << m_myBlock.mc_pdgId->at(iParticle) <<  " comes from " <<  abs(m_myBlock.mc_pdgId->at(parent_index)) << " which comes from " << abs(m_myBlock.mc_pdgId->at(grandparent_index)) << endl;
            break;
          }
          // but a rho+- may come from a K1(1270)+-, which in turn comes from a tau!
          else if (abs(m_myBlock.mc_pdgId->at(grandparent_index)) == 10323)
          {
            int nGrandGrandParents =  m_myBlock.mc_parent_index->at(grandparent_index).size();
            for (int iGrandGrandParent = 0; iGrandGrandParent < nGrandGrandParents; iGrandGrandParent++)
            {
              int grandgrandparent_index =  m_myBlock.mc_parent_index->at(grandparent_index).at(iGrandParent);
              // if  mother of  K1(1270)+-  is a tau, then particle is good!
              if (abs(m_myBlock.mc_pdgId->at(grandgrandparent_index)) == 15)
              {
                is_done1 = true;
                // cout << "Particle " << iParticle << "  with pdgId " << m_myBlock.mc_pdgId->at(iParticle) <<  " comes from " <<  abs(m_myBlock.mc_pdgId->at(parent_index)) << " which comes from " <<  abs(m_myBlock.mc_pdgId->at(grandparent_index)) <<   " which comes from " << abs(m_myBlock.mc_pdgId->at(grandgrandparent_index)) << endl;
                break;
              }
            }
          }
        }
      }
      // now check particles whose mother is a K0 (pdgID = 311), or a K0S (pdgId = 310), or a  K*0(892)  (pdgId = 313), or a K*(892)(+-) (pdgId = 323), or a K1(1270)+- (pdgId = 10323), or a K1(1440)+- (pdgId = 20323)
      bool is_done2 = false;
      if  (abs(m_myBlock.mc_pdgId->at(parent_index)) == 310 ||
           abs(m_myBlock.mc_pdgId->at(parent_index)) == 311 ||
           abs(m_myBlock.mc_pdgId->at(parent_index)) == 313 ||
           abs(m_myBlock.mc_pdgId->at(parent_index)) == 323 ||
           abs(m_myBlock.mc_pdgId->at(parent_index)) == 10323 ||
           abs(m_myBlock.mc_pdgId->at(parent_index)) == 20323
          )
      {
        int nGrandParents =  m_myBlock.mc_parent_index->at(parent_index).size();
        for (int iGrandParent = 0; iGrandParent < nGrandParents; iGrandParent++)
        {
          int grandparent_index =  m_myBlock.mc_parent_index->at(parent_index).at(iGrandParent);
          // if mother of K0, K0S etc. is a tau, then particle is good!
          if (abs(m_myBlock.mc_pdgId->at(grandparent_index)) == 15)
          {
            is_done2 = true;
            // cout << "Particle " << iParticle << "  with pdgId " << ->at(iParticle) << " comes from " <<  abs(m_myBlock.mc_pdgId->at(parent_index)) << " which comes from " << abs(m_myBlock.mc_pdgId->at(grandparent_index)) << endl;
            break;
          }
          // but a K*0(892), K0 may come from a K(1400)+- (pdgId = 20323) / K1(1270)+- (pdgId = 10323), which in turn come from a tau!
          else if (abs(m_myBlock.mc_pdgId->at(grandparent_index)) == 20323 ||abs( m_myBlock.mc_pdgId->at(grandparent_index)) == 10323 )
          {
            int nGrandGrandParents = m_myBlock.mc_parent_index->at(grandparent_index).size();
            for (int iGrandGrandParent = 0; iGrandGrandParent < nGrandGrandParents; iGrandGrandParent++)
            {
              int grandgrandparent_index = m_myBlock.mc_parent_index->at(grandparent_index).at(iGrandParent);
              // if  mother of  K(1400)+-, K1(1270)+-  is a tau, then particle is good!
              if (abs(m_myBlock.mc_pdgId->at(grandgrandparent_index)) == 15)
              {
                is_done2 = true;
                // cout << "Particle " << iParticle <<  "  with pdgId " <<m_myBlock.mc_pdgId->at(iParticle) << " comes from " <<  abs(m_myBlock.mc_pdgId->at(parent_index)) << " which comes from " << abs(m_myBlock.mc_pdgId->at(grandparent_index)) << " which comes from " << abs(m_myBlock.mc_pdgId->at(grandgrandparent_index)) << endl;
                break;
              }
            }
          }
        }
      }
      if(is_done1 || is_done2)
      {
        ancestor_is_tau = true;
        break;
      }
    }

    /////////////////////
    // TRUTH SELECTION //
    /////////////////////

    // if it's not tau's descendant, skip
    if (!ancestor_is_tau) continue;
    // if it is not a (charged pion || K+-) , skip
    if (abs(mc_pdgId) != 211 && abs(mc_pdgId) != 321) continue;
    // check that this particle is good tau daughter (i.e., reject bad (duplicated) children)
    if (!isGoodDaughter(mc_status, mc_barcode, mc_vtx_barcode)) continue;

    /////////////////////
    TruthParticle* mc_particle = new TruthParticle(); // deleted below
    mc_particle->SetPtEtaPhiM(mc_pt, mc_eta, mc_phi, mc_m);

    // Now classify each spurious track based on barcode and probability
    if ( track->Barcode() == mc_barcode &&
         ( !m_use_delta_r_matching || (track->DeltaR(*mc_particle) < m_delta_r_cut) )
       )
    {
      track->SetIsTruthMatched(true);
      /*
        cout << "Track truth index " << track->TruthTrackIndex() << endl;
      */
      mc_particle->SetStatus(mc_status);
      mc_particle->SetBarcode(mc_barcode);
      mc_particle->SetPdgId(mc_pdgId);
      mc_particle->SetVertexBarcode(mc_vtx_barcode);
      mc_particle->SetCharge(mc_charge);

      track->SetTruthTrackIndex(iParticle);
      track->m_truthpartner = mc_particle;
      /*
              cout  << " ----------  TRUTH PARTNER OF this TRACK has : " << "\n"
              << "    pT: "  << track->m_truthpartner->Pt()  << " [GeV]"     << "\n"
              << "    eta: " << track->m_truthpartner->Eta()		     << "\n"
              << "    phi: " << track->m_truthpartner->Phi()		     << "\n"
              << "    ---------- "                                      << "\n"
              << "    barcode"  << track->m_truthpartner->Barcode()	     << "\n"
              << "	DeltaR(track): " << track->DeltaRTruthPartner()   << endl;
              cout << " " << endl;
      */
      matchCounter++;
      //break;
    }
    else
    {
      delete mc_particle;
      mc_particle = NULL;
    }
  } // close loop over truth particles

  track->SetHowManyMatchingTruthTracks(matchCounter);
  track->SetTrackMatchingStatus();
}

// 2) VERSION 2 - pass as input a vector of truthtracks to find the barcode-matching truth track for input track
// useful if you start from a tau as input
void TruthTrackMatchingTool::TruthTrackMatching(vector<TruthParticle*> &truthtracks, Track *track)
{

  // loops on "truthtracks" vector.
  int nTruthTracks = truthtracks.size();

  int matchCounter = 0;  // when you find a truth-match to THIS track, increment it. In principle it should become 1 for good tau tracks, 0 for spurious
  for(int iTruthTrack = 0; iTruthTrack < nTruthTracks; iTruthTrack++)
  {

    track->SetTruthTrackIndex(truthtracks[iTruthTrack]->DaughterIndex());

    if ( track->Barcode() == truthtracks[iTruthTrack]->Barcode() &&
         ( !m_use_delta_r_matching || (track->DeltaR(*(truthtracks[iTruthTrack])) < m_delta_r_cut) )
       )
    {
      track->SetIsTruthMatched(true);
      track->m_truthpartner = new TruthParticle( *(truthtracks[iTruthTrack]) );
      matchCounter++;
      //break;
    }
  } // close loop over truth tracks

  track->SetHowManyMatchingTruthTracks(matchCounter);
  track->SetTrackMatchingStatus();
}

//*****************************************

void TruthTrackMatchingTool::TruthTauMatching(Tau *tau)  //Find corresponding truth tau in order to fill vector<TruthParticle*> m_truthtracks for myTau
{

  // fill a vector of truth taus of this event
  vector<TruthParticle*> truthtaus;

  int nParticles = m_myBlock.mc_pdgId->size();
  for (int iParticle = 0; iParticle < nParticles; iParticle++)
  {

    int mc_pdgId   = m_myBlock.mc_pdgId->at(iParticle);
    if (abs(mc_pdgId) == 15)
    {

      TruthParticle *this_truthtau = new TruthParticle();
      this_truthtau->SetPdgId(mc_pdgId);
      this_truthtau->SetBarcode(m_myBlock.mc_barcode->at(iParticle));
      this_truthtau->SetPtEtaPhiM(m_myBlock.mc_pt ->at(iParticle)/1e3,
                                  m_myBlock.mc_eta->at(iParticle),
                                  m_myBlock.mc_phi->at(iParticle),
                                  m_myBlock.mc_m  ->at(iParticle)/1e3);
      this_truthtau->SetCharge(m_myBlock.mc_charge->at(iParticle));

      vector<int> child_indexes = m_myBlock.mc_child_index->at(iParticle);
      //	this_truthtau->FindChargedDaughters(child_indexes);

      FindChargedDaughters(this_truthtau, child_indexes);

      // keep only truth taus with 1, 3 or > 3 prong
      if (this_truthtau->nChargedDaughters() == 1 || this_truthtau->nChargedDaughters() == 3 || this_truthtau->nChargedDaughters() > 3)
      {
        /*
        cout  << " ---------- THIS TRUTH TAU has : " << "\n"
        << "    pT: "  << this_truthtau->Pt()  << " [GeV]"     << "\n"
        << "    eta: " << this_truthtau->Eta()                 << "\n"
        << "    phi: " << this_truthtau->Phi()                 << "\n"
        << "    ---------- "                          << "\n"
        << " charged daughters: "  << this_truthtau->m_charged_daughters.size() << endl;
              */
        truthtaus.push_back(this_truthtau);
      }
      else
      {
        delete this_truthtau;
        this_truthtau = NULL;
      }
    }
  }

  // now loop over this vector and find the matching truth tau to input tau
  float min_deltaR = 0.2;
  int idxMatchingCandidate = -1; // in truthtaus vector
  for (unsigned int iTruthTau = 0; iTruthTau < truthtaus.size(); iTruthTau++)
  {

    float deltaR = tau->DeltaR(*truthtaus[iTruthTau]);
    if (deltaR < min_deltaR)
    {
      min_deltaR = deltaR;
      idxMatchingCandidate = iTruthTau;
    }
  }
  // set truth properties of tau from its matched truth partner
  if (idxMatchingCandidate != -1)
  {
    tau->SetTauMatchStatus(kTAUMATCHSTATUS_TRUTH_MATCHED);
    tau->SetIsTruthMatched(true);
    // set tau truth-partner
    tau->m_truthpartner = new TruthParticle(*(truthtaus[idxMatchingCandidate])); // a copy is needed since I must delete all the *truthtaus* below
    // fill vector of truth tracks of tau with good charged daughters of matched truth tau
    int nDgts = truthtaus[idxMatchingCandidate]->m_charged_daughters.size();
    for (int iDgt = 0; iDgt < nDgts; iDgt++)
    {
      TruthParticle *this_truthtrack = new TruthParticle(*(truthtaus[idxMatchingCandidate]->m_charged_daughters[iDgt])); // a copy is needed since I must delete all the *truthtaus* below
      tau->m_truthtracks.push_back(this_truthtrack);
    }
    if (nDgts == 1)      tau->SetTauType(kTAUTYPE_1PRONG);
    else if (nDgts == 3) tau->SetTauType(kTAUTYPE_3PRONG);
    else if (nDgts > 3 ) tau->SetTauType(kTAUTYPE_GT3PRONG);
  }

  // delete all allocated pointers inside *truthtaus* vector
  for (unsigned int iTruthTau = 0; iTruthTau < truthtaus.size(); iTruthTau++)
  {
    delete truthtaus[iTruthTau];
    truthtaus[iTruthTau] = NULL;
  }

}

void TruthTrackMatchingTool::TauTrkMatching(Tau *tau)   // calls TrkMatching for each tau track, and fills vector<Track*> m_tracks for myTau
{

  // loop over input tau's tau_tracks, and fill vector<Track*> m_tracks
  int nTauTracks = tau->m_tautracks.size();
  for (int iTauTrack = 0; iTauTrack < nTauTracks; iTauTrack++)
  {

    Track *this_track  = MakeTrk(tau->m_tautracks[iTauTrack]);
    tau->m_tracks.push_back(this_track);
  }
}

void TruthTrackMatchingTool::TauTruthTrackMatching(Tau *tau)   // calls TruthTrackMatching for each track of m_tracks
{

  int nTracks      = tau->m_tracks.size();
  for(int iTrack = 0; iTrack < nTracks; iTrack++)
  {
    // USES VERSION 1 OF TruthTrackMatching()
    //      TruthTrackMatching(tau->m_tracks[iTrack]);
    // USES VERSION 2 OF TruthTrackMatching() - this might be wrong because presently in tau->m_truthtracks you don't have pions from cascade decay of K0s
    //if want to use Tau truth tracks, look only at taus that are truth-matched to a 1-3 >3 prong tau
    if (tau->isTruthMatched() &&
        (tau->WhichTauType() == kTAUTYPE_1PRONG || tau->WhichTauType() == kTAUTYPE_3PRONG || tau->WhichTauType() == kTAUTYPE_GT3PRONG )
       ) TruthTrackMatching(tau->m_truthtracks, tau->m_tracks[iTrack]); // use VERSION 2 of TruthTrackMatching()
  }
}

void TruthTrackMatchingTool::FindChargedDaughters (TruthParticle *particle, vector<int> &idxs)
{

  int nDaughters = idxs.size();
  for (int iDgt = 0; iDgt < nDaughters; iDgt++)
  {

    TruthParticle *this_daughter = new TruthParticle();
    int daughter_index    = idxs.at(iDgt);
    this_daughter->SetStatus       (m_myBlock.mc_status->at(daughter_index));
    this_daughter->SetBarcode      (m_myBlock.mc_barcode->at(daughter_index));
    this_daughter->SetVertexBarcode(m_myBlock.mc_vx_barcode->at(daughter_index));
    int pdgid = m_myBlock.mc_pdgId->at(daughter_index);

    // Skip leptonic tau decays
    if (abs(pdgid) == 11 || abs(pdgid) == 13 || abs(pdgid) == 15)
    {
      delete this_daughter;
      this_daughter = NULL;
      break;
    }
    // Don't include neutrinos in visible decay
    if (abs(pdgid) == 12 || abs(pdgid) == 14 || abs(pdgid) == 16)
    {
      delete this_daughter;
      this_daughter = NULL;
      continue;
    }
    if (TruthTrackMatchingTool::isGoodDaughter(this_daughter->Status(), this_daughter->Barcode(), this_daughter->VertexBarcode()))
    {
      this_daughter->SetDaughterIndex(daughter_index);
      this_daughter->SetCharge       (m_myBlock.mc_charge->at(daughter_index) );
      this_daughter->SetPdgId        ( pdgid );
      this_daughter->SetPtEtaPhiM    (m_myBlock.mc_pt->at(daughter_index)/1e3,
                                      m_myBlock.mc_eta->at(daughter_index),
                                      m_myBlock.mc_phi->at(daughter_index),
                                      m_myBlock.mc_m->at(daughter_index)/1e3);
      //include only charged particles
      if (this_daughter->Charge() == 0)
      {
        delete this_daughter;
        this_daughter = NULL;   // caution: might not count K0s ... check this!
        continue;
      }

      particle->m_charged_daughters.push_back(this_daughter);

    }
    else
    {
      delete this_daughter;
      this_daughter = NULL;
    }
  }
  /*
    if(particle->m_charged_daughters.size() > 0){
    cout  << " ----------  TRUTH TAU has : " << "\n"
    << "    pT: "  << particle->Pt()  << " [GeV]"     << "\n"
    << "    eta: " << particle->Eta()                 << "\n"
    << "    phi: " << particle->Phi()                 << "\n"
    << "    ---------- "                          << "\n"
    << " charged daughters: "  << particle->m_charged_daughters.size() << endl;
    }
  */
}






// ***************************************************** //

// object classes //

// ***************************************************** //

//Track copy constructor
Track::Track (const Track& copyTrk) : TLorentzVector(copyTrk)
{

  m_barcode             = copyTrk.m_barcode;
  m_probability         = copyTrk.m_probability;
  m_tau_track_index     = copyTrk.m_tau_track_index;
  m_truth_track_index   = copyTrk.m_truth_track_index;
  m_charge              = copyTrk.m_charge;
  m_track_match_status  = copyTrk.m_track_match_status;
  m_is_truth_matched    = copyTrk.m_is_truth_matched;
  m_track_spurious_type = copyTrk.m_track_spurious_type;
  m_n_matching_truth    = copyTrk.m_n_matching_truth;

  m_truthpartner        = copyTrk.m_truthpartner;
}

void Track::SetTrackMatchingStatus()
{
  if(this->m_is_truth_matched)
  {

    // check if there are more than 1 truth track barcode-matching with THIS track
    if      (this->HowManyMatchingTruthTracks() == 1) this->SetTrackMatchStatus(TruthTrackMatchingTool::kTRACKMATCHSTATUS_TRUTH_MATCHED);
    else if (this->HowManyMatchingTruthTracks() > 1 ) this->SetTrackMatchStatus(TruthTrackMatchingTool::kTRACKMATCHSTATUS_MULTIMATCHED);
  }
  else
  {

    this->SetTrackMatchStatus(TruthTrackMatchingTool::kTRACKMATCHSTATUS_TRUTH_UNMATCHED);
    // Now classify each spurious track based on barcode and probability
    // protect against overcounting of fake tracks
    if (this->Probability() >= 0.5)
    {
      if      (this->Barcode() >= 10000 && this->Barcode() < 200000)
      {
        this->SetTrackSpuriousType(TruthTrackMatchingTool::kTRACKSPURIOUSTYPE_PILEUP);
        TruthTrackMatchingTool::s_counters_tracktype[TruthTrackMatchingTool::kTRACKSPURIOUSTYPE_PILEUP]++;
      }
      else if (this->Barcode() >= 200000)
      {
        this->SetTrackSpuriousType(TruthTrackMatchingTool::kTRACKSPURIOUSTYPE_CONVERSION);
        TruthTrackMatchingTool::s_counters_tracktype[TruthTrackMatchingTool::kTRACKSPURIOUSTYPE_CONVERSION]++;
      }
      else if (this->Barcode() > 0	&& this->Barcode() < 10000)
      {
        this->SetTrackSpuriousType(TruthTrackMatchingTool::kTRACKSPURIOUSTYPE_UE);
        TruthTrackMatchingTool::s_counters_tracktype[TruthTrackMatchingTool::kTRACKSPURIOUSTYPE_UE]++;
      }
      else if (this->Barcode() == 0)
      {
        this->SetTrackSpuriousType(TruthTrackMatchingTool::kTRACKSPURIOUSTYPE_FAKE);
        TruthTrackMatchingTool::s_counters_tracktype[TruthTrackMatchingTool::kTRACKSPURIOUSTYPE_FAKE]++;
      }
    }
    else
    {
      if (this->Barcode() < 200000)
      {
        this->SetTrackSpuriousType(TruthTrackMatchingTool::kTRACKSPURIOUSTYPE_FAKE);
        TruthTrackMatchingTool::s_counters_tracktype[TruthTrackMatchingTool::kTRACKSPURIOUSTYPE_FAKE]++;
      }
      else
      {
        this->SetTrackSpuriousType(TruthTrackMatchingTool::kTRACKSPURIOUSTYPE_CONVERSION);
        TruthTrackMatchingTool::s_counters_tracktype[TruthTrackMatchingTool::kTRACKSPURIOUSTYPE_CONVERSION]++;
      }
    }
  }
}

float Track::DeltaRTruthPartner()
{
  float dr = -1.;
  if (this->m_truthpartner) dr = this->DeltaR(*(this->m_truthpartner));
  return dr;
}

// Track destructor
Track::~Track()
{
  if(m_truthpartner) delete m_truthpartner;
  m_truthpartner = NULL;
}


//TruthParticle copy constructor
TruthParticle::TruthParticle (const TruthParticle& copyTruthParticle) : TLorentzVector(copyTruthParticle)
{

  m_status         = copyTruthParticle.m_status;
  m_barcode        = copyTruthParticle.m_barcode;
  m_vertex_barcode = copyTruthParticle.m_vertex_barcode;
  m_daughter_index = copyTruthParticle.m_daughter_index;
  m_charge         = copyTruthParticle.m_charge;
  m_pdgid          = copyTruthParticle.m_pdgid;

  m_charged_daughters.reserve(copyTruthParticle.m_charged_daughters.size());
  for(unsigned int iDgt = 0; iDgt < copyTruthParticle.m_charged_daughters.size(); iDgt++)
  {
    TruthParticle* copy_of_particle =  new TruthParticle(*copyTruthParticle.m_charged_daughters[iDgt]);
    m_charged_daughters.push_back(copy_of_particle);
  }
}

// TruthParticle destructor
TruthParticle::~TruthParticle()
{

  for (unsigned int iDgt = 0; iDgt < m_charged_daughters.size(); iDgt++)
  {
    if (m_charged_daughters[iDgt])
    {
      delete m_charged_daughters[iDgt];
      m_charged_daughters[iDgt] = NULL;
    }
  }
  m_charged_daughters.clear();
}

//Tau copy constructor
Tau::Tau (const Tau& copyTau) : TLorentzVector(copyTau)
{

  m_charge                   = copyTau.m_charge;
  m_tau_index                = copyTau.m_tau_index;
  m_matched_truth_tau_index  = copyTau.m_matched_truth_tau_index;
  m_tau_match_status         = copyTau.m_tau_match_status;
  m_is_truth_matched         = copyTau.m_is_truth_matched;
  m_tau_type                 = copyTau.m_tau_type;

  m_tracks.reserve(copyTau.m_tracks.size());
  for(unsigned int iTrk = 0; iTrk < copyTau.m_tracks.size(); iTrk++)
  {
    Track* copy_of_track = new Track(*copyTau.m_tracks[iTrk]);
    m_tracks.push_back(copy_of_track);
  }

  m_tautracks.reserve(copyTau.m_tautracks.size());
  for(unsigned int iTrk = 0; iTrk < copyTau.m_tautracks.size(); iTrk++)
  {
    Track* copy_of_tautrack = new Track(*copyTau.m_tautracks[iTrk]);
    m_tautracks.push_back(copy_of_tautrack);
  }

  m_truthtracks.reserve(copyTau.m_truthtracks.size());
  for(unsigned int iTrk = 0; iTrk < copyTau.m_truthtracks.size(); iTrk++)
  {
    TruthParticle* copy_of_truthtrack = new TruthParticle(*copyTau.m_truthtracks[iTrk]);
    m_truthtracks.push_back(copy_of_truthtrack);
  }

  m_truthpartner             = copyTau.m_truthpartner;
}

// Tau destructor
Tau::~Tau()
{

  for (unsigned int iTrk = 0; iTrk < m_tracks.size(); iTrk++)
  {
    if(m_tracks[iTrk])
    {
      delete m_tracks[iTrk];
      m_tracks[iTrk] = NULL;
    }
  }
  m_tracks.clear();

  for (unsigned int iTrk = 0; iTrk < m_tautracks.size(); iTrk++)
  {
    if(m_tautracks[iTrk])
    {
      delete m_tautracks[iTrk];
      m_tautracks[iTrk] = NULL;
    }
  }
  m_tautracks.clear();

  for (unsigned int iTrk = 0; iTrk < m_truthtracks.size(); iTrk++)
  {
    if(m_truthtracks[iTrk])
    {
      delete m_truthtracks[iTrk];
      m_truthtracks[iTrk] = NULL;
    }
  }
  m_truthtracks.clear();

  if(m_truthpartner) delete m_truthpartner;
  m_truthpartner = NULL;
}

#endif // DEVEL_TAUTRUTHTRACKMATCHINGTOOL
