/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * Unit test for TauJEt
 */

#include <iostream>
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"

void fill(xAOD::TauJet& p)
{
	p.setDetail(xAOD::TauJetParameters::ipSigLeadTrk, (float)1.1);
	p.setDetail(xAOD::TauJetParameters::numTopoClusters, (int)3);
}

void print(xAOD::TauJet& p)
{

	float f;
	int i;
	p.detail(xAOD::TauJetParameters::ipSigLeadTrk, f);
	p.detail(xAOD::TauJetParameters::numTopoClusters, i);
	std::cout << "standard way: "
			  << "ipSigLeadTrk=" << f
			  << ", numTopoClusters=" << i
			  <<std::endl;
	std::cout << "template way: "
			  << "ipSigLeadTrk=" 		<< p.detail<float>(xAOD::TauJetParameters::ipSigLeadTrk)
			  <<", numTopoClusters="    << p.detail<int>(xAOD::TauJetParameters::numTopoClusters)
			  <<std::endl;
}

int main()
{
	xAOD::TauJetContainer tjc;
	xAOD::TauJetAuxContainer aux;
	tjc.setStore( &aux );
	xAOD::TauJet* p = new xAOD::TauJet();
	tjc.push_back(p);

	fill(*p);
	print(*p);
	p->dump();
	return 0;
}
