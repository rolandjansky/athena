/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "../ALFA_BeamTransport/ALFA_BeamTrack.h"
//#include "../HepMCAnalysis_i/FileWrite.h"

//Athena Stuff
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthAlgorithm.h"

//FPTracker stuff
#include "FPTracker/setupBeamline.h"
#include "FPTracker/Beamline.h"
#include "FPTracker/Particle.h"
#include "FPTracker/getConfigFile.h"
#include "FPTracker/getMagnetConfigFiles.h"
#include "FPTracker/getAlfaMagnetConfigFiles.h"
#include "FPTracker/ConfigData.h"
//#include "FPTracker/readConfigData.h"
#include "FPTracker/FPTrackerConstants.h"
#include "FPTracker/Point.h"
//Boost
#include "boost/program_options/cmdline.hpp"
#include "boost/program_options/options_description.hpp"
#include "boost/program_options/parsers.hpp"
#include "boost/program_options/variables_map.hpp"



#include <iostream>

//constructor
ALFA_BeamTrack::ALFA_BeamTrack()
{
  
	//initialisation with standart values

	//declareProperty("ConfDir", m_ConfDir=  "./config");
	/*m_ConfDir= ...
  	m_AlfaTwiss = ...*/

	m_AlfaTwiss=true;
	m_ip     = 1;
        m_Magver = 3;
        m_Side = FPTracker::beam1;

  
}

//destructor
ALFA_BeamTrack::~ALFA_BeamTrack()
{
}

int ALFA_BeamTrack::initialize(const FPConfig &ConfigValues){
     
        m_ConfDir=  ConfigValues.ConfDir;
        m_AlfaTwiss = ConfigValues.UseALFA;
	
     
  	//initialisation of beampipes
  	if (m_AlfaTwiss) {std::cout<<"Will use alfa twiss files"<<std::endl;}
  
	
	/*
	
  	boost::shared_ptr< std::ifstream > p_confFile;
    	p_confFile =  FPTracker::getConfigFile(m_ConfDir, "FPData.txt") ;
    	m_ConfigData = FPTracker::readConfigData(p_confFile);
	
	//SetPosition of RP in meter
	
	//m_ConfigData.setPlane(ConfigValues.RPDistance);
	
	
	ConfigData configData;
	if( !readConfigData(p_confFile, configData) )
	{
		std::cout<<"fatal error reading config data\n";
		return  1;
	}
	
	
	*/
	
	//Set Configuration parameters of FPTracker library
	FPTracker::ConfigData ConfigData;
	
	
	ConfigData.IP=ConfigValues.IP;
	ConfigData.useaper=ConfigValues.useaper;
	ConfigData.apermb=ConfigValues.apermb;
	ConfigData.xcol1=ConfigValues.xcol1;
	ConfigData.xcol2=ConfigValues.xcol2;
	ConfigData.absZMagMax=ConfigValues.absZMagMax;
	ConfigData.endMarker=ConfigValues.RPDistance;
	ConfigData.setpbeam(ConfigValues.pbeam0);
	
	/*
	std::cout<<ConfigData.IP<<std::endl;
	std::cout<<ConfigData.useaper<<std::endl;
	std::cout<<ConfigData.apermb<<std::endl;
	std::cout<<ConfigData.xcol1<<std::endl;
	std::cout<<ConfigData.xcol2<<std::endl;
	std::cout<<ConfigData.absZMagMax<<std::endl;
	std::cout<<ConfigData.endMarker<<std::endl;
	std::cout<<ConfigData.pbeam0<<std::endl;
	std::cout<<ConfigData.brho<<std::endl;*/
	
	
	
	
	
	
	
	
    
	m_ip     = ConfigData.IP;
     	m_Magver = 3;
     
     	m_Side = FPTracker::beam1;
     	m_Magfile =  m_AlfaTwiss ? 
    	FPTracker::getAlfaMagnetConfigFiles(m_ConfDir, m_Side):
			FPTracker::getMagnetConfigFiles(m_ConfDir, m_ip, m_Magver, m_Side);
       	m_Beamline_1 = FPTracker::setupBeamline(ConfigData, m_Side, m_Magver, m_Magfile);
    	std::cout<<" beamline_1 start  "<<'\n';
	std::cout<<m_Beamline_1;
	std::cout<<" beamline_1 end  "<<"\n\n";
	
	m_Side = FPTracker::beam2;
	m_Magfile =  m_AlfaTwiss ? 
    	FPTracker::getAlfaMagnetConfigFiles(m_ConfDir, m_Side):
			FPTracker::getMagnetConfigFiles(m_ConfDir, m_ip, m_Magver, m_Side);
	m_Beamline_2 = setupBeamline(ConfigData, m_Side, m_Magver, m_Magfile);
  
	std::cout<<" beamline_2 start  "<<'\n';
	std::cout<<m_Beamline_2;
	std::cout<<" beamline_2 end  "<<"\n\n";
	
	return true;  
}
  
//calculates Position and Momentum at RP
int ALFA_BeamTrack::CalculatePosRP(FPTracker::Particle particle){
  
	FPTracker::Particle Particle=particle;
	FPTracker::Point lost_markerMom(-99,-99,-99); 
	

	std::cout<<"fpTracker: "<<Particle<<std::endl;
	Particle.direction().z()>0. ? m_Beamline_1.track(Particle):m_Beamline_2.track(Particle);
	std::cout<<"fpTracker: "<<Particle<<std::endl;

	//m_PosParticleRP=Particle.position(); //old version vith 97mm shift
	m_PosParticleRP=Particle.positionRelBeam();//version without shift

	FPTracker::Point momentum( Particle.direction().x()*Particle.momentum(),
			 Particle.direction().y()*Particle.momentum(),
			 Particle.direction().z()*Particle.momentum());

	m_MomParticleRP=momentum;

	
	if(Particle.isOutOfAperture())
	{
		//FPTracker::Point lost_markerPos(-99,-99,Particle.position().z());// old version with shift
		FPTracker::Point lost_markerPos(-99,-99,Particle.positionRelBeam().z());
		std::cout<<"Particle got lost"<<std::endl;
		//m_PosParticleRP=lost_marker;
		//write lost particle with -99 as maker in x and yposition z position ist the place where particle got lost
		m_PosParticleRP=lost_markerPos;
		
		m_MomParticleRP=lost_markerMom;		
	}


	return true;
}
  
//returns particle position
FPTracker::Point ALFA_BeamTrack::PosRP(){
	return m_PosParticleRP;
}
  
// returns momentum of particle
FPTracker::Point ALFA_BeamTrack::MomRP(){
	return m_MomParticleRP;
}

  
