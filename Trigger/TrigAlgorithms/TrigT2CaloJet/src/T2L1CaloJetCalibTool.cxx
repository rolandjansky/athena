/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     T2L1CaloJetCalibTool.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Matthew Tamsett <tamsett@cern.ch>
CREATED:  June 2012


PURPOSE: Calibration tool for L2FS jets. 
         


********************************************************************/
#include "TrigT2CaloJet/T2L1CaloJetCalibTool.h"
#include "TrigCaloEvent/Trig3Momentum.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "AthenaKernel/Timeout.h"

#include "GaudiKernel/IToolSvc.h"

//-----------------------------------------------------------------
//  Constructor
//-----------------------------------------------------------------
T2L1CaloJetCalibTool::T2L1CaloJetCalibTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent):
                                           T2CaloJetBaseTool(type, name, parent)
{  
    declareInterface<T2L1CaloJetCalibTool>( this );
    // configurables
    declareConstant("Calib_Factors", m_CalibConstants); 
    //declareProperty("Et_bin_edges",  m_et_bin_lower_edges); 
    declareProperty("Eta_bin_edges", m_eta_bin_lower_edges); 
    //declareConstant("Phi_bin_edges", m_phi_bin_lower_edges); 
    
  
}

//-----------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------
T2L1CaloJetCalibTool::~T2L1CaloJetCalibTool() 
{
}

//-----------------------------------------------------------------
//  Initialize:
//-----------------------------------------------------------------
StatusCode T2L1CaloJetCalibTool::initialize() 
{
    m_log = new MsgStream(msgSvc(), name());   
    (*m_log) << MSG::INFO << " Initalizing Tool: " << name () << endmsg;
    
    if ( m_eta_bin_lower_edges.empty() ) { 
        (*m_log) << MSG::ERROR << name() << ": No eta bin edges provided " << endmsg;
        return StatusCode::FAILURE;        
    } else { 
        (*m_log) << MSG::INFO << " eta bin edges provided: " << m_eta_bin_lower_edges.size() << endmsg;
        if((*m_log).level() <= MSG::DEBUG){
            for  ( unsigned i=0 ; i<m_eta_bin_lower_edges.size() ; i++ ) {
                (*m_log) << MSG::DEBUG   << "    - bin[" << i << "] lower edge: " << m_eta_bin_lower_edges[i] << endmsg;
            }
        }
    }
        

    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------
//  Execute
//-----------------------------------------------------------------
StatusCode T2L1CaloJetCalibTool::execute()
{
    return StatusCode::SUCCESS;
}

StatusCode T2L1CaloJetCalibTool::execute(TrigT2Jet* jet, double /*etamin*/, double /*etamax*/, double /*phimin*/, double /*phimax*/)
{

#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "===== Executing: T2L1CaloJetCalibTool " << name() << endmsg;
    }
#endif

    // === get the input jet
    
    
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){  
        (*m_log) << MSG::DEBUG   << "  Initial TrigT2CaloJet Et: "  << jet->e()/cosh(jet->eta())
                                 << " MeV, eta: "     << jet->eta() 
                                 << ", phi: "         << jet->phi() 
                                 << endmsg;
        (*m_log) << MSG::DEBUG   << "    - EM Et: "    << jet->eem0()/cosh(jet->eta())
                                 << " MeV, Had Et: "   << jet->ehad0()/cosh(jet->eta())
                                 << " MeV, emf: "      << jet->emf() 
                                 << endmsg;
    }
#endif
    // calibrate the jet
    int eta_bin = GetEtaBin(jet->eta());
    double transverse_energy = jet->e()/cosh(jet->eta());
    
    double CalibFactor = GetCalibFactor(eta_bin,transverse_energy/1000.); 
    double correction_factor = 1. / (1. + CalibFactor);
    double em_correction_factor  = correction_factor;
    double had_correction_factor = correction_factor;
    
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "    - Eta Bin:                   " << eta_bin << endmsg;
        (*m_log) << MSG::DEBUG << "    - CalibFactor:               " << CalibFactor << endmsg;
        (*m_log) << MSG::DEBUG << "    - Multiplicative correction: " << correction_factor << endmsg;
        (*m_log) << MSG::DEBUG << "    - em correction: "             << em_correction_factor << endmsg;
        (*m_log) << MSG::DEBUG << "    - had correction: "            << had_correction_factor << endmsg;
        
    }
#endif
    
    double corrected_transverse_energy = (jet->e()/cosh(jet->eta()))*correction_factor;
    double corrected_energy = corrected_transverse_energy*cosh(jet->eta() );      
    jet->setE(corrected_energy);
    
    double corrected_em_transverse_energy = (jet->eem0()/cosh(jet->eta()))*em_correction_factor;
    double corrected_em_energy = corrected_em_transverse_energy*cosh(jet->eta() );    
    jet->setEem0(corrected_em_energy);
    
    double corrected_had_transverse_energy = (jet->ehad0()/cosh(jet->eta()))*had_correction_factor;
    double corrected_had_energy = corrected_had_transverse_energy*cosh(jet->eta() );    
    jet->setEhad0(corrected_had_energy);
    
    double emf = 0;
    if (corrected_em_transverse_energy){
        emf = jet->eem0() / jet->e();
    }
    jet->setEmf(emf);
    
#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){  
        (*m_log) << MSG::DEBUG << "  Calibrated TrigT2CaloJet: Et: "         << jet->e()/cosh(jet->eta())
                               << " MeV, eta: "     << jet->eta() 
                               << ", phi: "         << jet->phi()
                               << endmsg;
        (*m_log) << MSG::DEBUG   << "    - EM Et: "    << jet->eem0()/cosh(jet->eta())
                                 << " MeV, Had Et: "   << jet->ehad0()/cosh(jet->eta())
                                 << " MeV, emf: "      << jet->emf() 
                                 << endmsg;
    }
#endif
   

#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "===== Finished: T2L1CaloJetCalibTool " << name() << endmsg;
    }
#endif
    
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------
//  Finalize:
//-----------------------------------------------------------------
StatusCode T2L1CaloJetCalibTool::finalize()
{
  delete m_log;
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------
//  Calibration tools:
//-----------------------------------------------------------------
StatusCode T2L1CaloJetCalibTool::setProperty (const std::string& propname,
                                                        const std::string& value)
{
    StatusCode sc = AlgTool::setProperty (propname, value);
    if (sc.isFailure()) return sc;
    
    sc=CaloRec::ToolWithConstantsMixin::setProperty (propname, value);
    if (sc.isFailure()) return sc;
    
    return StatusCode::SUCCESS;  
}

StatusCode T2L1CaloJetCalibTool::setProperty (const Property& p)
{
    StatusCode sc=AlgTool::setProperty (p);
    if (sc.isFailure()) return sc;
    
    sc=CaloRec::ToolWithConstantsMixin::setProperty (p);
    if (sc.isFailure()) return sc;
    
    return StatusCode::SUCCESS;
  
}

int T2L1CaloJetCalibTool::GetEtaBin(double jet_eta)
{
    for  ( unsigned i=0 ; i<m_eta_bin_lower_edges.size(); i++ ) {
        if (jet_eta < m_eta_bin_lower_edges[i]){
#ifndef NDEBUG
            if((*m_log).level() <= MSG::DEBUG){  
               (*m_log) << MSG::DEBUG << "      eta: "  << jet_eta
                               << " < bin edge["   << i 
                               << "]: "            << m_eta_bin_lower_edges[i]
                               << endmsg;
            }
#endif
            return i-1;
        }
    }
    if (jet_eta < 0)  return 0; 
    return m_eta_bin_lower_edges.size()-1;
}


double T2L1CaloJetCalibTool::GetCalibFactor(int etabin, double transverse_energy)
{
    //up to 7 parameter for the pol-fit
    unsigned Npar=7;
    double logET=1./log10(transverse_energy);
    double y=0;
    for (unsigned i=0;i<Npar;++i)
        y += m_CalibConstants[etabin][i]*pow(logET,i);
    return y;
}


