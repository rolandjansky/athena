/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Name    : MVCommonDataDescription.cxx
// Package : MuonValidation
// Author: M.Schott (LMU)
// CERN, MAY 1, 2006
//
// DESCRIPTION:
//
// This class stores the Hit/Digit-Information for one Event and for one Chamber
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
 

#include "MuonEvtValidator/MVCommonDataDescription.h"


MVCommonDataDescription::MVCommonDataDescription()
{
	clear();
	m_Type="MDT";
	m_id=-1;
	m_ExcludeDoubleCounting=false;
}


MVCommonDataDescription::MVCommonDataDescription(std::string Type)
{
	clear();
	m_Type=Type;
	m_id=-1;
	m_ExcludeDoubleCounting=false;
}


MVCommonDataDescription::MVCommonDataDescription(const MVCommonDataDescription& ob)
{
	m_Type 			= ob.m_Type;
	m_id			= ob.m_id;
	m_ExcludeDoubleCounting = ob.m_ExcludeDoubleCounting;
	m_Validation_CSC	= ob.m_Validation_CSC;
	m_Validation_TGC	= ob.m_Validation_TGC;
	m_Validation_MDT	= ob.m_Validation_MDT;
	m_Validation_RPC	= ob.m_Validation_RPC;
        m_HitCounter=0; 
}

MVCommonDataDescription& MVCommonDataDescription::operator = (const MVCommonDataDescription& ob)
{
	if(this==&ob) return *this; //check self-assignment
	m_Type			= ob.m_Type;
	m_id			= ob.m_id;
	m_ExcludeDoubleCounting = ob.m_ExcludeDoubleCounting;
	m_Validation_CSC	= ob.m_Validation_CSC;
	m_Validation_TGC	= ob.m_Validation_TGC;
	m_Validation_MDT	= ob.m_Validation_MDT;
	m_Validation_RPC	= ob.m_Validation_RPC;
	return *this;
}

void MVCommonDataDescription::clear()
{
	m_Type="MDT";
	m_HitCounter=0;
	m_id=-1;

	m_Validation_CSC.clear();
	m_Validation_TGC.clear();
	m_Validation_MDT.clear();
	m_Validation_RPC.clear();
}


void MVCommonDataDescription::setID(long id)
{
	m_id=id;
}

long MVCommonDataDescription::getID()
{
	return m_id;
}


/// For CSC-Chambers
/// Adding Information of one Digit or one Hit. For a description of the given data look at ...Note
void MVCommonDataDescription::addData(	int ChamberLayer,
					int WireLayer, 
					int MeasuresPhi,
					int Strip,
					float StripX,
					float StripY,
					float StripZ,
					float Charge)
{

	CSCHit	mCSCHit;

	int HitID;
	if (	(m_ExcludeDoubleCounting==true) && (hasSameEntry(0, ChamberLayer, WireLayer, MeasuresPhi, Strip, HitID)==true)) return;

	mCSCHit.ChamberLayer	= ChamberLayer;
	mCSCHit.WireLayer	= WireLayer;
	mCSCHit.MeasuresPhi	= MeasuresPhi;
	mCSCHit.Strip		= Strip;
	mCSCHit.StripX		= StripX;
	mCSCHit.StripY		= StripY;
	mCSCHit.StripZ		= StripZ;
	mCSCHit.Charge		= Charge;

	m_Validation_CSC.push_back(mCSCHit);

}

/// For MDT
/// Adding Information of one Digit or one Hit. For a description of the given data look at ...Note
void MVCommonDataDescription::addData(	int IDTube, 
					int IDMultilayer, 
					int IDLayer,
					float WireX, 
					float WireY, 
					float WireZ, 
					float Driftradius)
{

	MDTHit mMDTHit;

	int HitID;
	if ((m_ExcludeDoubleCounting==true) && (hasSameEntry(0, IDTube, IDMultilayer, IDLayer, HitID)==true)) return;

	mMDTHit.IDTube		= IDTube;
	mMDTHit.IDMultilayer	= IDMultilayer;
	mMDTHit.IDLayer		= IDLayer;
	mMDTHit.WireX		= WireX;
	mMDTHit.WireY		= WireY;
	mMDTHit.WireZ		= WireZ;
	mMDTHit.Driftradius	= Driftradius;

	m_Validation_MDT.push_back(mMDTHit);

}


/// For RPC
/// Adding Information of one Digit or one Hit. For a description of the given data look at ...Note
void MVCommonDataDescription::addData(	int DoubletR, 
					int DoubletZ, 
					int DoubletPhi,
					int DoubletGasGap,
					int MeasuresPhi,
					int Strip,
					float StripX,
					float StripY,
					float StripZ,
					float Stripdistance)
{

	RPCHit	mRPCHit;

	int HitID;
	if (	(m_ExcludeDoubleCounting==true) &&
		(hasSameEntry(0, DoubletR, DoubletZ, DoubletPhi, DoubletGasGap, MeasuresPhi, Strip, HitID)==true)) return;

	mRPCHit.DoubletR	= DoubletR;
	mRPCHit.DoubletZ	= DoubletZ;
	mRPCHit.DoubletPhi	= DoubletPhi;
	mRPCHit.DoubletGasGap	= DoubletGasGap;
	mRPCHit.MeasuresPhi	= MeasuresPhi;
	mRPCHit.Strip		= Strip;
	mRPCHit.StripX		= StripX;
	mRPCHit.StripY		= StripY;
	mRPCHit.StripZ		= StripZ;
	mRPCHit.Stripdistance	= Stripdistance;

	m_Validation_RPC.push_back(mRPCHit);

}

/// For TGC-Chamber
void MVCommonDataDescription::addData(	int GasGap, 
					int IsStrip, 
					int Channel,
					float ChannelPosX,
					float ChannelPosY,
					float ChannelPosZ)
{

	TGCHit	mTGCHit;

	int HitID;
	if (	(m_ExcludeDoubleCounting==true) && (hasSameEntry(0, true, GasGap, IsStrip, Channel, HitID)==true)) return;

	mTGCHit.GasGap		= GasGap;
	mTGCHit.IsStrip		= IsStrip;
	mTGCHit.Channel		= Channel;
	mTGCHit.ChannelPosX	= ChannelPosX;
	mTGCHit.ChannelPosY	= ChannelPosY;
	mTGCHit.ChannelPosZ	= ChannelPosZ;

	m_Validation_TGC.push_back(mTGCHit);

}

bool MVCommonDataDescription::hasSameEntry(	MVCommonDataDescription &ob,
						int MinDistance,
						int &NumberOfCommonHits)
{
	bool 		returnType=false;
	std::string	obType=ob.getType();

	NumberOfCommonHits=0;
	ob.startUpHitCounter();
	int HitID;

	if (obType=="MDT")
	{
		MDTHit tmp;
		while (ob.getNextHitInfo(tmp)==true)
		{
			returnType = hasSameEntry(	MinDistance,
							tmp.IDTube,
							tmp.IDMultilayer,
							tmp.IDLayer,
							HitID);
			if (returnType==true) NumberOfCommonHits++;
		}
	}

	if (obType=="CSC")
	{
		CSCHit tmp;
		while (ob.getNextHitInfo(tmp)==true)
		{
			returnType = hasSameEntry(	MinDistance,
							tmp.ChamberLayer,
							tmp.WireLayer,
							tmp.MeasuresPhi,
							tmp.Strip,
							HitID);
			if (returnType==true) NumberOfCommonHits++;
		}
	}
	if (obType=="RPC")
	{
		RPCHit tmp;
		while (ob.getNextHitInfo(tmp)==true)
		{
			returnType = hasSameEntry(	MinDistance,
							tmp.DoubletR,
							tmp.DoubletZ,
							tmp.DoubletPhi,
							tmp.DoubletGasGap,
							tmp.MeasuresPhi,
							tmp.Strip,
							HitID);
			if (returnType==true) NumberOfCommonHits++;
		}
	}
	if (obType=="TGC")
	{
		TGCHit tmp;
		while (ob.getNextHitInfo(tmp)==true)
		{
			returnType = hasSameEntry(	MinDistance,
							true,	/// This is just to distinguish MDTs and TGCs
							tmp.GasGap,
							tmp.IsStrip,
							tmp.Channel,
							HitID);
			if (returnType==true) NumberOfCommonHits++;
		}
	}

	if (NumberOfCommonHits>0) return true;
	return false;
}

void MVCommonDataDescription::startUpHitCounter()
{
	m_HitCounter=0;
}

bool MVCommonDataDescription::getNextHitInfo(CSCHit &tmp)
{
	if (m_HitCounter>=(int)m_Validation_CSC.size()) return false;
	tmp = m_Validation_CSC[m_HitCounter];
	m_HitCounter++;
	return true;
}

bool MVCommonDataDescription::getNextHitInfo(RPCHit &tmp)
{
	if (m_HitCounter>=(int)m_Validation_RPC.size()) return false;
	tmp = m_Validation_RPC[m_HitCounter];
	m_HitCounter++;
	return true;
}

bool MVCommonDataDescription::getNextHitInfo(TGCHit &tmp)
{
	if (m_HitCounter>=(int)m_Validation_TGC.size()) return false;
	tmp = m_Validation_TGC[m_HitCounter];
	m_HitCounter++;
	return true;
}

bool MVCommonDataDescription::getNextHitInfo(MDTHit &tmp)
{
	if (m_HitCounter>=(int)m_Validation_MDT.size()) return false;
	tmp = m_Validation_MDT[m_HitCounter];
	m_HitCounter++;
	return true;
}

bool MVCommonDataDescription::getNextHitPosition(float &x, float &y, float &z)
{
	getType();

	x=0.0;
	y=0.0;
	z=0.0;

	if (m_Type=="MDT")
	{
		if (m_HitCounter>=(int)m_Validation_MDT.size()) return false;
		x = m_Validation_MDT[m_HitCounter].WireX;
		y = m_Validation_MDT[m_HitCounter].WireY;
		z = m_Validation_MDT[m_HitCounter].WireZ;
	}
	if (m_Type=="CSC")
	{
		if (m_HitCounter>=(int)m_Validation_CSC.size()) return false;
		x = m_Validation_CSC[m_HitCounter].StripX;
		y = m_Validation_CSC[m_HitCounter].StripY;
		z = m_Validation_CSC[m_HitCounter].StripZ;
	}
	if (m_Type=="RPC")
	{
		if (m_HitCounter>=(int)m_Validation_RPC.size()) return false;
		x = m_Validation_RPC[m_HitCounter].StripX;
		y = m_Validation_RPC[m_HitCounter].StripY;
		z = m_Validation_RPC[m_HitCounter].StripZ;
	}
	if (m_Type=="TGC")
	{
		if (m_HitCounter>=(int)m_Validation_TGC.size()) return false;
		x = m_Validation_TGC[m_HitCounter].ChannelPosX;
		y = m_Validation_TGC[m_HitCounter].ChannelPosY;
		z = m_Validation_TGC[m_HitCounter].ChannelPosZ;
	}

	m_HitCounter++;
	return true;
}


std::string MVCommonDataDescription::getType()
{
	int nCSC = m_Validation_CSC.size();
	int nMDT = m_Validation_MDT.size();
	int nRPC = m_Validation_RPC.size();
	int nTGC = m_Validation_TGC.size();

	if ((nMDT>nCSC) && (nMDT>nTGC) && (nMDT>nRPC)) m_Type="MDT";
	if ((nCSC>nMDT) && (nCSC>nTGC) && (nCSC>nRPC)) m_Type="CSC";
	if ((nRPC>nMDT) && (nRPC>nTGC) && (nRPC>nCSC)) m_Type="RPC";
	if ((nTGC>nMDT) && (nTGC>nCSC) && (nTGC>nRPC)) m_Type="TGC";

	return m_Type;
}


/// For TGC
/// checks if this Digit/Hit is already stored withing this Event
bool MVCommonDataDescription::hasSameEntry(	int MinDistance,
						bool tag,	/// This is just to distinguish MDTs and TGCs
						int GasGap,
						int IsStrip,
						int Channel,
						int &j)
{
	bool __attribute__((__unused__)) useless = tag;// useless=1; //making the compiler happy
	int nDistance;
	for (j=0; j<(int)m_Validation_TGC.size(); j++)
	{
		nDistance = 	abs(m_Validation_TGC[j].GasGap-GasGap) +
				abs(m_Validation_TGC[j].IsStrip-IsStrip) +
				abs(m_Validation_TGC[j].Channel-Channel);
		if (nDistance<=MinDistance) return true;
	}
	j=-1;
	return false;
}

/// For RPC
/// checks if this Digit/Hit is already stored withing this Event
bool MVCommonDataDescription::hasSameEntry(int MinDistance, int DoubletR, int DoubletZ, int DoubletPhi,
					int DoubletGasGap, int MeasuresPhi, int Strip, int &j)
{
	int nDistance;
	for (j=0; j<(int)m_Validation_RPC.size(); j++)
	{
		nDistance = 	abs(m_Validation_RPC[j].DoubletR-DoubletR) +
				abs(m_Validation_RPC[j].DoubletZ-DoubletZ) +
				abs(m_Validation_RPC[j].DoubletPhi-DoubletPhi) +
				abs(m_Validation_RPC[j].DoubletGasGap-DoubletGasGap) +
				abs(m_Validation_RPC[j].MeasuresPhi-MeasuresPhi) +
				abs(m_Validation_RPC[j].Strip-Strip);

		if (nDistance<=MinDistance) return true;

	}
	j=-1;
	return false;
}

/// For MDT
/// checks if this Digit/Hit is already stored withing this Event
bool MVCommonDataDescription::hasSameEntry(int MinDistance, int IDTube, int IDMultilayer, int IDLayer, int &j)
{
	int nDistance;
	for (j=0; j<(int)m_Validation_MDT.size(); j++)
	{

		nDistance = 	abs(m_Validation_MDT[j].IDTube-IDTube) +
				abs(m_Validation_MDT[j].IDMultilayer-IDMultilayer) +
				abs(m_Validation_MDT[j].IDLayer-IDLayer);
		if (nDistance<=MinDistance) return true;
	}
	j=-1;
	return false;
}

/// For CSC
/// checks if this Digit/Hit is already stored withing this Event
bool MVCommonDataDescription::hasSameEntry(	int MinDistance,
						int ChamberLayer,
						int WireLayer,
						int MeasuresPhi,
						int Strip,
						int &j)
{
	int nDistance;
	for (j=0; j<(int)m_Validation_CSC.size(); j++)
	{
		nDistance=	abs(m_Validation_CSC[j].ChamberLayer-ChamberLayer) +
				abs(m_Validation_CSC[j].WireLayer-WireLayer) +
				abs(m_Validation_CSC[j].MeasuresPhi-MeasuresPhi) +
				abs(m_Validation_CSC[j].Strip-Strip);
		if (nDistance<=MinDistance) return true;
	}
	j=-1;
	return false;
}

/// returns numner of Hits or Digits in this Event
int MVCommonDataDescription::getNumberOfEntries()
{
	int nCSC = m_Validation_CSC.size();
	int nMDT = m_Validation_MDT.size();
	int nRPC = m_Validation_RPC.size();
	int nTGC = m_Validation_TGC.size();

	if ((nMDT>nCSC) && (nMDT>nTGC) && (nMDT>nRPC)) return nMDT;
	if ((nCSC>nMDT) && (nCSC>nTGC) && (nCSC>nRPC)) return nCSC;
	if ((nRPC>nMDT) && (nRPC>nTGC) && (nRPC>nCSC)) return nRPC;
	if ((nTGC>nMDT) && (nTGC>nCSC) && (nTGC>nRPC)) return nTGC;
	return 0;
}
