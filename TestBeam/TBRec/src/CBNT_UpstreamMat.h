/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_CBNT_UPSTREAMMAT_H
#define TBREC_CBNT_UPSTREAMMAT_H

#include "CBNT_TBRecBase.h"
#include <string>


class CBNT_UpstreamMat : public CBNT_TBRecBase {
public:
	CBNT_UpstreamMat(const std::string &name, ISvcLocator *pSvcLocator);
	virtual ~CBNT_UpstreamMat();
	virtual StatusCode CBNT_initialize() override;
	virtual StatusCode CBNT_execute() override;
	virtual StatusCode CBNT_finalize() override;
	virtual StatusCode CBNT_clear() override;

private:
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










