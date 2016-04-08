/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetDetDescrExample_PrintSiElements_h
#define InDetDetDescrExample_PrintSiElements_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
//#include "CLHEP/Geometry/Transform3D.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <fstream>

class IGeoModelSvc;

/////////////////////////////////////////////////////////////////////////////

class PrintSiElements : public AthAlgorithm {
public:
  PrintSiElements (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  StatusCode printElements(const std::string & managerName);

  std::string printTransform(const Amg::Transform3D & trans) const;
  void extractAlphaBetaGamma(const Amg::Transform3D & trans, double& alpha, double& beta, double &gamma) const;

private:
  std::ofstream m_fileout;
  bool m_firstEvent;
  // Alg properties
  bool m_modulesOnly;
  bool m_expandId;
  bool m_nominal;
  bool m_aligned;
  bool m_fullRotationMatrix;
  std::string m_outputFileName;
  ServiceHandle<IGeoModelSvc> m_geoModelSvc;
};

#endif // InDetDetDescrExample_PrintSiElements_h
