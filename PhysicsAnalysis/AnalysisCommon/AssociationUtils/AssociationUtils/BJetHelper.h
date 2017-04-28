#ifndef ASSOCIATIONUTILS_BJETHELPER_H
#define ASSOCIATIONUTILS_BJETHELPER_H

// EDM includes
#include "xAODJet/JetContainer.h"

// Local includes
#include "AssociationUtils/OverlapRemovalDefs.h"

namespace ORUtils
{

  /// @class BJetHelper
  /// @brief Helper class for b-jet decorations
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class BJetHelper
  {

    public:

      /// Constructor
      BJetHelper(const std::string& bJetLabel);

      /// Check if an object is labeled as a b-jet 
      bool isBJet(const xAOD::Jet& jet) const;

    private:

      /// BJet label accessor
      ort::inputAccessor_t m_bJetAccessor;

  }; // class BJetHelper

} // namespace ORUtils

#endif
