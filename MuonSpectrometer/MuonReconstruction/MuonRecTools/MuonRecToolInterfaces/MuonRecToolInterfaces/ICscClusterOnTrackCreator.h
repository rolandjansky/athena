/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_ICSCCLUSTERONTRACKCREATOR_H
#define MUON_ICSCCLUSTERONTRACKCREATOR_H

#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "GaudiKernel/ToolHandle.h"

static const InterfaceID IID_ICscClusterOnTrackCreator
    ("Muon::ICscClusterOnTrackCreator",1,0);

class ICscStripFitter;
class ICscClusterFitter;
class ICscClusterUtilTool;

namespace Muon {


  /** @brief Interface for tools calibrating CSCCluster, turning them into Muon::ICscClusterOnTrack object. 
      The interface inherits from IMuonClusterOnTrackCreator. 
  */
  class ICscClusterOnTrackCreator : virtual public IMuonClusterOnTrackCreator
  {      
  public:
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////
    
    static const InterfaceID& interfaceID();
    

    /** @brief Return some tools used by CscClusterOnTrackCreator       
    */
    virtual const ToolHandle<ICscStripFitter>& GetICscStripFitter() const = 0;
    virtual const ToolHandle<ICscClusterFitter>& GetICscClusterFitter() const = 0;
    virtual const ToolHandle<ICscClusterUtilTool>& GetICscClusterUtilTool() const = 0;
  };
  
  inline const InterfaceID& ICscClusterOnTrackCreator::interfaceID()
  {
    return IID_ICscClusterOnTrackCreator;
  }
} // end of name space

#endif // MUON_ICSCCLUSTERONTRACKCREATOR_H

