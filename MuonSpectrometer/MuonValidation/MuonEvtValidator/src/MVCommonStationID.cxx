/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Name    : MVCommonDataDescription.cxx
// Package : MuonValidation
// Authors: M.Schott (LMU) - N. Benekos (MPI)
// CERN, MAY 1, 2006 
//
// DESCRIPTION:
//
// This class generates an unique ID-number for a Detectorelement from its ID, Eta and Phi value 
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
  


#include "MuonEvtValidator/MVCommonStationID.h"

MVCommonStationID::MVCommonStationID():
 m_StationName(0),
 m_StationEta(0),
 m_StationPhi(0) 
{
	m_type="MDT";
}

MVCommonStationID::MVCommonStationID(std::string type):
 m_StationName(0),
 m_StationEta(0),
 m_StationPhi(0) 

{
	m_type=type;
       // m_StationEta=0;
       // m_StationName=0;
       // m_StationPhi=0; 

}

MVCommonStationID::MVCommonStationID(const MVCommonStationID& ob)
{
	m_StationName 	= ob.m_StationName;
	m_StationEta 	= ob.m_StationEta;
	m_StationPhi 	= ob.m_StationPhi;
	m_type		= ob.m_type;
}

MVCommonStationID&  MVCommonStationID::operator = (const MVCommonStationID& ob)
{

 	if (this == &ob) return *this; //check self-assignment
	m_StationName 	= ob.m_StationName;
	m_StationEta 	= ob.m_StationEta;
	m_StationPhi 	= ob.m_StationPhi;
	m_type		= ob.m_type;
	return *this;
}

bool MVCommonStationID::operator == (const MVCommonStationID& ob)
{
	if (	   (m_StationName == ob.m_StationName)
		&& (m_StationEta  == ob.m_StationEta)
		&& (m_StationPhi  == ob.m_StationPhi)
		&& (m_type	  == ob.m_type)) return true;
	return false;
}

/** Gives back the Unique ID-Number corresponding to the given values
Note: The InternalName is NOT necessary equivalent to the Station NameID
The two values could be converted via convertFromCommonStationNameNumber
*/
long MVCommonStationID::getLongIDNumber(int Name, int Eta, int Phi)
{
	return 	(Name-getMinStationName())
			*(getMaxStationEta()-getMinStationEta()+1)*(getMaxStationPhi()-getMinStationPhi()+1) +
		(Eta-getMinStationEta())
			*(getMaxStationPhi()-getMinStationPhi()+1) +
		(Phi-getMinStationPhi());
}

/// Gives back the Unique ID-Number of this Station
long MVCommonStationID::getLongIDNumber()
{
	return getLongIDNumber(m_StationName, m_StationEta, m_StationPhi);
}

/// Gives the Maximal Possible MVCommon-ID-Number
long MVCommonStationID::getMaxIDNumber()
{
	return getLongIDNumber(getMaxStationName(), getMaxStationEta(), getMaxStationPhi());
}

/// Calculates the NameID, the ETA and Phi Value of the given Unique-ID
void MVCommonStationID::getNameIDEtaPhi(long LongIdNumber, int &Name, int &Eta, int &Phi)
{
	Name = 	(int)(((float)(LongIdNumber))/
		((float)((getMaxStationEta()-getMinStationEta()+1)*(getMaxStationPhi()-getMinStationPhi()+1))))+
		getMinStationName();

	Eta = 	LongIdNumber -
		(Name-getMinStationName())
		*(((getMaxStationEta()-getMinStationEta()+1)*(getMaxStationPhi()-getMinStationPhi()+1)));

	Eta = (int)(((float)(Eta))/((float)(getMaxStationPhi()-getMinStationPhi()+1)))+getMinStationEta();

	Phi = 	LongIdNumber -
		(Name-getMinStationName())
		* (((getMaxStationEta()-getMinStationEta()+1)*(getMaxStationPhi()-getMinStationPhi()+1))) -
		(Eta-getMinStationEta())*((getMaxStationPhi()-getMinStationPhi()+1)) +
		getMinStationPhi();
}


/** The followimg function give back the maximal and minimal Value of StationName, Eta and Phi
 This differs for MDTs, RPCs, TGC, CSCs*/
int MVCommonStationID::getMaxStationName()
{
	if (m_type=="MDT") return 53;
	if (m_type=="RPC") return 53;
	if (m_type=="TGC") return 53;
	if (m_type=="CSC") return 53;
	return 0;
}


int MVCommonStationID::getMinStationName()
{
	if (m_type=="MDT") return 0;
	if (m_type=="RPC") return 0;
	if (m_type=="TGC") return 0;
	if (m_type=="CSC") return 0;
	return 0;
}


int MVCommonStationID::getMaxStationEta()
{
	if (m_type=="MDT") return 8;
	if (m_type=="RPC") return 8;
	if (m_type=="TGC") return 6;
	if (m_type=="CSC") return 1;
	return 0;
}


int MVCommonStationID::getMinStationEta()
{
	if (m_type=="MDT") return -8;
	if (m_type=="RPC") return -8;
	if (m_type=="TGC") return -6;
	if (m_type=="CSC") return -1;
	return 0;
}

int MVCommonStationID::getMaxStationPhi()
{
	if (m_type=="MDT") return 8;
	if (m_type=="RPC") return 8;
	if (m_type=="TGC") return 50;
	if (m_type=="CSC") return 8;
	return 0;
}

int MVCommonStationID::getMinStationPhi()
{
	if (m_type=="MDT") return 0;
	if (m_type=="RPC") return 0;
	if (m_type=="TGC") return 0;
	if (m_type=="CSC") return 0;
	return 0;
}

/** Converts the usual StationNameID to internal used StationNameID
The Idea is, that the internal numbering is done without spacing
*/
/*int MVCommonStationID::convertFromCommonStationNameNumber(int CommonStationNameID)
{
	if (m_type=="MDT")
	{
		switch (CommonStationNameID)
		{
		case 0  : return 0;//"BIL";
		case 1  : return 1;//"BIS";
		case 2  : return 2;//"BML";
		case 3  : return 3;//"BMS";
		case 4  : return 4;//"BOL";
		case 5  : return 5;//"BOS";
		case 6  : return 6;//"BEE";
		case 7  : return 7;//"BIR";
		case 8  : return 8;//"BMF";
		case 9  : return 9;//"BOF";
		case 10 : return 10;//"BOG";
		case 11 : return 11;//"BOH";
		case 12 : return 12;//"EIC"; ???
		case 13 : return 13;//"EIL";
		case 14 : return 14;//"EEL";
		case 15 : return 15;//"EES"; ???
		case 16 : return 16;//"EMC"; ???
		case 17 : return 17;//"EML";
		case 18 : return 18;//"EMS";
		case 19 : return 19;//"EOC"; ???
		case 20 : return 20;//"EOL";
		case 21 : return 21;//"EOS";
		case 49 : return 22;//"EIS";
		}
	}
	if (m_type=="RPC")
	{
		switch (CommonStationNameID) 
		{
		case 0  : return 0;//"BIL";
		case 1  : return 1;//"BIS";
		case 2  : return 2;//"BML";
		case 3  : return 3;//"BMS";
		case 4  : return 4;//"BOL";
		case 5  : return 5;//"BOS";
		case 6  : return 6;//"BEE";
		case 7  : return 7;//"BIR";
		case 8  : return 8;//"BMF";
		case 9  : return 9;//"BOF";
		case 10 : return 10;//"BOG";
		case 11 : return 11;//"BOH";
		}
	}
	if (m_type=="CSC")
	{
		switch (CommonStationNameID) 
		{
			case 50 : return 0;//"CSS";
			case 51 : return 1;//"CSL";
		}
	}
	if (m_type=="TGC")
	{
		switch (CommonStationNameID) 
		{
			case 41 : return 0;//"T1F";
			case 42 : return 1;//"T1E";
			case 43 : return 2;//"T2F";
			case 44 : return 3;//"T2E";
			case 45 : return 4;//"T3F";
			case 46 : return 5;//"T3E";
			case 47 : return 6;//"T4F";
			case 48 : return 7;//"T4E";
		}
	}
	return -1;
}

/// Converts the internal used StationNameID to the usual StationNameID
int MVCommonStationID::convertToCommonStationNameNumber(int InternStationNameID)
{
	for (int i=0; i<60; i++) if (convertFromCommonStationNameNumber(i)==InternStationNameID) return i;
	return -1;
}*/

		/// Gives back the StationName as String
std::string MVCommonStationID::getStationName(int CommonStationID)
{
	switch (CommonStationID) 
	{
		case 0  : return "BIL";
		case 1  : return "BIS";
		case 2  : return "BML";
		case 3  : return "BMS";
		case 4  : return "BOL";
		case 5  : return "BOS";
		case 6  : return "BEE";
		case 7  : return "BIR";
		case 8  : return "BMF";
		case 9  : return "BOF";
		case 10 : return "BOG";
		case 11 : return "BOH";
		case 12 : return "EIC";
		case 13 : return "EIL";
		case 14 : return "EEL";
		case 15 : return "EES";
		case 16 : return "EMC";
		case 17 : return "EML";
		case 18 : return "EMS";
		case 19 : return "EOC";
		case 20 : return "EOL";
		case 21 : return "EOS";
		case 22 : return "T1C";
		case 23 : return "T1L";
		case 24 : return "T1S";
		case 25 : return "T2C";
		case 26 : return "T2L";
		case 27 : return "T2S";
		case 28 : return "T3C";
		case 29 : return "T3L";
		case 30 : return "T3S";
		case 31 : return "CI1";
		case 32 : return "CI2";
		case 33 : return "CI3";
		case 34 : return "CI4";
		case 35 : return "FIL";
		case 36 : return "FIS";
		case 37 : return "FML";
		case 38 : return "FMS";
		case 39 : return "FOL";
		case 40 : return "FOS";
		case 41 : return "T1F";
		case 42 : return "T1E";
		case 43 : return "T2F";
		case 44 : return "T2E";
		case 45 : return "T3F";
		case 46 : return "T3E";
		case 47 : return "T4F";
		case 48 : return "T4E";
		case 49 : return "EIS";
		case 50 : return "CSS";
		case 51 : return "CSL";
		case 52 : return "BIM";
	}
	return "XXX";
}

int MVCommonStationID::getStationName(std::string StationName)
{

    if (StationName=="BIL") return 0;
    if (StationName=="BIS") return 1;
    if (StationName=="BML") return 2;
    if (StationName=="BMS") return 3;
    if (StationName=="BOL") return 4;
    if (StationName=="BOS") return 5;
    if (StationName=="BEE") return 6;
    if (StationName=="BIR") return 7;
    if (StationName=="BMF") return 8;
    if (StationName=="BOF") return 9;
    if (StationName=="BOG") return 10;
    if (StationName=="BOH") return 11;
    if (StationName=="EIC") return 12;
    if (StationName=="EIL") return 13;
    if (StationName=="EEL") return 14;
    if (StationName=="EES") return 15;
    if (StationName=="EMC") return 16;
    if (StationName=="EML") return 17;
    if (StationName=="EMS") return 18;
    if (StationName=="EOC") return 19;
    if (StationName=="EOL") return 20;
    if (StationName=="EOS") return 21;
    if (StationName=="T1C") return 22;
    if (StationName=="T1L") return 23;
    if (StationName=="T1S") return 24;
    if (StationName=="T2C") return 25;
    if (StationName=="T2L") return 26;
    if (StationName=="T2S") return 27;
    if (StationName=="T3C") return 28;
    if (StationName=="T3L") return 29;
    if (StationName=="T3S") return 30;
    if (StationName=="CI1") return 31;
    if (StationName=="CI2") return 32;
    if (StationName=="CI3") return 33;
    if (StationName=="CI4") return 34;
    if (StationName=="FIL") return 35;
    if (StationName=="FIS") return 36;
    if (StationName=="FML") return 37;
    if (StationName=="FMS") return 38;
    if (StationName=="FOL") return 39;
    if (StationName=="FOS") return 40;
    if (StationName=="T1F") return 41;
    if (StationName=="T1E") return 42;
    if (StationName=="T2F") return 43;
    if (StationName=="T2E") return 44;
    if (StationName=="T3F") return 45;
    if (StationName=="T3E") return 46;
    if (StationName=="T4F") return 47;
    if (StationName=="T4E") return 48;
    if (StationName=="EIS") return 49;
    if (StationName=="CSS") return 50;
    if (StationName=="CSL") return 51;
    if (StationName=="BIM") return 52;
  return -1;
}

std::string MVCommonStationID::getStationName()
{
	return getStationName(m_StationName);
}
