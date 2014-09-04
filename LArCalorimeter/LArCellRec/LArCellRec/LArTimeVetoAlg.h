/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TheLArCollisionsAlg.h
//

#ifndef _LArTimeVetoAlg_LArTimeVetoAlg_H
#define _LArTimeVetoAlg_LArTimeVetoAlg_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"


class AthenaAttributeList;

class LArTimeVetoAlg : public AthAlgorithm {
  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    LArTimeVetoAlg(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~LArTimeVetoAlg();
    
    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();

  private:
  int m_nevt;
  int m_nevtMasked;
  const DataHandle<AthenaAttributeList> m_dd_atrList;
  std::string m_folderName;

};
#endif
