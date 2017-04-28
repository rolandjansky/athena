#ifndef ASSOCIATIONUTILS_IOVERLAPTOOL_H
#define ASSOCIATIONUTILS_IOVERLAPTOOL_H

// Framework includes
#include "AsgTools/IAsgTool.h"

// EDM includes
#include "xAODBase/IParticleContainer.h"


namespace ORUtils
{

  /// @class IOverlapTool
  /// @brief Interface class for overlap removal tools.
  ///
  /// Tools that implement this interface will operate on particle containers
  /// and find and mark overlaps based on their custom logic.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IOverlapTool : virtual public asg::IAsgTool
  {

      /// Declare the interface
      ASG_TOOL_INTERFACE(IOverlapTool)

    public:

      /// @brief Find overlaps between two containers.
      /// The details will depend on the tool implementation, but the convention
      /// should be that if only one container is to be marked, it should be the
      /// first argument. Otherwise, both of them can be marked. The decoration
      /// logic may depend on configuration.
      virtual StatusCode findOverlaps(const xAOD::IParticleContainer& cont1,
                                      const xAOD::IParticleContainer& cont2) const = 0;

  }; // class IOverlapTool

} // namespace ORUtils

#endif
