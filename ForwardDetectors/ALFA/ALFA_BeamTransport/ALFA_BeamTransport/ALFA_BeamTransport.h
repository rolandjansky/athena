/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BeamTransport.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////



#ifndef ALFA_BEAMTRACKBEAMTRANSPORT_H
#define ALFA_BEAMTRACKBEAMTRANSPORT_H

// Gaudi includes
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>


#include "TH1.h"
#include "AtlasHepMC/GenEvent.h"

//FPTracker
#include "FPTracker/Particle.h"
#include "FPTracker/Point.h"

#include "ALFA_BeamTransport/ALFA_BeamTrack.h"
#include "ALFA_BeamTransport/ALFA_FPConfig.h"

#include <iostream>



class AtlasDetectorID;
class Identifier;
class StoreGateSvc;



/** @class ALFA_BeamTransport

      This is the ALFA Beam Transport package.
      It is an interface which uses FPTracker for beamtransportation.
      Getting from the HepMC eventrecord the particle data at the IP 
      it calculates the particle porition at the first RP.
      
      
      @author  Daniel Pelikan <daniel.pelikan@cern.ch>
  */  

class ALFA_BeamTransport : public ::AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       ALFA_BeamTransport(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~ALFA_BeamTransport();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();
       
       /**convert unit MeV to GeV for energy and momenta*/
     void MeVToGeV (HepMC::GenEvent* evt);
     /**convert GeV to MeV for HepMC event record*/
     void GeVToMeV (HepMC::GenEvent* evt);
     /**Addes Vertex with transported particle to HepMC event record*/
     int AddHepMCData(HepMC::GenEvent* evt);
     /**Selects particles for beam transported
     Sets event status code of outgoing particles from generator to status != 1*/
     int SelectParticles(HepMC::GenEvent* evt);
     /**Funktion which calls BeamTrack class to calcualte Position at RPs*/
     int DoBeamTracking(int evt_number);
     /**This function is event selection, tracking and HepMC save ine one function */
     int TransportSelectedParticle(HepMC::GenEvent* evt, int evt_number);
     

    private:
	  //some storegate variables
	  std::string m_key; 
	  std::string m_infokey;
	  
	  	  
	  //Set FPConfiguration variables
	  
	  FPConfig m_FPConfig;
	  
	  
	  //added
	  FPTracker::Particle m_Particle1;
	  FPTracker::Particle m_Particle2;
	  ALFA_BeamTrack m_BeamTracker;
	   //Position at RP
	  FPTracker::Point m_PosRP1;
	  FPTracker::Point m_PosRP3;
	  //Momentum at RP
	  FPTracker::Point m_MomRP1;
	  FPTracker::Point m_MomRP3;
	  
	  double m_EnergyRP1;
	  double m_EnergyRP3;
	  
	  std::string m_FPOutputBeam1;
	  std::string m_FPOutputBeam2;
	  
	  
	  //The two counters have to be set zero at the beginning at each execute run since 
	  //for every new call of the execute() funktion they have to be zero
	  //counter for particles marked as outgoing in event record
	  int m_pcount;
	  //counter for particles marked as incomming in event record
	  int m_pint;

	  double m_EtaCut;
	  double m_XiCut;
	 
	  
	  //debug stuff
	  std::ofstream m_FileBeam1;
	  std::ofstream m_FileBeam2;
	  
	  bool m_WriteDebugOutput;
    }; 


#endif 
