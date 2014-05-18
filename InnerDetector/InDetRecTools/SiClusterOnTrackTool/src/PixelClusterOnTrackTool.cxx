/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
//   Implementation file for class PixelClusterOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for PixelClusterOnTrack object production
///////////////////////////////////////////////////////////////////
// started 1.0 21/04/2004 I.Gavrilenko - see ChangeLog
///////////////////////////////////////////////////////////////////

#include "SiClusterOnTrackTool/PixelClusterOnTrackTool.h"

#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"

#include "InDetIdentifier/PixelID.h"
#include "PixelConditionsServices/IPixelOfflineCalibSvc.h"
#include "PixelConditionsTools/IModuleDistortionsTool.h"

#include "TrkSurfaces/PlaneSurface.h"

#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/IIncidentSvc.h"

#include "SiClusterizationTool/NnClusterizationFactory.h"
#include "EventPrimitives/EventPrimitives.h"
#include "PixelGeoModel/IBLParameterSvc.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

using CLHEP::mm;
using CLHEP::micrometer;



///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

namespace
{
  double distance(std::vector<Amg::Vector2D> & vectorOfPositions,
                  std::vector<Amg::Vector2D> & allLocalPositions,
                  std::vector<Amg::MatrixX> & allErrorMatrix,
                  int i,int j,int k)
  {
    return 
        std::pow(vectorOfPositions[i][0]-allLocalPositions[0][0],2)/allErrorMatrix[0](0,0)+
        std::pow(vectorOfPositions[j][0]-allLocalPositions[1][0],2)/allErrorMatrix[1](0,0)+
        std::pow(vectorOfPositions[k][0]-allLocalPositions[2][0],2)/allErrorMatrix[2](0,0)+
        std::pow(vectorOfPositions[i][1]-allLocalPositions[0][1],2)/allErrorMatrix[0](1,1)+
        std::pow(vectorOfPositions[j][1]-allLocalPositions[1][1],2)/allErrorMatrix[1](1,1)+
        std::pow(vectorOfPositions[k][1]-allLocalPositions[2][1],2)/allErrorMatrix[2](1,1);
  }
}


InDet::PixelClusterOnTrackTool::PixelClusterOnTrackTool
(const std::string& t,const std::string& n,const IInterface* p) :
        ::AthAlgTool(t,n,p),
	m_pixDistoTool("PixelDistortionsTool"),
	m_errorScalingTool("Trk::RIO_OnTrackErrorScalingTool/RIO_OnTrackErrorScalingTool"),
	m_calibSvc("PixelOfflineCalibSvc",n),
	m_disableDistortions(false),
	m_rel13like(false),
	m_applyNNcorrection(false),
	m_NNIBLcorrection(false),
	m_IBLAbsent(true),
	m_NnClusterizationFactory("InDet::NnClusterizationFactory/NnClusterizationFactory"),
	m_storeGate( "StoreGateSvc", n ),
	m_incidentSvc("IncidentSvc", n),
	m_IBLParameterSvc("IBLParameterSvc",n),	
	m_splitClusterMap(0),
	m_splitClusterMapName("SplitClusterAmbiguityMap"),
	m_doNotRecalibrateNN(false),
	m_noNNandBroadErrors(false)
{
  declareInterface<IRIO_OnTrackCreator>(this);
  
  declareProperty("ErrorScalingTool",         m_errorScalingTool,"The error scaling tool");
  declareProperty("PixelDistortionsTool",     m_pixDistoTool,       "Tool to retrieve pixel distortions");
  declareProperty("PositionStrategy",         m_positionStrategy=1,"Which calibration of cluster positions"); 
  declareProperty("ErrorStrategy",            m_errorStrategy=2,"Which calibration of cluster position errors"); 
  declareProperty("DisableDistortions",	      m_disableDistortions,	"Disable simulation of module distortions");
  declareProperty("Release13like",            m_rel13like, "Activate release-13 like settigs");
  declareProperty("PixelOfflineCalibSvc",     m_calibSvc, "Offline calibration svc");
  declareProperty("applyNNcorrection",        m_applyNNcorrection);
  declareProperty("NNIBLcorrection",	      m_NNIBLcorrection);
  declareProperty("EventStore",              m_storeGate );
  declareProperty("IncidentService",         m_incidentSvc );
  declareProperty("NnClusterizationFactory", m_NnClusterizationFactory);
  declareProperty("SplitClusterAmbiguityMap", m_splitClusterMapName);
  declareProperty("doNotRecalibrateNN",       m_doNotRecalibrateNN);
  declareProperty("m_noNNandBroadErrors",     m_noNNandBroadErrors);

}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::PixelClusterOnTrackTool::~PixelClusterOnTrackTool(){}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::PixelClusterOnTrackTool::initialize()
{
  StatusCode sc = AthAlgTool::initialize(); 

   msg(MSG::INFO)  << name() << " initialize()" << endreq;   
   msg(MSG::DEBUG) << "Error strategy is" << m_errorStrategy << endreq;
   msg(MSG::DEBUG) << "Release 13 flag is" << m_rel13like << endreq;

   if (m_IBLParameterSvc.retrieve().isFailure()) { 
     ATH_MSG_WARNING( "Could not retrieve IBLParameterSvc"); 
   } 
   else { 
     m_IBLParameterSvc->setBoolParameters(m_applyNNcorrection,"doPixelClusterSplitting");
     m_IBLParameterSvc->setBoolParameters(m_IBLAbsent,"IBLAbsent");
   } 
   
   //get the offline calibration service
   sc = m_calibSvc.retrieve();
   if (sc.isFailure() || !m_calibSvc) {
      msg(MSG::ERROR) << m_calibSvc.type() << " not found! "<< endreq;
      // return StatusCode::SUCCESS;
   }
   else{
      msg(MSG::INFO) << "Retrieved tool " <<  m_calibSvc.type() << endreq;
   }

   // get the error scaling tool
  if ( m_errorScalingTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_errorScalingTool << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_errorScalingTool << endreq;
    m_scalePixelCov = m_errorScalingTool->needToScalePixel();
    if (m_scalePixelCov) msg(MSG::DEBUG) << "Detected need for scaling Pixel errors." << endreq;
  }
 
  // the NN corrections
  if ( m_applyNNcorrection && m_NnClusterizationFactory.retrieve().isFailure())
  {
      msg(MSG::ERROR) << "Can't get " << m_NnClusterizationFactory << endreq;
      return StatusCode::FAILURE;
  } else if (m_applyNNcorrection)
      msg(MSG::INFO) << "Retireved " <<  m_NnClusterizationFactory << " for NN corrections." << endreq;
  
 
  // get the module distortions tool
  if (!m_disableDistortions) {
    if  (!m_pixDistoTool.empty()) {
      sc = m_pixDistoTool.retrieve();
      if ( sc != StatusCode::SUCCESS ) {
	msg(MSG::ERROR) << "Can't get pixel distortions tool " << endreq;
      } else {
	msg(MSG::INFO) << "Pixel distortions tool retrieved" << endreq;
      }
    } else {
      msg(MSG::INFO) << "No PixelDistortionsTool selected." << endreq;
    }
  }
  else{
    msg(MSG::INFO) << "No PixelDistortions will be simulated." << endreq;
  }
    
  if (detStore()->retrieve(m_pixelid, "PixelID").isFailure()) {
    ATH_MSG_FATAL ("Could not get Pixel ID helper");
    return StatusCode::FAILURE;
  }

  if (m_storeGate.retrieve().isFailure()){
    ATH_MSG_WARNING("Can not retrieve " << m_storeGate << ". Exiting.");
    return StatusCode::FAILURE;
  }

  if (m_incidentSvc.retrieve().isFailure()){
    ATH_MSG_WARNING("Can not retrieve " << m_incidentSvc << ". Exiting.");
    return StatusCode::FAILURE;
  }

  // register to the incident service: EndEvent needed for memory cleanup
  m_incidentSvc->addListener( this, "EndEvent");

  /* ME : booking of test histos is something to be hidden from production code !
     sc = service("HistogramDataSvc", m_HistSvc, true);
     // Define here the histograms;
     m_h_Resx = m_HistSvc->book(m_foldername,"Resx","X shift (um)",400,-400.,400.);
     m_h_Resy = m_HistSvc->book(m_foldername,"Resy","Y shift (um)",400,-400.,400.);
     m_h_Locx = m_HistSvc->book(m_foldername,"Locx","X position (mm)",400,-20.,20.);
     m_h_Locy = m_HistSvc->book(m_foldername,"Locy","Y position (mm)",800,-40.,40.); 
     m_h_ThetaTrack = m_HistSvc->book(m_foldername,"ThetaTrack","Theta Track (rad)",140,-7.,7);
     m_h_PhiTrack = m_HistSvc->book(m_foldername,"PhiTrack","Phi Track (rad)",140,-7.,7);    
  */
  return sc;
}

 
void InDet::PixelClusterOnTrackTool::handle(const Incident& inc) 
{
      if ( inc.type() == "EndEvent" ){
          ATH_MSG_VERBOSE("'EndEvent' incident caught. Refreshing Cache.");
	  m_splitClusterMap = 0;
      }     
 }


///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::PixelClusterOnTrackTool::finalize()
{
  StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Trk::SiClusterOnTrack  production
///////////////////////////////////////////////////////////////////


const InDet::PixelClusterOnTrack* InDet::PixelClusterOnTrackTool::correct
(const Trk::PrepRawData& rio,const Trk::TrackParameters& trackPar) const
{
  
  
  // check if cluster splitting actually could be done you could : switching off will only happen once
  bool clusterMapExists = m_applyNNcorrection ? m_storeGate->contains<InDet::PixelGangedClusterAmbiguities>(m_splitClusterMapName) : false;
  if (!clusterMapExists && m_applyNNcorrection){
      ATH_MSG_INFO("No PixelCluster splitting information available, switchng NN recalibration off (was: on).");
      ATH_MSG_INFO(" -> This is expected if you read ESD data prior to release 17.0.2.8");
      m_applyNNcorrection = false;
  }
  
  if (!m_applyNNcorrection || ((dynamic_cast<const InDetDD::SiDetectorElement*>(rio.detectorElement()))->isBlayer() && !m_NNIBLcorrection && !m_IBLAbsent))
  {
    return correctDefault(rio,trackPar);
  }
  else
  {
    if (m_errorStrategy==0 || m_errorStrategy ==1)
    {
      // version from Giacinto
      if (m_noNNandBroadErrors ) return 0; 

      // if we try broad errors, get Pixel Cluster to test if it is split
      const InDet::PixelCluster* pix = dynamic_cast<const InDet::PixelCluster*>(&rio);
      if(!pix) return 0;
      
      if (pix->isSplit())
	return correctNN(rio,trackPar);
      else
	return correctDefault(rio,trackPar);
    }
    else
    {
      return correctNN(rio,trackPar);
    }
  }
}



/** The correct method produces a PixelClusterOnTrack using the
 *  measured PixelCluster and the track prediction. 
 */
const InDet::PixelClusterOnTrack* InDet::PixelClusterOnTrackTool::correctDefault
(const Trk::PrepRawData& rio,const Trk::TrackParameters& trackPar) const{
  //  const InDet::SiCluster* SC = dynamic_cast<const InDet::SiCluster*> (&rio);
  const InDet::PixelCluster* pix = 0;
  if(!(pix = dynamic_cast<const InDet::PixelCluster*>(&rio))) return 0;

  ATH_MSG_VERBOSE ("Correct called with Error strategy " << m_errorStrategy);
  
  // PixelClusterOnTrack production
  //
  Trk::LocalParameters locpar;
  Amg::Vector3D   glob(pix->globalPosition());

  // Include IBL calibration constants
  static const int nbinphi=9;
  static const double phimin=-0.27, phimax=0.27;
  static double phix[nbinphi+1];
  for (int i=0; i<=nbinphi; i++) phix[i]=phimin+i*(phimax-phimin)/nbinphi;
  static const int nbineta=6;
  static const double etacen[nbineta]={-0.,1.,1.55,1.9,2.15,2.35};
  static double etax[nbineta+1];
  etax[0]=0.; etax[nbineta]=2.7;
  for (int i=0; i<nbineta-1; i++) etax[i+1]=(etacen[i]+etacen[i+1])/2.;
  static double calphi[nbinphi];
  static double caleta[nbineta][3];
  static double calerrphi[nbinphi][3];
  static double calerreta[nbineta][3];
#include "IBL_calibration.h"
  
  // Get pointer to detector element
  
  const InDetDD::SiDetectorElement* element = pix->detectorElement(); 
  if(!element) return 0;
  bool blayer = element->isBlayer();
  IdentifierHash                    iH = element->identifyHash();
  
  double errphi = -1; 
  double erreta = -1; 

  if(pix->rdoList().size() <=0) {
    ATH_MSG_WARNING ("Pixel RDO-list size is 0, check integrity of pixel clusters! stop ROT creation.");
    return NULL;
  }
  else{
  
    const InDetDD::PixelModuleDesign* design = 
           dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());

    // float phi = trackPar.parameters()[Trk::phi];
    // float theta = trackPar.parameters()[Trk::theta];
    // const Trk::Surface* surf = trackPar.associatedSurface();
    // float phis = surf->normal().phi();
    // float thetas = surf->normal().theta();
    // float angle2 = phi-phis;
    // if(angle2>PI) angle2 = 2*PI-angle2;
    // if(angle2<-1*PI) angle2 += 2*PI;
    // if(angle2> PI/2) angle2 -= PI;
    // if(angle2< -1*PI/2) angle2 += PI;

    // get candidate track angle in module local frame
    Amg::Vector3D my_track = trackPar.momentum();
    Amg::Vector3D my_normal = element->normal();
    Amg::Vector3D my_phiax = element->phiAxis();
    Amg::Vector3D my_etaax = element->etaAxis();
    float trkphicomp = my_track.dot(my_phiax);
    float trketacomp = my_track.dot(my_etaax);
    float trknormcomp = my_track.dot(my_normal);
    double bowphi = atan2(trkphicomp,trknormcomp);
    double boweta = atan2(trketacomp,trknormcomp);
    float etatrack = trackPar.eta();
    
    float tanl = element->getTanLorentzAnglePhi();
    int readoutside = element->design().readoutSide();
   
    // double PI = acos(-1.);
    // map the angles of inward-going tracks onto [-PI/2, PI/2]
    if(bowphi > M_PI/2) bowphi -= M_PI;
    if(bowphi < -M_PI/2) bowphi += M_PI;
    // finally, subtract the Lorentz angle effect
    // the readoutside term is needed because of a bug in old 
    // geometry versions (CSC-01-* and CSC-02-*)
    double angle = atan(tan(bowphi)-readoutside*tanl);

    // try to understand...
    const Identifier element_id = element->identify();
    int PixEtaModule = m_pixelid->eta_module(element_id);    
    int PixPhiModule = m_pixelid->phi_module(element_id);
    double PixTrkPt = trackPar.pT();
    double PixTrkEta = trackPar.eta();
    ATH_MSG_VERBOSE ("tanl = " <<  tanl << " readout side is " << readoutside <<
                     " module " << PixEtaModule << " " << PixPhiModule<<
                     " track pt, eta = " << PixTrkPt << " " << PixTrkEta <<
                     " track momentum phi, norm = " << trkphicomp << " " <<
                     trknormcomp << " bowphi = " << bowphi <<" angle = "<<angle);
     
    float omegaphi = pix->omegax();
    float omegaeta = pix->omegay();
    double localphi = -9999.;
    double localeta = -9999.;    
    
//    m_h_ThetaTrack->fill(boweta,1.);
//    m_h_PhiTrack->fill(bowphi,1.);
    

    std::vector<Identifier> rdos = pix->rdoList();
    std::vector<Identifier>::const_iterator oneRDO = rdos.begin();
    InDetDD::SiLocalPosition meanpos(0,0,0);
    int rowmin=9999; int rowmax=-9999;
    int colmin=9999; int colmax=-9999;
    for(; oneRDO != rdos.end(); oneRDO++){
      Identifier rId = *oneRDO;
      int row = m_pixelid->phi_index(rId);
      int col = m_pixelid->eta_index(rId);
      if(rowmin > row) rowmin = row;
      if(rowmax < row) rowmax = row;
      if(colmin > col) colmin = col;
      if(colmax < col) colmax = col;
      meanpos += design->positionFromColumnRow(col,row);
    }
    meanpos = meanpos/rdos.size();
    InDetDD::SiLocalPosition pos1 = 
         design->positionFromColumnRow(colmin,rowmin);
    InDetDD::SiLocalPosition pos2 = 
         design->positionFromColumnRow(colmax,rowmin);
    InDetDD::SiLocalPosition pos3 = 
         design->positionFromColumnRow(colmin,rowmax);
    InDetDD::SiLocalPosition pos4 = 
         design->positionFromColumnRow(colmax,rowmax);

    InDetDD::SiLocalPosition centroid = 0.25*(pos1+pos2+pos3+pos4); 
    double shift = element->getLorentzCorrection();
    int nrows = rowmax-rowmin+1;
    int ncol = colmax-colmin+1;
    double ang = 999.;
    double eta = 999.;

    //    ATH_MSG_VERBOSE ( << "Position strategy = " 
    //    << m_positionStrategy << "omegaphi = " << omegaphi )

    // TOT interpolation for collision data    
    // Force IBL to use digital clustering and broad errors.
    if(m_positionStrategy > 0 && omegaphi > -0.5 && omegaeta > -0.5 ){
       localphi = centroid.xPhi()+shift;
       localeta = centroid.xEta();

       //  msg(MSG::VERBOSE) << "here I am = " << endreq;

       // barrel 
       if(element->isBarrel() ){

	 //   double delta=0;
          ang = 180*angle/M_PI;
          double delta = 0.;
          if (m_IBLAbsent || !blayer) delta = m_calibSvc->getBarrelDeltaX(nrows,ang);
	  else { 	 //special calibration for IBL
	    if ( angle<phix[0] || angle>phix[nbinphi] || nrows!=2 ) 
	      delta=0.;
	    else{
	      int bin=-1;
	      while ( angle>phix[bin+1] ) bin++;
	      delta = calphi[bin];
	    }
	    if (m_calibSvc->includesIBLParams()) delta = m_calibSvc->getIBLDeltaX(nrows,ang); 
	  }
	  // if(nrows==2 && ang<14) delta = 0.06-0.0037*(14-ang);
	  // if(nrows==2 && ang > 14 && ang<30) delta = 0.06-0.0037*(ang-14);
          localphi += delta*(omegaphi-0.5);


          // settle the sign/pi periodicity issues
	  double thetaloc=-999.;
	  if(boweta > -0.5*M_PI && boweta < M_PI/2.){ 
            thetaloc = M_PI/2.-boweta;
	  }
	  else if(boweta > M_PI/2. && boweta < M_PI){ 
            thetaloc = 1.5*M_PI-boweta;
	  }
          else{ // 3rd quadrant
            thetaloc = -0.5*M_PI-boweta;
	  }
	  double etaloc = -1*log(tan(thetaloc/2.));

          if ( m_IBLAbsent || !blayer ) delta = m_calibSvc->getBarrelDeltaY(ncol,etaloc);
	  else { 	 //special calibration for IBL
	    etaloc = fabs(etaloc);
	    if ( etaloc<etax[0] || etaloc>etax[nbineta] ) 
	      delta=0.;
	    else{
	      int bin=-1;
	      while ( etaloc>etax[bin+1] ) bin++;
              if ( ncol==bin ) delta = caleta[bin][0];
              else if ( ncol==bin+1 ) delta = caleta[bin][1];
              else if ( ncol==bin+2 ) delta = caleta[bin][2];
              else delta = 0.;
	    }
	    if (m_calibSvc->includesIBLParams()) delta = m_calibSvc->getIBLDeltaY(ncol,etaloc);
	  }
          localeta += delta*(omegaeta-0.5);
       }
       else{
	 // collision endcap data
	 if(m_positionStrategy == 1){
	   // double deltax = 10*micrometer;
	   // double deltay = 10*micrometer;

          double deltax = m_calibSvc->getEndcapDeltaX();
          double deltay = m_calibSvc->getEndcapDeltaY();
          localphi += deltax*(omegaphi-0.5);
	  localeta += deltay*(omegaeta-0.5);


	 }
	 // SR1 cosmics endcap data 
	 // some parametrization dependent on track angle
	 // would be better here
	 else if(m_positionStrategy == 20){
          double deltax = 35*micrometer;
          double deltay = 35*micrometer;
          localphi += deltax*(omegaphi-0.5);      
          localeta += deltay*(omegaeta-0.5);
	 }
       }  
    }
    // digital
    else{
      localphi = meanpos.xPhi()+shift;
      localeta = meanpos.xEta();
    } 

    InDet::SiWidth width = pix->width();
    
    //Error strategies

    // For very shallow tracks the cluster can easily break as 
    // the average charge per pixel is of the order of the threshold
    // In this case, an error equal to the geometrical projection 
    // of the track path in silicon onto the module surface seems 
    // appropriate  
    if(fabs(angle) > 1){
      errphi = 250*micrometer*tan(fabs(angle))/sqrt(12.);
      erreta = width.z() > 250*micrometer*tan(fabs(boweta)) ? 
        width.z()/sqrt(12.) : 250*micrometer*tan(fabs(boweta))/sqrt(12.);
      ATH_MSG_VERBOSE ( "Shallow track with tanl = " <<  tanl << " bowphi = " <<
                        bowphi << " angle = " << angle << " width.z = " << width.z()<<
                        " errphi = " << errphi << " erreta = " << erreta);
    }
    else if( m_errorStrategy == 0 ){
       errphi = width.phiR()/sqrt(12.);          
       erreta = width.z()/sqrt(12.);
    }
    else if( m_errorStrategy == 1 ){
       errphi = (width.phiR()/nrows)/sqrt(12.);
       erreta = (width.z()/ncol)/sqrt(12.);
    }
    else if( m_errorStrategy == 2 ){
     
       if(element->isBarrel()){

           if ( m_IBLAbsent || !blayer ) errphi = m_calibSvc->getBarrelNewErrorPhi(ang,nrows);
	   else { 	 //special calibration for IBL
	     if ( angle<phix[0] || angle>phix[nbinphi] ) 
	       errphi = width.phiR()/sqrt(12.);
	     else{
	       int bin=-1;
	       while ( angle>phix[bin+1] ) bin++;
	       if ( nrows==1 ) errphi = calerrphi[bin][0];
	       else if ( nrows==2 ) errphi = calerrphi[bin][1];
	       else errphi=calerrphi[bin][2];
	     }
	   }

	   if(m_rel13like){
             erreta = m_calibSvc->getBarrelErrorEta(eta,ncol,nrows);
	   }
	   else if ( m_IBLAbsent || !blayer ) {
	     erreta =  m_calibSvc->getBarrelNewErrorEta(fabs(etatrack),nrows,ncol);
	   }
	   else { 	 //special calibration for IBL
	     double etaloc = fabs(etatrack);
	     if ( etaloc<etax[0] || etaloc>etax[nbineta] ) 
	       erreta = width.z()/sqrt(12.);
	     else{
	       /*
	       int bin=-1;
	       while ( etaloc>etax[bin+1] ) bin++;
	       */
	       int bin = 0;
	       while ( etaloc>etax[bin+1] ) ++bin;
	       if ( ncol==bin ) erreta = calerreta[bin][0];
	       else if ( ncol==bin+1 ) erreta = calerreta[bin][1];
	       else if ( ncol==bin+2 ) erreta = calerreta[bin][2];
	       else erreta = width.z()/sqrt(12.);
	     }
	   }
       }
       else{
           errphi = m_calibSvc->getEndCapErrorPhi(ncol,nrows);
           erreta = m_calibSvc->getEndCapErrorEta(ncol,nrows);
       }       
       if (errphi>erreta) erreta = width.z()/sqrt(12.);
    }
    
    //Bow correction for Barrel
    
    /*std::vector<double> VecPos;
    std::vector<double>* pvec;
    //pvec = new std::vector<double>;
    VecPos.push_back(localphi);
    VecPos.push_back(localeta);
    pvec = &VecPos; */
    
    
    Amg::Vector2D locpos = Amg::Vector2D(localphi,localeta);	
    if(element->isBarrel() && !m_disableDistortions ) {
      // ME: comment out: m_h_ThetaTrack->fill(boweta,1.);
      //msg(MSG::FATAL) << element->identify() << endreq;
      //msg(MSG::FATAL) << m_pixelid->print(element->identify()) << endreq;

      //       msg(MSG::INFO) <<  "YYY bowphi = " << bowphi 
      //  << " angle2 = " << angle2  
      //  << " theta-thetas = " << thetas-theta 
      //  << " boweta = " << boweta << endreq;  

      correctBow(element->identify(), locpos, bowphi, boweta);
    }
    

    locpar = Trk::LocalParameters(locpos);
    centroid = InDetDD::SiLocalPosition(localeta,localphi,0.);
    glob = element->globalPosition(centroid);
    
 }

  // Error matrix production
  
  Amg::MatrixX  cov = pix->localCovariance();

  // corrected phi error
  if(errphi>0) cov(0,0) = errphi*errphi; 
  if(erreta>0) cov(1,1) = erreta*erreta; 

  ATH_MSG_VERBOSE (" errphi =  " << errphi << " erreta = "<< erreta);
  
  // create new copy of error matrix
  if( m_scalePixelCov ){
    Amg::MatrixX* newCov = m_errorScalingTool->createScaledPixelCovariance(cov,element->isEndcap());
    if( !newCov ) {
      ATH_MSG_WARNING("Failed to create scaled error for Pixel");
      return 0;
    }
    cov = *newCov;
    delete newCov;
  }
  bool isbroad = (m_errorStrategy==0) ? true : false;
  return new InDet::PixelClusterOnTrack(pix,locpar,cov,iH,glob,pix->gangedPixel(),isbroad);
} 

void InDet::PixelClusterOnTrackTool::correctBow(const Identifier& id, Amg::Vector2D& localpos, const double phi, const double theta) const {
  Amg::Vector3D dir(tan(phi),tan(theta),1.);
  Amg::Vector2D newpos = 
    m_pixDistoTool->correctReconstruction(id, localpos, dir);
  localpos = newpos;
  return;
} 

const InDet::PixelClusterOnTrack* InDet::PixelClusterOnTrackTool::correct
(const Trk::PrepRawData& rio,const Trk::TrackParameters& trackPar,
 const InDet::PixelClusterStrategy strategy) const {
  int initial_errorStrategy;
  const InDet::PixelClusterOnTrack *newROT;
  switch ( strategy ) {
  case InDet::PIXELCLUSTER_OUTLIER : // if cluster is outlier, increase errors
  case InDet::PIXELCLUSTER_SHARED :
    initial_errorStrategy = m_errorStrategy;
    if(!m_rel13like) m_errorStrategy = 0; // error as size of cluster /sqrt(12)
    newROT = correct(rio,trackPar);
    m_errorStrategy = initial_errorStrategy;
    return newROT;
  default:
    return correct(rio, trackPar);
  }
}

// GP: NEW correct() method in case of NN based calibration  */

const InDet::PixelClusterOnTrack* InDet::PixelClusterOnTrackTool::correctNN
(const Trk::PrepRawData& rio,const Trk::TrackParameters& trackPar) const
{
  const InDet::PixelCluster* pixelPrepCluster=dynamic_cast<const InDet::PixelCluster*>(&rio);
  
  if(pixelPrepCluster==0) 
  {
    ATH_MSG_WARNING("This is not a pixel cluster, return 0.");
    return 0;
  }

  const InDetDD::SiDetectorElement* element = pixelPrepCluster->detectorElement(); 
  if(!element) 
  {
    ATH_MSG_WARNING ("Cannot access detector element. Aborting cluster correction...");
    return 0;
  }

  IdentifierHash iH = element->identifyHash();

  if (m_doNotRecalibrateNN)
  {
    
    Amg::Vector3D   glob(pixelPrepCluster->globalPosition());

    Amg::Vector3D my_track = trackPar.momentum();
    Amg::Vector3D my_normal = element->normal();
    Amg::Vector3D my_phiax = element->phiAxis();
    Amg::Vector3D my_etaax = element->etaAxis();
    float trkphicomp = my_track.dot(my_phiax);
    float trketacomp = my_track.dot(my_etaax);
    float trknormcomp = my_track.dot(my_normal);
    double bowphi = atan2(trkphicomp,trknormcomp);
    double boweta = atan2(trketacomp,trknormcomp);
      
    Amg::Vector2D locpos = pixelPrepCluster->localPosition();
    if(element->isBarrel() && !m_disableDistortions ) {
      correctBow(element->identify(), locpos, bowphi, boweta);
    }
    
    Trk::LocalParameters locpar = Trk::LocalParameters(locpos);
    Amg::MatrixX cov = pixelPrepCluster->localCovariance();

    // create new copy of error matrix
    if( m_scalePixelCov ){
      Amg::MatrixX* newCov = m_errorScalingTool->createScaledPixelCovariance(cov,element->isEndcap());
      if( !newCov ) {
	ATH_MSG_WARNING("Failed to create scaled error for Pixel");
	return 0;
      }
      cov = *newCov;
      delete newCov;
    }
    return new InDet::PixelClusterOnTrack(pixelPrepCluster,locpar,cov,iH,glob,pixelPrepCluster->gangedPixel(),false);
  }
  
  
  if (!m_splitClusterMap) {
    StatusCode sc = m_storeGate->retrieve(m_splitClusterMap, m_splitClusterMapName);
    if (sc.isFailure()) {
      ATH_MSG_DEBUG("Could not retrieve split cluster map.");
      return 0;
    } 
  }

  std::vector<Amg::Vector2D> vectorOfPositions;
  
  int numberOfSubclusters=1;
  vectorOfPositions.push_back(pixelPrepCluster->localPosition());
  
  InDet::PixelGangedClusterAmbiguities::const_iterator mapBegin=m_splitClusterMap->begin();
  InDet::PixelGangedClusterAmbiguities::const_iterator mapEnd=m_splitClusterMap->end();

  for (InDet::PixelGangedClusterAmbiguities::const_iterator mapIter=mapBegin;mapIter!=mapEnd;++mapIter)
  {
    const SiCluster* first=(*mapIter).first;
    const SiCluster* second=(*mapIter).second;
    if (first == &rio  && second != &rio)
    {
      ATH_MSG_DEBUG("Found additional split cluster in ambiguity map (+=1).");
      numberOfSubclusters+=1;

      const SiCluster* otherOne=second;

      const InDet::PixelCluster* pixelAddCluster=dynamic_cast<const InDet::PixelCluster*>(otherOne);
      if (pixelAddCluster==0)
      {
        ATH_MSG_WARNING("Pixel ambiguity map has empty pixel cluster. Please DEBUG!");
        continue;
      }
      vectorOfPositions.push_back(pixelAddCluster->localPosition());
      
      if(msgLvl(MSG::DEBUG))
      {
        msg(MSG::DEBUG) << "Found one more pixel cluster. Posistion x: " 
                        << pixelAddCluster->localPosition()[0] << "y: " << pixelAddCluster->localPosition()[1] << endreq;
      }
    }//find relevant element of map
  }//iterate over map

  // A.S. hack for the moment: isSplit is also set for modified (non-split) 1-pixel-clusters ... is this needed anyway ?
  //
  //if (pixelPrepCluster->isSplit() && numberOfSubclusters<2)
  //{
  //msg(MSG::WARNING) << " Cluster is split but no further clusters found in split cluster map." << endreq;
  //}
  
  //now you have numberOfSubclusters and the vectorOfPositions (Amg::Vector2D)

  const Trk::AtaPlane* parameters=dynamic_cast<const Trk::AtaPlane*>(&trackPar);
  if (parameters==0)
  {
    msg(MSG::WARNING) << "Parameters are not at a plane ! Aborting cluster correction... " << endreq;
    return 0;
  }

  std::vector<Amg::Vector2D>     allLocalPositions;
  std::vector<Amg::MatrixX>      allErrorMatrix;

  allLocalPositions=
      m_NnClusterizationFactory->estimatePositions(*pixelPrepCluster,
                                                   parameters->associatedSurface(),
                                                   *parameters,
                                                   allErrorMatrix,
                                                   numberOfSubclusters);
  
  if (allLocalPositions.size()  == 0)
  {
    if(msgLvl(MSG::DEBUG))
    {
      msg(MSG::DEBUG) << " Cluster cannot be treated by NN. Giving back to default clusterization " << endreq;
    }
    return correctDefault(rio,trackPar);
  }

  if (allLocalPositions.size() != size_t(numberOfSubclusters))
  {
    msg(MSG::WARNING) << "Returned position vector size " << allLocalPositions.size() << " not according to expected number of subclusters: " << numberOfSubclusters << ". Abort cluster correction..." << endreq;
    return 0;
  }
  
  Amg::Vector2D finalposition;
  Amg::MatrixX finalerrormatrix;

  // GP: now the not so nice part of matching the new result with the old one...
  // Takes the error into account to improve the matching

  if (numberOfSubclusters==1)
  {
    finalposition=allLocalPositions[0];
    finalerrormatrix=allErrorMatrix[0];
  }
  
  if (numberOfSubclusters==2)
  {
    double distancesq1=
        std::pow(vectorOfPositions[0][0]-allLocalPositions[0][0],2)/allErrorMatrix[0](0,0)+
        std::pow(vectorOfPositions[1][0]-allLocalPositions[1][0],2)/allErrorMatrix[1](0,0)+
        std::pow(vectorOfPositions[0][1]-allLocalPositions[0][1],2)/allErrorMatrix[0](1,1)+
        std::pow(vectorOfPositions[1][1]-allLocalPositions[1][1],2)/allErrorMatrix[1](1,1);

    double distancesq2=
        std::pow(vectorOfPositions[1][0]-allLocalPositions[0][0],2)/allErrorMatrix[0](0,0)+
        std::pow(vectorOfPositions[0][0]-allLocalPositions[1][0],2)/allErrorMatrix[1](0,0)+
        std::pow(vectorOfPositions[1][1]-allLocalPositions[0][1],2)/allErrorMatrix[0](1,1)+
        std::pow(vectorOfPositions[0][1]-allLocalPositions[1][1],2)/allErrorMatrix[1](1,1);

    if(msgLvl(MSG::DEBUG))
    {
      msg(MSG::DEBUG) << " Old pix (1) x: " << vectorOfPositions[0][0] << " y: " << vectorOfPositions[0][1] << endreq;
      msg(MSG::DEBUG) << " Old pix (2) x: " << vectorOfPositions[1][0] << " y: " << vectorOfPositions[1][1] << endreq;
      msg(MSG::DEBUG) << " Pix (1) x: " << allLocalPositions[0][0] << " +/- " << sqrt(allErrorMatrix[0](0,0)) 
                      << " y: " << allLocalPositions[0][1] << " +/- " << sqrt(allErrorMatrix[0](1,1)) << endreq;
      msg(MSG::DEBUG) << " Pix (2) x: " << allLocalPositions[1][0] << " +/- " << sqrt(allErrorMatrix[1](0,0)) 
                      << " y: " << allLocalPositions[1][1] << " +/- " << sqrt(allErrorMatrix[1](1,1)) << endreq;
      msg(MSG::DEBUG) << " Old (1) new (1) dist: " << sqrt(distancesq1) << " Old (1) new (2) " << sqrt(distancesq2) << endreq;
    }
    
    if (distancesq1<distancesq2)
    {
      finalposition=allLocalPositions[0];
      finalerrormatrix=allErrorMatrix[0];
    }
    else
    {
      finalposition=allLocalPositions[1];
      finalerrormatrix=allErrorMatrix[1];
    }
  }


  if (numberOfSubclusters==3)
  {
    double distances[6];

    distances[0]=distance(vectorOfPositions,allLocalPositions,allErrorMatrix,0,1,2);
    distances[1]=distance(vectorOfPositions,allLocalPositions,allErrorMatrix,0,2,1);
    distances[2]=distance(vectorOfPositions,allLocalPositions,allErrorMatrix,1,0,2);
    distances[3]=distance(vectorOfPositions,allLocalPositions,allErrorMatrix,1,2,0);
    distances[4]=distance(vectorOfPositions,allLocalPositions,allErrorMatrix,2,0,1);
    distances[5]=distance(vectorOfPositions,allLocalPositions,allErrorMatrix,2,1,0);

    int smallestDistanceIndex=-10;
    double minDistance=1e10;
    
    for (int i=0;i<6;i++)
    {
      if (msgLvl(MSG::VERBOSE))
      {
        msg(MSG::VERBOSE) << " distance n.: " << i << " distance is: " << distances[i] << endreq;
      }
      
      if (distances[i]<minDistance)
      {
        minDistance=distances[i];
        smallestDistanceIndex=i;
      }
    }
    
    if(msgLvl(MSG::DEBUG))
    {
      msg(MSG::DEBUG) << " The minimum distance is : " << minDistance << " for index: " << smallestDistanceIndex  << endreq;
    }
      
    if (smallestDistanceIndex==0 ||  smallestDistanceIndex==1)
    {
      finalposition=allLocalPositions[0];
      finalerrormatrix=allErrorMatrix[0];
    }
    if (smallestDistanceIndex==2 ||  smallestDistanceIndex==4)
    {
      finalposition=allLocalPositions[1];
      finalerrormatrix=allErrorMatrix[1];
    }
    if (smallestDistanceIndex==3 || smallestDistanceIndex==5)
    {
      finalposition=allLocalPositions[2];
      finalerrormatrix=allErrorMatrix[2];
    }
  }

  if (msgLvl(MSG::DEBUG))
  {
    msg(MSG::DEBUG) << " Old position x: " << pixelPrepCluster->localPosition()[0] << 
        " +/- " << sqrt(pixelPrepCluster->localCovariance()(0,0)) 
                    << " y: " << pixelPrepCluster->localPosition()[1] << 
        " +/- " << sqrt(pixelPrepCluster->localCovariance()(1,1)) << endreq;
    msg(MSG::DEBUG) << " Final position x: " << finalposition[0] << 
        " +/- " << sqrt(finalerrormatrix(0,0)) << 
        " y: " << finalposition[1] << " +/- " << 
        sqrt(finalerrormatrix(1,1)) << endreq;
  }
  


  Amg::Vector3D my_track = trackPar.momentum();
  Amg::Vector3D my_normal = element->normal();
  Amg::Vector3D my_phiax = element->phiAxis();
  Amg::Vector3D my_etaax = element->etaAxis();
  float trkphicomp = my_track.dot(my_phiax);
  float trketacomp = my_track.dot(my_etaax);
  float trknormcomp = my_track.dot(my_normal);
  double bowphi = atan2(trkphicomp,trknormcomp);
  double boweta = atan2(trketacomp,trknormcomp);
      
  if(element->isBarrel() && !m_disableDistortions ) {
    correctBow(element->identify(), finalposition, bowphi, boweta);
  }
    
  Amg::MatrixX  cov = finalerrormatrix;
  // create new copy of error matrix
  if( m_scalePixelCov ){
    Amg::MatrixX* newCov = m_errorScalingTool->createScaledPixelCovariance(cov,element->isEndcap());
    if( !newCov ) {
      ATH_MSG_WARNING("Failed to create scaled error for Pixel");
      return 0;
    }
    cov = *newCov;
    delete newCov;
  }

  InDetDD::SiLocalPosition centroid = InDetDD::SiLocalPosition(finalposition[1],
                                                               finalposition[0],
                                                               0);
  Trk::LocalParameters locpar = Trk::LocalParameters(finalposition);

  const Amg::Vector3D& glob = element->globalPosition(centroid);
    
  
  return new InDet::PixelClusterOnTrack(pixelPrepCluster,locpar,cov,iH,
                                        glob,
                                        pixelPrepCluster->gangedPixel(),
                                        false);

}
