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
#include "PixelGeoModel/IIBLParameterSvc.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Attribute.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
using CLHEP::mm;
using CLHEP::micrometer;

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

namespace
{
  //using x*x might be quicker than pow(x,2), depends on compiler optimisation
  inline double square(const double x){
    return x*x;
  }
  double distance(const std::vector<Amg::Vector2D> & vectorOfPositions,
                  const std::vector<Amg::Vector2D> & allLocalPositions,
                  const std::vector<Amg::MatrixX> & allErrorMatrix,
                  const int i,const int j,const int k)
  {
    return 
        square(vectorOfPositions[i][0]-allLocalPositions[0][0])/allErrorMatrix[0](0,0)+
        square(vectorOfPositions[j][0]-allLocalPositions[1][0])/allErrorMatrix[1](0,0)+
        square(vectorOfPositions[k][0]-allLocalPositions[2][0])/allErrorMatrix[2](0,0)+
        square(vectorOfPositions[i][1]-allLocalPositions[0][1])/allErrorMatrix[0](1,1)+
        square(vectorOfPositions[j][1]-allLocalPositions[1][1])/allErrorMatrix[1](1,1)+
        square(vectorOfPositions[k][1]-allLocalPositions[2][1])/allErrorMatrix[2](1,1);
  }
  
  //avoid a lot of '/sqrt(12)' in loops, declare the constant here
  // constexpr sqrt is a gcc extension, unsupported by clang.
  //constexpr double TOPHAT_SIGMA=1./sqrt(12.);
  static const double TOPHAT_SIGMA=1./sqrt(12.);
}


InDet::PixelClusterOnTrackTool::PixelClusterOnTrackTool
(const std::string& t,const std::string& n,const IInterface* p) :
        ::AthAlgTool(t,n,p),
  m_pixDistoTool("PixelDistortionsTool"),
  m_errorScalingTool("Trk::RIO_OnTrackErrorScalingTool/RIO_OnTrackErrorScalingTool"),
  m_calibSvc("PixelOfflineCalibSvc",n),
  m_detStore(nullptr),
  m_scalePixelCov{},
  m_disableDistortions(false),
  m_rel13like(false),
  m_pixelid(nullptr),
  m_applyNNcorrection(false),
  m_applydRcorrection(false),
  m_NNIBLcorrection(false),
  m_IBLAbsent(true),
  m_NnClusterizationFactory("InDet::NnClusterizationFactory/NnClusterizationFactory"),
  m_storeGate( "StoreGateSvc", n ),
  m_incidentSvc("IncidentSvc", n),
  m_IBLParameterSvc("IBLParameterSvc",n), 
  m_splitClusterMap(0),
  m_splitClusterMapName("SplitClusterAmbiguityMap"),
  m_dRMap(0),
  m_dRMapName("dRMap"),
  m_doNotRecalibrateNN(false),
  m_noNNandBroadErrors(false),
  m_usingTIDE_Ambi(false)
{
  declareInterface<IRIO_OnTrackCreator>(this);
  
  declareProperty("ErrorScalingTool",         m_errorScalingTool,"The error scaling tool");
  declareProperty("PixelDistortionsTool",     m_pixDistoTool,       "Tool to retrieve pixel distortions");
  declareProperty("PositionStrategy",         m_positionStrategy=1,"Which calibration of cluster positions"); 
  declareProperty("ErrorStrategy",            m_errorStrategy=2,"Which calibration of cluster position errors"); 
  declareProperty("DisableDistortions",       m_disableDistortions, "Disable simulation of module distortions");
  declareProperty("Release13like",            m_rel13like, "Activate release-13 like settigs");
  declareProperty("PixelOfflineCalibSvc",     m_calibSvc, "Offline calibration svc");
  declareProperty("applyNNcorrection",        m_applyNNcorrection);
  declareProperty("applydRcorrection",        m_applydRcorrection);
  declareProperty("NNIBLcorrection",        m_NNIBLcorrection);
  declareProperty("EventStore",              m_storeGate );
  declareProperty("IncidentService",         m_incidentSvc );
  declareProperty("NnClusterizationFactory", m_NnClusterizationFactory);
  declareProperty("SplitClusterAmbiguityMap", m_splitClusterMapName);
  declareProperty("dRMapName",                m_dRMapName);
  declareProperty("doNotRecalibrateNN",       m_doNotRecalibrateNN);
  declareProperty("m_noNNandBroadErrors",     m_noNNandBroadErrors);
  declareProperty("RunningTIDE_Ambi",         m_usingTIDE_Ambi);

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
   ATH_MSG_DEBUG( name() << " initialize()");   
   ATH_MSG_DEBUG( "Error strategy is" << m_errorStrategy );
   ATH_MSG_DEBUG( "Release 13 flag is" << m_rel13like);

   if (m_IBLParameterSvc.retrieve().isFailure()) { 
     ATH_MSG_WARNING( "Could not retrieve IBLParameterSvc"); 
   } else { 
     m_IBLParameterSvc->setBoolParameters(m_applyNNcorrection,"doPixelClusterSplitting");
     m_IBLParameterSvc->setBoolParameters(m_IBLAbsent,"IBLAbsent");
   } 
   
   //get the offline calibration service
   ATH_CHECK( m_calibSvc.retrieve());
   
   // get the error scaling tool
  if ( m_errorScalingTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_errorScalingTool << endmsg;
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "Retrieved tool " << m_errorScalingTool);
    m_scalePixelCov = m_errorScalingTool->needToScalePixel();
    if (m_scalePixelCov) ATH_MSG_DEBUG( "Detected need for scaling Pixel errors." );
  }
 
  // the NN corrections
  if ( m_applyNNcorrection && m_NnClusterizationFactory.retrieve().isFailure()){
      msg(MSG::ERROR) << "Can't get " << m_NnClusterizationFactory << endmsg;
      return StatusCode::FAILURE;
  } else if (m_applyNNcorrection)
      ATH_MSG_INFO( "Retrieved " <<  m_NnClusterizationFactory << " for NN corrections." );
  
 
  // get the module distortions tool
  if (!m_disableDistortions) {
    if  (!m_pixDistoTool.empty()) {
      ATH_CHECK( m_pixDistoTool.retrieve());
      ATH_MSG_INFO( "Pixel distortions tool retrieved" );
    } else {
      ATH_MSG_INFO( "No PixelDistortionsTool selected." );
    }
  } else{
    ATH_MSG_INFO( "No PixelDistortions will be simulated." );
  }
    
  ATH_CHECK (detStore()->retrieve(m_pixelid, "PixelID")) ;
  
  ATH_CHECK ( service("DetectorStore", m_detStore));
  ATH_CHECK(m_storeGate.retrieve());
  
  ATH_CHECK(m_incidentSvc.retrieve());

  if (m_applydRcorrection){
    m_incidentSvc->addListener( this, IncidentType::BeginEvent);
  }
  // register to the incident service: EndEvent needed for memory cleanup
  m_incidentSvc->addListener( this, "EndEvent");

  return StatusCode::SUCCESS;
}

 
void InDet::PixelClusterOnTrackTool::handle(const Incident& inc){

  if ( inc.type() == IncidentType::BeginEvent ){
      if(m_fX.size()==0){
          const CondAttrListCollection* atrlistcol=0;
          if (StatusCode::SUCCESS==m_detStore->retrieve(atrlistcol,"/PIXEL/PixelClustering/PixelCovCorr")) {
              // loop over objects in collection
              std::vector<float> fx, fy, fb, fc, fd;
              fx.reserve(7); fy.reserve(7); fb.reserve(7); fc.reserve(7); fd.reserve(7);
              for (CondAttrListCollection::const_iterator citr=atrlistcol->begin(); citr!=atrlistcol->end();++citr) {
                  const coral::AttributeList& atrlist=citr->second;
                  
                  fx.push_back(atrlist["fX1"].data<float>());
                  fx.push_back(atrlist["fX2"].data<float>());
                  fx.push_back(atrlist["fX3"].data<float>());
                  fx.push_back(atrlist["fX4"].data<float>());
                  fx.push_back(atrlist["fX5"].data<float>());
                  fx.push_back(atrlist["fX6"].data<float>());
                  fx.push_back(atrlist["fX7"].data<float>());
                  m_fX.push_back(fx);
                  fx.clear();
                  
                  fy.push_back(atrlist["fY1"].data<float>());
                  fy.push_back(atrlist["fY2"].data<float>());
                  fy.push_back(atrlist["fY3"].data<float>());
                  fy.push_back(atrlist["fY4"].data<float>());
                  fy.push_back(atrlist["fY5"].data<float>());
                  fy.push_back(atrlist["fY6"].data<float>());
                  fy.push_back(atrlist["fY7"].data<float>());
                  m_fY.push_back(fy);
                  fy.clear();
                  
                  fb.push_back(atrlist["fB1"].data<float>());
                  fb.push_back(atrlist["fB2"].data<float>());
                  fb.push_back(atrlist["fB3"].data<float>());
                  fb.push_back(atrlist["fB4"].data<float>());
                  fb.push_back(atrlist["fB5"].data<float>());
                  fb.push_back(atrlist["fB6"].data<float>());
                  fb.push_back(atrlist["fB7"].data<float>());
                  m_fB.push_back(fb);
                  fb.clear();
                  
                  fc.push_back(atrlist["fC1"].data<float>());
                  fc.push_back(atrlist["fC2"].data<float>());
                  fc.push_back(atrlist["fC3"].data<float>());
                  fc.push_back(atrlist["fC4"].data<float>());
                  fc.push_back(atrlist["fC5"].data<float>());
                  fc.push_back(atrlist["fC6"].data<float>());
                  fc.push_back(atrlist["fC7"].data<float>());
                  m_fC.push_back(fc);
                  fc.clear();
                  
                  fd.push_back(atrlist["fD1"].data<float>());
                  fd.push_back(atrlist["fD2"].data<float>());
                  fd.push_back(atrlist["fD3"].data<float>());
                  fd.push_back(atrlist["fD4"].data<float>());
                  fd.push_back(atrlist["fD5"].data<float>());
                  fd.push_back(atrlist["fD6"].data<float>());
                  fd.push_back(atrlist["fD7"].data<float>());
                  m_fD.push_back(fd);
                  fd.clear();
              }
          }
          else {
                  msg(MSG::ERROR) << "Cannot find covariance corrections for key "
                  << "/PIXEL/PixelClustering/PixelCovCorr" << " - no correction " << endmsg;
              return;                                   
          }
      }
  }
  
  if ( inc.type() == "EndEvent" ){
    ATH_MSG_VERBOSE("'EndEvent' incident caught. Refreshing Cache.");
    m_splitClusterMap = 0;
  }     
}


///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::PixelClusterOnTrackTool::finalize(){
  return StatusCode::SUCCESS;
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
  }else{
    if (m_errorStrategy==0 || m_errorStrategy ==1){
      // version from Giacinto
      if (m_noNNandBroadErrors ) return 0; 
      // if we try broad errors, get Pixel Cluster to test if it is split
      const InDet::PixelCluster* pix = dynamic_cast<const InDet::PixelCluster*>(&rio);
      if(!pix) return 0;
      if (pix->isSplit())
        return correctNN(rio,trackPar);
      else
        return correctDefault(rio,trackPar);
    } else {
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
  
  ///UGLY!
#include "IBL_calibration.h"
  ///
  
  // Get pointer to detector element
  const InDetDD::SiDetectorElement* element = pix->detectorElement(); 
  if(!element) return 0;
  bool blayer = element->isBlayer();
  IdentifierHash iH = element->identifyHash();
  
  double errphi = -1; 
  double erreta = -1; 

  if(pix->rdoList().size() <=0) {
    ATH_MSG_WARNING ("Pixel RDO-list size is 0, check integrity of pixel clusters! stop ROT creation.");
    return NULL;
  } else {
  
    const InDetDD::PixelModuleDesign* design = 
           dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());

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
      // barrel 
      if(element->isBarrel() ){
    		ang = 180*angle/M_PI;
    		double delta = 0.;
    		if (m_IBLAbsent || !blayer) delta = m_calibSvc->getBarrelDeltaX(nrows,ang);
    		else {   //special calibration for IBL
      		if ( angle<phix[0] || angle>phix[nbinphi] || nrows!=2 ) {
        	delta=0.;
      	}else{
        	int bin=-1;
        	while ( angle>phix[bin+1] ) bin++;
        	if ((bin>=0) and (bin<nbinphi)){
          	delta = calphi[bin];
        	} else {
          	ATH_MSG_ERROR("bin out of range in line "<<__LINE__<<" of PixelClusterOnTrackTool.cxx.");
        	}
      	}
      	if (m_calibSvc->includesIBLParams()) delta = m_calibSvc->getIBLDeltaX(nrows,ang); 
    	}
    localphi += delta*(omegaphi-0.5);
    // settle the sign/pi periodicity issues
    double thetaloc=-999.;
    if(boweta > -0.5*M_PI && boweta < M_PI/2.){ 
      thetaloc = M_PI/2.-boweta;
    }else if(boweta > M_PI/2. && boweta < M_PI){ 
      thetaloc = 1.5*M_PI-boweta;
    } else{ // 3rd quadrant
      thetaloc = -0.5*M_PI-boweta;
    }
    double etaloc = -1*log(tan(thetaloc/2.));
    if ( m_IBLAbsent || !blayer ) delta = m_calibSvc->getBarrelDeltaY(ncol,etaloc);
    else {   //special calibration for IBL
      etaloc = fabs(etaloc);
      if ( etaloc<etax[0] || etaloc>etax[nbineta] ) 
        delta=0.;
      else{
        int bin=-1;
        while ( etaloc>etax[bin+1] ) bin++;
        if ((bin>=0) and (bin<nbineta)){
          if ( ncol==bin ) delta = caleta[bin][0];
          else if ( ncol==bin+1 ) delta = caleta[bin][1];
          else if ( ncol==bin+2 ) delta = caleta[bin][2];
          else delta = 0.;
        } else {//bin out of range of array indices
          ATH_MSG_ERROR("bin out of range in line "<<__LINE__<<" of PixelClusterOnTrackTool.cxx.");
        }
      }
      if (m_calibSvc->includesIBLParams()) delta = m_calibSvc->getIBLDeltaY(ncol,etaloc);
    }
  localeta += delta*(omegaeta-0.5);
  }else{
   // collision endcap data
  if(m_positionStrategy == 1){
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
      errphi = 250*micrometer*tan(fabs(angle))*TOPHAT_SIGMA;
      erreta = width.z() > 250*micrometer*tan(fabs(boweta)) ? 
        width.z()*TOPHAT_SIGMA : 250*micrometer*tan(fabs(boweta))*TOPHAT_SIGMA;
      ATH_MSG_VERBOSE ( "Shallow track with tanl = " <<  tanl << " bowphi = " <<
                        bowphi << " angle = " << angle << " width.z = " << width.z()<<
                        " errphi = " << errphi << " erreta = " << erreta);
    }
    else if( m_errorStrategy == 0 ){
       errphi = width.phiR()*TOPHAT_SIGMA;          
       erreta = width.z()*TOPHAT_SIGMA;
    }
    else if( m_errorStrategy == 1 ){
       errphi = (width.phiR()/nrows)*TOPHAT_SIGMA;
       erreta = (width.z()/ncol)*TOPHAT_SIGMA;
    }
    else if( m_errorStrategy == 2 ){
     
       if(element->isBarrel()){

           if ( m_IBLAbsent || !blayer ) errphi = m_calibSvc->getBarrelNewErrorPhi(ang,nrows);
     else {    //special calibration for IBL
       if ( angle<phix[0] || angle>phix[nbinphi] ) 
         errphi = width.phiR()*TOPHAT_SIGMA;
       else{
         int bin=-1;//cannot be used as array index, which will happen if angle<phix[bin+1]
         while ( angle>phix[bin+1] ) bin++;
         if ((bin >=0)  and (bin<nbinphi)){
            if ( nrows==1 ) errphi = calerrphi[bin][0];
            else if ( nrows==2 ) errphi = calerrphi[bin][1];
            else errphi=calerrphi[bin][2];
         } else {
           ATH_MSG_ERROR("bin out of range in line "<<__LINE__<<" of PixelClusterOnTrackTool.cxx.");
         }
       }
     }

     if(m_rel13like){
             erreta = m_calibSvc->getBarrelErrorEta(eta,ncol,nrows);
     }
     else if ( m_IBLAbsent || !blayer ) {
       erreta =  m_calibSvc->getBarrelNewErrorEta(fabs(etatrack),nrows,ncol);
     } else {    //special calibration for IBL
       double etaloc = fabs(etatrack);
       if ( etaloc<etax[0] || etaloc>etax[nbineta] ) 
         erreta = width.z()*TOPHAT_SIGMA;
       else{
         int bin = 0;
         while ( etaloc>etax[bin+1] ) ++bin;
         if (bin>=nbineta){
           ATH_MSG_ERROR("bin out of range in line "<<__LINE__<<" of PixelClusterOnTrackTool.cxx.");
         } else {
           if ( ncol==bin ) erreta = calerreta[bin][0];
           else if ( ncol==bin+1 ) erreta = calerreta[bin][1];
           else if ( ncol==bin+2 ) erreta = calerreta[bin][2];
           else erreta = width.z()*TOPHAT_SIGMA;
         }
       }
     }
    }else{
      errphi = m_calibSvc->getEndCapErrorPhi(ncol,nrows);
      erreta = m_calibSvc->getEndCapErrorEta(ncol,nrows);
    }       
    if (errphi>erreta) erreta = width.z()*TOPHAT_SIGMA;
  }
 
    Amg::Vector2D locpos = Amg::Vector2D(localphi,localeta);  
    if(element->isBarrel() && !m_disableDistortions ) {
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
    Amg::MatrixX* newCov = m_errorScalingTool->createScaledPixelCovariance(cov,element->identify());
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
      Amg::MatrixX* newCov = m_errorScalingTool->createScaledPixelCovariance(cov,element->identify());
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
  
  if (!m_dRMap &&  m_storeGate->contains<InDet::DRMap >(m_dRMapName) && m_applydRcorrection) {
    StatusCode sc = m_storeGate->retrieve(m_dRMap, m_dRMapName);
    if (sc.isFailure()) {
      ATH_MSG_DEBUG("Could not retrieve dR map.");
    }
  }
 
  Amg::Vector2D finalposition;
  Amg::MatrixX finalerrormatrix;
  
  if( m_usingTIDE_Ambi )
  {
    if( !getErrorsTIDE_Ambi( pixelPrepCluster, trackPar, finalposition, finalerrormatrix ) )
      return correctDefault(rio,trackPar);
  }
  else
  {
    if( !getErrorsDefaultAmbi( pixelPrepCluster, trackPar, finalposition, finalerrormatrix ) )
      return correctDefault(rio,trackPar);
  }

  if (msgLvl(MSG::DEBUG))
  {
    msg(MSG::DEBUG) << " Old position x: " << pixelPrepCluster->localPosition()[0] << 
        " +/- " << sqrt(pixelPrepCluster->localCovariance()(0,0)) 
                    << " y: " << pixelPrepCluster->localPosition()[1] << 
        " +/- " << sqrt(pixelPrepCluster->localCovariance()(1,1)) << endmsg;
    msg(MSG::DEBUG) << " Final position x: " << finalposition[0] << 
        " +/- " << sqrt(finalerrormatrix(0,0)) << 
        " y: " << finalposition[1] << " +/- " << 
        sqrt(finalerrormatrix(1,1)) << endmsg;
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
    Amg::MatrixX* newCov = m_errorScalingTool->createScaledPixelCovariance(cov,element->identify());
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

bool InDet::PixelClusterOnTrackTool::getErrorsDefaultAmbi( const InDet::PixelCluster* pixelPrepCluster, const Trk::TrackParameters& trackPar, 
                           Amg::Vector2D& finalposition,  Amg::MatrixX&  finalerrormatrix ) const
{

  std::vector<Amg::Vector2D> vectorOfPositions;
  int numberOfSubclusters=1;
  vectorOfPositions.push_back(pixelPrepCluster->localPosition());
  
  InDet::PixelGangedClusterAmbiguities::const_iterator mapBegin=m_splitClusterMap->begin();
  InDet::PixelGangedClusterAmbiguities::const_iterator mapEnd=m_splitClusterMap->end();

  for (InDet::PixelGangedClusterAmbiguities::const_iterator mapIter=mapBegin;mapIter!=mapEnd;++mapIter)
  {
    const SiCluster* first=(*mapIter).first;
    const SiCluster* second=(*mapIter).second;
    if (first == pixelPrepCluster  && second != pixelPrepCluster)
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
        msg(MSG::DEBUG) << "Found one more pixel cluster. Position x: " 
                        << pixelAddCluster->localPosition()[0] << "y: " << pixelAddCluster->localPosition()[1] << endmsg;
      }
    }//find relevant element of map
  }//iterate over map

  // A.S. hack for the moment: isSplit is also set for modified (non-split) 1-pixel-clusters ... is this needed anyway ?
  //
  //if (pixelPrepCluster->isSplit() && numberOfSubclusters<2)
  //{
  //msg(MSG::WARNING) << " Cluster is split but no further clusters found in split cluster map." << endmsg;
  //}
  
  //now you have numberOfSubclusters and the vectorOfPositions (Amg::Vector2D)

  const Trk::AtaPlane* parameters=dynamic_cast<const Trk::AtaPlane*>(&trackPar);
  if (parameters==0)
  {
    msg(MSG::WARNING) << "Parameters are not at a plane ! Aborting cluster correction... " << endmsg;
    return false;
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
      msg(MSG::DEBUG) << " Cluster cannot be treated by NN. Giving back to default clusterization " << endmsg;
    }
    return false;
  }

  if (allLocalPositions.size() != size_t(numberOfSubclusters))
  {
    msg(MSG::WARNING) << "Returned position vector size " << allLocalPositions.size() << " not according to expected number of subclusters: " << numberOfSubclusters << ". Abort cluster correction..." << endmsg;
    return false;
  }
  

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
        square(vectorOfPositions[0][0]-allLocalPositions[0][0])/allErrorMatrix[0](0,0)+
        square(vectorOfPositions[1][0]-allLocalPositions[1][0])/allErrorMatrix[1](0,0)+
        square(vectorOfPositions[0][1]-allLocalPositions[0][1])/allErrorMatrix[0](1,1)+
        square(vectorOfPositions[1][1]-allLocalPositions[1][1])/allErrorMatrix[1](1,1);

    double distancesq2=
        square(vectorOfPositions[1][0]-allLocalPositions[0][0])/allErrorMatrix[0](0,0)+
        square(vectorOfPositions[0][0]-allLocalPositions[1][0])/allErrorMatrix[1](0,0)+
        square(vectorOfPositions[1][1]-allLocalPositions[0][1])/allErrorMatrix[0](1,1)+
        square(vectorOfPositions[0][1]-allLocalPositions[1][1])/allErrorMatrix[1](1,1);

    if(msgLvl(MSG::DEBUG))
    {
      msg(MSG::DEBUG) << " Old pix (1) x: " << vectorOfPositions[0][0] << " y: " << vectorOfPositions[0][1] << endmsg;
      msg(MSG::DEBUG) << " Old pix (2) x: " << vectorOfPositions[1][0] << " y: " << vectorOfPositions[1][1] << endmsg;
      msg(MSG::DEBUG) << " Pix (1) x: " << allLocalPositions[0][0] << " +/- " << sqrt(allErrorMatrix[0](0,0)) 
                      << " y: " << allLocalPositions[0][1] << " +/- " << sqrt(allErrorMatrix[0](1,1)) << endmsg;
      msg(MSG::DEBUG) << " Pix (2) x: " << allLocalPositions[1][0] << " +/- " << sqrt(allErrorMatrix[1](0,0)) 
                      << " y: " << allLocalPositions[1][1] << " +/- " << sqrt(allErrorMatrix[1](1,1)) << endmsg;
      msg(MSG::DEBUG) << " Old (1) new (1) dist: " << sqrt(distancesq1) << " Old (1) new (2) " << sqrt(distancesq2) << endmsg;
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
      ATH_MSG_VERBOSE( " distance n.: " << i << " distance is: " << distances[i] );
      
      if (distances[i]<minDistance)
      {
        minDistance=distances[i];
        smallestDistanceIndex=i;
      }
    }
    
    ATH_MSG_DEBUG( " The minimum distance is : " << minDistance << " for index: " << smallestDistanceIndex );
      
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
  return true;
}

bool InDet::PixelClusterOnTrackTool::getErrorsTIDE_Ambi( const InDet::PixelCluster* pixelPrepCluster, 
							 const Trk::TrackParameters& trackPar,  
							 Amg::Vector2D& finalposition,  
							 Amg::MatrixX&  finalerrormatrix ) const
{
  std::vector<Amg::Vector2D> vectorOfPositions;
  int numberOfSubclusters = 1 + m_splitClusterMap->count( pixelPrepCluster );
    if(m_splitClusterMap->count( pixelPrepCluster )==0 && pixelPrepCluster->isSplit()) {
      numberOfSubclusters = 2;
  }
   if(m_splitClusterMap->count( pixelPrepCluster )!=0 && !pixelPrepCluster->isSplit()) {
      numberOfSubclusters = 1;
  }    
  //now you have numberOfSubclusters and the vectorOfPositions (Amg::Vector2D)

  const Trk::AtaPlane* parameters=dynamic_cast<const Trk::AtaPlane*>(&trackPar);
  if (parameters==0)
  {
    ATH_MSG_WARNING( "Parameters are not at a plane ! Aborting cluster correction... " );
    return false;
  }
  
  std::vector<Amg::Vector2D>     allLocalPositions;
  std::vector<Amg::MatrixX>      allErrorMatrix;

  allLocalPositions=
      m_NnClusterizationFactory->estimatePositions(*pixelPrepCluster,
                                                   parameters->associatedSurface(),
                                                   *parameters,
                                                   allErrorMatrix,
                                                   numberOfSubclusters);
  
  if (allLocalPositions.empty())
  {
    ATH_MSG_DEBUG(" Cluster cannot be treated by NN. Giving back to default clusterization, too big: " << pixelPrepCluster->tooBigToBeSplit());
    return false;
  }

  if (allLocalPositions.size() != size_t(numberOfSubclusters))
  {
    ATH_MSG_WARNING( "Returned position vector size " << allLocalPositions.size() << " not according to expected number of subclusters: " << numberOfSubclusters << ". Abort cluster correction..." );
    return false;
  }
  
  bool correctdR = false;
  std::vector<float>      correctiondR(3,0.);
  if(m_applydRcorrection && m_dRMap && m_dRMap->size()>0 && pixelPrepCluster->isSplit() ){
    const InDetDD::SiDetectorElement* det = pixelPrepCluster->detectorElement(); 
    if (det->isBarrel()){
        int pixelLayer = m_pixelid->layer_disk(det->identify());

        float correctiondRX = 1.0; // correction factor to be applied to covariance(!)
        float correctiondRZ = 1.0; // correction factor to be applied to covariance(!)
        
        const InDetDD::PixelModuleDesign* design (dynamic_cast<const InDetDD::PixelModuleDesign*>(&det->design()));
        if (not design) return false;
        float pitchX = design->phiPitch();
        float pitchZ = design->etaPitch();

        ATH_MSG_DEBUG("PixelClusterOnTrackTool: Trying to apply dR correction for pixelLayer " << pixelLayer);

        InDet::DRMap::const_iterator it = m_dRMap->find(pixelPrepCluster);
        if(it!=m_dRMap->end()){
            correctdR = true;
            float mindX = it->second.first;
            float mindZ = it->second.second;
            float mindXOverPitch = mindX / pitchX;
            float mindZOverPitch = mindZ / pitchZ;
            float dR = std::sqrt(mindX*mindX + mindZ*mindZ);
            
            ATH_MSG_DEBUG( " ---- Min dX ---- " << mindX << " mindZ " << mindZ);
            
            if(dR > 0. && dR < 2.0) {
                correctiondRX = splineIBLPullX(mindXOverPitch, pixelLayer);
                correctiondRZ = splineIBLPullX(mindZOverPitch, pixelLayer);
                
                if(correctiondRX > 3.75) correctiondRX = 3.75;
                if(correctiondRZ > 3.75) correctiondRZ = 3.75;
            }
            else{
                correctiondRX = 2.35;
                correctiondRZ = 1.70;
            }
        
            ATH_MSG_DEBUG("PixelClusterOnTrackTool: Correction factor calculation for distX/pitch= " << mindXOverPitch << " -> " << correctiondRX <<  " distZ/pitch= " << mindZOverPitch << " -> " << correctiondRZ);
        }
        else{
            ATH_MSG_WARNING("Split Pixel cluster not found in dRmap! " << pixelPrepCluster);
        }
    
        ATH_MSG_DEBUG( " ++++ Hit Error ++++ Layer " << pixelLayer << " Correction Flag " << m_applydRcorrection << " Split " <<pixelPrepCluster->isSplit() << " Scaling " << correctiondRX << " " << correctiondRZ);
    
        correctiondRX *= correctiondRX;
        correctiondRZ *= correctiondRZ;
        
        correctiondR[0] = correctiondRX*correctiondRX;
        correctiondR[1] = correctiondRZ*correctiondRZ;
        correctiondR[2] = correctiondRX*correctiondRZ;
    }
  }
  // AKM: now the not so nice part find the best match position option
  // Takes the error into account to scale the importance of the measurement

  if (numberOfSubclusters==1){
    finalposition=allLocalPositions[0];
    if(correctdR){
      allErrorMatrix[0](0,0) *= correctiondR[0];
      allErrorMatrix[0](1,1) *= correctiondR[1];
      allErrorMatrix[0](0,1) *= correctiondR[2];
      allErrorMatrix[0](1,0) *= correctiondR[2];
      ATH_MSG_DEBUG( " ++++ Hit Error ++++ " <<  numberOfSubclusters << " Split " << pixelPrepCluster->isSplit() << " Scaling " << sqrt(correctiondR[0]) << " " << sqrt(correctiondR[1]) << " Rescaled ErrX " << sqrt(allErrorMatrix[0](0,0)) << " Rescaled ErrZ " << sqrt(allErrorMatrix[0](1,1))); 
    }

    finalerrormatrix=allErrorMatrix[0]; 
    return true;
  }

  //Get the track parameters local position
  const Amg::Vector2D localpos =  trackPar.localPosition();
  //Use the track parameters cov to weight distcances 
  Amg::Vector2D localerr( 0.01,0.05 );  
  if( trackPar.covariance() ){
    localerr = Amg::Vector2D( sqrt( (*trackPar.covariance())(0,0) ), sqrt( (*trackPar.covariance())(1,1) ) );
  } 

  double minDistance(1e300);
  int index(0);
  
  for(unsigned int i(0); i < allLocalPositions.size(); ++i)
  {
    double distance=
        square(localpos[0]-allLocalPositions[i][0])/localerr[0]
        + square(localpos[1]-allLocalPositions[i][1])/localerr[1];
        
    if(distance<minDistance){
      index = i;
      minDistance = distance;     
    }
  }

  finalposition=allLocalPositions[index];

  if(correctdR){
    allErrorMatrix[index](0,0) *= correctiondR[0];
    allErrorMatrix[index](1,1) *= correctiondR[1];
    allErrorMatrix[index](0,1) *= correctiondR[2];
    allErrorMatrix[index](1,0) *= correctiondR[2];
     ATH_MSG_DEBUG( " ++++ Hit Error ++++ " <<  numberOfSubclusters << " Split " << pixelPrepCluster->isSplit() << " Scaling " << sqrt(correctiondR[0]) << " " << sqrt(correctiondR[1]) << " Rescaled ErrX " << sqrt(allErrorMatrix[index](0,0)) << " Rescaled ErrZ " << sqrt(allErrorMatrix[index](1,1))); 
  }

  finalerrormatrix=allErrorMatrix[index];
  return true;

}

double InDet::PixelClusterOnTrackTool::splineIBLPullX(float x, int layer) const {

   const int fNp = m_fY[layer].size();
   const int fKstep = 0;
   const double fDelta = -1;
   const double fXmin = *std::min_element( std::begin(m_fX[layer]), std::end(m_fX[layer]) );
   const double fXmax = *std::max_element( std::begin(m_fX[layer]), std::end(m_fX[layer]) );

   int klow=0;
   if(x<=fXmin) klow=0;
   else if(x>=fXmax) klow=fNp-1;
   else {
     if(fKstep) {
       // Equidistant knots, use histogramming
       klow = int((x-fXmin)/fDelta);
       if (klow < fNp-1) klow = fNp-1;
     } else {
       int khig=fNp-1, khalf;
       // Non equidistant knots, binary search
       while(khig-klow>1)
         if(x>m_fX[layer][khalf=(klow+khig)/2]) klow=khalf;
         else khig=khalf;
     }
   }
   // Evaluate now
   double dx=x-m_fX[layer][klow];
   return (m_fY[layer][klow]+dx*(m_fB[layer][klow]+dx*(m_fC[layer][klow]+dx*m_fD[layer][klow])));
}
