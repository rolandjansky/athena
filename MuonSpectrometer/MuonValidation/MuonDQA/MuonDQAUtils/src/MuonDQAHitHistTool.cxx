/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "MuonDQAUtils/MuonDQAHitHistTool.h"
#include "MuonDQAUtils/HistDQADataHelper.h"

//#include "TROOT.h"
#include "TH1.h"


namespace Muon {

  MuonDQAHitHistTool::MuonDQAHitHistTool(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa) 
  {
    declareInterface<MuonDQAHitHistTool>(this);
  }

  MuonDQAHitHistTool::~MuonDQAHitHistTool(){}

  StatusCode MuonDQAHitHistTool::initialize()
  {
    m_log = new MsgStream(msgSvc(),name());      
    StatusCode sc = AlgTool::initialize();
    if ( sc.isFailure() )
      {	         
	*m_log << MSG::FATAL << "Failure in MuonDQAHitHistTool::initialize()!" << endreq;
	return sc;
      }
    return sc;
  } 	   
	  
  StatusCode MuonDQAHitHistTool::finalize()
  {
    m_log = new MsgStream(msgSvc(),name());
    StatusCode sc = AlgTool::finalize(); 
    if( sc.isFailure() )
      {
	*m_log << MSG::FATAL << "Failure in MuonDQAHitHistTool::finalize()!" << endreq;
	return sc;
      }
     
    return sc;
  }

  MuonDQAHitHistTool::DetData::DetData() : name(""),res(-9999.),pull(-9999.),error(-9999.),lposX(-9999.),lposY(-9999.) 
  {}

  MuonDQAHitHistTool::DetHistos::DetHistos() :
    MuonDQAHist::MuonDQAHistosBase(),wasBooked(false),wasRegistered(false),res(0),resInner(0),resMiddle(0),resOuter(0),pull(0),pullInner(0),pullMiddle(0),pullOuter(0),error(0),lposX(0),lposY(0),resProf(0),pullProf(0) 
  {}

  MuonDQAHitHistTool::HitResidualData::HitResidualData()  : 
    all(),mdts(),mdtsBarrelA(),mdtsBarrelC(),mdtsECA(),mdtsECC(),
    mdtsBarrelA1(),mdtsBarrelA2(),mdtsBarrelA3(),mdtsBarrelA4(),mdtsBarrelA5(),mdtsBarrelA6(),mdtsBarrelA7(),mdtsBarrelA8(),
    mdtsBarrelC1(),mdtsBarrelC2(),mdtsBarrelC3(),mdtsBarrelC4(),mdtsBarrelC5(),mdtsBarrelC6(),mdtsBarrelC7(),mdtsBarrelC8(),
    mdtsECA1(),mdtsECA2(),mdtsECA3(),mdtsECA4(),mdtsECA5(),mdtsECA6(),mdtsECA7(),mdtsECA8(),
    mdtsECC1(),mdtsECC2(),mdtsECC3(),mdtsECC4(),mdtsECC5(),mdtsECC6(),mdtsECC7(),mdtsECC8(),
    cscsEta(),cscsEta1(),cscsEta2(),cscsEta3(),cscsEta4(),cscsEta5(),cscsEta6(),cscsEta7(),cscsEta8(),cscsEta9(),
    cscsEta10(),cscsEta11(),cscsEta12(),cscsEta13(),cscsEta14(),cscsEta15(),cscsEta16(),
    cscsPhi(),cscsPhi1(),cscsPhi2(),cscsPhi3(),cscsPhi4(),cscsPhi5(),cscsPhi6(),cscsPhi7(),cscsPhi8(),cscsPhi9(),
    cscsPhi10(),cscsPhi11(),cscsPhi12(),cscsPhi13(),cscsPhi14(),cscsPhi15(),cscsPhi16(),
    rpcsEta(),rpcsEtaA(),rpcsEtaC(),
    rpcsEtaA1(),rpcsEtaA2(),rpcsEtaA3(),rpcsEtaA4(),rpcsEtaA5(),rpcsEtaA6(),rpcsEtaA7(),rpcsEtaA8(),
    rpcsEtaC1(),rpcsEtaC2(),rpcsEtaC3(),rpcsEtaC4(),rpcsEtaC5(),rpcsEtaC6(),rpcsEtaC7(),rpcsEtaC8(),
    rpcsPhi(),rpcsPhiA(),rpcsPhiC(),
    rpcsPhiA1(),rpcsPhiA2(),rpcsPhiA3(),rpcsPhiA4(),rpcsPhiA5(),rpcsPhiA6(),rpcsPhiA7(),rpcsPhiA8(),
    rpcsPhiC1(),rpcsPhiC2(),rpcsPhiC3(),rpcsPhiC4(),rpcsPhiC5(),rpcsPhiC6(),rpcsPhiC7(),rpcsPhiC8(),
    tgcsEta(),tgcsEtaA(),tgcsEtaC(),tgcsPhi(),tgcsPhiA(),tgcsPhiC()
  {}
  
  MuonDQAHitHistTool::HitResidualHistos::HitResidualHistos()  : 
    MuonDQAHist::MuonDQAHistosBase(),wasBooked(false),wasRegistered(false),all(),mdts(),
    mdtsBarrelA(),mdtsBarrelC(),mdtsECA(),mdtsECC(),
    mdtsBarrelA1(),mdtsBarrelA2(),mdtsBarrelA3(),mdtsBarrelA4(),mdtsBarrelA5(),mdtsBarrelA6(),mdtsBarrelA7(),mdtsBarrelA8(),
    mdtsBarrelC1(),mdtsBarrelC2(),mdtsBarrelC3(),mdtsBarrelC4(),mdtsBarrelC5(),mdtsBarrelC6(),mdtsBarrelC7(),mdtsBarrelC8(),
    mdtsECA1(),mdtsECA2(),mdtsECA3(),mdtsECA4(),mdtsECA5(),mdtsECA6(),mdtsECA7(),mdtsECA8(),
    mdtsECC1(),mdtsECC2(),mdtsECC3(),mdtsECC4(),mdtsECC5(),mdtsECC6(),mdtsECC7(),mdtsECC8(),
    cscsEta(),cscsEta1(),cscsEta2(),cscsEta3(),cscsEta4(),cscsEta5(),cscsEta6(),cscsEta7(),cscsEta8(),cscsEta9(),
    cscsEta10(),cscsEta11(),cscsEta12(),cscsEta13(),cscsEta14(),cscsEta15(),cscsEta16(),
    cscsPhi(),cscsPhi1(),cscsPhi2(),cscsPhi3(),cscsPhi4(),cscsPhi5(),cscsPhi6(),cscsPhi7(),cscsPhi8(),cscsPhi9(),
    cscsPhi10(),cscsPhi11(),cscsPhi12(),cscsPhi13(),cscsPhi14(),cscsPhi15(),cscsPhi16(),
    rpcsEta(),rpcsEtaA(),rpcsEtaC(),
    rpcsEtaA1(),rpcsEtaA2(),rpcsEtaA3(),rpcsEtaA4(),rpcsEtaA5(),rpcsEtaA6(),rpcsEtaA7(),rpcsEtaA8(),
    rpcsEtaC1(),rpcsEtaC2(),rpcsEtaC3(),rpcsEtaC4(),rpcsEtaC5(),rpcsEtaC6(),rpcsEtaC7(),rpcsEtaC8(),
    rpcsPhi(),rpcsPhiA(),rpcsPhiC(),
    rpcsPhiA1(),rpcsPhiA2(),rpcsPhiA3(),rpcsPhiA4(),rpcsPhiA5(),rpcsPhiA6(),rpcsPhiA7(),rpcsPhiA8(),
    rpcsPhiC1(),rpcsPhiC2(),rpcsPhiC3(),rpcsPhiC4(),rpcsPhiC5(),rpcsPhiC6(),rpcsPhiC7(),rpcsPhiC8(),
    tgcsEta(),tgcsEtaA(),tgcsEtaC(),tgcsPhi(),tgcsPhiA(),tgcsPhiC()
  {}
  
  MuonDQAHitHistTool::AllCountData::AllCountData() : nall(0),nallCh(0)
  {} 
     
  MuonDQAHitHistTool::AllCountHistos::AllCountHistos() : MuonDQAHist::MuonDQAHistosBase(),wasBooked(false),wasRegistered(false),nall(0),nallCh(0)
  {}
 
  MuonDQAHitHistTool::MdtCountData::MdtCountData() : name(""),chi2(-1.),nmdt(0),nmdtCh(0)
  {}
    
  MuonDQAHitHistTool::MdtCountHistos::MdtCountHistos() : MuonDQAHist::MuonDQAHistosBase(),wasBooked(false),wasRegistered(false),nmdt(0),nmdtInner(0),nmdtMiddle(0),nmdtOuter(0),nmdtCh(0),nmdtProf(0),nsegm(0),chi2Prof(0)
  {}
 
  MuonDQAHitHistTool::ClusterCountData::ClusterCountData() : neta(0),nphi(0),netaCh(0),nphiCh(0)
  {}

  MuonDQAHitHistTool::ClusterCountHistos::ClusterCountHistos() : MuonDQAHist::MuonDQAHistosBase(),wasBooked(false),wasRegistered(false),neta(0),nphi(0),netaCh(0),nphiCh(0)
  {}
 
  MuonDQAHitHistTool::HitCountData::HitCountData() : 
    all(),mdts(),mdtsBarrelA(),mdtsBarrelC(),mdtsECA(),mdtsECC(),
    mdtsBarrelA1(),mdtsBarrelA2(),mdtsBarrelA3(),mdtsBarrelA4(),mdtsBarrelA5(),mdtsBarrelA6(),mdtsBarrelA7(),mdtsBarrelA8(),
    mdtsBarrelC1(),mdtsBarrelC2(),mdtsBarrelC3(),mdtsBarrelC4(),mdtsBarrelC5(),mdtsBarrelC6(),mdtsBarrelC7(),mdtsBarrelC8(),
    mdtsECA1(),mdtsECA2(),mdtsECA3(),mdtsECA4(),mdtsECA5(),mdtsECA6(),mdtsECA7(),mdtsECA8(),
    mdtsECC1(),mdtsECC2(),mdtsECC3(),mdtsECC4(),mdtsECC5(),mdtsECC6(),mdtsECC7(),mdtsECC8(),
    cscs(),rpcs(),tgcs()
  {}
    
  MuonDQAHitHistTool::HitCountHistos::HitCountHistos() : 
    MuonDQAHist::MuonDQAHistosBase(),wasBooked(false),wasRegistered(false),
    nprec(0),nprecCh(0),nphi(0),nphiCh(0),netaTrig(0),netaTrigCh(0),
    all(),mdts(),mdtsBarrelA(),mdtsBarrelC(),mdtsECA(),mdtsECC(),
    mdtsBarrelA1(),mdtsBarrelA2(),mdtsBarrelA3(),mdtsBarrelA4(),mdtsBarrelA5(),mdtsBarrelA6(),mdtsBarrelA7(),mdtsBarrelA8(),
    mdtsBarrelC1(),mdtsBarrelC2(),mdtsBarrelC3(),mdtsBarrelC4(),mdtsBarrelC5(),mdtsBarrelC6(),mdtsBarrelC7(),mdtsBarrelC8(),
    mdtsECA1(),mdtsECA2(),mdtsECA3(),mdtsECA4(),mdtsECA5(),mdtsECA6(),mdtsECA7(),mdtsECA8(),
    mdtsECC1(),mdtsECC2(),mdtsECC3(),mdtsECC4(),mdtsECC5(),mdtsECC6(),mdtsECC7(),mdtsECC8(),
    cscs(),rpcs(),tgcs()
  {}
   
  MuonDQAHitHistTool::HitData::HitData() : residualData(),countData()
  {}

  MuonDQAHitHistTool::HitHistos::HitHistos() : MuonDQAHist::MuonDQAHistosBase(),wasBooked(false),wasRegistered(false),residualHistos(),countHistos()
  {}

    
  bool MuonDQAHitHistTool::book( AllCountHistos& histos, std::string nallName, std::string nallChName, std::string prefix ) const {
    if( histos.wasBooked ) return false;
    MuonDQAHist::HistDQADataHelper* helper = MuonDQAHist::HistDQADataHelper::getHelper();
    histos.nall      = helper->createHist(nallName,prefix);
    histos.nallCh    = helper->createHist(nallChName,prefix);
    histos.wasBooked = true;
    return true;
  }

  bool MuonDQAHitHistTool::book( MdtCountHistos& histos, std::string nmdtName, std::string nmdtChName, std::string prefix ) const {
    if( histos.wasBooked ) return false;
    MuonDQAHist::HistDQADataHelper* helper = MuonDQAHist::HistDQADataHelper::getHelper();

    if ( strlen(prefix.c_str())!=14 && strlen(prefix.c_str())!=10 ) {    // only for Global and region - not sector
      histos.nmdt = helper->createHist(nmdtName,prefix);
    }
    else { // only for sector
      histos.nmdtInner = helper->createHist(nmdtName,prefix+"Inner");
      histos.nmdtMiddle = helper->createHist(nmdtName,prefix+"Middle");
      histos.nmdtOuter = helper->createHist(nmdtName,prefix+"Outer");
    }
    //    if ( strspn(prefix.c_str(),"Barrel")!=6 && strspn(prefix.c_str(),"EC")!=2 ) {    // only for Global
    if ( strstr(prefix.c_str(),"Barrel")==NULL && strstr(prefix.c_str(),"EC")==NULL ) {    // only for Global
      histos.nmdtCh = helper->createHist(nmdtChName,prefix);
    }
    histos.nmdtProf = helper->createHistProf(nmdtName+"Prof",prefix);
    if (histos.nmdtProf) histos.nmdtProf->SetBit(TH1::kCanRebin);
    histos.nsegm = helper->createHist("nsegm",prefix);
    if (histos.nsegm) histos.nsegm->SetBit(TH1::kCanRebin);
    histos.chi2Prof = helper->createHistProf("chi2Prof",prefix);
    if (histos.chi2Prof) histos.chi2Prof->SetBit(TH1::kCanRebin);

    histos.wasBooked = true;
    return true;
  }

  bool MuonDQAHitHistTool::book( ClusterCountHistos& histos, std::string netaName, std::string nphiName, 
				 std::string netaChName, std::string nphiChName, std::string prefix ) const {
    if( histos.wasBooked ) return false;

    MuonDQAHist::HistDQADataHelper* helper = MuonDQAHist::HistDQADataHelper::getHelper();
    histos.neta      = helper->createHist(netaName,prefix);
    histos.netaCh    = helper->createHist(netaChName,prefix);
    histos.nphi      = helper->createHist(nphiName,prefix);
    histos.nphiCh    = helper->createHist(nphiChName,prefix);
    histos.wasBooked = true;
    return true;
  }

  bool MuonDQAHitHistTool::book( HitCountHistos& histos, std::string prefix, bool doMDT, bool doRPC, bool doTGC, bool doCSC ) const {
    if( histos.wasBooked ) return false;

    MuonDQAHist::HistDQADataHelper* helper = MuonDQAHist::HistDQADataHelper::getHelper();

    if (doMDT || doCSC) {
      histos.nprec       = helper->createHist("nprec",prefix);
      histos.nprecCh     = helper->createHist("nprecCh",prefix); 
    }
    if (doRPC || doTGC || doCSC ) {
      histos.nphi        = helper->createHist("nphiHits",prefix);
      histos.nphiCh      = helper->createHist("nphiCh",prefix);
    }
    if (doRPC || doTGC) {
      histos.netaTrig    = helper->createHist("netaTrigHits",prefix);
      histos.netaTrigCh  = helper->createHist("netaTrigCh",prefix);
    }

    book( histos.all,"nall","nallCh",prefix );
    if (doMDT) {
      book( histos.mdts,"nmdt","nmdtCh",prefix );
      book( histos.mdtsBarrelA,"nmdt","nmdtCh",prefix+"BarrelA" );
      book( histos.mdtsBarrelC,"nmdt","nmdtCh",prefix+"BarrelC" );
      book( histos.mdtsECA,"nmdt","nmdtCh",prefix+"ECA" );
      book( histos.mdtsECC,"nmdt","nmdtCh",prefix+"ECC" );

      book( histos.mdtsBarrelA1,"nmdt","nmdtCh",prefix+"BarrelA1" );
      book( histos.mdtsBarrelA2,"nmdt","nmdtCh",prefix+"BarrelA2" );
      book( histos.mdtsBarrelA3,"nmdt","nmdtCh",prefix+"BarrelA3" );
      book( histos.mdtsBarrelA4,"nmdt","nmdtCh",prefix+"BarrelA4" );
      book( histos.mdtsBarrelA5,"nmdt","nmdtCh",prefix+"BarrelA5" );
      book( histos.mdtsBarrelA6,"nmdt","nmdtCh",prefix+"BarrelA6" );
      book( histos.mdtsBarrelA7,"nmdt","nmdtCh",prefix+"BarrelA7" );
      book( histos.mdtsBarrelA8,"nmdt","nmdtCh",prefix+"BarrelA8" );

      book( histos.mdtsBarrelC1,"nmdt","nmdtCh",prefix+"BarrelC1" );
      book( histos.mdtsBarrelC2,"nmdt","nmdtCh",prefix+"BarrelC2" );
      book( histos.mdtsBarrelC3,"nmdt","nmdtCh",prefix+"BarrelC3" );
      book( histos.mdtsBarrelC4,"nmdt","nmdtCh",prefix+"BarrelC4" );
      book( histos.mdtsBarrelC5,"nmdt","nmdtCh",prefix+"BarrelC5" );
      book( histos.mdtsBarrelC6,"nmdt","nmdtCh",prefix+"BarrelC6" );
      book( histos.mdtsBarrelC7,"nmdt","nmdtCh",prefix+"BarrelC7" );
      book( histos.mdtsBarrelC8,"nmdt","nmdtCh",prefix+"BarrelC8" );

      book( histos.mdtsECA1,"nmdt","nmdtCh",prefix+"ECA1" );
      book( histos.mdtsECA2,"nmdt","nmdtCh",prefix+"ECA2" );
      book( histos.mdtsECA3,"nmdt","nmdtCh",prefix+"ECA3" );
      book( histos.mdtsECA4,"nmdt","nmdtCh",prefix+"ECA4" );
      book( histos.mdtsECA5,"nmdt","nmdtCh",prefix+"ECA5" );
      book( histos.mdtsECA6,"nmdt","nmdtCh",prefix+"ECA6" );
      book( histos.mdtsECA7,"nmdt","nmdtCh",prefix+"ECA7" );
      book( histos.mdtsECA8,"nmdt","nmdtCh",prefix+"ECA8" );

      book( histos.mdtsECC1,"nmdt","nmdtCh",prefix+"ECC1" );
      book( histos.mdtsECC2,"nmdt","nmdtCh",prefix+"ECC2" );
      book( histos.mdtsECC3,"nmdt","nmdtCh",prefix+"ECC3" );
      book( histos.mdtsECC4,"nmdt","nmdtCh",prefix+"ECC4" );
      book( histos.mdtsECC5,"nmdt","nmdtCh",prefix+"ECC5" );
      book( histos.mdtsECC6,"nmdt","nmdtCh",prefix+"ECC6" );
      book( histos.mdtsECC7,"nmdt","nmdtCh",prefix+"ECC7" );
      book( histos.mdtsECC8,"nmdt","nmdtCh",prefix+"ECC8" );
    }
    if (doCSC) book( histos.cscs,"ncscEta","ncscPhi","ncscEtaCh","ncscPhiCh",prefix );
    if (doRPC) book( histos.rpcs,"nrpcEta","nrpcPhi","nrpcEtaCh","nrpcPhiCh",prefix );
    if (doTGC) book( histos.tgcs,"ntgcEta","ntgcPhi","ntgcEtaCh","ntgcPhiCh",prefix );
    histos.wasBooked = true;
    return true;
  }


  bool MuonDQAHitHistTool::book( MuonDQAHitHistTool::DetHistos& histos, 
				 std::string resName, std::string pullName, std::string errorName, std::string lposXName, std::string lposYName,
				 std::string prefix ) const {
    if( histos.wasBooked ) return false;
    MuonDQAHist::HistDQADataHelper* helper = MuonDQAHist::HistDQADataHelper::getHelper();
    if ( strlen(prefix.c_str())!=14 && strlen(prefix.c_str())!=10 ) {    // only for Global and region - not sector
      histos.res   = helper->createHist(resName,prefix);
      histos.pull  = helper->createHist(pullName,prefix);
    }
    else { // only for sector
      histos.resInner   = helper->createHist(resName,prefix+"Inner");
      histos.resMiddle  = helper->createHist(resName,prefix+"Middle");
      histos.resOuter   = helper->createHist(resName,prefix+"Outer");
      histos.pullInner  = helper->createHist(pullName,prefix+"Inner");
      histos.pullMiddle = helper->createHist(pullName,prefix+"Middle");
      histos.pullOuter  = helper->createHist(pullName,prefix+"Outer");
    }
    histos.error = helper->createHist(errorName,prefix);
    histos.lposX = helper->createHist(lposXName,prefix);
    histos.lposY = helper->createHist(lposYName,prefix);
    histos.resProf = helper->createHistProf(resName+"Prof",prefix);
    if (histos.resProf) histos.resProf->SetBit(TH1::kCanRebin);
    histos.pullProf = helper->createHistProf(pullName+"Prof",prefix);
    if (histos.pullProf) histos.pullProf->SetBit(TH1::kCanRebin);
    histos.wasBooked = true;
    return true;
  }

  bool MuonDQAHitHistTool::book( HitHistos& histos, std::string prefix, bool doMDT, bool doRPC, bool doTGC, bool doCSC ) const {
    if( histos.wasBooked ) return false;
    book( histos.residualHistos, prefix, doMDT, doRPC, doTGC, doCSC );
    book( histos.countHistos, prefix, doMDT, doRPC, doTGC, doCSC );   
    histos.wasBooked = true;
    return true;
  }

  bool MuonDQAHitHistTool::book( HitResidualHistos& histos, std::string prefix, bool doMDT, bool doRPC, bool doTGC, bool doCSC ) const {
    if( histos.wasBooked ) return false;
    book( histos.all,"allRes","allPull","allError","allRelPosX","allRelPosY", prefix);

    if (doMDT) {
      book( histos.mdts,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix);    
      book( histos.mdtsBarrelA,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY",prefix+"BarrelA");
      book( histos.mdtsBarrelC,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY",prefix+"BarrelC");
      book( histos.mdtsECA,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECA");
      book( histos.mdtsECC,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECC");

      book( histos.mdtsBarrelA1,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelA1");
      book( histos.mdtsBarrelA2,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelA2");
      book( histos.mdtsBarrelA3,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelA3");
      book( histos.mdtsBarrelA4,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelA4");
      book( histos.mdtsBarrelA5,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelA5");
      book( histos.mdtsBarrelA6,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelA6");
      book( histos.mdtsBarrelA7,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelA7");
      book( histos.mdtsBarrelA8,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelA8");

      book( histos.mdtsBarrelC1,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelC1");
      book( histos.mdtsBarrelC2,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelC2");
      book( histos.mdtsBarrelC3,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelC3");
      book( histos.mdtsBarrelC4,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelC4");
      book( histos.mdtsBarrelC5,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelC5");
      book( histos.mdtsBarrelC6,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelC6");
      book( histos.mdtsBarrelC7,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelC7");
      book( histos.mdtsBarrelC8,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"BarrelC8");
      
      book( histos.mdtsECA1,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECA1");
      book( histos.mdtsECA2,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECA2");
      book( histos.mdtsECA3,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECA3");
      book( histos.mdtsECA4,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECA4");
      book( histos.mdtsECA5,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECA5");
      book( histos.mdtsECA6,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECA6");
      book( histos.mdtsECA7,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECA7");
      book( histos.mdtsECA8,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECA8");

      book( histos.mdtsECC1,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECC1");
      book( histos.mdtsECC2,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECC2");
      book( histos.mdtsECC3,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECC3");
      book( histos.mdtsECC4,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECC4");
      book( histos.mdtsECC5,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECC5");
      book( histos.mdtsECC6,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECC6");
      book( histos.mdtsECC7,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECC7");
      book( histos.mdtsECC8,"mdtRes","mdtPull","mdtError","mdtSignedRadius","mdtRelPosY", prefix+"ECC8");
    }

    if (doCSC) {
      std::string hname = prefix;
      book( histos.cscsEta,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);      
      hname = "Sector1";
      book( histos.cscsEta1,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      hname = "Sector2";
      book( histos.cscsEta2,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      hname = "Sector3";
      book( histos.cscsEta3,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      hname = "Sector4";
      book( histos.cscsEta4,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      hname = "Sector5";
      book( histos.cscsEta5,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      hname = "Sector6";
      book( histos.cscsEta6,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      hname = "Sector7";
      book( histos.cscsEta7,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      hname = "Sector8";
      book( histos.cscsEta8,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      hname = "Sector9";
      book( histos.cscsEta9,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      hname = "Sector10";
      book( histos.cscsEta10,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      hname = "Sector11";
      book( histos.cscsEta11,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      hname = "Sector12";
      book( histos.cscsEta12,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      hname = "Sector13";
      book( histos.cscsEta13,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      hname = "Sector14";
      book( histos.cscsEta14,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      hname = "Sector15";
      book( histos.cscsEta15,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      hname = "Sector16";
      book( histos.cscsEta16,"cscResEta","cscPullEta","cscErrorEta","cscRelPosXEta","cscRelPosYEta",hname);
      
      hname = prefix;
      book( histos.cscsPhi,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      
      hname = "Sector1";
      book( histos.cscsPhi1,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      hname = "Sector2";
      book( histos.cscsPhi2,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      hname = "Sector3";
      book( histos.cscsPhi3,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      hname = "Sector4";
      book( histos.cscsPhi4,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      hname = "Sector5";
      book( histos.cscsPhi5,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      hname = "Sector6";
      book( histos.cscsPhi6,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      hname = "Sector7";
      book( histos.cscsPhi7,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      hname = "Sector8";
      book( histos.cscsPhi8,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      hname = "Sector9";
      book( histos.cscsPhi9,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      hname = "Sector10";
      book( histos.cscsPhi10,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      hname = "Sector11";
      book( histos.cscsPhi11,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      hname = "Sector12";
      book( histos.cscsPhi12,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      hname = "Sector13";
      book( histos.cscsPhi13,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      hname = "Sector14";
      book( histos.cscsPhi14,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      hname = "Sector15";
      book( histos.cscsPhi15,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
      hname = "Sector16";
      book( histos.cscsPhi16,"cscResPhi","cscPullPhi","cscErrorPhi","cscRelPosXPhi","cscRelPosYPhi",hname);
    } 

    if (doRPC) {
      book( histos.rpcsEta,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix);
      book( histos.rpcsEtaA,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SideA");
      book( histos.rpcsEtaC,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SideC");
      
      book( histos.rpcsEtaA1,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorA1");
      book( histos.rpcsEtaA2,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorA2");
      book( histos.rpcsEtaA3,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorA3");
      book( histos.rpcsEtaA4,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorA4");
      book( histos.rpcsEtaA5,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorA5");
      book( histos.rpcsEtaA6,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorA6");
      book( histos.rpcsEtaA7,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorA7");
      book( histos.rpcsEtaA8,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorA8");

      book( histos.rpcsEtaC1,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorC1");
      book( histos.rpcsEtaC2,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorC2");
      book( histos.rpcsEtaC3,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorC3");
      book( histos.rpcsEtaC4,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorC4");
      book( histos.rpcsEtaC5,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorC5");
      book( histos.rpcsEtaC6,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorC6");
      book( histos.rpcsEtaC7,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorC7");
      book( histos.rpcsEtaC8,"rpcResEta","rpcPullEta","rpcErrorEta","rpcRelPosXEta","rpcRelPosYEta",prefix+"SectorC8");
       
      book( histos.rpcsPhi,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix);
      book( histos.rpcsPhiA,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SideA");
      book( histos.rpcsPhiC,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SideC");
      	   
      book( histos.rpcsPhiA1,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorA1");
      book( histos.rpcsPhiA2,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorA2");
      book( histos.rpcsPhiA3,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorA3");
      book( histos.rpcsPhiA4,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorA4");
      book( histos.rpcsPhiA5,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorA5");
      book( histos.rpcsPhiA6,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorA6");
      book( histos.rpcsPhiA7,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorA7");
      book( histos.rpcsPhiA8,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorA8");

      book( histos.rpcsPhiC1,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorC1");
      book( histos.rpcsPhiC2,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorC2");
      book( histos.rpcsPhiC3,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorC3");
      book( histos.rpcsPhiC4,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorC4");
      book( histos.rpcsPhiC5,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorC5");
      book( histos.rpcsPhiC6,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorC6");
      book( histos.rpcsPhiC7,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorC7");
      book( histos.rpcsPhiC8,"rpcResPhi","rpcPullPhi","rpcErrorPhi","rpcRelPosXPhi","rpcRelPosYPhi",prefix+"SectorC8");
    }

    if (doTGC) {
      book( histos.tgcsEta,"tgcResEta","tgcPullEta","tgcErrorEta","tgcRelPosXEta","tgcRelPosYEta",prefix);      
      book( histos.tgcsEtaA,"tgcResEta","tgcPullEta","tgcErrorEta","tgcRelPosXEta","tgcRelPosYEta",prefix+"SideA");      
      book( histos.tgcsEtaC,"tgcResEta","tgcPullEta","tgcErrorEta","tgcRelPosXEta","tgcRelPosYEta",prefix+"SideC");      
      
      book( histos.tgcsPhi,"tgcResPhi","tgcPullPhi","tgcErrorPhi","tgcRelPosXPhi","tgcRelPosYPhi",prefix);
      book( histos.tgcsPhiA,"tgcResPhi","tgcPullPhi","tgcErrorPhi","tgcRelPosXPhi","tgcRelPosYPhi",prefix+"SideA");
      book( histos.tgcsPhiC,"tgcResPhi","tgcPullPhi","tgcErrorPhi","tgcRelPosXPhi","tgcRelPosYPhi",prefix+"SideC");
      
    }

    histos.wasBooked = true;
    return true;
    
  }


  void MuonDQAHitHistTool::registerHist( ManagedMonitorToolBase::MonGroup&  groupShift, ManagedMonitorToolBase::MonGroup&  groupDebug, ManagedMonitorToolBase::MonGroup&  groupBarrelA, ManagedMonitorToolBase::MonGroup&  groupBarrelC, ManagedMonitorToolBase::MonGroup&  groupEC, ManagedMonitorToolBase::MonGroup&  groupECA, ManagedMonitorToolBase::MonGroup&  groupECC, HitHistos& histos )   
  {
    if( histos.wasRegistered ) return;
    registerHist(groupShift,groupDebug,groupBarrelA,groupBarrelC,groupEC,groupECA,groupECC,histos.residualHistos);    
    registerHist(groupShift,groupDebug,groupBarrelA,groupBarrelC,groupEC,groupECA,groupECC,histos.countHistos);
    histos.wasRegistered = true;
  }


  void MuonDQAHitHistTool::registerHist( ManagedMonitorToolBase::MonGroup&  groupShift, ManagedMonitorToolBase::MonGroup&  groupDebug, ManagedMonitorToolBase::MonGroup&  groupBarrelA, ManagedMonitorToolBase::MonGroup&  groupBarrelC, ManagedMonitorToolBase::MonGroup&  groupEC, ManagedMonitorToolBase::MonGroup&  groupECA, ManagedMonitorToolBase::MonGroup&  groupECC, HitResidualHistos& histos ) 
  {
    if( histos.wasRegistered ) return;
    registerHist(groupDebug,histos.all); 
    registerHist(groupShift,groupDebug,histos.mdts);

    registerHist(groupBarrelA,histos.mdtsBarrelA);
    registerHist(groupBarrelA,histos.mdtsBarrelA1);
    registerHist(groupBarrelA,histos.mdtsBarrelA2);    
    registerHist(groupBarrelA,histos.mdtsBarrelA3);    
    registerHist(groupBarrelA,histos.mdtsBarrelA4);    
    registerHist(groupBarrelA,histos.mdtsBarrelA5);    
    registerHist(groupBarrelA,histos.mdtsBarrelA6);    
    registerHist(groupBarrelA,histos.mdtsBarrelA7);    
    registerHist(groupBarrelA,histos.mdtsBarrelA8);    

    registerHist(groupBarrelC,histos.mdtsBarrelC);
    registerHist(groupBarrelC,histos.mdtsBarrelC1);   
    registerHist(groupBarrelC,histos.mdtsBarrelC2);
    registerHist(groupBarrelC,histos.mdtsBarrelC3);    
    registerHist(groupBarrelC,histos.mdtsBarrelC4);    
    registerHist(groupBarrelC,histos.mdtsBarrelC5);    
    registerHist(groupBarrelC,histos.mdtsBarrelC6);    
    registerHist(groupBarrelC,histos.mdtsBarrelC7);    
    registerHist(groupBarrelC,histos.mdtsBarrelC8);    

    registerHist(groupECA,histos.mdtsECA);
    registerHist(groupECA,histos.mdtsECA1);    
    registerHist(groupECA,histos.mdtsECA2);    
    registerHist(groupECA,histos.mdtsECA3);    
    registerHist(groupECA,histos.mdtsECA4);    
    registerHist(groupECA,histos.mdtsECA5);    
    registerHist(groupECA,histos.mdtsECA6);    
    registerHist(groupECA,histos.mdtsECA7);    
    registerHist(groupECA,histos.mdtsECA8);    

    registerHist(groupECC,histos.mdtsECC);
    registerHist(groupECC,histos.mdtsECC1);
    registerHist(groupECC,histos.mdtsECC2);    
    registerHist(groupECC,histos.mdtsECC3);    
    registerHist(groupECC,histos.mdtsECC4);    
    registerHist(groupECC,histos.mdtsECC5);    
    registerHist(groupECC,histos.mdtsECC6);    
    registerHist(groupECC,histos.mdtsECC7);    
    registerHist(groupECC,histos.mdtsECC8);    

    registerHist(groupShift,groupDebug,histos.cscsEta);    
    registerHist(groupEC,histos.cscsEta1);    
    registerHist(groupEC,histos.cscsEta2);    
    registerHist(groupEC,histos.cscsEta3);    
    registerHist(groupEC,histos.cscsEta4);    
    registerHist(groupEC,histos.cscsEta5);    
    registerHist(groupEC,histos.cscsEta6);    
    registerHist(groupEC,histos.cscsEta7);    
    registerHist(groupEC,histos.cscsEta8);    
    registerHist(groupEC,histos.cscsEta9);    
    registerHist(groupEC,histos.cscsEta10);    
    registerHist(groupEC,histos.cscsEta11);    
    registerHist(groupEC,histos.cscsEta12);    
    registerHist(groupEC,histos.cscsEta13);    
    registerHist(groupEC,histos.cscsEta14);    
    registerHist(groupEC,histos.cscsEta15);    
    registerHist(groupEC,histos.cscsEta16);    
    registerHist(groupShift,groupDebug,histos.cscsPhi);    
    registerHist(groupEC,histos.cscsPhi1);    
    registerHist(groupEC,histos.cscsPhi2);    
    registerHist(groupEC,histos.cscsPhi3);    
    registerHist(groupEC,histos.cscsPhi4);    
    registerHist(groupEC,histos.cscsPhi5);    
    registerHist(groupEC,histos.cscsPhi6);    
    registerHist(groupEC,histos.cscsPhi7);    
    registerHist(groupEC,histos.cscsPhi8);    
    registerHist(groupEC,histos.cscsPhi9);    
    registerHist(groupEC,histos.cscsPhi10);    
    registerHist(groupEC,histos.cscsPhi11);    
    registerHist(groupEC,histos.cscsPhi12);    
    registerHist(groupEC,histos.cscsPhi13);    
    registerHist(groupEC,histos.cscsPhi14);    
    registerHist(groupEC,histos.cscsPhi15);    
    registerHist(groupEC,histos.cscsPhi16);    

    registerHist(groupShift,groupDebug,histos.rpcsEta);   
    registerHist(groupBarrelA,histos.rpcsEtaA);    
    registerHist(groupBarrelC,histos.rpcsEtaC);    

    registerHist(groupBarrelA,histos.rpcsEtaA1);    
    registerHist(groupBarrelA,histos.rpcsEtaA2);    
    registerHist(groupBarrelA,histos.rpcsEtaA3);    
    registerHist(groupBarrelA,histos.rpcsEtaA4);    
    registerHist(groupBarrelA,histos.rpcsEtaA5);    
    registerHist(groupBarrelA,histos.rpcsEtaA6);    
    registerHist(groupBarrelA,histos.rpcsEtaA7);    
    registerHist(groupBarrelA,histos.rpcsEtaA8);    

    registerHist(groupBarrelC,histos.rpcsEtaC1);
    registerHist(groupBarrelC,histos.rpcsEtaC2);    
    registerHist(groupBarrelC,histos.rpcsEtaC3);    
    registerHist(groupBarrelC,histos.rpcsEtaC4);    
    registerHist(groupBarrelC,histos.rpcsEtaC5);    
    registerHist(groupBarrelC,histos.rpcsEtaC6);    
    registerHist(groupBarrelC,histos.rpcsEtaC7);    
    registerHist(groupBarrelC,histos.rpcsEtaC8);    

    registerHist(groupShift,groupDebug,histos.rpcsPhi);   
    registerHist(groupBarrelA,histos.rpcsPhiA);    
    registerHist(groupBarrelC,histos.rpcsPhiC);    

    registerHist(groupBarrelA,histos.rpcsPhiA1);    
    registerHist(groupBarrelA,histos.rpcsPhiA2);    
    registerHist(groupBarrelA,histos.rpcsPhiA3);    
    registerHist(groupBarrelA,histos.rpcsPhiA4);    
    registerHist(groupBarrelA,histos.rpcsPhiA5);    
    registerHist(groupBarrelA,histos.rpcsPhiA6);    
    registerHist(groupBarrelA,histos.rpcsPhiA7);    
    registerHist(groupBarrelA,histos.rpcsPhiA8);    

    registerHist(groupBarrelC,histos.rpcsPhiC1);
    registerHist(groupBarrelC,histos.rpcsPhiC2);    
    registerHist(groupBarrelC,histos.rpcsPhiC3);    
    registerHist(groupBarrelC,histos.rpcsPhiC4);    
    registerHist(groupBarrelC,histos.rpcsPhiC5);    
    registerHist(groupBarrelC,histos.rpcsPhiC6);    
    registerHist(groupBarrelC,histos.rpcsPhiC7);    
    registerHist(groupBarrelC,histos.rpcsPhiC8);    

    registerHist(groupShift,groupDebug,histos.tgcsEta);   
    registerHist(groupECA,histos.tgcsEtaA);   
    registerHist(groupECC,histos.tgcsEtaC);
    registerHist(groupShift,groupDebug,histos.tgcsPhi);    
    registerHist(groupECA,histos.tgcsPhiA);    
    registerHist(groupECC,histos.tgcsPhiC);    
    histos.wasRegistered = true;
  }


  void MuonDQAHitHistTool::registerHist( ManagedMonitorToolBase::MonGroup&  groupShift, ManagedMonitorToolBase::MonGroup&  groupDebug, DetHistos& histos ) 
  { 
    if( histos.wasRegistered ) return;
    if (histos.res) {
      StatusCode sc=groupDebug.regHist(histos.res);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register res Histograms:" << endreq;
      }
    }
    if (histos.resInner) {
      StatusCode sc=groupDebug.regHist(histos.resInner);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register resInner Histograms:" << endreq;
      }
    }
    if (histos.resMiddle) {
      StatusCode sc=groupDebug.regHist(histos.resMiddle);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register resMiddle Histograms:" << endreq;
      }
    }
    if (histos.resOuter) {
      StatusCode sc=groupDebug.regHist(histos.resOuter);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register resOuter Histograms:" << endreq;
      }
    }
    if (histos.pull){ 
      StatusCode sc=groupShift.regHist(histos.pull);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register pull Histograms:" << endreq;
      } 
    }
    if (histos.pullInner){ 
      StatusCode sc=groupDebug.regHist(histos.pullInner);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register pullInner Histograms:" << endreq;
      } 
    }
    if (histos.pullMiddle){ 
      StatusCode sc=groupDebug.regHist(histos.pullMiddle);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register pullMiddle Histograms:" << endreq;
      } 
    }
    if (histos.pullOuter){ 
      StatusCode sc=groupDebug.regHist(histos.pullOuter);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register pullOuter Histograms:" << endreq;
      } 
    }
    if (histos.error){
      StatusCode sc=groupDebug.regHist(histos.error);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register error Histograms:" << endreq;
      }
    }
    if (histos.lposX) {
      if ( strstr(histos.lposX->GetTitle(),"MDT") != NULL ) { 
	StatusCode sc=groupShift.regHist(histos.lposX); 
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register lposX Histograms:" << endreq;
	}
      }
      else { 
	StatusCode sc=groupDebug.regHist(histos.lposX); 
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register lposX Histograms:" << endreq;
	}
      }
    }
    if (histos.lposY){    
      StatusCode sc=groupDebug.regHist(histos.lposY);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register lposY Histograms:" << endreq;
      }
    }
    if (histos.resProf){    
      StatusCode sc=groupDebug.regHist(histos.resProf);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register resProf Histograms:" << endreq;
      }
    }
    if (histos.pullProf){    
      StatusCode sc=groupDebug.regHist(histos.pullProf);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register pullProf Histograms:" << endreq;
      }
    }
    histos.wasRegistered = true;
  }
  

  void MuonDQAHitHistTool::registerHist( ManagedMonitorToolBase::MonGroup&  group, DetHistos& histos ) 
  {
    if( histos.wasRegistered ) return;
    if (histos.res)  
      {
	StatusCode sc=group.regHist(histos.res);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register res Histograms:" << endreq;
	}
      }
    if (histos.resInner)  
      {
	StatusCode sc=group.regHist(histos.resInner);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register resInner Histograms:" << endreq;
	}
      }
    if (histos.resMiddle)  
      {
	StatusCode sc=group.regHist(histos.resMiddle);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register resMiddle Histograms:" << endreq;
	}
      }
    if (histos.resOuter)  
      {
	StatusCode sc=group.regHist(histos.resOuter);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register resOuter Histograms:" << endreq;
	}
      }
    if (histos.pull)  
      {
	StatusCode sc=group.regHist(histos.pull);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register pull Histograms:" << endreq;
	}
      }
    if (histos.pullInner)  
      {
	StatusCode sc=group.regHist(histos.pullInner);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register pullInner Histograms:" << endreq;
	}
      }
    if (histos.pullMiddle)  
      {
	StatusCode sc=group.regHist(histos.pullMiddle);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register pullMiddle Histograms:" << endreq;
	}
      }
    if (histos.pullOuter)  
      {
	StatusCode sc=group.regHist(histos.pullOuter);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register pullOuter Histograms:" << endreq;
	}
      }
    if (histos.error){
      StatusCode sc=group.regHist(histos.error);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register error Histograms:" << endreq;
      }
    }
    if (histos.lposX)
      {
	StatusCode sc=group.regHist(histos.lposX);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register lposX Histograms:" << endreq;
	}
      }
    if (histos.lposY)
      {
	StatusCode sc=group.regHist(histos.lposY);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register lposY Histograms:" << endreq;
	}
      }
    if (histos.resProf)
      {
	StatusCode sc=group.regHist(histos.resProf);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register resProf Histograms:" << endreq;
	}
      }
    if (histos.pullProf)
      {
	StatusCode sc=group.regHist(histos.pullProf);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register pullProf Histograms:" << endreq;
	}
      }
    histos.wasRegistered = true;
  }

  void MuonDQAHitHistTool::registerHist( ManagedMonitorToolBase::MonGroup&  groupShift, ManagedMonitorToolBase::MonGroup&  groupDebug, ManagedMonitorToolBase::MonGroup&  groupBarrelA, ManagedMonitorToolBase::MonGroup&  groupBarrelC, ManagedMonitorToolBase::MonGroup&  /*groupEC*/, ManagedMonitorToolBase::MonGroup&  groupECA, ManagedMonitorToolBase::MonGroup&  groupECC, HitCountHistos& histos )
  {
    StatusCode sc;
    if( histos.wasRegistered ) return;
    if (histos.nprec) {
      sc=groupDebug.regHist(histos.nprec);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register nprec Histograms:" << endreq;
      }
    }
    if (histos.nprecCh)
      { 
	sc=groupDebug.regHist(histos.nprecCh);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register nprecCh Histograms:" << endreq;
	}
      }
	 
    if (histos.nphi){
      sc=groupDebug.regHist(histos.nphi);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register nphi Histograms:" << endreq;
      }
    }
    if (histos.nphiCh)
      {
	sc=groupDebug.regHist(histos.nphiCh);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register nphiCh Histograms:" << endreq;
	}
      }
    if (histos.netaTrig) 
      {
	sc=groupDebug.regHist(histos.netaTrig);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register netaTrig Histograms:" << endreq;
	}
      }
    if (histos.netaTrigCh) 
      {
	sc=groupDebug.regHist(histos.netaTrigCh);
	if (sc.isFailure() ) {
	  *m_log << MSG::FATAL << "Cannot register netaTrigCh Histograms:" << endreq;
	}
      }    

    registerHist(groupShift,groupDebug,histos.all);
    registerHist(groupDebug,histos.mdts);
    registerHist(groupBarrelA,histos.mdtsBarrelA);
    registerHist(groupBarrelC,histos.mdtsBarrelC);
    registerHist(groupECA,histos.mdtsECA);
    registerHist(groupECC,histos.mdtsECC);

    registerHist(groupBarrelA,histos.mdtsBarrelA1);
    registerHist(groupBarrelA,histos.mdtsBarrelA2);
    registerHist(groupBarrelA,histos.mdtsBarrelA3);
    registerHist(groupBarrelA,histos.mdtsBarrelA4);
    registerHist(groupBarrelA,histos.mdtsBarrelA5);
    registerHist(groupBarrelA,histos.mdtsBarrelA6);
    registerHist(groupBarrelA,histos.mdtsBarrelA7);
    registerHist(groupBarrelA,histos.mdtsBarrelA8);

    registerHist(groupBarrelC,histos.mdtsBarrelC1);
    registerHist(groupBarrelC,histos.mdtsBarrelC2);
    registerHist(groupBarrelC,histos.mdtsBarrelC3);
    registerHist(groupBarrelC,histos.mdtsBarrelC4);
    registerHist(groupBarrelC,histos.mdtsBarrelC5);
    registerHist(groupBarrelC,histos.mdtsBarrelC6);
    registerHist(groupBarrelC,histos.mdtsBarrelC7);
    registerHist(groupBarrelC,histos.mdtsBarrelC8);

    registerHist(groupECA,histos.mdtsECA1);
    registerHist(groupECA,histos.mdtsECA2);
    registerHist(groupECA,histos.mdtsECA3);
    registerHist(groupECA,histos.mdtsECA4);
    registerHist(groupECA,histos.mdtsECA5);
    registerHist(groupECA,histos.mdtsECA6);
    registerHist(groupECA,histos.mdtsECA7);
    registerHist(groupECA,histos.mdtsECA8);

    registerHist(groupECC,histos.mdtsECC1);
    registerHist(groupECC,histos.mdtsECC2);
    registerHist(groupECC,histos.mdtsECC3);
    registerHist(groupECC,histos.mdtsECC4);
    registerHist(groupECC,histos.mdtsECC5);
    registerHist(groupECC,histos.mdtsECC6);
    registerHist(groupECC,histos.mdtsECC7);
    registerHist(groupECC,histos.mdtsECC8);

    registerHist(groupDebug,histos.cscs);
    registerHist(groupDebug,histos.rpcs);
    registerHist(groupDebug,histos.tgcs);
    
    histos.wasRegistered = true;
  }


  void MuonDQAHitHistTool::registerHist( ManagedMonitorToolBase::MonGroup&  groupShift, ManagedMonitorToolBase::MonGroup&  groupDebug, AllCountHistos& histos ) 
  {
    StatusCode sc;
    if( histos.wasRegistered ) return;
    if (histos.nall) sc = groupShift.regHist(histos.nall);
    if (sc.isFailure() ) {
      *m_log << MSG::FATAL << "Cannot register nall Histograms:" << endreq;
    }
    if (histos.nallCh) sc= groupDebug.regHist(histos.nallCh);
    if (sc.isFailure() ) {
      *m_log << MSG::FATAL << "Cannot register nallCh Histograms:" << endreq;
    }    
    histos.wasRegistered = true;
  }


  void MuonDQAHitHistTool::registerHist(ManagedMonitorToolBase::MonGroup&  groupDebug, MdtCountHistos& histos )
  {
    StatusCode sc;
    if( histos.wasRegistered ) return;

    if (histos.nmdt) sc=groupDebug.regHist(histos.nmdt);
    if (sc.isFailure() ) {
      *m_log << MSG::FATAL << "Cannot register nmdt Histograms:" << endreq;
    } 
    if (histos.nmdtInner) sc=groupDebug.regHist(histos.nmdtInner);
    if (sc.isFailure() ) {
      *m_log << MSG::FATAL << "Cannot register nmdtInner Histograms:" << endreq;
    } 
    if (histos.nmdtMiddle) sc=groupDebug.regHist(histos.nmdtMiddle);
    if (sc.isFailure() ) {
      *m_log << MSG::FATAL << "Cannot register nmdtMiddle Histograms:" << endreq;
    } 
    if (histos.nmdtOuter) sc=groupDebug.regHist(histos.nmdtOuter);
    if (sc.isFailure() ) {
      *m_log << MSG::FATAL << "Cannot register nmdtOuter Histograms:" << endreq;
    } 
    if (histos.nmdtCh) sc=groupDebug.regHist(histos.nmdtCh); 
    if (sc.isFailure() ) {
      *m_log << MSG::FATAL << "Cannot register nmdtCh Histograms:" << endreq;
    }
    if (histos.nmdtProf) sc=groupDebug.regHist(histos.nmdtProf);
    if (sc.isFailure() ) {
      *m_log << MSG::FATAL << "Cannot register nmdtProf Histograms:" << endreq;
    } 
    if (histos.nsegm) sc=groupDebug.regHist(histos.nsegm);
    if (sc.isFailure() ) {
      *m_log << MSG::FATAL << "Cannot register nsegm Histograms:" << endreq;
    } 
    if (histos.chi2Prof) sc=groupDebug.regHist(histos.chi2Prof);
    if (sc.isFailure() ) {
      *m_log << MSG::FATAL << "Cannot register chi2Prof Histograms:" << endreq;
    } 

    histos.wasRegistered = true;
  }


  void MuonDQAHitHistTool::registerHist( ManagedMonitorToolBase::MonGroup& groupDebug, ClusterCountHistos& histos )
  {
    StatusCode sc;
    if( histos.wasRegistered ) return;
    if (histos.neta) sc=groupDebug.regHist(histos.neta); 
    if (sc.isFailure() ) {
      *m_log << MSG::FATAL << "Cannot register neta Histograms:" << endreq;
    }
    if (histos.nphi) sc=groupDebug.regHist(histos.nphi); 
    if (sc.isFailure() ) {
      *m_log << MSG::FATAL << "Cannot register nphi Histograms:" << endreq;
    }
    if (histos.netaCh) sc=groupDebug.regHist(histos.netaCh); 
    if (sc.isFailure() ) {
      *m_log << MSG::FATAL << "Cannot register netaCh Histograms:" << endreq;
    }
    if (histos.nphiCh) sc=groupDebug.regHist(histos.nphiCh); 
    if (sc.isFailure() ) {
      *m_log << MSG::FATAL << "Cannot register nphiCh Histograms:" << endreq;
    }
    histos.wasRegistered = true;
  }


  bool MuonDQAHitHistTool::fill( ClusterCountHistos& histos, const ClusterCountData& data ) const {
    if( !histos.wasBooked ) return false;
    if( !histos.wasRegistered ) return false;

    if( histos.neta ) histos.neta->Fill(data.neta);
    if( histos.netaCh ) histos.netaCh->Fill(data.netaCh);
    if( histos.nphi ) histos.nphi->Fill(data.nphi);
    if( histos.nphiCh ) histos.nphiCh->Fill(data.nphiCh);
    return true;
  }

  bool MuonDQAHitHistTool::fill( AllCountHistos& histos, const AllCountData& data ) const {
    if( !histos.wasBooked ) return false;
    if( !histos.wasRegistered ) return false;

    if( histos.nall )   histos.nall->Fill(data.nall);
    if( histos.nallCh ) histos.nallCh->Fill(data.nallCh);
    return true;
  }
  
  bool MuonDQAHitHistTool::fill( MdtCountHistos& histos, const MdtCountData& data ) const {
    if( !histos.wasBooked ) return false;
    if( !histos.wasRegistered ) return false;

    if (data.nmdt>0)
      {
	const char* name_char = data.name.c_str();
	if( histos.nmdt )   histos.nmdt->Fill(data.nmdt);
	if (data.name[1]=='I') {
	  if( histos.nmdtInner ) histos.nmdtInner->Fill(data.nmdt);
	}
	if (data.name[1]=='M') {
	  if( histos.nmdtMiddle ) histos.nmdtMiddle->Fill(data.nmdt);
	}
	if (data.name[1]=='O') {
	  if( histos.nmdtOuter ) histos.nmdtOuter->Fill(data.nmdt);
	}
	if( histos.nmdtCh ) histos.nmdtCh->Fill(data.nmdtCh);
	if( histos.nmdtProf ) histos.nmdtProf->Fill(name_char,data.nmdt);
	if( histos.nsegm ) histos.nsegm->Fill(name_char,1); 
	if( histos.chi2Prof ) histos.chi2Prof->Fill(name_char,data.chi2);
      }

    return true;
  }
  
  bool MuonDQAHitHistTool::fill( HitCountHistos& histos, const HitCountData& data ) const {
    if( !histos.wasBooked ) return false;
    if( !histos.wasRegistered ) return false;

    if( histos.nprec )       histos.nprec->Fill(data.mdts.nmdt + data.cscs.neta );
    if( histos.nprecCh )     histos.nprecCh->Fill(data.mdts.nmdtCh + data.cscs.netaCh );
    if( histos.netaTrig )    histos.netaTrig->Fill(data.rpcs.neta + data.tgcs.neta );
    if( histos.netaTrigCh )  histos.netaTrigCh->Fill(data.rpcs.netaCh + data.tgcs.netaCh );
    if( histos.nphi )        histos.nphi->Fill(data.rpcs.nphi + data.tgcs.nphi + data.cscs.nphi );
    if( histos.nphiCh )      histos.nphiCh->Fill(data.rpcs.nphiCh + data.tgcs.nphiCh + data.cscs.nphiCh );    
    
    fill( histos.all, data.all );
    fill( histos.mdts, data.mdts );
    fill( histos.mdtsBarrelA, data.mdtsBarrelA );
    fill( histos.mdtsBarrelC, data.mdtsBarrelC );
    fill( histos.mdtsECA, data.mdtsECA );
    fill( histos.mdtsECC, data.mdtsECC );

    fill( histos.mdtsBarrelA1, data.mdtsBarrelA1 );
    fill( histos.mdtsBarrelA2, data.mdtsBarrelA2 );
    fill( histos.mdtsBarrelA3, data.mdtsBarrelA3 );
    fill( histos.mdtsBarrelA4, data.mdtsBarrelA4 );
    fill( histos.mdtsBarrelA5, data.mdtsBarrelA5 );
    fill( histos.mdtsBarrelA6, data.mdtsBarrelA6 );
    fill( histos.mdtsBarrelA7, data.mdtsBarrelA7 );
    fill( histos.mdtsBarrelA8, data.mdtsBarrelA8 );

    fill( histos.mdtsBarrelC1, data.mdtsBarrelC1 );
    fill( histos.mdtsBarrelC2, data.mdtsBarrelC2 );
    fill( histos.mdtsBarrelC3, data.mdtsBarrelC3 );
    fill( histos.mdtsBarrelC4, data.mdtsBarrelC4 );
    fill( histos.mdtsBarrelC5, data.mdtsBarrelC5 );
    fill( histos.mdtsBarrelC6, data.mdtsBarrelC6 );
    fill( histos.mdtsBarrelC7, data.mdtsBarrelC7 );
    fill( histos.mdtsBarrelC8, data.mdtsBarrelC8 );

    fill( histos.mdtsECA1, data.mdtsECA1 );
    fill( histos.mdtsECA2, data.mdtsECA2 );
    fill( histos.mdtsECA3, data.mdtsECA3 );
    fill( histos.mdtsECA4, data.mdtsECA4 );
    fill( histos.mdtsECA5, data.mdtsECA5 );
    fill( histos.mdtsECA6, data.mdtsECA6 );
    fill( histos.mdtsECA7, data.mdtsECA7 );
    fill( histos.mdtsECA8, data.mdtsECA8 );

    fill( histos.mdtsECC1, data.mdtsECC1 );
    fill( histos.mdtsECC2, data.mdtsECC2 );
    fill( histos.mdtsECC3, data.mdtsECC3 );
    fill( histos.mdtsECC4, data.mdtsECC4 );
    fill( histos.mdtsECC5, data.mdtsECC5 );
    fill( histos.mdtsECC6, data.mdtsECC6 );
    fill( histos.mdtsECC7, data.mdtsECC7 );
    fill( histos.mdtsECC8, data.mdtsECC8 );

    fill( histos.cscs, data.cscs );
    fill( histos.rpcs, data.rpcs );
    fill( histos.tgcs, data.tgcs );

    return true;
  }

  bool MuonDQAHitHistTool::fill( HitHistos& histos, const HitData& hitData ) const {
    if( !histos.wasBooked ) return false;
    if( !histos.wasRegistered ) return false;

    fill( histos.residualHistos, hitData.residualData );
    fill( histos.countHistos, hitData.countData );
    return true;
  }


  bool MuonDQAHitHistTool::fill( HitResidualHistos& histos, const HitResidualData& data ) const {
    if( !histos.wasBooked ) return false;
    if( !histos.wasRegistered ) return false;

    fill( histos.all, data.all );
    fill( histos.mdts, data.mdts );
    fill( histos.mdtsBarrelA, data.mdtsBarrelA );
    fill( histos.mdtsBarrelC, data.mdtsBarrelC );
    fill( histos.mdtsECA, data.mdtsECA );
    fill( histos.mdtsECC, data.mdtsECC );

    fill( histos.mdtsBarrelA1, data.mdtsBarrelA1 );
    fill( histos.mdtsBarrelA2, data.mdtsBarrelA2 );
    fill( histos.mdtsBarrelA3, data.mdtsBarrelA3 );
    fill( histos.mdtsBarrelA4, data.mdtsBarrelA4 );
    fill( histos.mdtsBarrelA5, data.mdtsBarrelA5 );
    fill( histos.mdtsBarrelA6, data.mdtsBarrelA6 );
    fill( histos.mdtsBarrelA7, data.mdtsBarrelA7 );
    fill( histos.mdtsBarrelA8, data.mdtsBarrelA8 );

    fill( histos.mdtsBarrelC1, data.mdtsBarrelC1 );
    fill( histos.mdtsBarrelC2, data.mdtsBarrelC2 );
    fill( histos.mdtsBarrelC3, data.mdtsBarrelC3 );
    fill( histos.mdtsBarrelC4, data.mdtsBarrelC4 );
    fill( histos.mdtsBarrelC5, data.mdtsBarrelC5 );
    fill( histos.mdtsBarrelC6, data.mdtsBarrelC6 );
    fill( histos.mdtsBarrelC7, data.mdtsBarrelC7 );
    fill( histos.mdtsBarrelC8, data.mdtsBarrelC8 );

    fill( histos.mdtsECA1, data.mdtsECA1 );
    fill( histos.mdtsECA2, data.mdtsECA2 );
    fill( histos.mdtsECA3, data.mdtsECA3 );
    fill( histos.mdtsECA4, data.mdtsECA4 );
    fill( histos.mdtsECA5, data.mdtsECA5 );
    fill( histos.mdtsECA6, data.mdtsECA6 );
    fill( histos.mdtsECA7, data.mdtsECA7 );
    fill( histos.mdtsECA8, data.mdtsECA8 );

    fill( histos.mdtsECC1, data.mdtsECC1 );
    fill( histos.mdtsECC2, data.mdtsECC2 );
    fill( histos.mdtsECC3, data.mdtsECC3 );
    fill( histos.mdtsECC4, data.mdtsECC4 );
    fill( histos.mdtsECC5, data.mdtsECC5 );
    fill( histos.mdtsECC6, data.mdtsECC6 );
    fill( histos.mdtsECC7, data.mdtsECC7 );
    fill( histos.mdtsECC8, data.mdtsECC8 );

    fill( histos.cscsEta, data.cscsEta );
    fill( histos.cscsEta1, data.cscsEta1 );
    fill( histos.cscsEta2, data.cscsEta2 );
    fill( histos.cscsEta3, data.cscsEta3 );
    fill( histos.cscsEta4, data.cscsEta4 );
    fill( histos.cscsEta5, data.cscsEta5 );
    fill( histos.cscsEta6, data.cscsEta6 );
    fill( histos.cscsEta7, data.cscsEta7 );
    fill( histos.cscsEta8, data.cscsEta8 );
    fill( histos.cscsEta9, data.cscsEta9 );
    fill( histos.cscsEta10, data.cscsEta10 );
    fill( histos.cscsEta11, data.cscsEta11 );
    fill( histos.cscsEta12, data.cscsEta12 );
    fill( histos.cscsEta13, data.cscsEta13 );
    fill( histos.cscsEta14, data.cscsEta14 );
    fill( histos.cscsEta15, data.cscsEta15 );
    fill( histos.cscsEta16, data.cscsEta16 );

    fill( histos.cscsPhi, data.cscsPhi );
    fill( histos.cscsPhi1, data.cscsPhi1 );
    fill( histos.cscsPhi2, data.cscsPhi2 );
    fill( histos.cscsPhi3, data.cscsPhi3 );
    fill( histos.cscsPhi4, data.cscsPhi4 );
    fill( histos.cscsPhi5, data.cscsPhi5 );
    fill( histos.cscsPhi6, data.cscsPhi6 );
    fill( histos.cscsPhi7, data.cscsPhi7 );
    fill( histos.cscsPhi8, data.cscsPhi8 );
    fill( histos.cscsPhi9, data.cscsPhi9 );
    fill( histos.cscsPhi10, data.cscsPhi10 );
    fill( histos.cscsPhi11, data.cscsPhi11 );
    fill( histos.cscsPhi12, data.cscsPhi12 );
    fill( histos.cscsPhi13, data.cscsPhi13 );
    fill( histos.cscsPhi14, data.cscsPhi14 );
    fill( histos.cscsPhi15, data.cscsPhi15 );
    fill( histos.cscsPhi16, data.cscsPhi16 ); 

    fill( histos.rpcsEta, data.rpcsEta );
    fill( histos.rpcsEtaA, data.rpcsEtaA );
    fill( histos.rpcsEtaC, data.rpcsEtaC );

    fill( histos.rpcsEtaA1, data.rpcsEtaA1 );
    fill( histos.rpcsEtaA2, data.rpcsEtaA2 );
    fill( histos.rpcsEtaA3, data.rpcsEtaA3 );
    fill( histos.rpcsEtaA4, data.rpcsEtaA4 );
    fill( histos.rpcsEtaA5, data.rpcsEtaA5 );
    fill( histos.rpcsEtaA6, data.rpcsEtaA6 );
    fill( histos.rpcsEtaA7, data.rpcsEtaA7 );
    fill( histos.rpcsEtaA8, data.rpcsEtaA8 );

    fill( histos.rpcsEtaC1, data.rpcsEtaC1 );
    fill( histos.rpcsEtaC2, data.rpcsEtaC2 );
    fill( histos.rpcsEtaC3, data.rpcsEtaC3 );
    fill( histos.rpcsEtaC4, data.rpcsEtaC4 );
    fill( histos.rpcsEtaC5, data.rpcsEtaC5 );
    fill( histos.rpcsEtaC6, data.rpcsEtaC6 );
    fill( histos.rpcsEtaC7, data.rpcsEtaC7 );
    fill( histos.rpcsEtaC8, data.rpcsEtaC8 );

    fill( histos.rpcsPhi, data.rpcsPhi );
    fill( histos.rpcsPhiA, data.rpcsPhiA );
    fill( histos.rpcsPhiC, data.rpcsPhiC );

    fill( histos.rpcsPhiA1, data.rpcsPhiA1 );
    fill( histos.rpcsPhiA2, data.rpcsPhiA2 );
    fill( histos.rpcsPhiA3, data.rpcsPhiA3 );
    fill( histos.rpcsPhiA4, data.rpcsPhiA4 );
    fill( histos.rpcsPhiA5, data.rpcsPhiA5 );
    fill( histos.rpcsPhiA6, data.rpcsPhiA6 );
    fill( histos.rpcsPhiA7, data.rpcsPhiA7 );
    fill( histos.rpcsPhiA8, data.rpcsPhiA8 );

    fill( histos.rpcsPhiC1, data.rpcsPhiC1 );
    fill( histos.rpcsPhiC2, data.rpcsPhiC2 );
    fill( histos.rpcsPhiC3, data.rpcsPhiC3 );
    fill( histos.rpcsPhiC4, data.rpcsPhiC4 );
    fill( histos.rpcsPhiC5, data.rpcsPhiC5 );
    fill( histos.rpcsPhiC6, data.rpcsPhiC6 );
    fill( histos.rpcsPhiC7, data.rpcsPhiC7 );
    fill( histos.rpcsPhiC8, data.rpcsPhiC8 );

    fill( histos.tgcsEta, data.tgcsEta );
    fill( histos.tgcsEtaA, data.tgcsEtaA );
    fill( histos.tgcsEtaC, data.tgcsEtaC );
    fill( histos.tgcsPhi, data.tgcsPhi );
    fill( histos.tgcsPhiA, data.tgcsPhiA );
    fill( histos.tgcsPhiC, data.tgcsPhiC );
    return true;
  }

  bool MuonDQAHitHistTool::fill( DetHistos& histos, const std::vector<DetData>& detDataVec ) const {
    if( !histos.wasBooked ) return false;
    if( !histos.wasRegistered ) return false;

    std::vector<DetData>::const_iterator it = detDataVec.begin();
    std::vector<DetData>::const_iterator it_end = detDataVec.end();
    for(;it!=it_end;++it) fill( histos, *it );
    return true;
  }

  bool MuonDQAHitHistTool::fill( DetHistos& histos, const DetData& detData ) const {
    if( !histos.wasBooked ) return false;
    if( !histos.wasRegistered ) return false;

    const char* name_char = detData.name.c_str();
    if( histos.res )   histos.res->Fill(detData.res);
    if( histos.pull )  histos.pull->Fill(detData.pull);
    if (detData.name[1]=='I') {
      if( histos.resInner ) histos.resInner->Fill(detData.res);
      if( histos.pullInner )  histos.pullInner->Fill(detData.pull);
    }
    if (detData.name[1]=='M') {
      if( histos.resMiddle ) histos.resMiddle->Fill(detData.res);
      if( histos.pullMiddle )  histos.pullMiddle->Fill(detData.pull);
    }
    if (detData.name[1]=='O') {
      if( histos.resOuter ) histos.resOuter->Fill(detData.res);
      if( histos.pullOuter )  histos.pullOuter->Fill(detData.pull);
    }
    if( histos.error ) histos.error->Fill(detData.error); 
    if( histos.lposX ) histos.lposX->Fill(detData.lposX);
    if( histos.lposY ) histos.lposY->Fill(detData.lposY);
    if( histos.resProf ) {
      histos.resProf->Fill(name_char,detData.res);
    }
    if( histos.pullProf ) {
      histos.pullProf->Fill(name_char,detData.pull);
    }
    return true;
  }

}
 
 
