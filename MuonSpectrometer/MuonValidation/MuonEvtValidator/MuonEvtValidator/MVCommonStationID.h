/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
// Name    : MVCommonDataDescription.h
// Package : MuonValidation
// Authors: M.Schott (LMU) - N. Benekos (MPI)
// CERN, MAY 1, 2006
//
// DESCRIPTION:
//
// This class generates an unique ID-number for a Detectorelement from its ID, Eta and Phi value 
//
/////////////////////////////////////////////////////////////////////
 

#ifndef MVCOMMONSTATIONID_H
#define MVCOMMONSTATIONID_H

#include <vector>
#include <string>

using namespace std;


/// This Structure describes the result of comparing Hits and Digits for One Chamber and One Event 
struct MVCommonResiduumResults
{
	float CommonOverTotal1; /// Percentage of (Number of Common Hits)/(Number of Total Hits+Digits)
	float CommonOverTotal2; /// Percentage of (Number of Common Digits)/(Number of Total Hits+Digits)
	long  CommonHits;
	long  TotalHits1;
	long  TotalHits2;
	vector<float> vResZ0;
	vector<float> vResRadius;
};

/// This class generates an unique ID-number for a Detectorelement from its ID, Eta and Phi value 
class MVCommonStationID
{
	public:

		MVCommonStationID();
		MVCommonStationID(std::string type);
		MVCommonStationID(const MVCommonStationID& ob);
	
		MVCommonStationID& operator = (const MVCommonStationID& ob);
		bool operator == (const MVCommonStationID& ob);

		long getMaxIDNumber();	/// Gives the Maximal Possible MVCommon-ID-Number
		long getLongIDNumber();	/// Gives back the Unique ID-Number of this Station

		/** Gives back the Unique ID-Number corresponding to the given values
		Note: The InternalName is NOT necessary equivalent to the Station NameID
		The two values could be converted via convertFromCommonStationNameNumber
		*/
		long getLongIDNumber(int InternalName, int Eta, int Phi);

		/// Calculates the NameID, the ETA and Phi Value of the given Unique-ID
		void getNameIDEtaPhi(long LongIdNumber, int &InternalName, int &Eta, int &Phi);


		/// These function give back the maximal and minimal Value of StationName, Eta and Phi
		/// This differs for MDTs, RPCs, TGC, CSCs
		int 		getMaxStationName();
		int 		getMinStationName();
		int 		getMaxStationEta();
		int 		getMinStationEta();
		int 		getMaxStationPhi();
		int 		getMinStationPhi();

		/// Converts the usual StationNameID to internal used StationNameID
		//int 		convertFromCommonStationNameNumber(int CommonStationNameID);
		/// Converts the internal used StationNameID to the usual StationNameID
		//int 		convertToCommonStationNameNumber(int InternStationNameID);

		/// Gives back the StationName as String
		std::string 	getStationName(int CommonStationID);
		std::string 	getStationName();
		int 		getStationName(std::string StationName);

		std::string m_type;

		int 	m_StationName;
		int 	m_StationEta;
		int 	m_StationPhi;
};

#endif
