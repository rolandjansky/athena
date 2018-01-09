/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelToTPIDTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelToTPIDTool/PixelToTPIDTool.h"

// StoreGate, Athena, and Database stuff:
#include "Identifier/Identifier.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

// Tracking:
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/TrackInfo.h"

// Pixels:
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelGeoModel/IBLParameterSvc.h" 
#include "PixelConditionsServices/IPixelOfflineCalibSvc.h"

// CLHEP:
#include "CLHEP/Matrix/Vector.h"

// Particle masses

// Math functions:
#include <cmath>

#include "PathResolver/PathResolver.h"

//================ Constructor =================================================

InDet::PixelToTPIDTool::PixelToTPIDTool(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_IBLParameterSvc("IBLParameterSvc",n),
  m_overflowIBLToT(0),
  m_offlineCalibSvc("PixelOfflineCalibSvc", n),
  m_pixelid(nullptr)
{
  declareInterface<IPixelToTPIDTool>(this);
  declareProperty("CalibrationFile", m_filename = "mcpar_signed_234.txt");
  declareProperty("ReadFromCOOL", m_readfromcool = false);
  declareProperty("MinimumdEdxForMass", m_mindedxformass = 1.8);


  float energyPair = 3.68e-6; // Energy in MeV to create an electron-hole pair in silicon
  float sidensity = 2.329; // silicon density in g cm^-3
 
  //conversion Factor
  //{.025,.023,.020}; //{Old Planars,IBL_3Ds,IBL_Planars} the sensors thickness will be take into account in dEdx calculation
  
  m_conversionfactor=energyPair/sidensity;
  
  m_mydedx=0;  
}

//================ Destructor =================================================

InDet::PixelToTPIDTool::~PixelToTPIDTool()
{
  
}


//================ Initialisation =================================================

StatusCode InDet::PixelToTPIDTool::initialize()
{
  if (m_mydedx) {
    delete m_mydedx;
    m_mydedx=0;
  }
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;

  if (detStore()->retrieve(m_pixelid, "PixelID").isFailure()) { 
    ATH_MSG_FATAL ("Could not get Pixel ID helper"); 
    return StatusCode::FAILURE; 
  }

  // Register callback function for cache updates:
  //const DataHandle<CondAttrListCollection> aptr;
  if (m_readfromcool){
    const DataHandle<AthenaAttributeList> aptr;
    if (StatusCode::SUCCESS == detStore()->regFcn(&InDet::PixelToTPIDTool::update,this, aptr, "/PIXEL/PixdEdx" )) {
      ATH_MSG_DEBUG ("Registered callback for PixelToTPIDTool.");
    } else {
      ATH_MSG_ERROR ("Callback registration failed for PixelToTPIDTool! ");
    }   
    m_mydedx=new dEdxID;
  }
  else {
    std::string file_name = PathResolver::find_file (m_filename, "DATAPATH");
    if (file_name.size()==0) {
      msg(MSG::FATAL) << "Could not find dEdx calibration file" << m_filename << endmsg;
      return StatusCode::FAILURE;
    }
    m_mydedx=new dEdxID(file_name.c_str());
  }
  
  if ( !m_offlineCalibSvc.empty() ) {
    StatusCode sc = m_offlineCalibSvc.retrieve();
    if (sc.isFailure() || !m_offlineCalibSvc ) {
      ATH_MSG_ERROR( m_offlineCalibSvc.type() << " not found! ");
      return StatusCode::RECOVERABLE;
    }
    else{
      ATH_MSG_INFO ( "Retrieved tool " <<  m_offlineCalibSvc.type() );
    }
  }

  if (m_IBLParameterSvc.retrieve().isFailure()) { 
      ATH_MSG_FATAL("Could not retrieve IBLParameterSvc"); 
      return StatusCode::FAILURE; 
  } else  
      ATH_MSG_INFO("Retrieved service " << m_IBLParameterSvc); 
 

  //m_overflowIBLToT = m_offlineCalibSvc->getIBLToToverflow();

  ATH_MSG_INFO ("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::PixelToTPIDTool::finalize()
{
  delete m_mydedx;
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}


//============================================================================================

float InDet::PixelToTPIDTool::dEdx(const Trk::Track& track,
                                   int& nUsedHits,
                                   int& nUsedIBLOverflowHits) const
{
 

  // ------------------------------------------------------------------------------------
  // Loop over pixel hits on track, and calculate dE/dx:
  // ------------------------------------------------------------------------------------

  unsigned int pixelhits = 0;
  nUsedHits=0;
  nUsedIBLOverflowHits=0;
  float Pixel_sensorthickness=.025; //250 microns Pixel Planars
  float IBL_3D_sensorthickness=.023; //230 microns IBL 3D 
  float IBL_PLANAR_sensorthickness=.020;// 200 microns IBL Planars
  float dEdxValue=0;
  
  
  //std::multimap<float,int> chargesMap; //second value keeps track if the cluster is in IBL and has at least an overflow hit
  std::multimap<float,int> dEdxMap;
  
  
  //if (track.perigeeParameters()) std::cout << "pT: " << track.perigeeParameters()->pT() << std::endl;
  // Check for track states:
  const DataVector<const Trk::TrackStateOnSurface>* recoTrackStates = track.trackStateOnSurfaces();
  if (recoTrackStates) {
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIter    = recoTrackStates->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIterEnd = recoTrackStates->end();

    // Loop over track states on surfaces (i.e. generalized hits):
    for ( ; tsosIter != tsosIterEnd; ++tsosIter) {
	
      const Trk::MeasurementBase *measurement = (*tsosIter)->measurementOnTrack();
      if (measurement && !(*tsosIter)->type(Trk::TrackStateOnSurface::Outlier)) {
        if (!(*tsosIter)->trackParameters()) {
          msg(MSG::WARNING) << "No track parameters available for a state of type measurement, returning -1" << endmsg;
          msg(MSG::WARNING) << "Don't run this tool on slimmed tracks!" << endmsg;
          return -1;
        }

	const InDet::PixelClusterOnTrack *pixclus = dynamic_cast<const InDet::PixelClusterOnTrack*>(measurement);
	if (pixclus) {
          
          //bool isok=false;
          double locx=pixclus->localParameters()[Trk::locX];
	  double locy=pixclus->localParameters()[Trk::locY];
	  int bec=m_pixelid->barrel_ec(pixclus->identify());
	  int layer=m_pixelid->layer_disk(pixclus->identify());
	  int eta_module=m_pixelid->eta_module(pixclus->identify());//check eta module to select thickness
	  
	  /*
	  if ( ( bec==0 && fabs(locy)<30. &&  (( locx > -8.20 && locx < -0.60 ) || ( locx > 0.50 && locx < 8.10 ) ) ) ||
               ( std::abs(bec) == 2 && fabs(locy)<30. && ( ( locx > -8.15 && locx < -0.55 ) || ( locx > 0.55 && locx < 8.15 ) ) ) ) isok=true;
          
          if (!isok) continue;
          
	  */
	  
	  /* const Trk::CovarianceMatrix &measerr=pixclus->localErrorMatrix().covariance();
          const Trk::MeasuredTrackParameters *measpar=dynamic_cast<const Trk::MeasuredTrackParameters *>((*tsosIter)->trackParameters());
          if (!measpar) {
            msg(MSG::DEBUG) << "No measured track parameters available, returning -1" << endmsg;
            return -1;
          }
          const Trk::CovarianceMatrix &trackerr=measpar->localErrorMatrix().covariance();
          double resphi=pixclus->localParameters()[Trk::locX]-(*tsosIter)->trackParameters()->parameters()[Trk::locX];
          double reseta=pixclus->localParameters()[Trk::locY]-(*tsosIter)->trackParameters()->parameters()[Trk::locY];
          double pullphi = (measerr[0][0]>trackerr[0][0]) ? fabs(resphi/sqrt(measerr[0][0]-trackerr[0][0])) : 0;
          double pulleta = (measerr[1][1]>trackerr[1][1]) ? fabs(reseta/sqrt(measerr[1][1]-trackerr[1][1])) : 0;
          if (pullphi>5 || pulleta>5) continue; */
	  
	  float dotProd = (*tsosIter)->trackParameters()->momentum().dot((*tsosIter)->trackParameters()->associatedSurface().normal());
          float cosalpha=fabs(dotProd/(*tsosIter)->trackParameters()->momentum().mag());
          
	  if (std::abs(cosalpha)<.16) continue;
	  
	  
	  float charge=pixclus->prepRawData()->totalCharge()*cosalpha;
	  
	  
	  //keep track if this is an ibl cluster with overflow
	  int iblOverflow=0;
	  
	  
	  if ( (m_IBLParameterSvc->containsIBL()) and (bec==0) and (layer==0) ){ // check if IBL 
	  
	  //loop over ToT and check if anyone is overflow (ToT==14) check for IBL cluster overflow
	  
	  m_overflowIBLToT = m_offlineCalibSvc->getIBLToToverflow();
	  const std::vector<int>& ToTs = pixclus->prepRawData()->totList();
	  
	    for (int pixToT : ToTs) {
	      if (pixToT >= m_overflowIBLToT) {
		//overflow pixel hit -- flag cluster
		iblOverflow = 1;
		break; //no need to check other hits of this cluster
	      }
	    }// end
	  
	    //this is IBL layer -- @todo: check using proper service (safe against geometries)
	    if(((eta_module>=-10 && eta_module<=-7)||(eta_module>=6 && eta_module<=9)) && (fabs(locy)<10. && (locx>-8.33 && locx <8.3)) ){//check if IBL 3D and good cluster selection
	    	
			dEdxValue=charge*m_conversionfactor/IBL_3D_sensorthickness;
	    		dEdxMap.insert(std::pair<float,int>(dEdxValue, iblOverflow));
			pixelhits++;
			if(iblOverflow==1)nUsedIBLOverflowHits++;
	    
	    }else if((eta_module>=-6 && eta_module<=5) && (fabs(locy)<20. &&( locx >-8.33 && locx <8.3 )) ){//check if IBL planar and good cluster selection
	 
	 		dEdxValue=charge*m_conversionfactor/IBL_PLANAR_sensorthickness; 
	    		dEdxMap.insert(std::pair<float,int>(dEdxValue, iblOverflow));
			pixelhits++;
			if(iblOverflow==1)nUsedIBLOverflowHits++;
	    }else{
	    	dEdxValue=-1;
	    }//end check which IBL Module
	    
	    //PIXEL layer and ENDCAP
	  //}else if(layer !=0 && bec==0 && fabs(locy)<30. &&  (( locx > -8.20 && locx < -0.60 ) || ( locx > 0.50 && locx < 8.10 ) ) ){
	  }else if(bec==0 && fabs(locy)<30. &&  (( locx > -8.20 && locx < -0.60 ) || ( locx > 0.50 && locx < 8.10 ) ) ){
	  
	  	dEdxValue=charge*m_conversionfactor/Pixel_sensorthickness;
		dEdxMap.insert(std::pair<float,int>(dEdxValue, iblOverflow));
		pixelhits++;
		
	  }else if (std::abs(bec) == 2 && fabs(locy)<30. && ( ( locx > -8.15 && locx < -0.55 ) || ( locx > 0.55 && locx < 8.15 ) ) ){
	  
	  	dEdxValue=charge*m_conversionfactor/Pixel_sensorthickness;
		dEdxMap.insert(std::pair<float,int>(dEdxValue, iblOverflow));
		pixelhits++;
	  
	  }
	}//pixclus iterator
      }
    }
  }

  //Now calculate dEdx, multimap is already sorted in ascending order
  //float averageCharge=-1;
  
  float averagedEdx=0.;
  nUsedHits=0;
  int IBLOverflow=0;
  
  for (std::pair<float,int> itdEdx : dEdxMap) {
  
    //averageCharge += itCharge.first;
    
    if(itdEdx.second==0){
    	averagedEdx += itdEdx.first;
    	nUsedHits++;
    }
    
    if(itdEdx.second > 0){ 
    	IBLOverflow++;
    }
    
    
    
    //break, skipping last or the two last elements depending on total measurements
    if (((int)pixelhits >= 5) and ((int)nUsedHits >= (int)pixelhits-2)) break;
    
    //break, IBL Overflow case pixelhits==3 and 4 
    if((int)IBLOverflow>0 and ((int)pixelhits==3) and (int)nUsedHits==1) break;
    if((int)IBLOverflow>0 and ((int)pixelhits==4) and (int)nUsedHits==2) break;
    
    if (((int)pixelhits > 1) and ((int)nUsedHits >=(int)pixelhits-1)) break; 
    
    if((int)IBLOverflow>0 and (int)pixelhits==1){ //only IBL in overflow
    	averagedEdx=itdEdx.first;
    	break;
    }   
  
  }
  
  if (nUsedHits > 0 or (nUsedHits==0 and(int)IBLOverflow>0 and (int)pixelhits==1)) {
  	
	if(nUsedHits > 0) averagedEdx = averagedEdx / nUsedHits;
  	//if(nUsedHits == 0 and (int)IBLOverflow > 0 and (int)pixelhits == 1) averagedEdx = averagedEdx; 
    
    	ATH_MSG_DEBUG("NEW dEdx = " << averagedEdx);
    	ATH_MSG_DEBUG("Used hits: " << nUsedHits << ", IBL overflows: " << IBLOverflow );
    	ATH_MSG_DEBUG("Original number of measurements = " << pixelhits << "( map size = " << dEdxMap.size() << ") " );
    
    	return averagedEdx;  
  }

  return -1;
  
}

/* ----------------------------------------------------------------------------------- */
// Callback function to update constants from database:
/* ----------------------------------------------------------------------------------- */

StatusCode InDet::PixelToTPIDTool::update( IOVSVC_CALLBACK_ARGS_P(I,keys) )  {

  ATH_MSG_INFO ("Updating constants for the PixelToTPIDTool! ");

  // Callback function to update HT onset parameter cache when condDB data changes:
  for(std::list<std::string>::const_iterator key=keys.begin(); key != keys.end(); ++key)
    ATH_MSG_DEBUG("IOVCALLBACK for key " << *key << " number " << I);

  const char * storeName = "/PIXEL/PixdEdx";


  const AthenaAttributeList* atrlist;

  if (StatusCode::SUCCESS == detStore()->retrieve(atrlist, storeName ) && atrlist != 0) {
    std::string mystring=(*atrlist)["data"].data<std::string>();
    //std::cout << "mystring: " << mystring << std::endl; 
    std::istringstream mystringstream(mystring);

    int         fit_type;
    std::string fun_type;
    std::string bb_type;
    mystringstream >> fit_type;
    mystringstream >> fun_type;
    mystringstream >> bb_type;
    //std::cout << "types: " << fit_type << " " << fun_type << " " << bb_type << std::endl;  
    m_mydedx->setFun(fun_type,bb_type);
   
    if (fit_type==0){
      for (int i=0;i<NCLASS;i++){
        double *param = new double[NPAR];
        for (int j=0;j<NPAR;j++){
          mystringstream >> param[j];
          //std::cout << "m_par[" << i << "][" << j << "]: " << m_par[i][j] << std::endl;
        }
        m_mydedx->setPar(i,param);
        delete[] param;
      }
      m_mydedx->setPosNeg(false);
    } else {
      for (int i=0;i<2*NCLASS;i++){
        double *param = new double[NPAR];
        for (int j=0;j<NPAR;j++){
          mystringstream >> param[j];
          //std::cout << "m_par[" << i << "][" << j << "]: " << m_par[i][j] << std::endl;
        }
        m_mydedx->setPar(i,param);
        delete[] param;
      }
      m_mydedx->setPosNeg(true);
    }
       
  } else {
    // m_log << MSG::ERROR << "Problem reading condDB object." << endmsg;
    ATH_MSG_ERROR ("Problem reading condDB object. -");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


std::vector<float> InDet::PixelToTPIDTool::getLikelihoods(double dedx2, double p2, int nGoodPixels) const {
  double dedx=dedx2; 
  double p=p2/1000; 
  std::vector<float> vhypo; 
  vhypo.resize(3); 
  //double par[9];
  vhypo[0] = vhypo[1] = vhypo[2] = -1;
  if (nGoodPixels<=0) return vhypo;
  double vhypo2[3]={-1,-1,-1};
  m_mydedx->getP(dedx,p,nGoodPixels,vhypo2);
  vhypo[0]=vhypo2[0];
  vhypo[1]=vhypo2[1];
  vhypo[2]=vhypo2[2];

  return vhypo;

}

//to change
float InDet::PixelToTPIDTool::getMass(double dedx, double p, int nGoodPixels) const {
  if (dedx<m_mindedxformass) return 0.13957;
  return m_mydedx->getMass(dedx,p/1000,nGoodPixels); 
  //return m_mydedx->getMass(dedx/m_conversionfactor,p/1000,nGoodPixels);
}


//============================================================================================



  /* ----------------------------------------------------------------------------------- */

