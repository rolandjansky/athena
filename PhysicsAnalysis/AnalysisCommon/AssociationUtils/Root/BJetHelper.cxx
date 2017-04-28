// Local includes
#include "AssociationUtils/BJetHelper.h"

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  BJetHelper::BJetHelper(const std::string& bJetLabel)
    : m_bJetAccessor(bJetLabel)
  {}

  //---------------------------------------------------------------------------
  // Determine if jet is flagged as a bjet
  //---------------------------------------------------------------------------
  bool BJetHelper::isBJet(const xAOD::Jet& jet) const
  {
    return m_bJetAccessor(jet);
  }

} // namespace ORUtils
