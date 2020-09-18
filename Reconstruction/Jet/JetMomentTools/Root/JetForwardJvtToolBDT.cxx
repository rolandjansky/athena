///////////////////////// -*- C++ -*- /////////////////////////////
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
// JetForwardJvtToolBDT.cxx
// Implementation file for class JetForwardJvtToolBDT
// Author: Louis Portales <louis.portales@cern.ch>
///////////////////////////////////////////////////////////////////

// JetForwardJvtToolBDT includes
#include "JetMomentTools/JetForwardJvtToolBDT.h"
// Jet EDM
#include "xAODJet/JetAttributes.h"
#include "xAODMetaData/FileMetaData.h"
#include "PathResolver/PathResolver.h"


const double GeV = 1000.;
///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
JetForwardJvtToolBDT::JetForwardJvtToolBDT(const std::string& name) :
  asg::AsgTool(name)
{
}

// Destructor
///////////////
JetForwardJvtToolBDT::~JetForwardJvtToolBDT(){}

// Athena algtool's Hooks
////////////////////////////
StatusCode JetForwardJvtToolBDT::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
    
  if(m_isAna){
    // -- Retrieve MVfJVT WP configFile ONLY if tool used in 'Analysis mode'
    std::string filename = PathResolverFindCalibFile(std::string(m_configDir)+m_wpFile);
    if (filename.empty()){
      ATH_MSG_ERROR ( "Could NOT resolve file name " << m_wpFile);
      return StatusCode::FAILURE;
    }  else{
      ATH_MSG_INFO(" Config Files Path found = "<<filename);
    }

    // -- Retrieve WP histograms
    m_wpFileIn = std::make_unique<TFile> (filename.c_str(),"read");

    if ( m_OP=="TIGHTER") {
      m_mvfjvtThresh = std::unique_ptr< TH3D >( dynamic_cast<TH3D*>( m_wpFileIn->Get( "MVfJVT_tighter" ) ) );
    } else if ( m_OP=="TIGHT" ) {
      m_mvfjvtThresh = std::unique_ptr< TH3D >( dynamic_cast<TH3D*>( m_wpFileIn->Get( "MVfJVT_tight" ) ) );
    } else if ( m_OP=="DEFAULT" || m_OP=="LOOSE" ) {
      m_mvfjvtThresh = std::unique_ptr< TH3D >( dynamic_cast<TH3D*>( m_wpFileIn->Get( "MVfJVT_loose" ) ) );
    } else {
      ATH_MSG_ERROR(m_OP << " working point doesn't exist." );
      return StatusCode::FAILURE;
    }
    m_mvfjvtThresh->SetDirectory(0);
    m_wpFileIn->Close();
  }

  // -- Setup the tagger
  m_MVreader = std::make_unique< TMVA::Reader > ( "Silent" );
  float fjvt,width,time,cllambda2,cletawidth,cle,cliso,clemprob;
  m_MVreader->AddVariable( "fjvtdist",                       &fjvt         );
  m_MVreader->AddVariable( "Width_jet",                      &width        );
  m_MVreader->AddVariable( "timedist",                       &time         );
  m_MVreader->AddVariable( "jet_LeadingClusterSecondLambda", &cllambda2    );
  m_MVreader->AddVariable( "cl_etaWidthLead",                &cletawidth   );
  m_MVreader->AddVariable( "clsum_e",                        &cle          );
  m_MVreader->AddVariable( "cl_ISOLATIONsumE",               &cliso        );
  m_MVreader->AddVariable( "cl_EM_PROBABILITYsumE",          &clemprob     );
  for(unsigned int i = 0; i<m_MVconfig.size(); ++i) m_MVreader->BookMVA(TString::Format("BDT_%i",i+1),PathResolverFindCalibFile(m_configDir+m_MVconfig.value().at(i)));

  // "passMVfJVT" flag
  m_outMVKey = m_jetContainerName + "." + m_outMVKey.key();
  ATH_CHECK(m_outMVKey.initialize());

  // Moments values
  m_mvfjvtKey = m_jetContainerName + "." + m_mvfjvtKey.key();
  ATH_CHECK(m_mvfjvtKey.initialize());

  m_cllambda2Key = m_jetContainerName + "." + m_cllambda2Key.key();
  m_clwidthKey = m_jetContainerName + "." + m_clwidthKey.key();
  m_clisoKey = m_jetContainerName + "." + m_clisoKey.key();
  m_clemprobKey = m_jetContainerName + "." + m_clemprobKey.key();
  m_cleKey = m_jetContainerName + "." + m_cleKey.key();

  ATH_CHECK(m_cllambda2Key.initialize());
  ATH_CHECK(m_clwidthKey.initialize());
  ATH_CHECK(m_clisoKey.initialize());
  ATH_CHECK(m_clemprobKey.initialize());
  ATH_CHECK(m_cleKey.initialize());

  m_lcllambda2Key = m_jetContainerName + "." + m_lcllambda2Key.key();
  m_lcllambda2NTKey = m_jetContainerName + "." + m_lcllambda2NTKey.key();
  m_lclwidthKey = m_jetContainerName + "." + m_lclwidthKey.key();
  m_lclisoKey = m_jetContainerName + "." + m_lclisoKey.key();
  m_lclemprobKey = m_jetContainerName + "." + m_lclemprobKey.key();
  m_lcleKey = m_jetContainerName + "." + m_lcleKey.key();

  ATH_CHECK(m_lcllambda2Key.initialize());
  ATH_CHECK(m_lcllambda2NTKey.initialize());
  ATH_CHECK(m_lclwidthKey.initialize());
  ATH_CHECK(m_lclisoKey.initialize());
  ATH_CHECK(m_lclemprobKey.initialize());
  ATH_CHECK(m_lcleKey.initialize());

  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_vertexContainerKey.initialize());
  ATH_CHECK(m_caloClusterContainerKey.initialize());
  ATH_CHECK(m_trkMetKey.initialize());
  
  // Truth information
  m_isHSKey = m_jetContainerName + "." + m_isHSKey.key();
  m_isPUKey = m_jetContainerName + "." + m_isPUKey.key();

  ATH_CHECK(m_isHSKey.initialize());
  ATH_CHECK(m_isPUKey.initialize());
  
  return StatusCode::SUCCESS;
}


StatusCode JetForwardJvtToolBDT::decorate(const xAOD::JetContainer& jetCont) const {

  // -- Retrieve PV index if not provided by user
  //pvind = (m_pvind.value()==-1) ? getPV() : m_pvind;
  int pvind = m_pvind.value();
  if(pvind == -1) pvind = getPV();
  
  ATH_MSG_DEBUG("In JetForwardJvtToolBDT::modify: PV index = " << pvind);
  if( pvind == -1 ){
    ATH_MSG_WARNING( "Something went wrong with the HS primary vertex identification." );
    return StatusCode::FAILURE;
  }

  SG::WriteDecorHandle<xAOD::JetContainer, char> outMVHandle(m_outMVKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> mvfjvtHandle(m_mvfjvtKey);

  SG::WriteDecorHandle<xAOD::JetContainer, float> cllambda2Handle(m_cllambda2Key);
  SG::WriteDecorHandle<xAOD::JetContainer, float> clwidthHandle(m_clwidthKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> clisoHandle(m_clisoKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> clemprobHandle(m_clemprobKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> cleHandle(m_cleKey);
  std::vector<TVector2> pileupMomenta;
  for(const xAOD::Jet *jetF : jetCont) {

    float mvfjvt = -2;
    outMVHandle(*jetF) = 1;
    cllambda2Handle(*jetF) = 0;
    clwidthHandle(*jetF) = 0;
    cleHandle(*jetF) = 0;
    clisoHandle(*jetF) = 0;
    clemprobHandle(*jetF) = 0;

    // -- Get PU vertices momenta sums, then compute tagger value for forward jets
    if ( forwardJet(jetF) ){
      if( pileupMomenta.size()==0 ) {
	pileupMomenta = calculateVertexMomenta(&jetCont, pvind);
	if( pileupMomenta.size()==0 ) { 
	  ATH_MSG_DEBUG( "pileupMomenta is empty, this can happen for events with no PU vertices. fJVT won't be computed for this event and will be set to 0 instead." );
	  mvfjvtHandle(*jetF) = 0;
	  continue;
	}
      }
      mvfjvt = getMVfJVT(jetF, pvind, pileupMomenta);
      if(m_isAna) outMVHandle(*jetF) = passMVfJVT( mvfjvt, jetF->pt()/(GeV), fabs(jetF->eta()) );
      mvfjvtHandle(*jetF) = mvfjvt;
    }
  }
  return StatusCode::SUCCESS;

}


float JetForwardJvtToolBDT::getFJVT(const xAOD::Jet *jet, int pvind, std::vector<TVector2> pileupMomenta) const {

  TVector2 fjet(-jet->pt()*cos(jet->phi()),-jet->pt()*sin(jet->phi()));
  double fjvt = 0;
  ATH_MSG_DEBUG("In JetForwardJvtToolBDT::getFJVT -----> Starting looping on vertices (pileupMomenta.size() = "<<pileupMomenta.size());
  for (size_t pui = 0; pui < pileupMomenta.size(); pui++) {
    if (pui!=(size_t)pvind){
      double projection = pileupMomenta[pui]*fjet/fjet.Mod();
      if (projection>fjvt) fjvt = projection;
    }
  }
  return fjvt;
}


float JetForwardJvtToolBDT::getMVfJVT(const xAOD::Jet *jet, int pvind, std::vector<TVector2> pileupMomenta) const {

  if(m_isAna && !m_getTagger) return jet->auxdata<float>("DFCommonJets_MVfJVT");

  StatusCode sc = getInputs(jet);
  if( sc.isFailure() ) {
    ATH_MSG_WARNING(" Could not calculate BDT inputs");
    return -2;
  }

  SG::ReadHandle<xAOD::EventInfo> eventInfoHandle(m_eventInfoKey);
  if ( !eventInfoHandle.isValid() ) {
    ATH_MSG_WARNING(" xAOD::EventInfo " << m_eventInfoKey.key() << "is invalid");
    return -2;
  }
  float mu = eventInfoHandle->actualInteractionsPerCrossing();

  if (!forwardJet(jet)) return -2;

  SG::WriteDecorHandle<xAOD::JetContainer, float> cllambda2Handle(m_cllambda2Key);
  SG::WriteDecorHandle<xAOD::JetContainer, float> clwidthHandle(m_clwidthKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> clisoHandle(m_clisoKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> clemprobHandle(m_clemprobKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> cleHandle(m_cleKey);

  std::vector<float> MVinputs;
  MVinputs.push_back( getFJVT(jet, pvind, pileupMomenta)/jet->pt() );
  MVinputs.push_back( jet->getAttribute<float>("Width")            );
  MVinputs.push_back( jet->getAttribute<float>("Timing")           );
  MVinputs.push_back( cllambda2Handle(*jet)                        );
  MVinputs.push_back( clwidthHandle(*jet)                          );
  MVinputs.push_back( cleHandle(*jet)                              );
  MVinputs.push_back( clisoHandle(*jet)                            );
  MVinputs.push_back( clemprobHandle(*jet)                         );

  float pt = jet->pt()/(GeV);
  float eta = fabs(jet->eta());

  float score = -2.;
  if      ( pt < 30.  && pt >= 20. && eta >= 3.2 && mu>=50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_1"  ,1.);
  else if ( pt < 30.  && pt >= 20. && eta <  3.2 && mu>=50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_2"  ,1.);
  else if ( pt < 40.  && pt >= 30. && eta >= 3.2 && mu>=50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_3"  ,1.);
  else if ( pt < 40.  && pt >= 30. && eta <  3.2 && mu>=50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_4"  ,1.);
  else if ( pt < 50.  && pt >= 40. && eta >= 3.2 && mu>=50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_5"  ,1.);
  else if ( pt < 50.  && pt >= 40. && eta <  3.2 && mu>=50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_6"  ,1.);
  else if ( pt < 120. && pt >= 50. && eta >= 3.2 && mu>=50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_7"  ,1.);
  else if ( pt < 120. && pt >= 50. && eta <  3.2 && mu>=50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_8"  ,1.);
  else if ( pt < 30.  && pt >= 20. && eta >= 3.2 && mu<50.  ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_9"  ,1.);
  else if ( pt < 30.  && pt >= 20. && eta <  3.2 && mu<50.  ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_10" ,1.);
  else if ( pt < 40.  && pt >= 30. && eta >= 3.2 && mu<50.  ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_11" ,1.);
  else if ( pt < 40.  && pt >= 30. && eta <  3.2 && mu<50.  ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_12" ,1.);
  else if ( pt < 50.  && pt >= 40. && eta >= 3.2 && mu<50.  ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_13" ,1.);
  else if ( pt < 50.  && pt >= 40. && eta <  3.2 && mu<50.  ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_14" ,1.);
  else if ( pt < 120. && pt >= 50. && eta >= 3.2 && mu<50.  ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_15" ,1.);
  else if ( pt < 120. && pt >= 50. && eta <  3.2 && mu<50.  ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_16" ,1.);

  ATH_MSG_DEBUG("pt = " << pt << " | eta = " << eta << " | mu = " << mu  << " || MVfJVT = " << score );

  return score;
}

bool JetForwardJvtToolBDT::passMVfJVT( float mvfjvt, float pt, float eta ) const {

  double mvfjvtThresh = -999.;

  SG::ReadHandle<xAOD::EventInfo> eventInfoHandle(m_eventInfoKey);
  if ( !eventInfoHandle.isValid() ) {
    ATH_MSG_WARNING(" xAOD::EventInfo " << m_eventInfoKey.key() << "is invalid");
    return true;
  }

 float mu = eventInfoHandle->actualInteractionsPerCrossing();

  // -- Grab WP from histogram
  mvfjvtThresh = m_mvfjvtThresh->GetBinContent(m_mvfjvtThresh->GetXaxis()->FindBin(pt),
					       m_mvfjvtThresh->GetYaxis()->FindBin(eta),
					       m_mvfjvtThresh->GetZaxis()->FindBin(mu));

  if(mvfjvt==-2 || mvfjvt>mvfjvtThresh) return true;
  else return false;

}

StatusCode JetForwardJvtToolBDT::getInputs(const xAOD::Jet *jet) const {
  SG::WriteDecorHandle<xAOD::JetContainer, float> cllambda2Handle(m_cllambda2Key);
  SG::WriteDecorHandle<xAOD::JetContainer, float> clwidthHandle(m_clwidthKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> clisoHandle(m_clisoKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> clemprobHandle(m_clemprobKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> cleHandle(m_cleKey);

  if(!m_getTagger){
    SG::ReadDecorHandle<xAOD::JetContainer, float> lcllambda2NTHandle(m_lcllambda2NTKey);
    cllambda2Handle(*jet) = lcllambda2NTHandle(*jet);

   // -- Additional variables computed from cluster information
    SG::ReadHandle<xAOD::CaloClusterContainer> clustersHandle(m_caloClusterContainerKey);
    if( !clustersHandle.isValid() ) {
      ATH_MSG_ERROR(" xAOD::CaloClusterContainer " << m_caloClusterContainerKey.key() << "is invalid");
      return StatusCode::FAILURE;
    }

    int ind = 0;
    float maxpt = 0;
    float cle1 = 0;
    float cliso1 = 0;
    float clemprob1 = 0;
    float cle2 = 0;

    // Loop over clusters within DeltaR<0.6 of the jet axis to compute the (energy-weighted) moment sums used in the BDT definitions
    for (const xAOD::CaloCluster *cl: *clustersHandle) {
      if(cl->p4().DeltaR(jet->p4())>0.6) continue;
      cle1 += cl->e();
      cle2 += cl->e()*cl->e();
      cliso1 += cl->auxdata<float>("ISOLATION")*cl->e()*cl->e();
      clemprob1 += cl->auxdata<float>("EM_PROBABILITY")*cl->e()*cl->e();
      if(cl->rawE()/cosh(cl->rawEta()) > maxpt){
	maxpt = cl->rawE()/cosh(cl->rawEta());
	ind = cl->index();
      }
    }
    const xAOD::CaloCluster *cl = clustersHandle->at(ind);
    clwidthHandle(*jet) = TMath::CosH(cl->rawEta()) * TMath::ATan2( TMath::Sqrt(cl->auxdata<float>("SECOND_R")),
							     cl->auxdata<float>("CENTER_MAG"));

    cleHandle(*jet) = cle1;
    clisoHandle(*jet)= cliso1/cle2;
    clemprobHandle(*jet) =clemprob1/cle2;

  } else {
    SG::ReadDecorHandle<xAOD::JetContainer, float> lcllambda2Handle(m_lcllambda2Key);
    SG::ReadDecorHandle<xAOD::JetContainer, float> lclwidthHandle(m_lclwidthKey);
    SG::ReadDecorHandle<xAOD::JetContainer, float> lclisoHandle(m_lclisoKey);
    SG::ReadDecorHandle<xAOD::JetContainer, float> lclemprobHandle(m_lclemprobKey);
    SG::ReadDecorHandle<xAOD::JetContainer, float> lcleHandle(m_lcleKey);

    cllambda2Handle(*jet) = lcllambda2Handle(*jet);
    clwidthHandle(*jet)   = lclwidthHandle(*jet);
    clisoHandle(*jet)     = lclisoHandle(*jet);
    clemprobHandle(*jet)  = lclemprobHandle(*jet);
    cleHandle(*jet)       = lcleHandle(*jet);
  }
  return StatusCode::SUCCESS;
}

std::vector<TVector2> JetForwardJvtToolBDT::calculateVertexMomenta(const xAOD::JetContainer *jets, int pvind) const {

  std::vector<TVector2> pileupMomenta;

  SG::ReadHandle<xAOD::MissingETContainer> trkMetHandle(m_trkMetKey);
  if( !trkMetHandle.isValid() ) {
    ATH_MSG_WARNING(" xAOD::MissingETContainer " << m_trkMetKey.key() << "is invalid");
    return pileupMomenta;
  }
  SG::ReadHandle<xAOD::VertexContainer> vxContHandle(m_vertexContainerKey);
  if( !vxContHandle.isValid() ) {
    ATH_MSG_WARNING(" xAOD::VertexContainer " << m_vertexContainerKey.key() << "is invalid");
    return pileupMomenta;
  }
  ATH_MSG_DEBUG("In JetForwardJvtToolBDT::calculateVertexMomenta : Starting vertex loop  ");
  for(const xAOD::Vertex *vx : *vxContHandle) {
    ATH_MSG_DEBUG("                                                --> VertexType="<<vx->vertexType());
    if(vx->vertexType()!=xAOD::VxType::PriVtx && vx->vertexType()!=xAOD::VxType::PileUp) continue;
    TString vname = "PVTrack_vx";
    vname += vx->index();
    pileupMomenta.push_back((vx->index()==(size_t)pvind?0:-(1./m_jetScaleFactor))*TVector2(0.5*(*trkMetHandle)[vname.Data()]->mpx(),0.5*(*trkMetHandle)[vname.Data()]->mpy()));
  }
  for (const xAOD::Jet *jet : *jets) {
    if (!centralJet(jet)) continue;
    int jetvert = getJetVertex(jet);
    if (jetvert>=0) pileupMomenta[jetvert] += TVector2(0.5*jet->pt()*cos(jet->phi()),0.5*jet->pt()*sin(jet->phi()));
  }

  return pileupMomenta;
}

bool JetForwardJvtToolBDT::forwardJet(const xAOD::Jet *jet) const {

  if (fabs(jet->eta())<m_etaThresh) return false;
  if (jet->pt()<m_forwardMinPt || jet->pt()>m_forwardMaxPt) return false;
  return true;
}

bool JetForwardJvtToolBDT::centralJet(const xAOD::Jet *jet) const {

  if (fabs(jet->eta())>m_etaThresh) return false;
  if (jet->pt()<m_centerMinPt || (m_centerMaxPt>0 && jet->pt()>m_centerMaxPt)) return false;
  float jvt = 0;
  jet->getAttribute<float>(m_jvtMomentName,jvt);
  if (jvt>m_centerJvtThresh) return false;
  if (jet->pt()<m_maxStochPt && getDrpt(jet)<m_centerDrptThresh) return false;
  return true;
}

int JetForwardJvtToolBDT::getJetVertex(const xAOD::Jet *jet) const {

  std::vector<float> sumpts;
  jet->getAttribute<std::vector<float> >("SumPtTrkPt500",sumpts);
  double firstVal = 0;
  int bestMatch = -1;
  for (size_t i = 0; i < sumpts.size(); i++) {
    if (sumpts[i]>firstVal) {
      bestMatch = i;
      firstVal = sumpts[i];
    }
  }
  return bestMatch;
}

float JetForwardJvtToolBDT::getDrpt(const xAOD::Jet *jet) const {

  std::vector<float> sumpts;
  jet->getAttribute<std::vector<float> >("SumPtTrkPt500",sumpts);
  if (sumpts.size()<2) return 0;

  std::nth_element(sumpts.begin(),sumpts.begin()+sumpts.size()/2,sumpts.end(),std::greater<int>());
  double median = sumpts[sumpts.size()/2];
  std::nth_element(sumpts.begin(),sumpts.begin(),sumpts.end(),std::greater<int>());
  double max = sumpts[0];
  return (max-median)/jet->pt();
}

int JetForwardJvtToolBDT::getPV() const{

  SG::ReadHandle<xAOD::VertexContainer> vxContHandle(m_vertexContainerKey);
  if( !vxContHandle.isValid() ) {
    ATH_MSG_WARNING(" xAOD::VertexContainer " << m_vertexContainerKey.key() << "is invalid");
    return 0;
  } else {
    ATH_MSG_DEBUG("Successfully retrieved primary vertex container");
    for(const xAOD::Vertex *vx : *vxContHandle) {
      if(vx->vertexType()==xAOD::VxType::PriVtx) return vx->index();
    }
  }
  ATH_MSG_DEBUG("Couldn't identify the hard-scatter primary vertex (no vertex with \"vx->vertexType()==xAOD::VxType::PriVtx\" in the container)!");
  return 0;
}

StatusCode JetForwardJvtToolBDT::tagTruth(const xAOD::JetContainer *jets,const xAOD::JetContainer *truthJets) {
  SG::WriteDecorHandle<xAOD::JetContainer, char> isHSHandle(m_isHSKey);
  SG::WriteDecorHandle<xAOD::JetContainer, char> isPUHandle(m_isPUKey);
  
  for(const xAOD::Jet *jet : *jets) {
    bool ishs = false;
    bool ispu = true;
    for(const xAOD::Jet *tjet : *truthJets) {
      if (tjet->p4().DeltaR(jet->p4())<0.3 && tjet->pt()>10e3) ishs = true;
      if (tjet->p4().DeltaR(jet->p4())<0.6) ispu = false;
    }
    isHSHandle(*jet)=ishs;
    isPUHandle(*jet)=ispu;
  }
  return StatusCode::SUCCESS;
}
