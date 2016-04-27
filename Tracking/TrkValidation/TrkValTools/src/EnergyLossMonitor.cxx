/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLossMonitor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// InDet include
#include "TrkValTools/EnergyLossMonitor.h"
// Gaudi
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"


// constructor
Trk::EnergyLossMonitor::EnergyLossMonitor(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_outputNtuplePath("/NTUPLES/FILE1/EnergyLossMonitor/SingleTrackEnergyLoss"),
  m_outputNtupleDescription("Output of the Trk::EnergyLossMonitor AlgTool"),
  m_currentStep(0)
{
   declareInterface<IEnergyLossMonitor>(this);
   declareProperty("NtuplePath",             m_outputNtuplePath);
   declareProperty("NtupleDescription",      m_outputNtupleDescription);
}

// Athena standard methods
// initialize
StatusCode Trk::EnergyLossMonitor::initialize()
{ 
    // Retrieve the NTuple Service
    StatusCode sc = service( "NTupleSvc", m_ntupleSvc );
    if (sc.isFailure()){
      ATH_MSG_FATAL( "Couldn't get pointer to Ntuple service ! " );
      return StatusCode::FAILURE;
    } else {
    	NTuplePtr ntr(m_ntupleSvc, m_outputNtuplePath);
    if (!ntr)
      ntr = m_ntupleSvc->book(m_outputNtuplePath, CLID_ColumnWiseTuple, m_outputNtupleDescription);
			if (ntr){
				 sc = ntr->addItem("Steps",             m_steps, 0, 5000);
				 // Initial paramters
				 sc = ntr->addItem("InitialP",          m_initialP, 0., 1000000.);
				 sc = ntr->addItem("InitialE",          m_initialE, 0., 1000000.);
				 sc = ntr->addItem("InitialEta",        m_initialEta, -4., 4.);
				 sc = ntr->addItem("InitialPhi",        m_initialPhi, -4., 4.);
				 // kinematics       
				 sc = ntr->addItem("Eta",               m_steps, m_eta);
				 sc = ntr->addItem("Phi",               m_steps, m_phi);
				 // Beam Pipe
				 sc = ntr->addItem("Energy",            m_steps, m_E);
				 sc = ntr->addItem("Momentum",          m_steps, m_p);
				 sc = ntr->addItem("Eta",               m_steps, m_eta);
				 sc = ntr->addItem("Phi",               m_steps, m_phi);
				 sc = ntr->addItem("MaterialHitX",      m_steps, m_hitX);
				 sc = ntr->addItem("MaterialHitY",      m_steps, m_hitY);
				 sc = ntr->addItem("MaterialHitZ",      m_steps, m_hitZ);
				 sc = ntr->addItem("MaterialHitR",      m_steps, m_hitR);      

				} else {
         ATH_MSG_ERROR( "Ntuple booking failed!" );
         return StatusCode::FAILURE;
      }
    }
    ATH_MSG_DEBUG("initialize() successful" );    
    return sc;
}

// finalize
StatusCode Trk::EnergyLossMonitor::finalize()
{
    ATH_MSG_DEBUG("finalize() successful");
    return StatusCode::SUCCESS;
}


void Trk::EnergyLossMonitor::initializeTrack(double p, 
                                             double E,
                                             double eta,
                                             double phi) const
{
    m_initialP   = p;
    m_initialE   = E;
    m_initialEta = eta;
    m_initialPhi = phi;
    m_currentStep = 0;
}

void Trk::EnergyLossMonitor::recordTrackState(const Amg::Vector3D& pos, 
                                              const Amg::Vector3D& mom,
                                              double mass) const
{

  // Geant4 first step protection 
  if (pos.mag() > 0.01 && m_currentStep < 5000){ 
    double p                = mom.mag();
    m_E[m_currentStep]      = sqrt(p*p+mass*mass);
    m_p[m_currentStep]      = p;
    m_eta[m_currentStep]    = mom.eta();
    m_phi[m_currentStep]    = mom.phi();
    m_hitX[m_currentStep]   = pos.x();
    m_hitY[m_currentStep]   = pos.y();
    m_hitZ[m_currentStep]   = pos.z();
    m_hitR[m_currentStep]   = pos.perp();
    m_currentStep++;
  }
}

void Trk::EnergyLossMonitor::finalizeTrack() const
{
   m_steps = m_currentStep;
   StatusCode sc = m_ntupleSvc->writeRecord(m_outputNtuplePath);
   if (sc.isFailure()){
     ATH_MSG_WARNING( "Couldn't write ntuple record!" );
   }
}
