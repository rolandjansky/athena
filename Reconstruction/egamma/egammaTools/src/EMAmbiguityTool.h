/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMAMBIGUITYTOOL_H
#define EMAMBIGUITYTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
// #include "GaudiKernel/ToolHandle.h"
#include "egammaInterfaces/IEMAmbiguityTool.h"

#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"

/**
   @class EMAmbiguityTool
   This tool resolves the electron-converted photon ambiguity.
   @author Thomas Koffas
   @author Bruno Lenzi 
   @author Christos Anastopoulos
*/

class EMAmbiguityTool : virtual public IEMAmbiguityTool, public AthAlgTool {

public:
  EMAmbiguityTool (const std::string& type,const std::string& name, const IInterface* parent);
  virtual ~EMAmbiguityTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  unsigned int ambiguityResolve(const xAOD::Vertex* vx, 
				const xAOD::TrackParticle* trParticle) const;

private:

  /** @brief Minimum number of silicon hits to be an electron */
  unsigned int m_MinNoSiHits;
  unsigned int m_MinNoPixHits;
};

#endif // EMAMBIGUITYTOOL_H 
