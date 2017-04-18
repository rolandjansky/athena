// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IEfficiencyScaleFactorTool.h 790193 2016-12-16 16:36:15Z krasznaa $
#ifndef ASGANALYSISINTERFACES_IEFFICIENCYSCALEFACTORTOOL_H
#define ASGANALYSISINTERFACES_IEFFICIENCYSCALEFACTORTOOL_H

// ASG include(s):
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/CorrectionCode.h"

// Forward declaration(s):
namespace xAOD {
   class IParticle;
}

/// Namespace for generic Combined Performance code
namespace CP {

   /// General interface for tools providing analysis efficiency scale factors
   ///
   /// This is a generic interface that should be implemented by all analysis
   /// tools that provide efficiency scale factors for @c xAOD::IParticle
   /// type objects.
   ///
   /// @note If your tool needs type specific variables that the
   /// @c xAOD::IParticle interface doesn't provide, please rely on the
   /// @c xAOD::IParticle::type() function to identify whether you received
   /// an appropriate object. And then use @c static_cast to cast the reference
   /// to the right type. Like:
   ///
   /// @code{.cpp}
   /// if( p.type() != xAOD::Type::Electron ) {
   ///    ATH_MSG_ERROR( "Object of wrong type (" << p.type() << ") received" );
   ///    return CP::CorrectionCode::Error;
   /// }
   /// const xAOD::Electron& el = static_cast< const xAOD::Electron& >( p );
   /// @endcode
   ///
   /// Just be @b very careful about writing the code correctly, as a wrong
   /// @c static_cast could lead to memory corruption. But since its much
   /// faster than @c dynamic_cast, it's still recommended to use this sort of
   /// code.
   ///
   /// $Revision: 790193 $
   /// $Date: 2016-12-16 17:36:15 +0100 (Fri, 16 Dec 2016) $
   ///
   class IEfficiencyScaleFactorTool : virtual public ISystematicsTool {

      // Declare the interface of the tool
      ASG_TOOL_INTERFACE( CP::IEfficiencyScaleFactorTool )

   public:
      /// Default destructor
      virtual ~IEfficiencyScaleFactorTool() = default;

      /// Retrieve an efficiency scale factor for a particle type object
      ///
      /// This function is used to retrieve the efficiency scale factor for a
      /// specific particle object.
      ///
      /// @param p The particle for which a scale factor is to be calculated
      /// @param sf A reference to the scale factor variable to be set
      /// @return @c CP::CorrectionCode::Error in case of a serious problem,
      ///         @c CP::CorrectionCode::OutOfValidityRange if no scale factor
      ///         is available for the received object, or
      ///         @c CP::CorrectionCode::Ok if the scale factor could be
      ///         correctly provided
      ///
      virtual CorrectionCode getEfficiencyScaleFactor( const xAOD::IParticle& p,
                                                       double& sf ) const = 0;

      /// Decorate a particle with its efficiency scale factor
      ///
      /// This function can be used to decorate a particle type object with
      /// an efficiency scale factor. The name and type of the scale factor
      /// variable is up to the specific tool implementation.
      ///
      /// @param p The particle for which a scale factor is to be calculated
      /// @return @c CP::CorrectionCode::Error in case of a serious problem,
      ///         @c CP::CorrectionCode::OutOfValidityRange if no scale factor
      ///         is available for the received object, or
      ///         @c CP::CorrectionCode::Ok if the scale factor could be
      ///         correctly provided
      ///
      virtual CorrectionCode
      applyEfficiencyScaleFactor( const xAOD::IParticle& p ) const = 0;

   }; // class IEfficiencyScaleFactorTool

} // namespace CP

#endif // ASGANALYSISINTERFACES_IEFFICIENCYSCALEFACTORTOOL_H
