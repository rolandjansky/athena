/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ThinningUtils_EleLinkThinningTool_H
#define ThinningUtils_EleLinkThinningTool_H

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
namespace EleLinkThinningHelpers {
  class ThinningHelperBase;
}
/**
 * @class EleLinkThinningTool
 * @brief Class for applying thinning by following element links from objects
 * kept in the parent container.
 *
 * The most property here is LinkName and it is quite complicated. The tool is
 * able to follow chained element links (retrieve an element from the first
 * link, then retrieve a further element from *that* link). Each element link in
 * the chain is represented by a single helper class. These helper classes are
 * also responsible for performing the thinning of the received elements.
 * Helpers are equally able to represent ElementLink<X> and
 * std::vector<ElementLink<X>>.
 *
 * The helpers are defined by 6 properties:
 *    - auxName: The name of auxID corresponding to the ElementLink.
 *    - containerList: A list of allowed containers that the ElementLink can
 *      point to.
 *    - op: The thinning service operator to apply.
 *    - doThin: Whether thinning should be applied to the elements processed by
 *      this helper. This is true iff there are elements in the containerList.
 *    - maxToPass: The maximum number of element links that this helper should
 *      process.
 *    - previous: The previous helper in the chain.
 *
 * The list of allowed containers is used to ensure that the pointed to
 * containers are thinned on every event even if there are no valid links.
 * maxToPass allows ensuring that the helper processes at most a certain number
 * of elements.
 *
 * These are provided via the LinkName property. Each link in the chain is
 * represented by a string matching the pattern
 * <auxName>(list,of,containers)[maxToPass] where both the container list and
 * maxToPass are optional (if they are missing they will an empty list and the
 * maximum value of std::size_t respectively). These patterns are then chained
 * together using '.' characters.
 *
 * For example, to thin GSFTrackParticles and InDetTrackParticles to keep all
 * that come from the first conversion vertex of a photon the LinkName property
 * would be:
 * vertexLinks[1].trackParticleLinks(GSFTrackParticles).originalTrackParticle(InDetTrackParticles)
 * Note that this pattern would not keep the conversion itself!
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

    // Internals
    /// The helper objects. Mutable as it is set only on the first execute.
    mutable std::unique_ptr<EleLinkThinningHelpers::ThinningHelperBase> m_helper;

    /// Function which creates the helpers.
    StatusCode makeHelper() const;
};
  
#endif //> !ThinningUtils_EleLinkThinningTool_H
