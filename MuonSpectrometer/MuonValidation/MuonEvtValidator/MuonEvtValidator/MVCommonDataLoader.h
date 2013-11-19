/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
// Name    : MVCommonDataLoader.h
// Package : MuonValidation
// Authors: M.Schott (LMU) - N. Benekos (MPI)
// CERN, MAY 1, 2006
//
// DESCRIPTION:
//
// This class loads the created Hits- or Digits-collections from external root-Files.
// These root-Files are created by separate Athena-Algorithms
//
/////////////////////////////////////////////////////////////////////
 
#ifndef MVCOMMONDATALOADER_H
#define MVCOMMONDATALOADER_H

#include <TFile.h>
#include <TF1.h>
#include <TH2.h>
#include <TChain.h>
#include <map>

#include "MuonEvtValidator/MVCommonStationID.h"
#include "MuonEvtValidator/MVCommonDataDescription.h"
#include "MuonEvtValidator/MVCommonStationData.h"
/////////////////////////////////////////////////////////////////////

class MVCommonDataLoader
{
	public:
		MVCommonDataLoader();

		void excludeDoubleCounting(bool bflag)	{ m_exludeDoubleCounting=bflag;}

		/** Loading a AANtuple and map the Information to the given variables
		mapMdtStationData, ... */
		int loadDataFile(	int Type,		/// "Hits", "Digits", "Positions", "Segements", "Preprod"
					std::string FileName,	/// Filename
					int versionID,		/// Do you load the first file => 1, Do you load the 2nd File =>2
					map<long, MVCommonStationData> &mapMdtStationData,
					map<long, MVCommonStationData> &mapRpcStationData,
					map<long, MVCommonStationData> &mapCscStationData,
					map<long, MVCommonStationData> &mapTgcStationData);

		/// Setting up the counter for the next routine
		void startup();

		/// Gives back event by event the MDT-Data - NOT chamberwise !
		bool getNext_Data(	std::string s_identifier, MVCommonDataDescription &mvVersion1);

	private:

		/// Loading the MDT-Information from the given File and writing it to mapMdtStationData
		int loadMdtDataFile(	int Type, /// "Hits", "Digits", "Positions", "Segements", "Preprod"
					std::string FileName,
					int versionID,		/// Do you load the first file => 1, Do you load the 2nd File =>2
					map<long, MVCommonStationData> &mapMdtStationData);

		/// Loading the RPC-Information from the given File and writing it to mapRpcStationData
		int loadRpcDataFile(	int Type, /// "Hits", "Digits", "Positions", "Segements", "Preprod"
					std::string FileName,
					int versionID,		/// Do you load the first file => 1, Do you load the 2nd File =>2
					map<long, MVCommonStationData> &mapRpcStationData);

		/// Loading the CSC-Information from the given File and writing it to mapCscStationData
		int loadCscDataFile(	int Type, /// "Hits", "Digits", "Positions", "Segements", "Preprod"
					std::string FileName,
					int versionID,		/// Do you load the first file => 1, Do you load the 2nd File =>2
					map<long, MVCommonStationData> &mapCscStationData);

		/// Loading the TGC-Information from the given File and writing it to mapTgcStationData
		int loadTgcDataFile(	int Type, /// "Hits", "Digits", "Positions", "Segements", "Preprod"
					std::string FileName,
					int versionID,		/// Do you load the first file => 1, Do you load the 2nd File =>2
					map<long, MVCommonStationData> &mapTgcStationData);

		/// Variables describing one Event on MDT-Level.
		long	m_Validation_Mdt_EventNumber;			/// Event Number
		long	m_Validation_Mdt_RunNumber;			/// Run Number
		long	m_Validation_Mdt_NumberOfHits;			/// Number of MDT-Hits in this Event

		bool	m_exludeDoubleCounting;

		TChain* m_mdt_tree;
		TChain* m_rpc_tree;
		TChain* m_csc_tree;
		TChain* m_tgc_tree;
		int	m_nType;
		long	nMDTCounter;
		long	nRPCCounter;
		long	nCSCCounter;
		long	nTGCCounter;
		long	nNumberOfMDTEvents;
		long	nNumberOfRPCEvents;
		long	nNumberOfCSCEvents;
		long	nNumberOfTGCEvents;

		/** The following variables are stored for each digit! For a description of the given names, please
		in */
		int	m_Validation_MDT_NumberOfHits;
		int	m_Validation_MDT_EventNumber;
		int	m_Validation_MDT_RunNumber;
		double	m_Validation_MDT_HitX;
		double	m_Validation_MDT_HitY;
		double	m_Validation_MDT_HitZ;
		double	m_Validation_MDT_HitR;
		char	m_Validation_MDT_StationName[3]; // string in as char array
		int	m_Validation_MDT_StationNameInt; // string in as char array
		int	m_Validation_MDT_StationEta;
		int	m_Validation_MDT_StationPhi;
		int	m_Validation_MDT_IDTube;
		int	m_Validation_MDT_IDMultiLayer;
		int	m_Validation_MDT_IDLayer;
		double	m_Validation_MDT_LocalX;
		double	m_Validation_MDT_LocalY;
		double	m_Validation_MDT_LocalZ;
		double	m_Validation_MDT_LocalR;


		/// Variables describing one Event on RPC-Level.
		long	m_Validation_Rpc_EventNumber;
		long	m_Validation_Rpc_RunNumber;
		long	m_Validation_Rpc_NumberOfHits;
		/** The following variables are stored for each digit! For a description of the given names, please
		in */

		int	m_Validation_RPC_Type;
		int	m_Validation_RPC_NumberOfHits;
		int	m_Validation_RPC_EventNumber;
		int	m_Validation_RPC_RunNumber;
		double	m_Validation_RPC_HitX;
		double	m_Validation_RPC_HitY;
		double	m_Validation_RPC_HitZ;
		double	m_Validation_RPC_HitR;
		char	m_Validation_RPC_StationName[3]; // string in as char array
		int	m_Validation_RPC_StationNameInt;
		int	m_Validation_RPC_StationEta;
		int	m_Validation_RPC_StationPhi;
		int	m_Validation_RPC_DoubletR;
		int	m_Validation_RPC_DoubletZ;
		int	m_Validation_RPC_DoubletPhi;
		int	m_Validation_RPC_MeasuresPhi;
		int	m_Validation_RPC_Strip;
		int	m_Validation_RPC_DoubletGasGap;


		/// Variables describing one Event on CSC-Level.
		long	m_Validation_Csc_EventNumber;
		long	m_Validation_Csc_RunNumber;
		long	m_Validation_Csc_NumberOfHits;
		/** The following variables are stored for each digit! For a description of the given names, please
		in */

		int	m_Validation_CSC_Type;
		int	m_Validation_CSC_NumberOfHits;
		int	m_Validation_CSC_EventNumber;
		int	m_Validation_CSC_RunNumber;
		double	m_Validation_CSC_HitX;
		double	m_Validation_CSC_HitY;
		double	m_Validation_CSC_HitZ;
		double	m_Validation_CSC_HitR;
		char	m_Validation_CSC_StationName[3]; // string in as char array
		int	m_Validation_CSC_StationNameInt;
		int	m_Validation_CSC_StationEta;
		int	m_Validation_CSC_StationPhi;
		int	m_Validation_CSC_ChamberLayer;
		int	m_Validation_CSC_WireLayer;
		int	m_Validation_CSC_MeasuresPhi;
		int	m_Validation_CSC_Strip;


		/// Variables describing one Event on TGC-Level.
		long	m_Validation_Tgc_EventNumber;
		long	m_Validation_Tgc_RunNumber;
		long	m_Validation_Tgc_NumberOfHits;
		/** The following variables are stored for each digit! For a description of the given names, please
		in */

		int	m_Validation_TGC_Type;
		int	m_Validation_TGC_NumberOfHits;
		int	m_Validation_TGC_EventNumber;
		int	m_Validation_TGC_RunNumber;
		double	m_Validation_TGC_HitX;
		double	m_Validation_TGC_HitY;
		double	m_Validation_TGC_HitZ;
		double	m_Validation_TGC_HitR;
		char	m_Validation_TGC_StName[3]; // string in as char array
		int	m_Validation_TGC_StNameInt;
		int	m_Validation_TGC_StationEta;
		int	m_Validation_TGC_StationPhi;
		int	m_Validation_TGC_GasGap;
		int	m_Validation_TGC_IsStrip;
		int	m_Validation_TGC_Channel;


		/// Counter for the GetNextMDTEvent-Method
		long	lAktEvent_mdt1;
		long	lAktEvent_mdt2;
		long	lAktEvent_rpc1;
		long	lAktEvent_rpc2;
		long	lAktEvent_csc1;
		long	lAktEvent_csc2;
		long	lAktEvent_tgc1;
		long	lAktEvent_tgc2;

		/// Variables to store the Information Event by Event (NOT Chamber by Chamber)
		std::vector<MVCommonDataDescription> m_vecVersion1_mdt;
		std::vector<MVCommonDataDescription> m_vecVersion2_mdt;

		std::vector<MVCommonDataDescription> m_vecVersion1_csc;
		std::vector<MVCommonDataDescription> m_vecVersion2_csc;

		std::vector<MVCommonDataDescription> m_vecVersion1_rpc;
		std::vector<MVCommonDataDescription> m_vecVersion2_rpc;

		std::vector<MVCommonDataDescription> m_vecVersion1_tgc;
		std::vector<MVCommonDataDescription> m_vecVersion2_tgc;
};

#endif
