/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// 
// Implementation for ClusterMaker
//
//****************************************************************************

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/SiWidth.h"

#include "InDetIdentifier/PixelID.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "PixelConditionsServices/IPixelCalibSvc.h"

#include "EventPrimitives/EventPrimitives.h"


using CLHEP::micrometer;

namespace {
	inline double square(const double x){
		return x*x;
	}
	constexpr double ONE_TWELFTH = 1./12.;
}


namespace InDet {

// using namespace Trk;

// Constructor with parameters:
ClusterMakerTool::ClusterMakerTool(const std::string& t,
                                   const std::string& n,
                                   const IInterface* p) :
  AthAlgTool(t,n,p),
  m_issueErrorA(true),
  m_forceErrorStrategy1A(false),
  m_issueErrorB(true),
  m_forceErrorStrategy1B(false),
  m_calibSvc("PixelCalibSvc", n)
{ 
  declareInterface<ClusterMakerTool>(this);
  declareProperty("UsePixelCalibCondDB",m_calibrateCharge=true,"Compute deposited charge in Pixels");
  declareProperty("PixelCalibSvc",m_calibSvc);
}

//=============== Destructor =================================================
ClusterMakerTool::~ClusterMakerTool(){}

//================ Initialisation =============================================

StatusCode  ClusterMakerTool::initialize(){
  // Code entered here will be executed once at program start.

   ATH_MSG_INFO ( name() << " initialize()" );
   
   // Protect from the situation in which the PixelCalibSvc is not configured:
   // that should be the case if no PixelRDO are read in.
   // AA 01/10/2009
   if ( m_calibSvc.empty() ) {
     if ( m_calibrateCharge ) {
       ATH_MSG_WARNING("Requesting charge calibration, but ServiceHandle is not configured");
       ATH_MSG_WARNING("No charge calibration applied");
     }
     m_calibrateCharge = false;
   }
   
   if ( m_calibrateCharge ) {
     StatusCode sc = m_calibSvc.retrieve();
     if (sc.isFailure() || !m_calibSvc) {
       ATH_MSG_WARNING ( m_calibSvc.type() << " not found! " );
       ATH_MSG_WARNING ( "Continuing without calibrating charge" );
       m_calibrateCharge = false;
     }
     else{
      ATH_MSG_INFO ( "Retrieved tool " <<  m_calibSvc.type() ) ;
     }
   }

   if (not m_pixelLorentzAngleTool.empty()) {
     ATH_CHECK(m_pixelLorentzAngleTool.retrieve());
   } else {
     m_pixelLorentzAngleTool.disable();
   }
   if (not m_sctLorentzAngleTool.empty()) {
     ATH_CHECK(m_sctLorentzAngleTool.retrieve());
   } else {
     m_sctLorentzAngleTool.disable();
   }

   ATH_CHECK(m_clusterErrorKey.initialize());

   return StatusCode::SUCCESS;

}



// Compute the pixel cluster global position, and the error associated 
// to the position.
// Called by the pixel clustering tools
// 
// Input parameters
// - the cluster Identifier 
// - the position in local reference frame 
// - the list of identifiers of the Raw Data Objects belonging to the cluster
// - the width of the cluster
// - the module the cluster belongs to  
// - wheter the cluster contains ganged pixels
// - the error strategy, currently
//    0: cluster width/sqrt(12.)
//    1: pixel pitch/sqrt(12.)
//    2: parametrized as a function ofpseudorapidity and cluster size 
//       (default)
//   10: CTB parametrization (as a function of module and cluster size)
//       no magnetic field
// - TOT interpolation variable in local x and y directions 
//   [ Omegax = TOT1/(TOT1+TOT2), where TOT1 and TOT2 are the sum of the 
//     charges of the first and last row of the cluster respectively  
//     Omegay: similar definition with columns rather than rows ]
// OBSOLETE, kept just for backward compatibility


PixelCluster* ClusterMakerTool::pixelCluster(
                         const Identifier& clusterID,
                         const Amg::Vector2D& localPos,
                         const std::vector<Identifier>& rdoList,
                         const int lvl1a,
                         const std::vector<int>& totList,
                         const SiWidth& width,
                         const InDetDD::SiDetectorElement* element,
                         bool  ganged,
                         int errorStrategy,
                         const float omegax,
                         const float omegay,
         			           bool split,
                         double splitProb1,
                         double splitProb2) const{
  if ( errorStrategy==2 && m_issueErrorA ) {
    m_issueErrorA=false;
  }
  
  const AtlasDetectorID* aid = element->getIdHelper();
  const PixelID* pid = dynamic_cast<const PixelID*>(aid);
  if (not pid){
  	ATH_MSG_ERROR("Dynamic cast failed at "<<__LINE__<<" of ClusterMakerTool.cxx.");
  	return nullptr;
  }
  
  if ( errorStrategy==2 && m_forceErrorStrategy1A ) errorStrategy=1;
  // Fill vector of charges
  std::vector<float> chargeList;
  if (m_calibrateCharge) {
    int nRDO=rdoList.size();
    chargeList.reserve(nRDO);
    for (int i=0; i<nRDO; i++) {
      Identifier pixid=rdoList[i];
      int ToT=totList[i];

      float charge = m_calibSvc->getCharge(pixid,ToT);

      chargeList.push_back(charge);
    }
  }
// ask for Lorentz correction, get global position

  double shift = m_pixelLorentzAngleTool->getLorentzShift(element->identifyHash());
  const InDetDD::SiLocalPosition& localPosition = 
          InDetDD::SiLocalPosition(localPos[Trk::locY],
                          localPos[Trk::locX]+shift,0);
  Amg::Vector2D locpos(localPos[Trk::locX]+shift, localPos[Trk::locY]);
  // find global position of element
  Amg::Vector3D globalPos = element->globalPosition(localPosition);
  // error matrix
  const Amg::Vector2D& colRow = width.colRow();// made ref to avoid 
                                             // unnecessary copy EJWM
  Amg::MatrixX* errorMatrix = new Amg::MatrixX(2,2);
  errorMatrix->setIdentity();

  // switches are more readable **OPT**
  // actually they're slower as well (so I'm told) so perhaps
  // this should be re-written at some point EJWM
  double eta = fabs(globalPos.eta());
  double zPitch = width.z()/colRow.y();
  
  //const AtlasDetectorID* aid = element->getIdHelper();
  //const PixelID* pid = dynamic_cast<const PixelID*>(aid);
  
  
  int layer = pid->layer_disk(clusterID);
  int phimod = pid->phi_module(clusterID);
  switch (errorStrategy){
  case 0:
    errorMatrix->fillSymmetric(0,0,square(width.phiR())*ONE_TWELFTH);
    errorMatrix->fillSymmetric(1,1,square(width.z())*ONE_TWELFTH);
    break;
  case 1:
    errorMatrix->fillSymmetric(0,0,square(width.phiR()/colRow.x())*ONE_TWELFTH);
    errorMatrix->fillSymmetric(1,1,square(width.z()/colRow.y())*ONE_TWELFTH);
    break;
  case 2:                  
    // use parameterization only if the cluster does not 
    // contain long pixels or ganged pixels
    // Also require calibration service is available....
    if (!ganged && zPitch>399*micrometer && zPitch<401*micrometer) {
      SG::ReadCondHandle<PixelCalib::PixelOfflineCalibData> offlineCalibData(m_clusterErrorKey);
      if(element->isBarrel()){
        int ibin = offlineCalibData->getPixelClusterErrorData()->getBarrelBin(eta,int(colRow.y()),int(colRow.x()));
        double phiError = offlineCalibData->getPixelClusterErrorData()->getPixelBarrelPhiError(ibin);
        double etaError = offlineCalibData->getPixelClusterErrorData()->getPixelBarrelEtaError(ibin);
	      errorMatrix->fillSymmetric(0,0,square(phiError));
	      errorMatrix->fillSymmetric(1,1,square(etaError));
      }
      else {
        int ibin = offlineCalibData->getPixelClusterErrorData()->getEndcapBin(int(colRow.y()),int(colRow.x()));
        double phiError = offlineCalibData->getPixelClusterErrorData()->getPixelEndcapPhiError(ibin);
        double etaError = offlineCalibData->getPixelClusterErrorData()->getPixelEndcapRError(ibin);
	      errorMatrix->fillSymmetric(0,0,square(phiError));
	      errorMatrix->fillSymmetric(1,1,square(etaError));
			}
    }else{// cluster with ganged and/or long pixels
      errorMatrix->fillSymmetric(0,0,square(width.phiR()/colRow.x())*ONE_TWELFTH);
      errorMatrix->fillSymmetric(1,1,square(zPitch)*ONE_TWELFTH);
    }
    break;
    
  case 10:
    errorMatrix->fillSymmetric(0,0,square( getPixelCTBPhiError(layer,phimod,int(colRow.x()))));
    errorMatrix->fillSymmetric(1,1,square(width.z()/colRow.y())*ONE_TWELFTH);
    break;
    
  default:
    errorMatrix->fillSymmetric(0,0,square(width.phiR()/colRow.x())*ONE_TWELFTH);
    errorMatrix->fillSymmetric(1,1,square(width.z()/colRow.y())*ONE_TWELFTH);
    break;
  }
 PixelCluster* newCluster = 
   new PixelCluster(clusterID, locpos, 
                    rdoList, lvl1a, totList,chargeList, 
                    width, element, errorMatrix, omegax, omegay,
                    split,
                    splitProb1,
                    splitProb2);
 return newCluster;

}


// Compute the pixel cluster global position, and the error associated 
  // to the position.
  // Called by the pixel clustering tools
  // 
  // Input parameters
  // - the cluster Identifier 
  // - the position in local reference frame 
  // - the list of identifiers of the Raw Data Objects belonging to the cluster
  // - the width of the cluster
  // - the module the cluster belongs to  
  // - wheter the cluster contains ganged pixels
  // - the error strategy, currently
  //    0: cluster width/sqrt(12.)
  //    1: pixel pitch/sqrt(12.)
  //    2: parametrized as a function ofpseudorapidity and cluster size 
  //       (default)
  //   10: CTB parametrization (as a function of module and cluster size)
  //       no magnetic field
  // - const reference to a PixelID helper class  
PixelCluster* ClusterMakerTool::pixelCluster(
                         const Identifier& clusterID,
                         const Amg::Vector2D& localPos,
                         const std::vector<Identifier>& rdoList,
                         const int lvl1a,
                         const std::vector<int>& totList,
                         const SiWidth& width,
                         const InDetDD::SiDetectorElement* element,
                         bool  ganged,
                         int errorStrategy,
                         const PixelID& pixelID,
			 									 bool split,
                         double splitProb1,
                         double splitProb2) const{
	
 
  if (msgLvl(MSG::VERBOSE)) msg() << "ClusterMakerTool called, number " << endmsg;
  if ( errorStrategy==2 && m_issueErrorB ) {
    m_issueErrorB=false;
  }
  if ( errorStrategy==2 && m_forceErrorStrategy1B ) errorStrategy=1;

  // Fill vector of charges and compute charge balance
  const InDetDD::PixelModuleDesign* design = (dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design()));
  if (not design){
  	ATH_MSG_ERROR("Dynamic cast failed at "<<__LINE__<<" of ClusterMakerTool.cxx.");
  	return nullptr;
  }
  int rowMin = design->rows();
  int rowMax = 0;
  int colMin = design->columns();
  int colMax = 0;
  float qRowMin = 0;  float qRowMax = 0;
  float qColMin = 0;  float qColMax = 0;
  std::vector<float> chargeList;
  int nRDO=rdoList.size();
  if (m_calibrateCharge) chargeList.reserve(nRDO);
  for (int i=0; i<nRDO; i++) {
     Identifier pixid=rdoList[i];
     int ToT=totList[i];
     
     float charge = ToT;
     if (m_calibrateCharge){

       charge = m_calibSvc->getCharge(pixid,ToT);

       chargeList.push_back(charge);
     }
     //     std::cout << "tot, charge =  " << ToT << " " << charge << std::endl;
     int row = pixelID.phi_index(pixid);
     int col = pixelID.eta_index(pixid);
     if (row == rowMin) qRowMin += charge;
	   if (row < rowMin){ 
       rowMin = row; 
       qRowMin = charge;
	   }
	
     if (row == rowMax) qRowMax += charge;
	   if (row > rowMax){
       rowMax = row;
	     qRowMax = charge;
	   }
     if (col == colMin) qColMin += charge;
	   if (col < colMin){
       colMin = col;
       qColMin = charge;
	   }

     if (col == colMax) qColMax += charge;
	   if (col > colMax){
       colMax = col;
	     qColMax = charge;
	   }
  }
  Identifier newClusterID = pixelID.pixel_id(pixelID.wafer_id(clusterID),rowMin,colMin);
  // Compute omega for charge interpolation correction (if required)
  // Two pixels may have charge=0 (very rarely, hopefully)
  float omegax = -1;
  float omegay = -1;
  if(qRowMin+qRowMax > 0) omegax = qRowMax/float(qRowMin+qRowMax);
  if(qColMin+qColMax > 0) omegay = qColMax/float(qColMin+qColMax);   
    
  if (msgLvl(MSG::VERBOSE)) msg() << "omega =  " << omegax << " " << omegay << endmsg;

// ask for Lorentz correction, get global position
  double shift = m_pixelLorentzAngleTool->getLorentzShift(element->identifyHash());
  const InDetDD::SiLocalPosition& localPosition = 
    InDetDD::SiLocalPosition(localPos[Trk::locY],
			     localPos[Trk::locX]+shift,0);
  Amg::Vector2D locpos(localPos[Trk::locX]+shift, localPos[Trk::locY]);
// find global position of element
  Amg::Vector3D globalPos = element->globalPosition(localPosition);

  // error matrix
  const Amg::Vector2D& colRow = width.colRow();// made ref to avoid 
                                             // unnecessary copy EJWM
  Amg::MatrixX* errorMatrix = new Amg::MatrixX(2,2);
  errorMatrix->setIdentity();
	
  // switches are more readable **OPT**
  // actually they're slower as well (so I'm told) so perhaps
  // this should be re-written at some point EJWM
  double eta = fabs(globalPos.eta());
  double zPitch = width.z()/colRow.y();
  
  const AtlasDetectorID* aid = element->getIdHelper();
  const PixelID* pid = dynamic_cast<const PixelID*>(aid);
  if (not pid){
  	ATH_MSG_ERROR("Dynamic cast failed at "<<__LINE__<<" of ClusterMakerTool.cxx.");
  	delete errorMatrix;errorMatrix=nullptr;
  	return nullptr;
  }
  int layer = pid->layer_disk(clusterID);
  int phimod = pid->phi_module(clusterID);
  switch (errorStrategy){
  case 0:
    errorMatrix->fillSymmetric(0,0,square(width.phiR())*ONE_TWELFTH);
    errorMatrix->fillSymmetric(1,1,square(width.z())*ONE_TWELFTH);
    break;
  case 1:
    errorMatrix->fillSymmetric(0,0,square(width.phiR()/colRow.x())*ONE_TWELFTH);
    errorMatrix->fillSymmetric(1,1,square(width.z()/colRow.y())*ONE_TWELFTH);
    break;
  case 2:                  
    // use parameterization only if the cluster does not 
    // contain long pixels or ganged pixels
    // Also require calibration service is available....
    if (!ganged && zPitch>399*micrometer && zPitch<401*micrometer) {
	    
      SG::ReadCondHandle<PixelCalib::PixelOfflineCalibData> offlineCalibData(m_clusterErrorKey);
      if (element->isBarrel()) {
        int ibin = offlineCalibData->getPixelClusterErrorData()->getBarrelBin(eta,int(colRow.y()),int(colRow.x()));
        double phiError = offlineCalibData->getPixelClusterErrorData()->getPixelBarrelPhiError(ibin);
        double etaError = offlineCalibData->getPixelClusterErrorData()->getPixelBarrelEtaError(ibin);
				errorMatrix->fillSymmetric(0,0,pow(phiError,2));  
				errorMatrix->fillSymmetric(1,1,pow(etaError,2)); 
      }
      else {
        int ibin = offlineCalibData->getPixelClusterErrorData()->getEndcapBin(int(colRow.y()),int(colRow.x()));
        double phiError = offlineCalibData->getPixelClusterErrorData()->getPixelEndcapPhiError(ibin);
        double etaError = offlineCalibData->getPixelClusterErrorData()->getPixelEndcapRError(ibin);
				errorMatrix->fillSymmetric(0,0,square(phiError)); 
				errorMatrix->fillSymmetric(1,1,square(etaError)); 
			}
    }else{// cluster with ganged and/or long pixels
      errorMatrix->fillSymmetric(0,0,square(width.phiR()/colRow.x())*ONE_TWELFTH);
      errorMatrix->fillSymmetric(1,1,square(zPitch)*ONE_TWELFTH);
    }
    break;
    
  case 10:
    errorMatrix->fillSymmetric(0,0,square( getPixelCTBPhiError(layer,phimod,int(colRow.x()))));
    errorMatrix->fillSymmetric(1,1,square(width.z()/colRow.y())*ONE_TWELFTH);
    break;
    
  default:
    errorMatrix->fillSymmetric(0,0,square(width.phiR()/colRow.x())*ONE_TWELFTH);
    errorMatrix->fillSymmetric(1,1,square(width.z()/colRow.y())*ONE_TWELFTH);
    break;
  }
 PixelCluster* newCluster = 
   new PixelCluster(newClusterID, 
                    locpos,
                    rdoList,
                    lvl1a,
                    totList,
                    chargeList,
                    width,
                    element,
                    errorMatrix,
                    omegax,
                    omegay,
                    split,
                    splitProb1,
                    splitProb2);

 return newCluster;

}


// Computes global position and errors for SCT cluster.
  // Called by SCT Clustering tools
  // 
  // Input parameters
  // - the cluster Identifier 
  // - the position in local reference frame 
  // - the list of identifiers of the Raw Data Objects belonging to the cluster
  // - the width of the cluster
  // - the module the cluster belongs to  
  // - the error strategy, currently
  //    0: Cluster Width/sqrt(12.)
  //    1: Set to a different values for one and two-strip clusters (def.)

SCT_Cluster* ClusterMakerTool::sctCluster(
                         const Identifier& clusterID,
			 const Amg::Vector2D& localPos,
                         const std::vector<Identifier>& rdoList,
                         const SiWidth& width,
                         const InDetDD::SiDetectorElement* element,
                         int errorStrategy) const{

        double shift = m_sctLorentzAngleTool->getLorentzShift(element->identifyHash());
//        const InDetDD::SiLocalPosition& localPosition = 
//                        InDetDD::SiLocalPosition(localPos[Trk::locY),
//                                        localPos[Trk::locX)+shift,0);
        Amg::Vector2D locpos(localPos[Trk::locX]+shift, localPos[Trk::locY]);

	// error matrix
	const Amg::Vector2D& colRow = width.colRow();// made ref to avoid 
	// unnecessary copy EJWM

	Amg::MatrixX* errorMatrix = new Amg::MatrixX(2,2);
	errorMatrix->setIdentity();

	// switches are more readable **OPT**
	// actually they're slower as well (so I'm told) so perhaps
	// this should be re-written at some point EJWM
    
	switch (errorStrategy){
	case 0:
	  errorMatrix->fillSymmetric(0,0,square(width.phiR())*ONE_TWELFTH);
	  errorMatrix->fillSymmetric(1,1,square(width.z())*ONE_TWELFTH);
	  break;
	case 1:
	  // mat(1,1) = pow(width.phiR()/colRow.x(),2)/12;
	  // single strip - resolution close to pitch/sqrt(12)
	  // two-strip hits: better resolution, approx. 40% lower
	  if(colRow.x() == 1){
	    errorMatrix->fillSymmetric(0,0,square(1.05*width.phiR())*ONE_TWELFTH);
	  }
	  else if(colRow.x() == 2){
	    errorMatrix->fillSymmetric(0,0,square(0.27*width.phiR())*ONE_TWELFTH);
	  }
	  else{
	    errorMatrix->fillSymmetric(0,0,square(width.phiR())*ONE_TWELFTH);
	  }
	  errorMatrix->fillSymmetric(1,1,square(width.z()/colRow.y())*ONE_TWELFTH);
	  break;
	default:
	  // single strip - resolution close to pitch/sqrt(12)
	  // two-strip hits: better resolution, approx. 40% lower
	  if(colRow.x() == 1){
	    errorMatrix->fillSymmetric(0,0,square(width.phiR())*ONE_TWELFTH);
	  }
	  else if(colRow.x() == 2){
	    errorMatrix->fillSymmetric(0,0,square(0.27*width.phiR())*ONE_TWELFTH);
	  }
	  else{
	    errorMatrix->fillSymmetric(0,0,square(width.phiR())*ONE_TWELFTH);
	  }
	  errorMatrix->fillSymmetric(1,1,square(width.z()/colRow.y())*ONE_TWELFTH);
	  break;
	}

	// rotation for endcap SCT
	if(element->design().shape() == InDetDD::Trapezoid || element->design().shape() == InDetDD::Annulus) {
          double sn      = element->sinStereoLocal(localPos); 
          double sn2     = sn*sn;
          double cs2     = 1.-sn2;
          double w       = element->phiPitch(localPos)/element->phiPitch(); 
          double v0      = (*errorMatrix)(0,0)*w*w;
          double v1      = (*errorMatrix)(1,1);
	  errorMatrix->fillSymmetric(0,0,cs2*v0+sn2*v1);
	  errorMatrix->fillSymmetric(0,1,sn*sqrt(cs2)*(v0-v1));
	  errorMatrix->fillSymmetric(1,1,sn2*v0+cs2*v1);
	}

	//        delete localPos;
	//	localPos=0;
	SCT_Cluster* newCluster = new SCT_Cluster(clusterID, locpos, rdoList , width, element, errorMatrix);
	return newCluster;

}




//---------------------------------------------------------------------------
// CTB parameterization, B field off
double ClusterMakerTool::getPixelCTBPhiError(int layer, int phi, 
			                     int phiClusterSize) const{

 double sigmaL0Phi1[3] = { 8.2*micrometer,  9.7*micrometer, 14.6*micrometer};
 double sigmaL1Phi1[3] = {14.6*micrometer,  9.3*micrometer, 14.6*micrometer};
 double sigmaL2Phi1[3] = {14.6*micrometer,  8.6*micrometer, 14.6*micrometer};
 double sigmaL0Phi0[3] = {14.6*micrometer, 13.4*micrometer, 13.0*micrometer};
 double sigmaL1Phi0[3] = {14.6*micrometer,  8.5*micrometer, 11.0*micrometer};
 double sigmaL2Phi0[3] = {14.6*micrometer, 11.6*micrometer,  9.3*micrometer};

 if(phiClusterSize > 3) return 14.6*micrometer;

 if(layer == 0 && phi == 0) return sigmaL0Phi0[phiClusterSize-1];
 if(layer == 1 && phi == 0) return sigmaL1Phi0[phiClusterSize-1];
 if(layer == 2 && phi == 0) return sigmaL2Phi0[phiClusterSize-1];
 if(layer == 0 && phi == 1) return sigmaL0Phi1[phiClusterSize-1];
 if(layer == 1 && phi == 1) return sigmaL1Phi1[phiClusterSize-1];
 if(layer == 2 && phi == 1) return sigmaL2Phi1[phiClusterSize-1];

 // shouldn't really happen...
  if(msgLvl(MSG::WARNING)) msg() << "Unexpected layer and phi numbers: layer = "
	   << layer << " and phi = " << phi << endmsg;
 return 14.6*micrometer;

}

}


