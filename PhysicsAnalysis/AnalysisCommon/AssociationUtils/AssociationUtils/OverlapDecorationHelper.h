#ifndef ASSOCIATIONUTILS_OVERLAPDECORATIONHELPER_H
#define ASSOCIATIONUTILS_OVERLAPDECORATIONHELPER_H

// EDM includes
#include "xAODBase/IParticleContainer.h"

// Local includes
#include "AssociationUtils/OverlapRemovalDefs.h"

namespace ORUtils
{

  /// @class OverlapDecorationHelper
  /// @brief Encapsulates the code needed to access and set overlap-related
  /// decorations.
  ///
  /// This utility class is used by the OverlapTools, but in principle could
  /// probably be used by a user as well.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class OverlapDecorationHelper
  {

    public:

      /// @brief Constructor
      ///
      /// @param inputLabel Input decoration name
      /// @param outputLabel Output decoration name
      /// @param outputPassValue Specifies the boolean value to assign to
      ///        passing, or non-overlapping objects. Default value is false
      ///        for historical reasons.
      OverlapDecorationHelper(const std::string& inputLabel,
                              const std::string& outputLabel,
                              bool outputPassValue = false);

      /// Check if object is flagged as input for OR
      bool isInputObject(const xAOD::IParticle& obj) const;

      /// Check if an object has been rejected by decoration
      bool isRejectedObject(const xAOD::IParticle& obj) const;

      /// Check if object is surviving OR thus far
      bool isSurvivingObject(const xAOD::IParticle& obj) const;

      /// Get the user priority score, which is currently the input decoration
      char getObjectPriority(const xAOD::IParticle& obj) const;

      /// Set output decoration on object, pass or fail
      void setOverlapDecoration(const xAOD::IParticle& obj, bool result);

      /// Shorthand way to set an object as passing overlap removal
      void setObjectPass(const xAOD::IParticle& obj);

      /// Shorthand way to set an object as failing overlap removal
      void setObjectFail(const xAOD::IParticle& obj);

      /// Check if output decoration has been applied to a container.
      /// Returns false if the container is empty.
      /// Output logic independent.
      bool isDecorated(const xAOD::IParticleContainer& container) const;

      /// Initialize decorations for a container to "pass".
      /// Note that the value written depends on the output pass-value.
      void initializeDecorations(const xAOD::IParticleContainer& container);

      /// Helper method for setting all objects as passing
      void resetDecorations(const xAOD::IParticleContainer& container);

    private:

      /// Toggle usage of input label
      bool m_useInputLabel;

      /// Input label accessor
      ort::inputAccessor_t m_inputAccessor;
      /// Output decorator
      ort::outputDecorator_t m_outputDecorator;

      /// Output decoration logic
      bool m_outputPassValue;

  }; // class OverlapDecorationHelper

} // namespace ORUtils

#endif
