/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZVERTEXFROMKINE_H
#define ZVERTEXFROMKINE_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"

class ZVertexFromKine : public AthAlgorithm 
{
public:
  
   ZVertexFromKine(const std::string& name, ISvcLocator* pSvcLocator);
   ~ZVertexFromKine();
  
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:

   std::string m_mcEventCollectionName;
};


#endif
