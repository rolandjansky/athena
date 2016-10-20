// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTILECORRECTION_IJETTILECORRECTIONTOOL_H
#define JETTILECORRECTION_IJETTILECORRECTIONTOOL_H

// Framework include(s):
#include "PATInterfaces/ISystematicsTool.h"

// EDM include(s):
#include "xAODJet/Jet.h"

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"

namespace CP {

   /// Interface for the tool correcting for dead Tile modules
   ///
   /// Documentation to be written...
   ///
   /// @author Martin Tripiana <tripiana@cern.ch>
   ///
   /// $Revision$
   /// $Date$
   ///
   class IJetTileCorrectionTool : public virtual ISystematicsTool {

      /// Declare the interface that the class provides
      ASG_TOOL_INTERFACE( CP::IJetTileCorrectionTool )

   public:
      /// Apply the correction on a modifyable object
      virtual CorrectionCode applyCorrection( xAOD::Jet& jet ) = 0;

      /// Create a corrected copy from a constant jet
      virtual CorrectionCode correctedCopy( const xAOD::Jet& input,
                                            xAOD::Jet*& output ) = 0;

   }; // class IJetTileCorrectionTool

} // namespace CP

#endif // JETTILECORRECTION_IJETTILECORRECTIONTOOL_H
