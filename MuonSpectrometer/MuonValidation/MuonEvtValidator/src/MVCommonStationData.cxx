/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Name    : MVCommonDataDescription.cxx
// Package : MuonValidation
// Author:   M.Schott (LMU)
// CERN, MAY 1, 2006
//
// DESCRIPTION:
//
// This class describes one single MDT-Chamber and all operations that can be done within one chamber 
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
 

#include "MuonEvtValidator/MVCommonStationData.h"

/// Default Constructor - Empty
MVCommonStationData::MVCommonStationData():
 m_Station_Eta(0),
 m_Station_Phi(0),
 m_StationName(0)

{
	m_EventNumber_StationVersion1Events.clear();
	m_EventNumber_StationVersion2Events.clear();
        for(int i=0;i<8;i++)
        {        
                Pos_X[i]=0.; 
                Pos_Y[i]=0.;
                Pos_Z[i]=0.; 
        }      


}

/// Constructor for unique definition of MuonStation per Name,Eta and Phi
MVCommonStationData::MVCommonStationData(long StationName, long fStation_Eta, long fStation_Phi)
{
	m_EventNumber_StationVersion1Events.clear();
	m_EventNumber_StationVersion2Events.clear();
	setStationInfo(StationName, fStation_Eta, fStation_Phi);
        for(int i=0;i<8;i++)
        {        
                Pos_X[i]=0.; 
                Pos_Y[i]=0.;
                Pos_Z[i]=0.; 
        }      
}

/// Copy constructor
MVCommonStationData::MVCommonStationData::MVCommonStationData(const MVCommonStationData& ob)
{
	m_EventNumber_StationVersion1Events = ob.m_EventNumber_StationVersion1Events;
	m_EventNumber_StationVersion2Events = ob.m_EventNumber_StationVersion2Events;
	m_Station_Eta 		= ob.m_Station_Eta;
	m_Station_Phi 		= ob.m_Station_Phi;
	m_StationName 		= ob.m_StationName;
	for (int i=0; i<8; i++)
	{
		Pos_X[i] = ob.Pos_X[i];
		Pos_Y[i] = ob.Pos_Y[i];
		Pos_Z[i] = ob.Pos_Z[i];
	}
}

MVCommonStationData& MVCommonStationData::operator = (const MVCommonStationData& ob)
{
	if (this==&ob) return *this; //Check self-assignment
        m_EventNumber_StationVersion1Events = ob.m_EventNumber_StationVersion1Events;
	m_EventNumber_StationVersion2Events = ob.m_EventNumber_StationVersion2Events;
	m_Station_Eta 		= ob.m_Station_Eta;
	m_Station_Phi 		= ob.m_Station_Phi;
	m_StationName 		= ob.m_StationName;
	for (int i=0; i<8; i++)
	{
		Pos_X[i] = ob.Pos_X[i];
		Pos_Y[i] = ob.Pos_Y[i];
		Pos_Z[i] = ob.Pos_Z[i];
	}

	return *this;
}

void MVCommonStationData::setStationInfo(long StationName, long fStation_Eta, long fStation_Phi)
{
	m_Station_Eta = fStation_Eta;
	m_Station_Phi = fStation_Phi;
	m_StationName  = StationName;
}

/** Setting the Datadesciption for one Event (only the Data which corresponds to this station)
Version1: e.g. Digit-Information, Version2: e.g. Hit-Information*/
void MVCommonStationData::setVersion1Description(long EventNumber, MVCommonDataDescription &vtDes)
{
	m_EventNumber_StationVersion1Events[EventNumber]=vtDes;
	return;
}

/** Setting the Datadesciption for one Event (only the Data which corresponds to this station)
Version1: e.g. Digit-Information, Version2: e.g. Hit-Information*/
void MVCommonStationData::setVersion2Description(long EventNumber, MVCommonDataDescription &vtDes)
{
	m_EventNumber_StationVersion2Events[EventNumber]=vtDes;
	return;
}

std::string MVCommonStationData::getMuonStationName()
{
	return getMuonStationName(m_StationName);
//	return "";
}

std::string MVCommonStationData::getMuonStationName(long jtyp)
{
	int __attribute__((__unused__)) useless = jtyp;// useless=1; //making the compiler happy
	return "";
}

/** Calculates the Mean Number of Hits or Digits in this chamber per Event of tmpStationEvents*/
float MVCommonStationData::getMeanNumber(	map<long, MVCommonDataDescription> &tmpStationEvents,
						long &pass, long &total)
{
	map<long, MVCommonDataDescription>::iterator iterData;
	MVCommonDataDescription aktData;
	long nNumberOfTotalHits=0;	/// Number of total Hits/Digits in all Events
	long nNumberOfTotalEntries=0;	/// Number of stored Events in this Chamber

	iterData = tmpStationEvents.begin();

	/// Loop over all Events stored in this Station
	while (iterData!=tmpStationEvents.end())
	{
		aktData=(*iterData).second;
		nNumberOfTotalHits+=aktData.getNumberOfEntries();
		nNumberOfTotalEntries++;
		iterData++;
	}
	pass=nNumberOfTotalHits;
	total=nNumberOfTotalEntries;
	if (nNumberOfTotalEntries==0) return -1.0;
	return (((float)(nNumberOfTotalHits))/((float)(nNumberOfTotalEntries)));
}

/// Checks if event with ID exists already in tmpStationEvents 
bool MVCommonStationData::eventExists(long id, std::map<long, MVCommonDataDescription> &tmpStationEvents)
{
	map<long, MVCommonDataDescription>::iterator p;

	p=tmpStationEvents.find(id);
	if (p == tmpStationEvents.end()) return false;
		return true;
}

/** Calculates how many Digits or Hits can be identified with each other*/
MVCommonResiduumResults
	MVCommonStationData::getMeanDifference(	std::map<long, MVCommonDataDescription> &tmpStationEvents1,
						std::map<long, MVCommonDataDescription> &tmpStationEvents2,
						int minDifference)
{
	MVCommonResiduumResults mvCommonRes;
	mvCommonRes.CommonOverTotal1=-1.0;
	mvCommonRes.CommonOverTotal2=-1.0;
	mvCommonRes.vResRadius.clear();
	mvCommonRes.vResZ0.clear();
	map<long, MVCommonDataDescription>::iterator iterData;
	long aktEventID;
	MVCommonDataDescription 	aktEvent1;
	MVCommonDataDescription 	aktEvent2;
	int nNumberOfCommonHits	= 0;
	long nNumberOfCheckedEvents=0;
	float fCommonOverTotal1=0.0;
	float fCommonOverTotal2=0.0;
//	float fResRadius;
//	float fResZ0;
//	int i,j;
	//MDTHit mdthit;

	mvCommonRes.CommonHits=0;
	mvCommonRes.TotalHits1=0;
	mvCommonRes.TotalHits2=0;
	/// Loop over all Events of first file
	iterData = tmpStationEvents1.begin();
	while (iterData!=tmpStationEvents1.end())
	{
		aktEventID	= (*iterData).first;
		aktEvent1	= (*iterData).second;
		nNumberOfCommonHits=0;
		/// Check if the acutal Event of File_1 exisits in the second given File
		if (eventExists(aktEventID,tmpStationEvents2)==true)
		{
			nNumberOfCheckedEvents++;
			aktEvent2=tmpStationEvents2[aktEventID];

/*			std::cout<<"AktEvent1"<<std::endl;
			aktEvent1.startUpHitCounter();
			while (aktEvent1.getNextHitInfo(mdthit)==true)
			std::cout<<"Tube: "<<mdthit.IDTube<<"   MultiLayer: "<<mdthit.IDMultilayer<<"   Layer: "<<mdthit.IDLayer<<std::endl;

			std::cout<<"AktEvent2"<<std::endl;
			aktEvent2.startUpHitCounter();
			while (aktEvent2.getNextHitInfo(mdthit)==true)
			std::cout<<"Tube: "<<mdthit.IDTube<<"   MultiLayer: "<<mdthit.IDMultilayer<<"   Layer: "<<mdthit.IDLayer<<std::endl;*/

			/// Loop over all Hits/Digits stored in this event of First File (for this chamber)
			aktEvent2.hasSameEntry(aktEvent1,minDifference,nNumberOfCommonHits);
			mvCommonRes.CommonHits+=nNumberOfCommonHits;

			if (aktEvent1.getNumberOfEntries()!=0)
			{
				fCommonOverTotal1+=((float)(nNumberOfCommonHits))/((float)(aktEvent1.getNumberOfEntries()));
				mvCommonRes.TotalHits1+=aktEvent1.getNumberOfEntries();
			}
			if (aktEvent2.getNumberOfEntries()!=0)
			{
				fCommonOverTotal2+=((float)(nNumberOfCommonHits))/((float)(aktEvent2.getNumberOfEntries()));
				mvCommonRes.TotalHits2+=aktEvent2.getNumberOfEntries();
			}
		}
		iterData++;
	}
	if(nNumberOfCheckedEvents!=0)
	{
	mvCommonRes.CommonOverTotal1=fCommonOverTotal1/((float)nNumberOfCheckedEvents);
	mvCommonRes.CommonOverTotal2=fCommonOverTotal2/((float)nNumberOfCheckedEvents);
	}
			
	return mvCommonRes;
}


void MVCommonStationData::fillDataInHistoGramm(IHistogram1D* Histo, int HistoType)
{
	map<long, MVCommonDataDescription>::iterator iterData, iterDataEnd;
	MVCommonDataDescription aktData;

	if 	((HistoType!=1) &&
		(HistoType!=2)) return;

	if (HistoType==1)
	{
		iterData 	= m_EventNumber_StationVersion1Events.begin();
		iterDataEnd 	= m_EventNumber_StationVersion1Events.end();
	}
	if (HistoType==2)
	{
		iterData 	= m_EventNumber_StationVersion2Events.begin();
		iterDataEnd 	= m_EventNumber_StationVersion2Events.end();
	}

	while (iterData!=iterDataEnd)
	{
		aktData=(*iterData).second;
		Histo->fill(aktData.getNumberOfEntries());
		iterData++;
	}
}


char MVCommonStationData::getChar(std::string input, int i)
{
	if ((i>=0) && (i<(int)input.length()))
	{
		const char *temp=input.c_str();
		return temp[i];
	}

	return 0;
}

