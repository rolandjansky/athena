/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTCNVTOOLS_EVENTCNVSUPERTOOL
#define TRKEVENTCNVTOOLS_EVENTCNVSUPERTOOL

#include "TrkEventCnvTools/IEventCnvSuperTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaPoolUtilities/TPObjRef.h"

#include <atomic>

class Identifier;
class AtlasDetectorID;

namespace Trk
{
  class ITrkEventCnvTool;
  class Surface;
  class RIO_OnTrack;

    /** Tool to handle TP conversions for Tracking EDM.
    This tool has sub tools that do the detector-specific conversions*/
      class EventCnvSuperTool : virtual public IEventCnvSuperTool, public AthAlgTool
    {
    public:

      EventCnvSuperTool(const std::string& t,
        const std::string& n,
        const IInterface*  p);

      virtual ~EventCnvSuperTool ();

      virtual StatusCode initialize();

      virtual StatusCode finalize();

      const Trk::ITrkEventCnvTool* getCnvTool(const Identifier& id) const;

        /** From passed Identifier*/
      virtual const Trk::Surface* getSurface(const Identifier& id) const;

        /** Take the passed RoT and recreate it (i.e. fill missing pointers etc)*/
      virtual void recreateRIO_OnTrack( RIO_OnTrack *RoT ) const;

        /** Take the passed RoT and prepare the PRD ElementLink for writing to disc*/
      virtual void prepareRIO_OnTrack( RIO_OnTrack* Rot) const;

      virtual bool canHandleInDet() const            { return m_haveIdCnvTool;}
      virtual bool canHandleMuonSpectrometer() const { return m_haveMuonCnvTool;}
      virtual int  maxNumberOfErrors() const         { return m_maxErrCount;}
      
      virtual bool noGeometry() const {return !(m_haveIdCnvTool&&m_haveMuonCnvTool);}

    private:
      ToolHandle<Trk::ITrkEventCnvTool>   m_idCnvTool; //!< Tool used to handle ID RoTs etc
      ToolHandle<Trk::ITrkEventCnvTool>   m_muonCnvTool; //!< Tool used to handle Muon RoTs etc
      const AtlasDetectorID*              m_detID;             // atlas id helper

      bool                                m_haveIdCnvTool; //!< Flag set in intialise, letting us know whether the tool has been found.
      bool                                m_haveMuonCnvTool; //!< Flag set in intialise, letting us know whether the tool has been found.

      bool                                m_doMuons; //!< Property deciding whether to attempt Muon conversions
      bool                                m_doID;    //!< Property deciding whether to attempt ID conversions

      mutable std::atomic_int             m_errCount; //!< Current number of ERROR/WARNING messages sent to output
      int                                 m_maxErrCount; //!< Maximum number of permissable ERROR/WARNING messages sent to output.
    };
  }
#endif

