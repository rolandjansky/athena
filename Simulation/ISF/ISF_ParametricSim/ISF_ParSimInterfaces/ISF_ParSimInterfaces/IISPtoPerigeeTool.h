/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IISPtoPerigeeTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_PARSIMINTERFACES_IISPTOPERIGEETOOL_H
#define ISF_PARSIMINTERFACES_IISPTOPERIGEETOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Trk
#include "TrkParameters/TrackParameters.h"

#include <utility>


namespace ISF {
  class ISFParticle;
} 

namespace iParSim {
     
  /** 
   @class IISPtoPerigeeTool

   Interface definition for a track parameters extractor
 
   @author Miha.Muskinja -at- cern.ch 
   */
      
  class IISPtoPerigeeTool : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IISPtoPerigeeTool(){}

       /// Creates the InterfaceID and interfaceID() method
       DeclareInterfaceID(IISPtoPerigeeTool, 1, 0);

       virtual const Trk::TrackParameters* extractTrkParameters(const ISF::ISFParticle& isp) const = 0;

       virtual const Amg::Vector3D getPerigee() const = 0;
       
  };

} // end of namespace

#endif 

