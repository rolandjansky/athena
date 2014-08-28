/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PhotonAnalysisUtils/DiphotonVertexID.h"
#include "egammaAnalysisUtils/DiphotonVertexIDTool.h"

#include "PathResolver/PathResolver.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"

#include "PhotonAnalysisUtils/IComputePointingQuantities.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "egammaEvent/egamma.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"

#include "TMVA/Reader.h"
#include "TLorentzVector.h"

#include <algorithm>
#include <math.h>

DiphotonVertexID::DiphotonVertexID(const std::string& t, 
                                   const std::string& n,
                                   const IInterface* p) :
  AthAlgTool(t,n,p), m_storeGate(0), m_beamCondSvc(0), m_reader1(0), m_reader2(0)
{
  declareInterface<IDiphotonVertexID>(this);

  declareProperty ("maxVertices", m_maxVertices = 3, "Maximum number of vertices");
  declareProperty ("classifierCut1", m_classifierCut1 = 0.48, 
                   "Minimum value of classifier for conv. photon case");
  declareProperty ("classifierCut2", m_classifierCut2 = 0.36, 
                   "Minimum value of classifier for unconv. photon case");
  declareProperty ("conversionPtCut", m_convPtCut = 2e3, 
                   "Minimum track Pt to be accepted as Si conversion");
  declareProperty ("deltaPhiCut", m_deltaphiCut = 0.04, 
                   "Maximum deltaPhi between track and cluster to accept Si conversion");
  
  declareProperty ("vertexContainer", m_vertexContainer = "VxPrimaryCandidate");
  
  declareProperty ("PAUComputePointingQuantities", m_PAUComputePointingQuantities,
             "PAUComputePointingQuantities for calculating photon pointing information");
  declareProperty("BeamCondSvcName", m_beamCondSvcName="BeamCondSvc");
  declareProperty("useFixedBeamSpread",m_fixedBeamSpread=false);
  declareProperty("beamSpread",m_beamSpread=48);
  declareProperty("isData",m_isData);
}


DiphotonVertexID::~DiphotonVertexID()
{
  delete m_reader1;
  delete m_reader2;
}

StatusCode DiphotonVertexID::initialize()
{
  ATH_MSG_INFO("Initializing " << name() );
  
  // Retrieve StoreGate
  CHECK( service("StoreGateSvc", m_storeGate) );
  
  // Retrieve PAUComputePointingQuantities
  CHECK( m_PAUComputePointingQuantities.retrieve() );
  
  // Retrieve beamCondSvc
  CHECK ( service(m_beamCondSvcName,m_beamCondSvc) );
  
  // Find path for xml files
  std::string readerFile1 = "DiphotonVertex_MLP.weights.xml";
  std::string rFile1 = PathResolver::find_file (readerFile1, "DATAPATH");
  std::string readerFile2 = "DiphotonVertex_MLP2.weights.xml";
  std::string rFile2 = PathResolver::find_file (readerFile2, "DATAPATH");
  
  if ( rFile1 == "" || rFile2 == "") 
  {
    ATH_MSG_ERROR( "Can't access xml files " << readerFile1 << " " << readerFile2);
    return StatusCode::FAILURE;
  }
  
  // Instantiate readers
  m_reader1 = new TMVA::Reader( "!Color:Silent" );
  m_reader1->AddVariable( "log_PV_pt_scal2 := log10(PV_pt_scal2)", &log_PV_pt_scal2 );
  m_reader1->AddVariable( "PV_deltaphi", &PV_deltaphi );
  m_reader1->AddVariable( "PV_deltaz", &PV_deltaz );
  m_reader1->BookMVA( "MLP method" , rFile1);
  
  m_reader2 = new TMVA::Reader( "!Color:Silent" );
  m_reader2->AddVariable( "log_PV_pt_scal2 := log10(PV_pt_scal2)", &log_PV_pt_scal2 );
  m_reader2->AddVariable( "log_PV_pt_scal := log10(PV_pt_scal)", &log_PV_pt_scal );
  m_reader2->AddVariable( "PV_deltaphi", &PV_deltaphi );
  m_reader2->AddVariable( "PV_deltaz", &PV_deltaz );
  m_reader2->BookMVA( "MLP method", rFile2);


  // Find z pointing correction files
  std::string zCorr;
  if (m_isData) zCorr = "histo_zcorrection_data.root";
  else          zCorr = "histo_zcorrection_mc.root";
  std::string zCorrFileName = PathResolver::find_file (zCorr, "DATAPATH");
  if ( zCorrFileName == "" ) {
    ATH_MSG_ERROR("Can't access input z correction file " << zCorr);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Data z correction file found at " << zCorrFileName);
  TFile* zCorrFile = new TFile( zCorrFileName.c_str(), "READ" );
  if ( zCorrFile->IsOpen() ) {
    h_zCorr = dynamic_cast<TH1F*>(zCorrFile->Get("dz_trk_pointing_vs_etas2"));
  } else {
    ATH_MSG_ERROR("Can't open data z correction file " << zCorrFileName);
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}

StatusCode DiphotonVertexID::finalize()
{
  return StatusCode::SUCCESS;
}

std::vector< const Trk::VxCandidate* > DiphotonVertexID::getVertex(egamma *photon1,
                                                                    egamma *photon2, 
                                                                    bool ignoreConv)
{
  m_convertedPhotons.clear();
  mlp_values.clear();
  mlp_ranking.clear();
  m_case = 2;
  m_zCommon = m_zError = -9999.;  
  std::vector< const Trk::VxCandidate* > vertices;
  
  if (!photon1 || !photon2)
  {
    ATH_MSG_WARNING("Invalid pointers to photons");
    return vertices;
  }
   
  // Retrive the primary vertex container
  const VxContainer* vxTES;
  if( m_storeGate->retrieve(vxTES, m_vertexContainer).isFailure() )
  {
    ATH_MSG_WARNING("Could not retrive Primary Vertex container " << m_vertexContainer);
    return vertices;
  }
  if (vxTES->size() == 1)
  {
    ATH_MSG_DEBUG("Only one vertex in container");
    vertices.push_back(vxTES->front());
    return vertices;
  }
   
  if (!ignoreConv) // otherwise ignore photon conversions, treat case 2
  {
    ATH_MSG_DEBUG("Checking photon1 for vertices or Si conversions");
    const Trk::VxCandidate* vx1 = getPhotonVertex(photon1);
    if (vx1) vertices.push_back(vx1);

    ATH_MSG_DEBUG("Checking photon2 for vertices or Si conversions");
    const Trk::VxCandidate* vx2 = getPhotonVertex(photon2);
    if (vx2) vertices.push_back(vx2);
    
    if (vx1 && vx2)
      ATH_MSG_WARNING("Photons associated to different vertices");
      
    if (vx1 || vx2)
    {
      // Photons already associated to a primary vertex, return it
      m_case = 0;
      return vertices;
    }
  }
  
  // Calculate zCommon and zError
  getZandError(photon1, photon2);
  
  // ****** Calculate the MVA response for each vertex ****
  // Choose the appropriate reader
  m_case = (!m_convertedPhotons.empty() ? 1 : 2);
  ATH_MSG_DEBUG("Treating case " << m_case);  
  TMVA::Reader *reader = (m_case == 1) ? m_reader1 : m_reader2;
    
  // Loop over the primary vertices and get the MVA response for each
  VxContainer::const_iterator vtxItr = vxTES->begin(); 
  for ( ; vtxItr != vxTES->end(); ++vtxItr)
  {
    const Trk::VxCandidate* vxcand = (*vtxItr);
    // Skip the vertices with no tracks
    if (!vxcand->vxTrackAtVertex() || !vxcand->vxTrackAtVertex()->size())
      continue;
    
    float diphotonPhi = getDiphotonPhi(photon1, photon2);
    float mlp = getMVAResponse(diphotonPhi, vxcand, reader);
    
    // Insert the mlp result in mlp_values and the PV in mlp_ranking,
    // keeping mlp_values sorted in decreasing order
    vector<float>::iterator itMLP = mlp_values.begin();
    vector<const Trk::VxCandidate*>::iterator itRank = mlp_ranking.begin();
    for ( ; itMLP != mlp_values.end(); ++itMLP, ++itRank)
      if (mlp > (*itMLP) ) break;
    
    mlp_values.insert(itMLP, mlp);
    mlp_ranking.insert(itRank, *vtxItr);
  } // end of loop over primary vertices
  
  // Keep only the maximal number of allowed vertices
  // and remove those that have mlp below the threshold
  if (mlp_ranking.size()) // always get at least 1 vertex if available
    vertices.push_back( mlp_ranking.front() ); 
  for (unsigned int iV = 1; iV < mlp_ranking.size(); ++iV)
  {
    if (iV >= m_maxVertices) break; // already got the maximum number of vertices
    float mlpV = mlp_values[iV];
    if ( (m_case == 1 && mlpV < m_classifierCut1) || 
         (m_case == 2 && mlpV < m_classifierCut2) )
      break; // remaining vertices do not pass mlp cut
    vertices.push_back( mlp_ranking[iV] );
  }
  
  if (vertices.empty())
  {
    ATH_MSG_DEBUG("No vertex selected, returning first primary vertex");
    for ( ; vtxItr != vxTES->end(); ++vtxItr)
    {
      const Trk::VxCandidate* vxcand = (*vtxItr);
      if (vxcand && vxcand->vertexType() == Trk::PriVtx)
      {
        vertices.push_back( vxcand );
        break;
      }
    } 
  }
    
  return vertices;
}


float DiphotonVertexID::getDiphotonPhi(egamma *photon1, egamma *photon2)
{
  float px = photon1->cluster()->px() + photon2->cluster()->px();
  float py = photon1->cluster()->py() + photon2->cluster()->py();
  return atan2( py, px );
}
  

float DiphotonVertexID::getMVAResponse(float diphotonPhi,
                                       const Trk::VxCandidate *vxcand,
                                       TMVA::Reader *reader)
{
  float zVertex = vxcand->recVertex().position().z();
  PV_deltaz = (m_zCommon - zVertex)/m_zError;
  
  // skip vertices above 10 sigma from pointing
  if (m_case == 2 && fabs(PV_deltaz) > 10) 
  {
    ATH_MSG_DEBUG("MLP = -9999. deltaZ/sigma = " << PV_deltaz);
    return -9999.;
  }

  // Loop over the tracks and compute the sumPt, sumPt2 and phi
  TLorentzVector v, v_PV;
  float sumpt = 0., sumpt2 = 0.;
  
  const std::vector<Trk::VxTrackAtVertex*> *trkAtVtx = vxcand->vxTrackAtVertex();
  std::vector<Trk::VxTrackAtVertex*>::const_iterator itVxTrk; 
  for (itVxTrk = trkAtVtx->begin() ; itVxTrk != trkAtVtx->end(); ++itVxTrk)
  {
    const Trk::Perigee* perigee = 0;
    if ( (*itVxTrk)->perigeeAtVertex() )
      perigee = (const Trk::Perigee*) (*itVxTrk)->perigeeAtVertex();
    else
      perigee = (const Trk::Perigee*) (*itVxTrk)->initialPerigee();
    
    v.SetXYZM(perigee->momentum().x(), perigee->momentum().y(), 0., 0.);
    v_PV += v;
    sumpt += v.Pt();
    sumpt2 += DiphotonVertexIDTool::square( v.Pt() );
  }
      
  // Set the inputs for the MLP and calculate the response
  // log sumPt, log sumPt2, deltaPhi(diphoton,PV), deltaZ/sigma (above)
  PV_deltaphi = fabs(DiphotonVertexIDTool::deltaPhi(v_PV.Phi(), diphotonPhi));
  log_PV_pt_scal = log10(sumpt);
  log_PV_pt_scal2 = log10(sumpt2);
  
  float mlp = reader->EvaluateMVA("MLP method");
  ATH_MSG_DEBUG("MLP = " << mlp << 
                " deltaZ/sigma = " << PV_deltaz << 
                " deltaPhi = " << PV_deltaphi <<
                " sumpt = " << sumpt <<
                " sumpt2 = " << sumpt2);
  return mlp;
}


/** Return the primary vertex associated to the photon if any **/
const Trk::VxCandidate* DiphotonVertexID::getPhotonVertex(egamma *photon)
{
  if (!photon || !photon->nConversions())
    return 0;
  
  const Trk::VxCandidate *vxcand = photon->conversion();
  
  // Loop over conversion tracks
  // return the vertex associated to any of them or flag the SiSi conversions
  const std::vector<Trk::VxTrackAtVertex*> *tracksAtVertex = vxcand->vxTrackAtVertex();
  for (unsigned int itr = 0; itr< tracksAtVertex->size(); ++itr)
  {
    Trk::VxTrackAtVertex* trkAtVtx = tracksAtVertex->at(itr);
    const Rec::TrackParticle* tp = getTrackParticle(trkAtVtx);
    if (!tp) continue;

    // Flag photon as Si conversion if track passes pt and dphi cuts
    const Trk::Perigee *pAtVertex = dynamic_cast<const Trk::Perigee*> \
      (trkAtVtx->perigeeAtVertex());
    if (!pAtVertex) continue;
    
    double pt = pAtVertex->pT();
    double phi = pAtVertex->parameters()[Trk::phi];
    double deltaPhi = DiphotonVertexIDTool::deltaPhi(photon->cluster()->phi(), phi);
    
    /*
../src/DiphotonVertexID.cxx:319:11: error: 'MeasuredPerigee' in namespace 'Trk' does not name a type
../src/DiphotonVertexID.cxx:321:10: error: 'pAtVertex' was not declared in this scope
../src/DiphotonVertexID.cxx:323:17: error: 'pAtVertex' was not declared in this scope
    */

    if (pt < m_convPtCut || fabs(deltaPhi) > m_deltaphiCut)
      continue;

    // Get track summary and check if has Si hits
    const Trk::TrackSummary *trackSummary = tp->trackSummary();
    if (!trackSummary) continue;
    if (trackSummary->get(Trk::numberOfPixelHits) +\
        trackSummary->get(Trk::numberOfSCTHits) == 0)
      continue;
      
    ATH_MSG_DEBUG("Si conversion found");
    m_convertedPhotons.push_back(photon);
    
    // Check if the track is associated to a primary vertex
    // and return the vertex if so
    // (this is done only for tracks that pass the selection) 
    const Trk::VxCandidate *trkVx = tp->reconstructedVertex();
    if (trkVx && 
        (trkVx->vertexType() == Trk::PriVtx || trkVx->vertexType() == Trk::PileUp ))
    {
      ATH_MSG_DEBUG("Photon is associated to a primary (or pileup) vertex");
      return trkVx;
    }      
  } // end of loop over tracks
  return 0;
}


const Rec::TrackParticle* DiphotonVertexID::getTrackParticle(const Trk::VxTrackAtVertex *trackAtVertex)
{
  const Trk::ITrackLink* link = trackAtVertex->trackOrParticleLink();
  const Trk::LinkToTrackParticleBase *tr_part = \
    dynamic_cast<const Trk::LinkToTrackParticleBase*>(link);
  if(!tr_part) return 0;
  return dynamic_cast<const Rec::TrackParticle*>(**tr_part); 
}

std::pair<float, float> DiphotonVertexID::getZandError(egamma *photon)
{
  StatusCode sc = m_PAUComputePointingQuantities->execute(photon);
  if (sc.isFailure())
    ATH_MSG_WARNING("ComputePointingQuantities did not return success");
  
  float z, sigmaz;
  if (isConvertedPhoton(photon))
  {
    z = m_PAUComputePointingQuantities->return_HPV_zvertex() ;
    sigmaz = m_PAUComputePointingQuantities->return_HPV_sigma_zvertex() ;
  }
  else
  {
    z = m_PAUComputePointingQuantities->return_CaloPointing_zvertex() ;
    sigmaz = m_PAUComputePointingQuantities->return_CaloPointing_sigma_zvertex() ;

    // correct for oscillations in endcaps
    if (photon->cluster() && 
        (	 
         ( !photon->cluster()->inBarrel()  && photon->cluster()->inEndcap() ) || 
         (  photon->cluster()->inBarrel()  && photon->cluster()->inEndcap() && 
            photon->cluster()->eSample(CaloSampling::EME1) > photon->cluster()->eSample(CaloSampling::EMB1) )
         ) //EndCap case
        ) {
      z = z - h_zCorr->Interpolate(photon->cluster()->etaBE(2));
    }
    
  }
  return std::make_pair(z, sigmaz);
}

void DiphotonVertexID::getZandError(egamma *photon1, egamma *photon2)
{
  // Beamspot
  float zb = m_beamCondSvc->beamPos().z();
  float sigma_zb = -999999.;
  if (m_fixedBeamSpread)
    sigma_zb = m_beamSpread;
  else
    sigma_zb = m_beamCondSvc->beamSigma(2);
  ATH_MSG_VERBOSE("zb = " << zb << " sigma_zb = " << sigma_zb);
  
  m_zCommon = zb/DiphotonVertexIDTool::square(sigma_zb);
  m_zError  = 1./DiphotonVertexIDTool::square(sigma_zb);
  
  std::pair<float, float> z = getZandError(photon1);
  ATH_MSG_VERBOSE("z1 = " << z.first << " sigma_z1 = " << z.second);
  m_zCommon += z.first/DiphotonVertexIDTool::square(z.second);
  m_zError += 1./DiphotonVertexIDTool::square(z.second);

  z = getZandError(photon2);
  ATH_MSG_VERBOSE("z2 = " << z.first << " sigma_z2 = " << z.second);
  m_zCommon += z.first/DiphotonVertexIDTool::square(z.second);
  m_zError += 1./DiphotonVertexIDTool::square(z.second);
  
  m_zCommon /= m_zError;
  m_zError = 1./sqrt( m_zError );
  ATH_MSG_VERBOSE("zCommon = " << m_zCommon << " zError = " << m_zError);
}


/** Return true if photon is converted (with Si track and passing pt and deltaPhi cut) 
  * Must be called after getPhotonVertex **/
bool DiphotonVertexID::isConvertedPhoton(egamma *photon)
{
  std::vector< egamma* >::iterator it;
  it = std::find(m_convertedPhotons.begin(), m_convertedPhotons.end(), photon);
  return it != m_convertedPhotons.end();
}
