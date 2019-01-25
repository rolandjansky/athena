/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ThinningUtils_ThinAssociatedObjectsTool_H
#define ThinningUtils_ThinAssociatedObjectsTool_H

// athena includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IThinningSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/ExpressionParserHelper.h"

// STL includes
#include <memory>

// Forward declarations
class IForwardParentThinningTool;
namespace DerivationFramework {
  class ExpressionParserHelper;
}


/**
 * @class ThinAssociatedObjectsTool
 * @brief Tool to thin objects associated to a container.
 *        Often we keep low-level objects only because they are necessary for
 *        operations on some high-level object. If we thin out the high-level
 *        object we want to propagate this decision to the low-level objects as
 *        well.
 *
 *        Throughout this class the 'parent' container is the high-level object
 *        from which the thinning is set. The 'children' are the low-level
 *        containers which receive the thinning.
 *
 * @author Jon Burr
 */
class ThinAssociatedObjectsTool 
  : virtual public ::DerivationFramework::IThinningTool,
            public ::AthAlgTool
{
  public:
    /// Standard constructor
    ThinAssociatedObjectsTool( const std::string& type,
                               const std::string& name,
                               const IInterface* parent );

    /// Standard destructor
    virtual ~ThinAssociatedObjectsTool() {}

    /// Initialize the tool
    StatusCode initialize() override;

    /// Perform the specified thinning
    StatusCode doThinning() const override;
  private:
    // Properties

    /// The thinning service
    ServiceHandle<IThinningSvc> m_thinningSvc;

    /// The key to the parent container
    std::string m_sgKey;

    /// The selection string to use
    std::string m_selectionString;

    /// The child thinning tools
    ToolHandleArray<IForwardParentThinningTool> m_childTools;

    // TODO: Everywhere these ExpressionParsers seem to be created dynamically
    // but I can't quite see why they can't be made on the stack...
    /// The expression parser
    std::unique_ptr<DerivationFramework::ExpressionParserHelper> m_parser;

    /// Count the number of input objects seen
    mutable unsigned int m_nTotal{0};

    /// Count the number of input objects passing the selection. Note that this
    /// is incremented even if this tool isn't responsible for thinning this
    /// container.
    mutable unsigned int m_nPass{0};

};

#endif //> !ThinningUtils_ThinAssociatedObjectsTool_H
