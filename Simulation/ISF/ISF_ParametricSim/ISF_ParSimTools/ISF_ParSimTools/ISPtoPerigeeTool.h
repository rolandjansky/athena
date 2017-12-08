/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "InDetBeamSpotService/IBeamCondSvc.h"



namespace ISF {
    class ISFParticle;
}

namespace iParSim {


  /** 
   @class ISPtoPerigeeTool

   Extract the perigee parameters of an ISP particle

   @author Miha.Muskinja -at- cern.ch 
   */


  class ISPtoPerigeeTool : public AthAlgTool, 
        virtual public iParSim::IISPtoPerigeeTool 
  {
    public:

      /**Constructor */
      ISPtoPerigeeTool(const std::string&,const std::string&,const IInterface*);

      /**Destructor*/
      ~ISPtoPerigeeTool ();

      /** AlgTool initailize method.*/
      StatusCode initialize();
      
      /** AlgTool finalize method */
      StatusCode finalize();
      
      /** Return track parameters of the ISF Particle */
      const Trk::TrackParameters* extractTrkParameters(const ISF::ISFParticle& isp) const override;

      /** Return the position of the perigee (beam spot) */
      const Amg::Vector3D getPerigee() const override;

    protected:

      /** Extrapolator tool */
      ToolHandle<Trk::IExtrapolator>     m_extrapolator;

      /** Beam Conditions Service */
      ServiceHandle<IBeamCondSvc>        m_beamSpotSvc;

  };

} // end of namespace


#endif