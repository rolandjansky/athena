/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGElectronAmbiguityTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// 
//

#include "DerivationFrameworkEGamma/EGElectronAmbiguityTool.h"

#include "xAODTruth/xAODTruthHelpers.h"
#include "xAODEgamma/ElectronxAODHelpers.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "AthContainers/ConstDataVector.h"

#include "TMath.h"

namespace {
  SG::AuxElement::Decorator< float > drv("DFCommonSimpleConvRadius");
  SG::AuxElement::Decorator< float > dphiv("DFCommonSimpleConvPhi");
  SG::AuxElement::Decorator< float > dmee("DFCommonSimpleMee");
  SG::AuxElement::Decorator< float > dmeeVtx("DFCommonSimpleMeeAtVtx");
  SG::AuxElement::Decorator< float > dsep("DFCommonSimpleSeparation");
  SG::AuxElement::Decorator< int > dambi("DFCommonAddAmbiguity");

  SG::AuxElement::Decorator< float > dtrv("DFCommonProdTrueRadius");
  SG::AuxElement::Decorator< float > dtpv("DFCommonProdTruePhi");
  SG::AuxElement::Decorator< float > dtzv("DFCommonProdTrueZ");
} //> end anonymous namespace

namespace DerivationFramework {

  EGElectronAmbiguityTool::EGElectronAmbiguityTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ContainerName",    m_containerName    = "Electrons");
    declareProperty("VtxContainerName", m_VtxContainerName = "PrimaryVertices");
    declareProperty("tpContainerName",  m_tpContainerName  = "InDetTrackParticles");

    declareProperty("isMC", m_isMC);

    declareProperty("pTCut",            m_elepTCut = 9e3, "minimum pT for an electron to be studied");
    declareProperty("idCut",            m_idCut    = "DFCommonElectronsLHLoose", "minimal quality for an electron to be studied");

    declareProperty("nSiCut",           m_nSiCut  = 7,    "minimum number of Si hits in the other track");
    declareProperty("dzsinTCut",        m_dzCut   = 0.5,  "max dz sinTheta between ele and other tracks");
    declareProperty("SeparationCut",    m_sepCut  = 1.,   "first separation cut");
    declareProperty("DCTCut",           m_dctCut  = 0.02, "second separation cut");

    declareProperty("radiusCut",        m_rvECCut       = 20,  "minimum radius to be classified as external conversion");
    declareProperty("meeAtVtxCut",      m_meeAtVtxECCut = 100, "maximal mass at vertex to be classified as external conversion");
    declareProperty("meeCut",           m_meeICCut      = 100, "maximal mass at primary vertex to be classified as gamma*");

  }

  StatusCode EGElectronAmbiguityTool::addBranches() const
  {

    static SG::AuxElement::ConstAccessor<char> aidCut(m_idCut);

    // retrieve primary vertex
    const xAOD::Vertex *pvtx(nullptr);
    const xAOD::VertexContainer *vtxC(nullptr);
    if (evtStore()->retrieve(vtxC,m_VtxContainerName).isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve vertex container. Exiting." );
      return StatusCode::FAILURE;
    }
    for (auto vertex: *vtxC) {
      if (vertex->vertexType() == xAOD::VxType::VertexType::PriVtx) {
        pvtx = vertex;
        break;
      }
    }


    // retrieve electron container
    const xAOD::ElectronContainer* eleC = evtStore()->retrieve< const xAOD::ElectronContainer >( m_containerName );
    
    if( ! eleC ) {
      ATH_MSG_ERROR ("Couldn't retrieve Electron container with key: " << m_containerName );
      return StatusCode::FAILURE;
    }
    if (!pvtx) {
      ATH_MSG_DEBUG("No primary vertex found. Setting default values.");
      for (const xAOD::Electron* iele : *eleC) {
        drv(*iele) = -1;
        dphiv(*iele) = -1;
        dmee(*iele) = -1;
        dmeeVtx(*iele) = -1;
        dsep(*iele) = -1;
        dambi(*iele) = -1;
        dtrv(*iele) = -1;
        dtpv(*iele) = -1;
        dtzv(*iele) = -1;
      }
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Pvx z = " << pvtx->z() << ", number of electrons " << eleC->size());

    // Make a container of selected tracks : with Si hits, close to electron track 
    const xAOD::TrackParticleContainer* idtpC(nullptr);
    if (evtStore()->retrieve(idtpC,"InDetTrackParticles").isFailure()) {
      ATH_MSG_ERROR("No InDetTrackParticles container found");
      return StatusCode::FAILURE;
    }
    std::set<const xAOD::TrackParticle*> alreadyStored;
    std::set<const xAOD::TrackParticle*> eleIDtpStored, eleGSFtpStored;
    auto closeByTracks = std::make_unique<ConstDataVector<xAOD::TrackParticleContainer>>(SG::VIEW_ELEMENTS);

    for (auto ele : *eleC) {

      dambi(*ele) = -1;

      // Electron preselection
      if (ele->pt() < m_elepTCut || !(m_idCut.size() && aidCut.isAvailable(*ele) && aidCut(*ele)))
        continue;

      // Just for debug
      const xAOD::TrackParticle *eleGSFtp = ele->trackParticle();
      if (eleGSFtpStored.find(eleGSFtp) == eleGSFtpStored.end())
        eleGSFtpStored.insert(eleGSFtp);

      const xAOD::TrackParticle *eleIDtp = xAOD::EgammaHelpers::getOriginalTrackParticle(ele);
      if (eleIDtpStored.find(eleIDtp) == eleIDtpStored.end())
        eleIDtpStored.insert(eleIDtp);

      // The loop on track
      for (auto tp : *idtpC) {

        // Keep the electron track (I build a container to run vertexing on it...)
        if (tp == eleIDtp) {
          closeByTracks->push_back(tp);
          alreadyStored.insert(tp);
          continue;
        }

        // potential candidate to store if not already there
        if (alreadyStored.find(tp) != alreadyStored.end())
          continue;

        //// Keep only opposite charge
        //if (tp->charge() * ele->charge() > 0)
        //  continue;

        // Close-by
        double dR = eleIDtp->p4().DeltaR(tp->p4());
        double dz = fabs(eleIDtp->z0()-tp->z0())*sin(eleIDtp->theta());
        if (!(dR < 0.3 && dz < m_dzCut))
          continue;

        // With minimum number of Si hits
        if (xAOD::EgammaHelpers::numberOfSiHits(tp) < m_nSiCut)
          continue;

        alreadyStored.insert(tp);

        closeByTracks->push_back(tp); 
      }
    }

    if (closeByTracks->size() == 0)
      return StatusCode::SUCCESS;

    // Record the objects into the event store
    if (evtStore()->record ( closeByTracks.release(), "closeByTrackParticles").isFailure()) {
      ATH_MSG_ERROR("Could not record the skimmed track particle container");
      return StatusCode::FAILURE;
    }

    //
    const xAOD::TrackParticleContainer* idSeltpC(nullptr);
    if (msgLvl(MSG::DEBUG)) {
      if (evtStore()->retrieve(idSeltpC,"closeByTrackParticles").isFailure()) {
        ATH_MSG_ERROR("No closeByTrackParticles container found");
        return StatusCode::FAILURE;
      }
      const xAOD::TrackParticleContainer* tpC(nullptr);
      if (evtStore()->retrieve(tpC,"GSFTrackParticles").isFailure()) {
        ATH_MSG_ERROR("No TrackParticleInputContainer found");
        return StatusCode::FAILURE;
      }

      ATH_MSG_DEBUG("Number of input tracks " << idtpC->size() << " , number of selected close-by tracks " << idSeltpC->size() << " , number of GSF tracks " << tpC->size());
      for (auto trk : eleIDtpStored)
        ATH_MSG_DEBUG("ele  ID trk " << trk << " pt = " << trk->pt()*1e-3 << " eta = " << trk->eta() << " phi = " << trk->phi() << " nSi = " << xAOD::EgammaHelpers::numberOfSiHits(trk));
      for (auto trk : eleGSFtpStored)
        ATH_MSG_DEBUG("ele GSF trk " << trk << " pt = " << trk->pt()*1e-3 << " eta = " << trk->eta() << " phi = " << trk->phi() << " nSi = " << xAOD::EgammaHelpers::numberOfSiHits(trk));
      for (auto trk : *idSeltpC)
        ATH_MSG_DEBUG("closeby trk " << trk << " pt = " << trk->pt()*1e-3 << " eta = " << trk->eta() << " phi = " << trk->phi() << " nSi = " << xAOD::EgammaHelpers::numberOfSiHits(trk));
    }

    for (auto ele : *eleC) {

      // Electron preselection
      if (ele->pt() < m_elepTCut || !(m_idCut.size() && aidCut.isAvailable(*ele) && aidCut(*ele)))
        continue;      

      // Henri's circles
      if (decorateSimple(ele,pvtx).isFailure()) {
        ATH_MSG_ERROR("Cannot decorate the electron with the simple info");
        return StatusCode::FAILURE;
      }

    }      // loop on electrons to decorate

    return StatusCode::SUCCESS;
  }
}

StatusCode DerivationFramework::EGElectronAmbiguityTool::decorateSimple(const xAOD::Electron *ele, const xAOD::Vertex *pvtx) const {


  // This is the GSF electron track
  const xAOD::TrackParticle *eleGSFtrkP = ele->trackParticle();

  // And the ID one
  const xAOD::TrackParticle *eleIDtrkP = xAOD::EgammaHelpers::getOriginalTrackParticle(ele);

  // For the time being, use the ID one, to be consistent when we only find a good ID to make a conversion and no GSF
  bool useGSF = false; // hardcoded because it seems we will not use true. Kept for the time being, as not 100% sure
  const xAOD::TrackParticle *eletrkP = useGSF ? eleGSFtrkP : eleIDtrkP;

  ATH_MSG_DEBUG("Electron pt = " << ele->pt()*1e-3 << " eta = " << ele->eta() << " phi = " << ele->phi() << " GSF trk ptr = " << eleGSFtrkP << " ID trk ptr " << eleIDtrkP);

  if (m_isMC) {
    const xAOD::TruthParticle *truthEl = xAOD::TruthHelpers::getTruthParticle(*ele);
    double tpvr = -1, tpvp = 9e9, tpvz = 9e9;
    if (truthEl && fabs(truthEl->pdgId()) == 11 && truthEl->prodVtx() != nullptr) {
      tpvr = truthEl->prodVtx()->perp();
      tpvp = truthEl->prodVtx()->phi();
      tpvz = truthEl->prodVtx()->z();
    }
    dtrv(*ele) = tpvr;
    dtpv(*ele) = tpvp;
    dtzv(*ele) = tpvz;
  }

  // Find the closest track particle with opposite charge and a minimum nb of Si hits
  const xAOD::TrackParticleContainer *tpC(nullptr);
  if (evtStore()->retrieve(tpC,"closeByTrackParticles").isFailure()) {
    ATH_MSG_ERROR ("Failed to retrieve track particle container. Exiting." );
    return StatusCode::FAILURE;
  }
  const xAOD::TrackParticle *otrkP(nullptr);
  double detaMin = 9e9;
  for (auto tp : *tpC) {
    // Keep only opposite charge
    if (tp->charge() * eletrkP->charge() > 0)
      continue;

    // Close-by
    double dR = eletrkP->p4().DeltaR(tp->p4());
    double dz = fabs(eletrkP->z0()-tp->z0())*sin(eletrkP->theta());
    if (!(dR < 0.3 && dz < m_dzCut))
      continue;

    double deta = fabs(eletrkP->eta() - tp->eta());
    if (deta < detaMin) {
      otrkP   = tp;
      detaMin = deta;
    }
  }

  double rv = -9e9;
  double pv = -9e9;
  double mee = -1.;
  double meeAtVtx = -1.;
  double sep = -9e9;
  bool goodConv = false;

  if (otrkP) {

    // To be consistent with the other, use the ID track.
    TLorentzVector ep4; ep4.SetPtEtaPhiM(eletrkP->pt(),eletrkP->eta(),eletrkP->phi(),0.511);

    // Maybe could see if a GSF tp exists for this ID tp and use it if yes ?
    TLorentzVector op4; op4.SetPtEtaPhiM(otrkP->pt(),otrkP->eta(),otrkP->phi(),0.511);

    // Simple masses
    mee = (ep4+op4).M();
    op4.SetPhi(eletrkP->phi());
    meeAtVtx = (ep4+op4).M();

    // And the conversion point
    std::vector<double> helix1, helix2;
    helix1.resize(5);
    helix2.resize(5);
    helix(eletrkP,pvtx,helix1);
    helix(otrkP,pvtx,helix2);

    double beta(0.);
    if (helix1[4] < helix2[4])
      beta = TMath::PiOver2()-helix1[4];
    else
      beta = TMath::PiOver2()-helix2[4];

    double phi1(helix1[4] + beta);
    if (phi1 > TMath::TwoPi()) phi1 -= TMath::TwoPi();
    if (phi1 < 0.)             phi1 += TMath::TwoPi();

    double phi2(helix2[4] + beta);
    if (phi2 > TMath::TwoPi()) phi2 -= TMath::TwoPi();
    if (phi2 < 0.)             phi2 += TMath::TwoPi();

    /// HelixToCircle Main Track Electron
    double r1 = 1/(2.*fabs(helix1[1]));

    double charge1(1.);
    if (helix1[1]<0.) charge1 = -1.;
    double rcenter1(helix1[3]/charge1 + r1);
    double phicenter1(phi1 + TMath::PiOver2()*charge1);

    double x1 = rcenter1*cos(phicenter1);
    double y1 = rcenter1*sin(phicenter1);

    /// HelixToCircle Other Electron Conv Track
    double r2 = 1/(2.*fabs(helix2[1]));

    double charge2(1.);
    if(helix2[1]<0.) charge2 = -1.;
    double rcenter2(helix2[3]/charge2 + r2);
    double phicenter2(phi2 + TMath::PiOver2()*charge2);

    double x2 = rcenter2*cos(phicenter2);
    double y2 = rcenter2*sin(phicenter2);
    //////

    double dx(x1-x2);
    if (dx <  1e-9 && dx > 0.) dx =  1e-9;
    if (dx > -1e-9 && dx < 0.) dx = -1e-9;
    double slope((y1-y2)/dx);
    double b(y1 - slope*x1);
    double alpha(atan(slope));
    double d(sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));
    //only keeping opposite sign option
    double separation = d-r1-r2;
    double cpx1, cpx2;
    if (x1 > x2) {
      cpx1 = x1-r1*cos(alpha);
      cpx2 = x2+r2*cos(alpha); 
    } else {
      cpx1 = x1+r1*cos(alpha);
      cpx2 = x2 - r2*cos(alpha);
    }

    double temp1 = (cpx1+cpx2)/2;
    double temp2 = slope*temp1+b;
    double convX = cos(beta)*temp1 + sin(beta)*temp2;
    double convY = -sin(beta)*temp1+ cos(beta)*temp2;

    double dct(helix1[0]-helix2[0]);

    ///////
    if (fabs(separation) < m_sepCut && fabs(dct) < m_dctCut){
      goodConv = true;
      sep = separation;
      pv  = TMath::ATan2(convY,convX);
      rv  = sqrt(convX*convX + convY*convY);
      if (convX*cos(eletrkP->phi()) + convY*sin(eletrkP->phi()) < 0) rv *= -1.; 
    }
  } else {
    dambi(*ele) = -1; 
  }
  drv(*ele)     = rv;
  dphiv(*ele)   = pv;
  dmee(*ele)    = mee;
  dmeeVtx(*ele) = meeAtVtx;
  dsep(*ele)    = sep;
  if (goodConv && rv > m_rvECCut && meeAtVtx < m_meeAtVtxECCut)
    dambi(*ele) = 2;
  else if (otrkP) {
    if (mee < m_meeICCut)
      dambi(*ele) = 1;
    else
      dambi(*ele) = 0;
  }
  return StatusCode::SUCCESS;
}

void DerivationFramework::EGElectronAmbiguityTool::helix(const xAOD::TrackParticle *trkP, const xAOD::Vertex *pvtx, std::vector<double>&he) const {

  static const double PTTOCURVATURE = -0.301;

  he[0] = 1./tan(trkP->theta());
  he[1] = PTTOCURVATURE*trkP->charge()/trkP->pt();

  if (trkP->phi0() > 0.)
    he[4] = trkP->phi0();
  else
    he[4] = TMath::TwoPi() + trkP->phi0();

  double c1 = cos(trkP->phi0());
  double s1 = sin(trkP->phi0());
  he[3] = trkP->d0() + c1*pvtx->y() - s1*pvtx->x(); 

  c1 *= he[0];
  s1 *= he[0];
  he[2] = trkP->z0() - c1*pvtx->x() - s1*pvtx->y() + pvtx->z();
}

