// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TDirectoryReset.h 597264 2014-05-15 15:52:37Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TDIRECTORYRESET_H
#define XAODROOTACCESS_TOOLS_TDIRECTORYRESET_H

// ROOT include(s):
#include <TDirectory.h>

namespace xAOD {

   /// Helper class for making sure the current directory is preserved
   ///
   /// This class is used in the code to make sure that whenever a function
   /// call returns to the user, the current directory (gDirectory) would
   /// be the same as it was before the function call. Otherwise the user
   /// can have a few surprises...
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 597264 $
   /// $Date: 2014-05-15 17:52:37 +0200 (Thu, 15 May 2014) $
   ///
   class TDirectoryReset {

   public:
      /// Constructor with the current directory
      TDirectoryReset( ::TDirectory* dir = gDirectory ) : m_dir( dir ) {}
      /// Destructor
      ~TDirectoryReset() { if( m_dir ) m_dir->cd(); }

   private:
      /// The directory that we need to return to
      ::TDirectory* m_dir;

   }; // class TDirectoryReset

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TDIRECTORYRESET_H
