/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Name    : MVCommonDataLoader.cxx
// Package : MuonValidation
// Authors: M.Schott (LMU) - N. Benekos (MPI)
// CERN, MAY 1, 2006
//
// DESCRIPTION:
//
// This class loads the created Hits- or Digits-collections from external root-Files.
// These root-Files where created by separate Athena-Algorithms
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
 

#include "MuonEvtValidator/MVCommonDataLoader.h"

/// Default Constructor - Empty
MVCommonDataLoader::MVCommonDataLoader()
{
  m_exludeDoubleCounting=false;
  m_Validation_Mdt_EventNumber=0;
  m_Validation_Mdt_RunNumber=0;
  m_Validation_Mdt_NumberOfHits=0;
  
  m_mdt_tree=0;
  m_rpc_tree=0;
  m_csc_tree=0;
  m_tgc_tree=0;
  
  m_nType=0;
  
  nMDTCounter=0;
  nRPCCounter=0;
  nCSCCounter=0;
  nTGCCounter=0;

  nNumberOfMDTEvents=0;
  nNumberOfRPCEvents=0;
  nNumberOfCSCEvents=0;
  nNumberOfTGCEvents=0;

  m_Validation_MDT_NumberOfHits=0;
  m_Validation_MDT_EventNumber=0;
  m_Validation_MDT_RunNumber=0;
  m_Validation_MDT_HitX=0.;
  m_Validation_MDT_HitY=0.;
  m_Validation_MDT_HitZ=0.;
  m_Validation_MDT_HitR=0.;
  //m_Validation_MDT_StationName[3];//={0,0,0};
  m_Validation_MDT_StationNameInt=0;
  m_Validation_MDT_StationEta=0;
  m_Validation_MDT_StationPhi=0;
  m_Validation_MDT_IDTube=0;
  m_Validation_MDT_IDMultiLayer=0;
  m_Validation_MDT_IDLayer=0;
  m_Validation_MDT_LocalX=0.;
  m_Validation_MDT_LocalY=0.;
  m_Validation_MDT_LocalZ=0.;
  m_Validation_MDT_LocalR=0.;
  
  m_Validation_Rpc_EventNumber=0;
  m_Validation_Rpc_RunNumber=0;
  m_Validation_Rpc_NumberOfHits=0;

  m_Validation_RPC_Type=0;
  m_Validation_RPC_RunNumber=0;
  m_Validation_RPC_NumberOfHits=0;
  m_Validation_RPC_EventNumber=0;
  m_Validation_RPC_HitX=0.;
  m_Validation_RPC_HitY=0.; 
  m_Validation_RPC_HitZ=0.;
  m_Validation_RPC_HitR=0.;
  //m_Validation_RPC_StationName[3];//={0,0,0} 
  m_Validation_RPC_StationNameInt=0;
  m_Validation_RPC_StationEta=0;
  m_Validation_RPC_StationPhi=0; 
  m_Validation_RPC_DoubletR=0;
  m_Validation_RPC_DoubletZ=0;
  m_Validation_RPC_DoubletPhi=0; 
  m_Validation_RPC_MeasuresPhi=0;
  m_Validation_RPC_Strip=0;
  m_Validation_RPC_DoubletGasGap=0;
   
  m_Validation_Csc_EventNumber=0;
  m_Validation_Csc_RunNumber=0;
  m_Validation_Csc_NumberOfHits=0;

  m_Validation_CSC_Type=0;
  m_Validation_CSC_NumberOfHits=0;
  m_Validation_CSC_EventNumber=0;
  m_Validation_CSC_RunNumber=0;
  m_Validation_CSC_HitX=0.;
  m_Validation_CSC_HitY=0.;
  m_Validation_CSC_HitZ=0.;
  m_Validation_CSC_HitR=0.;
  //m_Validation_CSC_StationName[3];//={0,0,0};
  m_Validation_CSC_StationNameInt=0;
  m_Validation_CSC_StationEta=0;
  m_Validation_CSC_StationPhi=0;
  m_Validation_CSC_ChamberLayer=0;
  m_Validation_CSC_WireLayer=0;
  m_Validation_CSC_MeasuresPhi=0;
  m_Validation_CSC_Strip=0;

  m_Validation_Tgc_EventNumber=0;
  m_Validation_Tgc_RunNumber=0;
  m_Validation_Tgc_NumberOfHits=0;

  m_Validation_TGC_Type=0;
  m_Validation_TGC_NumberOfHits=0;
  m_Validation_TGC_EventNumber=0;
  m_Validation_TGC_RunNumber=0;
  m_Validation_TGC_HitX=0.;
  m_Validation_TGC_HitY=0.;
  m_Validation_TGC_HitZ=0.;
  m_Validation_TGC_HitR=0.;
  //m_Validation_TGC_StName[3];//={0,0,0};
  m_Validation_TGC_StNameInt=0;
  m_Validation_TGC_StationEta=0;
  m_Validation_TGC_StationPhi=0;
  m_Validation_TGC_GasGap=0;
  m_Validation_TGC_IsStrip=0;
  m_Validation_TGC_Channel=0;

  lAktEvent_mdt1=0.;
  lAktEvent_mdt2=0.;
  lAktEvent_rpc1=0.;
  lAktEvent_rpc2=0.;
  lAktEvent_csc1=0.;
  lAktEvent_csc2=0.;
  lAktEvent_tgc1=0.;
  lAktEvent_tgc2=0.;
  for(int i=0;i<3;i++)
  {
   m_Validation_TGC_StName[i]=0;
   m_Validation_CSC_StationName[i]=0;
   m_Validation_RPC_StationName[i]=0; 
   m_Validation_MDT_StationName[i]=0;
  }

}  


/** Loading a AANtuple and map the Information to the given variables
    mapMdtStationData, ... */
int MVCommonDataLoader::loadDataFile(	int Type,  ///"Hits", "Digits", "Positions", "Segements", "Preprod"
					std::string FileName,
					int versionID,		/// Do you load the first file => 1, Do you load the 2nd File =>2
					map<long, MVCommonStationData> &mapMdtStationData,
					map<long, MVCommonStationData> &mapRpcStationData,
					map<long, MVCommonStationData> &mapCscStationData,
					map<long, MVCommonStationData> &mapTgcStationData)
{
  loadMdtDataFile(Type, FileName, versionID, mapMdtStationData);
  loadRpcDataFile(Type, FileName, versionID, mapRpcStationData);
  loadCscDataFile(Type, FileName, versionID, mapCscStationData);
  loadTgcDataFile(Type, FileName, versionID, mapTgcStationData);
  return 0;
}


int MVCommonDataLoader::loadMdtDataFile(int Type, // "Hits", "Digits", "Positions", "Segements", "Preprod"
					std::string FileName,
					int versionID,		/// Do you load the first file => 1, Do you load the 2nd File =>2
					map<long, MVCommonStationData> &mapMdtStationData)
{
  MVCommonStationID vtStationID("MDT");
  map<long ,MVCommonDataDescription> map_vtStationData;
  vector<long> stationIDs;
  int nNumberOfEvents;
  long j, id;

  std::cout<< "Loading " << Type << "-File: "<<FileName<<std::endl;

  if ((Type==1) || (Type==2))
    {
      TChain* m_tree;

      //if (Type==1) m_tree = new TChain("MDTHitsValidation/MDTTestHits");
      //if (Type==2)
      if (Type==1)	m_tree = new TChain("MDTHitsValidation/MDTTestHits");
      else	m_tree = new TChain("MDTDigitsValidation/MDTTestDigits");

      m_tree->Add(FileName.c_str());

      nNumberOfEvents = static_cast<int>(m_tree->GetEntries());
      if (nNumberOfEvents<=0)
	{
	  std::cout << "File is empty | corrupted | does not exist!"<< std::endl;
	  return -1;
	}

      m_tree->SetBranchAddress("m_Validation_MDT_RunNumber", &m_Validation_MDT_RunNumber);
      m_tree->SetBranchAddress("m_Validation_MDT_EventNumber", &m_Validation_MDT_EventNumber);
      if (Type==1)
	{
	  m_tree->SetBranchAddress("m_Validation_MDT_NumberOfHits", &m_Validation_MDT_NumberOfHits);
	  m_tree->SetBranchAddress("m_Validation_MDT_StationName", &m_Validation_MDT_StationNameInt);
	}
      else
	{
	  m_tree->SetBranchAddress("m_Validation_MDT_NumberOfDigits", &m_Validation_MDT_NumberOfHits);
	  m_tree->SetBranchAddress("m_Validation_MDT_StationName",&m_Validation_MDT_StationNameInt);
	}
      m_tree->SetBranchAddress("m_Validation_MDT_StationEta", &m_Validation_MDT_StationEta);
      m_tree->SetBranchAddress("m_Validation_MDT_StationPhi", &m_Validation_MDT_StationPhi);
      if (Type==1)
	{
	  m_tree->SetBranchAddress("m_Validation_MDT_HitX", &m_Validation_MDT_HitX);
	  m_tree->SetBranchAddress("m_Validation_MDT_HitY", &m_Validation_MDT_HitY);
	  m_tree->SetBranchAddress("m_Validation_MDT_HitZ", &m_Validation_MDT_HitZ);
	} else
	  {
	    m_tree->SetBranchAddress("m_Validation_MDT_WireX", &m_Validation_MDT_HitX);
	    m_tree->SetBranchAddress("m_Validation_MDT_WireY", &m_Validation_MDT_HitY);
	    m_tree->SetBranchAddress("m_Validation_MDT_WireZ", &m_Validation_MDT_HitZ);
	  }
      m_tree->SetBranchAddress("m_Validation_MDT_Driftradius", &m_Validation_MDT_HitR);
      m_tree->SetBranchAddress("m_Validation_MDT_IDTube", &m_Validation_MDT_IDTube);
      if (Type==1)	m_tree->SetBranchAddress("m_Validation_MDT_IDMultiLayer", &m_Validation_MDT_IDMultiLayer);
      else m_tree->SetBranchAddress("m_Validation_MDT_IDMultilayer", &m_Validation_MDT_IDMultiLayer);
      m_tree->SetBranchAddress("m_Validation_MDT_IDLayer", &m_Validation_MDT_IDLayer);

      int oldEventNumber=0;
      j=0;
      while(j < nNumberOfEvents)
	{
	  // Getting Event

	  if (j==0)
	    {
	      m_tree->GetEvent(j);
	      oldEventNumber=m_Validation_MDT_EventNumber;
	    }

	  stationIDs.clear();
	  map_vtStationData.clear();

	  while (oldEventNumber==m_Validation_MDT_EventNumber)
	    {
	      if (Type==1)
		{
		  id = vtStationID.getLongIDNumber(	m_Validation_MDT_StationNameInt,
							m_Validation_MDT_StationEta,
							m_Validation_MDT_StationPhi);
		} else
		  {
		    id = vtStationID.getLongIDNumber(	m_Validation_MDT_StationNameInt,
							m_Validation_MDT_StationEta,
							m_Validation_MDT_StationPhi);
		  }

	      stationIDs.push_back(id);

	      map_vtStationData[id].setID(id);
	      if (m_exludeDoubleCounting==true) map_vtStationData[id].setDoubleCounting(true);
	      map_vtStationData[id].addData(
					    (int) m_Validation_MDT_IDTube,
					    (int) m_Validation_MDT_IDMultiLayer,
					    (int) m_Validation_MDT_IDLayer,
					    (float) m_Validation_MDT_HitX,
					    (float) m_Validation_MDT_HitY,
					    (float) m_Validation_MDT_HitZ,
					    (float) m_Validation_MDT_HitR
					    );
	      if (j<nNumberOfEvents)
		{
		  j++;
		  m_tree->GetEvent(j);
		} else
		  {
		    break;
		  }
	    }
	  map<long ,MVCommonDataDescription>::iterator iter;
	  for(iter = map_vtStationData.begin(); iter != map_vtStationData.end(); iter++)
	    {
	      id=(*iter).first;

	      if (versionID==1)	mapMdtStationData[id].setVersion1Description(oldEventNumber, map_vtStationData[id]);
	      if (versionID==2)	mapMdtStationData[id].setVersion2Description(oldEventNumber, map_vtStationData[id]);
	      if (versionID==1)	m_vecVersion1_mdt.push_back(map_vtStationData[id]);
	      if (versionID==2)	m_vecVersion2_mdt.push_back(map_vtStationData[id]);
	    }
	  oldEventNumber=m_Validation_MDT_EventNumber;
	}

      delete m_tree;
    }
  return 0;
}



int MVCommonDataLoader::loadRpcDataFile(int Type, // "Hits", "Digits", "Positions", "Segements", "Preprod"
					std::string FileName,
					int versionID,		/// Do you load the first file => 1, Do you load the 2nd File =>2
					map<long, MVCommonStationData> &mapRpcStationData)
{
  MVCommonStationID vtStationID("RPC");
  map<long ,MVCommonDataDescription> map_vtStationData;
  vector<long> stationIDs;
  int nNumberOfEvents;
  long j, id;

  std::cout<< "Loading " << Type << "-File: "<<FileName<<std::endl;

  if ((Type==1) || (Type==2))
    {
      TChain* m_tree;

      if (Type==1)	m_tree = new TChain("RPCHitsValidation/RPCTestHits");
      else	m_tree = new TChain("RPCDigitsValidation/RPCTestDigits");

      m_tree->Add(FileName.c_str());

      nNumberOfEvents = static_cast<int>(m_tree->GetEntries());
      if (nNumberOfEvents<=0)
	{
	  std::cout << "File is empty | corrupted | does not exist!"<< std::endl;
	  return -1;
	}

      m_tree->SetBranchAddress("m_Validation_RPC_RunNumber", &m_Validation_RPC_RunNumber);
      m_tree->SetBranchAddress("m_Validation_RPC_EventNumber", &m_Validation_RPC_EventNumber);
      if (Type==1)
	{
	  m_tree->SetBranchAddress("m_Validation_RPC_NumberOfHits", &m_Validation_RPC_NumberOfHits);
	  m_tree->SetBranchAddress("m_Validation_RPC_StationName", &m_Validation_RPC_StationNameInt);
	} else
	  {
	    m_tree->SetBranchAddress("m_Validation_RPC_NumberOfDigits", &m_Validation_RPC_NumberOfHits);
	    m_tree->SetBranchAddress("m_Validation_RPC_StationName", &m_Validation_RPC_StationNameInt);
	  }
      m_tree->SetBranchAddress("m_Validation_RPC_StationEta", &m_Validation_RPC_StationEta);
      m_tree->SetBranchAddress("m_Validation_RPC_StationPhi", &m_Validation_RPC_StationPhi);
      m_tree->SetBranchAddress("m_Validation_RPC_DoubletR", &m_Validation_RPC_DoubletR);
      m_tree->SetBranchAddress("m_Validation_RPC_DoubletZ", &m_Validation_RPC_DoubletZ);
      m_tree->SetBranchAddress("m_Validation_RPC_DoubletPhi", &m_Validation_RPC_DoubletPhi);
      m_tree->SetBranchAddress("m_Validation_RPC_DoubletGasGap", &m_Validation_RPC_DoubletGasGap);
      m_tree->SetBranchAddress("m_Validation_RPC_MeasuresPhi", &m_Validation_RPC_MeasuresPhi);
      m_tree->SetBranchAddress("m_Validation_RPC_Strip", &m_Validation_RPC_Strip);
      if (Type==1)
	{
	  m_tree->SetBranchAddress("m_Validation_RPC_HitX", &m_Validation_RPC_HitX);
	  m_tree->SetBranchAddress("m_Validation_RPC_HitY", &m_Validation_RPC_HitY);
	  m_tree->SetBranchAddress("m_Validation_RPC_HitZ", &m_Validation_RPC_HitZ);
	} else
	  {
	    m_tree->SetBranchAddress("m_Validation_RPC_StripX", &m_Validation_RPC_HitX);
	    m_tree->SetBranchAddress("m_Validation_RPC_StripY", &m_Validation_RPC_HitY);
	    m_tree->SetBranchAddress("m_Validation_RPC_StripZ", &m_Validation_RPC_HitZ);
	  }
      m_tree->SetBranchAddress("m_Validation_RPC_Stripdistance", &m_Validation_RPC_HitR);

      int oldEventNumber=0;
      j=0;
      while(j < nNumberOfEvents)
	{

	  if (j==0)
	    {
	      m_tree->GetEvent(j);
	      oldEventNumber=m_Validation_RPC_EventNumber;
	    }
	  // Getting Event

	  stationIDs.clear();
	  map_vtStationData.clear();
	  //if (j%1000==0)
	  while (oldEventNumber==m_Validation_RPC_EventNumber)
	    {
	      if (Type==1)
		{
		  id = vtStationID.getLongIDNumber(	m_Validation_RPC_StationNameInt,
							m_Validation_RPC_StationEta,
							m_Validation_RPC_StationPhi);
		} else
		  {
		    id = vtStationID.getLongIDNumber(	m_Validation_RPC_StationNameInt,
							m_Validation_RPC_StationEta,
							m_Validation_RPC_StationPhi);
		  }
	      stationIDs.push_back(id);


	      map_vtStationData[id].setID(id);
	      if (m_exludeDoubleCounting==true) map_vtStationData[id].setDoubleCounting(true);
	      map_vtStationData[id].addData(
					    (int) m_Validation_RPC_DoubletR,
					    (int) m_Validation_RPC_DoubletZ,
					    (int) m_Validation_RPC_DoubletPhi,
					    (int) m_Validation_RPC_DoubletGasGap,
					    (int) m_Validation_RPC_MeasuresPhi,
					    (int) m_Validation_RPC_Strip,
					    (float) m_Validation_RPC_HitX,
					    (float) m_Validation_RPC_HitY,
					    (float) m_Validation_RPC_HitZ,
					    (float) m_Validation_RPC_HitR
					    );
	      if (j<nNumberOfEvents)
		{
		  j++;
		  m_tree->GetEvent(j);
		} else break;
	    }

	  map<long ,MVCommonDataDescription>::iterator iter;
	  for(iter = map_vtStationData.begin(); iter != map_vtStationData.end(); iter++)
	    {
	      id=(*iter).first;
	      if (versionID==1)	mapRpcStationData[id].setVersion1Description(oldEventNumber, map_vtStationData[id]);
	      if (versionID==2)	mapRpcStationData[id].setVersion2Description(oldEventNumber, map_vtStationData[id]);
	      if (versionID==1)	m_vecVersion1_rpc.push_back(map_vtStationData[id]);
	      if (versionID==2)	m_vecVersion2_rpc.push_back(map_vtStationData[id]);
	    }

	  oldEventNumber=m_Validation_RPC_EventNumber;
	}

      delete m_tree;
    }
  return 0;
}


int MVCommonDataLoader::loadCscDataFile(int Type, // "Hits", "Digits", "Positions", "Segements", "Preprod"
					std::string FileName,
					int versionID,		/// Do you load the first file => 1, Do you load the 2nd File =>2
					map<long, MVCommonStationData> &mapCscStationData)
{
  MVCommonStationID vtStationID("CSC");
  map<long ,MVCommonDataDescription> map_vtStationData;
  vector<long> stationIDs;
  int nNumberOfEvents;
  long j, id;

  std::cout<< "Loading " << Type << "-File: "<<FileName<<std::endl;

  if ((Type==1) || (Type==2))
    {
      TChain* m_tree;

      if (Type==1)	m_tree = new TChain("CSCHitsValidation/CSCTestHits");
      else	m_tree = new TChain("CSCDigitsValidation/CSCTestDigits");

      m_tree->Add(FileName.c_str());

      nNumberOfEvents = static_cast<int>(m_tree->GetEntries());
      if (nNumberOfEvents<=0)
	{
	  std::cout << "File is empty | corrupted | does not exist!"<< std::endl;
	  return -1;
	}


      m_tree->SetBranchAddress("m_Validation_CSC_RunNumber", &m_Validation_CSC_RunNumber);
      m_tree->SetBranchAddress("m_Validation_CSC_EventNumber", &m_Validation_CSC_EventNumber);
      if (Type==1)
	{
	  m_tree->SetBranchAddress("m_Validation_CSC_NumberOfHits", &m_Validation_CSC_NumberOfHits);
	  m_tree->SetBranchAddress("m_Validation_CSC_StationName", &m_Validation_CSC_StationNameInt);
	} else
	  {
	    m_tree->SetBranchAddress("m_Validation_CSC_NumberOfDigits", &m_Validation_CSC_NumberOfHits);
	    m_tree->SetBranchAddress("m_Validation_CSC_StationName", &m_Validation_CSC_StationNameInt);
	  }

      m_tree->SetBranchAddress("m_Validation_CSC_StationEta", &m_Validation_CSC_StationEta);
      m_tree->SetBranchAddress("m_Validation_CSC_StationPhi", &m_Validation_CSC_StationPhi);
      m_tree->SetBranchAddress("m_Validation_CSC_ChamberLayer", &m_Validation_CSC_ChamberLayer);
      m_tree->SetBranchAddress("m_Validation_CSC_WireLayer", &m_Validation_CSC_WireLayer);
      m_tree->SetBranchAddress("m_Validation_CSC_MeasuresPhi", &m_Validation_CSC_MeasuresPhi);
      m_tree->SetBranchAddress("m_Validation_CSC_Strip", &m_Validation_CSC_Strip);
      if (Type==1)
	{
	  m_tree->SetBranchAddress("m_Validation_CSC_HitX", &m_Validation_CSC_HitX);
	  m_tree->SetBranchAddress("m_Validation_CSC_HitY", &m_Validation_CSC_HitY);
	  m_tree->SetBranchAddress("m_Validation_CSC_HitZ", &m_Validation_CSC_HitZ);
	} else
	  {
	    m_tree->SetBranchAddress("m_Validation_CSC_PosX", &m_Validation_CSC_HitX);
	    m_tree->SetBranchAddress("m_Validation_CSC_PosY", &m_Validation_CSC_HitY);
	    m_tree->SetBranchAddress("m_Validation_CSC_PosZ", &m_Validation_CSC_HitZ);
	  }
      //m_tree->SetBranchAddress("m_Validation_CSC_Charge", &);

      int oldEventNumber=0;
      j=0;
      while(j < nNumberOfEvents)
	{
	  if (j==0)
	    {
	      m_tree->GetEvent(j);
	      oldEventNumber=m_Validation_CSC_EventNumber;
	    }

	  // Getting Event

	  stationIDs.clear();
	  map_vtStationData.clear();
	  while (oldEventNumber==m_Validation_CSC_EventNumber)
	    {
	      if (Type==1)
		{
		  id = vtStationID.getLongIDNumber(	m_Validation_CSC_StationNameInt,
							m_Validation_CSC_StationEta,
							m_Validation_CSC_StationPhi);
		} else
		  {
		    id = vtStationID.getLongIDNumber(	m_Validation_CSC_StationNameInt,
							m_Validation_CSC_StationEta,
							m_Validation_CSC_StationPhi);
		  }
	      stationIDs.push_back(id);

	      map_vtStationData[id].setID(id);
	      if (m_exludeDoubleCounting==true) map_vtStationData[id].setDoubleCounting(true);
	      map_vtStationData[id].addData(
					    (int) m_Validation_CSC_ChamberLayer,
					    (int) m_Validation_CSC_WireLayer,
					    (int) m_Validation_CSC_MeasuresPhi,
					    (int) m_Validation_CSC_Strip,
					    (float) m_Validation_CSC_HitX,
					    (float) m_Validation_CSC_HitY,
					    (float) m_Validation_CSC_HitZ,
					    (float) 0.0
					    );
	      if (j<nNumberOfEvents)
		{
		  j++;
		  m_tree->GetEvent(j);
		} else break;
	    }


	  map<long ,MVCommonDataDescription>::iterator iter;
	  for(iter = map_vtStationData.begin(); iter != map_vtStationData.end(); iter++)
	    {
	      id=(*iter).first;
	      if (versionID==1)	mapCscStationData[id].setVersion1Description(oldEventNumber, map_vtStationData[id]);
	      if (versionID==2)	mapCscStationData[id].setVersion2Description(oldEventNumber, map_vtStationData[id]);
	      if (versionID==1)	m_vecVersion1_csc.push_back(map_vtStationData[id]);
	      if (versionID==2)	m_vecVersion2_csc.push_back(map_vtStationData[id]);
	    }
	  oldEventNumber=m_Validation_CSC_EventNumber;
	}

      delete m_tree;
    }
  return 0;
}


int MVCommonDataLoader::loadTgcDataFile(int Type, // "Hits", "Digits", "Positions", "Segements", "Preprod"
					std::string FileName,
					int versionID,		/// Do you load the first file => 1, Do you load the 2nd File =>2
					map<long, MVCommonStationData> &mapTgcStationData)
{
  MVCommonStationID vtStationID("TGC");
  map<long ,MVCommonDataDescription> map_vtStationData;
  vector<long> stationIDs;
  int nNumberOfEvents;
  long j, id;

  std::cout<< "Loading " << Type << "-File: "<<FileName<<std::endl;

  if ((Type==1) || (Type==2))
    {
      TChain* m_tree;

      if (Type==1)	m_tree = new TChain("TGCHitsValidation/TGCTestHits");
      else	m_tree = new TChain("TGCDigitsValidation/TGCTestDigits");

      m_tree->Add(FileName.c_str());

      nNumberOfEvents = static_cast<int>(m_tree->GetEntries());
      if (nNumberOfEvents<=0)
	{
	  std::cout << "File is empty | corrupted | does not exist!"<< std::endl;
	  return -1;
	}

      m_tree->SetBranchAddress("m_Validation_TGC_RunNumber", &m_Validation_TGC_RunNumber);
      m_tree->SetBranchAddress("m_Validation_TGC_EventNumber", &m_Validation_TGC_EventNumber);
      if (Type==1)
	{
	  m_tree->SetBranchAddress("m_Validation_TGC_NumberOfHits", &m_Validation_TGC_NumberOfHits);
	  m_tree->SetBranchAddress("m_Validation_TGC_StationName", &m_Validation_TGC_StNameInt);
	} else
	  {
	    m_tree->SetBranchAddress("m_Validation_TGC_NumberOfDigits", &m_Validation_TGC_NumberOfHits);
	    m_tree->SetBranchAddress("m_Validation_TGC_StationName", &m_Validation_TGC_StNameInt);
	  }

      m_tree->SetBranchAddress("m_Validation_TGC_StationEta", &m_Validation_TGC_StationEta);
      m_tree->SetBranchAddress("m_Validation_TGC_StationPhi", &m_Validation_TGC_StationPhi);
      m_tree->SetBranchAddress("m_Validation_TGC_GasGap", &m_Validation_TGC_GasGap);
      m_tree->SetBranchAddress("m_Validation_TGC_IsStrip", &m_Validation_TGC_IsStrip);
      m_tree->SetBranchAddress("m_Validation_TGC_Channel", &m_Validation_TGC_Channel);
      if (Type==1)
	{
	  m_tree->SetBranchAddress("m_Validation_TGC_HitX", &m_Validation_TGC_HitX);
	  m_tree->SetBranchAddress("m_Validation_TGC_HitY", &m_Validation_TGC_HitY);
	  m_tree->SetBranchAddress("m_Validation_TGC_HitZ", &m_Validation_TGC_HitZ);
	} else
	  {
	    m_tree->SetBranchAddress("m_Validation_TGC_ChannelPosX", &m_Validation_TGC_HitX);
	    m_tree->SetBranchAddress("m_Validation_TGC_ChannelPosY", &m_Validation_TGC_HitY);
	    m_tree->SetBranchAddress("m_Validation_TGC_ChannelPosZ", &m_Validation_TGC_HitZ);
	  }
      int oldEventNumber=0;
      j=0;
      while(j < nNumberOfEvents)
	{
	  if (j==0)
	    {
	      m_tree->GetEvent(j);
	      oldEventNumber=m_Validation_TGC_EventNumber;
	    }

	  // Getting Event
	  stationIDs.clear();
	  map_vtStationData.clear();

	  while (oldEventNumber==m_Validation_TGC_EventNumber)
	    {
	      if (Type==1)
		{
		  id = vtStationID.getLongIDNumber(	m_Validation_TGC_StNameInt,
							m_Validation_TGC_StationEta,
							m_Validation_TGC_StationPhi);
		} else
		  {
		    id = vtStationID.getLongIDNumber(	m_Validation_TGC_StNameInt,
							m_Validation_TGC_StationEta,
							m_Validation_TGC_StationPhi);
		  }
	      stationIDs.push_back(id);

	      map_vtStationData[id].setID(id);
	      if (m_exludeDoubleCounting==true) map_vtStationData[id].setDoubleCounting(true);
	      map_vtStationData[id].addData(		(int) m_Validation_TGC_GasGap,
							(int) m_Validation_TGC_IsStrip,
							(int) m_Validation_TGC_Channel,
							(float) m_Validation_TGC_HitX,
							(float) m_Validation_TGC_HitY,
							(float) m_Validation_TGC_HitZ);
	      if (j<nNumberOfEvents)
		{
		  j++;
		  m_tree->GetEvent(j);
		} else break;

	    }

	  map<long ,MVCommonDataDescription>::iterator iter;
	  for(iter = map_vtStationData.begin(); iter != map_vtStationData.end(); iter++)
	    {
	      id=(*iter).first;
	      if (versionID==1)	mapTgcStationData[id].setVersion1Description(oldEventNumber, map_vtStationData[id]);
	      if (versionID==2)	mapTgcStationData[id].setVersion2Description(oldEventNumber, map_vtStationData[id]);
	      if (versionID==1)	m_vecVersion1_tgc.push_back(map_vtStationData[id]);
	      if (versionID==2)	m_vecVersion2_tgc.push_back(map_vtStationData[id]);
	    }
	  oldEventNumber=m_Validation_TGC_EventNumber;
	}

      delete m_tree;
    }
  return 0;
}




/// Setting up the counter for the next routine
void MVCommonDataLoader::startup()
{
  lAktEvent_mdt1	= 0;
  lAktEvent_mdt2	= 0;
  lAktEvent_rpc1	= 0;
  lAktEvent_rpc2	= 0;
  lAktEvent_csc1	= 0;
  lAktEvent_csc2	= 0;
  lAktEvent_tgc1	= 0;
  lAktEvent_tgc2	= 0;
}

/// Gives back event by event the MDT-Data - NOT chamberwise !

bool MVCommonDataLoader::getNext_Data(	std::string s_identifier, MVCommonDataDescription &mvVersion)
{
  if ((s_identifier=="MDT_Version1") && (lAktEvent_mdt1<(int)(m_vecVersion1_mdt.size())))
    {
      mvVersion	= m_vecVersion1_mdt[lAktEvent_mdt1];
      lAktEvent_mdt1++;
      return true;
    }
  if ((s_identifier=="MDT_Version2") && (lAktEvent_mdt2<(int)(m_vecVersion2_mdt.size())))
    {
      mvVersion	= m_vecVersion2_mdt[lAktEvent_mdt2];
      lAktEvent_mdt2++;
      return true;
    }

  if ((s_identifier=="RPC_Version1") && (lAktEvent_rpc1<(int)(m_vecVersion1_rpc.size())))
    {
      mvVersion	= m_vecVersion1_rpc[lAktEvent_rpc1];
      lAktEvent_rpc1++;
      return true;
    }
  if ((s_identifier=="RPC_Version2") && (lAktEvent_rpc2<(int)(m_vecVersion2_rpc.size())))
    {
      mvVersion	= m_vecVersion2_rpc[lAktEvent_rpc2];
      lAktEvent_rpc2++;
      return true;
    }

  if ((s_identifier=="CSC_Version1") && (lAktEvent_csc1<(int)(m_vecVersion1_csc.size())))
    {
      mvVersion	= m_vecVersion1_csc[lAktEvent_csc1];
      lAktEvent_csc1++;
      return true;
    }
  if ((s_identifier=="CSC_Version2") && (lAktEvent_csc2<(int)(m_vecVersion2_csc.size())))
    {
      mvVersion	= m_vecVersion2_csc[lAktEvent_csc2];
      lAktEvent_csc2++;
      return true;
    }

  if ((s_identifier=="TGC_Version1") && (lAktEvent_tgc1<(int)(m_vecVersion1_tgc.size())))
    {
      mvVersion	= m_vecVersion1_tgc[lAktEvent_tgc1];
      lAktEvent_tgc1++;
      return true;
    }
  if ((s_identifier=="TGC_Version2") && (lAktEvent_tgc2<(int)(m_vecVersion2_tgc.size())))
    {
      mvVersion	= m_vecVersion2_tgc[lAktEvent_tgc2];
      lAktEvent_tgc2++;
      return true;
    }

  return false;
}
