/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RDBAccess_h
#define ALFA_RDBAccess_h

#include "GaudiKernel/Algorithm.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include <list>
#include <string>

//using namespace std;

typedef struct _FIBERDATA
{
	int nPotID;
	int nLayerID;
	int nFiberID;
	
	double fSlope;
	double fOffset;
	double fZPos;
} FIBERDATA, *PFIBERDATA;

class ALFA_RDBAccess
{
	public:
		ALFA_RDBAccess();
		~ALFA_RDBAccess();
	
	private:
//		bool ReadFiberGeometry(string element, string tag, string node);
		bool ReadFiberGeometry(IRDBAccessSvc* iAccessSvc, std::string element, std::string tag, std::string node);
	
	public:
		bool ReadGeometry(const eRPotName eRPName, eFiberType eFType,
                                  const std::string& element,
                                  const std::string& tag,
                                  const std::string& node);
		
	private:
		IRDBRecordset_ptr m_fiberGeometry;
		ISvcLocator* m_svcLocator;

	public:
		std::list<FIBERDATA> m_ListFiberData;
		std::list<FIBERDATA> m_ListODFiberData;
};

#endif
