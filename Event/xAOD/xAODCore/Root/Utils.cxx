/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Utils.cxx 672864 2015-06-05 16:55:45Z ssnyder $

// System include(s):
#include <cmath>
#include <sstream>

// Local include(s):
#include "xAODCore/tools/Utils.h"

namespace {

   /// Simple structure describing an elapsed amount of time
   ///
   /// In order to print some elapsed times in a nice way, the
   /// private functions of this source file use this structure.
   ///
   /// The amount of times measured by the code should be
   /// representable by this structure. (We shouldn't care about
   /// sub-milisecond times, or longer running periods than a
   /// few days...)
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 672864 $
   /// $Date: 2015-06-05 18:55:45 +0200 (Fri, 05 Jun 2015) $
   ///
   struct TimeStruct {
      int miliseconds; ///< Elapsed milisecods
      int seconds; ///< Elapsed seconds
      int minutes; ///< Elapsed minutes
      int hours; ///< Elapsed hours
      int days; ///< Elapsed days
   }; // struct TimeStruct

   /// Function creating a time structure
   ///
   /// This function is used to break down a simple elapsed time expressed in
   /// seconds into an easy-to-print structure. Shame that I couldn't find
   /// something in standard C/C++ to do it...
   ///
   /// @param secs The elapsed time expressed in seconds
   /// @returns A structure describing the elapsed time
   ///
   TimeStruct timeToStruct( ::Double_t secs ) {

      // Create the structure, fill its miliseconds variable,
      // and reset all the rest:
      TimeStruct result = {
         static_cast< ::Int_t >( std::fmod( secs, 1.0 ) * 1000.0 ),
         0, 0, 0, 0 };

      // If the elapsed time was less than a second, finish here:
      secs -= ( result.miliseconds * 1e-3 );
      if( std::abs( secs ) < 0.5 ) return result;

      // Calculate the number of seconds passed, and finish if the
      // amount of time passed was less than a minute:
      result.seconds =
         static_cast< ::Int_t >( std::fmod( secs, 60.0 ) );
      secs -= result.seconds;
      if( std::abs( secs ) < 0.5 ) return result;

      // Calculate the number of minutes passed, and finish if the
      // amount of time passed was less than an hour:
      result.minutes =
         static_cast< ::Int_t >( std::fmod( secs, 3600.0 ) * (1./60.) );
      secs -= result.minutes * 60.0;
      if( std::abs( secs ) < 0.5 ) return result;

      // Calculate the number of hours passed, and finish if the
      // amount of time passed was less than a day:
      result.hours =
         static_cast< ::Int_t >( std::fmod( secs, 86400.0 ) * (1./3600.) );
      secs -= result.hours * 3600.0;
      if( std::abs( secs ) < 0.5 ) return result;

      // Calculate the number of days passed. The function should
      // not expect to have to express a larger order of magnitude...
      result.days = static_cast< ::Int_t >( secs * (1./86400.));

      return result;
   }

} // private namespace

namespace xAOD {

   /// Since I wasn't able to find a nice function printing elapsed times
   /// in a human-readable format, I ended up writing one. This function
   /// is used in printing the statistics about an analysis.
   ///
   /// @param secs An amount of time passed, expressed in seconds
   /// @returns A formatted, human-readable version of the amount of time passed
   ///
   std::string Utils::timeToString( ::Double_t secs ) {

      // Do meat of the calculation:
      const TimeStruct ts = timeToStruct( secs );

      // Construct the string:
      std::ostringstream result;
      if( ts.days ) {
         result << ts.days << "d ";
      }
      if( ts.hours || ts.days ) {
         result << ts.hours << "h ";
      }
      if( ts.minutes || ts.hours || ts.days ) {
         result << ts.minutes << "m ";
      }
      if( ts.seconds || ts.minutes || ts.hours || ts.days ) {
         result << ts.seconds << "s ";
      }
      result << ts.miliseconds << "ms";

      return result.str();
   }

   /// This function is used to produce nicely readable printouts for
   /// amounts of data.
   ///
   /// @param bytes The amount of data expressed in bytes
   /// @returns A human-readable printout of the data size
   ///
   std::string Utils::sizeToString( ::Long64_t bytes ) {

      std::ostringstream result;

      if( std::abs( bytes ) > 1e12 ) {
         result << ( bytes * 1e-12 ) << " TB";
      } else if( std::abs( bytes ) > 1e9 ) {
         result << ( bytes * 1e-9 ) << " GB";
      } else if( std::abs( bytes ) > 1e6 ) {
         result << ( bytes * 1e-6 ) << " MB";
      } else if( std::abs( bytes ) > 1e3 ) {
         result << ( bytes * 1e-3 ) << " kB";
      } else {
         result << bytes << " bytes";
      }

      return result.str();
   }

   /// @param bytespersec The speed expressed in bytes / seconds
   /// @returns A human-readable printout of the data processing speed
   ///
   std::string Utils::speedToString( ::Double_t bytespersec ) {

      std::ostringstream result;

      if( ::fabs( bytespersec ) > 1e12 ) {
         result << ( bytespersec * 1e-12 ) << " TB/s";
      } else if( ::fabs( bytespersec ) > 1e9 ) {
         result << ( bytespersec * 1e-9 ) << " GB/s";
      } else if( ::fabs( bytespersec ) > 1e6 ) {
         result << ( bytespersec * 1e-6 ) << " MB/s";
      } else if( ::fabs( bytespersec ) > 1e3 ) {
         result << ( bytespersec * 1e-3 ) << " kB/s";
      } else {
         result << bytespersec << " B/s";
      }

      return result.str();
   }

} // namespace D3PDReader
