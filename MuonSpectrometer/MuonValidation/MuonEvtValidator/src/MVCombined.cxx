/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Name    : MVCombined.cxx
// Package : MuonValidation
// Authors: M.Schott (LMU) - N. Benekos (MPI)
// CERN, MAY 1, 2006
//
// DESCRIPTION:
//
// This class combines all results of the single MuonValidation results like Hits- or Digits-collections
//
///////////////////////////////////////////////////////////////////////////////////////////////////////


#include "MuonEvtValidator/MVCombined.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonEvtValidator/MVCommonResultDescription.h"
/////////////////////////////////////////////////////////////////////////////

MVCombined::MVCombined(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),n_EventCounter(0),m_storeGate(0),m_rootsvc(0)

{
  declareProperty("OutputFile",            m_OutputFile = "MVCommon.root");
  declareProperty("HistogramNameVersion1", m_Version1Name = "Hits");
  declareProperty("HistogramNameVersion2", m_Version2Name = "Digits");
  declareProperty("FileTyp1",              m_FileType1 = "Hits");
  declareProperty("FileTyp2",              m_FileType2 = "Digits");
  declareProperty("InputVersion1File",     m_InputVersion1File = "../InputFiles/ExampleHits.root");
  declareProperty("InputVersion2File",     m_InputVersion2File = "../InputFiles/ExampleDigits.root");
  declareProperty("InputPositioningFile",  m_InputPositioningFile = "../InputFiles/ExamplePositioningFile.root");
  declareProperty("InputPreprodDataFile",  m_InputPreprodDataFile = "../InputFiles/ExamplePreprodDataFile.root");
  declareProperty("InputSegmentsFile",     m_InputSegmentsFile = "../InputFiles/ExampleSegmentsFile.root");
  declareProperty("MinimalDistanceForHitIdentification", m_minimalDifference = 0);
  declareProperty("ExcludeDoubleCounting", m_excludeDoubleCounting = false);
  declareProperty("SpecialStation_NameID", m_SpecialStationNameID = -10);
  declareProperty("SpecialStation_Eta",    m_SpecialStationEta = -10);
  declareProperty("SpecialStation_Phi",    m_SpecialStationPhi = -10);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode MVCombined::initialize()
{

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;

  vtLoader.startup();

  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure())
    {
      log << MSG::ERROR << "Unable to retrieve pointer to StoreGateSvc" << endreq;
      return sc;
    }
  TString treeName;
  TString title;

  //std::string mdttreePath = "/MuonValidationTeam/MDTDigitsValidation/MDTTestDigits";
  //sc=ToolRootHistSvc()->getTree(mdttreePath, mdttree);

  m_2D_Histos[m_Version1Name+"MDT_ZR-Plane_complete"]=histoSvc()->book("/stat/MDT",m_Version1Name+"_MDT_ZR-Plane_complete",m_Version1Name+"_MDT_ZR-Plane_complete",100,-24000,24000,100,-14000,14000);
  m_2D_Histos[m_Version1Name+"MDT_XY-Plane_endcap"]=histoSvc()->book("/stat/MDT",m_Version1Name+"_MDT_XY-Plane_endcap",m_Version1Name+"_MDT_XY-Plane_endcap",100,-14000,14000,100,-14000,14000);
  m_2D_Histos[m_Version1Name+"MDT_XY-Plane_barrel"]=histoSvc()->book("/stat/MDT",m_Version1Name+"_MDT_XY-Plane_barrel",m_Version1Name+"_MDT_XY-Plane_barrel",100,-14000,14000,100,-14000,14000);
  m_2D_Histos[m_Version1Name+"RPC_ZR-Plane_barrel"]=histoSvc()->book("/stat/RPC",m_Version1Name+"_RPC_ZR-Plane_barrel",m_Version1Name+"_RPC_ZR-Plane_barrel",100,-14000,14000,100,-12000,12000);
  m_2D_Histos[m_Version1Name+"RPC_XY-Plane_barrel"]=histoSvc()->book("/stat/RPC",m_Version1Name+"_RPC_XY-Plane_barrel",m_Version1Name+"_RPC_XY-Plane_barrel",100,-12000,12000,100,-12000,12000);
  m_2D_Histos[m_Version1Name+"CSC_ZR-Plane_endcap"]=histoSvc()->book("/stat/CSC",m_Version1Name+"_CSC_ZR-Plane_endcap",m_Version1Name+"_CSC_ZR-Plane_endcap",50,-9000,9000,50,-3000,3000);
  m_2D_Histos[m_Version1Name+"CSC_XY-Plane_endcap"]=histoSvc()->book("/stat/CSC",m_Version1Name+"_CSC_XY-Plane_endcap",m_Version1Name+"_CSC_XY-Plane_endcap",50,-3000,3000,50,-3000,3000);
  m_2D_Histos[m_Version1Name+"TGC_ZR-Plane_endcap"]=histoSvc()->book("/stat/TGC",m_Version1Name+"_TGC_ZR-Plane_endcap",m_Version1Name+"_TGC_ZR-Plane_endcap",100,-16000,16000,100,-13000,13000);
  m_2D_Histos[m_Version1Name+"TGC_XY-Plane_endcap"]=histoSvc()->book("/stat/TGC",m_Version1Name+"_TGC_XY-Plane_endcap",m_Version1Name+"_TGC_XY-Plane_endcap",100,-13000,13000,100,-13000,13000);
  m_2D_Histos[m_Version2Name+"MDT_ZR-Plane_complete"]=histoSvc()->book("/stat/MDT",m_Version2Name+"_MDT_ZR-Plane_complete",m_Version2Name+"_MDT_ZR-Plane_complete",100,-24000,24000,100,-14000,14000);
  m_2D_Histos[m_Version2Name+"MDT_XY-Plane_endcap"]=histoSvc()->book("/stat/MDT",m_Version2Name+"_MDT_XY-Plane_endcap",m_Version2Name+"_MDT_XY-Plane_endcap",100,-14000,14000,100,-14000,14000);
  m_2D_Histos[m_Version2Name+"MDT_XY-Plane_barrel"]=histoSvc()->book("/stat/MDT",m_Version2Name+"_MDT_XY-Plane_barrel",m_Version2Name+"_MDT_XY-Plane_barrel",100,-14000,14000,100,-14000,14000);
  m_2D_Histos[m_Version2Name+"RPC_ZR-Plane_barrel"]=histoSvc()->book("/stat/RPC",m_Version2Name+"_RPC_ZR-Plane_barrel",m_Version2Name+"_RPC_ZR-Plane_barrel",100,-14000,14000,100,-12000,12000);
  m_2D_Histos[m_Version2Name+"RPC_XY-Plane_barrel"]=histoSvc()->book("/stat/RPC",m_Version2Name+"_RPC_XY-Plane_barrel",m_Version2Name+"_RPC_XY-Plane_barrel",100,-12000,12000,100,-12000,12000);
  m_2D_Histos[m_Version2Name+"CSC_ZR-Plane_endcap"]=histoSvc()->book("/stat/CSC",m_Version2Name+"_CSC_ZR-Plane_endcap",m_Version2Name+"_CSC_ZR-Plane_endcap",50,-9000,9000,50,-3000,3000);
  m_2D_Histos[m_Version2Name+"CSC_XY-Plane_endcap"]=histoSvc()->book("/stat/CSC",m_Version2Name+"_CSC_XY-Plane_endcap",m_Version2Name+"_CSC_XY-Plane_endcap",50,-3000,3000,50,-3000,3000);
  m_2D_Histos[m_Version2Name+"TGC_ZR-Plane_endcap"]=histoSvc()->book("/stat/TGC",m_Version2Name+"_TGC_ZR-Plane_endcap",m_Version2Name+"_TGC_ZR-Plane_endcap",100,-16000,16000,100,-13000,13000);
  m_2D_Histos[m_Version2Name+"TGC_XY-Plane_endcap"]=histoSvc()->book("/stat/TGC",m_Version2Name+"_TGC_XY-Plane_endcap",m_Version2Name+"_TGC_XY-Plane_endcap",100,-13000,13000,100,-13000,13000);


  msRootInterface.setNewGraph("MDT_Average_Phi_1", m_Version1Name.c_str(), "#phi", "Average Number of Hits");
  msRootInterface.setNewGraph("MDT_Average_Phi_2", m_Version2Name.c_str(), "#phi", "Average Number of Hits");
  msRootInterface.setNewGraph("MDT_Average_Eta_1", m_Version1Name.c_str(), "#eta", "Average Number of Hits");
  msRootInterface.setNewGraph("MDT_Average_Eta_2", m_Version2Name.c_str(), "#eta", "Average Number of Hits");
  msRootInterface.setNewGraph("MDT_Average_Name_1", m_Version1Name.c_str(), "Station Name", "Average Number of Hits");
  msRootInterface.setNewGraph("MDT_Average_Name_2", m_Version2Name.c_str(), "Station Name", "Average Number of Hits");
  msRootInterface.setNewGraph("MDT_Average_UniqueID_1", m_Version1Name.c_str(), "Unique ID Number", "Average Number of Hits");
  msRootInterface.setNewGraph("MDT_Average_UniqueID_2", m_Version2Name.c_str(), "Unique ID Number", "Average Number of Hits");

  msRootInterface.setNewGraph("MDT_AssoziatedHits_Eta_1to2", "MDT_AssoziatedHits_Eta_1to2", "#eta", "Ratio of Assoziated Hits");
  msRootInterface.setNewGraph("MDT_AssoziatedHits_Eta_2to1", "MDT_AssoziatedHits_Eta_2to1", "#eta", "Ratio of Assoziated Hits");
  msRootInterface.setNewGraph("MDT_AssoziatedHits_Name_1to2", "MDT_AssoziatedHits_Name_1to2", "Station ID Number", "Ratio of Assoziated Hits");
  msRootInterface.setNewGraph("MDT_AssoziatedHits_Name_2to1", "MDT_AssoziatedHits_Name_2to1", "Station ID Number", "Ratio of Assoziated Hits");

  msRootInterface.setNewGraph("RPC_Average_Phi_1", m_Version1Name.c_str(), "#phi", "Average Number of Hits");
  msRootInterface.setNewGraph("RPC_Average_Phi_2", m_Version2Name.c_str(), "#phi", "Average Number of Hits");
  msRootInterface.setNewGraph("RPC_Average_Eta_1", m_Version1Name.c_str(), "#eta", "Average Number of Hits");
  msRootInterface.setNewGraph("RPC_Average_Eta_2", m_Version2Name.c_str(), "#eta", "Average Number of Hits");
  msRootInterface.setNewGraph("RPC_Average_Name_1", m_Version1Name.c_str(), "Station ID Number", "Average Number of Hits");
  msRootInterface.setNewGraph("RPC_Average_Name_2", m_Version2Name.c_str(), "Station ID Number", "Average Number of Hits");
  msRootInterface.setNewGraph("RPC_Average_UniqueID_1", m_Version1Name.c_str(), "Unique ID Number", "Average Number of Hits");
  msRootInterface.setNewGraph("RPC_Average_UniqueID_2", m_Version2Name.c_str(), "Unique ID Number", "Average Number of Hits");

  msRootInterface.setNewGraph("CSC_Average_Phi_1", m_Version1Name.c_str(), "#phi", "Average Number of Hits");
  msRootInterface.setNewGraph("CSC_Average_Phi_2", m_Version2Name.c_str(), "#phi", "Average Number of Hits");
  msRootInterface.setNewGraph("CSC_Average_Eta_1", m_Version1Name.c_str(), "#eta", "Average Number of Hits");
  msRootInterface.setNewGraph("CSC_Average_Eta_2", m_Version2Name.c_str(), "#eta", "Average Number of Hits");
  msRootInterface.setNewGraph("CSC_Average_Name_1", m_Version1Name.c_str(), "Station ID Number", "Average Number of Hits");
  msRootInterface.setNewGraph("CSC_Average_Name_2", m_Version2Name.c_str(), "Station ID Number", "Average Number of Hits");
  msRootInterface.setNewGraph("CSC_Average_UniqueID_1", m_Version1Name.c_str(), "Unique ID Number", "Average Number of Hits");
  msRootInterface.setNewGraph("CSC_Average_UniqueID_2", m_Version2Name.c_str(), "Unique ID Number", "Average Number of Hits");

  msRootInterface.setNewGraph("TGC_Average_Phi_1", m_Version1Name.c_str(), "#phi", "Average Number of Hits");
  msRootInterface.setNewGraph("TGC_Average_Phi_2", m_Version2Name.c_str(), "#phi", "Average Number of Hits");
  msRootInterface.setNewGraph("TGC_Average_Eta_1", m_Version1Name.c_str(), "#eta", "Average Number of Hits");
  msRootInterface.setNewGraph("TGC_Average_Eta_2", m_Version2Name.c_str(), "#eta", "Average Number of Hits");
  msRootInterface.setNewGraph("TGC_Average_Name_1", m_Version1Name.c_str(), "Station ID Number", "Average Number of Hits");
  msRootInterface.setNewGraph("TGC_Average_Name_2", m_Version2Name.c_str(), "Station ID Number", "Average Number of Hits");
  msRootInterface.setNewGraph("TGC_Average_UniqueID_1", m_Version1Name.c_str(), "Unique ID Number", "Average Number of Hits");
  msRootInterface.setNewGraph("TGC_Average_UniqueID_2", m_Version2Name.c_str(), "Unique ID Number", "Average Number of Hits");


  if 	((m_SpecialStationNameID<0) ||
	 (m_SpecialStationNameID>51) ||
	 (m_SpecialStationEta<-8) ||
	 (m_SpecialStationEta>8) ||
	 (m_SpecialStationPhi<1) ||
	 (m_SpecialStationEta>8))
    {
      m_SpecialStationNameID=-10; 
      m_SpecialStationEta=-10;
      m_SpecialStationPhi=-10;
    }

  if (m_SpecialStationNameID>=0)
    {
      m_1D_Histos["Chamber_Version1"] = histoSvc()->book("/stat","Chamber_Version1","Chamber_Version1",120,0,120);
      m_1D_Histos["Chamber_Version2"] = histoSvc()->book("/stat","Chamber_Version2","Chamber_Version2",120,0,120);
      m_1D_Histos["Chamber_Res_Z"] = histoSvc()->book("/stat","Chamber_Res_Z","Chamber_Res_Z",100,-1,1);
      m_1D_Histos["Chamber_Res_R"]= histoSvc()->book("/stat","Chamber_Res_R","Chamber_Res_R",100,-1,1);
    }


  m_1D_Histos["Temp"] = histoSvc()->book("/stat","Temp","Temp",300,0,100);

  m_MdtStationDescription.clear();
  setup();
  log << MSG::INFO << "Setup done" << endreq;
  if(loadDataFiles().isFailure()) return StatusCode::FAILURE;
  log << MSG::INFO << "Input-Files loaded" << endreq;

  msRootInterface.clear();
  msRootInterface.setNewHistrogram("MDT_NumberOfEvents_Eta_1",m_Version1Name.c_str(),"#eta","Number Of Entries", 40, -10, 10);
  msRootInterface.setNewHistrogram("MDT_NumberOfEvents_Eta_2",m_Version2Name.c_str(),"#eta","Number Of Entries", 40, -10, 10);
  msRootInterface.setNewHistrogram("MDT_NumberOfEvents_Nam_1",m_Version1Name.c_str(),"StationName","Number Of Entries", 100, 0, 55);
  msRootInterface.setNewHistrogram("MDT_NumberOfEvents_Nam_2",m_Version2Name.c_str(),"StationName","Number Of Entries", 100, 0, 55);

  msRootInterface.setNewHistrogram("MDT_Number_Eta_1", m_Version1Name.c_str(), "#eta", "Number Of Entries", 40, -10, 10);
  msRootInterface.setNewHistrogram("MDT_Number_Eta_2", m_Version2Name.c_str(), "#eta", "Number Of Entries", 40, -10, 10);
  msRootInterface.setNewHistrogram("MDT_Number_Phi_1", m_Version1Name.c_str(), "#phi", "Number Of Entries", 40, 0, 10);
  msRootInterface.setNewHistrogram("MDT_Number_Phi_2", m_Version2Name.c_str(), "#phi", "Number Of Entries", 40, 0, 10);
  msRootInterface.setNewHistrogram("MDT_Number_Nam_1", m_Version1Name.c_str(), "StationName", "Number Of Entries",100, 0, 55);
  msRootInterface.setNewHistrogram("MDT_Number_Nam_2", m_Version2Name.c_str(), "StationName", "Number Of Entries",100, 0, 55);

  msRootInterface.setNewHistrogram("RPC_Number_Eta_1", m_Version1Name.c_str(), "#eta", "Number Of Entries", 40, -10, 10);
  msRootInterface.setNewHistrogram("RPC_Number_Eta_2", m_Version2Name.c_str(), "#eta", "Number Of Entries", 40, -10, 10);
  msRootInterface.setNewHistrogram("RPC_Number_Phi_1", m_Version1Name.c_str(), "#phi", "Number Of Entries", 40, 0, 10);
  msRootInterface.setNewHistrogram("RPC_Number_Phi_2", m_Version2Name.c_str(), "#phi", "Number Of Entries", 40, 0, 10);
  msRootInterface.setNewHistrogram("RPC_Number_Nam_1", m_Version1Name.c_str(), "StationName", "Number Of Entries",100, 0, 55);
  msRootInterface.setNewHistrogram("RPC_Number_Nam_2", m_Version2Name.c_str(), "StationName", "Number Of Entries",100, 0, 55);

  msRootInterface.setNewHistrogram("CSC_Number_Eta_1", m_Version1Name.c_str(), "#eta", "Number Of Entries", 40, -10, 10);
  msRootInterface.setNewHistrogram("CSC_Number_Eta_2", m_Version2Name.c_str(), "#eta", "Number Of Entries", 40, -10, 10);
  msRootInterface.setNewHistrogram("CSC_Number_Phi_1", m_Version1Name.c_str(), "#phi", "Number Of Entries", 40, 0, 10);
  msRootInterface.setNewHistrogram("CSC_Number_Phi_2", m_Version2Name.c_str(), "#phi", "Number Of Entries", 40, 0, 10);
  msRootInterface.setNewHistrogram("CSC_Number_Nam_1", m_Version1Name.c_str(), "StationName", "Number Of Entries",100, 0, 55);
  msRootInterface.setNewHistrogram("CSC_Number_Nam_2", m_Version2Name.c_str(), "StationName", "Number Of Entries",100, 0, 55);

  msRootInterface.setNewHistrogram("TGC_Number_Eta_1", m_Version1Name.c_str(), "#eta", "Number Of Entries", 40, -10, 10);
  msRootInterface.setNewHistrogram("TGC_Number_Eta_2", m_Version2Name.c_str(), "#eta", "Number Of Entries", 40, -10, 10);
  msRootInterface.setNewHistrogram("TGC_Number_Phi_1", m_Version1Name.c_str(), "#phi", "Number Of Entries", 40, 0, 50);
  msRootInterface.setNewHistrogram("TGC_Number_Phi_2", m_Version2Name.c_str(), "#phi", "Number Of Entries", 40, 0, 50);
  msRootInterface.setNewHistrogram("TGC_Number_Nam_1", m_Version1Name.c_str(), "StationName", "Number Of Entries",100, 0, 55);
  msRootInterface.setNewHistrogram("TGC_Number_Nam_2", m_Version2Name.c_str(), "StationName", "Number Of Entries",100, 0, 55);

  return StatusCode::SUCCESS;
}


void MVCombined::setup()
{
  MVCommonStationData vtMdtDummy;
  MVCommonStationData vtRpcDummy;
  MVCommonStationData vtCscDummy;
  MVCommonStationData vtTgcDummy;

  int id, eta, phi;
  MVCommonStationID vtStationID_Mdt("MDT");
  MVCommonStationID vtStationID_Rpc("RPC");
  MVCommonStationID vtStationID_Tgc("TGC");
  MVCommonStationID vtStationID_Csc("CSC");

  for (int n=0; n<=vtStationID_Mdt.getMaxIDNumber(); n++)
    {
      vtStationID_Mdt.getNameIDEtaPhi(n, id, eta, phi);
      vtMdtDummy.setStationInfo(id, eta, phi);
      m_MdtStationDescription[n]=vtMdtDummy;
    }

  for (int n=0; n<=vtStationID_Rpc.getMaxIDNumber(); n++)
    {
      vtStationID_Rpc.getNameIDEtaPhi(n, id, eta, phi);
      vtRpcDummy.setStationInfo(id, eta, phi);
      m_RpcStationDescription[n]=vtRpcDummy;
    }

  for (int n=0; n<=vtStationID_Csc.getMaxIDNumber(); n++)
    {
      vtStationID_Csc.getNameIDEtaPhi(n, id, eta, phi);
      vtCscDummy.setStationInfo(id, eta, phi);
      m_CscStationDescription[n]=vtCscDummy;
    }

  for (int n=0; n<=vtStationID_Tgc.getMaxIDNumber(); n++)
    {
      vtStationID_Tgc.getNameIDEtaPhi(n, id, eta, phi);
      vtTgcDummy.setStationInfo(id, eta, phi);
      m_TgcStationDescription[n]=vtTgcDummy;
    }

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode MVCombined::execute()
{
  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "execute()" << endreq;

  n_EventCounter++;

  //int sc;
  MVCommonDataDescription mvVersion;

  int InternalName;
  int Eta;
  int Phi;

  MVCommonStationID	vtStationID_MDT("MDT");
  MVCommonStationID	vtStationID_RPC("RPC");
  MVCommonStationID	vtStationID_CSC("CSC");
  MVCommonStationID	vtStationID_TGC("TGC");

  while (vtLoader.getNext_Data("MDT_Version1", mvVersion)==true)
    {
      MDTHit hit_mdt;
      mvVersion.startUpHitCounter();
      while (mvVersion.getNextHitInfo(hit_mdt)==true)
	{
	  vtStationID_MDT.getNameIDEtaPhi(mvVersion.getID(), InternalName, Eta, Phi);
	  msRootInterface.fill("MDT_Number_Nam_1",InternalName);
	  msRootInterface.fill("MDT_Number_Eta_1",Eta);
	  msRootInterface.fill("MDT_Number_Phi_1",Phi);
	  int signature;
	  hit_mdt.WireY<0 ? signature=-1 : signature=1;
	  m_2D_Histos[m_Version1Name+"MDT_ZR-Plane_complete"]->fill(hit_mdt.WireZ, signature*sqrt(hit_mdt.WireY*hit_mdt.WireY+hit_mdt.WireX*hit_mdt.WireX), 1.0);
	  if (vtStationID_MDT.getStationName(InternalName).substr (0,1)=="E")
	     m_2D_Histos[m_Version1Name+"MDT_XY-Plane_endcap"]->fill(hit_mdt.WireX, hit_mdt.WireY, 1.0);
	  else if (vtStationID_MDT.getStationName(InternalName).substr (0,1)=="B")
	     m_2D_Histos[m_Version1Name+"MDT_XY-Plane_barrel"]->fill(hit_mdt.WireX, hit_mdt.WireY, 1.0);
	  else log << MSG::INFO << vtStationID_MDT.getStationName(InternalName) << " is nor MDT barrel neither endcap!" << endreq;
	}
    }
  while (vtLoader.getNext_Data("MDT_Version2", mvVersion)==true)
    {
      MDTHit hit_mdt;
      mvVersion.startUpHitCounter();
      while (mvVersion.getNextHitInfo(hit_mdt)==true)
	{
	  vtStationID_MDT.getNameIDEtaPhi(mvVersion.getID(), InternalName, Eta, Phi);
	  msRootInterface.fill("MDT_Number_Nam_2",InternalName);
	  msRootInterface.fill("MDT_Number_Eta_2",Eta);
	  msRootInterface.fill("MDT_Number_Phi_2",Phi);
	  int signature;
	  hit_mdt.WireY<0 ? signature=-1 : signature=1;
	  m_2D_Histos[m_Version2Name+"MDT_ZR-Plane_complete"]->fill(hit_mdt.WireZ, signature*sqrt(hit_mdt.WireY*hit_mdt.WireY+hit_mdt.WireX*hit_mdt.WireX), 1.0);
	  if (vtStationID_MDT.getStationName(InternalName).substr (0,1)=="E")
	     m_2D_Histos[m_Version2Name+"MDT_XY-Plane_endcap"]->fill(hit_mdt.WireX, hit_mdt.WireY, 1.0);
	  else if (vtStationID_MDT.getStationName(InternalName).substr (0,1)=="B")
	     m_2D_Histos[m_Version2Name+"MDT_XY-Plane_barrel"]->fill(hit_mdt.WireX, hit_mdt.WireY, 1.0);
	  else log << MSG::INFO << vtStationID_MDT.getStationName(InternalName) << " is nor MDT barrel neither endcap!" << endreq;
	}
    }

  while (vtLoader.getNext_Data("RPC_Version1", mvVersion)==true)
    {
      RPCHit hit_rpc;
      mvVersion.startUpHitCounter();
      while (mvVersion.getNextHitInfo(hit_rpc)==true)
	{
	  vtStationID_RPC.getNameIDEtaPhi(mvVersion.getID(), InternalName, Eta, Phi);
	  msRootInterface.fill("RPC_Number_Nam_1",InternalName);
	  msRootInterface.fill("RPC_Number_Eta_1",Eta);
	  msRootInterface.fill("RPC_Number_Phi_1",Phi);
	  int signature;
	  hit_rpc.StripY<0 ? signature=-1 : signature=1;
	  m_2D_Histos[m_Version1Name+"RPC_ZR-Plane_barrel"]->fill(hit_rpc.StripZ, signature*sqrt(hit_rpc.StripY*hit_rpc.StripY+hit_rpc.StripX*hit_rpc.StripX), 1.0);
  	  m_2D_Histos[m_Version1Name+"RPC_XY-Plane_barrel"]->fill(hit_rpc.StripX, hit_rpc.StripY, 1.0);
	}
    }
  while (vtLoader.getNext_Data("RPC_Version2", mvVersion)==true)
    {
      RPCHit hit_rpc;
      mvVersion.startUpHitCounter();
      while (mvVersion.getNextHitInfo(hit_rpc)==true)
	{
	  vtStationID_RPC.getNameIDEtaPhi(mvVersion.getID(), InternalName, Eta, Phi);
	  msRootInterface.fill("RPC_Number_Nam_2",InternalName);
	  msRootInterface.fill("RPC_Number_Eta_2",Eta);
	  msRootInterface.fill("RPC_Number_Phi_2",Phi);
	  int signature;
	  hit_rpc.StripY<0 ? signature=-1 : signature=1;
	  m_2D_Histos[m_Version2Name+"RPC_ZR-Plane_barrel"]->fill(hit_rpc.StripZ, signature*sqrt(hit_rpc.StripY*hit_rpc.StripY+hit_rpc.StripX*hit_rpc.StripX), 1.0);
  	  m_2D_Histos[m_Version2Name+"RPC_XY-Plane_barrel"]->fill(hit_rpc.StripX, hit_rpc.StripY, 1.0);
	}
    }

  while (vtLoader.getNext_Data("CSC_Version1", mvVersion)==true)
    {
      CSCHit hit_csc;
      mvVersion.startUpHitCounter();
      while (mvVersion.getNextHitInfo(hit_csc)==true)
	{
	  vtStationID_CSC.getNameIDEtaPhi(mvVersion.getID(), InternalName, Eta, Phi);
	  msRootInterface.fill("CSC_Number_Nam_1",InternalName);
	  msRootInterface.fill("CSC_Number_Eta_1",Eta);
	  msRootInterface.fill("CSC_Number_Phi_1",Phi);
	  int signature;
	  hit_csc.StripY<0 ? signature=-1 : signature=1;
	  m_2D_Histos[m_Version1Name+"CSC_ZR-Plane_endcap"]->fill(hit_csc.StripZ, signature*sqrt(hit_csc.StripY*hit_csc.StripY+hit_csc.StripX*hit_csc.StripX), 1.0);
  	  m_2D_Histos[m_Version1Name+"CSC_XY-Plane_endcap"]->fill(hit_csc.StripX, hit_csc.StripY, 1.0);
	}
    }
  while (vtLoader.getNext_Data("CSC_Version2", mvVersion)==true)
    {
      CSCHit hit_csc;
      mvVersion.startUpHitCounter();
      while (mvVersion.getNextHitInfo(hit_csc)==true)
	{
	  vtStationID_CSC.getNameIDEtaPhi(mvVersion.getID(), InternalName, Eta, Phi);
	  msRootInterface.fill("CSC_Number_Nam_2",InternalName);
	  msRootInterface.fill("CSC_Number_Eta_2",Eta);
	  msRootInterface.fill("CSC_Number_Phi_2",Phi);
	  int signature;
	  hit_csc.StripY<0 ? signature=-1 : signature=1;
	  m_2D_Histos[m_Version2Name+"CSC_ZR-Plane_endcap"]->fill(hit_csc.StripZ, signature*sqrt(hit_csc.StripY*hit_csc.StripY+hit_csc.StripX*hit_csc.StripX), 1.0);
  	  m_2D_Histos[m_Version2Name+"CSC_XY-Plane_endcap"]->fill(hit_csc.StripX, hit_csc.StripY, 1.0);
	}
    }

  while (vtLoader.getNext_Data("TGC_Version1", mvVersion)==true)
    {
      TGCHit hit_tgc;
      mvVersion.startUpHitCounter();
      while (mvVersion.getNextHitInfo(hit_tgc)==true)
	{
	  vtStationID_TGC.getNameIDEtaPhi(mvVersion.getID(), InternalName, Eta, Phi);
	  msRootInterface.fill("TGC_Number_Nam_1",InternalName);
	  msRootInterface.fill("TGC_Number_Eta_1",Eta);
	  msRootInterface.fill("TGC_Number_Phi_1",Phi);
	  int signature;
	  hit_tgc.ChannelPosY<0 ? signature=-1 : signature=1;
	  m_2D_Histos[m_Version1Name+"TGC_ZR-Plane_endcap"]->fill(hit_tgc.ChannelPosZ, signature*sqrt(hit_tgc.ChannelPosY*hit_tgc.ChannelPosY+hit_tgc.ChannelPosX*hit_tgc.ChannelPosX), 1.0);
  	  m_2D_Histos[m_Version1Name+"TGC_XY-Plane_endcap"]->fill(hit_tgc.ChannelPosX, hit_tgc.ChannelPosY, 1.0);
	}
    }
  while (vtLoader.getNext_Data("TGC_Version2", mvVersion)==true)
    {
      TGCHit hit_tgc;
      mvVersion.startUpHitCounter();
      while (mvVersion.getNextHitInfo(hit_tgc)==true)
	{
	  vtStationID_TGC.getNameIDEtaPhi(mvVersion.getID(), InternalName, Eta, Phi);
	  msRootInterface.fill("TGC_Number_Nam_2",InternalName);
	  msRootInterface.fill("TGC_Number_Eta_2",Eta);
	  msRootInterface.fill("TGC_Number_Phi_2",Phi);
	  int signature;
	  hit_tgc.ChannelPosY<0 ? signature=-1 : signature=1;
	  m_2D_Histos[m_Version2Name+"TGC_ZR-Plane_endcap"]->fill(hit_tgc.ChannelPosZ, signature*sqrt(hit_tgc.ChannelPosY*hit_tgc.ChannelPosY+hit_tgc.ChannelPosX*hit_tgc.ChannelPosX), 1.0);
  	  m_2D_Histos[m_Version2Name+"TGC_XY-Plane_endcap"]->fill(hit_tgc.ChannelPosX, hit_tgc.ChannelPosY, 1.0);
	}
    }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode MVCombined::finalize()
{
  std::string sname;

  if(Generate_MDT_Histograms().isFailure()) return StatusCode::FAILURE;
  if(Generate_RPC_Histograms().isFailure()) return StatusCode::FAILURE;
  if(Generate_TGC_Histograms().isFailure()) return StatusCode::FAILURE;
  if(Generate_CSC_Histograms().isFailure()) return StatusCode::FAILURE;

  sname=m_OutputFile+":/MDT/";
  msRootInterface.setDirectory(sname.c_str());
  msRootInterface.writeOverlayHistograms("MDT_Number_Nam_1", "MDT_Number_Nam_2", "MDT_Number_Nam");
  msRootInterface.writeOverlayHistograms("MDT_Number_Phi_1", "MDT_Number_Phi_2", "MDT_Number_Phi");
  msRootInterface.writeOverlayHistograms("MDT_Number_Eta_1", "MDT_Number_Eta_2", "MDT_Number_Eta");

  msRootInterface.writeComparisionGraph("MDT_Average_Phi_1","MDT_Average_Phi_2", "MDT_Average_Phi");
  msRootInterface.writeComparisionGraph("MDT_Average_Eta_1","MDT_Average_Eta_2", "MDT_Average_Eta");
  msRootInterface.writeComparisionGraph("MDT_Average_Name_1","MDT_Average_Name_2", "MDT_Average_Name", "StationName");
  msRootInterface.writeComparisionGraph("MDT_Average_UniqueID_1","MDT_Average_UniqueID_2", "MDT_Average_UniqueID");

  msRootInterface.writeOverlayHistograms("MDT_NumberOfEvents_Eta_1","MDT_NumberOfEvents_Eta_2","MDT_NumberOfEvents_Eta");
  msRootInterface.writeOverlayHistograms("MDT_NumberOfEvents_Nam_1","MDT_NumberOfEvents_Nam_2","MDT_NumberOfEvents_Name");

  msRootInterface.writeGraph("MDT_AssoziatedHits_Eta_1to2");
  msRootInterface.writeGraph("MDT_AssoziatedHits_Eta_2to1");
  msRootInterface.writeGraph("MDT_AssoziatedHits_Name_1to2");
  msRootInterface.writeGraph("MDT_AssoziatedHits_Name_2to1");

  sname=m_OutputFile+":/RPC/";
  msRootInterface.setDirectory(sname.c_str());
  msRootInterface.writeOverlayHistograms("RPC_Number_Nam_1", "RPC_Number_Nam_2", "RPC_Number_Nam");
  msRootInterface.writeOverlayHistograms("RPC_Number_Phi_1", "RPC_Number_Phi_2", "RPC_Number_Phi");
  msRootInterface.writeOverlayHistograms("RPC_Number_Eta_1", "RPC_Number_Eta_2", "RPC_Number_Eta");

  msRootInterface.writeComparisionGraph("RPC_Average_Eta_1","RPC_Average_Eta_2", "RPC_Average_Eta");
  msRootInterface.writeComparisionGraph("RPC_Average_Name_1","RPC_Average_Name_2", "RPC_Average_Name");
  msRootInterface.writeComparisionGraph("RPC_Average_Phi_1","RPC_Average_Phi_2", "RPC_Average_Phi");
  msRootInterface.writeComparisionGraph("RPC_Average_UniqueID_1","RPC_Average_UniqueID_2", "RPC_Average_UniqueID");

  sname=m_OutputFile+":/CSC/";
  msRootInterface.setDirectory(sname.c_str());
  msRootInterface.writeOverlayHistograms("CSC_Number_Nam_1", "CSC_Number_Nam_2", "CSC_Number_Nam");
  msRootInterface.writeOverlayHistograms("CSC_Number_Phi_1", "CSC_Number_Phi_2", "CSC_Number_Phi");
  msRootInterface.writeOverlayHistograms("CSC_Number_Eta_1", "CSC_Number_Eta_2", "CSC_Number_Eta");

  msRootInterface.writeComparisionGraph("CSC_Average_Eta_1","CSC_Average_Eta_2", "CSC_Average_Eta");
  msRootInterface.writeComparisionGraph("CSC_Average_Name_1","CSC_Average_Name_2", "CSC_Average_Name");
  msRootInterface.writeComparisionGraph("CSC_Average_Phi_1","CSC_Average_Phi_2", "CSC_Average_Phi");
  msRootInterface.writeComparisionGraph("CSC_Average_UniqueID_1","CSC_Average_UniqueID_2", "CSC_Average_UniqueID");

  sname=m_OutputFile+":/TGC/";
  msRootInterface.setDirectory(sname.c_str());
  msRootInterface.writeOverlayHistograms("TGC_Number_Nam_1", "TGC_Number_Nam_2", "TGC_Number_Nam");
  msRootInterface.writeOverlayHistograms("TGC_Number_Phi_1", "TGC_Number_Phi_2", "TGC_Number_Phi");
  msRootInterface.writeOverlayHistograms("TGC_Number_Eta_1", "TGC_Number_Eta_2", "TGC_Number_Eta");

  msRootInterface.writeComparisionGraph("TGC_Average_Eta_1","TGC_Average_Eta_2", "TGC_Average_Eta");
  msRootInterface.writeComparisionGraph("TGC_Average_Name_1","TGC_Average_Name_2", "TGC_Average_Name");
  msRootInterface.writeComparisionGraph("TGC_Average_Phi_1","TGC_Average_Phi_2", "TGC_Average_Phi");
  msRootInterface.writeComparisionGraph("TGC_Average_UniqueID_1","TGC_Average_UniqueID_2", "TGC_Average_UniqueID");

  msRootInterface.clear();

  if (m_SpecialStationNameID>=0) GenerateSingleChamberHistograms(	m_SpecialStationNameID,	m_SpecialStationEta, m_SpecialStationPhi);
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endreq;

  TDirectory *tDir = new TDirectory();
  sname=m_OutputFile+":/";
  tDir->cd(sname.c_str());
  delete tDir;

  return StatusCode::SUCCESS;
}

StatusCode MVCombined::Generate_MDT_Histograms()
{
  int id, eta, phi;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Generate Mdt Histograms" << endreq;

  MVCommonStationID vtStationID_Mdt("MDT");

  MVCommonResultDescription vtRes_version1_mdt_id(vtStationID_Mdt.getMinStationName(),vtStationID_Mdt.getMaxStationName());
  MVCommonResultDescription vtRes_version1_mdt_eta(vtStationID_Mdt.getMinStationEta(),vtStationID_Mdt.getMaxStationEta());
  MVCommonResultDescription vtRes_version1_mdt_phi(vtStationID_Mdt.getMinStationPhi(),vtStationID_Mdt.getMaxStationPhi());
  MVCommonResultDescription vtRes_version2_mdt_id(vtStationID_Mdt.getMinStationName(),vtStationID_Mdt.getMaxStationName());
  MVCommonResultDescription vtRes_version2_mdt_eta(vtStationID_Mdt.getMinStationEta(),vtStationID_Mdt.getMaxStationEta());
  MVCommonResultDescription vtRes_version2_mdt_phi(vtStationID_Mdt.getMinStationPhi(),vtStationID_Mdt.getMaxStationPhi());
  MVCommonResultDescription vtRes_asso_version1_mdt_id(vtStationID_Mdt.getMinStationName(),vtStationID_Mdt.getMaxStationName());
  MVCommonResultDescription vtRes_asso_version1_mdt_eta(vtStationID_Mdt.getMinStationEta(),vtStationID_Mdt.getMaxStationEta());
  MVCommonResultDescription vtRes_asso_version2_mdt_id(vtStationID_Mdt.getMinStationName(),vtStationID_Mdt.getMaxStationName());
  MVCommonResultDescription vtRes_asso_version2_mdt_eta(vtStationID_Mdt.getMinStationEta(),vtStationID_Mdt.getMaxStationEta());
  MVCommonResiduumResults mvResiduumResult;

  int idMap;
  int __attribute__((__unused__)) internID;
  float mean, error;

  long tmpPass;
  long tmpTotal;

  internID=0;

  vtRes_asso_version1_mdt_id.clear();
  vtRes_asso_version2_mdt_id.clear();
  vtRes_asso_version1_mdt_eta.clear();
  vtRes_asso_version2_mdt_eta.clear();

  for (idMap=0; idMap<=vtStationID_Mdt.getMaxIDNumber(); idMap++)
    {
      vtStationID_Mdt.getNameIDEtaPhi(idMap, id, eta, phi);

      // Hits
      mean=m_MdtStationDescription[idMap].getMeanNumberOfVersion1(tmpPass,tmpTotal);
      if (mean>=0.0)
	{
	  vtRes_version1_mdt_id.add(id,tmpPass,tmpTotal);
	  vtRes_version1_mdt_phi.add(phi,tmpPass,tmpTotal);
	  vtRes_version1_mdt_eta.add(eta,tmpPass,tmpTotal);
	  msRootInterface.fill("MDT_NumberOfEvents_Eta_1",eta,tmpTotal);
	  msRootInterface.fill("MDT_NumberOfEvents_Nam_1",id,tmpTotal);
	  if (mean>0.0) msRootInterface.setGraphPoint("MDT_Average_UniqueID_1",idMap,mean,0.001,0.001);
	}

      // Digits
      mean=m_MdtStationDescription[idMap].getMeanNumberOfVersion2(tmpPass,tmpTotal);
      if (mean>=0.0)
	{
	  vtRes_version2_mdt_id.add(id,tmpPass,tmpTotal);
	  vtRes_version2_mdt_phi.add(phi,tmpPass,tmpTotal);
	  vtRes_version2_mdt_eta.add(eta,tmpPass,tmpTotal);
	  msRootInterface.fill("MDT_NumberOfEvents_Eta_2",eta,tmpTotal);
	  msRootInterface.fill("MDT_NumberOfEvents_Nam_2",id,tmpTotal);
	  if (mean>0.0) msRootInterface.setGraphPoint("MDT_Average_UniqueID_2",idMap,mean,0.001,0.001);
	}
      // Associated Digits and Hits
      mvResiduumResult=m_MdtStationDescription[idMap].getMeanDifference(m_minimalDifference);
      /*		if (mvResiduumResult.vResZ0.size()>0)
			{
			for (int n=0; n<(int)(mvResiduumResult.vResZ0.size()); n++)
			m_1D_Histos["MDT_Res_HitDig_Z"]->fill(mvResiduumResult.vResZ0[n]);
			}
			if ((int)(mvResiduumResult.vResRadius.size())>0)
			{
			for (int n=0; n<(int)(mvResiduumResult.vResRadius.size()); n++)
			m_1D_Histos["MDT_Res_HitDig_R"]->fill(mvResiduumResult.vResRadius[n]);
			}*/

      if ((mvResiduumResult.TotalHits1>0) && (mvResiduumResult.TotalHits2>0))
	{
	  vtRes_asso_version1_mdt_id.add(id, mvResiduumResult.CommonHits, mvResiduumResult.TotalHits1);
	  vtRes_asso_version1_mdt_eta.add(eta, mvResiduumResult.CommonHits, mvResiduumResult.TotalHits1);
	  vtRes_asso_version2_mdt_id.add(id, mvResiduumResult.CommonHits, mvResiduumResult.TotalHits2);
	  vtRes_asso_version2_mdt_eta.add(eta, mvResiduumResult.CommonHits, mvResiduumResult.TotalHits2);
	}
    }


  for (int id=vtStationID_Mdt.getMinStationName(); id<=vtStationID_Mdt.getMaxStationName(); id++)
    {
      vtRes_version2_mdt_id.getMean(id,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("MDT_Average_Name_2",id,mean,0.001,error);
      vtRes_version1_mdt_id.getMean(id,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("MDT_Average_Name_1",id,mean,0.001,error);

      vtRes_asso_version1_mdt_id.getMean(id,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("MDT_AssoziatedHits_Name_1to2",id,mean,0.001,error);
      vtRes_asso_version2_mdt_id.getMean(id,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("MDT_AssoziatedHits_Name_2to1",id,mean,0.001,error);

      //		m_1D_Histos["MDT_Asso_Version1_NameID"]->fill(id,vtRes_asso_version1_mdt_id.getMean(id));
      //		m_1D_Histos["MDT_Asso_Version2_NameID"]->fill(id,vtRes_asso_version2_mdt_id.getMean(id));

    }
  for (int eta=vtStationID_Mdt.getMinStationEta(); eta<=vtStationID_Mdt.getMaxStationEta(); eta++)
    {
      vtRes_version2_mdt_eta.getMean(eta,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("MDT_Average_Eta_2",eta,mean,0.001,error);
      vtRes_version1_mdt_eta.getMean(eta,mean,error);

      if (mean>0.0) msRootInterface.setGraphPoint("MDT_Average_Eta_1",eta,mean,0.001,error);

      vtRes_asso_version1_mdt_eta.getMean(eta,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("MDT_AssoziatedHits_Eta_1to2",eta,mean,0.001,error);
      vtRes_asso_version2_mdt_eta.getMean(eta,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("MDT_AssoziatedHits_Eta_2to1",eta,mean,0.001,error);


      //		m_1D_Histos["MDT_Asso_Version1_Eta"]->fill(eta,vtRes_asso_version1_mdt_eta.getMean(eta));
      //		m_1D_Histos["MDT_Asso_Version2_Eta"]->fill(eta,vtRes_asso_version2_mdt_eta.getMean(eta));
    }
  for (int phi=vtStationID_Mdt.getMinStationPhi(); phi<=vtStationID_Mdt.getMaxStationPhi(); phi++)
    {
      vtRes_version1_mdt_phi.getMean(phi,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("MDT_Average_Phi_1",phi,mean,0.001,error);
      vtRes_version2_mdt_phi.getMean(phi,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("MDT_Average_Phi_2",phi,mean,0.001,error);
    }

  return StatusCode::SUCCESS;
}


StatusCode MVCombined::Generate_RPC_Histograms()
{
  int id, eta, phi;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Generate Rpc Histograms" << endreq;

  MVCommonStationID vtStationID_Rpc("RPC");

  MVCommonResultDescription vtRes_version1_rpc_id(vtStationID_Rpc.getMinStationName(),vtStationID_Rpc.getMaxStationName());
  MVCommonResultDescription vtRes_version1_rpc_phi(vtStationID_Rpc.getMinStationPhi(),vtStationID_Rpc.getMaxStationPhi());
  MVCommonResultDescription vtRes_version1_rpc_eta(vtStationID_Rpc.getMinStationEta(),vtStationID_Rpc.getMaxStationEta());
  MVCommonResultDescription vtRes_version2_rpc_id(vtStationID_Rpc.getMinStationName(),vtStationID_Rpc.getMaxStationName());
  MVCommonResultDescription vtRes_version2_rpc_phi(vtStationID_Rpc.getMinStationPhi(),vtStationID_Rpc.getMaxStationPhi());
  MVCommonResultDescription vtRes_version2_rpc_eta(vtStationID_Rpc.getMinStationEta(),vtStationID_Rpc.getMaxStationEta());
  MVCommonResultDescription vtRes_asso_version1_rpc_id(vtStationID_Rpc.getMinStationName(),vtStationID_Rpc.getMaxStationName());
  MVCommonResultDescription vtRes_asso_version1_rpc_eta(vtStationID_Rpc.getMinStationEta(),vtStationID_Rpc.getMaxStationEta());
  MVCommonResultDescription vtRes_asso_version2_rpc_id(vtStationID_Rpc.getMinStationName(),vtStationID_Rpc.getMaxStationName());
  MVCommonResultDescription vtRes_asso_version2_rpc_eta(vtStationID_Rpc.getMinStationEta(),vtStationID_Rpc.getMaxStationEta());
  MVCommonResiduumResults mvResiduumResult;

  int idMap;
  int __attribute__((__unused__)) internID;
  float mean, error;

  long tmpPass;
  long tmpTotal;

  internID=0;

  // RPC
  internID=0;
  for (idMap=0; idMap<=vtStationID_Rpc.getMaxIDNumber(); idMap++)
    {
      vtStationID_Rpc.getNameIDEtaPhi(idMap, id, eta, phi);

      // Hits
      mean=m_RpcStationDescription[idMap].getMeanNumberOfVersion1(tmpPass,tmpTotal);
      if (mean>=0.0)
	{
	  vtRes_version1_rpc_id.add(id,tmpPass,tmpTotal);
	  vtRes_version1_rpc_phi.add(phi,tmpPass,tmpTotal);
	  vtRes_version1_rpc_eta.add(eta,tmpPass,tmpTotal);
	  if (mean>0.0) msRootInterface.setGraphPoint("RPC_Average_UniqueID_1",idMap,mean,0.001,0.001);
	}

      // Digits
      mean=m_RpcStationDescription[idMap].getMeanNumberOfVersion2(tmpPass,tmpTotal);
      if (mean>=0.0)
	{
	  vtRes_version2_rpc_id.add(id,tmpPass,tmpTotal);
	  vtRes_version2_rpc_phi.add(phi,tmpPass,tmpTotal);
	  vtRes_version2_rpc_eta.add(eta,tmpPass,tmpTotal);
	  if (mean>0.0) msRootInterface.setGraphPoint("RPC_Average_UniqueID_2",idMap,mean,0.001,0.001);
	}

    }
  for (int id=vtStationID_Rpc.getMinStationName(); id<=vtStationID_Rpc.getMaxStationName(); id++)
    {
      vtRes_version2_rpc_id.getMean(id,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("RPC_Average_Name_2",id,mean,0.001,error);
      vtRes_version1_rpc_id.getMean(id,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("RPC_Average_Name_1",id,mean,0.001,error);

    }
  for (int eta=vtStationID_Rpc.getMinStationEta(); eta<=vtStationID_Rpc.getMaxStationEta(); eta++)
    {
      vtRes_version2_rpc_eta.getMean(eta,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("RPC_Average_Eta_2",eta,mean,0.001,error);
      vtRes_version1_rpc_eta.getMean(eta,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("RPC_Average_Eta_1",eta,mean,0.001,error);
    }
  for (int phi=vtStationID_Rpc.getMinStationPhi(); phi<=vtStationID_Rpc.getMaxStationPhi(); phi++)
    {
      vtRes_version2_rpc_phi.getMean(phi,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("RPC_Average_Phi_2",phi,mean,0.001,error);
      vtRes_version1_rpc_phi.getMean(phi,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("RPC_Average_Phi_1",phi,mean,0.001,error);
    }

  return StatusCode::SUCCESS;
}


StatusCode MVCombined::Generate_CSC_Histograms()
{
  int id, eta, phi;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Generate Csc Histograms" << endreq;

  MVCommonStationID vtStationID_Csc("CSC");

  MVCommonResultDescription vtRes_version1_csc_id(vtStationID_Csc.getMinStationName(),vtStationID_Csc.getMaxStationName());
  MVCommonResultDescription vtRes_version1_csc_phi(vtStationID_Csc.getMinStationPhi(),vtStationID_Csc.getMaxStationPhi());
  MVCommonResultDescription vtRes_version1_csc_eta(vtStationID_Csc.getMinStationEta(),vtStationID_Csc.getMaxStationEta());
  MVCommonResultDescription vtRes_version2_csc_id(vtStationID_Csc.getMinStationName(),vtStationID_Csc.getMaxStationName());
  MVCommonResultDescription vtRes_version2_csc_phi(vtStationID_Csc.getMinStationPhi(),vtStationID_Csc.getMaxStationPhi());
  MVCommonResultDescription vtRes_version2_csc_eta(vtStationID_Csc.getMinStationEta(),vtStationID_Csc.getMaxStationEta());
  MVCommonResultDescription vtRes_asso_version1_csc_id(vtStationID_Csc.getMinStationName(),vtStationID_Csc.getMaxStationName());
  MVCommonResultDescription vtRes_asso_version1_csc_eta(vtStationID_Csc.getMinStationEta(),vtStationID_Csc.getMaxStationEta());
  MVCommonResultDescription vtRes_asso_version2_csc_id(vtStationID_Csc.getMinStationName(),vtStationID_Csc.getMaxStationName());
  MVCommonResultDescription vtRes_asso_version2_csc_eta(vtStationID_Csc.getMinStationEta(),vtStationID_Csc.getMaxStationEta());

  MVCommonResiduumResults mvResiduumResult;

  int idMap;
  int __attribute__((__unused__)) internID;
  float mean, error;

  long tmpPass;
  long tmpTotal;

  internID=0;

  // CSC
  internID=0;
  for (idMap=0; idMap<=vtStationID_Csc.getMaxIDNumber(); idMap++)
    {
      vtStationID_Csc.getNameIDEtaPhi(idMap, id, eta, phi);

      // Hits
      mean=m_CscStationDescription[idMap].getMeanNumberOfVersion1(tmpPass,tmpTotal);
      if (mean>=0.0)
	{
	  vtRes_version1_csc_id.add(id,tmpPass,tmpTotal);
	  vtRes_version1_csc_phi.add(phi,tmpPass,tmpTotal);
	  vtRes_version1_csc_eta.add(eta,tmpPass,tmpTotal);
	  if (mean>0.0) msRootInterface.setGraphPoint("CSC_Average_UniqueID_1",idMap,mean,0.001,0.001);
	}

      // Digits
      mean=m_CscStationDescription[idMap].getMeanNumberOfVersion2(tmpPass,tmpTotal);
      if (mean>=0.0)
	{
	  vtRes_version2_csc_id.add(id,tmpPass,tmpTotal);
	  vtRes_version2_csc_phi.add(phi,tmpPass,tmpTotal);
	  vtRes_version2_csc_eta.add(eta,tmpPass,tmpTotal);
	  if (mean>0.0) msRootInterface.setGraphPoint("CSC_Average_UniqueID_2",idMap,mean,0.001,0.001);
	}

    }
  for (int id=vtStationID_Csc.getMinStationName(); id<=vtStationID_Csc.getMaxStationName(); id++)
    {
      vtRes_version2_csc_id.getMean(id,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("CSC_Average_Name_2",id,mean,0.001,error);
      vtRes_version1_csc_id.getMean(id,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("CSC_Average_Name_1",id,mean,0.001,error);
    }
  for (int eta=vtStationID_Csc.getMinStationEta(); eta<=vtStationID_Csc.getMaxStationEta(); eta++)
    {
      vtRes_version2_csc_eta.getMean(eta,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("CSC_Average_Eta_2",eta,mean,0.001,error);
      vtRes_version1_csc_eta.getMean(eta,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("CSC_Average_Eta_1",eta,mean,0.001,error);
    }
  for (int phi=vtStationID_Csc.getMinStationPhi(); phi<=vtStationID_Csc.getMaxStationPhi(); phi++)
    {
      vtRes_version2_csc_phi.getMean(phi,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("CSC_Average_Phi_2",phi,mean,0.001,error);
      vtRes_version1_csc_phi.getMean(phi,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("CSC_Average_Phi_1",phi,mean,0.001,error);
    }

  return StatusCode::SUCCESS;
}


StatusCode MVCombined::Generate_TGC_Histograms()
{
  int id, eta, phi;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Generate TGC Histograms" << endreq;

  MVCommonStationID vtStationID_Tgc("TGC");

  MVCommonResultDescription vtRes_version1_tgc_id(vtStationID_Tgc.getMinStationName(),vtStationID_Tgc.getMaxStationName());
  MVCommonResultDescription vtRes_version1_tgc_phi(vtStationID_Tgc.getMinStationPhi(),vtStationID_Tgc.getMaxStationPhi());
  MVCommonResultDescription vtRes_version1_tgc_eta(vtStationID_Tgc.getMinStationEta(),vtStationID_Tgc.getMaxStationEta());
  MVCommonResultDescription vtRes_version2_tgc_id(vtStationID_Tgc.getMinStationName(),vtStationID_Tgc.getMaxStationName());
  MVCommonResultDescription vtRes_version2_tgc_phi(vtStationID_Tgc.getMinStationPhi(),vtStationID_Tgc.getMaxStationPhi());
  MVCommonResultDescription vtRes_version2_tgc_eta(vtStationID_Tgc.getMinStationEta(),vtStationID_Tgc.getMaxStationEta());
  MVCommonResultDescription vtRes_asso_version1_tgc_id(vtStationID_Tgc.getMinStationName(),vtStationID_Tgc.getMaxStationName());
  MVCommonResultDescription vtRes_asso_version1_tgc_eta(vtStationID_Tgc.getMinStationEta(),vtStationID_Tgc.getMaxStationEta());
  MVCommonResultDescription vtRes_asso_version2_tgc_id(vtStationID_Tgc.getMinStationName(),vtStationID_Tgc.getMaxStationName());
  MVCommonResultDescription vtRes_asso_version2_tgc_eta(vtStationID_Tgc.getMinStationEta(),vtStationID_Tgc.getMaxStationEta());
  MVCommonResiduumResults mvResiduumResult;

  int idMap;
  int __attribute__((__unused__)) internID;
  float mean, error;

  long tmpPass;
  long tmpTotal;


  // TGC
  internID=0;
  for (idMap=0; idMap<=vtStationID_Tgc.getMaxIDNumber(); idMap++)
    {
      vtStationID_Tgc.getNameIDEtaPhi(idMap, id, eta, phi);

      // Hits
      mean=m_TgcStationDescription[idMap].getMeanNumberOfVersion1(tmpPass,tmpTotal);
      if (mean>=0.0)
	{
	  vtRes_version1_tgc_id.add(id,tmpPass,tmpTotal);
	  vtRes_version1_tgc_phi.add(phi,tmpPass,tmpTotal);
	  vtRes_version1_tgc_eta.add(eta,tmpPass,tmpTotal);
	  if (mean>0.0) msRootInterface.setGraphPoint("TGC_Average_UniqueID_1",idMap,mean,0.001,0.001);
	}

      // Digits
      mean=m_TgcStationDescription[idMap].getMeanNumberOfVersion2(tmpPass,tmpTotal);
      if (mean>=0.0)
	{
	  vtRes_version2_tgc_id.add(id,tmpPass,tmpTotal);
	  vtRes_version2_tgc_phi.add(phi,tmpPass,tmpTotal);
	  vtRes_version2_tgc_eta.add(eta,tmpPass,tmpTotal);
	  if (mean>0.0) msRootInterface.setGraphPoint("TGC_Average_UniqueID_2",idMap,mean,0.001,0.001);
	}
    }


  for (int id=vtStationID_Tgc.getMinStationName(); id<=vtStationID_Tgc.getMaxStationName(); id++)
    {
      vtRes_version2_tgc_id.getMean(id,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("TGC_Average_Name_2",id,mean,0.001,error);
      vtRes_version1_tgc_id.getMean(id,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("TGC_Average_Name_1",id,mean,0.001,error);

    }
  for (int eta=vtStationID_Tgc.getMinStationEta(); eta<=vtStationID_Tgc.getMaxStationEta(); eta++)
    {
      vtRes_version2_tgc_eta.getMean(eta,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("TGC_Average_Eta_2",eta,mean,0.001,error);
      vtRes_version1_tgc_eta.getMean(eta,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("TGC_Average_Eta_1",eta,mean,0.001,error);
    }
  for (int phi=vtStationID_Tgc.getMinStationPhi(); phi<=vtStationID_Tgc.getMaxStationPhi(); phi++)
    {
      vtRes_version2_tgc_phi.getMean(phi,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("TGC_Average_Phi_2",phi,mean,0.001,error);
      vtRes_version1_tgc_phi.getMean(phi,mean,error);
      if (mean>0.0) msRootInterface.setGraphPoint("TGC_Average_Phi_1",phi,mean,0.001,error);
    }

  return StatusCode::SUCCESS;
}


StatusCode MVCombined::GenerateSingleChamberHistograms(int id, int eta, int phi)
{
  int __attribute__((__unused__)) useless = id+eta+phi;// useless=1; //making the compiler happy
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Generate Histograms for Single Chamber" << endreq;

  /*	MVCommonStationID vtStationID;

  long idMap;

  vtStationID.m_StationName=id;
  vtStationID.m_StationEta=eta;
  vtStationID.m_StationPhi=phi;
  idMap=vtStationID.getLongIDNumber();

  m_MdtStationDescription[idMap].fillDataInHistoGramm(m_1D_Histos["Chamber_Version1"], 1);
  m_MdtStationDescription[idMap].fillDataInHistoGramm(m_1D_Histos["Chamber_Version2"], 2);

  MVCommonResiduumResults mvResiduumResult =
  m_MdtStationDescription[idMap].getMeanDifference(m_minimalDifference);
  if ((int)(mvResiduumResult.vResZ0.size())>0)
  {
  for (int n=0; n<(int)(mvResiduumResult.vResZ0.size()); n++)
  m_1D_Histos["Chamber_Res_Z"]->fill(mvResiduumResult.vResZ0[n]);
  }
  if ((int)(mvResiduumResult.vResRadius.size())>0)
  {
  for (int n=0; n<(int)(mvResiduumResult.vResRadius.size()); n++)
  m_1D_Histos["Chamber_Res_R"]->fill(mvResiduumResult.vResRadius[n]);
  }
  */
  return StatusCode::SUCCESS;
}


StatusCode MVCombined::loadDataFiles()
{
  int ntyp=1;
  if (m_FileType1=="Hits") 	ntyp=1;
  if (m_FileType1=="Digits")	ntyp=2;

  std::cout << "...loading...." << std::endl;
  vtLoader.excludeDoubleCounting(m_excludeDoubleCounting);
  vtLoader.loadDataFile(ntyp,m_InputVersion1File,1,m_MdtStationDescription,m_RpcStationDescription,m_CscStationDescription,m_TgcStationDescription);

  if (m_FileType2=="Hits") 	ntyp=1;
  if (m_FileType2=="Digits")	ntyp=2;
  vtLoader.loadDataFile(ntyp,m_InputVersion2File,2,m_MdtStationDescription,m_RpcStationDescription,m_CscStationDescription,m_TgcStationDescription);

  return StatusCode::SUCCESS;
}

ITHistSvc* MVCombined::ToolRootHistSvc()
{
  MsgStream log(msgSvc(), name());
  StatusCode sc = service("THistSvc",m_rootsvc, true);
  if( sc.isFailure() )
    {
      log << MSG::WARNING << ">>> Unable to locate the MDTDigitsTestAlg Histogram service" << endreq;
    }
  return m_rootsvc;
}
