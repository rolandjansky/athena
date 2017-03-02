#ifndef ASSOCIATIONUTILS_OBJLINKOVERLAPTOOL_H
#define ASSOCIATIONUTILS_OBJLINKOVERLAPTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"

// Local inlcudes
#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/BaseOverlapTool.h"

namespace ORUtils
{

  /// @class ObjLinkOverlapTool
  /// @brief A generic tool which finds overlaps using ElementLinks.
  ///
  /// Allows to relate objects upstream (like when making derivations when
  /// all the information is available) and reuse the relation later when
  /// applying the full analysis selections. One expected use-case is for the
  /// Electron-Muon shared track matching.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class ObjLinkOverlapTool : public virtual IOverlapTool,
                             public BaseOverlapTool
  {

      /// Create proper constructor for Athena
      ASG_TOOL_CLASS(ObjLinkOverlapTool, IOverlapTool)

    public:

      /// Standalone constructor
      ObjLinkOverlapTool(const std::string& name);

      /// @brief Identify overlaps by ElementLink.
      /// Flags objects in cont1 which are linked to objects in cont2.
      virtual StatusCode
      findOverlaps(const xAOD::IParticleContainer& cont1,
                   const xAOD::IParticleContainer& cont2) const override;

    protected:

      /// Initialize the tool
      virtual StatusCode initializeDerived() override;

    private:

      //
      // Configurable properties
      //

      //
      // Utilities
      //

      /// Helper for reading object links
      std::unique_ptr<OverlapLinkHelper> m_objLinkHelper;

  }; // class ObjLinkOverlapTool

} // namespace ORUtils

#endif
