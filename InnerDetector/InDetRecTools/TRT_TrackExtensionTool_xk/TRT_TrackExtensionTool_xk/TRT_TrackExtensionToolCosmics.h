/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_TrackExtensionToolCosmics
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
//  Concret implementation for base class ITRT_TrackExtensionTool
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 22/08/2005 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_TrackExtensionToolCosmics_H
#define TRT_TrackExtensionToolCosmics_H

#include <vector>

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ITRT_TrackExtensionTool.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "TrkEventPrimitives/PropDirection.h"

#include "StoreGate/ReadHandleKey.h"

class MsgStream;
class TRT_ID;
class AtlasDetectorID;

namespace Trk{

  class Surface;
  class IExtrapolator;
  class IPropagator;
}

namespace InDet {

  /**
  @class TRT_TrackExtensionToolCosmics 
  
  @author Christian.Schmitt@cern.ch     
  */

  class TRT_TrackExtensionToolCosmics : 

    virtual public ITRT_TrackExtensionTool, public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      TRT_TrackExtensionToolCosmics
	(const std::string&,const std::string&,const IInterface*);
      virtual ~TRT_TrackExtensionToolCosmics();
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      ///////////////////////////////////////////////////////////////////
      // Main methods for track extension to TRT
      ///////////////////////////////////////////////////////////////////
      
      virtual std::vector<const Trk::MeasurementBase*>& extendTrack
	(const Trk::Track&);
      virtual std::vector<const Trk::MeasurementBase*>& extendTrack
	(const Trk::TrackParameters&);
      virtual Trk::TrackSegment* findSegment
	(const Trk::TrackParameters&);
      virtual Trk::Track* newTrack
	(const Trk::Track&);
      virtual void newEvent();

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump(MsgStream&    out) const;
      std::ostream& dump(std::ostream& out) const;

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      ToolHandle<Trk::IExtrapolator>        m_extrapolator;
      ToolHandle<Trk::IPropagator>          m_propagator  ;
      ToolHandle<Trk::IRIO_OnTrackCreator>  m_riontrackD  ; //
      ToolHandle<Trk::IRIO_OnTrackCreator>  m_riontrackN  ; //

      int                              m_outputlevel    ; // Print level
      int                              m_nprint         ; // Kind of print
      int                              m_minNumberDCs   ; // Min. number of DriftCircles
      double                           m_roadwidth      ; // Max width of the road
      double                           m_roadwidth_locz ; // Max width of the road along the straw
      bool                             m_searchNeighbour; // Also search neighbouring detector elements?
      bool                             m_boundarycheck  ; // Do a boundary check in the extrapolation?
      std::string                      m_trtmanager     ; // Name of TRT det. manager 
      //std::string                      m_trtname        ; // Name container with TRT clusters

      SG::ReadHandleKey<TRT_DriftCircleContainer> m_trtname {this,"TRT_ClustersContainer","TRT_DriftCircles","RHK to retrieve TRT_DriftCircles"};

      std::vector<const Trk::MeasurementBase*>  m_measurement  ;

      AtlasDetectorID*                       m_idHelper; //<! Detector ID helper
    
      const TRT_ID* m_trtid ;

      MsgStream&    dumpConditions(MsgStream   & out) const;
      MsgStream&    dumpEvent     (MsgStream   & out) const;


      Trk::Surface *m_trtcylinder; 
      Trk::Surface *m_trtdiscA,*m_trtdiscC;

      void analyze_tpars(const std::vector<const Trk::TrackParameters* >* tpars);
      Trk::Surface* findBoundarySurface(const Trk::TrackParameters& par, Trk::PropDirection dir);
      Amg::Vector3D intersect(const Trk::Surface *surf,const Trk::Perigee *per);

    };

  MsgStream&    operator << (MsgStream&   ,const TRT_TrackExtensionToolCosmics&);
  std::ostream& operator << (std::ostream&,const TRT_TrackExtensionToolCosmics&); 

} // end of name space

#endif // TRT_TrackExtensionToolCosmics_H

