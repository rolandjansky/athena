/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ServiceAccessor_H
#define ServiceAccessor_H


#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "GaudiKernel/IHistogramSvc.h"

#include <iostream>

// access the message service
inline IMessageSvc* msgSvc() 
{
	static IMessageSvc* mS=0;

	if (!mS)
	{
		StatusCode status;
		ISvcLocator* svcLocator = Gaudi::svcLocator();
		status=svcLocator->service("MessageSvc", mS);
		if (status.isFailure())
			std::cout<<" msgSvc(); could not access MessageSvc"<<std::endl;
			
	}
	return mS;	
}

// access the histogram service
inline IHistogramSvc* histoSvc() 
{
	static IHistogramSvc* hS=0;

	if (!hS)
	{
		StatusCode status;
		ISvcLocator* svcLocator = Gaudi::svcLocator();
		status=svcLocator->service("HistogramDataSvc",hS);
		if (status.isFailure())
			std::cout<<" histoSvc(); could not access HistogramDataSvc"<<std::endl;
	}
	return hS;
}

// access store gate

#include "StoreGate/StoreGateSvc.h"

inline StoreGateSvc* storeGateSvc()
{
	static StoreGateSvc* sG=0;
	
	if (!sG)
	{
		StatusCode status;
     	ISvcLocator* svcLocator = Gaudi::svcLocator();
		status = svcLocator->service("StoreGateSvc", sG);
		if (status.isFailure())
			std::cout<<" storeGateSvc(); could not access StoreGateSvc"<<std::endl;
	}
	return sG;
}

#include "GaudiKernel/INTupleSvc.h"

inline INTupleSvc* ntupleSvc()
{
	static INTupleSvc* nS=0;
	
	if (!nS)
	{
		StatusCode status;
		ISvcLocator* svcLocator = Gaudi::svcLocator();
		status = svcLocator->service("NTupleSvc",nS);
		if (status.isFailure())
			std::cout<<" ntupleSvc(); could not access NTupleSvc"<<std::endl;
	}
	return nS;
}

#endif
