#ifndef ASSOCIATIONUTILS_IOBJECTASSOCIATOR_H
#define ASSOCIATIONUTILS_IOBJECTASSOCIATOR_H

// EDM includes
#include "xAODBase/IParticle.h"

namespace ORUtils
{

  /// @class IParticleAssociator
  /// @brief Interface for a class which checks for a match between IParticles.
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IParticleAssociator
  {
    public:
      /// Virtual destructor
      virtual ~IParticleAssociator(){};

      /// Test association between two IParticles
      virtual bool objectsMatch(const xAOD::IParticle& p1,
                                const xAOD::IParticle& p2) const = 0;
  };

} // namespace ORUtils

#endif
