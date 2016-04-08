/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetDetDescrExample_InDetUpdateCaches_h
#define InDetDetDescrExample_InDetUpdateCaches_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Algorithm.h"

#include <vector>
#include <string>


namespace InDetDD {
  class InDetDetectorManager;
}
 
class InDetUpdateCaches : public AthAlgorithm 
{

public:
  InDetUpdateCaches (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  void updateAll();

private:
  bool m_first;
  std::vector<const InDetDD::InDetDetectorManager *> m_detManagers;

  // Alg properties
  bool m_inInit;
  bool m_inExec;

  float getMem();

};

#endif //  InDetDetDescrExample_InDetUpdateCaches_h
