/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTests/Analysis.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/LArID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawUtils/LArRawDetSelector.h"
#include "LArIdentifier/LArOnlineID.h"
#include "GeneratorObjects/McEventCollection.h"
#include "StoreGate/ReadHandle.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"
#include "AthenaKernel/ITriggerTime.h"




namespace MyAnalysis {

  //Constructor
  Analysis:: Analysis(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_thistSvc(0),
    m_hist_etraw_emb_s0(0),
    m_hist_etraw_emb_s1(0),
    m_hist_etraw_emb_s2(0),
    m_hist_etraw_emb_s3(0),
    m_hist_etraw_emec_s0(0),
    m_hist_etraw_emec_s1(0),
    m_hist_etraw_emec_s2(0),
    m_hist_etraw_emec_s3(0),
    m_hist_clusnoise(0),
    m_hist_clusnoise1(0),
    m_hist_clusnoise2(0),
    m_hist_clusnoise3(0),
    m_hist_ot(0),
    m_hist_ot_em(0),
    m_hist_ot_hec(0),
    m_hist_ot_fcal(0),
    m_hist_it(0),
    m_hist_it_em(0),
    m_hist_it_hec(0),
    m_hist_it_fcal(0),
    m_hist_hittime(0),
    m_hist_hitener(0),
    m_hist_nhitlar(0),
    m_hist_cal0(0),
    m_hist_cal1(0),
    m_hist_cal2(0),
    m_hist_cal3(0),
    m_triggerTimeTool(0),
    m_larem_id(0),
    m_calodm_id(0),
    m_ntuple(0)
  {

    m_clusternoise=false;
    m_raw=false;
    m_check=false;
    m_cell=false;
    m_hit=true;
    m_calhit=false;
    m_cluster=false;
    m_useTriggerTime    = false;
    m_doTruth=true;

    m_nevt = 0;
    m_nt_e = 0;
    m_nt_eta = 0;
    m_nt_phi = 0;
    m_nt_e0 = 0;
    m_nt_e1 = 0;
    m_nt_e2 = 0;
    m_nt_e3 = 0;
    m_nt_c0 = 0;
    m_nt_c1 = 0;
    m_nt_c2 = 0;
    m_nt_c3 = 0;
    m_nt_hb0 = 0;
    m_nt_hb1 = 0;
    m_nt_hb2 = 0;
    m_nt_hb3 = 0;
    m_nt_he0 = 0;
    m_nt_he1 = 0;
    m_nt_he2 = 0;
    m_nt_he3 = 0;
    m_nt_tb0 = 0;
    m_nt_tb1 = 0;
    m_nt_tb2 = 0;
    m_nt_tb3 = 0;
    m_nt_eclus = 0;
    m_nt_eclus0 = 0;
    m_nt_eclus1 = 0;
    m_nt_eclus2 = 0;
    m_nt_eclus3 = 0;
    m_nt_etaclus = 0;
    m_nt_phiclus = 0;
    m_nt_etaclus551 = 0;
    m_nt_etaclus552 = 0;
    m_nt_eclus35 = 0;
    m_nt_eclus37 = 0;
    m_nt_eclus350 = 0;
    m_nt_eclus351 = 0;
    m_nt_eclus352 = 0;
    m_nt_eclus353 = 0;
    m_nt_etaclus35 = 0;
    m_nt_etaclus351 = 0;
    m_nt_etaclus352 = 0;
    m_nt_etaclus371 = 0;
    m_nt_etaclus372 = 0;
    m_nt_eclusg = 0;
    m_nt_etaclusg551 = 0;
    m_nt_etaclusg552 = 0;
    m_nt_eclusg35 = 0;
    m_nt_etaclusg351 = 0;
    m_nt_etaclusg352 = 0;
    m_nt_eclusg37 = 0;
    m_nt_etaclusg371 = 0;
    m_nt_etaclusg372 = 0;
    m_nt_eActEM = 0;
    m_nt_eActNonEM = 0;
    m_nt_eActEscaped = 0;
    m_nt_eActInvisible = 0;
    m_nt_eInactEM = 0;
    m_nt_eInactNonEM = 0;
    m_nt_eInactEscaped = 0;
    m_nt_eInactInvisible = 0;
    m_nt_eDeadEM = 0;
    m_nt_eDeadNonEM = 0;
    m_nt_eDeadEscaped = 0;
    m_nt_eDeadInvisible = 0;
    m_nt_eTile = 0;
    m_nt_edead_1_0 = 0;
    m_nt_edead_1_1_0 = 0;
    m_nt_edead_1_1_1 = 0;
    m_nt_edead_1_1_2 = 0;
    m_nt_edead_1_1_3 = 0;
    m_nt_edead_1_1_4 = 0;
    m_nt_edead_1_1_5 = 0;
    m_nt_edead_1_1_6 = 0;
    m_nt_edead_1_1_7 = 0;
    m_nt_edead_1_2_0 = 0;
    m_nt_edead_1_2_1 = 0;
    m_nt_edead_1_2_2 = 0;
    m_nt_edead_1_2_3 = 0;
    m_nt_edead_1_2_4 = 0;
    m_nt_edead_1_2_5 = 0;
    m_nt_edead_1_3 = 0;
    m_nt_eleak = 0;
    m_xconv = 0;
    m_yconv = 0;
    m_zconv = 0;
    m_xvert = 0;
    m_yvert = 0;
    m_zvert = 0;
    

    declareProperty("ClusterNoise",m_clusternoise);
    declareProperty("Check",m_check);
    declareProperty("LoopRaw",m_raw);
    declareProperty("LoopHit",m_hit);
    declareProperty("LoopCalHit",m_calhit);
    declareProperty("LoopCell",m_cell);
    declareProperty("LoopCluster",m_cluster);
    declareProperty("UseTriggerTime",m_useTriggerTime);
    declareProperty("TriggerTimeToolName",m_triggerTimeToolName);
    declareProperty("DoTruth",m_doTruth);

  }
  
  //__________________________________________________________________________
  //Destructor
  Analysis::~Analysis()
  {
    ATH_MSG_DEBUG( "Analysis destructor called"  );
  }
  //__________________________________________________________________________
  StatusCode Analysis::initialize()
  {
    
    ATH_MSG_DEBUG("Analysis initialize()"  );

    //retrieve ID helpers 
    const CaloIdManager* mgr = nullptr;
    ATH_CHECK( detStore()->retrieve( mgr ) );
    m_larem_id   = mgr->getEM_ID();
    m_calodm_id  = mgr->getDM_ID();

//  retrieve CaloDetDescrMgr 
    const CaloDetDescrManager* calodetdescrmgr = nullptr;
    ATH_CHECK( detStore()->retrieve(calodetdescrmgr) );

// initialize hit list
    if (m_check) {
       int ncellem=m_larem_id->channel_hash_max();
       CellInfo cell0;
       cell0.bec=0;
       cell0.sampling=0;
       cell0.eta=0.; 
       cell0.phi=0.; 
       cell0.Ehit=-9999.;
       cell0.Thit=-9999.;
       cell0.Eraw=-9999.;
       cell0.Ecell=-9999.;
       cell0.identifier=0;
       m_CellListEM.resize(ncellem,cell0);

       const CaloDetDescrElement* calodde=0;
       for (int i=0;i<ncellem;i++){
         IdentifierHash idHash=i;
         Identifier id=m_larem_id->channel_id(idHash);
         int BvsEC=abs(m_larem_id->barrel_ec(id));
         int calotype;
         if (BvsEC == 1) calotype=0;
         else calotype=1;

         CellInfo mycell;
         mycell.bec=calotype;
         mycell.sampling= m_larem_id->sampling(id);
         calodde = calodetdescrmgr->get_element(id);
         mycell.eta=calodde->eta();
         mycell.phi=calodde->phi();
         mycell.Ehit=-9999.;
         mycell.Thit=-9999.;
         mycell.Eraw=-9999.;
         mycell.Ecell=-9999.;
         mycell.identifier=id;
         m_CellListEM[idHash]=mycell;
       }
    }   // m_check

    ATH_CHECK( m_cablingKey.initialize() );
    ATH_CHECK( service("THistSvc",m_thistSvc) );
    ATH_MSG_INFO( " retrieved THistSvc" );

    StatusCode sc;
    m_hist_etraw_emb_s0 = new TH1D("m_hist_etraw_emb_s0","E EMB S0",4000,-100.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_etraw_emb_s0",m_hist_etraw_emb_s0);
    m_hist_etraw_emb_s1 =  new TH1D("m_hist_etraw_emb_s1","E EMB S1",4000,-100.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_etraw_emb_s1",m_hist_etraw_emb_s1);
    m_hist_etraw_emb_s2 =  new TH1D("m_hist_etraw_emb_s2","E EMB S2",4000,-100.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_etraw_emb_s2",m_hist_etraw_emb_s2);
    m_hist_etraw_emb_s3 = new TH1D("m_hist_etraw_emb_s3","E EMB S3",4000,-100.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_etraw_emb_s3",m_hist_etraw_emb_s3);
    m_hist_etraw_emec_s0 = new TH1D("m_hist_etraw_emec_s0","E EMEC S0",4000,-400.,400.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_etraw_emec_s0",m_hist_etraw_emec_s0);
    m_hist_etraw_emec_s1 = new TH1D("m_hist_etraw_emec_s1","E EMEC S1",4000,-100.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_etraw_emec_s1",m_hist_etraw_emec_s1);
    m_hist_etraw_emec_s2 = new TH1D("m_hist_etraw_emec_s2","E EMEC S2",4000,-100.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_etraw_emec_s2",m_hist_etraw_emec_s2);
    m_hist_etraw_emec_s3 = new TH1D("m_hist_etraw_emec_s3","E EMEC S3",4000,-100.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_etraw_emec_s3",m_hist_etraw_emec_s3);

    m_hist_clusnoise = new TH1D("m_hist_clusnoise","Et in 5*3 EMB",400,-2000.,2000.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_clusnoise",m_hist_clusnoise);
    m_hist_clusnoise1 = new TH1D("m_hist_clusnoise1","Et in 5*3 0-0.5",400,-2000.,2000.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_clusnoise1",m_hist_clusnoise1);
    m_hist_clusnoise2 = new TH1D("m_hist_clusnoise2","Et in 5*3 0.5-1.",400,-2000.,2000.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_clusnoise2",m_hist_clusnoise2);
    m_hist_clusnoise3 = new TH1D("m_hist_clusnoise3","Et in 5*3 1-1.4",400,-2000.,2000.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_clusnoise3",m_hist_clusnoise3);

    m_hist_ot = new TH1D("m_hist_ot","E OT Larhit ",2000,0.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_ot",m_hist_ot);
    m_hist_ot_em = new TH1D("m_hist_ot_em","E EM OT Larhit ",2000,0.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_ot_em",m_hist_ot_em);
    m_hist_ot_hec = new TH1D("m_hist_ot_hec","E HEC OT Larhit ",2000,0.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_ot_hec",m_hist_ot_hec);
    m_hist_ot_fcal = new TH1D("m_hist_ot_fcal","E FCAL OT Larhit ",2000,0.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_ot_fcal",m_hist_ot_fcal);
    m_hist_it = new TH1D("m_hist_it","E IT Larhit ",2000,0.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_it",m_hist_it);
    m_hist_it_em = new TH1D("m_hist_it_em","E EM IT Larhit ",2000,0.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_it_em",m_hist_it_em);
    m_hist_it_hec = new TH1D("m_hist_it_hec","E HEC IT Larhit ",2000,0.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_it_hec",m_hist_it_hec);
    m_hist_it_fcal = new TH1D("m_hist_it_fcal","E FCAL IT Larhit ",2000,0.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_it_fcal",m_hist_it_fcal);


    m_hist_hittime = new TH1D("m_hist_hittime","Hit time",200,0.,200.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_hittime",m_hist_hittime);
    m_hist_hitener = new TH1D("m_hist_hitener","Hit Ener",500,0.,100.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_hitener",m_hist_hitener);
    m_hist_nhitlar = new TH1D("m_hist_nhitlar","Nhits LAr",1000,0.,100000.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_nhitlar",m_hist_nhitlar);

    m_hist_cal0 =  new TH1D("m_hist_cal0","Energy EM ",100,0.,10000.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_cal0",m_hist_cal0);
    m_hist_cal1 = new TH1D("m_hist_cal1","Energy NonEM ",100,0.,10000.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_cal1",m_hist_cal1);
    m_hist_cal2 = new TH1D("m_hist_cal2","Energy Invisible ",100,0.,10000.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_cal2",m_hist_cal2);
    m_hist_cal3 = new TH1D("m_hist_cal3","Energy Escaped ",100,0.,10000.);
    sc = m_thistSvc->regHist("/file1/analysis/m_hist_cal3",m_hist_cal3);

    m_ntuple = new TTree("m_ntuple","my ntuple");
    m_ntuple->Branch("etrue",&m_nt_e,"etrue/D");
    m_ntuple->Branch("eta",&m_nt_eta,"eta/D");
    m_ntuple->Branch("phi",&m_nt_phi,"phi/D");
    m_ntuple->Branch("eraw0",&m_nt_e0,"eraw0/D");
    m_ntuple->Branch("eraw1",&m_nt_e1,"eraw1/D");
    m_ntuple->Branch("eraw2",&m_nt_e2,"eraw2/D");
    m_ntuple->Branch("eraw3",&m_nt_e3,"eraw3/D");
    m_ntuple->Branch("ecell0",&m_nt_c0,"ecell0/D");
    m_ntuple->Branch("ecell1",&m_nt_c1,"ecell1/D");
    m_ntuple->Branch("ecell2",&m_nt_c2,"ecell2/D");
    m_ntuple->Branch("ecell3",&m_nt_c3,"ecell3/D");
    m_ntuple->Branch("ehitb0",&m_nt_hb0,"ehitb0/D");
    m_ntuple->Branch("ehitb1",&m_nt_hb1,"ehitb1/D");
    m_ntuple->Branch("ehitb2",&m_nt_hb2,"ehitb2/D");
    m_ntuple->Branch("ehitb3",&m_nt_hb3,"ehitb3/D");
    m_ntuple->Branch("ehite0",&m_nt_he0,"ehite0/D");
    m_ntuple->Branch("ehite1",&m_nt_he1,"ehite1/D");
    m_ntuple->Branch("ehite2",&m_nt_he2,"ehite2/D");
    m_ntuple->Branch("ehite3",&m_nt_he3,"ehite3/D");
    m_ntuple->Branch("timeb0",&m_nt_tb0,"timeb0/D");
    m_ntuple->Branch("timeb1",&m_nt_tb1,"timeb1/D");
    m_ntuple->Branch("timeb2",&m_nt_tb2,"timeb2/D");
    m_ntuple->Branch("timeb3",&m_nt_tb3,"timeb3/D");
    m_ntuple->Branch("eclus",&m_nt_eclus,"eclus/D");
    m_ntuple->Branch("eclus0",&m_nt_eclus0,"eclus0/D");
    m_ntuple->Branch("eclus1",&m_nt_eclus1,"eclus1/D");
    m_ntuple->Branch("eclus2",&m_nt_eclus2,"eclus2/D");
    m_ntuple->Branch("eclus3",&m_nt_eclus3,"eclus3/D");
    m_ntuple->Branch("etaclus",&m_nt_etaclus,"etaclus/D");
    m_ntuple->Branch("phiclus",&m_nt_phiclus,"phiclus/D");
    m_ntuple->Branch("etaclus551",&m_nt_etaclus551,"etaclus551/D");
    m_ntuple->Branch("etaclus552",&m_nt_etaclus552,"etaclus552/D");
    m_ntuple->Branch("eclus35",&m_nt_eclus35,"eclus35/D");
    m_ntuple->Branch("eclus350",&m_nt_eclus350,"eclus350/D");
    m_ntuple->Branch("eclus351",&m_nt_eclus351,"eclus351/D");
    m_ntuple->Branch("eclus352",&m_nt_eclus352,"eclus352/D");
    m_ntuple->Branch("eclus353",&m_nt_eclus353,"eclus353/D");
    m_ntuple->Branch("etaclus35",&m_nt_etaclus35,"etaclus35/D");
    m_ntuple->Branch("etaclus351",&m_nt_etaclus351,"etaclus351/D");
    m_ntuple->Branch("etaclus352",&m_nt_etaclus352,"etaclus352/D");
    m_ntuple->Branch("eclus37",&m_nt_eclus37,"eclus37/D");
    m_ntuple->Branch("etaclus371",&m_nt_etaclus371,"etaclus371/D");
    m_ntuple->Branch("etaclus372",&m_nt_etaclus372,"etaclus372/D");
    m_ntuple->Branch("eclusg",&m_nt_eclusg,"eclusg/D");
    m_ntuple->Branch("etaclusg551",&m_nt_etaclusg551,"etaclusg551/D");
    m_ntuple->Branch("etaclusg552",&m_nt_etaclusg552,"etaclusg552/D");
    m_ntuple->Branch("eclusg35",&m_nt_eclusg35,"eclusg35/D");
    m_ntuple->Branch("etaclusg351",&m_nt_etaclusg351,"etaclusg351/D");
    m_ntuple->Branch("etaclusg352",&m_nt_etaclusg352,"etaclusg352/D");
    m_ntuple->Branch("eclusg37",&m_nt_eclusg37,"eclusg37/D");
    m_ntuple->Branch("etaclusg371",&m_nt_etaclusg371,"etaclusg371/D");
    m_ntuple->Branch("etaclusg372",&m_nt_etaclusg372,"etaclusg372/D");
    m_ntuple->Branch("eActEM",&m_nt_eActEM,"eActEM/D");
    m_ntuple->Branch("eActNonEM",&m_nt_eActNonEM,"eActNonEM/D");
    m_ntuple->Branch("eActInvisible",&m_nt_eActInvisible,"eActInvisible/D");
    m_ntuple->Branch("eActEscaped",&m_nt_eActEscaped,"eActEscaped/D");
    m_ntuple->Branch("eInactEM",&m_nt_eInactEM,"eInactEM/D");
    m_ntuple->Branch("eInactNonEM",&m_nt_eInactNonEM,"eInactNonEM/D");
    m_ntuple->Branch("eInactInvisible",&m_nt_eInactInvisible,"eInactInvisible/D");
    m_ntuple->Branch("eInactEscaped",&m_nt_eInactEscaped,"eInactEscaped/D");
    m_ntuple->Branch("eDeadEM",&m_nt_eDeadEM,"eDeadEM/D");
    m_ntuple->Branch("eDeadNonEM",&m_nt_eDeadNonEM,"eDeadNonEM/D");
    m_ntuple->Branch("eDeadInvisible",&m_nt_eDeadInvisible,"eDeadInvisible/D");
    m_ntuple->Branch("eDeadEscaped",&m_nt_eDeadEscaped,"eDeadEscaped/D");
    m_ntuple->Branch("eTileCal",&m_nt_eTile,"eTileCal/D");
    m_ntuple->Branch("edead_1_0",&m_nt_edead_1_0,"edead_1_0/D");
    m_ntuple->Branch("edead_1_1_0",&m_nt_edead_1_1_0,"edead_1_1_0/D");
    m_ntuple->Branch("edead_1_1_1",&m_nt_edead_1_1_1,"edead_1_1_1/D");
    m_ntuple->Branch("edead_1_1_2",&m_nt_edead_1_1_2,"edead_1_1_2/D");
    m_ntuple->Branch("edead_1_1_3",&m_nt_edead_1_1_3,"edead_1_1_3/D");
    m_ntuple->Branch("edead_1_1_4",&m_nt_edead_1_1_4,"edead_1_1_4/D");
    m_ntuple->Branch("edead_1_1_5",&m_nt_edead_1_1_5,"edead_1_1_5/D");
    m_ntuple->Branch("edead_1_1_6",&m_nt_edead_1_1_6,"edead_1_1_6/D");
    m_ntuple->Branch("edead_1_1_7",&m_nt_edead_1_1_7,"edead_1_1_7/D");
    m_ntuple->Branch("edead_1_2_0",&m_nt_edead_1_2_0,"edead_1_2_0/D");
    m_ntuple->Branch("edead_1_2_1",&m_nt_edead_1_2_1,"edead_1_2_1/D");
    m_ntuple->Branch("edead_1_2_2",&m_nt_edead_1_2_2,"edead_1_2_2/D");
    m_ntuple->Branch("edead_1_2_3",&m_nt_edead_1_2_3,"edead_1_2_3/D");
    m_ntuple->Branch("edead_1_2_4",&m_nt_edead_1_2_4,"edead_1_2_4/D");
    m_ntuple->Branch("edead_1_2_5",&m_nt_edead_1_2_5,"edead_1_2_5/D");
    m_ntuple->Branch("edead_1_3",&m_nt_edead_1_3,"edead_1_3/D");
    m_ntuple->Branch("eleak",&m_nt_eleak,"eleak/D");
    m_ntuple->Branch("xconv",&m_xconv,"xconv/D");
    m_ntuple->Branch("yconv",&m_yconv,"yconv/D");
    m_ntuple->Branch("zconv",&m_zconv,"zconv/D");
    m_ntuple->Branch("xvert",&m_xvert,"xvert/D");
    m_ntuple->Branch("yvert",&m_yvert,"yvert/D");
    m_ntuple->Branch("zvert",&m_zvert,"zvert/D");
   
    sc = m_thistSvc->regTree("/file1/analysis",m_ntuple);
   
    ATH_MSG_INFO( " histos booked and registred "  );
    ATH_MSG_INFO("size of LArHit " << sizeof(LArHit)  );


  if (m_useTriggerTime) {

     IToolSvc* p_toolSvc = 0;
     sc = service("ToolSvc", p_toolSvc);
     IAlgTool* algtool;
 
     ListItem theTool(m_triggerTimeToolName.value());
     sc = p_toolSvc->retrieveTool(theTool.type(), theTool.name(),algtool);
     if (sc.isFailure()) {
       ATH_MSG_ERROR( "Unable to find tool for " << m_triggerTimeToolName.value()  );
        m_triggerTimeTool = 0;
     }
     else {
        m_triggerTimeTool=dynamic_cast<ITriggerTime*>(algtool);
        ATH_MSG_INFO( "retrieved TriggerTime tool: "
                      << m_triggerTimeToolName.value()  );
     }
  }


    m_nevt=0;

    ATH_CHECK( m_mcCollName.initialize() );
    ATH_CHECK( m_hitContainerNames.initialize (m_hit) );
    ATH_CHECK( m_caloCellName.initialize (m_cell) );
    ATH_CHECK( m_rawChannelName.initialize (m_raw) );
    ATH_CHECK( m_calibHitContainerNames.initialize (m_calhit) );
    ATH_CHECK( m_cluster55Name.initialize (m_cluster) );
    ATH_CHECK( m_cluster55gamName.initialize (m_cluster) );
    ATH_CHECK( m_cluster35Name.initialize (m_cluster) );
    ATH_CHECK( m_cluster35gamName.initialize (m_cluster) );
    ATH_CHECK( m_cluster37Name.initialize (m_cluster) );
    ATH_CHECK( m_cluster37gamName.initialize (m_cluster) );

    return StatusCode::SUCCESS; 
  }
  //__________________________________________________________________________
  StatusCode Analysis::finalize()
  {
    ATH_MSG_DEBUG("Analysis finalize()"  );
    return StatusCode::SUCCESS; 
  }
  
  //__________________________________________________________________________
  StatusCode Analysis::execute()
  {
    const EventContext& ctx = getContext();

    //.............................................
    
    ATH_MSG_DEBUG( "Analysis execute()"  );

//    m_lar_dd_man = LArDetDescrManager::instance();
//    const AtlasDetectorID* atlasID = m_lar_dd_man->get_em_id();
    m_id = m_larem_id->lar_em();


   m_nevt++;

   if (m_nevt==-1) {
    std::cout << "retrieving fsampl from detStore " << std::endl;
    const ILArfSampl*    dd_fSampl = nullptr;
    ATH_CHECK( detStore()->retrieve(dd_fSampl) );

    int ncell=m_CellListEM.size();
    std::cout << " start loop over cells " << std::endl;
    for (int i=0;i<ncell;i++) {
       Identifier id= m_CellListEM[i].identifier;
       int isamp=m_larem_id->sampling(id);
       int ieta=m_larem_id->eta(id);
       int region=m_larem_id->region(id);
       int iphi=m_larem_id->phi(id);       
       int BvsEC=m_larem_id->barrel_ec(id);
       if (BvsEC > 0 && iphi==0) {
           float SF= dd_fSampl->FSAMPL(id);
           std::cout << "** sampling fraction " << BvsEC << " " <<
             isamp <<  " " << region <<
             " " << ieta << " " << SF << std::endl;
       }
    }
   }


   if ((m_nevt%100)==1)
     ATH_MSG_INFO( " ***** Event " << m_nevt  );

// reset E list to 0
   if (m_check) {
    int ncell=m_CellListEM.size();
    for(int i=0;i<ncell;i++) {
       m_CellListEM[i].Ehit=0.;
       m_CellListEM[i].Thit=0.;
       m_CellListEM[i].Eraw=-9999.;
       m_CellListEM[i].Ecell=-9999.;
    }
   }

// get trigger time
   double trigtime=0;
   if (m_useTriggerTime && m_triggerTimeTool) {
     trigtime = m_triggerTimeTool->time();
     std::cout  << "Trigger time used : " << trigtime << std::endl;
   }
  

// Loop over LAr hits

  double etot_hit_b[4] = {0.,0.,0.,0.};
  double etot_hit_e[4] = {0.,0.,0.,0.};
  double hit_max_e[4] = {0.,0.,0.,0.};
  double time_max_cell[4]  = {0.,0.,0.,0.};
  int nhit_tot=0;
  int nhit_ot=0;
  if (m_hit) {
    for (const SG::ReadHandleKey<LArHitContainer>& k : m_hitContainerNames) {
      SG::ReadHandle<LArHitContainer> hit_container (k, ctx);
      int ihit = 0;
      for (const LArHit* hit : *hit_container) {
        ihit++;
        nhit_tot++;
        m_hist_hittime->Fill(hit->time());
        m_hist_hitener->Fill(hit->energy());
        if (hit->time() > 2.5) {
//          if (hit->time() > 25.) {
          m_hist_ot->Fill(hit->energy());
          nhit_ot++;
          if (m_larem_id->is_lar_em(hit->cellID()))
            m_hist_ot_em->Fill(hit->energy());
          if (m_larem_id->is_lar_hec(hit->cellID()))
            m_hist_ot_hec->Fill(hit->energy());
          if (m_larem_id->is_lar_fcal(hit->cellID()))
            m_hist_ot_fcal->Fill(hit->energy());
        } else {
          m_hist_it->Fill(hit->energy());
          if (m_larem_id->is_lar_em(hit->cellID()))
            m_hist_it_em->Fill(hit->energy());
          if (m_larem_id->is_lar_hec(hit->cellID()))
            m_hist_it_hec->Fill(hit->energy());
          if (m_larem_id->is_lar_fcal(hit->cellID()))
            m_hist_it_fcal->Fill(hit->energy());
        }
        Identifier cellID=hit->cellID();
        if (m_larem_id->is_lar_em(cellID)) {
// fill list
          if (m_check) {
            IdentifierHash idHash=0;
            idHash = m_larem_id->channel_hash(cellID);
            double hit_time = hit->time() - trigtime;
            if (std::fabs(hit_time)<25.) {
              m_CellListEM[idHash].Ehit += hit->energy();
              m_CellListEM[idHash].Thit += hit->energy()*hit_time;
            }
          }
          int sampl = m_larem_id->sampling(cellID);
// barrel vs EndCap
          if (abs(m_larem_id->barrel_ec(cellID))==1) {
            etot_hit_b[sampl]+=hit->energy();
            if (hit->energy() > hit_max_e[sampl]) {
              hit_max_e[sampl]=hit->energy();
              time_max_cell[sampl]=hit->time();
            }
          }
          else
            etot_hit_e[sampl]+=hit->energy();

          if (std::fabs(hit->energy())>1000.)  {
            ATH_MSG_INFO( " Hit  " << m_larem_id->show_to_string(cellID)
                          << " ieta/iphi/isampl/E/t " << m_larem_id->eta(cellID) 
                          << " " << m_larem_id->phi(cellID) << " " << 
                          m_larem_id->sampling(cellID) << " " << hit->energy() 
                          << " " << hit->time()  );
          }
        }     // lar_em
      }        // loop over hits
      ATH_MSG_DEBUG( " number of hits found " << ihit  );
    }             // loop over hit containers
//   log << MSG::INFO << " Total number of LAr hits " << nhit_tot << " "
//                  << nhit_ot << endmsg;
   double xhit = (double) nhit_tot;
   m_hist_nhitlar->Fill(xhit);
 }    //  m_hit true

//   std::cout << " EtotPS " << etot_hit_b[0] << std::endl;
 
// tmp
//   if (nhit_tot==0) return StatusCode::SUCCESS;


// loop over generated particles

      SG::ReadHandle<McEventCollection> mcCollptr (m_mcCollName);
      double e_true=0.;
      double eta_true=-999.;
      double phi_true=-999.;
      int nn=0;
      if (m_doTruth) {
         McEventCollection::const_iterator itr;
         for (itr = mcCollptr->begin(); itr!=mcCollptr->end(); ++itr) 
         {
          HepMC::GenEvent::particle_const_iterator itrPart;
          for (itrPart = (*itr)->particles_begin(); itrPart!=(*itr)->particles_end(); ++itrPart )
          {
          HepMC::GenParticle *part=*itrPart;
           if(  (abs(part->pdg_id())==11 || part->pdg_id()==22 || part->pdg_id()==13
                || (abs(part->pdg_id())==211) )
                && part->momentum().e()> 5000.)
           {
             nn++;
             if (nn==1) {
               e_true = part->momentum().e();
               eta_true = part->momentum().pseudoRapidity();
               phi_true = part->momentum().phi();
               ATH_MSG_INFO( " true particle found " << part->pdg_id() << " " <<
                             e_true << " " << eta_true << " " << phi_true  );
               HepMC::GenVertex* vertex = part->end_vertex();
               if (vertex) {
                  m_xconv = vertex->point3d().x();
                  m_yconv = vertex->point3d().y();
                  m_zconv = vertex->point3d().z();
               }
               else {
                  m_xconv=9999.;
                  m_yconv=9999.;
                  m_zconv=9999.;
               }
               vertex = part->production_vertex();
               if (vertex) { 
                  m_xvert = vertex->point3d().x();
                  m_yvert = vertex->point3d().y();
                  m_zvert = vertex->point3d().z();
               }
               else {
                  m_xvert=9999.;
                  m_yvert=9999.;
                  m_zvert=9999.;
               }
             }

  
           } // e or gamma found
          }  // loop over particle
         }   // loop over mcCollptr
      }    // doTruth


// Loop over CaloCells
  double etot_cell[4]={0.,0.,0.,0.};
  if (m_cell) {
    SG::ReadHandle<CaloCellContainer> cell_container (m_caloCellName, ctx);
    for (const CaloCell* cell : *cell_container) {
      Identifier cellID = cell->ID();
      double energy=  cell->energy();
      // fill list
      if (m_larem_id->is_lar_em(cellID)) {
        if (m_check) {
          IdentifierHash idHash=0;
          idHash = m_larem_id->channel_hash(cellID);
          m_CellListEM[idHash].Ecell = energy;
        }
        int sampl=m_larem_id->sampling(cellID);
        etot_cell[sampl]+=energy;
      }
      if (fabs(energy)>1000.) {
        double eta = cell->eta();
        double phi = cell->phi();
        double eta0 = cell->caloDDE()->eta_raw();
        double phi0 = cell->caloDDE()->phi_raw();
        ATH_MSG_INFO( "Cell " << m_larem_id->show_to_string(cellID)
                      << " eta/phi/energy " << eta << " " << eta0 << " " << phi << " "
                      << phi0 << " " << energy  );
      }
      int sampling = cell->caloDDE()->getSampling();
      if (sampling==CaloCell_ID::TileGap3) std::cout << "   *** found tile gap cells in cell list" << std::endl;

    }
  }     // m_cell
//
  
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};

  double etot_raw[4]={0.,0.,0.,0.};
  if (m_raw && cabling) {
// Loop over LArRawChannel
    int nraw=0;
    SG::ReadHandle<LArRawChannelContainer> rawchannel_container(m_rawChannelName, ctx);
    ATH_MSG_INFO( "*** Stat loop over LArRawChannel in Myanalysis"  );
    LArRawDetSelector  selObj;
    selObj.setDet(m_id);
    for (const LArRawChannel& hit : *rawchannel_container) {
       nraw++;
       if (!selObj.select(&hit)) continue; 
       double energy=(double)hit.energy();  //  energy in MeV from RawChannel

       const HWIdentifier ch_id = hit.channelID();
       Identifier  cellID =
        cabling->cnvToIdentifier(ch_id);
// fill list
       if (m_larem_id->is_lar_em(cellID)) {
        if (m_check) {
         IdentifierHash idHash=0;
         idHash = m_larem_id->channel_hash(cellID);
         m_CellListEM[idHash].Eraw = energy;
        }
        int sampl=m_larem_id->sampling(cellID);
//GU only barrel
//        if (abs(m_larem_id->barrel_ec(cellID))==1) {
          etot_raw[sampl]+=energy;
//        }
       }

       if (std::fabs(energy)>10000. && m_larem_id->is_lar_em(cellID) ) {
        double time=(double)hit.time();      // time in ps from RawChannel
        double chi2=(double)hit.quality();   // chi2 from RawChannel
        ATH_MSG_INFO( " RawChannel ieta/iphi/isampl/E/t" 
                      << " " << m_larem_id->eta(cellID)
                      << " " << m_larem_id->phi(cellID) 
                      << " " << m_larem_id->sampling(cellID) 
                      << " " << energy << " " << time << " " << chi2  );
       }


    }
    ATH_MSG_INFO( "number of EM raw Channels " << nraw  );

  }  
// loop over Calibration Hits

  double etot_cal=0;
  double eTile=0;
  double ecal[4]={0.,0.,0.,0.};
  double eAct[4]={0.,0.,0.,0.};
  double eInact[4]={0.,0.,0.,0.};
  double eDead[4]={0.,0.,0.,0.};
  double edead_1_0 =0;
  double edead_1_1_0=0;
  double edead_1_1_1=0;
  double edead_1_1_2=0;
  double edead_1_1_3=0;
  double edead_1_1_4=0;
  double edead_1_1_5=0;
  double edead_1_1_6=0;
  double edead_1_1_7=0;
  double edead_1_2_0=0;
  double edead_1_2_1=0;
  double edead_1_2_2=0;
  double edead_1_2_3=0;
  double edead_1_2_4=0;
  double edead_1_2_5=0;
  double edead_1_3 = 0;
  double eleak = 0.;


  if (m_calhit)  {
    ATH_MSG_INFO( "CalibrationHitContainer.size " 
                  << m_calibHitContainerNames.size()  );
    for (const SG::ReadHandleKey<CaloCalibrationHitContainer>& k : m_calibHitContainerNames)
    {
      SG::ReadHandle<CaloCalibrationHitContainer> calocalibrationhit_container(k, ctx);
      int ihit = 0;
      for (const CaloCalibrationHit* calibhit : *calocalibrationhit_container)
      {
          ihit++;
          nhit_tot++;
          etot_cal = etot_cal + calibhit->energyTotal();
          ecal[0]=ecal[0] + calibhit->energyEM();
          ecal[1]=ecal[1] + calibhit->energyNonEM();
          ecal[2]=ecal[2] + calibhit->energyInvisible();
          ecal[3]=ecal[3] + calibhit->energyEscaped();
          if (k.key() == "LArCalibrationHitActive") {
            eAct[0]  += calibhit->energyEM();
            eAct[1]  += calibhit->energyNonEM();
            eAct[2]  += calibhit->energyInvisible();
            eAct[3]  += calibhit->energyEscaped();
          }
          if (k.key() == "LArCalibrationHitInactive") {
            eInact[0]  += calibhit->energyEM();
            eInact[1]  += calibhit->energyNonEM();
            eInact[2]  += calibhit->energyInvisible();
            eInact[3]  += calibhit->energyEscaped();
          }
          if (k.key() == "LArCalibrationHitDeadMaterial") {
            eDead[0]  += calibhit->energyEM();
            eDead[1]  += calibhit->energyNonEM();
            eDead[2]  += calibhit->energyInvisible();
            eDead[3]  += calibhit->energyEscaped();
          }
          if (k.key() == "TileCalibHitActiveCell" ||
              k.key() == "TileCalibHitInactiveCell")
          {
            eTile+= calibhit->energyTotal();
          }
          Identifier id = calibhit->cellID();
          if (m_larem_id->is_lar_em(id) && abs(m_larem_id->barrel_ec(id))==1) {
             int is = m_larem_id->sampling(id);
             int ir = m_larem_id->region(id);
             int ie = m_larem_id->eta(id);
             int ip = m_larem_id->phi(id);
             if (!CheckLArIdentifier(is,ir,ie,ip)) {
               std::cout << "Bad LAr identifier " << is << " " << ir << " " <<ie << " " << ip << std::endl;
             }
          }

// escaped energy => leakage
          eleak += calibhit->energyEscaped();

          if (calibhit->energyEscaped() > 0.) {
            std::cout << " Escaped energy found Identifier= " << m_larem_id->show_to_string(id) << "  Energy = " << calibhit->energyEscaped() << std::endl;
          }

//          if (iHitContainer==1) {
//             std::cout << " DM identifier lar? " << m_calodm_id->is_lar_dm(id)  << " " << m_calodm_id->is_lar(id)
//             << " Tile? " << m_calodm_id->is_tile_dm(id)  << " " << m_calodm_id->is_tile(id)
//             << " pos_neg " << m_calodm_id->pos_neg_z(id) 
//             << " type    " << m_calodm_id->dmat(id)
//             << " sampling " << m_calodm_id->sampling(id)
//             << " region   " << m_calodm_id->region(id) 
//             << " eta      " << m_calodm_id->eta(id)
//             << " phi      " << m_calodm_id->phi(id)  << std::endl;
//          }
        

// tile DM => leakage
          if (m_calodm_id->is_tile_dm(id) ) 
            eleak += (calibhit->energyEM()+calibhit->energyNonEM()+calibhit->energyInvisible());

          if (m_calodm_id->is_lar_dm(id) ) {
             int it = m_calodm_id->dmat(id);
             int is = m_calodm_id->sampling(id);
             int ir = m_calodm_id->region(id);
             int ie = m_calodm_id->eta(id);
             int ip = m_calodm_id->phi(id);
             if (!CheckDMIdentifier(it,is,ir,ie,ip)) {
               std::cout << "Bad DM identifier " << it << " " << is << " " << ir << " " <<ie << " " << ip << std::endl;
             }
             if (it==1 && is==0) edead_1_0 += (calibhit->energyEM()+calibhit->energyNonEM());
             if (it==1 && is==1 && ir==0) edead_1_1_0 += (calibhit->energyEM()+calibhit->energyNonEM());
             if (it==1 && is==1 && ir==1) edead_1_1_1 += (calibhit->energyEM()+calibhit->energyNonEM());
             if (it==1 && is==1 && ir==2) edead_1_1_2 += (calibhit->energyEM()+calibhit->energyNonEM());
             if (it==1 && is==1 && ir==3) edead_1_1_3 += (calibhit->energyEM()+calibhit->energyNonEM());
             if (it==1 && is==1 && ir==4) edead_1_1_4 += (calibhit->energyEM()+calibhit->energyNonEM());
             if (it==1 && is==1 && ir==5) edead_1_1_5 += (calibhit->energyEM()+calibhit->energyNonEM());
             if (it==1 && is==1 && ir==6) edead_1_1_6 += (calibhit->energyEM()+calibhit->energyNonEM());
             if (it==1 && is==1 && ir==7) edead_1_1_7 += (calibhit->energyEM()+calibhit->energyNonEM());
             if (it==1 && is==2 && ir==0) edead_1_2_0 += (calibhit->energyEM()+calibhit->energyNonEM());
             if (it==1 && is==2 && ir==1) edead_1_2_1 += (calibhit->energyEM()+calibhit->energyNonEM());
             if (it==1 && is==2 && ir==2) edead_1_2_2 += (calibhit->energyEM()+calibhit->energyNonEM());
             if (it==1 && is==2 && ir==3) edead_1_2_3 += (calibhit->energyEM()+calibhit->energyNonEM());
             if (it==1 && is==2 && ir==4) edead_1_2_4 += (calibhit->energyEM()+calibhit->energyNonEM());
             if (it==1 && is==2 && ir==5) edead_1_2_5 += (calibhit->energyEM()+calibhit->energyNonEM());
             if (it==1 && is==3) edead_1_3 += (calibhit->energyEM()+calibhit->energyNonEM());

//LAr DM sampling0/region0 && sampling2 => leakage
             if (it==1 && is==0 && ir==0) 
               eleak += (calibhit->energyEM()+calibhit->energyNonEM()+calibhit->energyInvisible());
             if (it==1 && is==2)
                eleak += (calibhit->energyEM()+calibhit->energyNonEM()+calibhit->energyInvisible());
             
          }
        }     // loop over hits
    }         // loop over calo calibration hit containers
    ATH_MSG_INFO( "Etot calibration hits " << etot_cal    
                  << " " << ecal[0] << " " << ecal[1] << " " << ecal[2] << " " << ecal[3]
                  );
    m_hist_cal0->Fill(ecal[0]);
    m_hist_cal1->Fill(ecal[1]);
    m_hist_cal2->Fill(ecal[2]);
    m_hist_cal3->Fill(ecal[3]);
  }   // m_calhit true


// loop over cells and compare energies

  if (m_check) {
   int ncell=m_CellListEM.size();
   for (int i=0;i<ncell;i++) {
    double ehit=m_CellListEM[i].Ehit; 
    double thit=m_CellListEM[i].Thit;
    double eraw=m_CellListEM[i].Eraw; 
    double ecell=m_CellListEM[i].Ecell; 
    int sampling=m_CellListEM[i].sampling;
    int calotype=m_CellListEM[i].bec;
    double eta=fabs(m_CellListEM[i].eta);
    if ((ehit > 10000. || eraw > 60000.) && calotype==0)  {    // 20 MeV cut
      ATH_MSG_INFO( "calo/samp/eta " << calotype << " " << sampling << " " << eta
                    << " energies Hit/Raw/Cell " << ehit << " " << eraw << " " << ecell << " " << thit  );
    }   // cut on Ehit
//    double et = eraw/cosh(eta);
    if (ehit < 0.0001) {  // no true energy in this cell
     if (sampling==0) {
      if (calotype==0) m_hist_etraw_emb_s0->Fill(eraw);
      else             m_hist_etraw_emec_s0->Fill(eraw);
     }
     if (sampling==1) {
      if (calotype==0) m_hist_etraw_emb_s1->Fill(eraw);
      else             m_hist_etraw_emec_s1->Fill(eraw);
     }
     if (sampling==2) {
      if (calotype==0) m_hist_etraw_emb_s2->Fill(eraw);
      else             m_hist_etraw_emec_s2->Fill(eraw);
     }
     if (sampling==3) {
      if (calotype==0) m_hist_etraw_emb_s3->Fill(eraw);
      else             m_hist_etraw_emec_s3->Fill(eraw);
     }
    }

   }      // loop over cells
  }

// build pseudo cluster 3*5 in middle, 1*1 in strip and presampler

  if (m_clusternoise) {
  std::cout << "start loop over pseudo clusters " << std::endl;
  for (int iside=-1;iside<=1;iside+=2) {
   for (int ieta=1;ieta<56;ieta++) {
    for (int iphi=0;iphi<255;iphi++) {
     double eta=0.025*((double)ieta);
     double sume=0;
     int ireg=0;
     int sampling;

// sampling 2
     sampling=2;
     for (int jeta=ieta-1;jeta<=ieta+1;jeta++) {
      for (int jphi=iphi-2;jphi<=iphi+2;jphi++) {
       int kphi=jphi;
       if (jphi>255) kphi=jphi-255;
       if (jphi<0) kphi=jphi+255;
// build identifier and find hash id
       Identifier myid = m_larem_id->channel_id(iside,sampling,ireg,jeta,kphi);
       IdentifierHash hid = m_larem_id->channel_hash(myid);
       if (/*hid>=0 &&*/ hid < m_CellListEM.size())
         sume+=m_CellListEM[hid].Ecell;
      }
     }
// sampling 1
     sampling=1;
     int ietas1=ieta*8;
     int ietas2=ietas1+8;
     int jphi=iphi/4; 
     for (int jeta=ietas1;jeta<ietas2;jeta++) {
      Identifier myid = m_larem_id->channel_id(iside,sampling,ireg,jeta,jphi);
      IdentifierHash hid = m_larem_id->channel_hash(myid);
      if (/*hid>=0 &&*/ hid < m_CellListEM.size())
         sume+=m_CellListEM[hid].Ecell;
     }
// sampling 0
     sampling=0;
     int jeta=ieta;
     jphi=iphi/4;
     Identifier myid = m_larem_id->channel_id(iside,sampling,ireg,jeta,jphi);
     IdentifierHash hid = m_larem_id->channel_hash(myid);
     if (/*hid>=0 &&*/ hid < m_CellListEM.size())
       sume+=m_CellListEM[hid].Ecell;

// goes to Et
     sume = sume/cosh(eta);

     m_hist_clusnoise->Fill(sume);
     if (eta<0.5) m_hist_clusnoise1->Fill(sume);
     else if(eta<1.0) m_hist_clusnoise2->Fill(sume);
     else m_hist_clusnoise3->Fill(sume);
 
    }
    std::cout << "end loop over phi" << std::endl;
   }
  }
  }  // m_clusternoise
    

// check cluster

  m_nt_eclus=0.;
  m_nt_eclus0 = 0;                                        
  m_nt_eclus1 = 0.;
  m_nt_eclus2 = 0.;
  m_nt_eclus3 = 0.;
  m_nt_etaclus = 0.;
  m_nt_phiclus = 0.;
  m_nt_eclus35 = 0.,
  m_nt_eclus37 = 0.,
  m_nt_etaclus35 = 0.;
  m_nt_etaclus351 = 0.;
  m_nt_eclusg=0;
  m_nt_eclusg35=0;
  m_nt_eclusg37=0;
  if (m_cluster) {
// 5x5
    {
      SG::ReadHandle<CaloClusterContainer> cluster_container (m_cluster55Name, ctx);
      ATH_MSG_INFO( " start loop over clusters "  );
      for (const CaloCluster* clus : *cluster_container) {
//       clus->print();
         ATH_MSG_INFO( " **** Cluster E   " << clus->energy() << "  Et=" 
                       << clus->et() << " " 
                       << " eta/phi " << clus->eta() << " " << clus->phi() 
                       << " phi2nd " << clus->phiSample(CaloSampling::EMB2)  );
         m_nt_eclus=clus->energy();
         m_nt_eclus0 = clus->eSample(CaloSampling::PreSamplerB);
         m_nt_eclus1 = clus->eSample(CaloSampling::EMB1);
         m_nt_eclus2 = clus->eSample(CaloSampling::EMB2);
         m_nt_eclus3 = clus->eSample(CaloSampling::EMB3);
         m_nt_etaclus = clus->eta();
         m_nt_phiclus = clus->phi();
         if (std::fabs(m_nt_etaclus)<1.475) {
           m_nt_etaclus551 = clus->etaSample(CaloSampling::EMB1);
           m_nt_etaclus552 = clus->etaSample(CaloSampling::EMB2);
         } else {
           m_nt_etaclus551 = clus->etaSample(CaloSampling::EME1);
           m_nt_etaclus552 = clus->etaSample(CaloSampling::EME2);
         }

// for clusters in the crack, loop over the cells
        std::cout << " * start loop over cells " << std::endl;
        if (std::fabs(m_nt_etaclus)>1.45 && std::fabs(m_nt_etaclus)<1.55) {
           CaloCluster::cell_iterator cellIter    = clus->cell_begin();
           CaloCluster::cell_iterator cellIterEnd = clus->cell_end();
           for( ;cellIter!=cellIterEnd;cellIter++) {
             const CaloCell* cell = (*cellIter);
             int sampling = cell->caloDDE()->getSampling();
             std::cout << " sampling " << sampling << std::endl;
             if (sampling==CaloCell_ID::TileGap3) std::cout << "   *** found tile gap " << std::endl;
           }
        }

      }
    }
// 5x5 photons
    {   
      SG::ReadHandle<CaloClusterContainer> cluster_container (m_cluster55gamName, ctx);
      ATH_MSG_INFO( " start loop over gamma 5x5 clusters "  );
      for (const CaloCluster* clus : *cluster_container) {
//       clus->print();
         m_nt_eclusg = clus->energy();
         ATH_MSG_INFO( " **** Cluster E   " << clus->energy() << "  Et="
                       << clus->et() << " " 
                       << " eta/phi " << clus->eta() << " " << clus->phi()  
                       << " phi2nd " << clus->phiSample(CaloSampling::EMB2)  );
          if (std::fabs(clus->eta())<1.475) {
           m_nt_etaclusg551 =  clus->etaSample(CaloSampling::EMB1);
           m_nt_etaclusg552 =  clus->etaSample(CaloSampling::EMB2);
          } else {
           m_nt_etaclusg551 =  clus->etaSample(CaloSampling::EME1);
           m_nt_etaclusg552 =  clus->etaSample(CaloSampling::EME2);
          } 
      }
    }

// 3x5
    {   
      SG::ReadHandle<CaloClusterContainer> cluster_container (m_cluster35Name, ctx);
      ATH_MSG_INFO( " start loop over clusters "  );
      for (const CaloCluster* clus : *cluster_container) {
       ATH_MSG_INFO( " **** Cluster E35 " << clus->energy() << "  Et=" 
                     << clus->et() << " " 
                     << " eta/phi " << clus->eta() << " " << clus->phi()  );
       m_nt_eclus35 = clus->energy();
       m_nt_eclus350 = clus->eSample(CaloSampling::PreSamplerB);
       m_nt_eclus351 = clus->eSample(CaloSampling::EMB1);
       m_nt_eclus352 = clus->eSample(CaloSampling::EMB2);
       m_nt_eclus353 = clus->eSample(CaloSampling::EMB3);
       m_nt_etaclus35 = clus->eta();
       if (std::fabs(m_nt_etaclus35)<1.475) {
        m_nt_etaclus351 = clus->etaSample(CaloSampling::EMB1);
        m_nt_etaclus352 = clus->etaSample(CaloSampling::EMB2);
       } else {
        m_nt_etaclus351 = clus->etaSample(CaloSampling::EME1);
        m_nt_etaclus352 = clus->etaSample(CaloSampling::EME2);
       }
      }
    }

// 3x5 photons
    {  
      SG::ReadHandle<CaloClusterContainer> cluster_container (m_cluster35gamName, ctx);
      ATH_MSG_INFO( " start loop over clusters "  );
      for (const CaloCluster* clus : *cluster_container) {
       ATH_MSG_INFO( " **** Cluster E35 " << clus->energy() << "  Et="
                     << clus->et() << " "
                     << " eta/phi " << clus->eta() << " " << clus->phi()  );
       m_nt_eclusg35 = clus->energy();
       if (std::fabs(clus->eta())<1.475) {
        m_nt_etaclusg351 = clus->etaSample(CaloSampling::EMB1);
        m_nt_etaclusg352 = clus->etaSample(CaloSampling::EMB2);
        std::cout << " etaclusg351 " << m_nt_etaclusg351 << std::endl;
       } else {
        m_nt_etaclusg351 = clus->etaSample(CaloSampling::EME1);
        m_nt_etaclusg352 = clus->etaSample(CaloSampling::EME2);
       }
      }
    }

// 3x7
    {   
      SG::ReadHandle<CaloClusterContainer> cluster_container (m_cluster37Name, ctx);
      ATH_MSG_INFO( " start loop over clusters "  );
      for (const CaloCluster* clus : *cluster_container) {
       ATH_MSG_INFO( " **** Cluster E37 " << clus->energy() << "  Et=" 
                     << clus->et() << " " 
                     << " eta/phi " << clus->eta() << " " << clus->phi()  );
        m_nt_eclus37 = clus->energy();
        if (std::fabs(clus->eta())<1.475) {
          m_nt_etaclus371 = clus->etaSample(CaloSampling::EMB1);
          m_nt_etaclus372 = clus->etaSample(CaloSampling::EMB2);
        } else {
          m_nt_etaclus371 = clus->etaSample(CaloSampling::EME1);
          m_nt_etaclus372 = clus->etaSample(CaloSampling::EME2);
        }

      }
    }

// 3x7 gam
    {
      SG::ReadHandle<CaloClusterContainer> cluster_container (m_cluster37gamName, ctx);
      ATH_MSG_INFO( " start loop over clusters "  );
      for (const CaloCluster* clus : *cluster_container) {
       ATH_MSG_INFO( " **** Cluster E37 " << clus->energy() << "  Et="
                     << clus->et() << " "
                     << " eta/phi " << clus->eta() << " " << clus->phi()  );
        m_nt_eclusg37 = clus->energy();
        if (std::fabs(clus->eta())<1.475) {
          m_nt_etaclusg371 = clus->etaSample(CaloSampling::EMB1);
          m_nt_etaclusg372 = clus->etaSample(CaloSampling::EMB2);
        } else {
          m_nt_etaclusg371 = clus->etaSample(CaloSampling::EME1);
          m_nt_etaclusg372 = clus->etaSample(CaloSampling::EME2);
        }

      }
    }

  }     // m_cluster


// fill ntuple

  m_nt_e=e_true;
  m_nt_eta=eta_true;
  m_nt_phi=phi_true;
  m_nt_e0=etot_raw[0];
  m_nt_e1=etot_raw[1];
  m_nt_e2=etot_raw[2];
  m_nt_e3=etot_raw[3];
  m_nt_c0=etot_cell[0];
  m_nt_c1=etot_cell[1];
  m_nt_c2=etot_cell[2];
  m_nt_c3=etot_cell[3];
  m_nt_hb0=etot_hit_b[0];
  m_nt_hb1=etot_hit_b[1];
  m_nt_hb2=etot_hit_b[2];
  m_nt_hb3=etot_hit_b[3];
  m_nt_he0=etot_hit_e[0];
  m_nt_he1=etot_hit_e[1];
  m_nt_he2=etot_hit_e[2];
  m_nt_he3=etot_hit_e[3];
  m_nt_tb0=time_max_cell[0];
  m_nt_tb1=time_max_cell[1];
  m_nt_tb2=time_max_cell[2];
  m_nt_tb3=time_max_cell[3];

  m_nt_eActEM = eAct[0];
  m_nt_eActNonEM = eAct[1];
  m_nt_eActInvisible = eAct[2];
  m_nt_eActEscaped = eAct[3];
  m_nt_eInactEM = eInact[0];
  m_nt_eInactNonEM = eInact[1];
  m_nt_eInactInvisible = eInact[2];
  m_nt_eInactEscaped = eInact[3];
  m_nt_eDeadEM = eDead[0];
  m_nt_eDeadNonEM = eDead[1];
  m_nt_eDeadInvisible = eDead[2];
  m_nt_eDeadEscaped = eDead[3];
  m_nt_eTile = eTile;

  m_nt_edead_1_0 = edead_1_0;
  m_nt_edead_1_1_0 = edead_1_1_0;
  m_nt_edead_1_1_1 = edead_1_1_1;
  m_nt_edead_1_1_2 = edead_1_1_2;
  m_nt_edead_1_1_3 = edead_1_1_3;
  m_nt_edead_1_1_4 = edead_1_1_4;
  m_nt_edead_1_1_5 = edead_1_1_5;
  m_nt_edead_1_1_6 = edead_1_1_6;
  m_nt_edead_1_1_7 = edead_1_1_7;
  m_nt_edead_1_2_0 = edead_1_2_0;
  m_nt_edead_1_2_1 = edead_1_2_1;
  m_nt_edead_1_2_2 = edead_1_2_2;
  m_nt_edead_1_2_3 = edead_1_2_3;
  m_nt_edead_1_2_4 = edead_1_2_4;
  m_nt_edead_1_2_5 = edead_1_2_5;
  m_nt_edead_1_3 = edead_1_3;
  m_nt_eleak = eleak;


  m_ntuple->Fill();

  return StatusCode::SUCCESS;
 }

bool Analysis::CheckLArIdentifier(int sampling, int region, int eta, int phi)
{
  if (sampling<0 || sampling >3) return false;
  if (sampling==0) {
    if (region!=0) return false;
    if (eta<0 || eta>60) return false;
    if (phi<0 || phi>63) return false;
  }
  if (sampling==1) {
    if (region<0 || region >1) return false;
    if (region==0) {
       if (eta<1 || eta>447) return false;
       if (phi<0 || phi>63) return false;
    }
    if (region==1) {
       if (eta<0 || eta>2) return false;
       if (phi<0 || phi>255) return false;
    }
  }
  if (sampling==2) {
    if (region<0 || region >1) return false;
    if (region==0) {
        if (eta<0 || eta>55) return false;
        if (phi<0 || phi>255) return false;
     }
     if (region==1) {
        if (eta!=0) return false;
        if (phi<0 || phi>255) return false;
     }
  }
  if (sampling==3) {
     if (region !=0) return false;
     if (eta<0 || eta>26) return false;
     if (phi<0 || phi>255) return false;
  }
  return true;
}


bool Analysis::CheckDMIdentifier(int type, int sampling, int region, int eta, int phi)
{

 if (type <1 || type > 2) return false;
 if (type==1) { 
    if (sampling<0 || sampling>3) return false;
     if (sampling==0) {
        if (region <0 || region >5) return false;
        if (phi<0 || phi>63) return false;
        if (eta<0 || eta>49) return false;
     }
    if (sampling==1) {
       if (region<0 || region>7) return false;
       if (phi<0 || phi>63) return false;
       if (region==0) {
         if (eta<0 || eta>14) return false;
       }
       if (region==1) {
         if (eta<0 || eta>14) return false;
       }
       if (region==2) {
         if (eta<0 || eta>15) return false;
       }
       if (region==3) {
// valid range goes up to eta=1.6 in bin of 0.1 => etamax=15
          if (eta<0 || eta>15) return false;
       }
       if (region==4) {
          if (eta !=0) return false;
       }
       if (region==5) {
         if (eta<0 || eta>2) return false;
       }
       if (region==6) {
         if (eta<0 || eta>18) return false;
       }
       if (region==7) {
         if (eta<0 || eta>17) return false;
       }
    }
    if (sampling==2) {
       if (region <0 || region>5  ) return false;
       if (phi<0 || phi>63) return false;
       if (region==0){
          if (eta<0 || eta>9) return false;
       }
       if (region==2) {
          if (eta<0 || eta>4) return false;
       }
       if (region==3) {
         if (eta<0 || eta>17) return false;
       }
       if (region==4) {
         if (eta<0 || eta>17) return false;
       }
       if (region==5) {
         if (eta<0 || eta>6) return false;
       }
    }
    if (sampling==3) {
      if (region<0 || region>2) return false;
      if (region==0) {
        if (eta<0 || eta>32) return false;
        if (phi<0 || phi>63) return false;
      }
      if (region==1) {
        if (eta<0 || eta>14) return false;
        if (phi<0 || phi>63) return false;
      }
      if (region==2) {
        if (eta !=0) return false;
        if (phi !=0) return false;
      }
    }
 }  
 if (type==2) {
    if (sampling<0 || sampling >3) return false;
    if (sampling==0) {
      if (region<0 || region>5) return false;
      if (region==0) {
         if (eta !=0) return false;
         if (phi<0 || phi>63) return false;
      }
      if (region==1) {
         if (eta !=0) return false;
         if (phi<0 || phi>63) return false;
      }
      if (region==2) {
         if (eta<0 || eta>9) return false;
         if (phi<0 || phi>31) return false;
      }
      if (region==3) {
         if (eta<0 || eta>3) return false;
         if (phi<0 || phi>31) return false;
      }
      if (region==4) {
         if (eta!=0) return false;
         if (phi<0 || phi>63) return false;
      }
      if (region==5) {
         if (eta<0 || eta > 19) return false;
         if (phi<0 || phi>63) return false;
      }
   }
   if (sampling==1) {
     if (region<0 || region>5) return false;
      if (region==0) {
         if (eta !=0) return false;
         if (phi<0 || phi>63) return false;
      }
      if (region==1) {
         if (eta !=0) return false;
         if (phi<0 || phi>63) return false;
      }
      if (region==2) {
         if (eta<0 || eta>9) return false;
         if (phi<0 || phi>31) return false;
      }
      if (region==3) {
         if (eta<0 || eta>3) return false;
         if (phi<0 || phi>31) return false;
      }
      if (region==4) {
         if (eta!=0) return false;
         if (phi<0 || phi>63) return false;
      }
      if (region==5) {
         if (eta<0 || eta > 19) return false;
         if (phi<0 || phi>63) return false;
      }
   }
   if (sampling==2) {
     if (region<0 || region>5) return false;
      if (region==0) {
         if (eta !=0) return false;
         if (phi<0 || phi>63) return false;
      }
      if (region==1) {
         if (eta !=0) return false;
         if (phi<0 || phi>63) return false;
      }
      if (region==2) {
         if (eta<0 || eta>9) return false;
         if (phi<0 || phi>31) return false;
      }
      if (region==3) {
         if (eta<0 || eta>3) return false;
         if (phi<0 || phi>31) return false;
      }
      if (region==4) {
         if (eta!=0) return false;
         if (phi<0 || phi>63) return false;
      }
      if (region==5) {
         if (eta<0 || eta > 19) return false;
         if (phi<0 || phi>63) return false;
      }
   }
   if (sampling==3) {
    if (region<0 || region>5) return false;
      if (region==0) {
         if (eta !=0) return false;
         if (phi<0 || phi>63) return false;
      }
      if (region==1) {
         if (eta !=0) return false;
         if (phi<0 || phi>63) return false;
      }
      if (region==2) {
         if (eta<0 || eta>9) return false;
         if (phi<0 || phi>31) return false;
      }
      if (region==3) {
         if (eta<0 || eta>3) return false;
         if (phi<0 || phi>31) return false;
      }
      if (region==4) {
         if (eta!=0) return false;
         if (phi<0 || phi>63) return false;
      }
      if (region==5) {
         if (eta<0 || eta > 19) return false;
         if (phi<0 || phi>63) return false;
      }
   }

 } 
 return true;
} 

} // end of namespace bracket
