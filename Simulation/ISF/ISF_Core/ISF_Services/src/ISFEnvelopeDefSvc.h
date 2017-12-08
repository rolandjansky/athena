/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISFEnvelopeDefSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ISFENVELOPEDEFSVC_H
#define ISFENVELOPEDEFSVC_H

// STL includes
#include <string>
#include <vector>
#include <utility>

// GaudiKernel & Athena
#include "AthenaBaseComps/AthService.h"

// interface header file
#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h"

namespace ISF {

  class ISFEnvelopeDefSvc : public IEnvelopeDefSvc, virtual public AthService {

    public:
      /** public AthService constructor */
      ISFEnvelopeDefSvc(const std::string& name, ISvcLocator* svc);

      /** Destructor */
      ~ISFEnvelopeDefSvc();

      /** Query the interfaces. */
      StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

      /** AthService initialize method.*/
      StatusCode initialize();
      /** AthService finalize method */
      StatusCode finalize();

      /** return a vector of (r,z) pairs, defining the respective envelope */
      const RZPairVector& getRZBoundary( AtlasDetDescr::AtlasRegion region ) const;

      /** return a vector of (r,z) pairs, defining the envelope on the z>0 region */
      const RZPairVector &getRPositiveZBoundary( AtlasDetDescr::AtlasRegion region ) const;

    private:
      /** ServiceHandle to the common ATLAS envelope definition service */
      ServiceHandle<IEnvelopeDefSvc>            m_atlasEnvDefSvc;

      /** internal (r,z) representation for BeamPipe and InnerDetector volumes */
      RZPairVector                              m_rzBeamPipe;
      RZPairVector                              m_rzInDet;
      /** internal (r,z) representation for the positive z-side only,
       *  one RZPairVector for BeamPipe and InnerDetector each */
      RZPairVector                              m_rposzBeamPipe;
      RZPairVector                              m_rposzInDet;

  };


} // namespace ISF

#endif // ISFENVELOPEDEFSVC_H

