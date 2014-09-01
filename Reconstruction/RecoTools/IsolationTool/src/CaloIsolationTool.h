/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONTOOL_CALOISOLATIONTOOL_H
#define ISOLATIONTOOL_CALOISOLATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "IsolationTool/ICaloIsolationTool.h"
#include "xAODBase/IParticle.h"
#include "xAODPrimitives/IsolationType.h"
#include "ITrackInCaloTools/ITrackInCaloTools.h"
#include "xAODTracking/TrackParticle.h"

namespace xAOD {

  class CaloIsolationTool: public AthAlgTool, virtual public ICaloIsolationTool {
  public:
    CaloIsolationTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~CaloIsolationTool(void); // destructor
  
    StatusCode initialize();
    StatusCode finalize();

    /**ICaloIsolationTool interface: */    
    bool caloIsolation( CaloIsolation& result, const IParticle& tp, const std::vector<Iso::IsolationType>& cones, SubtractionStrategy strategy = Ecore ); 

    /**ICaloIsolationTool interface: */    
    bool caloIsolation( CaloIsolation& result, const TrackParticle& tp, const std::vector<Iso::IsolationType>& cones, SubtractionStrategy strategy = Ecore );

  private:

    bool etConeIsolation( CaloIsolation& result, const TrackParticle& tp, const std::vector<Iso::IsolationType>& isoTypes, 
                          CaloIsolationTool::SubtractionStrategy strategy );

    ToolHandle<ITrackInCaloTools>        m_toCalo;
  };

}	// end of namespace

#endif


