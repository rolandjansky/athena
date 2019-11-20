/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
         (const Trk::Track&,
          InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const override;

      virtual std::vector<const Trk::MeasurementBase*>& extendTrack
         (const Trk::TrackParameters&,
          InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const override;

      virtual Trk::TrackSegment* findSegment
	 (const Trk::TrackParameters&,
          InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const override;

      virtual Trk::Track* newTrack
         (const Trk::Track&,
          InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const override;

      virtual std::unique_ptr<InDet::ITRT_TrackExtensionTool::IEventData> newEvent() const override;

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

      class EventData : public InDet::ITRT_TrackExtensionTool::IEventData
      {
         friend class TRT_TrackExtensionToolCosmics;
      public:
         EventData(const TRT_DriftCircleContainer *trtcontainer) : m_trtcontainer(trtcontainer) {}

         ~EventData() {
            delete m_trtcylinder;
            delete m_trtdiscA;
            delete m_trtdiscC;
         }

         virtual unsigned int type() const override { return s_type;}


         static
         InDet::TRT_TrackExtensionToolCosmics::EventData &
         getEventData(InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) {
            if (virt_event_data.type() != InDet::TRT_TrackExtensionToolCosmics::EventData::s_type) {
               throw std::logic_error("EventData mismatch in call TRT_TrackExtensionTool_xk::extendTrack" );
            }
            return static_cast<InDet::TRT_TrackExtensionToolCosmics::EventData&>(virt_event_data);
         }

      protected:

         static constexpr unsigned int s_type = 0xa875311a; // first 32bit of sha1sum of InDet::TRT_TrackExtensionTool_xk::EventData

         const TRT_DriftCircleContainer  *m_trtcontainer = nullptr;
         Trk::Surface                    *m_trtcylinder  = nullptr;
         Trk::Surface                    *m_trtdiscA     = nullptr;
         Trk::Surface                    *m_trtdiscC     = nullptr;

         std::vector<const Trk::MeasurementBase*>  m_measurement  ;
      };


      AtlasDetectorID*                       m_idHelper; //<! Detector ID helper
    
      const TRT_ID* m_trtid ;

      MsgStream&    dumpConditions(MsgStream   & out) const;
      MsgStream&    dumpEvent     (MsgStream   & out) const;



      void analyze_tpars(const std::vector<const Trk::TrackParameters* >* tpars,
                         InDet::TRT_TrackExtensionToolCosmics::EventData &event_data) const;

      Trk::Surface* findBoundarySurface(const Trk::TrackParameters& par,
                                        Trk::PropDirection dir,
                                        InDet::TRT_TrackExtensionToolCosmics::EventData &event_data) const;

      static Amg::Vector3D intersect(const Trk::Surface *surf,const Trk::Perigee *per);

    };

  MsgStream&    operator << (MsgStream&   ,const TRT_TrackExtensionToolCosmics&);
  std::ostream& operator << (std::ostream&,const TRT_TrackExtensionToolCosmics&); 

} // end of name space

#endif // TRT_TrackExtensionToolCosmics_H

