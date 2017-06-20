/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



/// @file   AFPSiDBasicKalmanTool.cxx
/// @author Grzegorz Gach <gach@agh.edu.pl>
/// @date   2017-03-01
/// 
/// @brief  Implementation file for AFPSiDBasicKalmanTool used in tracks reconstruction.



// AFP_LocReco includes
#include "AFP_LocReco/AFPSiDBasicKalmanTool.h"
#include "AFP_Geometry/AFP_Geometry.h"
#include "CLHEP/Geometry/Point3D.h"

#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPTrack.h"
#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrackAuxContainer.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// STL includes
#include<list>


// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

using namespace std;

AFPSiDBasicKalmanTool::AFPSiDBasicKalmanTool( const std::string& type, 
					      const std::string& name, 
					      const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent   )
{
  declareInterface< IAFPSiDLocRecoTool >(this);

  declareProperty( "hitsContainerName", m_hitsContainerName = "AFPSiHitContainer", "Name of the container with hits from which tracks are to be reconstructed" );
  declareProperty( "tracksContainerName", m_tracksContainerName = "AFPTrackContainer", "Name of the container in which tracks are saved");

  m_listResults.clear();

  m_AmpThresh = 0;
  m_iDataType = 0;
	
  Fk.clear();
  Ck.clear();
  CkP.clear();
  Rk.clear();

  xk.clear();
  xkP.clear();
  rk.clear();
  rkP.clear();
  mk.clear();
  chik.clear();
	
  zk.clear();
  HID.clear();
	
  xkS.clear();
  CkS.clear();
  chikS.clear();
	
  z0 = 0;

  
}

// Destructor
///////////////
AFPSiDBasicKalmanTool::~AFPSiDBasicKalmanTool()
{
  m_listResults.clear();
}


bool AFPSiDBasicKalmanTool::ReadGeometryDetCS()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::ReadGeometryDetCS()");

  StatusCode sc;
	
  for(Int_t nStationID = 0; nStationID < SIDSTATIONID; nStationID++)
    {				
      for (Int_t nPlateID = 0; nPlateID < SIDCNT; nPlateID++)
	{
			
	  HepGeom::Point3D<double> LocPoint = HepGeom::Point3D<double>(-SID_SENSORXDIM+SID_DEATH_EDGE, -SID_SENSORYDIM+SID_LOWER_EDGE, 0.*CLHEP::mm); //changed! (death edge info from AFP_Geometry)
	  HepGeom::Point3D<double> GloPoint = HepGeom::Point3D<double>();
	  sc = m_pGeometry->GetPointInSIDSensorGlobalCS(nStationID, nPlateID, LocPoint, GloPoint);
			
	  if (sc.isFailure())
	    {
	      ATH_MSG_WARNING("AFP_Geometry::GetPointInSIDSensorGlobalCS() Failed");

	      m_fsSID[nStationID][nPlateID] = SID_NOMINALSLOPE;
	      m_fxSID[nStationID][nPlateID] = LocPoint.x();
	      m_fySID[nStationID][nPlateID] = LocPoint.y();
	      m_fzSID[nStationID][nPlateID] = SID_NOMINALSPACING*nPlateID;
	    }
			
	  else
	    {
	      m_fsSID[nStationID][nPlateID] = SID_NOMINALSLOPE;
	      m_fxSID[nStationID][nPlateID] = GloPoint.x(); 
	      m_fySID[nStationID][nPlateID] = GloPoint.y(); 
	      m_fzSID[nStationID][nPlateID] = GloPoint.z();	
					
	      ATH_MSG_DEBUG("Global edge position of SID sensor: " <<GloPoint.x()<< "\t" <<GloPoint.y()<< "\t" <<GloPoint.z()<< "\t");	
	    }
			
	}
    }	

  ATH_MSG_DEBUG("end AFP_SIDLocReco::ReadGeometryDetCS()");

  return 1;
}

StatusCode AFPSiDBasicKalmanTool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  AFP_CONFIGURATION config;
  config.clear();
  m_pGeometry = new AFP_Geometry(&config);


  m_AmpThresh = 1000;
  m_iDataType = 0;
	
  Float_t delta_pixel_x = 0.050*CLHEP::mm; // size of pixel along x, in mm
  Float_t delta_pixel_y = 0.250*CLHEP::mm; // size of pixel along y, in mm

  Float_t interPlaneX[4][4] ={0,0,0,0,
			      0,0,0,0,
			      0,0.,0.,0,
			      0,0.,0,0};
  Float_t interPlaneY[4][4] ={0,0,0,0,
			      0,0,0,0,
			      0, 0, 0, 0,
			      0, 0, 0, 0};
  Float_t Alpha[4][4] ={0.2443461,0.2443461,0.2443461,0.2443461, 
			0.2443461,0.2443461,0.2443461,0.2443461, 
			0.2443461,0.2443461,0.2443461,0.2443461, 
			0.2443461,0.2443461,0.2443461,0.2443461};

  //read geometry
  if(ReadGeometryDetCS())
    {
      ATH_MSG_DEBUG("Geometry loaded successfully");
    }
  else
    {
      ATH_MSG_WARNING("Could not load geometry");
      return StatusCode::SUCCESS;
    }
	
  // x-y-z map of all pixels
  for(Int_t nStationID = 0; nStationID < SIDSTATIONID; nStationID++)
    {				
      for (Int_t nPlateID = 0; nPlateID < SIDCNT; nPlateID++)
	{

	  if ( m_iDataType == 1) m_fsSID[nStationID][nPlateID] = Alpha[nStationID][nPlateID];

	  for (Int_t nPixel_x = 0; nPixel_x < 336; nPixel_x++)
	    {	
	      for (Int_t nPixel_y = 0; nPixel_y < 80; nPixel_y++)
		{	                                 		
		  fxMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] = m_fxSID[nStationID][nPlateID]+(delta_pixel_x*(nPixel_x-168))*cos(m_fsSID[nStationID][nPlateID]);		//sign changed!	
		  fyMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] = m_fySID[nStationID][nPlateID]+(delta_pixel_y*nPixel_y); //sign changed!
		  fzMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] = m_fzSID[nStationID][nPlateID] - ((nStationID<2)?1.:-1.)*(delta_pixel_x*(nPixel_x-168))*sin(m_fsSID[nStationID][nPlateID]); //sign changed!
		  if( m_iDataType == 1) {
		    fxMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] += interPlaneX[nStationID][nPlateID] + 168*delta_pixel_x*cos(m_fsSID[nStationID][0]);
		    fyMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] += interPlaneY[nStationID][nPlateID];
		    fzMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] += -168*((nStationID<2)?1.:-1.)*delta_pixel_x*sin(m_fsSID[nStationID][0]); }
		  else {
		    fxMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] += 168*delta_pixel_x*cos(m_fsSID[nStationID][0]);
		    fzMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] += -168*((nStationID<2)?1.:-1.)*delta_pixel_x*sin(m_fsSID[nStationID][0]); } 
		}
	    }
	}
    }
	
  // Kalman matrices initialization
  Hk = CLHEP::HepMatrix(2, 4, 0);
  Hk[0][0] = 1.;
  Hk[1][2] = 1.;
	
  Qk = CLHEP::HepMatrix(4, 4, 0);	
	
  Vk = CLHEP::HepMatrix(2, 2, 0);
  Vk[0][0] = delta_pixel_x*delta_pixel_x;
  Vk[1][1] = delta_pixel_y*delta_pixel_y;
	
  C0 = CLHEP::HepMatrix(4, 4, 0);
  C0[0][0] = delta_pixel_x*delta_pixel_x/3.;
  C0[1][1] = delta_pixel_x*delta_pixel_x/SID_NOMINALSPACING/SID_NOMINALSPACING/3.;
	
  C0[2][2] = delta_pixel_y*delta_pixel_y/3.;
  C0[3][3] = delta_pixel_y*delta_pixel_y/SID_NOMINALSPACING/SID_NOMINALSPACING/3.;
	
  return StatusCode::SUCCESS;
}


StatusCode AFPSiDBasicKalmanTool::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode AFPSiDBasicKalmanTool::AFPCollectionReading()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::AFPCollectionReading()");

  SIDHIT SIDHit;

  m_ListSIDHits.clear();

  StatusCode sc = evtStore()->retrieve(m_siHitContainer, m_hitsContainerName); 
  if(sc.isFailure() || !m_siHitContainer)
    {
      return StatusCode::SUCCESS;
    }

  xAOD::AFPSiHitContainer::const_iterator hitIter = m_siHitContainer->begin();
  xAOD::AFPSiHitContainer::const_iterator mcSIDGenEnd = m_siHitContainer->end();

  for(;hitIter!=mcSIDGenEnd;++hitIter) {
    xAOD::AFPSiHit* hit = *hitIter;
    SIDHit.fADC  			= hit->depositedCharge();
    SIDHit.fTDC 			= 0;
    SIDHit.nDetectorID  		= hit->pixelLayerID();
    SIDHit.nStationID 		= hit->stationID();
    SIDHit.nPixelRow		= hit->pixelVertID();
    SIDHit.nPixelCol		= hit->pixelHorizID();

    m_ListSIDHits.push_back(SIDHit);
  }

  ATH_MSG_DEBUG("end AFP_SIDLocReco::AFPCollectionReading()");

  return StatusCode::SUCCESS;
}

StatusCode AFPSiDBasicKalmanTool::reconstructTracks()
{
  MsgStream LogStream(Athena::getMessageSvc(), "AFPSiDBasicKalmanTool::Execute()");

  m_listResults.clear();
  pTrkSeeds.clear();
  m_ListSIDHits.clear();
  m_MapSIDHitPos.clear();  


  CHECK(AFPCollectionReading());

  // x-y-z map of hit pixels
  list<SIDHIT>::const_iterator iter;
  for (iter=m_ListSIDHits.begin(); iter!=m_ListSIDHits.end(); iter++)
    {
      if ((*iter).fADC > m_AmpThresh)
	{
	  FillSIDHITPOS(*iter, m_MapSIDHitPos);
	}
    }


  ////////////////////////
  GetTrkSeeds();
  ////////////////////////

  vector<SIDHITSEED>::const_iterator iter1;
  for (iter1= pTrkSeeds.begin(); iter1!= pTrkSeeds.end(); iter1++)
    {
      ClearMatrix();

      Int_t LastPlate = (*iter1).nLastPlate;
      Int_t DoubleHole1=1;
      Int_t DoubleHole2=0;

      if ( FillTrkPropagators(*iter1, ++LastPlate) )
	{
	  ++LastPlate;
	  DoubleHole1=0;
	  for (Int_t i=LastPlate; i<SIDCNT; i++)
	    {
	      if ( FillTrkPropagators((*iter1).nStationID, i) )
		{
		  if(DoubleHole1==1) DoubleHole1=0;
		}
	      else
		{
		  ++DoubleHole1;
		}
	      if(DoubleHole1==2) break;
				
	    }	
	  if(DoubleHole1==2) {
	    continue;	
	  }
	}
		
      else if ( FillTrkPropagators(*iter1, ++LastPlate) )
	{
	  ++LastPlate;
	  DoubleHole2=0;
	  for (Int_t i=LastPlate; i<SIDCNT; i++)
	    {
	      if ( FillTrkPropagators((*iter1).nStationID, i) )
		{
		  if(DoubleHole2==1) DoubleHole2=0;
		}
	      else
		{
		  ++DoubleHole2;
		}
	      if(DoubleHole2==2) break;
				
	    }	
	  if(DoubleHole2==2) {
	    continue;		
	  }
	}
		
      else {
	continue; // double hole after the seed pixels
      }

      if( DoubleHole1 > 1 || DoubleHole2 != 0) {
	continue ;
      }
	

      LogStream << MSG::DEBUG << "Found new track candidate with parameters:" << endreq;
      LogStream << MSG::DEBUG << "Hit ID's : ";
      vector<Int_t>::const_iterator iter2;
      for (iter2= HID.begin(); iter2!= HID.end(); iter2++) LogStream << MSG::DEBUG << (*iter2) << "\t";
      LogStream << endreq;
		
      LogStream << MSG::DEBUG << "Filtered parameters : X, DX, Y, DY, chi2" << endreq;
      vector< CLHEP::HepVector >::const_iterator iter3;
      vector< Float_t >::const_iterator iter4 = chik.begin();
      LogStream << MSG::DEBUG << (*iter1).fSeedX << "\t" << (*iter1).fSeedDX << "\t" << (*iter1).fSeedY << "\t" << (*iter1).fSeedDY << "\t" << 0 << endreq;
      for (iter3=xk.begin(); iter3!= xk.end(); iter3++)
	{
	  LogStream << MSG::DEBUG << (*iter3)[0] << "\t" << (*iter3)[1] << "\t" << (*iter3)[2] << "\t" << (*iter3)[3] << "\t" << (*iter4) << endreq;
	  iter4++;
	}

                		
      ////////////
      Smooth();
      ////////////				

      Float_t Chi2Sum = 0;
		
      LogStream << MSG::DEBUG << "Smoothed parameters : X, DX, Y, DY, chi2" << endreq;
      vector< CLHEP::HepVector >::const_reverse_iterator iter5;
      vector< Float_t >::const_reverse_iterator iter6 = chikS.rbegin();
      for (iter5=xkS.rbegin(); iter5!= xkS.rend(); ++iter5)
	{				
	  LogStream << MSG::DEBUG << (*iter5)[0] << "\t" << (*iter5)[1] << "\t" << (*iter5)[2] << "\t" << (*iter5)[3] << "\t" << (*iter6) << endreq;
	  Chi2Sum += (*iter6);
	  ++iter6;					
	}
      LogStream << endreq;
		
		
      //////////////////////////////////////////
      // fill tracking collection
      SIDRESULT Results;		
      Results.nStationID  = (*iter1).nStationID;	
      Results.x_pos = xkS.back()[0];
      Results.y_pos = xkS.back()[2];
      Results.z_pos = z0;
      Results.x_slope = xkS.back()[1];
      Results.y_slope = xkS.back()[3];
      Results.z_slope = 1.;	
      Results.nHits = xkS.size();
      Results.nHoles = SIDCNT - xkS.size();
      Results.fChi2 = xkS.size() + (MAXCHI2TRK - Chi2Sum/(Float_t)chikS.size())/(MAXCHI2TRK+1.);			
      Results.ListHitID = HID;				
      m_listResults.push_back(Results);
      //////////////////////////////////////////
    }

  //////////////////////
  FilterTrkCollection();
  //////////////////////
		
	
  if (m_listResults.size()!=0)
    {
      list<SIDRESULT>::const_iterator iter7;
      LogStream << MSG::INFO << "Filtered tracks parameters : X, DX, Y, DY, Z, quality: " << endreq;
      for (iter7=m_listResults.begin(); iter7!=m_listResults.end(); iter7++)
	{
	  LogStream << MSG::INFO << std::fixed << std::setprecision(6) \
		    << (*iter7).x_pos << "\t" << (*iter7).x_slope << "\t" << (*iter7).y_pos << "\t" << (*iter7).y_slope <<"\t"<< (*iter7).z_pos << "\t" << (*iter7).fChi2 << endreq;
	}
      LogStream << endreq;
    }
	

  saveToXAOD().isSuccess();

  return StatusCode::SUCCESS;
}

void AFPSiDBasicKalmanTool::FillSIDHITPOS(const SIDHIT &SIDHit, map<Int_t, SIDHITPOS> &MapSIDHitPos)
{
  SIDHITPOS SIDHitPos;


  Int_t nStID    = SIDHit.nStationID; 
  Int_t nPlID    = SIDHit.nDetectorID;
  Int_t nPixCol  = SIDHit.nPixelCol;
  Int_t nPixRow  = SIDHit.nPixelRow;

  SIDHitPos.nStationID  = nStID;
  SIDHitPos.nPlateID    = nPlID;
  SIDHitPos.fPixX       = fxMapSID[nStID][nPlID][nPixCol][nPixRow];
  SIDHitPos.fPixY       = fyMapSID[nStID][nPlID][nPixCol][nPixRow];
  SIDHitPos.fPixZ       = fzMapSID[nStID][nPlID][nPixCol][nPixRow];	
  SIDHitPos.fAmp	      = SIDHit.fADC;

  Int_t HitID = nStID*1000000+nPlID*100000+nPixCol*100+nPixRow;

  // Remove close hit pixel in X wrt signal amplitude
  // Actually do simple clustering
  map<Int_t, SIDHITPOS>::iterator iter0 = MapSIDHitPos.find(HitID-100);
  map<Int_t, SIDHITPOS>::iterator iter1 = MapSIDHitPos.find(HitID+100);
	
  if( iter0==MapSIDHitPos.end() && iter1==MapSIDHitPos.end())
    {
      MapSIDHitPos.insert(pair<Int_t, SIDHITPOS>(HitID, SIDHitPos));
    }
	
  else if( iter0!=MapSIDHitPos.end() )
    {
      SIDHitPos.fPixX = SIDHitPos.fPixX*SIDHitPos.fAmp + (*iter0).second.fPixX*(*iter0).second.fAmp;
      SIDHitPos.fPixY = SIDHitPos.fPixY*SIDHitPos.fAmp + (*iter0).second.fPixY*(*iter0).second.fAmp;
      SIDHitPos.fPixZ = SIDHitPos.fPixZ*SIDHitPos.fAmp + (*iter0).second.fPixZ*(*iter0).second.fAmp;

      SIDHitPos.fAmp += (*iter0).second.fAmp;
      SIDHitPos.fPixX /=SIDHitPos.fAmp;
      SIDHitPos.fPixY	/=SIDHitPos.fAmp;
      SIDHitPos.fPixZ	/=SIDHitPos.fAmp;                

      MapSIDHitPos.erase(iter0);
      MapSIDHitPos.insert(pair<Int_t, SIDHITPOS>(HitID, SIDHitPos));
    }
  else
    {
      SIDHitPos.fPixX = SIDHitPos.fPixX*SIDHitPos.fAmp + (*iter1).second.fPixX*(*iter1).second.fAmp;
      SIDHitPos.fPixY = SIDHitPos.fPixY*SIDHitPos.fAmp + (*iter1).second.fPixY*(*iter1).second.fAmp;
      SIDHitPos.fPixZ = SIDHitPos.fPixZ*SIDHitPos.fAmp + (*iter1).second.fPixZ*(*iter1).second.fAmp;

      SIDHitPos.fAmp += (*iter1).second.fAmp;
      SIDHitPos.fPixX	/=SIDHitPos.fAmp;
      SIDHitPos.fPixY /=SIDHitPos.fAmp;
      SIDHitPos.fPixZ /=SIDHitPos.fAmp;

      MapSIDHitPos.erase(iter1);
      MapSIDHitPos.insert(pair<Int_t, SIDHITPOS>(HitID, SIDHitPos));
    }
}

StatusCode AFPSiDBasicKalmanTool::saveToXAOD ()
{
  xAOD::AFPTrackContainer* containerToFill = new xAOD::AFPTrackContainer();
  CHECK( evtStore()->record(containerToFill, m_tracksContainerName) );
  xAOD::AFPTrackAuxContainer* trackAuxContainer = new xAOD::AFPTrackAuxContainer();
  CHECK( evtStore()->record(trackAuxContainer, m_tracksContainerName + "Aux.") );
  containerToFill->setStore(trackAuxContainer);


  for(list<SIDRESULT>::const_iterator iter=m_listResults.begin(); iter!=m_listResults.end(); iter++) {
    if (iter->nStationID != -1) {
      xAOD::AFPTrack* track = new xAOD::AFPTrack;
      containerToFill->push_back(track);
			    
      track->setStationID(iter->nStationID);
      track->setXLocal(iter->x_pos);
      track->setYLocal(iter->y_pos);
      track->setZLocal(iter->z_pos);
      track->setXSlope(iter->x_slope);
      track->setYSlope(iter->y_slope);
      track->setNHits(iter->nHits);
      track->setNHoles(iter->nHoles);
      track->setChi2(iter->fChi2);

      std::vector<int>::const_iterator end = iter->ListHitID.end();
      for (std::vector<int>::const_iterator hitIter = iter->ListHitID.begin(); hitIter != end; ++hitIter) {
	const int pixelRow = (*hitIter)%100;
	const int pixelCol = (( (*hitIter)/100) % 1000);
	const int pixelLayer = ( (*hitIter)/100000) % 10;
	const int pixelStation = ( (*hitIter)/1000000) % 10;
	unsigned int result = 0;
	auto endHits = m_siHitContainer->end();
	for (auto origHitIter = m_siHitContainer->begin(); origHitIter != endHits; ++origHitIter) {
	  if (
	      (*origHitIter)->pixelVertID() == pixelRow
	      && (*origHitIter)->pixelHorizID() == pixelCol
	      && (*origHitIter)->pixelLayerID() == pixelLayer
	      && (*origHitIter)->stationID() == pixelStation
	      )
	    break;

	  result++;
	}

	// check if the hit was found
	if (result < m_siHitContainer->size()) {
	  ATH_MSG_DEBUG("To the list of hits in a track adding hit "<<result<<"/"<<m_siHitContainer->size()<<".");

	  ElementLink< xAOD::AFPSiHitContainer >* hitLink = new ElementLink< xAOD::AFPSiHitContainer >;
	  hitLink->toIndexedElement(*m_siHitContainer, result);
	  track->addHit(*hitLink);
	}
	else
	  ATH_MSG_WARNING("Track hit not found in hits list. HitID: "<<(*hitIter)
			  <<"  station: "<<pixelStation
			  <<"  layer: "<<pixelLayer
			  <<"  row: "<<pixelRow
			  <<"  col: "<<pixelCol
			  <<"  dataType: "<<m_iDataType
			  );
      }
    }
  }

  return StatusCode::SUCCESS;
}

void AFPSiDBasicKalmanTool::GetTrkSeeds()
{	
  map<Int_t, SIDHITPOS> MapSIDHitPosSeed0;
  map<Int_t, SIDHITPOS> MapSIDHitPosSeed1;

  list<SIDHIT>::const_iterator iter;
  for (iter=m_ListSIDHits.begin(); iter!=m_ListSIDHits.end(); iter++)
    {
      if ((*iter).fADC > m_AmpThresh && (*iter).nDetectorID == 0)
	{
	  FillSIDHITPOS(*iter, MapSIDHitPosSeed0); 
	}
		
      else if ((*iter).fADC > m_AmpThresh && (*iter).nDetectorID == 1)
	{
	  FillSIDHITPOS(*iter, MapSIDHitPosSeed1);
	}
    }

  map<Int_t, SIDHITPOS>::const_iterator iter0;
  map<Int_t, SIDHITPOS>::const_iterator iter1;
  for (iter0=MapSIDHitPosSeed0.begin(); iter0!=MapSIDHitPosSeed0.end(); iter0++)
    {
      for (iter1=MapSIDHitPosSeed1.begin(); iter1!=MapSIDHitPosSeed1.end(); iter1++)
	{
	  if ( (*iter0).second.nStationID == (*iter1).second.nStationID && 1 /* place for additional discriminant condition*/)
	    {
	      SIDHITSEED SIDHitSeed;
	      SIDHitSeed.nHitID1 = (*iter0).first;
	      SIDHitSeed.nHitID2 = (*iter1).first;
	      SIDHitSeed.nLastPlate = 1;
	      SIDHitSeed.nStationID = (*iter0).second.nStationID;
	      SIDHitSeed.fSeedX  = (*iter1).second.fPixX;
	      SIDHitSeed.fSeedDX = ((*iter1).second.fPixX-(*iter0).second.fPixX)/abs((*iter1).second.fPixZ-(*iter0).second.fPixZ);
	      SIDHitSeed.fSeedY  = (*iter1).second.fPixY;
	      SIDHitSeed.fSeedDY = ((*iter1).second.fPixY-(*iter0).second.fPixY)/abs((*iter1).second.fPixZ-(*iter0).second.fPixZ);
	      SIDHitSeed.fSeedZ  = (*iter1).second.fPixZ;

	      pTrkSeeds.push_back(SIDHitSeed);			
	    }
	}
    }


}


bool AFPSiDBasicKalmanTool::FillTrkPropagators(const SIDHITSEED &SIDHitSeed, Int_t plateF)
{
  // This sensoor is not active

  if( SIDHitSeed.nStationID == 2 && plateF==3) return 0;

  HID.clear();
  HID.push_back(SIDHitSeed.nHitID1);
  HID.push_back(SIDHitSeed.nHitID2);
	
  Float_t Zdist = (fzMapSID[SIDHitSeed.nStationID][plateF][1][0] - fzMapSID[SIDHitSeed.nStationID][plateF][0][0])/(fxMapSID[SIDHitSeed.nStationID][plateF][1][0] - fxMapSID[SIDHitSeed.nStationID][plateF][0][0])*(SIDHitSeed.fSeedX - fxMapSID[SIDHitSeed.nStationID][plateF][0][0]) + fzMapSID[SIDHitSeed.nStationID][plateF][0][0] - SIDHitSeed.fSeedZ;
	
  CLHEP::HepMatrix Fi(4, 4, 1);
  Fi[0][1] = Zdist;
  Fi[2][3] = Zdist;
	
  CLHEP::HepVector x0i(4); 
  x0i[0] = SIDHitSeed.fSeedX;
  x0i[1] = SIDHitSeed.fSeedDX;
  x0i[2] = SIDHitSeed.fSeedY;
  x0i[3] = SIDHitSeed.fSeedDY;
  CLHEP::HepVector m0i(2); 
  m0i[0] = SIDHitSeed.fSeedX;
  m0i[1] = SIDHitSeed.fSeedY;

	
  CLHEP::HepVector xiP = Fi*x0i;
  CLHEP::HepMatrix CiP = Fi*C0*Fi.T();//+Qk;
	
  //mk finder
  CLHEP::HepVector mi(2);
  Float_t XYdist=100.*CLHEP::mm;
  Int_t HitID=-1;
  map<Int_t, SIDHITPOS>::const_iterator iter;
  for (iter=m_MapSIDHitPos.begin(); iter!=m_MapSIDHitPos.end(); iter++)
    {
      if( (*iter).second.nStationID != SIDHitSeed.nStationID ) continue;
      if( (*iter).second.nPlateID != plateF ) 		 continue;
      Float_t minXYdist = sqrt( pow((*iter).second.fPixX-xiP[0],2)+pow((*iter).second.fPixY-xiP[2],2) );
      if( minXYdist < XYdist )
	{
	  XYdist = minXYdist;
	  mi[0] = (*iter).second.fPixX;
	  mi[1] = (*iter).second.fPixY;
	  HitID = (*iter).first;
	}
    }
  /////////////////////////
  if (HitID == -1) return 0;  // no hits in layer
  /////////////////////////
	
  CLHEP::HepVector riP = mi - Hk*xiP;
  CLHEP::HepMatrix RiP = Vk + Hk*CiP*Hk.T();

  CLHEP::HepMatrix Ki = CiP*Hk.T()*qr_inverse(RiP);
  CLHEP::HepVector xi = xiP + Ki*riP;
	
  CLHEP::HepMatrix Ci = CiP - Ki*Hk*CiP;
	
  CLHEP::HepVector ri = mi - Hk*xi;
  CLHEP::HepMatrix Ri = Vk - Hk*Ki*Vk;
	
  //chi2 statistics to remove fakes
  CLHEP::HepVector chii = ri.T()*qr_inverse(Ri)*ri;

  //////////////////////////////
  if (chii[0]>MAXCHI2HIT) return 0; // no good hit in layer
  //////////////////////////////
	
  Fk.push_back(Fi);
  CkP.push_back(CiP);
  Ck.push_back(Ci);
  mk.push_back(mi);
  xk.push_back(xi);
  xkP.push_back(xiP);
  chik.push_back(chii[0]);
	
  m0 = m0i;
  x0 = x0i;	
	
  HID.push_back(HitID);
  zk.push_back( (fzMapSID[SIDHitSeed.nStationID][plateF][1][0] - fzMapSID[SIDHitSeed.nStationID][plateF][0][0])/(fxMapSID[SIDHitSeed.nStationID][plateF][1][0] - fxMapSID[SIDHitSeed.nStationID][plateF][0][0])*(xi[0] - fxMapSID[SIDHitSeed.nStationID][plateF][0][0]) + fzMapSID[SIDHitSeed.nStationID][plateF][0][0] );

	
  return 1;
}


bool AFPSiDBasicKalmanTool::FillTrkPropagators(Int_t stID, Int_t plateF)
{
  Float_t Zdist = (fzMapSID[stID][plateF][1][0] - fzMapSID[stID][plateF][0][0])/(fxMapSID[stID][plateF][1][0] - fxMapSID[stID][plateF][0][0])*(xk.back()[0] - fxMapSID[stID][plateF][0][0]) + fzMapSID[stID][plateF][0][0] - zk.back();
	
  // This sensor is not active
  if( stID == 2 && plateF == 3) return 0;

  CLHEP::HepMatrix Fi(4, 4, 1);
  Fi[0][1] = Zdist;
  Fi[2][3] = Zdist;
	
  CLHEP::HepVector xiP = Fi*xk.back();
  CLHEP::HepMatrix CiP = Fi*Ck.back()*Fi.T();//+Qk;
	
  //mk finder
  CLHEP::HepVector mi(2);
  Float_t XYdist=100.*CLHEP::mm;
  Int_t HitID=-1;
  map<Int_t, SIDHITPOS>::const_iterator iter;
  for (iter=m_MapSIDHitPos.begin(); iter!=m_MapSIDHitPos.end(); iter++)
    {
      if( (*iter).second.nStationID != stID ) continue;
      if( (*iter).second.nPlateID != plateF ) continue;
      Float_t minXYdist = sqrt( pow((*iter).second.fPixX-xiP[0],2)+pow((*iter).second.fPixY-xiP[2],2) );
      if( minXYdist < XYdist )
	{
	  XYdist = minXYdist;
	  mi[0] = (*iter).second.fPixX;
	  mi[1] = (*iter).second.fPixY;
	  HitID = (*iter).first;
	}
    }


  /////////////////////////
  if (HitID == -1) return 0;  // no hits in layer
  /////////////////////////
	
  CLHEP::HepVector riP = mi - Hk*xiP;
  CLHEP::HepMatrix RiP = Vk + Hk*CiP*Hk.T();

  CLHEP::HepMatrix Ki = CiP*Hk.T()*qr_inverse(RiP);
  CLHEP::HepVector xi = xiP + Ki*riP;
	
  CLHEP::HepMatrix Ci = CiP - Ki*Hk*CiP;
	
  CLHEP::HepVector ri = mi - Hk*xi;
  CLHEP::HepMatrix Ri = Vk - Hk*Ki*Vk;
	
  //chi2 statistics to remove fakes
  CLHEP::HepVector chii = ri.T()*qr_inverse(Ri)*ri;
	

  //////////////////////////////
  if (chii[0]>MAXCHI2HIT) return 0; // no good hit in layer
  //////////////////////////////
	
	
  Fk.push_back(Fi);
  CkP.push_back(CiP);
  Ck.push_back(Ci);
  mk.push_back(mi);
  xk.push_back(xi);
  xkP.push_back(xiP);
  chik.push_back(chii[0]);
	
  HID.push_back(HitID);
  zk.push_back( (fzMapSID[stID][plateF][1][0] - fzMapSID[stID][plateF][0][0])/(fxMapSID[stID][plateF][1][0] - fxMapSID[stID][plateF][0][0])*(xi[0] - fxMapSID[stID][plateF][0][0]) + fzMapSID[stID][plateF][0][0] );

  return 1;
}


void AFPSiDBasicKalmanTool::Smooth()
{
  CLHEP::HepVector xiS;
  CLHEP::HepMatrix CiS;


  if( xk.size() > 1) {
    for (int i=(xk.size()-2); i>=0; i--)
      {
	if(i==((int)xk.size()-2))
	  {
	    xiS = xk[xk.size()-1];	// !!!!!!!!!
	    CiS = Ck[xk.size()-1];	// !!!!!!!!!
			
	    xkS.push_back(xiS);
	    CkS.push_back(CiS);
	    CLHEP::HepVector riiS = mk[xk.size()-1] - Hk*xiS;
	    CLHEP::HepMatrix RiiS = Vk - Hk*CiS*Hk.T();
	    CLHEP::HepVector chiiS = riiS.T()*qr_inverse(RiiS)*riiS;
	    chikS.push_back(chiiS[0]);
	  }
	else
	  {
	    xiS = xkS.back();
	    CiS = CkS.back();
	  }
		
	CLHEP::HepMatrix Ai = Ck[i]*Fk[i+1].T()*qr_inverse(CkP[i+1]);		
	CLHEP::HepVector xiiS = xk[i] + Ai*( xiS - xkP[i+1] );				
	CLHEP::HepMatrix CiiS = Ck[i] + Ai*( CiS - CkP[i+1] )*Ai.T();		
		
	CLHEP::HepVector riiS = mk[i] - Hk*xiiS;
	CLHEP::HepMatrix RiiS = Vk - Hk*CiiS*Hk.T();
	CLHEP::HepVector chiiS = riiS.T()*qr_inverse(RiiS)*riiS;
		
	xkS.push_back(xiiS);
	CkS.push_back(CiiS);
	chikS.push_back(chiiS[0]);
      }
  }
  else if (xk.size() > 0) {
    xiS = xk[xk.size()-1];  // !!!!!!!!!
    CiS = Ck[xk.size()-1];  // !!!!!!!!!

    xkS.push_back(xiS);
    CkS.push_back(CiS);
    CLHEP::HepVector riiS = mk[xk.size()-1] - Hk*xiS;
    CLHEP::HepMatrix RiiS = Vk - Hk*CiS*Hk.T();
    CLHEP::HepVector chiiS = riiS.T()*qr_inverse(RiiS)*riiS;
    chikS.push_back(chiiS[0]);
  }

  CLHEP::HepMatrix Ai = C0*Fk[0].T()*qr_inverse(CkP[0]);

  CLHEP::HepVector xiiS = x0 + Ai*( xkS.back() - xkP[0] );		

  CLHEP::HepMatrix CiiS = C0 + Ai*( CkS.back() - CkP[0] )*Ai.T();
		
  CLHEP::HepVector riiS = m0 - Hk*xiiS;
  CLHEP::HepMatrix RiiS = Vk - Hk*CiiS*Hk.T();


  CLHEP::HepVector chiiS = riiS.T()*qr_inverse(RiiS)*riiS;
	
  CkS.push_back(CiiS);
  xkS.push_back(xiiS);
  chikS.push_back(chiiS[0]);

  // first hit !!!!!!!!!!
  CLHEP::HepMatrix Fi(4, 4, 1);
  Fi[0][1] = (*m_MapSIDHitPos.find(HID[1])).second.fPixZ - (*m_MapSIDHitPos.find(HID[0])).second.fPixZ;
  Fi[2][3] = (*m_MapSIDHitPos.find(HID[1])).second.fPixZ - (*m_MapSIDHitPos.find(HID[0])).second.fPixZ;
	
  Ai = qr_inverse(Fi); // when Qk == 0
  xiiS = Ai*( xkS.back() );			
  CiiS = C0 + Ai*( CkS.back() - C0 )*Ai.T();
		
  riiS = Hk*Ai*(x0 - xkS.back());
  RiiS = Vk - Hk*CiiS*Hk.T();
  chiiS = riiS.T()*qr_inverse(RiiS)*riiS;
	
  CkS.push_back(CiiS);
  xkS.push_back(xiiS);
  chikS.push_back(chiiS[0]);
	
  z0 = (*m_MapSIDHitPos.find(HID[0])).second.fPixZ;
}

void AFPSiDBasicKalmanTool::FilterTrkCollection()
{
  //filtering tracking collection using shared hits + quality requirement
  list<SIDRESULT>::iterator iter1;
  list<SIDRESULT>::iterator iter2;
  for (iter1=m_listResults.begin(); iter1!=m_listResults.end(); iter1++)
    {
      for (iter2=m_listResults.begin(); iter2!=m_listResults.end(); )
	{
	  if ( GetSharedHits((*iter1).ListHitID, (*iter2).ListHitID) > MAXSHAREDHITS && iter1!=iter2 )
	    {
	      if ( (*iter1).fChi2 < (*iter2).fChi2 )
		{
		  ++iter2;
		}
	      else
		{
		  iter2 = m_listResults.erase(iter2);
		}		
	    }	
	  else 	++iter2;
	}
	
    }
	
}

Int_t AFPSiDBasicKalmanTool::GetSharedHits(const vector<Int_t> &HID1, const vector<Int_t>  &HID2)
{
  Int_t SharedHits = 0;
	
  vector<Int_t>::const_iterator iter1;
  vector<Int_t>::const_iterator iter2;
	
  for (iter1=HID1.begin(); iter1!=HID1.end(); iter1++)
    {
      for (iter2=HID2.begin(); iter2!=HID2.end(); iter2++)
	{
	  if ((*iter1) == (*iter2)) SharedHits++;
	}	
    }
	
  return SharedHits;
}

void AFPSiDBasicKalmanTool::ClearMatrix()
{
  Fk.clear();
  Ck.clear();
  CkP.clear();
  Rk.clear();

  xk.clear();
  xkP.clear();
  rk.clear();
  rkP.clear();
  mk.clear();
  chik.clear();	
	
  zk.clear();
  HID.clear();	
	
  xkS.clear();
  CkS.clear();
  chikS.clear();
	

}

void AFPSiDBasicKalmanTool::GetData()
{
  MsgStream LogStream(Athena::getMessageSvc(), "AFPSiDBasicKalmanTool::GetData()");
  LogStream << MSG::DEBUG << "begin AFPSiDBasicKalmanTool::GetData()" << endreq;

  LogStream << MSG::DEBUG << "end AFPSiDBasicKalmanTool::GetData()" << endreq;
}
