/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ITrackCutSvcr.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_INDETTRACKCUTSVC_H
#define TRK_INDETTRACKCUTSVC_H

#include "TrkValInterfaces/ITrackCutSvc.h"
#include "AthenaBaseComps/AthService.h"

namespace InDet {

 
/** @class ITrackTCutSvc
    @brief services that allows to centrally modify track cuts
    for ITk 1.6 reconstructions
*/

class InDetTrackCutSvc : public AthService, virtual public Trk::ITrackCutSvc {
 
    public:
 
      StatusCode initialize();
      
      StatusCode finalize();
 
      /** Query the interfaces.
      /   Input: riid, Requested interface ID
      /          ppvInterface, Pointer to requested interface
      /   Return: StatusCode indicating SUCCESS or FAILURE.
      / N.B. Don't forget to release the interface after use!!! **/
      StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

      Trk::TrackCuts trackCuts() const final;
 
    protected:
 
      /** Standard Constructor */
      InDetTrackCutSvc(const std::string& name, ISvcLocator* svc);
 
      /** Standard Destructor */
      virtual ~InDetTrackCutSvc();
 
      friend class SvcFactory<InDetTrackCutSvc>;
      
      Trk::TrackCuts m_trackCuts;
      
      };
      
      inline Trk::TrackCuts InDetTrackCutSvc::trackCuts() const { return m_trackCuts; }

} // end namespace
#endif
