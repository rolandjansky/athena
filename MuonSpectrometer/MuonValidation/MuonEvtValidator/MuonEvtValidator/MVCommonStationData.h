/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
// Name    : MVCommonDataDescription.h
// Package : MuonValidation
// Author  : M.Schott (LMU)
// CERN, MAY 1, 2006
//
// DESCRIPTION:
//
// This class describes one single MDT-Chamber and all operations that can be done within one chamber 
//
/////////////////////////////////////////////////////////////////////
 
#ifndef MVCOMMONSTATIONDATA_H
#define MVCOMMONSTATIONDATA_H

#include "AIDA/IHistogram1D.h"
#include <TF1.h>
#include <TH2.h>
#include <map>
#include "MuonEvtValidator/MVCommonStationID.h"
#include "MuonEvtValidator/MVCommonDataDescription.h"


class MVCommonStationData
{
	public:
		/// Default Constructor - Empty
		MVCommonStationData();

		/// Constructor for unique definition of MuonStation per Name,Eta and Phi
		MVCommonStationData(long StationName, long fStation_Eta, long fStation_Phi);

		/// Copy constructor
		MVCommonStationData(const MVCommonStationData& ob);
		MVCommonStationData& operator = (const MVCommonStationData& ob);

		void setStationInfo(long StationName, long fStation_Eta, long fStation_Phi);

		/** Setting the Datadesciption for one Event (only the Data which corresponds to this station)
		Version1: e.g. Digit-Information, Version2: e.g. Hit-Information*/
		void setVersion1Description(long EventNumber, MVCommonDataDescription &vtDes);
		void setVersion2Description(long EventNumber, MVCommonDataDescription &vtDes);

		/** Calculates how many Digits or Hits can be identified with each other*/
		MVCommonResiduumResults getMeanDifference(int minDifference)
			{ return getMeanDifference(	m_EventNumber_StationVersion1Events,
							m_EventNumber_StationVersion2Events,
							minDifference);}

		/** Calculates the Mean Number of Hits or Digits in this chamber per Event of
		the first inputfile*/
		float getMeanNumberOfVersion1(long &pass, long &total)
			{ return getMeanNumber(m_EventNumber_StationVersion1Events, pass, total);}

		/** Calculates the Mean Number of Hits or Digits in this chamber per Event of
		the second inputfile*/
		float getMeanNumberOfVersion2(long &pass, long &total)
			{ return getMeanNumber(m_EventNumber_StationVersion2Events, pass, total);}

		void fillDataInHistoGramm(IHistogram1D* Histo, int HistoType);

		/// Gives back the Station Name as String
		std::string getMuonStationName();

	private:

		/// Checks if event with ID exists already in tmpStationEvents
		bool eventExists(long id, std::map<long, MVCommonDataDescription> &tmpStationEvents);

		std::string getMuonStationName(long jtyp);

		/** Calculates the Mean Number of Hits or Digits in this chamber per Event of tmpStationEvents*/
		float getMeanNumber(
			std::map<long, MVCommonDataDescription> &tmpStationEvents, long &pass, long &total);

		/** Calculates how many Digits or Hits can be identified with each other*/
		MVCommonResiduumResults getMeanDifference(
					std::map<long, MVCommonDataDescription> &tmpStationEvents1,
					std::map<long, MVCommonDataDescription> &tmpStationEvents2,
					int minDifference);

		char getChar(std::string input, int i);

		/** Data Description of first and second inputfile - Map-ID is equivalent to Run-Number*/
		std::map<long, MVCommonDataDescription> m_EventNumber_StationVersion1Events;
		std::map<long, MVCommonDataDescription> m_EventNumber_StationVersion2Events;

		/// Unique Station Identifiers
		long m_Station_Eta;
		long m_Station_Phi;
		long  m_StationName;
		/// Chamber-Positions
		float Pos_X[8];
		float Pos_Y[8];
		float Pos_Z[8];
};

#endif
