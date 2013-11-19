/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
// Name    : MVCombined.h
// Package : MuonValidation
// Author: M.Schott (LMU)
// CERN, MAY 1, 2006
//
// DESCRIPTION:
//
// This class combines all results of the single MuonValidation results like Hits- or Digits-collections
// The Basic Idea is to store all the digit and hit information chamber per chamber and event by event
// The comparison is done within the chamberdescription
/////////////////////////////////////////////////////////////////////

 

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include <vector>
#include <map>
#include <TFile.h>
#include <TF1.h>
#include <TH2.h>
#include <TH1.h>
#include <TChain.h>
#include <TGraphErrors.h>
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "StoreGate/StoreGateSvc.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"


#include "MuonEvtValidator/MVCommonStationID.h"
#include "MuonEvtValidator/MVCommonStationData.h"
#include "MuonEvtValidator/MVCommonStationData.h"
#include "MuonEvtValidator/MVCommonDataDescription.h"
#include "MuonEvtValidator/MVCommonDataLoader.h"
#include "MuonEvtValidator/MSRootInterface.h"
/////////////////////////////////////////////////////////////////////////////

class MVCombined:public Algorithm
{
	public:

		/** Main constructor */
		MVCombined (const std::string& name, ISvcLocator* pSvcLocator);
	
		StatusCode initialize();
		StatusCode execute();
		StatusCode finalize();

	private:
		/** initialising this Algorithm */
		void setup();

		void writeTGraph();

		/** Routines for the Histogramm generation for MDT, RPC, CSC and TGC chambers separatly */
		StatusCode Generate_MDT_Histograms();
		StatusCode Generate_RPC_Histograms();
		StatusCode Generate_CSC_Histograms();
		StatusCode Generate_TGC_Histograms();

		/** Routine for Histogramm generation for a single chamber */
		StatusCode GenerateSingleChamberHistograms(int id, int eta, int phi);

		/** MuonValidation Loader Object. This object loads the AANtuples that
		contain the digit or hit informations*/
		MVCommonDataLoader vtLoader;

		/** Loading the AANtuples using the MVCommonDataLoader-Object*/
		StatusCode loadDataFiles();

		ITHistSvc*	ToolRootHistSvc();

		/** Each Station (MDT, RPC, CSC, TGC) can be identified with a unique ID
		- see MVCommonStationID - this ID is used as Map-Identifier under which the
		data of this chamber is stored*/

		bool	m_excludeDoubleCounting;

		std::map<long, MVCommonStationData> m_MdtStationDescription;
		std::map<long, MVCommonStationData> m_RpcStationDescription;
		std::map<long, MVCommonStationData> m_CscStationDescription;
		std::map<long, MVCommonStationData> m_TgcStationDescription;

		//MVCommonMuonStationData<MVMdtDataDescription> test;

		long n_EventCounter;

		/// a handle on Store Gate 
		StoreGateSvc* m_storeGate;

		// OutputFile
		std::string m_OutputFile;

		/// Filename of first & second inputfile. These files get compared
		std::string m_InputVersion1File;
		std::string m_InputVersion2File;

		/** Names to distinguish between the final Histogramms. Each Histogramm
		that is generated from the first input-file starts with "m_Version1Name"*/
		std::string m_Version1Name;
		std::string m_Version2Name;

		/** Names to tell the progromm which input-File it gets, "Hits", "Digits"*/
		std::string m_FileType1;
		std::string m_FileType2;

		/// These filenames will be implemented soon
		std::string m_InputPositioningFile;
		std::string m_InputPreprodDataFile;
		std::string m_InputSegmentsFile;

		int m_minimalDifference;

		/// Identifier of MDT-Chamber which is validated alone
		int m_SpecialStationNameID;
		int m_SpecialStationEta;
		int m_SpecialStationPhi;

		/// Histogramms
		std::map<std::string, IHistogram1D*> m_1D_Histos;
		std::map<std::string, IHistogram2D*> m_2D_Histos;

		MSRootInterface msRootInterface;
	protected:
		ITHistSvc*     m_rootsvc;
};
