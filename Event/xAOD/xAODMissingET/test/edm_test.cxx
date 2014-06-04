/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"

#include <cstdio>

using namespace xAOD;

int main()
{
  MissingET* m0 = new MissingET( 10., 10.,sqrt(200.),"MET0");
  MissingET* m1 = new MissingET(  5.,-10.,sqrt(125.),"MET1");

  MissingETAuxContainer aux;
  MissingETContainer metColl;
  metColl.setStore( &aux );

  metColl.push_back(m0);
  metColl.push_back(m1);

  printf("xAODMissingET::edm_test: have %i objects in container\n",
	 (int)metColl.size());

  MissingETContainer::const_iterator 
     fm0(metColl.find("MET1"));
  if ( fm0 == metColl.end() )
     { printf("xAODMissingET::edm_test: cannot find object MET1\n"); }
  else 
     { printf("xAODMissingET::edm_test: found object MET1\n"); }

  return 0;
}
