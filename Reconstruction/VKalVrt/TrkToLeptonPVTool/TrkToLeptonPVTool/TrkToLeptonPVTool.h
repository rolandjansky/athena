/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//
// TrkToLeptonPVTool.h - Description
//
/*
   Tool to match a track to a vertex assumed to be a Primary Vertex.
   This PV can be taken elsewhere or obtained with the tool itsels.
   Example:  check compatibility of a track to PV obtained with 2 energetic leptons from Z decay.
   See README fro more details

    Author: Vadim Kostyukhin
    e-mail: vadim.kostyukhin@cern.ch
*/
#ifndef TrkToLeptonPVTool_H
#define TrkToLepronPVTool_H

#include <vector>
#include <utility>
#include <memory>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"
//

class IChronoStatSvc;
class IBeamCondSvc;

namespace Trk{
    class TrkVKalVrtFitter;
    class IVertexFitter;
}
//------------------------------------------------------------------------
  static const InterfaceID IID_TrkToLeptonPVTool("ITrkToLeptonPVTool", 1, 0);

  class ITrkToLeptonPVTool : virtual public IAlgTool {
    public:
      static const InterfaceID& interfaceID() { return IID_TrkToLeptonPVTool;}
//---------------------------------------------------------------------------
//Interface itself

      virtual std::unique_ptr<const xAOD::Vertex> matchTrkToPV( const xAOD::TrackParticle*, const xAOD::Vertex*, const xAOD::EventInfo*) const =0;
      virtual std::unique_ptr<const xAOD::Vertex> npartVertex(const std::vector<const xAOD::TrackParticle*>&, const xAOD::EventInfo *) const =0;
  };




  class TrkToLeptonPVTool : public AthAlgTool, virtual public ITrkToLeptonPVTool
  {

   public:
       /* Constructor */
      TrkToLeptonPVTool(const std::string& type, const std::string& name, const IInterface* parent);
       /* Destructor */
      virtual ~TrkToLeptonPVTool();


      StatusCode initialize();
      StatusCode finalize();


      std::unique_ptr<const xAOD::Vertex> matchTrkToPV(const xAOD::TrackParticle *trk, const xAOD::Vertex * PV, const xAOD::EventInfo *) const;
      std::unique_ptr<const xAOD::Vertex> npartVertex(const std::vector<const xAOD::TrackParticle*>&, const xAOD::EventInfo *) const;
//------------------------------------------------------------------------------------------------------------------
// Private data and functions
//

   private:

    ServiceHandle< IBeamCondSvc > m_beamService; 
    ToolHandle< Trk::IVertexFitter >       m_fitterSvc;

    Trk::TrkVKalVrtFitter*   m_fitSvc{};
    IChronoStatSvc * m_timingProfile{}; 

    //float m_bl{};

 };


#endif
