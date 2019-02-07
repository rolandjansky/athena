/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



// Dear emacs, this is -*- c++ -*-
// $Id: CorrectionTool.h 600738 2014-06-07 14:34:05Z krasznaa $
#ifndef PATINTERFACES_CORRECTIONTOOL_H
#define PATINTERFACES_CORRECTIONTOOL_H

#include "PATInterfaces/CorrectionCode.h"

namespace CP {

   /// Helper base class for "correction tools"
   ///
   /// This class is provided to make the coding of CP tools that apply some
   /// modification to objects a little easier. The tool needs to provide a
   /// separate, proper interface class. This class will just provide the
   /// implementation of the functions defined in that interface. So the CP
   /// tool itself will look like:
   ///
   /// <code>
   /// class MySmearingTool : public virtual IMySmearingTool,<br/>
   ///                        public virtual CP::CorrectionTool< MyContainerType >,<br/>
   ///                        public asg::AsgTool {<br/>
   ///    ...<br/>
   /// };
   /// </code>
   ///
   /// The idea is that the user only needs to implement one function in the
   /// final tool in order to implement the interface outlined in
   /// https://cds.cern.ch/record/1667206
   ///
   /// @author Nils Krumnack <krumnack@iastate.edu>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 600738 $
   /// $Date: 2014-06-07 16:34:05 +0200 (Sat, 07 Jun 2014) $
   ///
   template< class T >
   class CorrectionTool {

   public:
      /// description: the type of xAOD container used
      typedef T xAODContainerType;
      /// description: the type of xAOD object used
      typedef typename xAODContainerType::base_value_type xAODObjectType;

      /// effects: standard destructor
      /// rationale: virtual destruct for base class
      virtual ~CorrectionTool() {}

      /// effects: apply the correction and store the output in the object
      /// returns: success
      virtual CorrectionCode
      applyCorrection( xAODObjectType& inputObject ) = 0;

      /// effects: make a clone of the input object, apply the
      ///   correction to it and store the output in the object
      /// returns: success
      /// rationale: the interface class provides a default
      ///   implementation in terms of applyCorrection, that the derived
      ///   classes can override for efficiency
      CorrectionCode
      correctedCopy( const xAODObjectType& inputObject,
                     xAODObjectType*& outputObject );

      /// effects: apply the correction to all the members of the
      ///   container
      /// returns: overall success
      /// rationale: this will apply the correction to every single
      ///   member (even if some members fail)
      /// rationale: the interface class provides a default
      ///   implementation in terms of applyCorrection, that the derived
      ///   classes can override for efficiency
      CorrectionCode
      applyContainerCorrection( xAODContainerType& inputContainer );

   }; // class CorrectionTool

} // namespace CP

// Include the implementation:
#include "PATInterfaces/CorrectionTool.icc"

#endif // PATINTERFACES_CORRECTIONTOOL_H
