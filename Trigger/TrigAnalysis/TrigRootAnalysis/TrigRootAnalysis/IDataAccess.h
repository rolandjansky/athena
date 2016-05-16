// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IDataAccess.h 726661 2016-02-28 01:18:32Z ssnyder $
#ifndef TRIGROOTANALYSIS_IDATAACCESS_H
#define TRIGROOTANALYSIS_IDATAACCESS_H

// STL include(s):
#include <vector>

// ROOT include(s):
#include <Rtypes.h>

namespace D3PD {

   // Forward declaration(s):
   class ChainGroup;

   namespace Trig {

      /**
       *  @short Interface providing access to the event-wise trigger data
       *
       *         In order to be able to split up the functionality of the TDT into
       *         multiple parts, the separate parts communicate with each other
       *         through such interfaces. This interface gives access to all the
       *         other components to the event-wise trigger data.
       *
       * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
       *
       * $Revision: 726661 $
       * $Date: 2016-02-28 02:18:32 +0100 (Sun, 28 Feb 2016) $
       */
      class IDataAccess {

         /// The ChainGroup class is a friend of ours...
         friend class D3PD::ChainGroup;

      public:
         /// Virtual destructor to make vtable happy
         virtual ~IDataAccess() {}

         /// Get the detail level that the D3PD was produced with
         virtual ::Int_t GetDetailLevel() = 0;

         /// Types of LVL1 result bits
         enum L1ResultType {
            TBP = 0,
            TAP = 1,
            TAV = 2
         };
         /// Types of HLT results
         enum HLTResultType {
            Physics       = 0,
            Raw           = 1,
            Resurrected   = 2,
            PassedThrough = 3
         };

         /// Get the Super Master Key of the current event
         virtual ::Int_t GetSMK() = 0;
         /// Get the LVL1 prescale key of the current event
         virtual ::Int_t GetL1PSK() = 0;
         /// Get the HLT prescale key of the current event
         virtual ::Int_t GetHLTPSK() = 0;

         /// Function for retrieving the encoded LVL1 result
         virtual const std::vector< unsigned int >*
         GetL1Result( L1ResultType type ) = 0;
         /// Function for retrieving the encoded LVL2 result
         virtual const std::vector< short >*
         GetL2Result( HLTResultType type ) = 0;
         /// Function for retrieving the encoded EF result
         virtual const std::vector< short >*
         GetEFResult( HLTResultType type ) = 0;

      }; // class IDataAccess

   } // namespace Trig

} // namespace D3PD

#endif // TRIGROOTANALYSIS_IDATAACCESS_H
