/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISPtoPerigee.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_PARSIMTOOLS_ISPTOPERIGEETOOL_H
#define ISF_PARSIMTOOLS_ISPTOPERIGEETOOL_H

#include "ISF_ParSimInterfaces/IISPtoPerigeeTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "BeamSpotConditionsData/BeamSpotData.h"


namespace ISF {
    class ISFParticle;
}

namespace iParSim {


  /** 
   @class ISPtoPerigeeTool

   Extract the perigee parameters of an ISP particle

   @author Miha.Muskinja -at- cern.ch 
   */


  class ISPtoPerigeeTool : public extends<AthAlgTool, iParSim::IISPtoPerigeeTool>
  {
    public:

      /**Constructor */
      ISPtoPerigeeTool(const std::string&,const std::string&,const IInterface*);

      /**Destructor*/
      virtual ~ISPtoPerigeeTool ();

      /** AlgTool initialize method.*/
      virtual StatusCode initialize() override;
      
      /** AlgTool finalize method */
      virtual StatusCode finalize() override;
      
      /** Return track parameters of the ISF Particle */
      virtual
      const Trk::TrackParameters* extractTrkParameters(const ISF::ISFParticle& isp) const override;

      /** Return the position of the perigee (beam spot) */
      virtual
      const Amg::Vector3D getPerigee() const override;

    protected:

      /** Extrapolator tool */
      ToolHandle<Trk::IExtrapolator>     m_extrapolator;
      SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

  };

} // end of namespace


#endif
