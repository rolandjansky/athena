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
#include "EventPrimitives/EventPrimitivesHelpers.h"

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
  const double TOPHAT_SIGMA=1./sqrt(12.);
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
  m_noNNandBroadErrors(false),
  m_usingTIDE_Ambi(false),
  m_useCentroidPosition(false),
  m_correctLorentzShift(true),
  m_enableTheta(false),
  m_returnUnchanged(false),
  m_correctDigitalCentroid(false),
  m_minClusterSize(0)
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
  declareProperty("NNIBLcorrection",          m_NNIBLcorrection);
  declareProperty("EventStore",               m_storeGate );
  declareProperty("IncidentService",          m_incidentSvc );
  declareProperty("NnClusterizationFactory",  m_NnClusterizationFactory);
  declareProperty("SplitClusterAmbiguityMap", m_splitClusterMapName);
  declareProperty("doNotRecalibrateNN",       m_doNotRecalibrateNN);
  declareProperty("m_noNNandBroadErrors",     m_noNNandBroadErrors);
  declareProperty("RunningTIDE_Ambi",         m_usingTIDE_Ambi);
  declareProperty("UseCentroidPosition",      m_useCentroidPosition);
  declareProperty("CorrectLorentzShift",      m_correctLorentzShift);
  declareProperty("EnableTheta",              m_enableTheta);
  declareProperty("ReturnUnchanged",          m_returnUnchanged);
  declareProperty("CorrectDigitalCentroid",   m_correctDigitalCentroid); 
  declareProperty("MinClusterSize",           m_minClusterSize);

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
   ATH_MSG_INFO( name() << " initialize()");   
   ATH_MSG_DEBUG( "Error strategy is" << m_errorStrategy );
   ATH_MSG_DEBUG( "Release 13 flag is" << m_rel13like);

   if (m_IBLParameterSvc.retrieve().isFailure()) { 
     ATH_MSG_WARNING( "Could not retrieve IBLParameterSvc"); 
   } else { 
     m_IBLParameterSvc->setBoolParameters(m_applyNNcorrection,"doPixelClusterSplitting");
     m_IBLParameterSvc->setBoolParameters(m_IBLAbsent,"IBLAbsent");
   } 
   
   //get the offline calibration service
   sc = m_calibSvc.retrieve();
   if (sc.isFailure() || !m_calibSvc) {
      ATH_MSG_ERROR( m_calibSvc.type() << " not found! ");
   } else{
      ATH_MSG_INFO( "Retrieved tool " <<  m_calibSvc.type());
   }

   // get the error scaling tool
  if ( m_errorScalingTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_errorScalingTool << endreq;
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "Retrieved tool " << m_errorScalingTool);
    m_scalePixelCov = m_errorScalingTool->needToScalePixel();
    if (m_scalePixelCov) ATH_MSG_DEBUG( "Detected need for scaling Pixel errors." );
  }
 
  // the NN corrections
  if ( m_applyNNcorrection && m_NnClusterizationFactory.retrieve().isFailure()){
      msg(MSG::ERROR) << "Can't get " << m_NnClusterizationFactory << endreq;
      return StatusCode::FAILURE;
  } else if (m_applyNNcorrection)
      ATH_MSG_INFO( "Retrieved " <<  m_NnClusterizationFactory << " for NN corrections." );
  
 
  // get the module distortions tool
  if (!m_disableDistortions) {
    if  (!m_pixDistoTool.empty()) {
      sc = m_pixDistoTool.retrieve();
      if ( sc != StatusCode::SUCCESS ) {
        msg(MSG::ERROR) << "Can't get pixel distortions tool " << endreq;
      } else {
        ATH_MSG_INFO( "Pixel distortions tool retrieved" );
      }
    } else {
      ATH_MSG_INFO( "No PixelDistortionsTool selected." );
    }
  } else{
    ATH_MSG_INFO( "No PixelDistortions will be simulated." );
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

 
void InDet::PixelClusterOnTrackTool::handle(const Incident& inc){
  if ( inc.type() == "EndEvent" ){
    ATH_MSG_VERBOSE("'EndEvent' incident caught. Refreshing Cache.");
    m_splitClusterMap = 0;
  }     
}


///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::PixelClusterOnTrackTool::finalize(){
  return AlgTool::finalize();
}

///////////////////////////////////////////////////////////////////
// Trk::SiClusterOnTrack  production
///////////////////////////////////////////////////////////////////


const InDet::PixelClusterOnTrack* InDet::PixelClusterOnTrackTool::correct
  (const Trk::PrepRawData& rio,const Trk::TrackParameters& trackPar) const
{
  if (m_returnUnchanged)
    return correctUnchanged(rio, trackPar);

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
    int i = 0;
    float totalChargeNorm = 0.;
    const std::vector<int>& chListRecreated = pix->totList();
    for(; oneRDO != rdos.end(); oneRDO++, i++){
      float charge = m_useCentroidPosition ? chListRecreated[i] : 0.;
      totalChargeNorm += charge;
      Identifier rId = *oneRDO;
      int row = m_pixelid->phi_index(rId);
      int col = m_pixelid->eta_index(rId);
      if(rowmin > row) rowmin = row;
      if(rowmax < row) rowmax = row;
      if(colmin > col) colmin = col;
      if(colmax < col) colmax = col;
      meanpos += m_useCentroidPosition ? design->positionFromColumnRow(col,row)*charge : design->positionFromColumnRow(col,row);
    }
    meanpos = m_useCentroidPosition ? meanpos/totalChargeNorm : meanpos/rdos.size();

    if(element->isBarrel() && m_correctDigitalCentroid && pix->width().colRow()[1]>m_minClusterSize) {
      int colWidth = colmax-colmin+1;
      int rowWidth = rowmax-rowmin+1;
      const int numberOfRows = design->rows();
      const int numberOfColumns = design->columns();
      //*** correction on local X : digital average of the most populated row and its most populated neighbor ***
      if(rowWidth>1){  //only modify the localX of clusters with phiWidth>1
        int *NumberHitsEachRow = new int[numberOfRows](); // all elements initialized to zero
        std::vector<Identifier>::const_iterator tpRDO = rdos.begin();
        for(; tpRDO != rdos.end(); tpRDO++){
          Identifier tpId = *tpRDO;
          int row = m_pixelid->phi_index(tpId);
          for(int ii=rowmin; ii<=rowmax;ii++){
            if(row==ii ) {
              NumberHitsEachRow[ii]++;
              break;
            }
          }
        }
        //find out the row with highest number of fired pixels
        int refmax(-9999); int row1(-9999); int row2(-9999);
        for(int ii=rowmin; ii<=rowmax;ii++){
          if((NumberHitsEachRow[ii])>refmax) {
            refmax = NumberHitsEachRow[ii];
            row1 = ii;
          }
        }
        //find out the neighboring row with the second highest number of hits
        if((row1-1)>=rowmin && (row1+1)<=rowmax ) {//make sure they are valid rows
          if(NumberHitsEachRow[row1-1] > NumberHitsEachRow[row1+1] ) row2 = row1-1;
          else row2 = row1+1;
        }
        else if((row1-1)>=rowmin && (row1+1)>rowmax ) row2 = row1-1;
        else if((row1-1)<rowmin && (row1+1)<=rowmax ) row2 = row1+1;
         
        InDetDD::SiLocalPosition avgPos(0,0,0);
        std::vector<Identifier>::const_iterator TpRDO = rdos.begin();
        for(; TpRDO != rdos.end(); TpRDO++){
          Identifier tpId = *TpRDO;
          int row = m_pixelid->phi_index(tpId);
          int col = m_pixelid->eta_index(tpId);
          if(row==row1 || row==row2 ) avgPos += design->positionFromColumnRow(col,row);
        }
        avgPos = avgPos / (NumberHitsEachRow[row1]+NumberHitsEachRow[row2]);
        InDetDD::SiLocalPosition tp_meanpos(meanpos.xEta(), avgPos.xPhi(), meanpos.xDepth());
        meanpos = tp_meanpos;
        delete[] NumberHitsEachRow;
      }
      //*** correction on local Y : (col_Min + col_Max)/2 ***
      InDetDD::SiLocalPosition left_pos = design->positionFromColumnRow(colmin,rowmin);
      InDetDD::SiLocalPosition right_pos = design->positionFromColumnRow(colmax,rowmin);
      InDetDD::SiLocalPosition middle_pos = (left_pos+right_pos)/2.0;
      InDetDD::SiLocalPosition tp_meanpos(middle_pos.xEta(), meanpos.xPhi(), meanpos.xDepth());
      meanpos = tp_meanpos;
      //*** correct local Y for clusters on module edge ***
      if (colmin <= 0 || colmax >= (numberOfColumns-1) ) {
        Amg::Vector3D globalPos = element->globalPosition(meanpos);
        int expectedClusterLength = element->thickness() / design->etaPitch() * fabs(globalPos.z()) / globalPos.perp() + 1;
        if( (colWidth >= expectedClusterLength/2.) && (colWidth <= expectedClusterLength) ){
          if (colmin <= 0)  
            meanpos += InDetDD::SiLocalPosition( design->etaPitch()*(colWidth - expectedClusterLength) / 2., 0.);
          else if (colmax >= (numberOfColumns-1)) 
            meanpos += InDetDD::SiLocalPosition( design->etaPitch()*(expectedClusterLength - colWidth) / 2., 0.);
        }
      }
    } // end if isBarrel and m_correctDigitalCentroid

    InDetDD::SiLocalPosition pos1 = 
         design->positionFromColumnRow(colmin,rowmin);
    InDetDD::SiLocalPosition pos2 = 
         design->positionFromColumnRow(colmax,rowmin);
    InDetDD::SiLocalPosition pos3 = 
         design->positionFromColumnRow(colmin,rowmax);
    InDetDD::SiLocalPosition pos4 = 
         design->positionFromColumnRow(colmax,rowmax);

    InDetDD::SiLocalPosition centroid = 0.25*(pos1+pos2+pos3+pos4); 
    double shift = m_correctLorentzShift ? element->getLorentzCorrection() : 0.;
    int nrows = rowmax-rowmin+1;
    int ncol = colmax-colmin+1;
    double ang = 999.;
    double eta = 999.;

    // ATH_MSG_VERBOSE ( << "Position strategy = " << m_positionStrategy << "omegaphi = " << omegaphi );

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

    } else if (m_errorStrategy == 55) { // ErrorStrategy for the centroid method

      double eta = fabs(PixTrkEta);
      ATH_MSG_DEBUG ( "Track eta = " << eta );
      std::vector < double > etas =             {0.,   0.8,   1.6,   2.4,   3.2,   4.0};
      
      // get the needed bin in eta
      unsigned bin = 0;
      if (eta>etas.back()) bin = 4;
      else {
	for (;bin<etas.size();bin++) 
	  if (eta>etas.at(bin) and eta<etas.at(bin+1))
	    break;
      }
            
      std::vector < double > onehitx_errx_barrel  = {12.64, 13.01, 14.06, 19.38, 19.38};
      std::vector < double > onehity_errx_barrel  = {12.58, 12.58, 12.58, 12.58, 12.58};

      std::vector < double > onehitx_errx_endcap  = {14.47, 14.47, 14.47, 15.51, 17.13};
      std::vector < double > onehity_errx_endcap  = {14.47, 14.47, 14.47, 15.51, 17.13};

      std::vector < double > onehitx_erry_barrel  = { 9.35, 11.17, 19.87, 43.40, 43.40};
      std::vector < double > onehity_erry_barrel  = {12.58, 12.58, 12.58, 12.58, 12.58};
      
      std::vector < double > onehitx_erry_endcap  = { 8.89,  8.89,  8.89, 10.77, 14.44};
      std::vector < double > onehity_erry_endcap  = { 8.89,  8.89,  8.89, 10.77, 14.44};

      std::vector < double > twohitx_errx_barrel  = {15.25, 14.13, 16.46, 20.37, 20.37};
      std::vector < double > twohity_errx_barrel  = {13.69, 13.15, 14.97, 19.80, 19.80};

      std::vector < double > twohitx_errx_endcap  = {26.47, 26.47, 26.47, 26.47, 26.47};
      std::vector < double > twohity_errx_endcap  = {15.57, 15.57, 15.57, 17.20, 20.82};

      std::vector < double > twohitx_erry_barrel  = {12.64, 18.01, 31.26, 33.23, 33.23};
      std::vector < double > twohity_erry_barrel  = { 9.33, 13.43, 24.79, 24.80, 24.80};
      
      std::vector < double > twohitx_erry_endcap  = {24.56, 24.56, 24.56, 24.56, 24.56};
      std::vector < double > twohity_erry_endcap  = { 9.56,  9.56,  9.56, 13.11, 15.38};
      
      if (element->isBarrel()) {
	if (ncol==1 and nrows==1) { 
	  errphi = 0.55*sqrt((0.001*onehitx_errx_barrel.at(bin))*(0.001*onehitx_errx_barrel.at(bin))+
			(0.001*onehity_errx_barrel.at(bin))*(0.001*onehity_errx_barrel.at(bin)));
	  erreta = 0.4*sqrt((0.001*onehitx_erry_barrel.at(bin))*(0.001*onehitx_erry_barrel.at(bin))+
			(0.001*onehity_erry_barrel.at(bin))*(0.001*onehity_erry_barrel.at(bin)));	    
	} else if (ncol==1) {
	  errphi = 0.55*sqrt((0.001*twohitx_errx_barrel.at(bin))*(0.001*twohitx_errx_barrel.at(bin))+
			(0.001*onehity_errx_barrel.at(bin))*(0.001*onehity_errx_barrel.at(bin)));
	  erreta = 0.4*sqrt((0.001*twohitx_erry_barrel.at(bin))*(0.001*twohitx_erry_barrel.at(bin))+
			(0.001*onehity_erry_barrel.at(bin))*(0.001*onehity_erry_barrel.at(bin)));	    	  
	} else if (nrows==1) {
	  errphi = 0.55*sqrt((0.001*onehitx_errx_barrel.at(bin))*(0.001*onehitx_errx_barrel.at(bin))+
			(0.001*twohity_errx_barrel.at(bin))*(0.001*twohity_errx_barrel.at(bin)));
	  erreta = 0.25*sqrt((0.001*onehitx_erry_barrel.at(bin))*(0.001*onehitx_erry_barrel.at(bin))+
			(0.001*twohity_erry_barrel.at(bin))*(0.001*twohity_erry_barrel.at(bin)));	    
	} else {
	  errphi = 0.55*sqrt((0.001*twohitx_errx_barrel.at(bin))*(0.001*twohitx_errx_barrel.at(bin))+
			(0.001*twohity_errx_barrel.at(bin))*(0.001*twohity_errx_barrel.at(bin)));
	  erreta = 0.25*sqrt((0.001*twohitx_erry_barrel.at(bin))*(0.001*twohitx_erry_barrel.at(bin))+
			(0.001*twohity_erry_barrel.at(bin))*(0.001*twohity_erry_barrel.at(bin)));	    
	}
      } else {
	if (ncol==1 and nrows==1) { 
	  errphi = 0.62*sqrt((0.001*onehitx_errx_endcap.at(bin))*(0.001*onehitx_errx_endcap.at(bin))+
			(0.001*onehity_errx_endcap.at(bin))*(0.001*onehity_errx_endcap.at(bin)));
	  erreta = 0.58*sqrt((0.001*onehitx_erry_endcap.at(bin))*(0.001*onehitx_erry_endcap.at(bin))+
			(0.001*onehity_erry_endcap.at(bin))*(0.001*onehity_erry_endcap.at(bin)));	    
	} else if (ncol==1) {
	  errphi = 0.62*sqrt((0.001*twohitx_errx_endcap.at(bin))*(0.001*twohitx_errx_endcap.at(bin))+
			(0.001*onehity_errx_endcap.at(bin))*(0.001*onehity_errx_endcap.at(bin)));
	  erreta = 0.58*sqrt((0.001*twohitx_erry_endcap.at(bin))*(0.001*twohitx_erry_endcap.at(bin))+
			(0.001*onehity_erry_endcap.at(bin))*(0.001*onehity_erry_endcap.at(bin)));	    	  
	} else if (nrows==1) {
	  errphi = 0.62*sqrt((0.001*onehitx_errx_endcap.at(bin))*(0.001*onehitx_errx_endcap.at(bin))+
			(0.001*twohity_errx_endcap.at(bin))*(0.001*twohity_errx_endcap.at(bin)));
	  erreta = 0.58*sqrt((0.001*onehitx_erry_endcap.at(bin))*(0.001*onehitx_erry_endcap.at(bin))+
			(0.001*twohity_erry_endcap.at(bin))*(0.001*twohity_erry_endcap.at(bin)));	    
	} else {
	  errphi = 0.62*sqrt((0.001*twohitx_errx_endcap.at(bin))*(0.001*twohitx_errx_endcap.at(bin))+
			(0.001*twohity_errx_endcap.at(bin))*(0.001*twohity_errx_endcap.at(bin)));
	  erreta = 0.58*sqrt((0.001*twohitx_erry_endcap.at(bin))*(0.001*twohitx_erry_endcap.at(bin))+
			(0.001*twohity_erry_endcap.at(bin))*(0.001*twohity_erry_endcap.at(bin)));	    
	}
      }
    }  else if (m_errorStrategy == 56) { // ErrorStrategy for the digital clustering for extended
      
      float resixerrPhiEta[3][20] = { { 12.8155, 12.4459, 11.8122, 11.3227, 11.6338,  11.969, 11.6714, 11.4465, 11.9577, 13.3711, 12.1114,  13.653, 14.0032, 17.1381, 17.7182,  23.757,  27.102, 35.4813, 41.7938 }, 
				      { 8.50122, 8.88776, 9.34805, 8.33644, 8.20739, 7.79424,  8.4458, 8.22095, 8.19467,  8.5154,  8.3079, 9.94034, 9.33012, 10.4806, 13.5793, 19.0882, 23.3954,  30.473, 40.8789 }, 
				      { 38.3358, 37.6715, 36.8202, 31.8976, 25.1102, 22.4736, 23.4847, 18.5858,  18.073, 12.9081, 13.5599, 17.9827, 14.1882, 14.0561, 16.4488, 20.7201, 26.3962, 35.4345, 41.0186 }  };
      float resiyerrPhiEta[3][20] = { { 12.1206, 12.9119, 22.2239, 24.2145, 26.1517, 28.3423,  29.572, 33.0003, 35.2121, 36.7793, 36.7461, 38.6023, 41.9922,  44.336, 47.8194, 55.2781, 60.6739,  67.178, 59.8876 }, 
				      { 10.9868, 12.7898, 19.9045, 23.8799, 25.5273, 29.5437, 31.3188, 33.1777, 36.6881, 39.9433, 38.1606, 44.1598, 47.7479, 49.9697, 56.8268, 64.4802,  73.852, 78.5667, 77.4028 }, 
				      { 9.6642,  18.4506, 30.5653, 49.0731, 54.1141, 47.7152, 53.8129, 37.0657, 44.6385, 52.8298,  44.955, 31.8936, 55.2857, 52.1608, 57.3309, 71.8902, 73.1629, 76.9447, 78.2209 }  };
      
      const InDet::SiWidth width = pix->width();
      int iphi = int(width.colRow().x())-1;
      int ieta = int(width.colRow().y())-1;
      
      if(iphi > 2) iphi = 3;
      if(ieta > 50) ieta = 19;
      else if(ieta > 30) ieta = 18;
      else if(ieta > 25) ieta = 17;
      else if(ieta > 20) ieta = 16;
      else if(ieta > 15) ieta = 15;
      
      errphi = (0.001*resixerrPhiEta[iphi][ieta]);
      erreta = (0.001*resiyerrPhiEta[iphi][ieta]);

    } else if (m_errorStrategy == 57) { // ErrorStrategy for the digital clustering for inclined
      
      float resixerrPhiEta[3][20] = { {14.7267, 12.978, 11.8348, 12.9027, 13.0772, 13.4423, 13.8713, 13.0597, 14.4129, 13.6007, 14.3261, 14.8391, 15.3077, 14.589, 14.7879, 12.0581, 12.0581, 12.0581, },
					{11.5659, 11.7667, 12.195, 12.7977, 12.4252, 11.5971, 10.4044, 11.5002, 11.9504, 12.2448, 11.6027, 12.4971, 13.5084, 12.7884, 11.8548, 10.0407, 10.0407, 10.0407, },
					{25.9162, 29.4513, 27.5037, 28.5633, 27.4287, 30.7974, 30.9356, 35.0998, 32.3591, 33.8183, 15.2461, 39.1667, 35.2378, 18.0494, 25.0996, 56.6821, 56.6821, 56.6821, },};
					
	float resiyerrPhiEta[3][20] = { {11.3189, 7.03764, 4.1913, 6.84301, 9.40462, 10.252, 14.9464, 15.281, 16.3515, 14.8884, 16.8228, 18.1884, 18.7202, 15.5909, 20.1345, 18.3459, 18.3459, 18.3459, },
					{11.3641, 6.00973, 6.39404, 15.9256, 17.6599, 19.7328, 18.5501, 20.0178, 20.2986, 21.2614, 22.8865, 19.9454, 26.7562, 21.626, 34.1667, 29.2284, 29.2284, 29.2284, },
					{11.8251, 14.8126, 20.0304, 33.5938, 41.3037, 53.4389, 70.4089, 68.3941, 57.7403, 66.769, 51.7588, 74.5818, 52.4853, 41.3343, 79.2477, 51.9132, 51.9132, 51.9132, },};


      const InDet::SiWidth width = pix->width();
      int iphi = int(width.colRow().x())-1;
      int ieta = int(width.colRow().y())-1;
      
      if(iphi > 2) iphi = 3;
      if(ieta > 50) ieta = 19;
      else if(ieta > 30) ieta = 18;
      else if(ieta > 25) ieta = 17;
      else if(ieta > 20) ieta = 16;
      else if(ieta > 15) ieta = 15;
      
      errphi = (0.001*resixerrPhiEta[iphi][ieta]);
      erreta = (0.001*resiyerrPhiEta[iphi][ieta]);
      
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

  ATH_MSG_DEBUG ("m_enableTheta, width, isBarrel, layer = " << m_enableTheta << "   -   " << pix->width().colRow()[1] << "   -   " << element->isBarrel() << "   -   " << m_pixelid->layer_disk(element->identify()) );
  
  // this works if you have a barrel cluster, m_enableTheta is true and the size of the cluster is > 10 pixels in the z direction
  if ( m_enableTheta and pix->width().colRow()[1]>10 and element->isBarrel()) twoDimToThreeDim(locpar, cov, *pix);
  
  ATH_MSG_DEBUG ("Creating PixelClusterOnTrack with locpar = " << locpar << " and cov = " << cov);

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
      msg(MSG::DEBUG) << " Cluster cannot be treated by NN. Giving back to default clusterization " << endreq;
    }
    return false;
  }

  if (allLocalPositions.size() != size_t(numberOfSubclusters))
  {
    msg(MSG::WARNING) << "Returned position vector size " << allLocalPositions.size() << " not according to expected number of subclusters: " << numberOfSubclusters << ". Abort cluster correction..." << endreq;
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



bool InDet::PixelClusterOnTrackTool::getErrorsTIDE_Ambi( const InDet::PixelCluster* pixelPrepCluster, const Trk::TrackParameters& trackPar,  
                        Amg::Vector2D& finalposition,  Amg::MatrixX&  finalerrormatrix ) const
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
  
  if (allLocalPositions.size()  == 0)
  {
    ATH_MSG_DEBUG(" Cluster cannot be treated by NN. Giving back to default clusterization, too big: " << pixelPrepCluster->tooBigToBeSplit());
    return false;
  }

  if (allLocalPositions.size() != size_t(numberOfSubclusters))
  {
    ATH_MSG_WARNING( "Returned position vector size " << allLocalPositions.size() << " not according to expected number of subclusters: " << numberOfSubclusters << ". Abort cluster correction..." );
    return false;
  }
  


  // AKM: now the not so nice part find the best match position option
  // Takes the error into account to scale the importance of the measurement

  if (numberOfSubclusters==1)
  {
    finalposition=allLocalPositions[0];
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
  finalerrormatrix=allErrorMatrix[index];
 
  return true;

}

void InDet::PixelClusterOnTrackTool::twoDimToThreeDim(Trk::LocalParameters& lpar, Amg::MatrixX& cov, const InDet::PixelCluster& pix) const 
{  
  const InDetDD::SiDetectorElement* element = pix.detectorElement(); 
  const InDetDD::PixelModuleDesign* design = 
    dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());

  std::vector<Identifier> rdos = pix.rdoList();
  std::vector<Identifier>::const_iterator oneRDO = rdos.begin();
  //int rowmin=9999; int rowmax=-9999;
  int colmin=9999; int colmax=-9999;
  int colmin_rowmin=9999; int colmax_rowmin=-9999;
  int colmin_rowmax=9999; int colmax_rowmax=-9999;
  
  //  make a copy
  Trk::LocalParameters oldlpar = lpar;
  Amg::MatrixX oldcov  = cov;

  // loop the first time:
  // get the min col and the max col
  for(std::vector<Identifier>::const_iterator oneRDO = rdos.begin(); oneRDO != rdos.end(); oneRDO++){
    Identifier rId = *oneRDO;
    int row = m_pixelid->phi_index(rId);
    int col = m_pixelid->eta_index(rId);
    ATH_MSG_DEBUG ( "Looping on (row, col) = (" << row << ", " << col << ")");
    if(colmin > col) colmin = col;
    if(colmax < col) colmax = col;   
  }
  
  // loop the second time:
  //for both min and max col you need to get the min and max row
  for(std::vector<Identifier>::const_iterator oneRDO = rdos.begin(); oneRDO != rdos.end(); oneRDO++){
    Identifier rId = *oneRDO;
    int col = m_pixelid->eta_index(rId);
    if (col == colmin) {
      int row = m_pixelid->phi_index(rId);
      ATH_MSG_DEBUG ( "Looping on (row, col) = (" << row << ", " << col << ")");
      if(colmin_rowmin > row) colmin_rowmin = row;
      if(colmax_rowmin < row) colmax_rowmin = row;
    } 
    if ( col == colmax) {
      int row = m_pixelid->phi_index(rId);
      ATH_MSG_DEBUG ( "Looping on (row, col) = (" << row << ", " << col << ")");
      if(colmin_rowmax > row) colmin_rowmax = row;
      if(colmax_rowmax < row) colmax_rowmax = row;
    }
  }
  
  ATH_MSG_DEBUG ( "Defined colmin, colmax, colmin_rowmin, colmax_rowmin, colmin_rowmax, colmax_rowmax = " << colmin << ", " << colmax << ", " << colmin_rowmin << ", " << colmax_rowmin << ", " << colmin_rowmax << ", " << colmax_rowmax);
  //ATH_MSG_DEBUG ( "Using pos1  = " << colmin << ", " << floor((colmin_rowmax+colmin_rowmin)/2.));
  //ATH_MSG_DEBUG ( "Using pos2  = " << colmax << ", " << floor((colmax_rowmax+colmax_rowmin)/2.));
  
  // now that you have the min and max rows for both min and max cols you can define the initial position and the last position
  // since you are only interested in theta you can weight the position between the max and the min rows 
  
  double initialLocX = 0.;
  double initialLocY = 0.;
  double finalLocX = 0.;
  double finalLocY = 0.;

  // If the module you have a cluster like this (X are the firing pixels)
  // Theta is evaluated considering the initial point and the final point in the silicon
  // To do this I have evaluated the average entry position and the average exit position
  // y (eta) : is easy since you have the min and the max column
  // x (phi) is averaged for min col and max col 
  // this is what is written below (reported in the draw):
  // pos_1 (colmin, colmin_rowmax) = ( 3, 4)
  // pos_3 (colmin, colmin_rowmin) = ( 3, 3)
  // pos_2 (colmax, colmax_rowmax) = (11, 4)
  // pos_4 (colmax, colmax_rowmin) = (11, 4)
  //
  // the average entry is ( 3, average(4,3,4,4)=3.75)
  // the average exit  is (11, average(4,3,4,4)=3.75)
  //
  // entry and exit will be then shifted up and down by 1/2 thickness
  // --> this requires you know if the cluster in in the positive or negative barrel
  //     since for negative barrel you need first to swap entry and exit
  //
  // positive barrel: entry is shifted down by 1/2 thickness 
  //                  exit  is shifted   up by 1/2 thickness 
  // 
  // positive barrel: swap(entry, exit)
  //                  entry is shifted down by 1/2 thickness 
  //                  exit  is shifted   up by 1/2 thickness 
  //
  //
  //     rows (phi)   
  //              ^ 
  //              | ____________________________
  //              | |_|_|_|_|_|_|_|_|_|_|_|_|_|_|
  //              | |_|_|_|_|_|_|X|X|_|_|_|_|_|_|
  //              | |_|_|X|X|X|X|X|X|X|X|X|_|_|_|
  //              | |_|_|X|X|X|X|X|X|X|X|_|_|_|_|
  //              | |_|_|_|_|X|X|_|_|_|_|_|_|_|_|
  //              | |_|_|_|_|_|_|_|_|_|_|_|_|_|_|
  //              |--------------------------------> cols (eta)
  //
  //
  //
  
  InDetDD::SiLocalPosition pos_1 = design->positionFromColumnRow(colmin,colmin_rowmax);
  InDetDD::SiLocalPosition pos_3 = design->positionFromColumnRow(colmin,colmin_rowmin);

  InDetDD::SiLocalPosition pos_2 = design->positionFromColumnRow(colmax,colmax_rowmax);
  InDetDD::SiLocalPosition pos_4 = design->positionFromColumnRow(colmax,colmax_rowmin);

  double min_x = (pos_1.xPhi()+pos_3.xPhi())*0.5;
  double max_x = (pos_2.xPhi()+pos_4.xPhi())*0.5;

  InDetDD::SiLocalPosition pos1(pos_1.xEta(),(min_x+max_x)*0.5);
  InDetDD::SiLocalPosition pos2(pos_2.xEta(),(min_x+max_x)*0.5);
  
  if (element->center().z()>0.) { // positive region on the barrel
    initialLocX = pos1.xPhi();
    initialLocY = pos1.xEta();
    finalLocX = pos2.xPhi();
    finalLocY = pos2.xEta();  
  } else {  // negative region on the barrel
    initialLocX = pos2.xPhi();
    initialLocY = pos2.xEta();
    finalLocX = pos1.xPhi();
    finalLocY = pos1.xEta();  
  }

  // shift the initial and the final positions accordingly  
  // initial is shifted down pf half thickness
  // final is shifted up pf half thickness
  Amg::Vector3D initial_loc(initialLocX, initialLocY, -0.5*element->thickness());
  Amg::Vector3D final_loc(finalLocX, finalLocY, 0.5*element->thickness());

  ATH_MSG_DEBUG ( "initial_local = " << initial_loc );
  ATH_MSG_DEBUG ( "final_local = " << final_loc );
  
  // converting to global coordinates
  Amg::Vector3D initial_global = element->globalPosition(initial_loc);
  Amg::Vector3D final_global   = element->globalPosition(final_loc);

  ATH_MSG_DEBUG ( "initial_global = " << initial_global );
  ATH_MSG_DEBUG ( "final_global = " << final_global );

  // using the global of the track
  Amg::Vector3D global = final_global - initial_global;

  ATH_MSG_DEBUG ( "global = " << global );

  double theta = global.theta();
  double phi = global.phi();

  ATH_MSG_DEBUG ( "Track Info (theta, phi) = " << theta << " " << phi );

  // to evaluate the error on theta you can move entry and exit of 1/2 pixel in eta
  // CASE A: is the nominal value
  // CASE B: theta increase
  // CASE C: theta decrease
  //
  //                                   BAC 
  // __________________________________|||_____________
  // |_|_|_|_|X|X|X|X|X|X|X|X|X|X|X|X|X|X|_|_|_|_|_|_|
  //         |||
  //         CAB
  //
  // sigma_theta = delta_theta = max (fabs(theta_A-theta_B),fabs(theta_A-theta_C))
  // 
  //
  

  // this applies on theta  --> shiftig the local Y +/- half pixel
  // CASE B
  Amg::Vector3D initial_loc_halfLess_theta(initialLocX, initialLocY-design->length()/design->columns()*0.5, -0.5*element->thickness());
  Amg::Vector3D final_loc_halfLess_theta(finalLocX, finalLocY+design->length()/design->columns()*0.5, 0.5*element->thickness());
  Amg::Vector3D initial_global_halfLess_theta = element->globalPosition(initial_loc_halfLess_theta);
  Amg::Vector3D final_global_halfLess_theta   = element->globalPosition(final_loc_halfLess_theta);
  Amg::Vector3D global_halfLess_theta = final_global_halfLess_theta - initial_global_halfLess_theta;
  double theta_halfLess = global_halfLess_theta.theta();
  
  //CASE C
  Amg::Vector3D initial_loc_halfMore_theta(initialLocX, initialLocY+design->length()/design->columns()*0.5, -0.5*element->thickness());
  Amg::Vector3D final_loc_halfMore_theta(finalLocX, finalLocY-design->length()/design->columns()*0.5, 0.5*element->thickness());
  Amg::Vector3D initial_global_halfMore_theta = element->globalPosition(initial_loc_halfMore_theta);
  Amg::Vector3D final_global_halfMore_theta   = element->globalPosition(final_loc_halfMore_theta);
  Amg::Vector3D global_halfMore_theta = final_global_halfMore_theta - initial_global_halfMore_theta;
  double theta_halfMore = global_halfMore_theta.theta();
    
  // sigma_theta = delta_theta = max (fabs(theta_A-theta_B),fabs(theta_A-theta_C))
  double delta_theta = std::max(fabs(theta_halfLess-theta),fabs(theta_halfMore-theta));
  
  ATH_MSG_DEBUG ( "Track Info (delta_theta) = " << delta_theta );
  
  // calculate your global phi/theta and sigmas of it
  double theta_lc       = theta;
  double sigma_theta_lc = delta_theta;
  
  // now create new ones which are 3-Dim
  
  // (1) local parameters
  std::vector<Trk::DefinedParameter> dPars;
  // first copy over
  dPars.push_back(Trk::DefinedParameter(oldlpar.get(Trk::locX), Trk::locX)); // copy the local x parameter
  dPars.push_back(Trk::DefinedParameter(oldlpar.get(Trk::locY), Trk::locY)); // copy the local y parameter
  // now add
  dPars.push_back(Trk::DefinedParameter(theta_lc, Trk::theta)); // add the global theta parameter
  
  // set to the new one
  lpar = Trk::LocalParameters(dPars);

  // (2) covariance
  Amg::MatrixX  newcov(3,3);
  newcov.setZero();
  // first copy over
  (newcov)(Trk::locX,Trk::locX) = (oldcov)(Trk::locX,Trk::locX); // copy the local x parameter
  (newcov)(Trk::locY,Trk::locY) = (oldcov)(Trk::locY,Trk::locY); // copy the local x parameter
  // now add 
  (newcov)(2,2) = sigma_theta_lc*sigma_theta_lc;
  
  // memory cleanup
  cov = newcov;
}


const InDet::PixelClusterOnTrack* InDet::PixelClusterOnTrackTool::correctUnchanged
(const Trk::PrepRawData& rio,const Trk::TrackParameters& ) const {

  const InDet::PixelCluster* pix = 0;
  if(!(pix = dynamic_cast<const InDet::PixelCluster*>(&rio))) return 0;

  const InDetDD::SiDetectorElement* el = pix->detectorElement(); 
  if(!el) return 0;
  
  IdentifierHash       iH   = el->identifyHash();
  Amg::Vector3D        glob  (pix->globalPosition());
  Trk::LocalParameters locpar(pix->localPosition());
  Amg::MatrixX         cov   (pix->localCovariance());
  bool                 isbroad = false;
  
  ATH_MSG_DEBUG("correctUnchanged returns : ");
  ATH_MSG_DEBUG("LocalParameters = " << locpar[Trk::locX] << " - " << locpar[Trk::locY]);
  ATH_MSG_DEBUG("Covariance      = " << cov);
  ATH_MSG_DEBUG("GlobalPosition  = " << glob.x() << " - " << glob.y() << " - " << glob.z());
  
 return new InDet::PixelClusterOnTrack(pix,locpar,cov,iH,glob,pix->gangedPixel(),isbroad);
}
