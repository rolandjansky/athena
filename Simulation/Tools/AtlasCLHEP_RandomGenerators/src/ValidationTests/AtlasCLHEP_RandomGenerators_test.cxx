/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////

#include "AtlasCLHEP_RandomGenerators_test.h"
#include "CLHEP/Random/Ranlux64Engine.h"
#include "CLHEP/Random/RanecuEngine.h"
#include "CLHEP/Random/MTwistEngine.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandGaussQ.h"
#include "CLHEP/Random/RandExponential.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandExpZiggurat.h"

#include "AtlasCLHEP_RandomGenerators/RandBinomialFixedP.h"

// Gaudi includes
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/Chrono.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ListItem.h"

#include <string>

//#include "rnorrexp_org.icc"

namespace AtlasCLHEP_RandomGenerators {

  AtlasCLHEP_RandomGenerators_test::AtlasCLHEP_RandomGenerators_test(const std::string& name, ISvcLocator* pSvcLocator):
            AthAlgorithm(name,pSvcLocator),
            m_chrono(0),
            m_ranlux64(nullptr),
            m_ranecu(nullptr),
            m_mtwist(nullptr),
            m_histSvc(nullptr),
            m_rndmSvc1("AtRanluxGenSvc", name),
            m_randomEngine1(0),
            m_randomEngineName1("rnd_AtRanluxGenSvc"),
            m_rndmSvc2("AtDSFMTGenSvc", name),
            m_randomEngine2(0),
            m_randomEngineName2("rnd_AtDSFMTGenSvc")
  {
    declareProperty("ntest", m_ntest);  
  }
  
  //__________________________________________________________________________
  AtlasCLHEP_RandomGenerators_test::~AtlasCLHEP_RandomGenerators_test(){
  }

  //__________________________________________________________________________
  StatusCode AtlasCLHEP_RandomGenerators_test::initialize()
  {
    if(!serviceLocator()->service("ChronoStatSvc" , m_chrono ).isSuccess()){
      ATH_MSG_FATAL( "Cannot retrieve ChronoStatSvc ");
      return StatusCode::FAILURE;
    }
    
    m_ranlux64=new CLHEP::Ranlux64Engine();
    m_ranecu=new CLHEP::RanecuEngine();
    m_mtwist=new CLHEP::MTwistEngine();
    
    // Random number service
    if ( m_rndmSvc1.retrieve().isFailure() ) {
      msg(MSG::ERROR)<< "Could not retrieve " << m_rndmSvc1 << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_rndmSvc2.retrieve().isFailure() ) {
      msg(MSG::ERROR)<< "Could not retrieve " << m_rndmSvc2 << endmsg;
      return StatusCode::FAILURE;
    }
      
    //Get own engine with own seeds:
    m_randomEngine1 = m_rndmSvc1->GetEngine(m_randomEngineName1);
    if (!m_randomEngine1) {
      msg(MSG::ERROR)<< "Could not get random engine '" << m_randomEngineName1 << "'" << endmsg;
      return StatusCode::FAILURE;
    }
    m_randomEngine2 = m_rndmSvc2->GetEngine(m_randomEngineName2);
    if (!m_randomEngine2) {
      msg(MSG::ERROR)<< "Could not get random engine '" << m_randomEngineName2 << "'" << endmsg;
      return StatusCode::FAILURE;
    }
    
    if((service("THistSvc", m_histSvc)).isFailure()) {
      msg(MSG::ERROR) << "Cannot allocate THistSvc service" << endmsg;
      return StatusCode::FAILURE;
    }
    
    m_hflat=new TH1D("RandFlat","RandFlat",1000,0,1);
    if(!m_histSvc->regHist(std::string("/PLOTS/") + m_hflat->GetName(), m_hflat).isSuccess()) {
      msg(MSG::WARNING) << "Could not register histogram "<< m_hflat->GetName() << endmsg;
    }
    
    m_hgauss1=new TH1D("RandGauss","RandGauss",700,-7,7);
    if(!m_histSvc->regHist(std::string("/PLOTS/") + m_hgauss1->GetName(), m_hgauss1).isSuccess()) {
      msg(MSG::WARNING) << "Could not register histogram "<< m_hgauss1->GetName() << endmsg;
    }
    
    m_hgauss2=new TH1D("RandGaussQ","RandGaussQ",700,-7,7);
    if(!m_histSvc->regHist(std::string("/PLOTS/") + m_hgauss2->GetName(), m_hgauss2).isSuccess()) {
      msg(MSG::WARNING) << "Could not register histogram "<< m_hgauss2->GetName() << endmsg;
    }
    
    m_hgauss3=new TH1D("RandGaussZigurat","RandGaussZigurat",700,-7,7);
    if(!m_histSvc->regHist(std::string("/PLOTS/") + m_hgauss3->GetName(), m_hgauss3).isSuccess()) {
      msg(MSG::WARNING) << "Could not register histogram "<< m_hgauss3->GetName() << endmsg;
    }
    
    m_hexp1=new TH1D("RandExponential","RandExponential",900,0,30);
    if(!m_histSvc->regHist(std::string("/PLOTS/") + m_hexp1->GetName(), m_hexp1).isSuccess()) {
      msg(MSG::WARNING) << "Could not register histogram "<< m_hexp1->GetName() << endmsg;
    }
    
    m_hexp2=new TH1D("RandExpZiggurat","RandExpZiggurat",900,0,30);
    if(!m_histSvc->regHist(std::string("/PLOTS/") + m_hexp2->GetName(), m_hexp2).isSuccess()) {
      msg(MSG::WARNING) << "Could not register histogram "<< m_hexp2->GetName() << endmsg;
    }

    m_hbin1=new TH1D("RandBinomial","RandBinomial",4,-0.5,3.5);
    if(!m_histSvc->regHist(std::string("/PLOTS/") + m_hbin1->GetName(), m_hbin1).isSuccess()) {
      msg(MSG::WARNING) << "Could not register histogram "<< m_hbin1->GetName() << endmsg;
    }

    m_hbin2=new TH1D("RandBinomialFixedP","RandBinomialFixedP",4,-0.5,3.5);
    if(!m_histSvc->regHist(std::string("/PLOTS/") + m_hbin2->GetName(), m_hbin2).isSuccess()) {
      msg(MSG::WARNING) << "Could not register histogram "<< m_hbin2->GetName() << endmsg;
    }
    

    return StatusCode::SUCCESS; 
  }
  
  StatusCode AtlasCLHEP_RandomGenerators_test::finalize()
  {
    
    return StatusCode::SUCCESS; 
  }
  
  //_________________________________________________________________________
  StatusCode AtlasCLHEP_RandomGenerators_test::execute()
  {
    static int ievent=0;
    int ntest=m_ntest;
    
    msg(MSG::DEBUG)<<"event="<<ievent<<" ntest="<<ntest<<endmsg;
    
    m_chrono -> chronoStart("flat");
    double sum_flat=0;
    for(int i=0;i<ntest;++i) {
      double g=m_randomEngine2->flat();
      m_hflat->Fill(g);
      sum_flat+=g;
    }  
    sum_flat/=ntest;
    m_chrono -> chronoStop("flat");
    msg(MSG::DEBUG)<<" avg "<<"flat"<<"="<<sum_flat<<endmsg;

    m_chrono -> chronoStart("RandGauss");
    double sum_rnd1=0;
    for(int i=0;i<ntest;++i) {
      double g=CLHEP::RandGauss::shoot(m_randomEngine2);
      m_hgauss1->Fill(g);
      sum_rnd1+=g;
    }  
    sum_rnd1/=ntest;
    m_chrono -> chronoStop("RandGauss");
    msg(MSG::DEBUG)<<" avg "<<"RandGauss"<<"="<<sum_rnd1<<endmsg;

    m_chrono -> chronoStart("RandGaussQ");
    double sum_rnd2=0;
    for(int i=0;i<ntest;++i) {
      double g=CLHEP::RandGaussQ::shoot(m_randomEngine2);
      m_hgauss2->Fill(g);
      sum_rnd2+=g;
    }  
    sum_rnd2/=ntest;
    m_chrono -> chronoStop("RandGaussQ");
    msg(MSG::DEBUG)<<" avg "<<"RandGaussQ"<<"="<<sum_rnd2<<endmsg;

    m_chrono -> chronoStart("RandGaussZiggurat");
    double sum_zig=0;
    for(int i=0;i<ntest;++i) {
      double g=CLHEP::RandGaussZiggurat::shoot(m_randomEngine2);
      m_hgauss3->Fill(g);
      sum_zig+=g;
    }  
    sum_zig/=ntest;
    m_chrono -> chronoStop("RandGaussZiggurat");
    msg(MSG::DEBUG)<<" avg "<<"RandGaussZiggurat"<<"="<<sum_zig<<endmsg;

    m_chrono -> chronoStart("RandExponential");
    double sum_exp=0;
    for(int i=0;i<ntest;++i) {
      double g=CLHEP::RandExponential::shoot(m_randomEngine2);
      m_hexp1->Fill(g);
      sum_exp+=g;
    }  
    sum_exp/=ntest;
    m_chrono -> chronoStop("RandExponential");
    msg(MSG::DEBUG)<<" avg "<<"RandExponential"<<"="<<sum_exp<<endmsg;

    m_chrono -> chronoStart("RandExpZiggurat");
    double sum_expZ=0;
    for(int i=0;i<ntest;++i) {
      double g=CLHEP::RandExpZiggurat::shoot(m_randomEngine2);
      m_hexp2->Fill(g);
      sum_expZ+=g;
    }  
    sum_expZ/=ntest;
    m_chrono -> chronoStop("RandExpZiggurat");
    msg(MSG::DEBUG)<<" avg "<<"RandExpZiggurat"<<"="<<sum_expZ<<endmsg;

    CLHEP::RandBinomialFixedP lu(*m_randomEngine2,1,0.4,12); // Needs to be called with the random engine as reference, as a pointer gets deleted by CLHEP::RandBinomial!

    for(int n=1;n<6;n+=1) {
      double sum_bin=0;
      m_chrono -> chronoStart("RandBinomial_"+std::to_string(n));
      for(int i=0;i<ntest;++i) {
        double g=CLHEP::RandBinomial::shoot(m_randomEngine2,n,0.4);
        if(n==3) m_hbin1->Fill(g);
        sum_bin+=g;
      }  
      m_chrono -> chronoStop("RandBinomial_"+std::to_string(n));
      sum_bin/=ntest;
      msg(MSG::DEBUG) << " avg RandBinomial("<<n<<")="<<sum_bin<< endmsg;

      double sum_stdbin=0;
      m_chrono -> chronoStart("RBinomialFP_"+std::to_string(n));
      for(int i=0;i<ntest;++i) {
        double g=lu.fire(m_randomEngine2, n );
        if(n==3) m_hbin2->Fill(g);
        sum_stdbin+=g;
      }  
      m_chrono -> chronoStop("RBinomialFP_"+std::to_string(n));
      sum_stdbin/=ntest;
      msg(MSG::DEBUG) << " avg RandBinomialFixedP("<<n<<")="<<sum_stdbin<< endmsg;
    }  


    /*

    m_chrono -> chronoStart(m_randomEngineName1);
    double sum_rnd1=0;
    for(int i=0;i<ntest;++i) {
      sum_rnd1+=m_randomEngine1->flat();
    }  
    sum_rnd1/=ntest;
    m_chrono -> chronoStop(m_randomEngineName1);
    msg(MSG::DEBUG)<<" avg "<<m_randomEngineName1<<"="<<sum_rnd1<<endmsg;
    
    m_chrono -> chronoStart(m_randomEngineName2);
    double sum_rnd2=0;
    for(int i=0;i<ntest;++i) {
      sum_rnd2+=m_randomEngine2->flat();
    }  
    sum_rnd2/=ntest;
    m_chrono -> chronoStop(m_randomEngineName2);
    msg(MSG::DEBUG)<<" avg "<<m_randomEngineName2<<"="<<sum_rnd2<<endmsg;
    
    m_chrono -> chronoStart("ranlux64");
    m_ranlux64->setSeed(seed,1);
    double sum_ranlux64=0;
    for(int i=0;i<ntest;++i) {
      sum_ranlux64+=m_ranlux64->flat();
    }  
    sum_ranlux64/=ntest;
    m_chrono -> chronoStop("ranlux64");
    msg(MSG::DEBUG)<<" avg ranlux64="<<sum_ranlux64<<endmsg;
    
    m_chrono -> chronoStart("ranecu");
    m_ranecu->setSeed(seed,1);
    double sum_ranecu=0;
    for(int i=0;i<ntest;++i) {
      sum_ranecu+=m_ranecu->flat();
    }  
    sum_ranecu/=ntest;
    m_chrono -> chronoStop("ranecu");
    msg(MSG::DEBUG)<<" avg ranecu="<<sum_ranecu<<endmsg;
    
    m_chrono -> chronoStart("mtwist");
    m_mtwist->setSeed(seed,1);
    double sum_mtwist=0;
    for(int i=0;i<ntest;++i) {
      sum_mtwist+=m_mtwist->flat();
    }  
    sum_mtwist/=ntest;
    m_chrono -> chronoStop("mtwist");
    msg(MSG::DEBUG)<<" avg mtwist="<<sum_mtwist<<endmsg;
    
    */
    
    ++ievent;

    return StatusCode::SUCCESS;
  }

} // end of namespace bracket



