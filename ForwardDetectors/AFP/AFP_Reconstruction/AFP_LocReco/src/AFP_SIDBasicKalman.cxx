/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_SIDBasicKalman.h"

AFP_SIDBasicKalman::AFP_SIDBasicKalman()
{
	m_MyFile = NULL;
	m_histS1_PixMap = NULL;
	m_histS2_PixMap = NULL;
	m_histS3_PixMap = NULL;
	m_histS4_PixMap = NULL;
	
	m_listResults.clear();

	m_AmpThresh = 0;
	m_iDataType = 0;
	
	m_pTrkSeeds.clear();
	
	m_Fk.clear();
	m_Ck.clear();
	m_CkP.clear();
	m_Rk.clear();

	m_xk.clear();
	m_xkP.clear();
	m_rk.clear();
	m_rkP.clear();
	m_mk.clear();
	m_chik.clear();
	
	m_zk.clear();
	m_HID.clear();
	
	m_xkS.clear();
	m_CkS.clear();
	m_chikS.clear();
	
	m_z0 = 0;
}

AFP_SIDBasicKalman::~AFP_SIDBasicKalman()
{
	m_listResults.clear();
}

StatusCode AFP_SIDBasicKalman::Initialize(float fAmpThresh, int iDataType, const std::list<SIDHIT> &ListSIDHits, Float_t fsSID[SIDSTATIONID][SIDCNT], Float_t fxSID[SIDSTATIONID][SIDCNT], Float_t fySID[SIDSTATIONID][SIDCNT], Float_t fzSID[SIDSTATIONID][SIDCNT])
{

      	m_AmpThresh = (float)fAmpThresh;
        m_AmpThresh = 1000;
	m_iDataType = iDataType;
	m_ListSIDHits = ListSIDHits;
	
	Float_t delta_pixel_x = 0.050*CLHEP::mm; // size of pixel along x, in mm
	Float_t delta_pixel_y = 0.250*CLHEP::mm; // size of pixel along y, in mm

        Float_t interPlaneX[4][4] ={{0,0,0,0},
                                    {0,0,0,0},
                                    {0,-0.187358,-0.127971,0},
                                    {0,-0.101478,-0.0661546,-0.0675869}};
        Float_t interPlaneY[4][4] ={{0,0,0,0},
                                    {0,0,0,0},
                                    {0,-0.134756, -0.204807,0},
                                    {0, -0.155841, -0.334444,-0.341143}};
        Float_t Alpha[4][4] ={{0,0,0,0},
                              {0,0,0,0}, 
                              {0.2443461,0.253198,0.227862,0}, 
                              {0.2443461,0.250661,0.252849,0.247896}};

	
	// x-y-z map of all pixels
	for(Int_t nStationID = 0; nStationID < SIDSTATIONID; nStationID++)
	{				
		for (Int_t nPlateID = 0; nPlateID < SIDCNT; nPlateID++)
		{

                        if ( iDataType == 1) fsSID[nStationID][nPlateID] = Alpha[nStationID][nPlateID];

			for (Int_t nPixel_x = 0; nPixel_x < 336; nPixel_x++)
			{	
				for (Int_t nPixel_y = 0; nPixel_y < 80; nPixel_y++)
				{	                                 		
				m_fxMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] = fxSID[nStationID][nPlateID]+(delta_pixel_x*(nPixel_x-168))*cos(fsSID[nStationID][nPlateID]);		//sign changed!	
				m_fyMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] = fySID[nStationID][nPlateID]+(delta_pixel_y*nPixel_y); //sign changed!
				m_fzMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] = fzSID[nStationID][nPlateID] - ((nStationID<2)?1.:-1.)*(delta_pixel_x*(nPixel_x-168))*sin(fsSID[nStationID][nPlateID]); //sign changed!
			        if( iDataType == 1) {
                                m_fxMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] += interPlaneX[nStationID][nPlateID] + 168*delta_pixel_x*cos(fsSID[nStationID][0]);
                                m_fyMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] += interPlaneY[nStationID][nPlateID];
                                m_fzMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] += -168*((nStationID<2)?1.:-1.)*delta_pixel_x*sin(fsSID[nStationID][0]); }
                                else {
                                m_fxMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] += 168*delta_pixel_x*cos(fsSID[nStationID][0]);
                                m_fzMapSID[nStationID][nPlateID][nPixel_x][nPixel_y] += -168*((nStationID<2)?1.:-1.)*delta_pixel_x*sin(fsSID[nStationID][0]); } 
                               }
			}
		}
	}

	// x-y-z map of hit pixels
        std::list<SIDHIT>::const_iterator iter;
	for (iter=ListSIDHits.begin(); iter!=ListSIDHits.end(); iter++)
	{
         	if ((*iter).fADC > m_AmpThresh)
		{
			FillSIDHITPOS(*iter, m_MapSIDHitPos);
			
			/*SIDHITPOS SIDHitPos;
			Int_t nStID    = (*iter).nStationID;
			Int_t nPlID    = (*iter).nDetectorID;
			Int_t nPixCol  = (*iter).nPixelCol;
			Int_t nPixRow  = (*iter).nPixelRow;

		
			SIDHitPos.nStationID  = nStID;
			SIDHitPos.nPlateID    = nPlID;
			SIDHitPos.fPixX       = fxSID[nStID][nPlID] - (delta_pixel_x*nPixCol+delta_pixel_x/2.)*cos(fsSID[nStID][nPlID]);
			SIDHitPos.fPixY       = fySID[nStID][nPlID] - (delta_pixel_y*nPixRow+delta_pixel_y/2.);
			SIDHitPos.fPixZ       = ((nStID<2)?1.:-1.)*fzSID[nStID][nPlID] + ((nStID<2)?1.:-1.)*(delta_pixel_x*nPixCol+delta_pixel_x/2.)*sin(fsSID[nStID][nPlID]);	
		
			Int_t HitID = nStID*1000000+nPlID*100000+nPixCol*100+nPixRow;
			
			if(m_MapSIDHitPos.find(HitID-100)==m_MapSIDHitPos.end() && m_MapSIDHitPos.find(HitID+100)==m_MapSIDHitPos.end())
			{
			m_MapSIDHitPos.insert(pair<Int_t, SIDHITPOS>(HitID, SIDHitPos));
			}*/
		}
	}
	
	// Kalman matrices initialization
	m_Hk = CLHEP::HepMatrix(2, 4, 0);
	m_Hk[0][0] = 1.;
	m_Hk[1][2] = 1.;
	
	m_Qk = CLHEP::HepMatrix(4, 4, 0);	
	
	m_Vk = CLHEP::HepMatrix(2, 2, 0);
	m_Vk[0][0] = delta_pixel_x*delta_pixel_x;
	m_Vk[1][1] = delta_pixel_y*delta_pixel_y;
	
	m_C0 = CLHEP::HepMatrix(4, 4, 0);
	m_C0[0][0] = delta_pixel_x*delta_pixel_x/3.;
	m_C0[1][1] = delta_pixel_x*delta_pixel_x/SID_NOMINALSPACING/SID_NOMINALSPACING/3.;
	//m_C0[1][0] = 0;
	//m_C0[0][1] = 0;
	
	m_C0[2][2] = delta_pixel_y*delta_pixel_y/3.;
	m_C0[3][3] = delta_pixel_y*delta_pixel_y/SID_NOMINALSPACING/SID_NOMINALSPACING/3.;
	//m_C0[3][2] = 0;
	//m_C0[2][3] = 0;
		
	//HistInitialize();
	
	return StatusCode::SUCCESS;
}

StatusCode AFP_SIDBasicKalman::Execute()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_SIDBasicKalman::Execute()");
	
//	/*
        std::map<Int_t, SIDHITPOS>::const_iterator iter0;
	for (iter0=m_MapSIDHitPos.begin(); iter0!=m_MapSIDHitPos.end(); iter0++)
	{
                /*
		if( (*iter0).second.nStationID==1 )      
		{	
			m_histS1_PixMap->Fill((*iter0).second.fPixZ/CLHEP::mm, (*iter0).second.fPixX/CLHEP::mm);
			m_histS3_PixMap->Fill((*iter0).second.fPixZ/CLHEP::mm, (*iter0).second.fPixY/CLHEP::mm);
		}
		else if( (*iter0).second.nStationID==0 )
		{
			m_histS2_PixMap->Fill((*iter0).second.fPixZ/CLHEP::mm, (*iter0).second.fPixX/CLHEP::mm);
			m_histS4_PixMap->Fill((*iter0).second.fPixZ/CLHEP::mm, (*iter0).second.fPixY/CLHEP::mm);
		}
               */
	}
//	*/


	////////////////////////
	GetTrkSeeds();
	////////////////////////

        std::vector<SIDHITSEED>::const_iterator iter1;
	for (iter1= m_pTrkSeeds.begin(); iter1!= m_pTrkSeeds.end(); iter1++)
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
			if(DoubleHole1==2) continue;	
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
			if(DoubleHole2==2) continue;		
		}
		
		else continue; // double hole after the seed pixels
	
                if( DoubleHole1 > 1 || DoubleHole2 != 0) continue ;
	

		LogStream << MSG::DEBUG << "Found new track candidate with parameters:" << endmsg;
		LogStream << MSG::DEBUG << "Hit ID's : ";
                std::vector<Int_t>::const_iterator iter2;
		for (iter2= m_HID.begin(); iter2!= m_HID.end(); iter2++) LogStream << MSG::DEBUG << (*iter2) << "\t";
		LogStream << endmsg;
		
		LogStream << MSG::DEBUG << "Filtered parameters : X, DX, Y, DY, chi2" << endmsg;
                std::vector< CLHEP::HepVector >::const_iterator iter3;
                std::vector< Float_t >::const_iterator iter4 = m_chik.begin();
                LogStream << MSG::DEBUG << (*iter1).fSeedX << "\t" << (*iter1).fSeedDX << "\t" << (*iter1).fSeedY << "\t" << (*iter1).fSeedDY << "\t" << 0 << endmsg;
		for (iter3=m_xk.begin(); iter3!= m_xk.end(); iter3++)
		{
			LogStream << MSG::DEBUG << (*iter3)[0] << "\t" << (*iter3)[1] << "\t" << (*iter3)[2] << "\t" << (*iter3)[3] << "\t" << (*iter4) << endmsg;
			iter4++;
		}

                		
		////////////
		Smooth();
		////////////				

		Float_t Chi2Sum = 0;
		
		LogStream << MSG::DEBUG << "Smoothed parameters : X, DX, Y, DY, chi2" << endmsg;
                std::vector< CLHEP::HepVector >::const_reverse_iterator iter5;
                std::vector< Float_t >::const_reverse_iterator iter6 = m_chikS.rbegin();
		for (iter5=m_xkS.rbegin(); iter5!= m_xkS.rend(); ++iter5)
		{				
			LogStream << MSG::DEBUG << (*iter5)[0] << "\t" << (*iter5)[1] << "\t" << (*iter5)[2] << "\t" << (*iter5)[3] << "\t" << (*iter6) << endmsg;
			Chi2Sum += (*iter6);
			++iter6;					
		}
		LogStream << endmsg;
		
		
		//////////////////////////////////////////
		// fill tracking collection
		SIDRESULT Results;		
		Results.nStationID  = (*iter1).nStationID;	
		Results.x_pos = m_xkS.back()[0];
		Results.y_pos = m_xkS.back()[2];
		Results.z_pos = m_z0;
		Results.x_slope = m_xkS.back()[1];
		Results.y_slope = m_xkS.back()[3];
		Results.z_slope = 1.;	
		Results.nHits = m_xkS.size();
		Results.nHoles = SIDCNT - m_xkS.size();
		Results.fChi2 = m_xkS.size() + (MAXCHI2TRK - Chi2Sum/(Float_t)m_chikS.size())/(MAXCHI2TRK+1.);			
		Results.ListHitID = m_HID;				
		m_listResults.push_back(Results);
		//////////////////////////////////////////
	}
	
        
	//////////////////////
	FilterTrkCollection();
	//////////////////////
		
	
	if (m_listResults.size()!=0)
	{
                std::list<SIDRESULT>::const_iterator iter7;
		LogStream << MSG::INFO << "Filtered tracks parameters : X, DX, Y, DY, Z, quality: " << endmsg;
		for (iter7=m_listResults.begin(); iter7!=m_listResults.end(); iter7++)
		{
		LogStream << MSG::INFO << std::fixed << std::setprecision(6) \
		<< (*iter7).x_pos << "\t" << (*iter7).x_slope << "\t" << (*iter7).y_pos << "\t" << (*iter7).y_slope <<"\t"<< (*iter7).z_pos << "\t" << (*iter7).fChi2 << endmsg;
		}
		LogStream << endmsg;
	}
	
	
	return StatusCode::SUCCESS;
}

StatusCode AFP_SIDBasicKalman::Finalize(std::list<SIDRESULT>* pListResults)
{
	*pListResults = m_listResults;

	//HistFinalize();
	
	return StatusCode::SUCCESS;
}

void AFP_SIDBasicKalman::FillSIDHITPOS(const SIDHIT &SIDHit, std::map<Int_t, SIDHITPOS> &MapSIDHitPos)
{
	SIDHITPOS SIDHitPos;


        Int_t nStID    = SIDHit.nStationID; 
	Int_t nPlID    = SIDHit.nDetectorID;
	Int_t nPixCol  = SIDHit.nPixelCol;
	Int_t nPixRow  = SIDHit.nPixelRow;

   // if ( m_iDataType ==1 ) {
   //   nPixCol  = 336-SIDHit.nPixelRow;
   //   nPixRow  = SIDHit.nPixelCol;
   // }
		
	SIDHitPos.nStationID  = nStID;
	SIDHitPos.nPlateID    = nPlID;
	SIDHitPos.fPixX       = m_fxMapSID[nStID][nPlID][nPixCol][nPixRow];
	SIDHitPos.fPixY       = m_fyMapSID[nStID][nPlID][nPixCol][nPixRow];
	SIDHitPos.fPixZ       = m_fzMapSID[nStID][nPlID][nPixCol][nPixRow];	
	SIDHitPos.fAmp	      = SIDHit.fADC;
		
	Int_t HitID = nStID*1000000+nPlID*100000+nPixCol*100+nPixRow;
	// Remove close hit pixel in X wrt signal amplitude
        // Actually do simple clustering
        std::map<Int_t, SIDHITPOS>::iterator iter0 = MapSIDHitPos.find(HitID-100);
        std::map<Int_t, SIDHITPOS>::iterator iter1 = MapSIDHitPos.find(HitID+100);
	
	if( iter0==MapSIDHitPos.end() && iter1==MapSIDHitPos.end())
	{
          MapSIDHitPos.insert(std::pair<Int_t, SIDHITPOS>(HitID, SIDHitPos));
	}
	
	else if( iter0!=MapSIDHitPos.end() )
	{
//		if ( (*iter0).second.fAmp < SIDHitPos.fAmp )
//		{


                SIDHitPos.fPixX = SIDHitPos.fPixX*SIDHitPos.fAmp + (*iter0).second.fPixX*(*iter0).second.fAmp;
                SIDHitPos.fPixY = SIDHitPos.fPixY*SIDHitPos.fAmp + (*iter0).second.fPixY*(*iter0).second.fAmp;
                SIDHitPos.fPixZ = SIDHitPos.fPixZ*SIDHitPos.fAmp + (*iter0).second.fPixZ*(*iter0).second.fAmp;

                SIDHitPos.fAmp += (*iter0).second.fAmp;
                SIDHitPos.fPixX /=SIDHitPos.fAmp;
                SIDHitPos.fPixY	/=SIDHitPos.fAmp;
                SIDHitPos.fPixZ	/=SIDHitPos.fAmp;                

		MapSIDHitPos.erase(iter0);
		MapSIDHitPos.insert(std::pair<Int_t, SIDHITPOS>(HitID, SIDHitPos));
//		}
	}
	
	else
	{
//		if ( (*iter1).second.fAmp < SIDHitPos.fAmp )
//		{

                SIDHitPos.fPixX = SIDHitPos.fPixX*SIDHitPos.fAmp + (*iter1).second.fPixX*(*iter1).second.fAmp;
                SIDHitPos.fPixY = SIDHitPos.fPixY*SIDHitPos.fAmp + (*iter1).second.fPixY*(*iter1).second.fAmp;
                SIDHitPos.fPixZ = SIDHitPos.fPixZ*SIDHitPos.fAmp + (*iter1).second.fPixZ*(*iter1).second.fAmp;

      	        SIDHitPos.fAmp += (*iter1).second.fAmp;
               	SIDHitPos.fPixX	/=SIDHitPos.fAmp;
       	       	SIDHitPos.fPixY /=SIDHitPos.fAmp;
       	       	SIDHitPos.fPixZ /=SIDHitPos.fAmp;

		MapSIDHitPos.erase(iter1);
		MapSIDHitPos.insert(std::pair<Int_t, SIDHITPOS>(HitID, SIDHitPos));
//		}
	}

}

void AFP_SIDBasicKalman::GetTrkSeeds()
{	
        std::map<Int_t, SIDHITPOS> MapSIDHitPosSeed0;
        std::map<Int_t, SIDHITPOS> MapSIDHitPosSeed1;

        std::list<SIDHIT>::const_iterator iter;
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


        std::map<Int_t, SIDHITPOS>::const_iterator iter0;
        std::map<Int_t, SIDHITPOS>::const_iterator iter1;
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
			
			m_pTrkSeeds.push_back(SIDHitSeed);			
			}
		}
	}


}


bool AFP_SIDBasicKalman::FillTrkPropagators(const SIDHITSEED &SIDHitSeed, Int_t plateF)
{
        // This sensoor is not active

        if( SIDHitSeed.nStationID == 2 && plateF==3) return 0;

        m_HID.clear();
	m_HID.push_back(SIDHitSeed.nHitID1);
	m_HID.push_back(SIDHitSeed.nHitID2);
	
	Float_t Zdist = (m_fzMapSID[SIDHitSeed.nStationID][plateF][1][0] - m_fzMapSID[SIDHitSeed.nStationID][plateF][0][0])/(m_fxMapSID[SIDHitSeed.nStationID][plateF][1][0] - m_fxMapSID[SIDHitSeed.nStationID][plateF][0][0])*(SIDHitSeed.fSeedX - m_fxMapSID[SIDHitSeed.nStationID][plateF][0][0]) + m_fzMapSID[SIDHitSeed.nStationID][plateF][0][0] - SIDHitSeed.fSeedZ;
	
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
	CLHEP::HepMatrix CiP = Fi*m_C0*Fi.T();//+m_Qk;
	
	//m_mk finder
	CLHEP::HepVector mi(2);
	Float_t XYdist=100.*CLHEP::mm;
	Int_t HitID=-1;
        std::map<Int_t, SIDHITPOS>::const_iterator iter;
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
	
	CLHEP::HepVector riP = mi - m_Hk*xiP;
	CLHEP::HepMatrix RiP = m_Vk + m_Hk*CiP*m_Hk.T();

	CLHEP::HepMatrix Ki = CiP*m_Hk.T()*qr_inverse(RiP);
	CLHEP::HepVector xi = xiP + Ki*riP;
	
	CLHEP::HepMatrix Ci = CiP - Ki*m_Hk*CiP;
	
	CLHEP::HepVector ri = mi - m_Hk*xi;
	CLHEP::HepMatrix Ri = m_Vk - m_Hk*Ki*m_Vk;
	
	//chi2 statistics to remove fakes
	CLHEP::HepVector chii = ri.T()*qr_inverse(Ri)*ri;

	//////////////////////////////
	if (chii[0]>MAXCHI2HIT) return 0; // no good hit in layer
	//////////////////////////////
	
	m_Fk.push_back(Fi);
	m_CkP.push_back(CiP);
	m_Ck.push_back(Ci);
	m_mk.push_back(mi);
	m_xk.push_back(xi);
	m_xkP.push_back(xiP);
	m_chik.push_back(chii[0]);
	
	m_m0 = m0i;
	m_x0 = x0i;	
	
	m_HID.push_back(HitID);
	m_zk.push_back( (m_fzMapSID[SIDHitSeed.nStationID][plateF][1][0] - m_fzMapSID[SIDHitSeed.nStationID][plateF][0][0])/(m_fxMapSID[SIDHitSeed.nStationID][plateF][1][0] - m_fxMapSID[SIDHitSeed.nStationID][plateF][0][0])*(xi[0] - m_fxMapSID[SIDHitSeed.nStationID][plateF][0][0]) + m_fzMapSID[SIDHitSeed.nStationID][plateF][0][0] );

	
	return 1;
}


bool AFP_SIDBasicKalman::FillTrkPropagators(Int_t stID, Int_t plateF)
{
	Float_t Zdist = (m_fzMapSID[stID][plateF][1][0] - m_fzMapSID[stID][plateF][0][0])/(m_fxMapSID[stID][plateF][1][0] - m_fxMapSID[stID][plateF][0][0])*(m_xk.back()[0] - m_fxMapSID[stID][plateF][0][0]) + m_fzMapSID[stID][plateF][0][0] - m_zk.back();
	
        // This sensor is not active
        if( stID == 2 && plateF == 3) return 0;

	CLHEP::HepMatrix Fi(4, 4, 1);
	Fi[0][1] = Zdist;
	Fi[2][3] = Zdist;
	
	CLHEP::HepVector xiP = Fi*m_xk.back();
	CLHEP::HepMatrix CiP = Fi*m_Ck.back()*Fi.T();//+m_Qk;
	
	//m_mk finder
	CLHEP::HepVector mi(2);
	Float_t XYdist=100.*CLHEP::mm;
	Int_t HitID=-1;
	std::map<Int_t, SIDHITPOS>::const_iterator iter;
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
	
	CLHEP::HepVector riP = mi - m_Hk*xiP;
	CLHEP::HepMatrix RiP = m_Vk + m_Hk*CiP*m_Hk.T();

	CLHEP::HepMatrix Ki = CiP*m_Hk.T()*qr_inverse(RiP);
	CLHEP::HepVector xi = xiP + Ki*riP;
	
	CLHEP::HepMatrix Ci = CiP - Ki*m_Hk*CiP;
	
	CLHEP::HepVector ri = mi - m_Hk*xi;
	CLHEP::HepMatrix Ri = m_Vk - m_Hk*Ki*m_Vk;
	
	//chi2 statistics to remove fakes
	CLHEP::HepVector chii = ri.T()*qr_inverse(Ri)*ri;
	

	//////////////////////////////
	if (chii[0]>MAXCHI2HIT) return 0; // no good hit in layer
	//////////////////////////////
	
	
	m_Fk.push_back(Fi);
	m_CkP.push_back(CiP);
	m_Ck.push_back(Ci);
	m_mk.push_back(mi);
	m_xk.push_back(xi);
	m_xkP.push_back(xiP);
	m_chik.push_back(chii[0]);
	
	m_HID.push_back(HitID);
	m_zk.push_back( (m_fzMapSID[stID][plateF][1][0] - m_fzMapSID[stID][plateF][0][0])/(m_fxMapSID[stID][plateF][1][0] - m_fxMapSID[stID][plateF][0][0])*(xi[0] - m_fxMapSID[stID][plateF][0][0]) + m_fzMapSID[stID][plateF][0][0] );

	return 1;
}


void AFP_SIDBasicKalman::Smooth()
{
	CLHEP::HepVector xiS;
	CLHEP::HepMatrix CiS;


        if( m_xk.size() > 1) {
        for (Int_t i=(static_cast<int>(m_xk.size())-2); i>=0; i--)
	{
                if(i==(static_cast<int>(m_xk.size())-2))
		{
			xiS = m_xk[m_xk.size()-1];	// !!!!!!!!!
			CiS = m_Ck[m_xk.size()-1];	// !!!!!!!!!
			
			m_xkS.push_back(xiS);
			m_CkS.push_back(CiS);
			CLHEP::HepVector riiS = m_mk[m_xk.size()-1] - m_Hk*xiS;
			CLHEP::HepMatrix RiiS = m_Vk - m_Hk*CiS*m_Hk.T();
			CLHEP::HepVector chiiS = riiS.T()*qr_inverse(RiiS)*riiS;
			m_chikS.push_back(chiiS[0]);
		}
		else
		{
			xiS = m_xkS.back();
			CiS = m_CkS.back();
		}
		
		//CLHEP::HepMatrix Ai = qr_inverse(m_Fk[i+1]); // when m_Qk == 0
		CLHEP::HepMatrix Ai = m_Ck[i]*m_Fk[i+1].T()*qr_inverse(m_CkP[i+1]);		
		CLHEP::HepVector xiiS = m_xk[i] + Ai*( xiS - m_xkP[i+1] );				
		CLHEP::HepMatrix CiiS = m_Ck[i] + Ai*( CiS - m_CkP[i+1] )*Ai.T();		
		
		CLHEP::HepVector riiS = m_mk[i] - m_Hk*xiiS;
		CLHEP::HepMatrix RiiS = m_Vk - m_Hk*CiiS*m_Hk.T();
		CLHEP::HepVector chiiS = riiS.T()*qr_inverse(RiiS)*riiS;
		
		m_xkS.push_back(xiiS);
		m_CkS.push_back(CiiS);
		m_chikS.push_back(chiiS[0]);
	}
        }
        else if (m_xk.size() > 0) {
                        xiS = m_xk[m_xk.size()-1];  // !!!!!!!!!
                        CiS = m_Ck[m_xk.size()-1];  // !!!!!!!!!

                        m_xkS.push_back(xiS);
                        m_CkS.push_back(CiS);
                        CLHEP::HepVector riiS = m_mk[m_xk.size()-1] - m_Hk*xiS;
                        CLHEP::HepMatrix RiiS = m_Vk - m_Hk*CiS*m_Hk.T();
                        CLHEP::HepVector chiiS = riiS.T()*qr_inverse(RiiS)*riiS;
                        m_chikS.push_back(chiiS[0]);


        }
        



	//CLHEP::HepMatrix Ai = qr_inverse(m_Fk[0]); // when m_Qk == 0
	CLHEP::HepMatrix Ai = m_C0*m_Fk[0].T()*qr_inverse(m_CkP[0]);

	CLHEP::HepVector xiiS = m_x0 + Ai*( m_xkS.back() - m_xkP[0] );		

	CLHEP::HepMatrix CiiS = m_C0 + Ai*( m_CkS.back() - m_CkP[0] )*Ai.T();
		
	CLHEP::HepVector riiS = m_m0 - m_Hk*xiiS;
	CLHEP::HepMatrix RiiS = m_Vk - m_Hk*CiiS*m_Hk.T();


	CLHEP::HepVector chiiS = riiS.T()*qr_inverse(RiiS)*riiS;
	
	m_CkS.push_back(CiiS);
	m_xkS.push_back(xiiS);
	m_chikS.push_back(chiiS[0]);

	

	// first hit !!!!!!!!!!
	CLHEP::HepMatrix Fi(4, 4, 1);
	Fi[0][1] = (*m_MapSIDHitPos.find(m_HID[1])).second.fPixZ - (*m_MapSIDHitPos.find(m_HID[0])).second.fPixZ;
	Fi[2][3] = (*m_MapSIDHitPos.find(m_HID[1])).second.fPixZ - (*m_MapSIDHitPos.find(m_HID[0])).second.fPixZ;
	
	Ai = qr_inverse(Fi); // when m_Qk == 0
	xiiS = Ai*( m_xkS.back() );			
	CiiS = m_C0 + Ai*( m_CkS.back() - m_C0 )*Ai.T();
		
	riiS = m_Hk*Ai*(m_x0 - m_xkS.back());
	RiiS = m_Vk - m_Hk*CiiS*m_Hk.T();
	chiiS = riiS.T()*qr_inverse(RiiS)*riiS;
	
	m_CkS.push_back(CiiS);
	m_xkS.push_back(xiiS);
	m_chikS.push_back(chiiS[0]);
	
	// z position of the seed hit0
	//Int_t StID = (*m_MapSIDHitPos.find(m_HID[0])).second.nStationID;
	//Int_t PlID = (*m_MapSIDHitPos.find(m_HID[0])).second.nPlateID;	
	//m_z0 = (m_fzMapSID[StID][PlID][1][0] - m_fzMapSID[StID][PlID][0][0])/(m_fxMapSID[StID][PlID][1][0] - m_fxMapSID[StID][PlID][0][0])*(xiiS[0] - m_fxMapSID[StID][PlID][0][0]) + m_fzMapSID[StID][PlID][0][0];
	m_z0 = (*m_MapSIDHitPos.find(m_HID[0])).second.fPixZ;
}

void AFP_SIDBasicKalman::FilterTrkCollection()
{
	//filtering tracking collection using shared hits + quality requirement
        std::list<SIDRESULT>::iterator iter1;
        std::list<SIDRESULT>::iterator iter2;
	for (iter1=m_listResults.begin(); iter1!=m_listResults.end(); iter1++)
	{
		for (iter2=m_listResults.begin(); iter2!=m_listResults.end(); )
		{
			if ( GetSharedHits((*iter1).ListHitID, (*iter2).ListHitID) > MAXSHAREDHITS && iter1!=iter2 )
			{
				if ( (*iter1).fChi2 < (*iter2).fChi2 )
				{
					//m_listResults.erase(iter1);
					//--iter1;
					++iter2;
				}
				else
				{
					iter2 = m_listResults.erase(iter2);
					//--iter2;
				}		
			}	
			else 	++iter2;
		}
	
	}
	
}

Int_t AFP_SIDBasicKalman::GetSharedHits(const std::vector<Int_t> &HID1, const std::vector<Int_t>  &HID2)
{
	Int_t SharedHits = 0;
	
        std::vector<Int_t>::const_iterator iter1;
        std::vector<Int_t>::const_iterator iter2;
	
	for (iter1=HID1.begin(); iter1!=HID1.end(); iter1++)
	{
		for (iter2=HID2.begin(); iter2!=HID2.end(); iter2++)
		{
			if ((*iter1) == (*iter2)) SharedHits++;
		}	
	}
	
	return SharedHits;
}

void AFP_SIDBasicKalman::ClearMatrix()
{
	m_Fk.clear();
	m_Ck.clear();
	m_CkP.clear();
	m_Rk.clear();

	m_xk.clear();
	m_xkP.clear();
	m_rk.clear();
	m_rkP.clear();
	m_mk.clear();
	m_chik.clear();	
	
	m_zk.clear();
	m_HID.clear();	
	
	m_xkS.clear();
	m_CkS.clear();
	m_chikS.clear();
	

}

void AFP_SIDBasicKalman::HistInitialize()
{
	//supporting histograms with binning convention: z, x
	m_MyFile = new TFile("PixMap.root","RECREATE");
	m_histS1_PixMap = new TH2F("histS1x_PixMap", "",
                                 600,
                                 m_fzMapSID[1][0][0][0]-5.,
                                 m_fzMapSID[1][SIDCNT-1][335][0]+5.,
                                 400,
                                 m_fxMapSID[1][SIDCNT-1][335][0]-2.,
                                 m_fxMapSID[1][0][0][0]+2.);
	m_histS2_PixMap = new TH2F("histS2x_PixMap", "",
                                 600,
                                 m_fzMapSID[0][0][0][0]-5.,
                                 m_fzMapSID[0][SIDCNT-1][335][0]+5.,
                                 400,
                                 m_fxMapSID[0][SIDCNT-1][335][0]-2.,
                                 m_fxMapSID[0][0][0][0]+2.);
	m_histS3_PixMap = new TH2F("histS1y_PixMap", "",
                                 600,
                                 m_fzMapSID[1][0][0][0]-5.,
                                 m_fzMapSID[1][SIDCNT-1][335][0]+5.,
                                 400,
                                 m_fyMapSID[1][0][0][79]-2.,
                                 m_fyMapSID[1][0][0][0]+2.);
	m_histS4_PixMap = new TH2F("histS2y_PixMap", "",
                                 600,
                                 m_fzMapSID[0][0][0][0]-5.,
                                 m_fzMapSID[0][SIDCNT-1][335][0]+5.,
                                 400,
                                 m_fyMapSID[0][0][0][79]-2.,
                                 m_fyMapSID[0][0][0][0]+2.);
}

void AFP_SIDBasicKalman::HistFinalize()
{
		
	m_MyFile->Write();
	//m_MyFile->Close();
	delete m_histS1_PixMap;
	delete m_histS2_PixMap;
	delete m_histS3_PixMap;
	delete m_histS4_PixMap;
	delete m_MyFile;
}


void AFP_SIDBasicKalman::GetData()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_SIDBasicKalman::GetData()");
	LogStream << MSG::DEBUG << "begin AFP_SIDBasicKalman::GetData()" << endmsg;

	/////
	/////

	LogStream << MSG::DEBUG << "end AFP_SIDBasicKalman::GetData()" << endmsg;
}
