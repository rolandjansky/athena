/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     T2L1TowerCalibTool.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Matthew Tamsett <tamsett@cern.ch>, Carlos Sandoval <csandova@mail.cern.ch>
CREATED:  December 2011


PURPOSE: Calibration tool for L1 towers. 
         


********************************************************************/
#include "TrigT2CaloJet/T2L1TowerCalibTool.h"
#include "TrigCaloEvent/Trig3Momentum.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "AthenaKernel/Timeout.h"

#include "GaudiKernel/IToolSvc.h"

//-----------------------------------------------------------------
//  Constructor
//-----------------------------------------------------------------
T2L1TowerCalibTool::T2L1TowerCalibTool(const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent):
                                       T2CaloJetBaseTool(type, name, parent),
                                       m_grid_element(0)
{  
    declareInterface<T2L1TowerCalibTool>( this );
    // configurables
    declareConstant("Calib_Factors", m_CalibConstants); 
    
  
}

//-----------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------
T2L1TowerCalibTool::~T2L1TowerCalibTool() 
{
}

//-----------------------------------------------------------------
//  Initialize:
//-----------------------------------------------------------------
StatusCode T2L1TowerCalibTool::initialize() 
{
    m_log = new MsgStream(msgSvc(), name());   
    (*m_log) << MSG::INFO << " Initalizing Tool: " << name () << endmsg;
    
    // output
    m_grid_element = new Trig3Momentum();

    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------
//  Execute
//-----------------------------------------------------------------
StatusCode T2L1TowerCalibTool::execute()
{
    return StatusCode::SUCCESS;
}

StatusCode T2L1TowerCalibTool::execute(TrigT2Jet* jet, double /*etamin*/, double /*etamax*/, double /*phimin*/, double /*phimax*/)
{

#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "===== Executing: T2L1TowerCalibTool " << name() << endmsg;
    }
#endif

    // === get the input particles
    
    std::vector<Trig3Momentum>* old_grid = jet->grid();
    std::vector<Trig3Momentum>* new_grid = new std::vector<Trig3Momentum>();
    new_grid->reserve(8000);

#ifndef NDEBUG
   if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG  << "A total of " << old_grid->size()  << " particles are to be calibrated" << endmsg;
    }
#endif
    
    // === Unpack the grid
    std::vector<Trig3Momentum>::const_iterator begin_grid, end_grid; 
    begin_grid = old_grid->begin();
    end_grid = old_grid->end();  
    int jet_counter = 0; 
    for (; begin_grid != end_grid; ++begin_grid){
#ifndef NDEBUG
        if((*m_log).level() <= MSG::VERBOSE){  
            (*m_log) << MSG::VERBOSE << "  Initial Trig3Momentum["  << jet_counter
                                   << "]: Et: "         << begin_grid->e()/cosh(begin_grid->eta())
                                   << " MeV, eta: "     << begin_grid->eta() 
                                   << ", phi: "         << begin_grid->phi() 
                                   << ", sampling: "    << std::hex << begin_grid->caloSample() << std::dec
                                   << ", provenance: "  << begin_grid->provenance() << endmsg;
        }
#endif
        // calibrate the towers
        double transverse_energy_uncorrected = begin_grid->e()/cosh(begin_grid->eta());
        if (begin_grid->provenance()!=9999){
            // If provenance is set then this tower has been split into its EM and HAD components for clustering.
            // Calibration is currently based on the sum of energy, therefore we must recover the sum energy before 
            // obtaining the calibration factor
            double extra_energy = old_grid->at(begin_grid->provenance()).e()/cosh(begin_grid->eta());
            transverse_energy_uncorrected = transverse_energy_uncorrected + extra_energy;
#ifndef NDEBUG
            if((*m_log).level() <= MSG::VERBOSE){  
                (*m_log) << MSG::VERBOSE << "    - EM + HAD ET:  "  << transverse_energy_uncorrected << endmsg;
            }
#endif
        }
        
        int eta_bin = GetEtaBin(begin_grid->eta() );
        int phi_bin = GetPhiBin(begin_grid->phi());
        int et_bin = GetEtBin(transverse_energy_uncorrected);
        double CalibFactor = GetEtaCalibFactor(eta_bin,phi_bin,et_bin); 
        
#ifndef NDEBUG
        if((*m_log).level() <= MSG::VERBOSE){
            (*m_log) << MSG::VERBOSE << "    - Eta Bin:     " << eta_bin << endmsg;
            (*m_log) << MSG::VERBOSE << "    - Phi Bin:     " << phi_bin << endmsg;
            (*m_log) << MSG::VERBOSE << "    - Et Bin:      " << et_bin  << endmsg;
            (*m_log) << MSG::VERBOSE << "    - CalibFactor: " << CalibFactor << endmsg;
        }
#endif
        
        double corrected_transverse_energy = (begin_grid->e()/cosh(begin_grid->eta()))*CalibFactor;
        double corrected_energy = corrected_transverse_energy*cosh(begin_grid->eta() );
          
        m_grid_element->setE(corrected_energy);
        m_grid_element->setEta(begin_grid->eta());
        m_grid_element->setPhi(begin_grid->phi());   
        m_grid_element->setCaloSample(begin_grid->caloSample()); 
        
        new_grid->push_back(*m_grid_element);     
        

#ifndef NDEBUG
        if((*m_log).level() <= MSG::DEBUG){  
            (*m_log) << MSG::DEBUG << "  Calibrated Trig3Momentum["  << jet_counter
                                   << "]: Et: "         << m_grid_element->e()/cosh(m_grid_element->eta())
                                   << " MeV, eta: "     << m_grid_element->eta() 
                                   << ", phi: "         << m_grid_element->phi() 
                                   << ", sampling: "    << std::hex << m_grid_element->caloSample() << std::dec << endmsg;
        }
#endif
        jet_counter++;

    }

#ifndef NDEBUG
   if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG  << "A total of " << new_grid->size()  << " particles were calibrated" << endmsg;
    }
#endif

    jet->setGrid(new_grid);
    delete old_grid;

#ifndef NDEBUG
    if((*m_log).level() <= MSG::DEBUG){
        (*m_log) << MSG::DEBUG << "===== Finished: T2L1TowerCalibTool " << name() << endmsg;
    }
#endif
    
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------
//  Finalize:
//-----------------------------------------------------------------
StatusCode T2L1TowerCalibTool::finalize()
{
  delete m_log;
  delete m_grid_element;
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------
//  Calibration tools:
//-----------------------------------------------------------------
StatusCode T2L1TowerCalibTool::setProperty (const std::string& propname,
                                                        const std::string& value)
{
    StatusCode sc = AlgTool::setProperty (propname, value);
    if (sc.isFailure()) return sc;
    
    sc=CaloRec::ToolWithConstantsMixin::setProperty (propname, value);
    if (sc.isFailure()) return sc;
    
    return StatusCode::SUCCESS;  
}

StatusCode T2L1TowerCalibTool::setProperty (const Property& p)
{
    StatusCode sc=AlgTool::setProperty (p);
    if (sc.isFailure()) return sc;
    
    sc=CaloRec::ToolWithConstantsMixin::setProperty (p);
    if (sc.isFailure()) return sc;
    
    return StatusCode::SUCCESS;
  
}

int T2L1TowerCalibTool::GetEtaBin(double jet_eta)
{
    //45 eta bins
    int ieta=int(((jet_eta*10)+45)/2);
    // if (jet_eta<0) ieta-=1;
    if ( ieta <  0 ) return 0;
    if ( ieta > 43 ) return 44;
    return ieta;
}


int T2L1TowerCalibTool::GetPhiBin(double jet_phi)
{
    //32 eta bins
    int iphi=int(((jet_phi*10)+M_PI*10)/2);
    // if (jet_eta<0) ieta-=1;
    if ( iphi <  0 ) return 0;
    if ( iphi > 31 ) return 31;
    return iphi;
}



int T2L1TowerCalibTool::GetEtBin(double jet_et)
{
    //21 Et bins
    int iet = int(log10(jet_et/1000.)*10-10);
    if ( iet <  0 ) return 0;
    if ( iet > 19 ) return 20;
    return iet;
}


double T2L1TowerCalibTool::GetEtaCalibFactor(int etabin, int /* phibin */, int etbin)
{
    return m_CalibConstants[etabin][etbin];
}


