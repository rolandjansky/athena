/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************************************************************** 
//                  
//               Tool to flag electrons as conversion with 0, 1 or 2 tracks reconstructed
//                               Jean-Francois MARCHAND - 09/06/2007
//
//
// Returns :
//
//   int ConversionFlag == -1 if the electron cannot be flagged as converted photon
//
//   int ConversionFlag == 1  not used, reserved for converted photons from photon container
//   int ConversionFlag == 2  not used, reserved for converted photons from photon container
//
//   int ConversionFlag == 11  if a single track conversion is found using extrapolation method
//   int ConversionFlag == 12  if a double track conversion is found using extrapolation method
//
// if the flag useDBT is false (default) :
//   int ConversionFlag == 21  if the electron has a TRT standalone track as best track match
//   int ConversionFlag == 20  not used
//
// if the flag useDBT is true :
//   int ConversionFlag == 21  if electron has a TRT standalone track as trackmatch, and if the BDT value is >BDT cut (= should be converted photon)
//   int ConversionFlag == 20  if electron has a TRT standalone track as trackmatch, and if the BDT value is <BDT cut (= should be unconverted photon)
//
//
// **************************************************************************************************

#include "PhotonAnalysisUtils/PAUconversionFlagTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/RecVertex.h"

//#include "xAODTracking/Vertex.h"
//#include "xAODTracking/TrackParticle.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkTrack/LinkToTrack.h"

#include "egammaEvent/egamma.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/Photon.h"
#include "egammaEvent/EMTrackMatch.h"

#include "egammaEvent/EMShower.h"
#include "CaloEvent/CaloCluster.h"

#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
#include "TMVA/Reader.h"
#include "TMVA/IMethod.h"

#include "TrkEventPrimitives/PropDirection.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"

#include "AthContainers/ConstDataVector.h"

#include <algorithm>
#include <math.h>
#include <functional>


using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;


Trk::ParticleMasses PAUconversionFlagTool::s_particleMasses;

// constructor and destructor for PAUconversionFlagTool::conversion_var_t
PAUconversionFlagTool::conversion_var_t::conversion_var_t() :
  conv_flag(-1), conv_vtx(0), conv_R(-9999), conv_vtx_chi2(-9999) {}


// constructor
PAUconversionFlagTool::PAUconversionFlagTool(const std::string& t, const std::string& n, const IInterface* p) : 
  AthAlgTool(t,n,p),
  m_VertexFinderTool("InDet::InDetConversionFinderTools"), 
  m_extrapolateToCaloTool("PAUexToCalo"),
  m_singleTrkConvTool("InDet::SingleTrackConversionTool")
{
  declareInterface<IPAUconversionFlagTool>(this);
  declareProperty("ExtrapolateToCaloTool",       m_extrapolateToCaloTool);
  declareProperty("useBDT", m_useBDT=false);
  declareProperty("BDTCut", m_BDTCut=0.2);
  declareProperty("DBTFileName", m_BDTFileName="TMVAnalysis_BDT.weights.txt");
  declareProperty("narrowDeltaEta", m_narrowDeltaEta = 0.05  );
  declareProperty("narrowDeltaPhi", m_narrowDeltaPhi = 0.05  );
  declareProperty("narrowDeltaPhiBrem", m_narrowDeltaPhiBrem = 0.10  );
  declareProperty("deltaRmax", m_drmax=0.20);
  declareProperty("TrackParticleInputName", m_TrackParticleInputName="TrackParticleCandidate");
  declareProperty( "VertexFinderTool", m_VertexFinderTool );
  declareProperty( "SingleTrkConvTool", m_singleTrkConvTool, "Single track conversion tool" );
}


// destructor
PAUconversionFlagTool::~PAUconversionFlagTool()
{}


// initialize
StatusCode PAUconversionFlagTool::initialize()
{
  MsgStream log(msgSvc(), name());
  log <<  MSG::INFO  << name() <<"in initialize()..." << endreq;    
  
  // Pointer to Tool Service
  IToolSvc* p_toolSvc = 0;
  StatusCode sc = service("ToolSvc", p_toolSvc);
  if (sc.isFailure()) {
    log << MSG::FATAL << " Tool Service not found " << endreq;
    return StatusCode::FAILURE;
  }
  
  /* Get the extrapolate to Calo tool */
  if ( m_extrapolateToCaloTool.retrieve().isFailure() ) {
    log << MSG::ERROR << "Failed to retrieve tool " << m_extrapolateToCaloTool << endreq;
    return StatusCode::SUCCESS;
  }
  else {
     log << MSG::INFO << "Retrieved tool " << m_extrapolateToCaloTool << endreq;
  }

  /* Get the single conversion tool */
  if ( m_singleTrkConvTool.retrieve().isFailure() ) {
    log << MSG::ERROR << "Failed to retrieve tool " << m_singleTrkConvTool << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Retrieved tool " << m_singleTrkConvTool << endreq;
  }

  StoreGateSvc* detStore;
  sc = service("DetectorStore",detStore);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to get pointer to Detector Store Service" << endreq;
    return sc;
  }

  sc = detStore->retrieve(m_calo_helper);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve calo helper from DetectorStore" << endreq;
    return sc;
  }

  if (m_useBDT) {
    m_reader = new TMVA::Reader();  
    m_reader->AddVariable( "f1", &f1 );
    m_reader->AddVariable( "weta1", &weta1 );
    m_reader->AddVariable( "wtots1", &wtots1 );
    m_reader->AddVariable( "fracs1", &fracs1 );
    m_reader->AddVariable( "weta2", &weta2);
    m_reader->AddVariable( "TRThits+TRTHThits", &sumTRThits);
    m_reader->AddVariable( "e_ps", &energy_ps);
    m_reader->BookMVA( "BDT method", m_BDTFileName);
  }

  log << MSG::DEBUG << "Before VertexFinder tool" << endreq;

  /* Get the vertex finder tool */
  if ( m_VertexFinderTool.retrieve().isFailure() ) {
    log << MSG::ERROR << "Failed to retrieve tool " << m_VertexFinderTool << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Retrieved tool " << m_VertexFinderTool << endreq;
  }

  log <<  MSG::INFO  << name() <<" initialize() successful" << endreq;    
  return StatusCode::SUCCESS;
}


// finalize
StatusCode PAUconversionFlagTool::finalize()
{
  MsgStream log(msgSvc(), name());
  log <<  MSG::VERBOSE  << "... in finalize() ..." << endreq ;
  return StatusCode::SUCCESS;
}


const Trk::VxCandidate* PAUconversionFlagTool::ConvTrack(const Rec::TrackParticle* track, const VxContainer* convContainer) {
  if ( track == NULL ) return NULL ;
  if ( convContainer == NULL ) return NULL ;

  float Rmin=999999999;
  const Trk::VxCandidate* best_vtx = 0;

  // start loop over conversions
  VxContainer::const_iterator vtxItr  = convContainer->begin();
  VxContainer::const_iterator vtxItrE = convContainer->end();
  for (; vtxItr != vtxItrE; vtxItr++) {
    const Trk::VxCandidate* vxcand = (*vtxItr);
    const std::vector<Trk::VxTrackAtVertex*>* trklist = vxcand->vxTrackAtVertex();
    for (unsigned int i=0; i<trklist->size();i++) {
      Trk::ITrackLink* link = ((*trklist)[i])->trackOrParticleLink();
      const Trk::LinkToTrackParticleBase* tr_part = dynamic_cast<const Trk::LinkToTrackParticleBase*>(link);
      if(!tr_part) continue;
      const Rec::TrackParticle* tp = dynamic_cast<const Rec::TrackParticle*>(**tr_part); 
      if (tp==track) {
	// Conversion vertex
	float Rconv = sqrt(vxcand->recVertex().position()[0]*vxcand->recVertex().position()[0] + 
			   vxcand->recVertex().position()[1]*vxcand->recVertex().position()[1]);
	if (Rconv<Rmin) {
	  Rmin=Rconv;
	  best_vtx = vxcand;
	}
	else continue;
      }
    }
  }
  return best_vtx;  
}


PAUconversionFlagTool::conversion_var_t PAUconversionFlagTool::ConvFlag(const Analysis::Electron* electron, 
								  const VxContainer* /*convContainer*/ , 
								  const PhotonContainer* /*photonContainer*/ ) {
 MsgStream log(msgSvc(), name());
  conversion_var_t conv_var;
  conv_var.conv_flag = -1;
  conv_var.conv_vtx = NULL;
  conv_var.conv_R=-9999;
  conv_var.conv_vtx_chi2 = -9999;

  bool foundInC1C2extrapol = false;

  int electron_author = electron->author();
  if (electron_author!=2 && electron_author!=8) { // not softE and not forward electron


    /*********************************************************************************************/
    /**   First step : Exactly the extrapolation method from egamma tool conversionAODRec.cxx   **/
    const Rec::TrackParticleContainer* TrackParticleInputContainer;
    StatusCode sc = evtStore()->retrieve(TrackParticleInputContainer, m_TrackParticleInputName);
    if(sc.isFailure()  ||  !TrackParticleInputContainer) {
      log << MSG::DEBUG << "no TrackParticleInputContainer found in TDS" << endreq;
      return conv_var;
    }

    const CaloCluster* egCalo = electron->cluster();

    const Trk::VxCandidate* matchVxCand(0);
    ConstDataVector<Rec::TrackParticleContainer> tmpTrkCol (SG::VIEW_ELEMENTS);
    VxContainer* tmpConversionContainer(0);
    
    // Reconstruct the conversions from the TrackParticles around the cluster
    std::vector<const Trk::VxCandidate*> recVxCand(0);
    double eta = egCalo->eta();
    double phi = egCalo->phi();
    
    Rec::TrackParticleContainer::const_iterator tpItr   = TrackParticleInputContainer->begin();
    Rec::TrackParticleContainer::const_iterator tpItrE  = TrackParticleInputContainer->end();
    for(; tpItr!=tpItrE; ++tpItr) {
      double eta_track = (*tpItr)->eta();
      double phi_track = (*tpItr)->phi();
      const Trk::TrackSummary*  trkSum     = (*tpItr)->trackSummary();
      int numSi   =  trkSum->get(Trk::numberOfPixelHits)+trkSum->get(Trk::numberOfSCTHits);
      bool isTRT  =  numSi<4? true:false;
      double deta = isTRT? 0: (eta - eta_track);
      double dphi = m_phiHelper.diff(phi,phi_track);
      double dr   = sqrt(dphi*dphi + deta*deta);
      if(dr < m_drmax) {
        tmpTrkCol.push_back(*tpItr);
      }
    }
    
    if(tmpTrkCol.size()!=0) {// return conv_var;//continue;
    
      // Reconstruct conversion vertices
      tmpConversionContainer = m_VertexFinderTool->findVertex(tmpTrkCol.asDataVector());
      if (tmpConversionContainer->size()!=0) {//  return conv_var;//continue;
	
	VxContainer::iterator ic  = tmpConversionContainer->begin();
	VxContainer::iterator ice = tmpConversionContainer->end();
	for(; ic!=ice; ++ic) recVxCand.push_back(*ic);
	
	matchVxCand = matchConversionToCalo(egCalo, recVxCand);
	
	if(matchVxCand) {
	  Trk::VxCandidate*  myVxCandidate = matchVxCand->clone();
	  if (myVxCandidate->vxTrackAtVertex()->size()==1) {
	    conv_var.conv_flag = 11;
	  }
	  if (myVxCandidate->vxTrackAtVertex()->size()==2) {
	    conv_var.conv_flag = 12;
	  }
	  conv_var.conv_vtx = myVxCandidate;
	  conv_var.conv_R = sqrt(myVxCandidate->recVertex().position()[0]*myVxCandidate->recVertex().position()[0] + 
				 myVxCandidate->recVertex().position()[1]*myVxCandidate->recVertex().position()[1]);
	  conv_var.conv_vtx_chi2 = myVxCandidate->recVertex().fitQuality().chiSquared();
	  foundInC1C2extrapol=true;
	}
      }
    }
    /**  End of first step  **/
    /*************************/

    /********************************************************************************/
    /**  Second step : search for electrons with TRT standalone tracks             **/
    /**                only if first step is not successfull...                    **/
    /** MH 12/3/2014: we're skipping this part for now, since the xAOD migration breaks all the stuff below. **/

    if (foundInC1C2extrapol==false) {
      log << MSG::ERROR << "xAOD migration broke the search for electrons with TRT standalone tracks." << endreq;
    }
    /*
    const Trk::VxCandidate* convVx = NULL ;
    if (foundInC1C2extrapol==false) {
      const Rec::TrackParticle* track = electron->trackParticle();
      //bool hit_pixelBarrel0 = track->trackSummary()->isHit(Trk::pixelBarrel0);
      int nhits  = track->trackSummary()->get(Trk::numberOfPixelHits) + track->trackSummary()->get(Trk::numberOfSCTHits);
      if ( nhits==0 ) {//( !hit_pixelBarrel0 ){
	// Single track conversion
	log <<  MSG::DEBUG  << "Found TRT standalone single track conversion in electron container" << endreq;

	// to distinguish converted/unconverted photon using BDT method
	if (m_useBDT) {
	  if (BDTvalue(electron)>m_BDTCut) {//converted photon
	    conv_var.conv_flag = 21;
	    matchVxCand =  m_singleTrkConvTool->buildSingleTrackParticleConversion(track);   
	  }
	  else //unconverteed photon
	    conv_var.conv_flag = 20;
	}
	else {
	  conv_var.conv_flag = 21; //don't retrieve "unconverted photons"
	  matchVxCand =  m_singleTrkConvTool->buildSingleTrackParticleConversion(track);   
	}

	if(matchVxCand) {
	  Trk::VxCandidate*  myVxCandidate = matchVxCand->clone();
	  conv_var.conv_vtx = myVxCandidate;
	  conv_var.conv_R = sqrt(myVxCandidate->recVertex().position()[0]*myVxCandidate->recVertex().position()[0] + 
				 myVxCandidate->recVertex().position()[1]*myVxCandidate->recVertex().position()[1]);
	  conv_var.conv_vtx_chi2 = myVxCandidate->recVertex().fitQuality().chiSquared();
	}
	else {
	  conv_var.conv_vtx = convVx;
	  conv_var.conv_R=-9999.;
	  conv_var.conv_vtx_chi2 = -9999;
	}	  

	// Conversion radius extraction 
	// need to be changed so it takes into account beam spot position...
	// double p=0,theta=0,d0=0,phi0=0,Rc=0,xc=0,yc=0,A=0,B=0,Aprim=0,Bprim=0,Cprim=0,solution1=0,solution2=0;
	// bool hasSol = false;
	// p = double(1./(track->measuredPerigee()->parameters()[Trk::qOverP]));
	// theta = double(track->measuredPerigee()->parameters()[Trk::theta]);
	// d0 = double(track->measuredPerigee()->parameters()[Trk::d0]);
	// phi0 = double(track->measuredPerigee()->parameters()[Trk::phi0]);
	// //0.299792458=c/10^9 comes from h=c=1 convention...
	// Rc = double(fabs(p)*sin(theta)*1./(0.299792458*2));
	// if (p>0) {
	//   xc = double((Rc-d0)*sin(phi0));
	//   yc = double((Rc-d0)*cos(phi0));
	// }
	// if (p<0) {
	//   xc = double((Rc+d0)*sin(phi0));
	//   yc = double((Rc+d0)*cos(phi0));
	// }
	// A = xc+(yc*yc*1./xc)-(Rc*Rc*1./xc);
	// B = -yc*1./xc;
	// Aprim = B*B+1;
	// Bprim = 2*A*B-B*xc-yc;
	// Cprim = A*A-A*xc;
        // 
	// RealQuadEquation equation(Aprim,Bprim,Cprim);
	// hasSol = equation.hasSolution;
	// solution1 = equation.first;
	// solution2 = equation.second;
	// 
	// float first_hit_radius = (track->trackParameters())[0]->position().perp();
	// log <<  MSG::DEBUG  << "Extracted radius = " << sqrt(solution1*solution1+(A+B*solution1)*(A+B*solution1)) << " - first hit radius = " << first_hit_radius << endreq;
        // 
	// if (hasSol) conv_var.conv_R = sqrt(solution1*solution1+(A+B*solution1)*(A+B*solution1));
      }
    }
 */


  }

  return conv_var;
  
}


bool PAUconversionFlagTool::isConvInPhotonContainer(const PhotonContainer* photonContainer, const Trk::VxCandidate* convVx) {
  PhotonContainer::const_iterator photIt  = photonContainer->begin();
  PhotonContainer::const_iterator photItE = photonContainer->end();
  bool found = false;
  for ( ; photIt != photItE ; ++photIt ) {
    const Analysis::Photon* phot = *photIt ;
    const Trk::VxCandidate* vtx_conv_pho = phot->conversion();
    if (vtx_conv_pho && vtx_conv_pho==convVx) found = true;
    else continue;
  }
  if (found==true) return true;
  else return false;
}


float PAUconversionFlagTool::BDTvalue(const Analysis::Electron* electron) {
  /** Have to distinguish converted/unconverted photon using BDT method **/
  float BDT_value = -9999.;
  const EMShower* shower = electron->detail<EMShower>("egDetailAOD");
  const Rec::TrackParticle* track = electron->trackParticle() ;
  const CaloCluster* clus;
  clus = electron->cluster();
  if (shower && clus){
    float e_ps = 0.;
    /** energy in presampler **/
    if (electron->author()!=2) { //No cell for softe
      CaloCluster::cell_iterator cellIter    = clus->cell_begin();
      CaloCluster::cell_iterator cellIterEnd = clus->cell_end();
      for( ;cellIter!=cellIterEnd;cellIter++) {
	const CaloCell* cell = (*cellIter) ;
	Identifier id  = cell->ID();
	if ( m_calo_helper->calo_sample(id) == CaloCell_ID::PreSamplerB || m_calo_helper->calo_sample(id) == CaloCell_ID::PreSamplerE )
	  e_ps  += cell->energy() ; 
      }
    }
    else e_ps = -9999.;
    
    f1 = (shower)->parameter(egammaParameters::f1);
    if ((shower)->parameter(egammaParameters::weta1)>0) 
      weta1 = (shower)->parameter(egammaParameters::weta1);
    else 
      weta1 = 0.;
    if ((shower)->parameter(egammaParameters::wtots1)>0)
      wtots1 = (shower)->parameter(egammaParameters::wtots1);
    else 
      wtots1 = 0.;
    fracs1 = (shower)->parameter(egammaParameters::fracs1);
    weta2 = (shower)->parameter(egammaParameters::weta2);
    sumTRThits = float(track->trackSummary()->get(Trk::numberOfTRTHits) + track->trackSummary()->get(Trk::numberOfTRTHighThresholdHits));
    energy_ps = e_ps;
    
    BDT_value = m_reader->EvaluateMVA( "BDT method");
  }

  return BDT_value;

}






/////////////////////////////////////////////////////////////////////////////////////////////////
//* Search for the best matched conversion 
const Trk::VxCandidate* PAUconversionFlagTool::matchConversionToCalo(const CaloCluster* egCalo, 
                                                                std::vector<const Trk::VxCandidate*> vtxVect ) 
{
  const Trk::VxCandidate*  tmpVxCand(0);
  
  if(vtxVect.size()==0)  return tmpVxCand;

  unsigned int numOfDT =0;
  
  double bestR   = 10000.;
  //double bestPt  = -1.;
  
  std::vector<const Trk::VxCandidate*>::iterator  ivtx = vtxVect.begin();
  for(; ivtx!=vtxVect.end(); ++ivtx) {
    
    double currentR       = 10000.;
    double currentPt      = -1.;
    
    unsigned int numTrackAtVertex = (*ivtx)->vxTrackAtVertex()->size();
    if(numTrackAtVertex <1 ||numTrackAtVertex >2 )    continue;
    
    std::vector<const Rec::TrackParticle*> recTrackParticle(0);
    
    for( unsigned int counter =0; counter < numTrackAtVertex; ++counter) {
      
      Trk::VxTrackAtVertex* trackAtVtx = (*(*ivtx)->vxTrackAtVertex())[counter];
      
      const Trk::ITrackLink*  trLink = trackAtVtx->trackOrParticleLink();
      const Trk::TrackParticleBase* tempTrkPB(0);
      
      if(trLink!=0) {
        const Trk::LinkToTrackParticleBase* linkToTrackPB = dynamic_cast<const Trk::LinkToTrackParticleBase*>(trLink);
        if(linkToTrackPB!=0) {
          if(linkToTrackPB->isValid())  tempTrkPB = linkToTrackPB->cachedElement();
        }
      }
      if(tempTrkPB) {
        //* Dynamic_cast TrackParticleBase to TrackParticle
        const Rec::TrackParticle* tpTrk  = dynamic_cast<const Rec::TrackParticle*>(tempTrkPB);
        recTrackParticle.push_back(tpTrk);
      }
      
    }
    
    if( recTrackParticle.size()!= numTrackAtVertex)  continue;  //* next VxCandidate

    bool isMatch = false;
    
    std::vector<const Rec::TrackParticle*>::iterator  itrk = recTrackParticle.begin();
    if(numTrackAtVertex==1) {
      const Rec::TrackParticle* tmpTP = *itrk;
      const Trk::TrackSummary*  trkSum     = tmpTP->trackSummary();
      int numSi   =  trkSum->get(Trk::numberOfPixelHits)+trkSum->get(Trk::numberOfSCTHits);
      bool isTRT  =  numSi<4? true:false;
 
      isMatch = extrapolateToCalo(egCalo, tmpTP, isTRT);

      
    }else if(numTrackAtVertex==2) {
      
      const Rec::TrackParticle* tmp1TP = *itrk;
      const Rec::TrackParticle* tmp2TP = *(++itrk);
      
      const Trk::TrackSummary*  trkSum1= tmp1TP->trackSummary();
      const Trk::TrackSummary*  trkSum2= tmp2TP->trackSummary();
      
      int nS1    = trkSum1->get(Trk::numberOfPixelHits)+trkSum1->get(Trk::numberOfSCTHits);
      int nS2    = trkSum2->get(Trk::numberOfPixelHits)+trkSum2->get(Trk::numberOfSCTHits);
      bool isTRT = (nS1<4 || nS2<4)? true:false;
      
      //* Require fitted perigee parameters at vertex
      const Trk::NeutralPerigee* phPar = createPhotonParameters(*ivtx);
      isMatch = extrapolateToCalo(egCalo, phPar, isTRT);
    }
    
    if(isMatch) { 
      selectBestConversion(*ivtx, currentR, currentPt);
      if(currentR < bestR) {
        //if(currentPt > bestPt) {
        if(numTrackAtVertex==2 || (numOfDT==0 && numTrackAtVertex==1)) {
          bestR = currentR; tmpVxCand = *ivtx; //bestPt = currentPt;
        }
      }
    }
  }
  return tmpVxCand;
  
  
}



/////////////////////////////////////////////////////////////////////////////////////////////
//* Extrapolate TrackParticle to electron CaloCluster
/////////////////////////////////////////////////////////////////////////////////////////////

bool PAUconversionFlagTool::extrapolateToCalo(const CaloCluster* egCalo, const Rec::TrackParticle* trkTB, bool isTRT){

  double          deltaEta   = -999.;
  double          deltaPhi   = -999.;

  const Trk::TrackParameters* result   = 0;
  double offset = 0.;
  CaloCell_ID::CaloSample sample;

  //* From the last measurement on the track 
  const Trk::TrackParameters* TrkPar = 0;
  const std::vector<const Trk::TrackParameters*> vecPar = trkTB->trackParameters();
  double rMax = 0.;
  for(int i=0; i<int(vecPar.size()); ++i) {
   if(vecPar[i]->position().perp()>rMax) {
      rMax = vecPar[i]->position().perp();
      TrkPar = vecPar[i];
    }
  }
  
  if ( (egCalo->inBarrel()  && !egCalo->inEndcap()) ||
       (egCalo->inBarrel()  &&  egCalo->inEndcap() &&
        egCalo->eSample(CaloSampling::EMB2) > egCalo->eSample(CaloSampling::EME2) ) ) {
    sample  = CaloCell_ID::EMB2;
    result = m_extrapolateToCaloTool->extrapolate( *TrkPar, sample, offset, Trk::nonInteracting, Trk::alongMomentum);

    if (result) {
      deltaEta = egCalo->etaBE(2) - result->position().eta();
      deltaPhi = m_phiHelper.diff(egCalo->phiBE(2),result->position().phi());
    }
  }else if( ( !egCalo->inBarrel()  && egCalo->inEndcap() ) ||
            (  egCalo->inBarrel()  && egCalo->inEndcap() &&
               egCalo->eSample(CaloSampling::EME2) > egCalo->eSample(CaloSampling::EMB2) ) ) {
    
    sample = CaloCell_ID::EME2;
    result = m_extrapolateToCaloTool->extrapolate( *TrkPar, sample, offset, Trk::nonInteracting, Trk::alongMomentum);
    // result = m_extrapolateToCaloTool->extrapolate( *tmpTrack, sample, offset);  
    if (result) {
      deltaEta = egCalo->etaBE(2) - result->position().eta();
      deltaPhi = m_phiHelper.diff(egCalo->phiBE(2),result->position().phi());
    }
  }
  
  if(result) delete result;
  else       return false;

  // asymmetric deltaPhi: define sign such that brem tail is on -ve side
  if(TrkPar->charge()>0)  deltaPhi = -deltaPhi;

  if((fabs(deltaEta)< m_narrowDeltaEta || isTRT) && deltaPhi < m_narrowDeltaPhi && deltaPhi > -m_narrowDeltaPhiBrem) 
    return true;
  
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//* Extrapolate double-track conversion with the neutral perigee
/////////////////////////////////////////////////////////////////////////////////////////////

bool PAUconversionFlagTool::extrapolateToCalo(const CaloCluster* egCalo, const Trk::NeutralPerigee* phPerigee, bool isTRT){

  double          deltaEta   = -999.;
  double          deltaPhi   = -999.;

  const Trk::NeutralParameters* result   = 0;
  double offset = 0.;
  CaloCell_ID::CaloSample sample;

     
  if ( (egCalo->inBarrel()  && !egCalo->inEndcap()) ||
       (egCalo->inBarrel()  &&  egCalo->inEndcap() &&
        egCalo->eSample(CaloSampling::EMB2) > egCalo->eSample(CaloSampling::EME2) ) ) {
    sample  = CaloCell_ID::EMB2;
    //      result = m_extrapolateToCaloTool->extrapolate( PerigeeParameters, sample, offset, Trk::electron);   
    result = m_extrapolateToCaloTool->extrapolate(*phPerigee, sample, offset, Trk::photon, Trk::alongMomentum);
    if (result) {
      deltaEta = egCalo->etaBE(2) - result->position().eta();
      deltaPhi = m_phiHelper.diff(egCalo->phiBE(2),result->position().phi());
    }
  }else if( ( !egCalo->inBarrel()  && egCalo->inEndcap() ) ||
            (  egCalo->inBarrel()  && egCalo->inEndcap() &&
               egCalo->eSample(CaloSampling::EME2) > egCalo->eSample(CaloSampling::EMB2) ) ) {
    
    sample = CaloCell_ID::EME2;
    //      result = m_extrapolateToCaloTool->extrapolate( PerigeeParameters, sample, offset, Trk::electron);  
    result = m_extrapolateToCaloTool->extrapolate(*phPerigee, sample, offset,Trk::photon, Trk::alongMomentum);
    if (result) {
      deltaEta = egCalo->etaBE(2) - result->position().eta();
      deltaPhi = m_phiHelper.diff(egCalo->phiBE(2),result->position().phi());
    }
  }
  if(result) delete result;
  else       return false;

  if ((fabs(deltaEta) < m_narrowDeltaEta || isTRT) && fabs(deltaPhi) < m_narrowDeltaPhi) 
    return true;
  
  return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Create NeutralPerigee 
/////////////////////////////////////////////////////////////////////////////////////////////
const Trk::NeutralPerigee* PAUconversionFlagTool::createPhotonParameters(const Trk::VxCandidate *myCandidate )
{
  const double pi2 = 2.*M_PI, pi=M_PI;
  
  const Trk::NeutralPerigee* phPar = 0;

  const std::vector<Trk::VxTrackAtVertex*> trkAtVx(*(myCandidate->vxTrackAtVertex()));
  
  if(int(trkAtVx.size())==2){
    const Trk::TrackParameters* perigee1 = trkAtVx[0]->perigeeAtVertex();
    const Trk::TrackParameters* perigee2 = trkAtVx[1]->perigeeAtVertex();
    if(perigee1 && perigee2){
      HepLorentzVector momentum;
      Amg::Vector3D sum_mom = perigee1->momentum() + perigee2->momentum();
      double m2 = pow(PAUconversionFlagTool::s_particleMasses.mass[Trk::electron],2);
      double ee = sqrt(m2 +  perigee1->momentum().mag2()) + sqrt(m2 +  perigee2->momentum().mag2());
      momentum.setPx(sum_mom.x()); momentum.setPy(sum_mom.y()); momentum.setPz(sum_mom.z()); momentum.setE(ee);
    
      //Get the photon inverse momentum
      double qP = sqrt(sum_mom.x()*sum_mom.x() + sum_mom.y()*sum_mom.y() + sum_mom.z()*sum_mom.z());
      double inv_qP = 0.;
      if(qP!=0.) inv_qP = 1./qP;
    
      //Get the photon phi and theta
      double qPt = sqrt(sum_mom.x()*sum_mom.x() + sum_mom.y()*sum_mom.y());
      double ph_phi   = atan2(sum_mom.y(),sum_mom.x());
      double ph_theta = atan2(qPt,sum_mom.z());
    
      //Protect for theta and phi being out of range
      if     (ph_theta > pi) ph_theta = fmod(ph_theta+pi,pi2)-pi;
      else if(ph_theta <-pi) ph_theta = fmod(ph_theta-pi,pi2)+pi;
      if(ph_theta<0.){ ph_theta = -ph_theta; ph_phi+=pi; }
      if     (ph_phi > pi) ph_phi = fmod(ph_phi+pi,pi2)-pi;
      else if(ph_phi <-pi) ph_phi = fmod(ph_phi-pi,pi2)+pi;
    
      //VxCandidate global position
      const Trk::RecVertex & vertex = myCandidate->recVertex();
      double vx = vertex.position().x();
      double vy = vertex.position().y();
      double vz = vertex.position().z();
      Amg::Vector3D gp(vx,vy,vz);
    
      const AmgSymMatrix(5)* cov1 = perigee1->covariance();
      const AmgSymMatrix(5)* cov2 = perigee2->covariance();
      AmgSymMatrix(5) cov =  (*cov1) + (*cov2);

      //Construct the photon neutral parameters as neutral perigee
      const Trk::NeutralPerigee* neutPar = new Trk::NeutralPerigee(gp,ph_phi,ph_theta,inv_qP,gp,&cov);
      phPar = neutPar;
    }
  }
  
  return phPar;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Select the best matched conversion
// The same logic as it is used in EMConversionBuilder
/////////////////////////////////////////////////////////////////////////////////////////////

void PAUconversionFlagTool::selectBestConversion(const Trk::VxCandidate * myCandidate, double& rad, double& p){
  MsgStream mlog(msgSvc(), name());
  
  //double reducedChi2 = 10000.;
  double vtxRad      = 10000.;
  //double inv_mass    = 10000.;
  double photonP     = -1000.;
  
  const std::vector<Trk::VxTrackAtVertex*> trkAtVx(*(myCandidate->vxTrackAtVertex()));
  
  const Trk::RecVertex & vertex = myCandidate->recVertex();
  //const Trk::FitQuality & theFitQuality=vertex.fitQuality() ;
  //reducedChi2 = theFitQuality.chiSquared()/theFitQuality.numberDoF();
  vtxRad = vertex.position().perp();
  
  //invariant mass, photon pT
  if(int(trkAtVx.size())==2){
    const Trk::TrackParameters* perigee1 = trkAtVx[0]->perigeeAtVertex();
    const Trk::TrackParameters* perigee2 = trkAtVx[1]->perigeeAtVertex();
    HepLorentzVector momentum;
    Amg::Vector3D sum_mom = perigee1->momentum() + perigee2->momentum();
    double m2 = pow(PAUconversionFlagTool::s_particleMasses.mass[Trk::electron],2);
    double ee = sqrt(m2 +  perigee1->momentum().mag2()) + sqrt(m2 +  perigee2->momentum().mag2());
    momentum.setPx(sum_mom.x()); momentum.setPy(sum_mom.y()); momentum.setPz(sum_mom.z()); momentum.setE(ee);
    //inv_mass = momentum.m();
    photonP = sqrt(momentum.x()*momentum.x() + momentum.y()*momentum.y());
  }else{
    const Trk::TrackParameters* perigee1 = trkAtVx[0]->perigeeAtVertex();
    Amg::Vector3D mom = perigee1->momentum();
    photonP = sqrt(mom.x()*mom.x() + mom.y()*mom.y());
  }
  
  rad  = vtxRad; p = photonP;
}



//   Trk::VxCandidate* PAUconversionFlagTool::buildSingleTrackParticleConversion(const Trk::TrackParticleBase* track)
//   {	
//     ///Create a RecVertex at the first measurement of the track.
//     const Trk::ParametersBase* trkPar = 0;
//     const std::vector<const Trk::ParametersBase*> vecPar = track->trackParameters();
//     
//     for(int i=0; i<int(vecPar.size()); ++i) {
//       if(vecPar[i]->position().perp()<10.)   continue; 
//       trkPar = vecPar[i];
//       break;
//     }
//     
//     const Trk::MeasuredTrackParameters* mtp = dynamic_cast<const Trk::MeasuredTrackParameters*>(trkPar); if(!mtp) return 0;
//     const Trk::TrackParameters* tp = dynamic_cast<const Trk::TrackParameters*>(trkPar); if(!tp) return 0;
//     const Trk::GlobalPosition* gp  = &(trkPar->position());
//     const Trk::ErrorMatrix* em     = &(mtp->localErrorMatrix());
//     double chi2 = track->fitQuality()->chiSquared();
//     int Ndf     = track->fitQuality()->numberDoF();
//     
//     ///Need to compute a global position covariance matrix as J.C.JT
//     const HepTransform3D& T = trkPar->associatedSurface()->transform();
//     HepSymMatrix nCovVtx(3);
//     if(gp->perp() < 550.) { //Plane surface
//       ///The local position parameters covariance matrix C (2x2)
//       double p11 = em->covValue(Trk::locX);
//       double p12 = em->covValue(Trk::locX, Trk::locY);
//       double p21 = em->covValue(Trk::locY, Trk::locX);
//       double p22 = em->covValue(Trk::locY);
//       
//       ///The Jacobian matrix J (3x2)
//       double Ax[3] = {T(0,0),T(1,0),T(2,0)};
//       double Ay[3] = {T(0,1),T(1,1),T(2,1)};
//       double a11 = Ax[0]; double a12 = Ay[0];
//       double a21 = Ax[1]; double a22 = Ay[1];
//       double a31 = Ax[2]; double a32 = Ay[2];
//       
//       ///The A = J.C (3x2)
//       double A11 = a11*p11 + a12*p21; double A12 = a11*p12 + a12*p22;
//       double A21 = a21*p11 + a22*p21; double A22 = a21*p12 + a22*p22;
//       double A31 = a31*p11 + a32*p21; double A32 = a31*p12 + a32*p22;
//       
//       ///The A.JT = J.C.JT (3x3)
//       double P11 = a11*A11 + A12*a12; double P12 = A11*a21 + A12*a22; double P13 = A11*a31 + A12*a32;
//       double P21 = A21*a11 + A22*a12; double P22 = A21*a21 + A22*a22; double P23 = A21*a31 + A22*a32;
//       double P31 = A31*a11 + A32*a12; double P32 = A31*a21 + A32*a22; double P33 = A31*a31 + A32*a32;
//       
//       ///Construct the new covariance matrix (3x3)
//       nCovVtx(1,1) = P11; nCovVtx(1,2) = P12; nCovVtx(1,3) = P13;
//       nCovVtx(2,1) = P21; nCovVtx(2,2) = P22; nCovVtx(2,3) = P23;
//       nCovVtx(3,1) = P31; nCovVtx(3,2) = P32; nCovVtx(3,3) = P33;
//       //std::cout << "COVARIANCE MATRIX ACCORDING TO IGOR " << nCovVtx << std::endl;
//     } else {//Straight line surface
//       ///The local position parameters covariance matrix C (2x2)
//       double p11 = em->covValue(Trk::locR);
//       double p12 = em->covValue(Trk::locR, Trk::locZ);
//       double p21 = em->covValue(Trk::locZ, Trk::locR);
//       double p22 = em->covValue(Trk::locZ);
//       
//       ///The straight line surface (wire) global directions
//       double A[3] = {T(0,2),T(1,2),T(2,2)};
//       
//       ///The particle global direction
//       double Px = (tp->cosPhi()) * (tp->sinTheta());
//       double Py = (tp->sinPhi()) * (tp->sinTheta());
//       double Pz = tp->cosTheta();
//       
//       ///The Jacobian matrix J (3x2)
//       double Bx = A[1]*Pz-A[2]*Py;
//       double By = A[2]*Px-A[0]*Pz;
//       double Bz = A[0]*Py-A[1]*Px;
//       double Bn = 1./sqrt(Bx*Bx+By*By+Bz*Bz); Bx*=Bn; By*=Bn; Bz*=Bn;
//       double a11 = Bx; double a12 = A[0];
//       double a21 = By; double a22 = A[1];
//       double a31 = Bz; double a32 = A[2];
//       
//       ///The A = J.C (3x2)
//       double A11 = a11*p11 + a12*p21; double A12 = a11*p12 + a12*p22;
//       double A21 = a21*p11 + a22*p21; double A22 = a21*p12 + a22*p22;
//       double A31 = a31*p11 + a32*p21; double A32 = a31*p12 + a32*p22;
//       
//       ///The A.JT = J.C.JT (3x3)
//       double P11 = a11*A11 + A12*a12; double P12 = A11*a21 + A12*a22; double P13 = A11*a31 + A12*a32;
//       double P21 = A21*a11 + A22*a12; double P22 = A21*a21 + A22*a22; double P23 = A21*a31 + A22*a32;
//       double P31 = A31*a11 + A32*a12; double P32 = A31*a21 + A32*a22; double P33 = A31*a31 + A32*a32;
//       
//       ///Construct the new covariance matrix (3x3)
//       nCovVtx(1,1) = P11; nCovVtx(1,2) = P12; nCovVtx(1,3) = P13;
//       nCovVtx(2,1) = P21; nCovVtx(2,2) = P22; nCovVtx(2,3) = P23;
//       nCovVtx(3,1) = P31; nCovVtx(3,2) = P32; nCovVtx(3,3) = P33;
//       //std::cout << "COVARIANCE MATRIX ACCORDING TO IGOR " << nCovVtx << std::endl;
//     } 
//     
//     HepSymMatrix CovMtx(3);
//     CovMtx(1,1) = em->covValue(Trk::phi);
//     CovMtx(1,2) = em->covValue(Trk::phi, Trk::theta);
//     CovMtx(1,3) = em->covValue(Trk::phi, Trk::qOverP);
//     CovMtx(2,1) = em->covValue(Trk::phi, Trk::theta);
//     CovMtx(2,2) = em->covValue(Trk::theta);
//     CovMtx(2,3) = em->covValue(Trk::theta, Trk::qOverP);
//     CovMtx(3,1) = em->covValue(Trk::phi, Trk::qOverP);
//     CovMtx(3,2) = em->covValue(Trk::theta, Trk::qOverP);
//     CovMtx(3,3) = em->covValue(Trk::qOverP);
//     
//     Trk::RecVertex* tmpRecV = new Trk::RecVertex(*gp , Trk::ErrorMatrix(new Trk::CovarianceMatrix(nCovVtx)), Ndf, chi2 );
//     
//     ///Create the corresponding vector of tracks at that RecVertex. Contains one track with a new redefined measured perigee.
//     std::vector<Trk::VxTrackAtVertex*>* tmpVTAV = new std::vector<Trk::VxTrackAtVertex*>();
//     const HepVector& iv = tp->parameters();
//     Trk::MeasuredPerigee * tmpMeasPer  =  new Trk::MeasuredPerigee( 0.,0.,iv[2],
//                                                                     iv[3],
//                                                                     iv[4],
//                                                                     *gp,
//                                                                     new Trk::ErrorMatrix(new Trk::CovarianceMatrix(CovMtx)));
//     Trk::VxTrackAtVertex* trkV = new Trk::VxTrackAtVertex(1., tmpMeasPer);
//     
//     Trk::LinkToTrackParticleBase * linkTT = new Trk::LinkToTrackParticleBase();
//     linkTT->setElement(track);
//     trkV->setOrigTrack(linkTT);
//     tmpVTAV->push_back(trkV);
//     
//     ///Create the VxCandidate by combining the RecVertex and the vector of TrackAtVertex.
//     Trk::VxCandidate* vx = new Trk::VxCandidate(*tmpRecV,*tmpVTAV);
//     delete  tmpRecV;        delete tmpVTAV;
//     return  vx;
//   }
