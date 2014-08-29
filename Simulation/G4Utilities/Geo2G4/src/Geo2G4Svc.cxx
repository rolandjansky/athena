/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4/Geo2G4Svc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "StoreGate/StoreGateSvc.h"

#include "Geo2G4/Geo2G4SvcAccessor.h"
#include "Geo2G4/GenericVolumeBuilder.h"

void InitializeBuilders();

Geo2G4Svc::Geo2G4Svc(const std::string& n, ISvcLocator* svc):
  Service(n,svc), log(msgSvc(),name()),m_pIncSvc(0),defaultBuilder(0),m_getTopTransform(true)
{
	log<<MSG::INFO<<"Creating the Geo2G4Svc "<<endreq;
	declareProperty("GetTopTransform", m_getTopTransform);
}
Geo2G4Svc::~Geo2G4Svc()
{;}

StatusCode Geo2G4Svc::initialize()
{
	static int initialized=0;
	if (initialized)
		log<<MSG::INFO<<" Geo2G4Svc already initialized "<<endreq;
	else
	{
		log<<MSG::INFO<<"Initializing the Geo2G4Svc "<<endreq;
		log<<MSG::INFO<<"Creating all builders available "<<endreq;
		InitializeBuilders();

		std::string nameBuilder = "Extended_Parameterised_Volume_Builder";
		SetDefaultBuilder(nameBuilder);
		log<<MSG::INFO<<nameBuilder << " --> set as default builder" <<endreq;
		log<<MSG::INFO<<nameBuilder << " --> ParamOn flag = " << defaultBuilder->GetParam() << endreq;
		initialized=1;
  ListVolumeBuilders();
	}

	return StatusCode::SUCCESS;
}

StatusCode Geo2G4Svc::finalize()
{
	log<<MSG::INFO<<"Finalizing the Geo2G4Svc "<<endreq;

	return StatusCode::SUCCESS;
}

StatusCode Geo2G4Svc::queryInterface(const InterfaceID& riid, void**ppvInt)
{
	StatusCode sc=StatusCode::FAILURE;
	const InterfaceID& iid=intID();
	if (riid==iid)
	{
		*ppvInt=dynamic_cast<IGeo2G4Svc*>(this);
		sc=StatusCode::SUCCESS;
	}
	else
	{
		sc=Service::queryInterface(riid,ppvInt);
	}
	return sc;
}

void Geo2G4Svc::handle(const Incident& )
{
}

void Geo2G4Svc::RegisterVolumeBuilder(VolumeBuilder* vb)
{
	std::string key=vb->GetKey();
	if (m_builders.find(key)!=m_builders.end())
	{
		log<<MSG::INFO<<"Trying to set an already existing "
		   <<"builder "<<key<<endreq;
		log<<MSG::INFO<<"\t request ignored, nothing done "<<endreq;
	}
	else
	{
		m_builders[key]=vb;
		log<<MSG::INFO<<"Volume builder registered "<<key<<endreq;
	}
}
void Geo2G4Svc::ListVolumeBuilders()
{
	BuilderMap::const_iterator it;
	std::cout<<"---- List of all Volume Builders registered with Geo2G4Svc ----"<<std::endl;
	std::cout<<"---------------------------------------------------------------"<<std::endl;
	for (it=m_builders.begin();it!=m_builders.end();it++)
		std::cout<<" Volume Builder  "<<(*it).second->GetKey()<<std::endl;
	std::cout<<"---------------------------------------------------------------"<<std::endl;
	std::cout<<" default builder is "<<defaultBuilder->GetKey()<<std::endl;
}
void Geo2G4Svc::UnregisterVolumeBuilder(VolumeBuilder* vb)
{
	std::string key=vb->GetKey();
	if (m_builders.find(key)!=m_builders.end())
	{
		log<<MSG::INFO<<"Removing builder "<<key<<" from the list"<<endreq;
		m_builders.erase(key);
	}
	else
	{
		log<<MSG::ERROR<<"Trying to remove a not-existing builder "<<key<<endreq;
		log<<MSG::ERROR<<"\t request ignored, nothing done "<<endreq;
	}
}
VolumeBuilder* Geo2G4Svc::GetVolumeBuilder(std::string s)
{
	if (m_builders.find(s)!=m_builders.end())
	{
		return m_builders[s];
	}
	else
	{
		log<<MSG::ERROR<<"Trying to retrieve a not existing builder "<<
			s<<endreq;
		log<<MSG::ERROR<<"\treturning Default Builder"<<endreq;
	}
	return defaultBuilder;
}
