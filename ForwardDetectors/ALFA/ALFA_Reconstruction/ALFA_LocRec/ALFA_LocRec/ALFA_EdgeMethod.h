/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_EDGEMETHOD_H
#define ALFA_EDGEMETHOD_H


//#include "ReadGeometry.h"
//#include "FullActivateTree.h"

#include <utility>
#include <algorithm>
#include <vector>

#include "Rtypes.h"
#include "TMath.h"


#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

//#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/getMessageSvc.h"

#include "ALFA_LocRec/ALFA_UserObjects.h"
#include "ALFA_Geometry/ALFA_constants.h"


class ALFA_EdgeMethod
{
public:
	ALFA_EdgeMethod();
	ALFA_EdgeMethod(Bool_t bOpt_Sisters, Bool_t bOpt_UseGaps);
	~ALFA_EdgeMethod();

	void Initialize(Int_t iRPot, Float_t faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Float_t fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], const std::list<MDHIT> &ListMDHits);


	// Typedef for Edge, Corridor and Track
	typedef std::pair< std::pair< Float_t, Bool_t >, Bool_t > Edge; 								// typedef for fibers edges: <edge position, fiber side>			(left edge -> kTRUE; right edge -> kFALSE)
	typedef std::pair< std::pair< Float_t, Float_t >, UInt_t > Corridor; 			// typedef for corridors:    <<position, width>, numbers of hits>
	typedef std::pair< Corridor, Corridor > Track; 							// typedef for tracks:       <corridors in u, corridors in v>


//	void SetOptions(TString options = "");

private:
	// Sorting function:
	// - sort edges according to positions
	static Bool_t functionSortEdges( Edge edg1, Edge edg2 );
	// - sort corridors according to number of hits, if equal: according to corridor width (used in first iteration)
	static Bool_t functionSortCorrsOne( Corridor corr1, Corridor corr2 );
	// - sort tracks according to number of hits
	static Bool_t functionSortTracks( Track track1, Track track2 );

	// Find all edges of hit fibers and put them in vector &edges
	void findEdges(UInt_t no_Detector, UInt_t no_Orient, std::vector< Edge > &edges);
	// Find all possible corridors from finded &edges and put them in vector &corridors
	void findCorridors(std::vector< Edge > &edges, std::vector< Corridor > &corridors);

	// Test if corridors can merge into one track
	Bool_t testTrack(/*Corridor corr_U, Corridor corr_V*/);


	// First iteration
	// Look for corridors (in one orientation) and choose the best 5 using sorting @functionSortCorrsOne
	Bool_t iterOne(UInt_t no_Detector, UInt_t no_Orient, std::vector< Corridor > &corridors );
	// Execute @iterOne for both orientation, merge coridors in tracks, test them and choose the best 10 tracks
	Bool_t iterationOne(UInt_t no_Detector, std::vector<Track> &tracks );

	// Second and third iteration
	// Look for corridors (in one orientation) and choose the closest to previous using sorting @functionSortCorrsNext
	Bool_t iterNext(UInt_t no_Detector, UInt_t no_Orient, Float_t pos, Int_t level, Corridor &corr);
	// Execute @iterNext for both orientation, merge coridors in tracks and test them
	Bool_t iterationNext(UInt_t no_Detector, std::vector<Track> &tracks );


public:
	// Local reconstruction: execute @iterationOne, and twice @iterationNext
	// Reconstruct the tracks and storage them in &tracks
	Bool_t EdgeMethod(UInt_t no_Detector, std::vector<Track> &tracks);


	// Additional function:
	// - fill table Fib_SelMD (select the fibers with contain both coridors edge, not only positions)
	void selectedFibers(UInt_t no_Detector, Track &track, Int_t * selectedFib);


private:
	void readUVONE(Int_t no_Detector, Double_t u_pos = 91.0, Double_t v_pos = -91.0);




public:
	/// Log a message using the Athena controlled logging system
	MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
	/// Check whether the logging system is active at the provided verbosity level
	bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
private:
	/// Private message stream member
	mutable Athena::MsgStreamMember m_msg;

	Float_t m_faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];	//slope, offset and Z-pos for MD fibers [2*10][64]
	Float_t m_fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];
	Bool_t m_bFiberHitsMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];
	Int_t m_iMultiMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT];
	//Geometry in u&v
	Double_t m_uv_geo[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];

	Bool_t m_bOpt_Sisters;
	Bool_t m_bOpt_UseGaps;

};

class CSortCorrsNext
{
private:
	Float_t m_fPosTr;
public:
	CSortCorrsNext(const Float_t fPosTr);
	bool operator()( ALFA_EdgeMethod::Corridor corr1, ALFA_EdgeMethod::Corridor corr2 ) const;
};

#endif // ALFA_EDGEMETHOD_H
