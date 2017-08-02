/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * NAME : 	SimpleSuperCellChecks.cxx
 * PACKAGE : 	Trigger/L1CaloUpgrade/SimpleSuperCellChecks
 *
 * AUTHOR : 	Denis Oliveira Damazio
 *
 *
 * **/

#include "SimpleSuperCellChecks.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH1I.h"
#include <math.h>
#include "TrigL1CaloUpgrade/BCID.h"

SimpleSuperCellChecks::SimpleSuperCellChecks( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm (name, pSvcLocator), m_scidtool("CaloSuperCellIDTool")  {
}

SimpleSuperCellChecks::~SimpleSuperCellChecks(){}

StatusCode SimpleSuperCellChecks::initialize(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "initializing SimpleSuperCellChecks" << endreq;
	std::string filename=name();
	filename+=".BasicCheck.root";
        counter=0;
	m_file = new TFile (filename.c_str(),"RECREATE");
	m_nSCells = new TH1I("nSCells","nSCells",400,0,40000);
	m_EtSCells = new TH1F("EtSCells","EtSCells",60,0,30e3);
        m_TimeSCells = new TH1F("TimeSCells","TimeSCells",50,-25.0,25.0);
	m_TimeSCellsAT = new TH1F("TimeSCellsAT","TimeSCells Above Threshold",50,-25.0,25.0);
        m_QualitySCells = new TH1F  ("QualitySCells","QualitySCells",65536,0,65536);
	m_QualitySCellsAT = new TH1F("QualitySCellsAT","QualitySCells Above Threshold",65536,0,65536);

	m_TimeResol = new TH1F("TimeResol","Time Resolution",200,-100.0,100.0);
	m_TimeResolAT = new TH1F("TimeResolAT","Time Resolution above threshold",200,-100.0,100.0);
	m_TimeResol_vs_pt = new TH2F("TimeResol_vs_pt","Time Resolution versus pt",100,0,50,200,-100.0,100.0);
	m_TimeResol_vs_time = new TH2F("TimeResol_vs_time","Time Resolution versus time",100,-50.0,50.0,200,-100.0,100.0);
	m_TimeComp = new TH2F("TimeComp","Time Comparison",240,-30,30,240,-30,30);
	m_TimeCompWide = new TH2F("TimeCompWide","Time Comparison Wide",240,-120,120,240,-120,120);

        m_TimeResolW = new TH1F("TimeResolW","Time Resolution",200,-100.0,100.0);
        m_TimeResolWAT = new TH1F("TimeResolWAT","Time Resolution above threshold",200,-100.0,100.0);
        m_TimeResolW_vs_pt = new TH2F("TimeResolW_vs_pt","Time Resolution versus pt",100,0,50,200,-100.0,100.0);
	m_TimeResolW_vs_time = new TH2F("TimeResolW_vs_time","Time Weighted Resolution versus time",100,-50.0,50.0,200,-100.0,100.0);
        m_TimeCompW = new TH2F("TimeCompW","Time Comparison",240,-30,30,240,-30,30);
        m_TimeCompWWide = new TH2F("TimeCompWWide","Time Weighted Comparison Wide",240,-120,120,240,-120,120);

	m_QualityNCells = new TH1F("QualNSCells","Number of the Cells with good quality",140,0,35000);
	m_PrimaryVtx = new TH1F("PrimaryVertex","Primary Vertex",120,0,120);

	m_file->mkdir("EMB");
	m_file->cd("EMB");

	m_nSCells_perLayer.push_back( new TH1I( "nSCells_PreSamplerB","nSCells PreSamplerB",50,1900,1950) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_EMB1","nSCells EMB1",50,7525,7575) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_EMB2","nSCells EMB2",50,7280,7330) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_EMB3","nSCells EMB3",50,1760,1810) );

        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_PreSamplerB", "EtCells PreSamplerB",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_EMB1", "EtCells EMB1",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_EMB2", "EtCells EMB2",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_EMB3", "EtCells EMB3",60,0,10e3) );

        m_EtaSCells_perLayer.push_back( new TH1F( "EtaCells_PreSamplerB", "EtaCells PreSamplerB",30,-1.5,1.5) );
        m_EtaSCells_perLayer.push_back( new TH1F( "EtaCells_EMB1", "EtaCells EMB1",60,-1.5,1.5) );
        m_EtaSCells_perLayer.push_back( new TH1F( "EtaCells_EMB2", "EtaCells EMB2",60,-1.5,1.5) );
        m_EtaSCells_perLayer.push_back( new TH1F( "EtaCells_EMB3", "EtaCells EMB3",30,-1.5,1.5) );

        m_PhiSCells_perLayer.push_back( new TH1F( "PhiCells_PreSamplerB", "PhiCells PreSamplerB",64,-M_PI,M_PI) );
        m_PhiSCells_perLayer.push_back( new TH1F( "PhiCells_EMB1", "PhiCells EMB1",64,-M_PI,M_PI) );
        m_PhiSCells_perLayer.push_back( new TH1F( "PhiCells_EMB2", "PhiCells EMB2",64,-M_PI,M_PI) );
        m_PhiSCells_perLayer.push_back( new TH1F( "PhiCells_EMB3", "PhiCells EMB3",64,-M_PI,M_PI) );

	m_resolution_perLayer.push_back( new TH1F("resolution_PreSamplerB","resolution PreSamplerB",60,-30,30) );
	m_resolution_perLayer.push_back( new TH1F("resolution_EMB1","resolution EMB1",60,-30,30) );
	m_resolution_perLayer.push_back( new TH1F("resolution_EMB2","resolution EMB2",60,-30,30) );
	m_resolution_perLayer.push_back( new TH1F("resolution_EMB3","resolution EMB3",60,-30,30) );

	m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_PreSamplerB","resolution PreSamplerB",60,-30,30) );
	m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_EMB1","resolution EMB1",60,-30,30) );
	m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_EMB2","resolution EMB2",60,-30,30) );
	m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_EMB3","resolution EMB3",60,-30,30) );

	m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_PreSamplerB","resolution PreSamplerB",60,-30,30) );
	m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_EMB1","resolution EMB1",60,-30,30) );
	m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_EMB2","resolution EMB2",60,-30,30) );
	m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_EMB3","resolution EMB3",60,-30,30) );

	m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_PreSamplerB","resol vs et PreSamplerB",120,0,60,60,-30,30) );
	m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_EMB1","resol vs et EMB1",120,0,60,60,-30,30) );
	m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_EMB2","resol vs et EMB2",120,0,60,60,-30,30) );
	m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_EMB3","resol vs et EMB3",120,0,60,60,-30,30) );

	m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_PreSamplerB","resol vs #eta PreSamplerB",30,-1.5,1.5,60,-30,30) );
	m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_EMB1","resol vs #eta EMB1",60,-1.5,1.5,60,-30,30) );
	m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_EMB2","resol vs #eta EMB2",60,-1.5,1.5,60,-30,30) );
	m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_EMB3","resol vs #eta EMB3",30,-1.5,1.5,60,-30,30) );

        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_PreSamplerB","time resol PreSamplerB",200,-100.0,100.0) );
        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_EMB1","time resol EMB1",200,-100.0,100.0) );
        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_EMB2","time resol EMB2",200,-100.0,100.0) );
        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_EMB3","time resol EMB3",200,-100.0,100.0) );

	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_PreSamplerB","resol vs nvtx PreSamplerB",120,0,120,60,-30,30) );
	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_EMB1","resol vs nvtx EMB1",120,0,120,60,-30,30) );
	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_EMB2","resol vs nvtx EMB2",120,0,120,60,-30,30) );
	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_EMB3","resol vs nvtx EMB3",120,0,120,60,-30,30) );

	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_PreSamplerB","resol vs bcid PreSamplerB",3500,0,3500,60,-30,30) );
	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_EMB1","resol vs bcid EMB1",3500,0,3500,60,-30,30) );
	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_EMB2","resol vs bcid EMB2",3500,0,3500,60,-30,30) );
	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_EMB3","resol vs bcid EMB3",3500,0,3500,60,-30,30) );

	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_PreSamplerB","resol vs bcidN PreSamplerB",150,0,150,60,-30,30) );
	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_EMB1","resol vs bcidN EMB1",150,0,150,60,-30,30) );
	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_EMB2","resol vs bcidN EMB2",150,0,150,60,-30,30) );
	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_EMB3","resol vs bcidN EMB3",150,0,150,60,-30,30) );

	m_file->cd("/");
	m_file->mkdir("EMEC");
	m_file->cd("EMEC");
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_PreSamplerE","nSCells PreSamplerE",50,350,400) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_EME1","nSCells EME1",50,5350,5400) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_EME2","nSCells EME2",50,5850,5900) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_EME3","nSCells EME3",50,1500,1550) );

        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_PreSamplerE", "EtCells PreSamplerE",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_EME1", "EtCells EME1",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_EME2", "EtCells EME2",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_EME3", "EtCells EME3",60,0,10e3) );

        m_EtaSCells_perLayer.push_back( new TH1F( "EtaCells_PreSamplerE", "EtaCells PreSamplerE",38,-1.9,1.9) );
        m_EtaSCells_perLayer.push_back( new TH1F( "EtaCells_EME1", "EtaCells EME1",104,-2.6,2.6) );
        m_EtaSCells_perLayer.push_back( new TH1F( "EtaCells_EME2", "EtaCells EME2",104,-2.6,2.6) );
        m_EtaSCells_perLayer.push_back( new TH1F( "EtaCells_EME3", "EtaCells EME3",104,-2.6,2.6) );

        m_PhiSCells_perLayer.push_back( new TH1F( "PhiCells_PreSamplerE", "PhiCells PreSamplerE",64,-M_PI,M_PI) );
        m_PhiSCells_perLayer.push_back( new TH1F( "PhiCells_EME1", "PhiCells EME1",64,-M_PI,M_PI) );
        m_PhiSCells_perLayer.push_back( new TH1F( "PhiCells_EME2", "PhiCells EME2",64,-M_PI,M_PI) );
        m_PhiSCells_perLayer.push_back( new TH1F( "PhiCells_EME3", "PhiCells EME3",64,-M_PI,M_PI) );

	m_resolution_perLayer.push_back( new TH1F("resolution_PreSamplerE","resolution PreSamplerE",60,-30,30) );
	m_resolution_perLayer.push_back( new TH1F("resolution_EME1","resolution EME1",60,-30,30) );
	m_resolution_perLayer.push_back( new TH1F("resolution_EME2","resolution EME2",60,-30,30) );
	m_resolution_perLayer.push_back( new TH1F("resolution_EME3","resolution EME3",60,-30,30) );

	m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_PreSamplerE","resolution PreSamplerE",60,-30,30) );
	m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_EME1","resolution EME1",60,-30,30) );
	m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_EME2","resolution EME2",60,-30,30) );
	m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_EME3","resolution EME3",60,-30,30) );

	m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_PreSamplerE","resolution PreSamplerE",60,-30,30) );
	m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_EME1","resolution EME1",60,-30,30) );
	m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_EME2","resolution EME2",60,-30,30) );
	m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_EME3","resolution EME3",60,-30,30) );

	m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_PreSamplerE","resol vs et PreSamplerE",120,0,60,60,-30,30) );
	m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_EME1","resol vs et EME1",120,0,60,60,-30,30) );
	m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_EME2","resol vs et EME2",120,0,60,60,-30,30) );
	m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_EME3","resol vs et EME3",120,0,60,60,-30,30) );

	m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_PreSamplerE","resol vs #eta PreSamplerE",50,-5,5,60,-30,30) );
	m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_EME1","resol vs #eta EME1",50,-5,5,60,-30,30) );
	m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_EME2","resol vs #eta EME2",50,-5,5,60,-30,30) );
	m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_EME3","resol vs #eta EME3",50,-5,5,60,-30,30) );

	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_PreSamplerE","resol vs nvtx PreSamplerE",120,0,120,60,-30,30) );
	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_EME1","resol vs nvtx EME1",120,0,120,60,-30,30) );
	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_EME2","resol vs nvtx EME2",120,0,120,60,-30,30) );
	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_EME3","resol vs nvtx EME3",120,0,120,60,-30,30) );

	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_PreSamplerE","resol vs bcid PreSamplerE",3500,0,3500,60,-30,30) );
	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_EME1","resol vs bcid EME1",3500,0,3500,60,-30,30) );
	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_EME2","resol vs bcid EME2",3500,0,3500,60,-30,30) );
	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_EME3","resol vs bcid EME3",3500,0,3500,60,-30,30) );

	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_PreSamplerE","resol vs bcidN PreSamplerE",150,0,150,60,-30,30) );
	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_EME1","resol vs bcidN EME1",150,0,150,60,-30,30) );
	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_EME2","resol vs bcidN EME2",150,0,150,60,-30,30) );
	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_EME3","resol vs bcidN EME3",150,0,150,60,-30,30) );

        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_PreSamplerE","time resol PreSamplerE",200,-100.0,100.0) );
        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_EME1","time resol EME1",200,-100.0,100.0) );
        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_EME2","time resol EME2",200,-100.0,100.0) );
        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_EME3","time resol EME3",200,-100.0,100.0) );

	m_file->cd("/");
	m_file->mkdir("HEC");
	m_file->cd("HEC");
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_HEC","nSCells HEC",50,0,2000) );

        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_HEC", "EtCells HEC",60,0,10e3) );
        m_EtaSCells_perLayer.push_back( new TH1F( "EtaCells_HEC", "EtaCells HEC",66,-3.3,3.3) );
        m_PhiSCells_perLayer.push_back( new TH1F( "PhiCells_HEC", "PhiCells HEC",64,-M_PI,M_PI) );

	m_resolution_perLayer.push_back( new TH1F("resolution_HEC","resolution HEC",60,-30,30) );
	m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_HEC","resolution HEC",60,-30,30) );
	m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_HEC","resolution HEC",60,-30,30) );
	m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_HEC","resol vs et HEC",120,0,60,60,-30,30) );
	m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_HEC","resol vs #eta HEC",50,-5,5,60,-30,30) );
	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_HEC","resol vs nvtx HEC",120,0,120,60,-30,30) );
	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_HEC","resol vs bcid HEC",3500,0,3500,60,-30,30) );
	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_HEC","resol vs bcidN HEC",150,0,150,60,-30,30) );


        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_HEC","time resol HEC",200,-100.0,100.0) );

	m_file->cd("/");
	m_file->mkdir("FCAL");
	m_file->cd("FCAL");
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_FCAL0","nSCells FCAL0",50,350,400) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_FCAL1","nSCells FCAL1",50,230,280) );
	m_nSCells_perLayer.push_back( new TH1I( "nSCells_FCAL2","nSCells FCAL2",50,100,150) );

        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_FCAL0", "EtCells FCAL0",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_FCAL1", "EtCells FCAL1",60,0,10e3) );
        m_EtSCells_perLayer.push_back( new TH1F( "EtCells_FCAL2", "EtCells FCAL2",60,0,10e3) );

        m_EtaSCells_perLayer.push_back( new TH1F( "EtaCells_FCAL0", "EtaCells FCAL0",100,-5,5) );
        m_EtaSCells_perLayer.push_back( new TH1F( "EtaCells_FCAL1", "EtaCells FCAL1",100,-5,5) );
        m_EtaSCells_perLayer.push_back( new TH1F( "EtaCells_FCAL2", "EtaCells FCAL2",100,-5,5) );

        m_PhiSCells_perLayer.push_back( new TH1F( "PhiCells_FCAL0", "PhiCells FCAL0",64,-M_PI,M_PI) );
        m_PhiSCells_perLayer.push_back( new TH1F( "PhiCells_FCAL1", "PhiCells FCAL1",64,-M_PI,M_PI) );
        m_PhiSCells_perLayer.push_back( new TH1F( "PhiCells_FCAL2", "PhiCells FCAL2",64,-M_PI,M_PI) );


	m_resolution_perLayer.push_back( new TH1F("resolution_FCAL0","resolution FCAL0",60,-30,30) );
	m_resolution_perLayer.push_back( new TH1F("resolution_FCAL1","resolution FCAL1",60,-30,30) );
	m_resolution_perLayer.push_back( new TH1F("resolution_FCAL2","resolution FCAL2",60,-30,30) );

	m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_FCAL0","resolution FCAL0",60,-30,30) );
	m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_FCAL1","resolution FCAL1",60,-30,30) );
	m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_FCAL2","resolution FCAL2",60,-30,30) );

	m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_FCAL0","resolution FCAL0",60,-30,30) );
	m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_FCAL1","resolution FCAL1",60,-30,30) );
	m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_FCAL2","resolution FCAL2",60,-30,30) );

	m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_FCAL0","resol vs et FCAL0",120,0,60,60,-30,30) );
	m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_FCAL1","resol vs et FCAL1",120,0,60,60,-30,30) );
	m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_FCAL2","resol vs et FCAL2",120,0,60,60,-30,30) );

	m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_FCAL0","resol vs #eta FCAL0",50,-5,5,60,-30,30) );
	m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_FCAL1","resol vs #eta FCAL1",50,-5,5,60,-30,30) );
	m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_FCAL2","resol vs #eta FCAL2",50,-5,5,60,-30,30) );

	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_FCAL0","resol vs nvtx FCAL0",120,0,120,60,-30,30) );
	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_FCAL1","resol vs nvtx FCAL1",120,0,120,60,-30,30) );
	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_FCAL2","resol vs nvtx FCAL2",120,0,120,60,-30,30) );

	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_FCAL0","resol vs bcid FCAL0",3500,0,3500,60,-30,30) );
	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_FCAL1","resol vs bcid FCAL1",3500,0,3500,60,-30,30) );
	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_FCAL2","resol vs bcid FCAL2",3500,0,3500,60,-30,30) );

	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_FCAL0","resol vs bcidN FCAL0",150,0,150,60,-30,30) );
	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_FCAL1","resol vs bcidN FCAL1",150,0,150,60,-30,30) );
	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_FCAL2","resol vs bcidN FCAL2",150,0,150,60,-30,30) );

        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_FCAL0","time resol FCAL0",200,-100.0,100.0) );
        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_FCAL1","time resol FCAL1",200,-100.0,100.0) );
        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_FCAL2","time resol FCAL2",200,-100.0,100.0) );


	m_file->cd("/");
	m_file->mkdir("EMEC2");
	m_file->cd("EMEC2");
        m_resolution_perLayer.push_back( new TH1F("resolution_PreSamplerE2","resolution PreSamplerE2",60,-30,30) );
        m_resolution_perLayer.push_back( new TH1F("resolution_EME12","resolution EME12",60,-30,30) );
        m_resolution_perLayer.push_back( new TH1F("resolution_EME22","resolution EME22",60,-30,30) );
        m_resolution_perLayer.push_back( new TH1F("resolution_EME32","resolution EME32",60,-30,30) );

        m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_PreSamplerE2","resolution PreSamplerE2",60,-30,30) );
        m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_EME12","resolution EME12",60,-30,30) );
        m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_EME22","resolution EME22",60,-30,30) );
        m_resolutionMEt_perLayer.push_back( new TH1F("resolutionMEt_EME32","resolution EME32",60,-30,30) );

        m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_PreSamplerE2","resolution PreSamplerE2",60,-30,30) );
        m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_EME12","resolution EME12",60,-30,30) );
        m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_EME22","resolution EME22",60,-30,30) );
        m_resolutionBCID_perLayer.push_back( new TH1F("resolutionBCID_EME32","resolution EME32",60,-30,30) );

        m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_PreSamplerE2","resol vs et PreSamplerE2",120,0,60,60,-30,30) );
        m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_EME12","resol vs et EME12",120,0,60,60,-30,30) );
        m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_EME22","resol vs et EME22",120,0,60,60,-30,30) );
        m_resol_vs_et_perLayer.push_back( new TH2F("resol_vs_et_EME32","resol vs et EME32",120,0,60,60,-30,30) );

        m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_PreSamplerE2","resol vs #eta PreSamplerE2",50,-5,5,60,-30,30) );
        m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_EME12","resol vs #eta EME12",50,-5,5,60,-30,30) );
        m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_EME22","resol vs #eta EME22",50,-5,5,60,-30,30) );
        m_resol_vs_eta_perLayer.push_back( new TH2F("resol_vs_eta_EME32","resol vs #eta EME32",50,-5,5,60,-30,30) );

	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_PreSamplerE2","resol vs nvtx PreSamplerE2",120,0,120,60,-30,30) );
	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_EME12","resol vs nvtx EME12",120,0,120,60,-30,30) );
	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_EME22","resol vs nvtx EME22",120,0,120,60,-30,30) );
	m_resol_vs_nvtx_perLayer.push_back( new TH2F("resol_vs_nvtx_EME32","resol vs nvtx EME32",120,0,120,60,-30,30) );

	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_PreSamplerE2","resol vs bcid PreSamplerE2",120,0,120,60,-30,30) );
	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_EME12","resol vs bcid EME12",120,0,120,60,-30,30) );
	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_EME22","resol vs bcid EME22",120,0,120,60,-30,30) );
	m_resol_vs_bcid_perLayer.push_back( new TH2F("resol_vs_bcid_EME32","resol vs bcid EME32",120,0,120,60,-30,30) );

	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_PreSamplerE2","resol vs bcidN PreSamplerE2",150,0,150,60,-30,30) );
	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_EME12","resol vs bcidN EME12",150,0,150,60,-30,30) );
	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_EME22","resol vs bcidN EME22",150,0,150,60,-30,30) );
	m_resol_vs_bcidN_perLayer.push_back( new TH2F("resol_vs_bcidN_EME32","resol vs bcidN EME32",150,0,150,60,-30,30) );

        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_PreSamplerE2","time resol PreSamplerE2",200,-100.0,100.0) );
        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_EME12","time resol EME12",200,-100.0,100.0) );
        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_EME22","time resol EME22",200,-100.0,100.0) );
        m_TimeResolAT_perLayer.push_back( new TH1F("timeResol_AT_EME32","time resol EME32",200,-100.0,100.0) );

	m_file->cd("/");
	m_resolution = new TH1F("resolution","resolution",60,-30,30);
	m_resol_vs_et = new TH2F("resol_vs_et","resol vs et",120,0,60,60,-30,30);
	m_resol_vs_eta = new TH2F("resol_vs_eta","resol vs #eta",50,-5,5,60,-30,30);
	m_resol_vs_nvtx = new TH2F("resol_vs_nvtx","resol vs nvtx",120,0,120,60,-30,30);
	m_resol_vs_bcid = new TH2F("resol_vs_bcid","resol vs bcid",3500,0,3500,60,-30,30);
	m_resol_vs_bcidN = new TH2F("resol_vs_bcidN","resol vs bcidN",150,0,150,60,-30,30);

	// for cell <-> SCell comparison
	if ( m_scidtool.retrieve().isFailure() ){
		msg << MSG::ERROR << "cannot perform comparisons between cell and SuperCells" << endreq;
	}

	return StatusCode::SUCCESS;
}

StatusCode SimpleSuperCellChecks::finalize(){
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "finalizing SimpleSuperCellChecks" << endreq;
	m_file->Write();
	m_file->Close();
	return StatusCode::SUCCESS;
}

StatusCode SimpleSuperCellChecks::execute(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "execute SimpleSuperCellChecks" << endreq;
	const xAOD::EventInfo* evt(0);
	if ( evtStore()->retrieve(evt,"EventInfo").isFailure() ){
		msg << MSG::WARNING << "did not find EventInfo container" << endreq;
	}
	long bunch_crossing(-1);
	long bunch_crossingNor(-1);
	if ( evt ) {
	   bunch_crossing = evt->bcid();
	   bunch_crossingNor = bcids_from_start ( bunch_crossing );
	}
        const CaloCellContainer* scells;
        const CaloCellContainer* allcalo(NULL);
	if ( evtStore()->retrieve(scells,"SCell").isFailure() ){
		msg << MSG::WARNING << "did not find cell container" << endreq;
		return StatusCode::SUCCESS;
	}
	if ( evtStore()->retrieve(allcalo,"AllCalo").isFailure() ){
		msg << MSG::WARNING << "did not find cell container for regular cells, no resolution test possible" << endreq;
	}
	const xAOD::VertexContainer* nvtx(NULL);
	int nvtxs=0;
	if ( evtStore()->retrieve(nvtx,"PrimaryVertices").isFailure() ) {
		msg << MSG::WARNING << "did not find Vectices container" << endreq;
		return StatusCode::SUCCESS;
	}
	if ( nvtx != NULL) nvtxs = nvtx->size();
	m_PrimaryVtx->Fill ( nvtxs );
	unsigned int count_sCells=0;
	unsigned int count_sCells_Layer0=0;
	unsigned int count_sCells_Layer1=0;
	unsigned int count_sCells_Layer2=0;
	unsigned int count_sCells_Layer3=0;
	unsigned int count_sCells_Layer4=0;
	unsigned int count_sCells_Layer5=0;
	unsigned int count_sCells_Layer6=0;
	unsigned int count_sCells_Layer7=0;
	unsigned int count_sCells_Layer8=0;
	unsigned int count_sCells_Layer9=0;
	unsigned int count_sCells_Layer10=0;
	unsigned int count_sCells_Layer11=0;
        for(auto scell : *scells) {
		count_sCells++;
		m_EtSCells->Fill ( scell->et() );
		m_TimeSCells->Fill ( scell->time() );
		m_QualitySCells->Fill ( scell->provenance() );
		int index=-1;
		if ( scell->caloDDE()->getSampling() == CaloSampling::PreSamplerB ){
			count_sCells_Layer0++;
			index=0;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EMB1 ){
			count_sCells_Layer1++;
			index=1;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EMB2 ){
			count_sCells_Layer2++;
			index=2;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EMB3 ){
			count_sCells_Layer3++;
			index=3;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::PreSamplerE ){
			count_sCells_Layer4++;
			index=4;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EME1 ){
			count_sCells_Layer5++;
			index=5;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EME2 ){
			count_sCells_Layer6++;
			index=6;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EME3 ){
			count_sCells_Layer7++;
			index=7;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::HEC0 ){
			count_sCells_Layer8++;
			index=8;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::FCAL0 ){
			count_sCells_Layer9++;
			index=9;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::FCAL1 ){
			count_sCells_Layer10++;
			index=10;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::FCAL2 ){
			count_sCells_Layer11++;
			index=11;
		}
		if ( index > -1 ) {
			m_EtSCells_perLayer [index]->Fill( scell->et() );
			m_EtaSCells_perLayer[index]->Fill( scell->eta() );
			m_PhiSCells_perLayer[index]->Fill( scell->phi() );
		}
		if ( scell->et() < 3e3 ) continue;
	}
	m_nSCells->Fill( count_sCells );
	m_nSCells_perLayer[0]->Fill ( count_sCells_Layer0 );
	m_nSCells_perLayer[1]->Fill ( count_sCells_Layer1 );
	m_nSCells_perLayer[2]->Fill ( count_sCells_Layer2 );
	m_nSCells_perLayer[3]->Fill ( count_sCells_Layer3 );
	m_nSCells_perLayer[4]->Fill ( count_sCells_Layer4 );
	m_nSCells_perLayer[5]->Fill ( count_sCells_Layer5 );
	m_nSCells_perLayer[6]->Fill ( count_sCells_Layer6 );
	m_nSCells_perLayer[7]->Fill ( count_sCells_Layer7 );
	m_nSCells_perLayer[8]->Fill ( count_sCells_Layer8 );
	m_nSCells_perLayer[9]->Fill ( count_sCells_Layer9 );
	m_nSCells_perLayer[10]->Fill ( count_sCells_Layer10 );
	m_nSCells_perLayer[11]->Fill ( count_sCells_Layer11 );

	if ( !allcalo || !m_scidtool ) { // VERY IMPORTANT
		msg << MSG::ERROR << "Nothing more to be done, finish here" << endreq;
		return StatusCode::SUCCESS;
	}
	const CaloIdManager* m_calo_id_manager;
	if ( (detStore()->retrieve(m_calo_id_manager,"CaloIdManager")).isFailure() ){
		msg << MSG::ERROR << "Not able to map Calo IDs." << endreq;
		return StatusCode::SUCCESS;
	}
	const CaloCell_SuperCell_ID* calo_sc_id = m_calo_id_manager->getCaloCell_SuperCell_ID();
	std::vector<float> sc_ets; sc_ets.resize(40000,0.);
	std::vector<std::vector<float> > sc_etsk; sc_etsk.resize(40000);
	std::vector<std::vector<float> > sc_times; sc_times.resize(40000);
        for(auto cell : *allcalo) {
		int idx = calo_sc_id->calo_cell_hash( m_scidtool->offlineToSuperCellID ( cell->ID() ) );
		if ( idx < 0 or idx > 37000 ) { msg << MSG::DEBUG << "Problems with index : " << cell->ID().get_identifier32().get_compact() << " " << m_scidtool->offlineToSuperCellID ( cell->ID() ) << " " << idx << endreq; continue; }
		sc_ets[idx] += cell->et();
		sc_etsk[idx].push_back( cell->et() );
		sc_times[idx].push_back( cell->time() );
	}
	unsigned int cc=0;
	unsigned int ccQual=0;
        for(auto scell : *scells) {
		float SCell_et=scell->et();
		int idx = scell->caloDDE()->calo_hash();
		float resol = -999.;
		if ( scell->provenance() & 0x40 ) ccQual++;
		if ( sc_ets[idx] > 100 ) {
			// calculate resolution
			resol = sc_ets[idx] - SCell_et;
			resol *=100.0; // result in %
			resol /= sc_ets[idx];
			m_resolution->Fill(resol);
			m_resol_vs_et->Fill( sc_ets[idx] /1e3, resol);
			if ( sc_ets[idx] > 1000 ){
			m_resol_vs_eta->Fill( scell->eta(), resol);
			}
			m_resol_vs_nvtx->Fill( nvtxs, resol);
			m_resol_vs_bcid->Fill( bunch_crossing, resol);
			m_resol_vs_bcidN->Fill( bunch_crossingNor, resol);
		}
		float sc_time = 0.0;
		float sc_timew = 0.0;
		float sc_energy = 0.0;
		int n_times = 0;
		for(unsigned int ii=0; ii< sc_etsk[idx].size(); ii++){
			if ( fabsf(sc_times[idx][ii]) > 0.00001 ) {
				n_times++;
				sc_time += sc_times[idx][ii];
			}
                        if ( sc_etsk[idx][ii]>10 ) {
				sc_timew += sc_times[idx][ii]*sc_etsk[idx][ii];
				sc_energy += sc_etsk[idx][ii];
			}
		}
		if ( n_times > 0 ) sc_time /= (float)n_times ; else sc_time=0;
		if ( fabsf(sc_energy)>0.0001 ) sc_timew /= sc_energy; else sc_timew=0;
		float res_time = -110.0;
		if ( (fabsf(sc_time)> 0.3) && (fabsf(scell->time() )>0.3) ) res_time = 100.0*(scell->time()-sc_time ) / scell->time();
		float res_timew = -110.0;
		if ( (fabsf(sc_timew)>0.3) && (fabsf(scell->time() )>0.3) ) res_timew = 100.0*(scell->time()-sc_timew ) / scell->time();

		if ( ( fabsf( scell->eta() )<2.49 ) && ( scell->caloDDE()->getSampling() <= CaloSampling::EME3 ) ){
		m_TimeResol->Fill( res_time );
		m_TimeResol_vs_pt->Fill( scell->et()/1e3,res_time );
		m_TimeResol_vs_time->Fill( scell->time()*1e-3, res_time );
		if ( scell->et() > 4e3 ) {
			m_TimeResolAT->Fill( res_time );
			m_TimeComp->Fill( scell->time(), sc_time );
			m_TimeCompWide->Fill( scell->time(), sc_time );
		}
                m_TimeResolW->Fill( res_timew );
                m_TimeResolW_vs_pt->Fill( scell->et()/1e3,res_timew );
                m_TimeResolW_vs_time->Fill( scell->time()*1e-3,res_timew );
                if ( scell->et() > 4e3 ) {
                        m_TimeResolWAT->Fill( res_timew );
                        m_TimeCompW->Fill( scell->time(), sc_timew );
                        m_TimeCompWWide->Fill( scell->time(), sc_timew );
                }
		} // end of check of EM central region

		if ( fabsf( resol + 999 ) < 0.1 ) {cc++; continue;}
		int index=-1;
		int index2=0;
		if ( scell->caloDDE()->getSampling() == CaloSampling::PreSamplerB ){
			count_sCells_Layer0++;
			index=0;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EMB1 ){
			count_sCells_Layer1++;
			index=1;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EMB2 ){
			count_sCells_Layer2++;
			index=2;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EMB3 ){
			count_sCells_Layer3++;
			index=3;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::PreSamplerE ){
			count_sCells_Layer4++;
			index=4;
			if ( fabsf( scell->eta() ) > 2.49 ) index2=8;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EME1 ){
			count_sCells_Layer5++;
			index=5;
			if ( fabsf( scell->eta() ) > 2.49 ) index2=8;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EME2 ){
			count_sCells_Layer6++;
			index=6;
			if ( fabsf( scell->eta() ) > 2.49 ) index2=8;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::EME3 ){
			count_sCells_Layer7++;
			index=7;
			if ( fabsf( scell->eta() ) > 2.49 ) index2=8;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::HEC0 ){
			count_sCells_Layer8++;
			index=8;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::FCAL0 ){
			count_sCells_Layer9++;
			index=9;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::FCAL1 ){
			count_sCells_Layer10++;
			index=10;
		}
		if ( scell->caloDDE()->getSampling() == CaloSampling::FCAL2 ){
			count_sCells_Layer11++;
			index=11;
		}
		if ( index > -1 ) {
		m_resolution_perLayer[index+index2]->Fill(resol);
		m_resol_vs_et_perLayer[index+index2]->Fill(sc_ets[idx] /1e3, resol);
		if ( sc_ets[idx] > 1000 ){
			m_TimeSCellsAT->Fill ( scell->time() );
			m_QualitySCellsAT->Fill ( scell->provenance() );
			m_resolutionMEt_perLayer[index+index2]->Fill(resol);
			if ( scell->provenance() & 0x40 ) m_resolutionBCID_perLayer[index+index2]->Fill(resol);
			if ( sc_ets[idx] > 2000 ) {
			m_resol_vs_eta_perLayer[index+index2]->Fill( scell->eta(), resol);
			m_resol_vs_nvtx_perLayer[index+index2]->Fill( nvtxs, resol);
			m_resol_vs_bcid_perLayer[index+index2]->Fill( bunch_crossing, resol);
			m_resol_vs_bcidN_perLayer[index+index2]->Fill( bunch_crossingNor, resol);
			}
			if ( scell->et() > 4e3 ) {
				m_TimeResolAT_perLayer[index+index2]->Fill( res_time );
			}
		}
		}
		cc++;
		
		
	}
	m_QualityNCells->Fill( ccQual );
	
	
	counter++;
	return StatusCode::SUCCESS;
}

