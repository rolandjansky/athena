/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IProcessSamplingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASINTERFACES_IPROCESSSAMPLINGTOOL_H
#define ISF_FATRASINTERFACES_IPROCESSSAMPLINGTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "TrkExUtils/ExtrapolationCell.h"
#include "TrkExInterfaces/IExtrapolationEngine.h"
#include "ISF_Event/ISFParticleVector.h"
//#include "TrkExInterfaces/ITimedExtrapolator.h"

namespace Trk{
  class Track;
  struct PathLimit;
}
  
namespace ISF {
  class ISFParticle;    
}

namespace iFatras {
  
  static const InterfaceID IID_IProcessSamplingTool("IProcessSamplingTool", 1, 0);
    
  /** 
   @class IProcessSamplingTool

   sampling the process/free path
       
   @author Sarka.Todorova -at- cern.ch
   
   */
      
  class IProcessSamplingTool : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IProcessSamplingTool(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IProcessSamplingTool; }

       /** Process, path limit */
       virtual Trk::PathLimit sampleProcess(double momentum, double charge, Trk::ParticleHypothesis pHypothesis) const=0;

       /** Process simulation */
       virtual ISF::ISFParticleVector  interact(const ISF::ISFParticle* isp,
						Trk::ExCellCharged& eCell,
						const Trk::Material* mat=0) const=0;
	 
       /** Process simulation */
       virtual ISF::ISFParticleVector  interact(const ISF::ISFParticle* isp,
						Trk::ExCellNeutral& eCell,
						const Trk::Material* mat=0) const=0;
  };

} // end of namespace

#endif 

