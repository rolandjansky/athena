///////////////////////// -*- C++ -*- /////////////////////////////
// METSignificance.h
// Header file for class METSignificance
// Author: P. Francavilla<francav@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef METUTILITIES_MET_METSIGNIFICANCE_H
#define METUTILITIES_MET_METSIGNIFICANCE_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"

// METInterface includes
#include "METInterface/IMETSignificance.h"

// EDM includes
#include "xAODJet/JetContainer.h"

// Tracking Tool
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

// Forward declaration

namespace met {

  // typedefs
  typedef ElementLink<xAOD::IParticleContainer> obj_link_t;

  class METSignificance
  : public asg::AsgTool,
  virtual public IMETSignificance

  {
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METSignificance, IMETSignificance)

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    // Copy constructor:

    /// Constructor with parameters:
    METSignificance(const std::string& name);

    /// Destructor:
    virtual ~METSignificance();

    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
      
    StatusCode varianceMET(xAOD::MissingETContainer* metCont);


    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
  private:

    /// Default constructor:
    METSignificance();

  };

} //> end namespace met
#endif //> !METUTILITIES_MET_METSIGNIFICANCE_H
