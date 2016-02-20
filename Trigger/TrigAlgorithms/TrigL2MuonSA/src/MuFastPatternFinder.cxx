/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/MuFastPatternFinder.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonCalibEvent/MdtCalibHit.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "Identifier/Identifier.h"

#include "xAODTrigMuon/TrigMuonDefs.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_MuFastPatternFinder("IID_MuFastPatternFinder", 1, 0);

const InterfaceID& TrigL2MuonSA::MuFastPatternFinder::interfaceID() { return IID_MuFastPatternFinder; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastPatternFinder::MuFastPatternFinder(const std::string& type, 
						     const std::string& name,
						     const IInterface*  parent): 
   AthAlgTool(type,name,parent),
   m_msg(0),
   m_mdtCalibrationSvc(0)
{
   declareInterface<TrigL2MuonSA::MuFastPatternFinder>(this);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastPatternFinder::~MuFastPatternFinder() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastPatternFinder::initialize()
{
   // Get a message stream instance
   m_msg = new MsgStream( msgSvc(), name() );
   msg() << MSG::DEBUG << "Initializing MuFastPatternFinder - package version " << PACKAGE_VERSION << endreq ;
   
   StatusCode sc;
   sc = AthAlgTool::initialize();
   if (!sc.isSuccess()) {
      msg() << MSG::ERROR << "Could not initialize the AthAlgTool base class." << endreq;
      return sc;
   }
   
   // retrieve the mdtidhelper  
   StoreGateSvc* detStore(0);                                                                                                                     sc = serviceLocator()->service("DetectorStore", detStore); 
   if (sc.isFailure()) { 
     msg() << MSG::ERROR << "Could not retrieve DetectorStore." << endreq; 
     return sc;
   } 
   msg() << MSG::DEBUG << "Retrieved DetectorStore." << endreq;
   const MuonGM::MuonDetectorManager* muonMgr;                                                                 
   sc = detStore->retrieve( muonMgr,"Muon" ); 
   if (sc.isFailure()) return sc; 
   msg() << MSG::DEBUG << "Retrieved GeoModel from DetectorStore." << endreq; 
   m_mdtIdHelper = muonMgr->mdtIdHelper();                                                                                                    

   // Locate MDT calibration service
   sc = serviceLocator()->service("MdtCalibrationSvc", m_mdtCalibrationSvc );
   if(sc.isFailure()) {
      msg() << MSG::WARNING << "Unable to retrieve the MDT calibration Service" << endreq;
      msg() << MSG::WARNING << "Proceed using dummy calibration for MDT" << endreq;
   }
   
   // 
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MuFastPatternFinder::doMdtCalibration(TrigL2MuonSA::MdtHitData& mdtHit, double track_phi, double phi0, bool isEndcap)
{
   // if was error in getting servce,
   if( ! m_mdtCalibrationSvc ) return;

   // 
   int StationName  = mdtHit.name;
   int StationEta   = mdtHit.StationEta;
   int StationPhi   = mdtHit.StationPhi;
   int Multilayer   = mdtHit.Multilayer;
   int Layer        = mdtHit.TubeLayer;
   int Tube         = mdtHit.Tube;

   msg() << MSG::DEBUG << "...StationName/StationEta/StationPhi/Multilayer/Layer/Tube="
	 << StationName << "/" << StationEta << "/" << StationPhi << "/" << Multilayer << "/"
	 << Layer << "/" << Tube << endreq;

   Identifier id = m_mdtIdHelper->channelID(StationName,StationEta,
					    StationPhi,Multilayer,Layer,Tube);

   int tdcCounts    = (int)mdtHit.DriftTime;
   int adcCounts    = mdtHit.Adc;

   double R    = mdtHit.R;
   //   double InCo = mdtHit.cInCo;
   double InCo = cos(fabsf(track_phi - phi0))!=0 ? 1./(cos(fabsf(track_phi - phi0))): 0; 
   double X    = (isEndcap)? R*cos(track_phi): R*InCo*cos(track_phi);
   double Y    = (isEndcap)? R*sin(track_phi): R*InCo*sin(track_phi);
   double Z    = mdtHit.Z;
   const Amg::Vector3D point(X,Y,Z);
   const Amg::Vector3D point0(0.,0.,0.);

   MdtCalibHit calHit(id, tdcCounts, adcCounts, point, 0 );
   // Need to overwrite uninitialised variables
   calHit.setLocalPos(point0);
   calHit.setLocalPointOfClosestApproach(point0);
   calHit.setGlobalPointOfClosestApproach(point0);
   calHit.setDriftTime(0.);
   calHit.setDriftRadius(0., 0.);
   calHit.setDistanceToTrack(0., 0.);
   calHit.setTimeFromTrackDistance(0., 0.);
   calHit.setDistanceToReadout(0.);
   calHit.setBFieldPerp(0.);
   calHit.setBFieldPara(0.);
   calHit.setTemperature(0.);
   calHit.setLocXtwin(0.);
   calHit.setSigma2LocXtwin(0.);

   msg() << MSG::DEBUG << "... MDT hit raw digit tdcCounts/adcCounts=" << tdcCounts << "/" << adcCounts << endreq;

   msg() << MSG::DEBUG << "... MDT hit position X/Y/Z/track_phi/Multilayer/Layer/Tube="
	 << X << "/" << Y << "/" << Z << "/" << track_phi << "/" << Multilayer << "/" << Layer << "/" << Tube << endreq;

   m_mdtCalibrationSvc->driftRadiusFromTime( calHit, point.mag() );
   double driftSpace = calHit.driftRadius();
   double driftSigma = calHit.sigmaDriftRadius();

   msg() << MSG::DEBUG << "... MDT hit calibrated driftSpace/driftSigma=" << driftSpace << "/" << driftSigma << endreq;

   const double ZERO_LIMIT = 1e-4;
   
   if( fabs(driftSpace) > ZERO_LIMIT ) {
      mdtHit.DriftSpace = driftSpace;
      mdtHit.DriftSigma = driftSigma;
   }
   else {
      mdtHit.DriftSpace = 0;
      mdtHit.DriftSigma = 0;
   }

}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastPatternFinder::findPatterns(const TrigL2MuonSA::MuonRoad&            muonRoad,
							   TrigL2MuonSA::MdtHits&                   mdtHits,
							   std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns)
{

   // find only 1 track pattern
   v_trackPatterns.clear();
   TrigL2MuonSA::TrackPattern trackPattern;
   for (int i=0; i<xAOD::L2MuonParameters::Chamber::MaxChamber; i++) trackPattern.mdtSegments[i].clear();

   // Saddress = pos1/3 = 0:Large, 1:Large-SP, 2:Small, 3:Small-SP
   trackPattern.s_address = (muonRoad.isEndcap)? -1: muonRoad.Special + 2*muonRoad.LargeSmall;

   const unsigned int MAX_STATION =  11;
   const unsigned int MAX_LAYER   =  12;

   TrigL2MuonSA::MdtLayerHits v_mdtLayerHits[MAX_STATION][MAX_LAYER];

   for(unsigned int i_station=0; i_station<MAX_STATION; i_station++) {
      for(unsigned int i_layer=0; i_layer<MAX_LAYER; i_layer++) {
	 v_mdtLayerHits[i_station][i_layer].ntot       = 0;
	 v_mdtLayerHits[i_station][i_layer].ntot_all   = 0;
	 v_mdtLayerHits[i_station][i_layer].ndigi      = 0;
	 v_mdtLayerHits[i_station][i_layer].ndigi_all  = 0;
	 v_mdtLayerHits[i_station][i_layer].ResSum     = 0.;
      }
   }

   unsigned int i_layer_max   = 0;
   unsigned int chamber_max = 0;

   double trigger_phi = muonRoad.phiMiddle; // phi fit result to be used
   double phi0; // phi at the first layer to be used

   for(unsigned int i_hit=0; i_hit<mdtHits.size(); i_hit++) {

     unsigned int chamber = mdtHits[i_hit].Chamber;

     if( chamber > chamber_max ) chamber_max = chamber;
     if (chamber >= xAOD::L2MuonParameters::Chamber::MaxChamber) continue;

     double aw = muonRoad.aw[chamber][0];
     double bw = muonRoad.bw[chamber][0];
     double Z = mdtHits[i_hit].Z;
     double R = mdtHits[i_hit].R;
     //
     double residual = calc_residual(aw,bw,Z,R);
     mdtHits[i_hit].R        = R;
     mdtHits[i_hit].Residual = residual;
     mdtHits[i_hit].isOutlier = 0;
     unsigned int i_layer = mdtHits[i_hit].Layer;
     double rWidth = muonRoad.rWidth[chamber][i_layer];

     msg() << MSG::DEBUG << "... chamber/Z/R/aw/bw/residual/rWidth="
	   << chamber << "/" << Z << "/" << R << "/" << aw << "/" << bw << "/" << residual << "/" << rWidth << endreq;
     
     if( fabs(residual) > rWidth ) {
       mdtHits[i_hit].isOutlier   = 2;
       continue;
     }

     msg() << MSG::DEBUG << " --> included at i_layer=" << i_layer << endreq;
     if( mdtHits[i_hit].TubeLayer < 1 || i_layer > (MAX_LAYER-1) ) {
       msg() << MSG::WARNING << "strange i_layer=" << i_layer << endreq;
       return StatusCode::FAILURE;
     }
     if( i_layer > i_layer_max ) i_layer_max = i_layer;
     //      v_mdtLayerHits[i_station][i_layer].indexes[v_mdtLayerHits[i_station][i_layer].ndigi] = i_hit;
     v_mdtLayerHits[chamber][i_layer].indexes.push_back(i_hit);
     v_mdtLayerHits[chamber][i_layer].ndigi++;
     v_mdtLayerHits[chamber][i_layer].ndigi_all++;
     v_mdtLayerHits[chamber][0].ntot++;
     v_mdtLayerHits[chamber][0].ntot_all++;
     v_mdtLayerHits[chamber][0].ResSum += residual;
   }
   

   const double DeltaMin = 0.025;

   for(unsigned int chamber=0; chamber<=chamber_max; chamber++) {
     
     msg() << MSG::DEBUG << " --- chamber=" << chamber << endreq;
     
     double ResMed = 0;
     
     msg() << MSG::DEBUG << "removing outliers..." << endreq;
     
     // remove outlier
     while(1) {
       if (chamber==9) break;//BME skips this loop
       unsigned int layer = 999999;
       double DistMax  = 0.;
       double Residual = 0.;
       unsigned int i_hit_max = 999999;
       ResMed = (v_mdtLayerHits[chamber][0].ntot!=0)? 
	 v_mdtLayerHits[chamber][0].ResSum/v_mdtLayerHits[chamber][0].ntot : 0.;
       for(unsigned int i_layer=0; i_layer<=i_layer_max; i_layer++) {
	 for(unsigned int idigi=0; idigi<v_mdtLayerHits[chamber][i_layer].ndigi_all;idigi++) {
	   
	   unsigned int i_hit = v_mdtLayerHits[chamber][i_layer].indexes[idigi];
	   if(mdtHits[i_hit].isOutlier > 0) continue;
	   double DistMed = fabs(mdtHits[i_hit].Residual - ResMed);
	   if(DistMed>=DistMax) {
	     DistMax   = DistMed;
	     Residual  = mdtHits[i_hit].Residual;
	     layer     = i_layer;
	     i_hit_max = i_hit;
	   }
	 }
       }
       msg() << MSG::DEBUG << "ResMed=" << ResMed << ": DistMax/layer/i_hit_max/ntot=" 
	     << DistMax << "/" << layer << "/" << i_hit_max << "/" << v_mdtLayerHits[chamber][0].ntot << endreq;
       // break conditions
       if(layer == 999999) break;
       if(v_mdtLayerHits[chamber][layer].ndigi==1) break;
       double Mednew = (v_mdtLayerHits[chamber][0].ResSum - Residual)/(v_mdtLayerHits[chamber][0].ntot - 1);
       double Delta = 2.*fabs((ResMed - Mednew)/(ResMed + Mednew));
       msg() << MSG::DEBUG << "Mednew/Delta/DeltaMin=" << Mednew << "/" << Delta << "/" << DeltaMin << endreq;
       if(Delta<=DeltaMin) break;
       
       // if not, delete the maxRes and continue;
       v_mdtLayerHits[chamber][0].ResSum = v_mdtLayerHits[chamber][0].ResSum - Residual;
       v_mdtLayerHits[chamber][0].ntot--;
       v_mdtLayerHits[chamber][layer].ndigi--;
       mdtHits[i_hit_max].isOutlier = 2;
     }
     
     msg() << MSG::DEBUG << "choosing one at each layer..." << endreq;
     
     // choose one at each layer, and record it in segment
      TrigL2MuonSA::MdtHits mdtSegment;
      mdtSegment.clear();

      phi0 = 0;
      for(unsigned int i_layer=0; i_layer<=i_layer_max; i_layer++) {
	
	msg() << MSG::DEBUG << "i_layer=" << i_layer << ": ndigi=" << v_mdtLayerHits[chamber][i_layer].ndigi << endreq;
	
	// choose one at each layer
	while( v_mdtLayerHits[chamber][i_layer].ndigi>=2 ) {
	  double ResMax = 0.;
	  unsigned int i_hit_max = 999999;
	  for(unsigned int idigi=0;idigi<v_mdtLayerHits[chamber][i_layer].ndigi_all;idigi++) {
	    unsigned int i_hit = v_mdtLayerHits[chamber][i_layer].indexes[idigi];
	    if( mdtHits[i_hit].isOutlier > 0 ) continue;
	    if( fabs(mdtHits[i_hit].Residual) >= ResMax ) {
	      ResMax = fabs(mdtHits[i_hit].Residual);
	      i_hit_max = i_hit;
	    }
	  }
	  msg() << MSG::DEBUG << "ResMax=" << ResMax << ": i_hit_max=" << i_hit_max << endreq; 
	  if( i_hit_max == 999999 ) break;
	  v_mdtLayerHits[chamber][0].ResSum = v_mdtLayerHits[chamber][0].ResSum - ResMax;
	  v_mdtLayerHits[chamber][0].ntot--;
	  v_mdtLayerHits[chamber][i_layer].ndigi--;
	  mdtHits[i_hit_max].isOutlier = 1;
	}
	
	// record it into segement
	for(unsigned int i_digi=0; i_digi< v_mdtLayerHits[chamber][i_layer].ndigi_all; i_digi++) {
	  unsigned int i_hit = v_mdtLayerHits[chamber][i_layer].indexes[i_digi];
	  if (i_layer==0) phi0 = mdtHits[i_hit].cPhi0;
	  doMdtCalibration(mdtHits[i_hit], trigger_phi, phi0, muonRoad.isEndcap);
	  if (mdtHits[i_hit].isOutlier > 1) continue;
	  mdtSegment.push_back(mdtHits[i_hit]);
	}
      }
      
      //
      msg() << MSG::DEBUG << "nr of hits in segment=" << mdtSegment.size() << endreq;
      trackPattern.mdtSegments[chamber] = mdtSegment;
      
   } // end loop on stations.
   
   v_trackPatterns.push_back(trackPattern);
   
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonSA::MuFastPatternFinder::calc_residual(double aw,double bw,double x,double y)
{
   const double ZERO_LIMIT = 1e-4;
   if( fabs(aw) < ZERO_LIMIT ) return y-bw;
   double ia  = 1/aw;
   double iaq = ia*ia;
   double dz  = x - (y-bw)*ia;
   return dz/sqrt(1.+iaq);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastPatternFinder::finalize()
{
   msg() << MSG::DEBUG << "Finalizing MuFastPatternFinder - package version " << PACKAGE_VERSION << endreq;
   
   // delete message stream
   if ( m_msg ) delete m_msg;
   
   StatusCode sc = AthAlgTool::finalize(); 
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
