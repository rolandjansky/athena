/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
// Name    : MVCommonDataDescription.h
// Package : MuonValidation
// Authors: M.Schott (LMU) - N. Benekos (MPI)
//
// DESCRIPTION:
//
// This class stores the Hit/Digit-Information for one Event and for one Chamber
/////////////////////////////////////////////////////////////////////
 
#ifndef MVCOMMONDATADESCRIPTION_H
#define MVCOMMONDATADESCRIPTION_H

#include "GaudiKernel/Algorithm.h"
#include <vector>
#include <string>

struct CSCHit
{
	int	ChamberLayer;
	int	WireLayer;
	int	MeasuresPhi;
	int	Strip;
	float	StripX;
	float	StripY;
	float	StripZ;
	float	Charge;
};


struct MDTHit
{
	int	IDTube;
	int	IDMultilayer;
	int	IDLayer;
	float	WireX;
	float	WireY;
	float	WireZ;
	float	Driftradius;
};

struct RPCHit
{
	int	DoubletR;
	int	DoubletZ;
	int	DoubletPhi;
	int	DoubletGasGap;
	int	MeasuresPhi;
	int	Strip;
	float	StripX;
	float	StripY;
	float	StripZ;
	float	Stripdistance;
};

struct TGCHit
{
	int	GasGap;
	int	IsStrip;
	int	Channel;
	float	ChannelPosX;
	float	ChannelPosY;
	float	ChannelPosZ;
};

class MVCommonDataDescription
{
	public:
		MVCommonDataDescription();
		MVCommonDataDescription(std::string Type);
		MVCommonDataDescription(const MVCommonDataDescription& ob);
		MVCommonDataDescription& operator = (const MVCommonDataDescription& ob);

		void clear();

		void setType(std::string Type);

		void setDoubleCounting(bool bflag)	{ m_ExcludeDoubleCounting=bflag;}

		void setID(long id);
		long getID();

		/// For CSC-Chamber
		/// Adding Information of one Digit or one Hit. For a description of the given data look at ...Note
		void addData(	int ChamberLayer, 
				int WireLayer, 
				int MeasuresPhi,
				int Strip,
				float StripX,
				float StripY,
				float StripZ,
				float Charge);

		/// For MDT-Chamber
		/// Adding Information of one Digit or one Hit. For a description of the given data look at ...Note
		void addData(	int IDTube,
				int IDMultilayer, 
				int IDLayer,
				float WireX, 
				float WireY, 
				float WireZ, 
				float Driftradius);

		/// For RPC-Chamber
		/// Adding Information of one Digit or one Hit. For a description of the given data look at ...Note
		void addData(	int DoubletR, 
				int DoubletZ, 
				int DoubletPhi,
				int DoubletGasGap,
				int MeasuresPhi,
				int Strip,
				float StripX,
				float StripY,
				float StripZ,
				float Stripdistance);


		/// For TGC-Chamber
		void addData(	int GasGap,
				int IsStrip,
				int Channel,
				float ChannelPosX,
				float ChannelPosY,
				float ChannelPosZ);

		bool hasSameEntry(	MVCommonDataDescription &ob,
					int MinDistance,
					int &NumberOfCommenEntries);

		void		startUpHitCounter();
		bool		getNextHitInfo(MDTHit &tmp);
		bool		getNextHitInfo(CSCHit &tmp);
		bool		getNextHitInfo(RPCHit &tmp);
		bool		getNextHitInfo(TGCHit &tmp);
		bool 		getNextHitPosition(float &x, float &y, float &z);

		int		getNumberOfEntries();
		std::string	getType();

	private:
		bool	m_ExcludeDoubleCounting;

		int	m_HitCounter;

		/// For CSC-Chamber
		/// checks if this Digit/Hit is already stored withing this Event
		bool hasSameEntry(	int minDistance, /// Mininal Distance to next hit (0=same Hit)
					int ChamberLayer, 
					int WireLayer, 
					int MeasuresPhi,
					int Strip,
					int &HitID);

		/// For MDT-Chamber
		/// checks if this Digit/Hit is already stored withing this Event
		bool hasSameEntry(	int minDistance, /// Mininal Distance to next hit (0=same Hit)
					int IDTube, 
					int IDMultilayer, 
					int IDLayer, 
					int &HitID);

		/// For RPC-Chamber
		/// checks if this Digit/Hit is already stored withing this Event
		bool hasSameEntry(	int minDistance, /// Mininal Distance to next hit (0=same Hit)
					int DoubletR, 
					int DoubletZ, 
					int DoubletPhi, 
					int DoubletGasGap, 
					int MeasuresPhi, 
					int Strip, 
					int &HitID);

		/// For TGC-Chamber
		/// checks if this Digit/Hit is already stored withing this Event
		bool hasSameEntry(	int minDistance, /// Mininal Distance to next hit (0=same Hit)
					bool tag,	/// This is just to distinguish MDTs and TGCs
					int GasGap, 
					int IsStrip, 
					int Channel,
					int &HitID);


		std::string m_Type;	// MDT  RPC  CSC  TGC

		long m_id;

		/// CSC-Data-Description
		std::vector<CSCHit>   m_Validation_CSC;

		/// MDT-Data-Description
		/// Variables for storing all informations of each digit/hit
		std::vector<MDTHit>   m_Validation_MDT;

		/// RPC-Data-Description
		/// Variables for storing all informations of each digit/hit
		std::vector<RPCHit>   m_Validation_RPC;

		/// TGC-Data-Description
		/// Variables for storing all informations of each digit/hit
		std::vector<TGCHit>   m_Validation_TGC;

};

#endif
