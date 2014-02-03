/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4SHOWERLIBSVC_G4SHOWERLIBSVCTEST_H
#define G4SHOWERLIBSVC_G4SHOWERLIBSVCTEST_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"

#include "LArG4ShowerLibSvc/ILArG4ShowerLibSvc.h"

class LArG4ShowerLibSvcTest : public AthAlgorithm {

 public:

   LArG4ShowerLibSvcTest (const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~LArG4ShowerLibSvcTest ();

   StatusCode initialize();
   StatusCode finalize();
   StatusCode execute();

 private:

   ServiceHandle<ILArG4ShowerLibSvc> m_showerLibSvc;

};

#endif // G4SHOWERLIBSVC_G4SHOWERLIBSVCTEST_H
