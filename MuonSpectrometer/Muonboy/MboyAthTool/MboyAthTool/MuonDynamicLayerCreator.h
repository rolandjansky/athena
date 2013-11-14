/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonDynamicLayerCreator_H
#define MuonDynamicLayerCreator_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 

#include "TrkDetDescrInterfaces/IDynamicLayerCreator.h"

#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/PlaneSurface.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkSurfaces/CylinderSurface.h"

#include "TrkExInterfaces/IPropagator.h"
#include "TrkExInterfaces/INavigator.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthTool/MboyCollectMatterTool.h"

namespace Trk {
  
  /** @class MuonDynamicLayerCreator
      
  @author mlimper@nikhef.nl
  */
  
  class MuonDynamicLayerCreator : public AthAlgTool,
    virtual public IDynamicLayerCreator {
      
  public:
      
      /** AlgTool like constructor */
      MuonDynamicLayerCreator(const std::string&,const std::string&,const IInterface*);
      
      /**Virtual destructor*/
      virtual ~MuonDynamicLayerCreator();
      
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
      
      /** return all MaterialLayers associated to this track **/
      const std::vector< std::pair<const TrackParameters*, const Layer* > >* materialLayers(const Trk::TrackParameters& parm,
							const Trk::Surface& sf,
							const Trk::IPropagator* prop = 0) const ;
  private:      
      
      bool                            m_noreducs;                     // switches between different layer-functions from MboyCollectMatterTool
      
      double m_MuSpectro_Radius ; // Definition of Muon Spectro Entrance
      double m_MuSpectro_Zcyli  ; // Definition of Muon Spectro Entrance
      double m_RadiusProtection ; // Small radii protection
   
      ToolHandle< INavigator > p_INavigator ; //!< Pointer On INavigator
      ToolHandle< IPropagator > p_IPropagator ; //!< Pointer On IPropagator
      ToolHandle< MboyCollectMatterTool > p_MboyCollectMatterTool ; //!< Pointer On MboyCollectMatterTool

    };
  
} 

#endif 
