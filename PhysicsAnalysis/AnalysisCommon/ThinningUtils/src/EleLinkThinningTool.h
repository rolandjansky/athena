#ifndef ThinningUtils_EleLinkThinningTool_H
#define ThinningUtils_EleLinkThinningTool_H

// package includes
#include "EleLinkThinningHelpers.h"

// athena includes
#include "DerivationFrameworkInterfaces/IForwardParentThinningTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainersInterfaces/AuxTypes.h"

// STL
#include <stack>

// forward declare
namespace SG {
  class AuxVectorData;
}

/**
 * @class EleLinkThinningTool
 * @brief Class for applying thinning by following element links from objects
 * kept in the parent container.
 *
 * @author Jon Burr
 */
class EleLinkThinningTool 
  : virtual public IForwardParentThinningTool,
            public AthAlgTool
{
  public:
    EleLinkThinningTool(
        const std::string& type,
        const std::string& name,
        const IInterface* parent);

    ~EleLinkThinningTool();

    StatusCode initialize() override;

    StatusCode forwardParentThinning(
        const xAOD::IParticleContainer* parent,
        const std::vector<bool>& mask) const override;
  private:
    // Properties
    /// The thinning service
    ServiceHandle<IThinningSvc> m_thinningSvc;

    /// The name of the link to follow
    std::string m_linkName;

    /// Whether to apply the AND operator in thinning
    bool m_and;

    /// The maximum number of particles to thin in the final layer
    std::size_t m_maxThin;
    // Internals
    /// The helper objects. Mutable as it is set only on the first execute.
    mutable EleLinkThinningHelpers::ThinningHelperVec m_helperVec;

    /// Function which creates the helpers.
    StatusCode makeHelpers() const;
};
  
#endif //> !ThinningUtils_EleLinkThinningTool_H
