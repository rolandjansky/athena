/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "Gaudi/Property.h"

#include "egammaEvent/ElectronContainer.h"

//#include "CLHEP/Units/PhysicalConstants.h"
#include <vector>
#include "MissingETPerformance/MissingETScaleTool.h"
#include "MissingETPerformance/Zboson.h"
#include "AthenaKernel/Units.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;
using Athena::Units::GeV;

//------------------------------------------------------------------------------
MissingETScaleTool::MissingETScaleTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
  declareInterface<MissingETScaleTool>( this );
    declareProperty("output_stream",            m_output_stream =           "ZToTwoTausStream");
    declareProperty("tau_container",            m_tau_container =           "ZToTwoTausChosenTauJets" );
    declareProperty("lepton_container",         m_lepton_container =        "ZToTwoTausChosenLepton" );
    declareProperty("met_container",            m_met_container =           "MET_RefFinal" );
    declareProperty("plot_name_base",           m_plot_name_base =          "ScalePlot" );
    //declareProperty("is_cut_tool",              m_is_cut_tool =             false );
    declareProperty("flags_key",                m_flags_key =               "TauJet_flags" );
    declareProperty("weights_key",              m_weights_key =             "TauJet_weights" );
   declareProperty("PassflagZtautau",          m_passflagZtautau     =  "ZToTwoTausPassedInvMassAnalysis");    
   declareProperty("FolderName",           m_folderName="");
}

//------------------------------------------------------------------------------

StatusCode MissingETScaleTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);

  if (sc.isFailure()) {
     msg() << MSG::ERROR
          << "Unable to retrieve pointer to THistSvc"
          << endmsg;
     return sc;
  }

  // define variables for scale plots
  //-------------------------------------------------------------------
  m_nsca     =  20;
  m_binsca   =  0.025;
  m_binscat  =  0.01;
  m_insca    =  0.75;
  m_inscat   =  0.9;
  m_upmass   =  200.;
  char name2[20];
  for (int ih2= 0; ih2< 20 ; ih2++){
    sprintf(name2,"hs2_%d",ih2);
    m_hs2[ih2] = new TH1F(name2, " name2 ", 50, 0.,m_upmass);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "METScale/MET_"+name2, m_hs2[ih2]);
  }
  return StatusCode::SUCCESS;
}//end CBNT_initialize()

StatusCode MissingETScaleTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode MissingETScaleTool::execute(MissingETData *data) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endmsg;

  sc = MissingETScale(data);

  return sc;
}
StatusCode MissingETScaleTool::MissingETScale(MissingETData *data) {

  msg() << MSG::DEBUG <<" -- MissingETScaleTool::execute() -- "<<endmsg;

  bool passed_preconditions = true;
  msg() << MSG::DEBUG << "Sono qui " << endmsg;
  // getting tau container from SG 
  const INavigable4MomentumCollection* tau_cont = 0;

  if( evtStore()->retrieve(tau_cont, m_tau_container).isFailure() || !tau_cont )
    {
      msg() << MSG::DEBUG <<"Failed to retrieve " << m_tau_container <<endmsg;
      return StatusCode::FAILURE;
    }
  if(tau_cont->size() == 0)
    {
      msg() << MSG::DEBUG <<"Event does not have any chosen tau-jets."<<endmsg; 
      passed_preconditions = false;
    }

  // getting lepton container from SG
  const INavigable4MomentumCollection* lepton_cont = 0;
  if( evtStore()->retrieve(lepton_cont, m_lepton_container).isFailure() || !lepton_cont )
    {
      msg() << MSG::DEBUG <<"Failed to retrieve " << m_lepton_container<<endmsg; 
      return StatusCode::FAILURE;
    }
  if(lepton_cont->size() != 1)
    {
      msg() << MSG::DEBUG <<"Event does not have a single chosen lepton. lepton_cont->size() = " << lepton_cont->size()<< endmsg; 
      passed_preconditions = false;
    }

  //ATH_MSG_DEBUG("Has lepton and tau-jet.");
  //double met;
  double metx;
  double mety;
  //met = sqrt(pow(data->refFinal()->etx(),2)+pow(data->refFinal()->ety(),2))/1000.; 
  metx = data->refFinal()->etx();
  mety = data->refFinal()->ety();

/*
    // retrieve flags
    std::vector<int>* flags;
    if ( sc=evtStore()->retrieve(flags, m_flags_key).isFailure() )
    {
        //ATH_MSG_FATAL( "Failed to retrieve " << m_flags_key);
        return StatusCode::FAILURE;
    }

    // retrieve weights
    std::vector<float>* weights;
    if ( sc=evtStore()->retrieve(weights, m_weights_key).isFailure() )
    {
        //ATH_MSG_FATAL( "Failed to retrieve " << m_weights_key );
        return StatusCode::FAILURE;
    }

    if((tau_cont->size() != flags->size())||(tau_cont->size() != weights->size()))
    {
        //ATH_MSG_FATAL("tau_cont, flags, and weights were not all the same size: "
              << tau_cont->size() << " " << flags->size() << " " << weights->size() );
        return StatusCode::FAILURE;
    }
    // get event weight
    float event_weight = 1.;//this->get_event_weight();
*/
  unsigned int num_taus = tau_cont->size();

  if(passed_preconditions)
    {
      // loop over taus
      //ATH_MSG_DEBUG("calculating scaled mass for each tau, num taus = " << num_taus);
      for(unsigned int i=0; i < num_taus; i++)
        { 
	  msg() << MSG::WARNING << "Num taus " <<num_taus<< endmsg;
	  // if((*flags)[i])
	  // {
	  //this->mCurva2((*lepton_cont)[0], (*tau_cont)[i], met, (*weights)[i] * event_weight ); 
	  this->mCurva2((*lepton_cont)[0], (*tau_cont)[i], metx, mety, 1. ); 
	  
	  //(*flags)[i]=400.;
	  //}
        }
    }
  return StatusCode::SUCCESS;
}//end MissingETScale()

StatusCode MissingETScaleTool::finalize()
{
    return StatusCode::SUCCESS;
}

void MissingETScaleTool::mCurva2( const INavigable4Momentum* lep, const INavigable4Momentum* tau, double metx, double mety, float weight )
{
  msg() << MSG::DEBUG <<" -- MissingETScaleTool::mCurva() -- " <<endmsg;
  float riscaptmi;
  float  myTTmasssca;
  double metxr;
  double metyr;

  //fill histos for curva2 in real life --loose cuts--
  msg() << MSG::DEBUG <<"looping over scale increments"<<endmsg;
  for (int ih=0; ih<m_nsca; ih++)
  {
    msg() << MSG::DEBUG <<"ih=" << ih <<endmsg;
    //  msg() << MSG::DEBUG <<"hist: " << m_h_scale_plots.at(ih)->GetName() );

    // rescale etmiss vector

    riscaptmi=m_insca+m_binsca*ih;
    //  MissingET temp_met;
	
    //  temp_met.setEx( met->etx() * riscaptmi );
    metxr = metx * riscaptmi;
    metyr = mety * riscaptmi;
    //  temp_met.setEy( met->ety() * riscaptmi );

  
    // recalculate mass
    myTTmasssca = invTTmass(metxr, metyr,
			    lep->px(),lep->py(),lep->pz(),lep->p(),
			    tau->px(),tau->py(),tau->pz(),tau->p());
		
    // m_mass_tool->calculate( lep, tau, &temp_met ); 
    if (ih ==10) {msg() << MSG::WARNING << "massa" <<myTTmasssca<< endmsg;}			
    // m_h_scale_plots.at(ih)->Fill(myTTmasssca / GeV, weight);
    m_hs2[ih]->Fill(myTTmasssca, weight);
  }
}//end mCurva2()


double MissingETScaleTool::invTTmass(double pxMiss, double pyMiss, 
      double pxLept, double pyLept, double pzLept, double pLept,
      double pxTauJet, double pyTauJet, double pzTauJet, double pTauJet) 
{
//----------------------------------------------------------------
//   calculation of reconstructed invariant tau-tau mass 
//                                           D. Cavalli/S.Resconi
//----------------------------------------------------------------
  
//    double masstau=1776.99;

  double myTTmassd;
  myTTmassd=-100.;

  const double inv_pLept   = 1. / pLept;
  const double rexy        = pyTauJet/pxTauJet;
  const double rex         = pxTauJet/pTauJet;
  const double rcx         = pxLept * inv_pLept;
  const double rcy         = pyLept * inv_pLept;
  const double ta          = pyMiss-rexy*pxMiss;
  const double tb          = rcy-rexy*rcx;
  const double eNu1        = ta/tb;
  const double eNu2        = (pxMiss-rcx*eNu1)/rex;

  double eTau1; 
  double eTau2; 
  double cosTheta;

//       std::cout <<  " eNu1 " <<  eTau1<< "  eTau2 " << eTau2<<
  if(eNu1>0 && eNu2>0) {
    eTau1 = pLept+eNu1;
    eTau2 = pTauJet+eNu2;
    cosTheta = (pxLept*pxTauJet+pyLept*pyTauJet+pzLept*pzTauJet) /
      (pLept*pTauJet);

//       std::cout <<  " eTau1 " <<  eTau1<< "  eTau2 " << eTau2<<
//              " cosTheta " << cosTheta<<  std::endl;
 
    myTTmassd= 2*eTau1*eTau2*(1-cosTheta);
    myTTmassd= sqrt(myTTmassd)/GeV;
  }//end of positive neutrinos

  return myTTmassd;
}
