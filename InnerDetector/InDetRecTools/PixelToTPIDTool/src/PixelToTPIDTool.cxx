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
  AthAlgTool(t,n,p)
{
  declareInterface<IPixelToTPIDTool>(this);
  declareProperty("CalibrationFile", m_filename = "mcpar_signed_234.txt");
  declareProperty("ReadFromCOOL", m_readfromcool = false);
  declareProperty("MinimumdEdxForMass", m_mindedxformass = 1.8);


  m_nusedhits=-1;
  m_nUsedIBLOverflowHits=0;
  float energyPair = 3.68e-6; // Energy in MeV to create an electron-hole pair in silicon
  float sidensity = 2.329; // silicon density in g cm^-3
  float sensorthickness = .025; // 250 microns
  m_conversionfactor=energyPair/(sidensity*sensorthickness);
  m_mydedx=0;  
  m_slimwarn=0;  
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
      msg(MSG::FATAL) << "Could not find dEdx calibration file" << m_filename << endreq;
      return StatusCode::FAILURE;
    }
    m_mydedx=new dEdxID(file_name.c_str());
  }
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

int InDet::PixelToTPIDTool::numberOfUsedHitsdEdx()
{
  return m_nusedhits;
}

int InDet::PixelToTPIDTool::numberOfUsedIBLOverflowHits()
{
  return m_nUsedIBLOverflowHits;
}

float InDet::PixelToTPIDTool::dEdx(const Trk::Track& track)
{
 

  // ------------------------------------------------------------------------------------
  // Loop over pixel hits on track, and calculate dE/dx:
  // ------------------------------------------------------------------------------------

  unsigned int pixelhits       = 0;
  m_nusedhits=0;
  std::multimap<float,int> chargesMap; //second value keeps track if the cluster is in IBL and has at least an overflow hit
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
          if (m_slimwarn<10){
            msg(MSG::WARNING) << "No track parameters available for a state of type measurement, returning -1" << endreq;
            msg(MSG::WARNING) << "Don't run this tool on slimmed tracks!" << endreq;
            m_slimwarn++;
            if (m_slimwarn==10) msg(MSG::WARNING) << "(last message!)" << endreq;
          }
          return -1;
        }

	const InDet::PixelClusterOnTrack *pixclus = dynamic_cast<const InDet::PixelClusterOnTrack*>(measurement);
	if (pixclus) {
          
          bool isok=false;
          double locx=pixclus->localParameters()[Trk::locX];
	  double locy=pixclus->localParameters()[Trk::locY];
	  int bec=m_pixelid->barrel_ec(pixclus->identify());
	  int layer=m_pixelid->layer_disk(pixclus->identify());
	  if ( ( bec==0 && fabs(locy)<30. &&  (( locx > -8.20 && locx < -0.60 ) || ( locx > 0.50 && locx < 8.10 ) ) ) ||
               ( std::abs(bec) == 2 && fabs(locy)<30. && ( ( locx > -8.15 && locx < -0.55 ) || ( locx > 0.55 && locx < 8.15 ) ) ) ) isok=true;
          
          if (!isok) continue;
          /* const Trk::CovarianceMatrix &measerr=pixclus->localErrorMatrix().covariance();
          const Trk::MeasuredTrackParameters *measpar=dynamic_cast<const Trk::MeasuredTrackParameters *>((*tsosIter)->trackParameters());
          if (!measpar) {
            msg(MSG::DEBUG) << "No measured track parameters available, returning -1" << endreq;
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
          //std::cout << "pixel charge: " << charge << std::endl;
	  // ------------------------------------------------------------------------------------
	  // Get the necessary input for the probability calculations:
	  // ------------------------------------------------------------------------------------
	  pixelhits++;
	  //keep track if this is an ibl cluster with overflow
	  int iblOverflow=0;
	  if ((bec==0) and (layer==0)) {
	    //this is IBL layer -- @todo: check using proper service (safe against geometries)
	    //loop over ToT and check if anyone is overflow (ToT==14)
	    const std::vector<int>& ToTs = pixclus->prepRawData()->totList();
	    for (int pixToT : ToTs) {
	      if (pixToT == 14) {
		//overflow pixel hit -- flag cluster
		iblOverflow = 1;
		break; //no need to check other hits of this cluster
	      }
	    }
	  } // end check for IBL cluster overflow
	  chargesMap.insert(std::pair<float,int>(charge, iblOverflow)); //add hit, see if IBL overflow below
	}
      }
    }
  }

  //Now calculate dEdx, multimap is already sorted in ascending order
  float averageCharge=-1;
  m_nusedhits=0;
  m_nUsedIBLOverflowHits=0;
  
  for (std::pair<float,int> itCharge : chargesMap) {
    averageCharge += itCharge.first;
    m_nusedhits++;
    if (itCharge.second > 0) m_nUsedIBLOverflowHits++;
    //break, skipping last or the two last elements depending on total measurements
    if ((pixelhits >= 5) and (m_nusedhits >= pixelhits-2)) break;
    if ((pixelhits > 1) and (m_nusedhits >= pixelhits-1)) break;    
  }
  if (m_nusedhits > 0) {
    averageCharge = averageCharge / m_nusedhits;
    ATH_MSG_DEBUG("NEW dEdx = " << averageCharge << " -> " << averageCharge*m_conversionfactor);
    ATH_MSG_DEBUG(" Used hits: " << m_nusedhits << ", IBL overflows: " << m_nUsedIBLOverflowHits );
    ATH_MSG_DEBUG(" Original number of measurements = " << pixelhits << "( map size = " << chargesMap.size() << ") " );
    return averageCharge*m_conversionfactor;
  }

  return -1;
}

/* ----------------------------------------------------------------------------------- */
// Callback function to update constants from database:
/* ----------------------------------------------------------------------------------- */

StatusCode InDet::PixelToTPIDTool::update( IOVSVC_CALLBACK_ARGS_P(I,keys) ) {

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
    // m_log << MSG::ERROR << "Problem reading condDB object." << endreq;
    ATH_MSG_ERROR ("Problem reading condDB object. -");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


std::vector<float> InDet::PixelToTPIDTool::getLikelihoods(double dedx2, double p2, int nGoodPixels){
  double dedx=dedx2/m_conversionfactor; 
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

float InDet::PixelToTPIDTool::getMass(double dedx, double p, int nGoodPixels){
  if (dedx<m_mindedxformass) return 0.13957;
  return m_mydedx->getMass(dedx/m_conversionfactor,p/1000,nGoodPixels);
}


//============================================================================================



  /* ----------------------------------------------------------------------------------- */

