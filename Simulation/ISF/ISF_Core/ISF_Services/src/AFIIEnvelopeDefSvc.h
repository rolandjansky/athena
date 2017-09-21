/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AFIIEnvelopeDefSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef AFIIENVELOPEDEFSVC_H
#define AFIIENVELOPEDEFSVC_H

// STL includes
#include <string>
#include <vector>
#include <utility>

// GaudiKernel & Athena
#include "AthenaBaseComps/AthService.h"

// interface header file
#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h"

namespace ISF {

  class AFIIEnvelopeDefSvc : public extends<AthService, IEnvelopeDefSvc> {

    public:
      /** public AthService constructor */
      AFIIEnvelopeDefSvc(const std::string& name, ISvcLocator* svc);

      /** Destructor */
      ~AFIIEnvelopeDefSvc();

      /** AthService initialize method.*/
      StatusCode initialize();
      /** AthService finalize method */
      StatusCode finalize();

      /** return a vector of (r,z) pairs, defining the respective envelope */
      const RZPairVector& getRZBoundary( AtlasDetDescr::AtlasRegion region ) const;

      /** return a vector of (r,z) pairs, defining the envelope on the z>0 region */
      const RZPairVector &getRPositiveZBoundary( AtlasDetDescr::AtlasRegion region ) const;

    private:
      /** return boundary with shifted z values */
      RZPairVector getShiftedBoundary( AtlasDetDescr::AtlasRegion region, double shiftFromZ, double shiftToZ ) const;

      /** ServiceHandle to the standard ISF envelope definition service */
      ServiceHandle<IEnvelopeDefSvc>            m_isfEnvDefSvc;

      /** internal tolerance on coordinates */
      double                                    m_tolerance;

      /** maximum desired extent (halfz) of the modified inner detector volume */
      double                                    m_idMaxExtentZ;

      /** internal (r,z) representation for BeamPipe, InnerDetector and calo volumes */
      RZPairVector                              m_rzBeamPipe;
      RZPairVector                              m_rzInDet;
      RZPairVector                              m_rzCalo;
      /** internal (r,z) representation for the positive z-side only,
       *  one RZPairVector for BeamPipe and InnerDetector each */
      RZPairVector                              m_rposzBeamPipe;
      RZPairVector                              m_rposzInDet;
      RZPairVector                              m_rposzCalo;

  };


} // namespace ISF

#endif // AFIIENVELOPEDEFSVC_H

