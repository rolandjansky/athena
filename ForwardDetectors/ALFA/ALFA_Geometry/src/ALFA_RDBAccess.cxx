/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_Geometry/ALFA_GeometryReader.h"
#include "ALFA_Geometry/ALFA_RDBAccess.h"


ALFA_RDBAccess::ALFA_RDBAccess()
  : m_svcLocator(nullptr)
{

}

ALFA_RDBAccess::~ALFA_RDBAccess()
{
}

bool ALFA_RDBAccess::ReadGeometry(const eRPotName eRPName, eFiberType eFType, std::string element, std::string tag, std::string node)
{
	bool bRes = false;
	
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_RDBAaccess::ReadGeometry");
	
	m_svcLocator = Gaudi::svcLocator();
	IRDBAccessSvc* iAccessSvc = NULL;

	StatusCode result = m_svcLocator->service("RDBAccessSvc", iAccessSvc);
	if (result.isFailure() || iAccessSvc == NULL)
	{
		LogStream << MSG::FATAL << " Could not initialize RDBAccessSvc! ((RP no."<<eRPName<<", Fiber type "<<eFType<<")"<< endmsg;

		throw GaudiException(" Could not initalize RDBAccessSvc ", "ALFA_RDBAccess::ReadGeometry", StatusCode::FAILURE);
	}

	/*
	IGeoModelSvc *geoModel = 0;
	result = m_svcLocator->service("GeoModelSvc",geoModel);
	if (result.isFailure() || geoModel == 0)
	{
		LogStream << MSG::FATAL << " Could not initialize GeoModelSvc! (RP no."<<eRPName<<", Fiber type "<<eFType<<")"<< endmsg;

		throw GaudiException(" Could not initalize GeoModelSvc ", "ALFA_RDBAccess::ReadGeometry", StatusCode::FAILURE);
	}*/

	bRes=ReadFiberGeometry(iAccessSvc, element, tag, node);
//	bRes|=ReadFiberGeometry("ALFAFibreUp","ALFA-00","ALFA");

	return bRes;
}

bool ALFA_RDBAccess::ReadFiberGeometry(IRDBAccessSvc* iAccessSvc, std::string element, std::string tag, std::string node)
{
	bool bRes = false;
	
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_RDBAaccess::ReadFiberGeometry");

	std::list<FIBERDATA> listFData;
	listFData.clear();

	m_fiberGeometry = iAccessSvc->getRecordsetPtr(element, tag, node);
	
	if (!m_fiberGeometry->size())
	{
		bRes = false;
		LogStream << MSG::FATAL << " ERROR: Unable to retrieve " << element <<" data from database" << endmsg;
		//throw GaudiException(" Unable to retrieve database data ", "ALFA_RDBAccess::ReadFiberGeometry", StatusCode::FAILURE);
	}
	else
	{
		FIBERDATA FiberData;
		IRDBRecordset::const_iterator AccessSvc_iter;
		
		for(AccessSvc_iter = m_fiberGeometry->begin(); AccessSvc_iter != m_fiberGeometry->end(); AccessSvc_iter++)
		{
			FiberData.nPotID   = (*AccessSvc_iter)->getInt("POTID");
			FiberData.nLayerID = (*AccessSvc_iter)->getInt("COORDID");
			FiberData.nFiberID = (*AccessSvc_iter)->getInt("FIBREID");
			FiberData.fSlope   = (*AccessSvc_iter)->getDouble("SLOPE");
			FiberData.fOffset  = (*AccessSvc_iter)->getDouble("OFFSET");
			FiberData.fZPos    = (*AccessSvc_iter)->getDouble("ZL");
			
			listFData.push_back(FiberData);
		}
		
		if (listFData.size())
		{
			m_ListFiberData=listFData;
			bRes = true;
		}
	}

	listFData.clear();
	
	return bRes;
}






