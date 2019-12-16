///////////////////////// -*- C++ -*- /////////////////////////////
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    declareProperty( "OverlapDec",        m_orLabel          = ""                                           );
    declareProperty( "OutputDecMV",       m_outLabelMV       = "passMVFJVT"                                 );
    declareProperty( "EtaThresh",         m_etaThresh        = 2.5                                          );
    declareProperty( "ForwardMinPt",      m_forwardMinPt     = 20e3                                         );
    declareProperty( "ForwardMaxPt",      m_forwardMaxPt     = 120e3                                        );
    declareProperty( "CentralMinPt",      m_centerMinPt      = 20e3                                         );
    declareProperty( "CentralMaxPt",      m_centerMaxPt      = -1                                           );
    declareProperty( "CentralJvtThresh",  m_centerJvtThresh  = 0.11                                         );
    declareProperty( "JvtMomentName",     m_jvtMomentName    = "Jvt"                                        );
    declareProperty( "CentralDrptThresh", m_centerDrptThresh = 0.2                                          );
    declareProperty( "CentralMaxStochPt", m_maxStochPt       = 35e3                                         );
    declareProperty( "JetScaleFactor",    m_jetScaleFactor   = 0.4                                          );
    declareProperty( "OperatingPoint",    m_OP               = "DEFAULT"                                    );// "DEFAULT"/"TIGHT"/"TIGHTER"
    declareProperty( "retrieveTagger",    m_getTagger        = false                                        );
    declareProperty( "AnaToolMode",       m_isAna            = false                                        );// false: computes all input moments from scratch, true: retrieve precomputed moments
    declareProperty( "VtxContName",       m_vtxcont          = "PrimaryVertices"                            );
    declareProperty( "PVIndexHS",         m_pvind            = -1                                           );
    declareProperty( "WPfile",            m_wpFile           = "MVfJVT_WPs.Nov2019.root"                    );
    declareProperty( "configDir",         m_configDir        = "JetPileupTag/MVfJVT/"                       );
    declareProperty( "ConfigFiles",       m_MVconfig         = {
	  "weights/MVfJVT_pt2030_etaHigh_muHigh.May2019.weights.xml",
	  "weights/MVfJVT_pt2030_etaLow_muHigh.May2019.weights.xml",
	  "weights/MVfJVT_pt3040_etaHigh_muHigh.May2019.weights.xml",
	  "weights/MVfJVT_pt3040_etaLow_muHigh.May2019.weights.xml",
	  "weights/MVfJVT_pt4050_etaHigh_muHigh.May2019.weights.xml",
	  "weights/MVfJVT_pt4050_etaLow_muHigh.May2019.weights.xml",
	  "weights/MVfJVT_pt50plus_etaHigh_muHigh.May2019.weights.xml",
	  "weights/MVfJVT_pt50plus_etaLow_muHigh.May2019.weights.xml",
	  "weights/MVfJVT_pt2030_etaHigh_muLow.May2019.weights.xml",
	  "weights/MVfJVT_pt2030_etaLow_muLow.May2019.weights.xml",
	  "weights/MVfJVT_pt3040_etaHigh_muLow.May2019.weights.xml",
	  "weights/MVfJVT_pt3040_etaLow_muLow.May2019.weights.xml",
	  "weights/MVfJVT_pt4050_etaHigh_muLow.May2019.weights.xml",
	  "weights/MVfJVT_pt4050_etaLow_muLow.May2019.weights.xml",
	  "weights/MVfJVT_pt50plus_etaHigh_muLow.May2019.weights.xml",
	  "weights/MVfJVT_pt50plus_etaLow_muLow.May2019.weights.xml"}); // pt [20,30,40,50,120] || |eta| [2.5,3.2,4.5] || mu [0,50,inf.]
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
      std::string filename = PathResolverFindCalibFile(m_configDir+m_wpFile);
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
    for(unsigned int i = 0; i<m_MVconfig.size(); ++i) m_MVreader->BookMVA(TString::Format("BDT_%i",i+1),PathResolverFindCalibFile(m_configDir+m_MVconfig.at(i)));

    // "passMVfJVT" flag
    Dec_outMV = std::make_unique< SG::AuxElement::Decorator<char> >(m_outLabelMV);

    // Moments values
    Dec_mvfjvt     = std::make_unique< SG::AuxElement::Decorator<float> > ( "MVfJVT"  );
    Dec_cllambda2  = std::make_unique< SG::AuxElement::Decorator<float> > ( "LeadclSecondLambda" );
    Dec_cletawidth = std::make_unique< SG::AuxElement::Decorator<float> > ( "LeadclWidth"        );
    Dec_cle        = std::make_unique< SG::AuxElement::Decorator<float> > ( "Sumcle"             );
    Dec_cliso      = std::make_unique< SG::AuxElement::Decorator<float> > ( "SumclIso"           );
    Dec_clemprob   = std::make_unique< SG::AuxElement::Decorator<float> > ( "SumclEMprob"        );

    // Truth information
    Dec_isHS       = std::make_unique< SG::AuxElement::Decorator<char> > ( "isJvtHS" );
    Dec_isPU       = std::make_unique< SG::AuxElement::Decorator<char> > ( "isJvtPU" );

    return StatusCode::SUCCESS;
}


int JetForwardJvtToolBDT::modify(xAOD::JetContainer& jetCont) const {

  // -- Retrieve PV index if not provided by user
  int pvind = (m_pvind==-1) ? getPV() : m_pvind;
  ATH_MSG_DEBUG("In JetForwardJvtToolBDT::modify: PV index = " << pvind);
  if( pvind == -1 ){
    ATH_MSG_ERROR( "Something went wrong with the HS primary vertex identification." );
    return 1;
  }

  std::vector<TVector2> pileupMomenta;
  for(const xAOD::Jet *jetF : jetCont) {

    (*Dec_mvfjvt)(*jetF) = -2;
    (*Dec_outMV)(*jetF) = 1;
    (*Dec_cllambda2)(*jetF) = 0;
    (*Dec_cletawidth)(*jetF) = 0;
    (*Dec_cle)(*jetF) = 0;
    (*Dec_cliso)(*jetF) = 0;
    (*Dec_clemprob)(*jetF) = 0;

    // -- Get PU vertices momenta sums, then compute tagger value for forward jets
    if ( forwardJet(jetF) ){
      if( pileupMomenta.size()==0 ) {
	pileupMomenta = calculateVertexMomenta(&jetCont, pvind);
	if(pileupMomenta.size()==0) return StatusCode::FAILURE; // This would follow error messages defined in 'calculateVertexMomenta' function.
      }
      (*Dec_mvfjvt)(*jetF) = getMVfJVT(jetF, pvind, pileupMomenta);
      if(m_isAna) (*Dec_outMV)(*jetF) = passMVfJVT( (*Dec_mvfjvt)(*jetF), jetF->pt()/(GeV), fabs(jetF->eta()) );
    }
  }
  return 0;

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

  ATH_CHECK(getInputs(jet));

  const xAOD::EventInfo *eventInfo = nullptr;
  if ( evtStore()->retrieve(eventInfo, "EventInfo").isFailure() )
  {
    ATH_MSG_ERROR(" Could not retrieve EventInfo ");
    return StatusCode::FAILURE;
  }
  float mu = eventInfo->actualInteractionsPerCrossing();

  if (!forwardJet(jet)) return -2;

  std::vector<float> MVinputs;
  MVinputs.push_back( getFJVT(jet, pvind, pileupMomenta)/jet->pt() );
  MVinputs.push_back( jet->getAttribute<float>("Width")            );
  MVinputs.push_back( jet->getAttribute<float>("Timing")           );
  MVinputs.push_back( (*Dec_cllambda2)(*jet)                       );
  MVinputs.push_back( (*Dec_cletawidth)(*jet)                      );
  MVinputs.push_back( (*Dec_cle)(*jet)                             );
  MVinputs.push_back( (*Dec_cliso)(*jet)                           );
  MVinputs.push_back( (*Dec_clemprob)(*jet)                        );

  float pt = jet->pt()/(GeV);
  float eta = fabs(jet->eta());

  float score = -2.;
  if      ( pt < 30.  && pt >= 20. && eta >= 3.2 && mu>50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_1"  ,1.);
  else if ( pt < 30.  && pt >= 20. && eta <  3.2 && mu>50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_2"  ,1.);
  else if ( pt < 40.  && pt >= 30. && eta >= 3.2 && mu>50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_3"  ,1.);
  else if ( pt < 40.  && pt >= 30. && eta <  3.2 && mu>50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_4"  ,1.);
  else if ( pt < 50.  && pt >= 40. && eta >= 3.2 && mu>50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_5"  ,1.);
  else if ( pt < 50.  && pt >= 40. && eta <  3.2 && mu>50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_6"  ,1.);
  else if ( pt < 120. && pt >= 50. && eta >= 3.2 && mu>50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_7"  ,1.);
  else if ( pt < 120. && pt >= 50. && eta <  3.2 && mu>50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_8"  ,1.);
  else if ( pt < 30.  && pt >= 20. && eta >= 3.2 && mu<50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_9"  ,1.);
  else if ( pt < 30.  && pt >= 20. && eta <  3.2 && mu<50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_10" ,1.);
  else if ( pt < 40.  && pt >= 30. && eta >= 3.2 && mu<50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_11" ,1.);
  else if ( pt < 40.  && pt >= 30. && eta <  3.2 && mu<50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_12" ,1.);
  else if ( pt < 50.  && pt >= 40. && eta >= 3.2 && mu<50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_13" ,1.);
  else if ( pt < 50.  && pt >= 40. && eta <  3.2 && mu<50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_14" ,1.);
  else if ( pt < 120. && pt >= 50. && eta >= 3.2 && mu<50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_15" ,1.);
  else if ( pt < 120. && pt >= 50. && eta <  3.2 && mu<50. ) score = m_MVreader->EvaluateMVA( MVinputs, "BDT_16" ,1.);

  ATH_MSG_DEBUG("pt = " << pt << " | eta = " << eta << " | mu = " << mu  << " || MVfJVT = " << score );

  return score;
}

bool JetForwardJvtToolBDT::passMVfJVT( float mvfjvt, float pt, float eta ) const {

  double mvfjvtThresh = -999.;

  const xAOD::EventInfo *eventInfo = nullptr;
  if ( evtStore()->retrieve(eventInfo, "EventInfo").isFailure() )
  {
    ATH_MSG_ERROR(" Could not retrieve EventInfo ");
    return StatusCode::FAILURE;
  }

 float mu = eventInfo->actualInteractionsPerCrossing();

  // -- Grab WP from histogram
  mvfjvtThresh = m_mvfjvtThresh->GetBinContent(m_mvfjvtThresh->GetXaxis()->FindBin(pt),
					       m_mvfjvtThresh->GetYaxis()->FindBin(eta),
					       m_mvfjvtThresh->GetZaxis()->FindBin(mu));

  if(mvfjvt==-2 || mvfjvt>mvfjvtThresh) return true;
  else return false;

}

StatusCode JetForwardJvtToolBDT::getInputs(const xAOD::Jet *jet) const {

  if(!m_getTagger){
    jet->getAttribute(xAOD::JetAttribute::LeadingClusterSecondLambda,(*Dec_cllambda2)(*jet));

   // -- Additional variables computed from cluster information
    const xAOD::CaloClusterContainer* clusters  = nullptr;
    if( evtStore()->retrieve(clusters, "CaloCalTopoClusters").isFailure())
    {
      ATH_MSG_ERROR("Unable to retrieve CaloCalTopoClusters container.");
      return StatusCode::FAILURE;
    }

    int ind = 0;
    float maxpt = 0;
    float cle1 = 0;
    float cliso1 = 0;
    float clemprob1 = 0;
    float cle2 = 0;

    // Loop over clusters within DeltaR<0.6 of the jet axis to compute the (energy-weighted) moment sums used in the BDT definitions
    for (const xAOD::CaloCluster *cl: *clusters) {
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
    const xAOD::CaloCluster *cl = clusters->at(ind);
    (*Dec_cletawidth)(*jet) = TMath::CosH(cl->rawEta()) * TMath::ATan2( TMath::Sqrt(cl->auxdata<float>("SECOND_R")),
							     cl->auxdata<float>("CENTER_MAG"));

    (*Dec_cle)(*jet) = cle1;
    (*Dec_cliso)(*jet)= cliso1/cle2;
    (*Dec_clemprob)(*jet) =clemprob1/cle2;

  } else {
    (*Dec_cllambda2)(*jet)  = jet->auxdata<float>("DFCommonJets_MVfJVT_LeadclSecondLambda");
    (*Dec_cletawidth)(*jet) = jet->auxdata<float>("DFCommonJets_MVfJVT_LeadclWidth");
    (*Dec_cliso)(*jet)      = jet->auxdata<float>("DFCommonJets_MVfJVT_SumclIso");
    (*Dec_clemprob)(*jet)   = jet->auxdata<float>("DFCommonJets_MVfJVT_SumclEMprob");
    (*Dec_cle)(*jet)        = jet->auxdata<float>("DFCommonJets_MVfJVT_Sumcle");
  }
  return StatusCode::SUCCESS;
}

std::vector<TVector2> JetForwardJvtToolBDT::calculateVertexMomenta(const xAOD::JetContainer *jets, int pvind) const {

  std::vector<TVector2> pileupMomenta;

  const xAOD::MissingETContainer* trkMet  = nullptr;
  if( evtStore()->retrieve(trkMet, "MET_Track").isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve MET_Track container");
    return pileupMomenta;
  }
  const xAOD::VertexContainer *vxCont = 0;
  if( evtStore()->retrieve(vxCont, m_vtxcont).isFailure() ){
    ATH_MSG_ERROR("Unable to retrieve primary vertex container \"" << m_vtxcont << "\"");
    return pileupMomenta;
  }
  ATH_MSG_DEBUG("In JetForwardJvtToolBDT::calculateVertexMomenta : Starting vertex loop  ");
  for(const xAOD::Vertex *vx : *vxCont) {
    ATH_MSG_DEBUG("                                                --> VertexType="<<vx->vertexType());
    if(vx->vertexType()!=xAOD::VxType::PriVtx && vx->vertexType()!=xAOD::VxType::PileUp) continue;
    TString vname = "PVTrack_vx";
    vname += vx->index();
    pileupMomenta.push_back((vx->index()==(size_t)pvind?0:-(1./m_jetScaleFactor))*TVector2(0.5*(*trkMet)[vname.Data()]->mpx(),0.5*(*trkMet)[vname.Data()]->mpy()));
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

  const xAOD::VertexContainer *vxCont = 0;
  if( evtStore()->retrieve(vxCont, m_vtxcont).isFailure() ) {
    ATH_MSG_ERROR("Unable to retrieve primary vertex container");
    return -1;
  } else {
    ATH_MSG_DEBUG("Successfully retrieved primary vertex container");
    for(const xAOD::Vertex *vx : *vxCont) {
      if(vx->vertexType()==xAOD::VxType::PriVtx) return vx->index();
    }
  }
  ATH_MSG_WARNING("Couldn't identify the hard-scatter primary vertex (no vertex with \"vx->vertexType()==xAOD::VxType::PriVtx\" in the container)!");
  return 0;
}

StatusCode JetForwardJvtToolBDT::tagTruth(const xAOD::JetContainer *jets,const xAOD::JetContainer *truthJets) {
  for(const xAOD::Jet *jet : *jets) {
    bool ishs = false;
    bool ispu = true;
    for(const xAOD::Jet *tjet : *truthJets) {
      if (tjet->p4().DeltaR(jet->p4())<0.3 && tjet->pt()>10e3) ishs = true;
      if (tjet->p4().DeltaR(jet->p4())<0.6) ispu = false;
    }
    (*Dec_isHS)(*jet)=ishs;
    (*Dec_isPU)(*jet)=ispu;
  }
  return StatusCode::SUCCESS;
}
