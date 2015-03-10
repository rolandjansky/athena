/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CBNT_UPSTREAMMAT_H
#define CBNT_UPSTREAMMAT_H

#include "CBNT_TBRecBase.h"
#include "GaudiKernel/NTuple.h"
#include <string>

class StoreGateSvc;

class CBNT_UpstreamMat : public CBNT_TBRecBase {
public:
	CBNT_UpstreamMat(const std::string &name, ISvcLocator *pSvcLocator);
	virtual ~CBNT_UpstreamMat();
	virtual StatusCode CBNT_initialize();
	virtual StatusCode CBNT_execute();
	virtual StatusCode CBNT_finalize();
	virtual StatusCode CBNT_clear();

private:
	StoreGateSvc * m_storeGate;
	std::string m_key;
	bool m_notFound;
   //Ntuple variables:

	long     m_nElec;
	std::vector<float>    *m_ElecEne;
	std::vector<float>    *m_ElecAng;
	long     m_nPhot;
	std::vector<float>    *m_PhotEne;
	std::vector<float>    *m_PhotAng;
};

#endif // CBNT_EVENTINFO_H










