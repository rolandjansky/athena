/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  PlanarClusterOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface for PlanarClusterOnTrack production
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRARECOTOOLS_PLANARCLUSTERONTRACKTOOL_H
#define ISF_FATRARECOTOOLS_PLANARCLUSTERONTRACKTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"

#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "ISF_FatrasEvent/PlanarClusterOnTrack.h"

#include "TrkParameters/TrackParameters.h"
#include "GeoPrimitives/GeoPrimitives.h"


class StoreGateSvc;
class PixelID;
class IPixelOfflineCalibSvc;


namespace iFatras {

  class PlanarClusterOnTrackTool: public AthAlgTool, virtual public Trk::IRIO_OnTrackCreator,  virtual public IIncidentListener
  {
    
  public:
    
    //! AlgTool constructor 
    PlanarClusterOnTrackTool(const std::string&,const std::string&,
			     const IInterface*);
    
    virtual ~PlanarClusterOnTrackTool ();
    
    //! AlgTool initialisation
    virtual StatusCode initialize();
    
    //! AlgTool termination
    virtual StatusCode finalize  ();
    
    virtual const PlanarClusterOnTrack* correct(const Trk::PrepRawData&,
						const Trk::TrackParameters&) const;
    /** handle for incident service */
    void handle(const Incident& inc) ;

  private:
    bool        m_useDefinedParameters;
    
  };

}

#endif // ISF_FATRARECOTOOLS_PLANARCLUSTERONTRACKTOOL_H
