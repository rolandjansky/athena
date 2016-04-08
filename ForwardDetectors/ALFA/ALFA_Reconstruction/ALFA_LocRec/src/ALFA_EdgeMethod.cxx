/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRec/ALFA_EdgeMethod.h"
using namespace std;

ALFA_EdgeMethod::ALFA_EdgeMethod()
{
	m_bOpt_Sisters = kFALSE;
	m_bOpt_UseGaps = kFALSE;
}

ALFA_EdgeMethod::ALFA_EdgeMethod(Bool_t bOpt_Sisters, Bool_t bOpt_UseGaps)
{
	m_bOpt_Sisters = bOpt_Sisters;
	m_bOpt_UseGaps = bOpt_UseGaps;
}

ALFA_EdgeMethod::~ALFA_EdgeMethod()
{

}

void ALFA_EdgeMethod::Initialize(Int_t iRPot, Float_t faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Float_t fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], const std::list<MDHIT> &ListMDHits)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_MDTracking::Initialize()");
	ATH_MSG_DEBUG("begin ALFA_EdgeMethod::Initialize()");

	for (Int_t iPot = 0; iPot < RPOTSCNT; iPot++)
	{
		for (Int_t iLayer = 0; iLayer < ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
		{
			for (Int_t iFiber = 0; iFiber < ALFAFIBERSCNT; iFiber++)
			{
				m_faMD[iPot][iLayer][iFiber] = faMD[iPot][iLayer][iFiber];
				m_fbMD[iPot][iLayer][iFiber] = fbMD[iPot][iLayer][iFiber];
			}
		}
	}

	memset(&m_bFiberHitsMD, 0, sizeof(m_bFiberHitsMD));
	memset(&m_iMultiMD, 0, sizeof(m_iMultiMD));
	std::list<MDHIT>::const_iterator iter;
	for (iter=ListMDHits.begin(); iter!=ListMDHits.end(); iter++)
	{
		if (iRPot == (*iter).iRPot)
		{
			m_bFiberHitsMD[iRPot][(*iter).iPlate][(*iter).iFiber] = true;
			m_iMultiMD[iRPot][(*iter).iPlate]++;
		}
	}


	ATH_MSG_DEBUG("end ALFA_EdgeMethod::Initialize()");
}

//void SetOptions(TString options){
//	if(options.CountChar('S') + options.CountChar('s') > 0 )
//		m_bOpt_Sisters = kTRUE;
//	else
//		m_bOpt_Sisters = kFALSE;


//	if(options.CountChar('G') + options.CountChar('g') > 0 )
//		m_bOpt_UseGaps = kTRUE;
//	else
//		m_bOpt_UseGaps = kFALSE;
//}

Bool_t ALFA_EdgeMethod::functionSortEdges( Edge edg1, Edge edg2 ){
//	std::cout << "begin ALFA_EdgeMethod::functionSortEdges()" << std::endl;
	return ( edg1.first.first < edg2.first.first );
}

Bool_t ALFA_EdgeMethod::functionSortCorrsOne( Corridor corr1, Corridor corr2 ){
//	std::cout << "begin ALFA_EdgeMethod::functionSortCorrsOne()" << std::endl;
	if( corr1.second == corr2.second ) return ( corr1.first.second  > corr2.first.second );
	else return  ( corr1.second > corr2.second );
}

Bool_t ALFA_EdgeMethod::functionSortTracks( Track track1, Track track2 ){
//	std::cout << "begin ALFA_EdgeMethod::functionSortTracks()" << std::endl;
	if( track1.first.second + track1.second.second == track2.first.second + track2.second.second )
		return track1.first.first.second + track1.second.first.second > track2.first.first.second + track2.second.first.second;
	else return  track1.first.second + track1.second.second > track2.first.second + track2.second.second;
}

void ALFA_EdgeMethod::findEdges( UInt_t no_Detector, UInt_t no_Orient, std::vector< Edge > &edges)
{
	ATH_MSG_DEBUG("begin ALFA_EdgeMethod::findEdges()");

	edges.clear();
	UInt_t left;

	for(UInt_t nL=no_Orient; nL<20; nL+=2){
		for(UInt_t nF=0; nF<64; nF++){
			if(m_bFiberHitsMD[no_Detector][nL][nF]){
				left = nF;
				while( nF!=63 && m_bFiberHitsMD[no_Detector][nL][nF+1]){nF++;}						/// WITHOUT OR WITH ADJACENT

				if( m_iMultiMD[no_Detector][nL] > 3 ){
					edges.push_back( make_pair( make_pair( m_uv_geo[no_Detector][nL][left] - 0.240, kTRUE), kFALSE));
					edges.push_back( make_pair( make_pair( m_uv_geo[no_Detector][nL][nF] + 0.240, kFALSE), kFALSE));
				} else {
					edges.push_back( make_pair( make_pair( m_uv_geo[no_Detector][nL][left] - 0.240, kTRUE), kTRUE));
					edges.push_back( make_pair( make_pair( m_uv_geo[no_Detector][nL][nF] + 0.240, kFALSE), kTRUE));
				}
			}
		}
	}
}

void ALFA_EdgeMethod::findCorridors(std::vector< Edge > &edges, std::vector< Corridor > &corridors)
{
	ATH_MSG_DEBUG("begin ALFA_EdgeMethod::findCorridors()");

	corridors.clear();

	Int_t level = 0;
	Int_t alive = 0;
	Bool_t max = kFALSE;
	Float_t leftEd = 9999.0;


	for(UInt_t i = 0; i< (edges.size()-1); i++){
		if( edges.at(i).first.second ){
			level++;
			if( edges.at(i).second ){
				alive++;
			}
			if( edges.at(i+1).first.second ){
				continue;
			} else {
				if( alive < 3 || max ){
					continue;
				} else {
					leftEd = edges.at(i).first.first;
					max = kTRUE;
				}
			}
		} else {
			level--;
			if( edges.at(i).second ){
				alive--;
			}
			if( max ){
				if( edges.at(i+1).first.second ){
					if( edges.at(i+2).first.second ){
						corridors.push_back( make_pair( make_pair( 0.5*(leftEd + edges.at(i).first.first), edges.at(i).first.first - leftEd), level+1) );
						max = kFALSE;
					}
					continue;
				} else {
					corridors.push_back( make_pair( make_pair( 0.5*(leftEd + edges.at(i).first.first), edges.at(i).first.first - leftEd), level+1) );
					max = kFALSE;
				}
			}
		}
	}

	if( !corridors.empty() ){
		for(UInt_t i = 0; i < corridors.size()-1; i++){
			if( TMath::Abs( corridors.at(i).first.first - corridors.at(i+1).first.first ) < 0.480){
				if( corridors.at(i).second > corridors.at(i+1).second ){
					corridors.erase(corridors.begin()+i+1);
					i--;
				} else if ( corridors.at(i).second < corridors.at(i+1).second ){
					corridors.erase(corridors.begin()+i);
					i--;
				}
			}
		}
	}

}

Bool_t ALFA_EdgeMethod::testTrack(/*Corridor corr_U, Corridor corr_V*/)
{
	ATH_MSG_DEBUG("begin ALFA_EdgeMethod::testTrack()");

	// Place for other conditions, eg. track position
//	if( 0.5*TMath::Sqrt2()*(-corr_U.first.first+corr_V.first.first)  < -136 ) return kFALSE;

	return kTRUE;
}

Bool_t ALFA_EdgeMethod::iterOne(UInt_t no_Detector, UInt_t no_Orient, std::vector< Corridor > &corridors )
{
	ATH_MSG_DEBUG("begin ALFA_EdgeMethod::iterOne()");

	std::vector< Edge > edges;

	findEdges( no_Detector, no_Orient, edges);
	if( edges.empty() ) return kFALSE;
	sort( edges.begin(), edges.end(), functionSortEdges );

	findCorridors(edges, corridors);
	if( corridors.empty() )
		return kFALSE;

//	sort( corridors.begin(), corridors.end(), functionSortCorrsOne );
//	if( corridors.size() > 10 ){
//		corridors.resize(10);
//	}

	if(m_bOpt_Sisters){
		// Cut for sisters
		// Other corridors must have more than half of first corridor hits
		UInt_t minHits = 0.5*corridors.front().second;
		UInt_t maxCorr = 1;
		while( maxCorr < corridors.size() && minHits < corridors.at(maxCorr).second ){
			maxCorr++;
		}
		if( maxCorr != corridors.size() ){
			corridors.resize( maxCorr );
		}
	}

	sort( corridors.begin(), corridors.end(), functionSortCorrsOne );
	if( corridors.size() > 5 ){
		corridors.resize(5);
	}
	return kTRUE;
}


Bool_t ALFA_EdgeMethod::iterationOne(UInt_t no_Detector, std::vector<Track> &tracks )
{
	ATH_MSG_DEBUG("begin ALFA_EdgeMethod::iterationOne()");

	tracks.clear();
	readUVONE(no_Detector);

	std::vector< Corridor > corr_U;
	std::vector< Corridor > corr_V;
	if( !iterOne(no_Detector, 0, corr_U) || !iterOne(no_Detector, 1, corr_V) ) return kFALSE;

	for(UInt_t i = 0; i < corr_U.size(); i++){
		for(UInt_t j = 0; j < corr_V.size(); j++){
			if( testTrack( /*corr_U.at(i), corr_V.at(j)*/ ) ){
				tracks.push_back(make_pair( corr_U.at(i), corr_V.at(j) ));
			}
		}
	}

	if( tracks.empty() ) return kFALSE;
	sort( tracks.begin(), tracks.end(), functionSortTracks );
	if( tracks.size() > 10 ){
		tracks.resize(10);
	}
	return kTRUE;
}

Bool_t ALFA_EdgeMethod::iterNext(UInt_t no_Detector, UInt_t no_Orient, Float_t pos, Int_t level, Corridor &corr)
{
	ATH_MSG_DEBUG("begin ALFA_EdgeMethod::iterNext()");

	std::vector< Edge > edges;
	findEdges( no_Detector, no_Orient, edges);
	sort( edges.begin(), edges.end(), functionSortEdges );

	std::vector< Corridor > corridors;
	findCorridors(edges, corridors);
	if( corridors.empty() ) return kFALSE;

	sort( corridors.begin(), corridors.end(), CSortCorrsNext(pos));
	if( TMath::Abs( pos - corridors.front().first.first ) > 0.480 || TMath::Abs( level - (Int_t)corridors.front().second ) > 1 ) return kFALSE;

	corr = corridors.front();
	return kTRUE;
}

Bool_t ALFA_EdgeMethod::iterationNext(UInt_t no_Detector, std::vector<Track> &tracks )
{
	ATH_MSG_DEBUG("begin ALFA_EdgeMethod::iterationNext()");

	Corridor corr_U;
	Corridor corr_V;
	Bool_t rem;

	for(UInt_t i = 0; i < tracks.size(); i++){
		readUVONE(no_Detector, tracks.at(i).first.first.first, tracks.at(i).second.first.first );
		rem = iterNext(no_Detector, 0, tracks.at(i).first.first.first, tracks.at(i).first.second, corr_U) && iterNext(no_Detector, 1, tracks.at(i).second.first.first, tracks.at(i).second.second, corr_V);
		if( rem && testTrack(/*corr_U, corr_V*/) ){
			tracks.at(i) = make_pair( corr_U, corr_V );
		} else {
			tracks.erase(tracks.begin()+i);
			i--;
		}
	}

	if( tracks.empty() ) return kFALSE;
	return kTRUE;
}

Bool_t ALFA_EdgeMethod::EdgeMethod(UInt_t no_Detector, std::vector<Track> &tracks)
{
	ATH_MSG_DEBUG("begin ALFA_EdgeMethod::EdgeMethod()");

	if(!iterationOne(no_Detector, tracks )) return kFALSE;
	if(!iterationNext(no_Detector, tracks )) return kFALSE;
	if(!iterationNext(no_Detector, tracks )) return kFALSE;

	for(UInt_t i = 0; i < tracks.size(); i++){
		for(UInt_t j = i+1; j < tracks.size(); j++){
			if( TMath::Abs( tracks.at(i).first.first.first - tracks.at(j).first.first.first) < 0.002 && TMath::Abs( tracks.at(i).second.first.first - tracks.at(j).second.first.first ) < 0.002 ){
				tracks.erase( tracks.begin() + j );
				j--;
			}
		}
	}

	ATH_MSG_DEBUG("end ALFA_EdgeMethod::EdgeMethod()");
	return kTRUE;
}


void ALFA_EdgeMethod::selectedFibers(UInt_t no_Detector, Track &track, Int_t * selectedFib)
{
	ATH_MSG_DEBUG("begin ALFA_EdgeMethod::selectedFibers()");

	readUVONE(no_Detector, track.first.first.first, track.second.first.first );

	UInt_t nF = 0;
	for(UInt_t nL = 0; nL < 20; nL+=2){
		selectedFib[nL] = 9999;
		while( nF != 63   &&   (m_uv_geo[no_Detector][nL][nF] + 0.2401) <=  (track.first.first.first + 0.5 * track.first.first.second) ){
			nF++;
		}

		if( (m_uv_geo[no_Detector][nL][nF]-0.2401) <= track.first.first.first - 0.5 * track.first.first.second ){
			if( m_bFiberHitsMD[no_Detector][nL][nF] ){
				selectedFib[nL] = nF;
			} else {
				selectedFib[nL] = nF + 9000;
			}
		} else {
			if( nF != 0 && m_bFiberHitsMD[no_Detector][nL][nF] && m_bFiberHitsMD[no_Detector][nL][nF-1] ){
				selectedFib[nL] = nF + 2999;
			} else if( (m_uv_geo[no_Detector][nL][nF]-0.2401) <= track.first.first.first + 0.5 * track.first.first.second && m_bFiberHitsMD[no_Detector][nL][nF] ){
				selectedFib[nL] = nF + 2000;
			} else if ( nF != 0 && (m_uv_geo[no_Detector][nL][nF-1]+0.2401) <= track.first.first.first - 0.5 * track.first.first.second && m_bFiberHitsMD[no_Detector][nL][nF-1] ){
				selectedFib[nL] = nF + 999;
			} else {
				selectedFib[nL] = nF + 9000;
			}
		}

		nF > 4 ? nF -= 5 : nF = 0;
	}

	nF = 0;
	for(UInt_t nL = 1; nL < 20; nL+=2){
		selectedFib[nL] = 9999;
		while( nF != 63   &&   (m_uv_geo[no_Detector][nL][nF] + 0.2401) <=  track.second.first.first + 0.5 * track.second.first.second ){
			nF++;
		}

		if( (m_uv_geo[no_Detector][nL][nF]-0.2401) <= track.second.first.first - 0.5 * track.second.first.second ){
			if( m_bFiberHitsMD[no_Detector][nL][nF] ){
				selectedFib[nL] = nF;
			} else {
				selectedFib[nL] = nF + 9000;
			}
		} else {
			if( nF != 0 && m_bFiberHitsMD[no_Detector][nL][nF] && m_bFiberHitsMD[no_Detector][nL][nF-1] ){
				selectedFib[nL] = nF + 2999;
			} else if( (m_uv_geo[no_Detector][nL][nF]-0.2401) <= track.second.first.first + 0.5 * track.second.first.second && m_bFiberHitsMD[no_Detector][nL][nF] ){
				selectedFib[nL] = nF + 2000;
			} else if ( nF != 0 && (m_uv_geo[no_Detector][nL][nF-1]+0.2401) <= track.second.first.first - 0.5 * track.second.first.second && m_bFiberHitsMD[no_Detector][nL][nF-1] ){
				selectedFib[nL] = nF + 999;
			} else {
				selectedFib[nL] = nF + 9000;
			}
		}

		nF > 4 ? nF -= 5 : nF = 0;
	}

	if(m_bOpt_UseGaps){
		Int_t fLow, fCur;
		Float_t fLeft, fRight;

		for(UInt_t nL = 0; nL < 16; nL+=2){
			fLow = selectedFib[nL]%1000;
//			if( fLow > 0 && fLow < 63 && selectedFib[nL] < 8000 && selectedFib[nL+2] > 8000 && selectedFib[nL+4] < 8000  ){
			if( fLow > 1 && fLow < 62 && selectedFib[nL] < 8000 && selectedFib[nL+2] > 8000 && selectedFib[nL+4] < 8000  ){
				if( m_uv_geo[no_Detector][nL][fLow] > m_uv_geo[no_Detector][nL+2][fLow] ){
					if( m_uv_geo[no_Detector][nL][fLow] > m_uv_geo[no_Detector][nL+2][fLow+1] ){
						fCur = fLow+1;
					} else {
						fCur = fLow;
					}
				} else {
					if( m_uv_geo[no_Detector][nL][fLow] > m_uv_geo[no_Detector][nL+2][fLow-1] ){
						fCur = fLow-1;
					} else {
						fCur = fLow-2;
					}
				}
				fLeft = m_uv_geo[no_Detector][nL+2][fCur] + 0.2399;
				fRight = m_uv_geo[no_Detector][nL+2][fCur+1] - 0.2401;

				if( fLeft < fRight ){

					if( fLeft >  track.first.first.first - 0.5 * track.first.first.second  && fLeft < track.first.first.first + 0.5 * track.first.first.second ){
						track.first.first.first = 0.5 * ( track.first.first.first + 0.5 * track.first.first.second + fLeft );
						track.first.first.second = 2 * ( track.first.first.first - fLeft );
					}

					if( fRight >  track.first.first.first - 0.5 * track.first.first.second  && fRight < track.first.first.first + 0.5 * track.first.first.second ){
						track.first.first.first = 0.5 * ( track.first.first.first - 0.5 * track.first.first.second + fRight );
						track.first.first.second = 2 * ( fRight - track.first.first.first );
					}
				}
				nL += 2;
			}
		}

		for(UInt_t nL = 1; nL < 17; nL+=2){
			fLow = selectedFib[nL]%1000;
//			if( fLow > 0 && fLow < 63 && selectedFib[nL] < 8000 && selectedFib[nL+2] > 8000 && selectedFib[nL+4] < 8000  ){
			if( fLow > 1 && fLow < 62 && selectedFib[nL] < 8000 && selectedFib[nL+2] > 8000 && selectedFib[nL+4] < 8000  ){
				if( m_uv_geo[no_Detector][nL][fLow] > m_uv_geo[no_Detector][nL+2][fLow] ){
					if( m_uv_geo[no_Detector][nL][fLow] > m_uv_geo[no_Detector][nL+2][fLow+1] ){
						fCur = fLow+1;
					} else {
						fCur = fLow;
					}
				} else {
					if( m_uv_geo[no_Detector][nL][fLow] > m_uv_geo[no_Detector][nL+2][fLow-1] ){
						fCur = fLow-1;
					} else {
						fCur = fLow-2;
					}
				}
				fLeft = m_uv_geo[no_Detector][nL+2][fCur] + 0.2399;
				fRight = m_uv_geo[no_Detector][nL+2][fCur+1] - 0.2401;
				if( fLeft < fRight ){

					if( fLeft >  track.second.first.first - 0.5 * track.second.first.second  && fLeft < track.second.first.first + 0.5 * track.second.first.second ){
						track.second.first.first = 0.5 * ( track.second.first.first + 0.5 * track.second.first.second + fLeft );
						track.second.first.second = 2 * ( track.second.first.first - fLeft );
					}

					if( fRight >  track.second.first.first - 0.5 * track.second.first.second  && fRight < track.second.first.first + 0.5 * track.second.first.second ){
						track.second.first.first = 0.5 * ( track.second.first.first - 0.5 * track.second.first.second + fRight );
						track.second.first.second = 2 * ( fRight - track.second.first.first );
					}
				}

				nL += 2;
			}
		}
	}
}




//Read fibers positions in u&v for pot no_Detector (usig u_pos and v_pos for correction)
void ALFA_EdgeMethod::readUVONE(Int_t no_Detector, Double_t u_pos, Double_t v_pos)
{
	ATH_MSG_DEBUG("begin ALFA_EdgeMethod::readUVONE()");

	for(Int_t no_Layer=0; no_Layer<20; no_Layer++){
		if(m_faMD[no_Detector][no_Layer][0]<0){
			for(Int_t no_Fiber=0; no_Fiber<64; no_Fiber++){
				m_uv_geo[no_Detector][no_Layer][no_Fiber]=(v_pos*(1+m_faMD[no_Detector][no_Layer][no_Fiber])-1.414213562373095*m_fbMD[no_Detector][no_Layer][no_Fiber])/(1-m_faMD[no_Detector][no_Layer][no_Fiber]);
			}
		} else {
			for(Int_t no_Fiber=0; no_Fiber<64; no_Fiber++){
				m_uv_geo[no_Detector][no_Layer][no_Fiber]=(u_pos*(1-m_faMD[no_Detector][no_Layer][no_Fiber])+1.414213562373095*m_fbMD[no_Detector][no_Layer][no_Fiber])/(1+m_faMD[no_Detector][no_Layer][no_Fiber]);
			}
		}
	}
}



CSortCorrsNext::CSortCorrsNext(const Float_t fPosTr)
	: m_fPosTr(fPosTr)
{

}

bool CSortCorrsNext::operator()( ALFA_EdgeMethod::Corridor corr1, ALFA_EdgeMethod::Corridor corr2 ) const
{
	return (TMath::Abs( corr1.first.first - m_fPosTr ) < TMath::Abs( corr2.first.first - m_fPosTr ));
}
