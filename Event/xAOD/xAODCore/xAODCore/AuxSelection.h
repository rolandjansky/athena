// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxSelection.h 583607 2014-02-17 11:02:08Z krasznaa $
#ifndef XAODCORE_AUXSELECTION_H
#define XAODCORE_AUXSELECTION_H

// System include(s):
#include <set>
#include <string>

// EDM include(s):
#include "AthContainersInterfaces/AuxTypes.h"

namespace xAOD {

   /// Class helping in dealing with dynamic branch selection
   ///
   /// This class is used by the code internally to select which of the
   /// dynamic branches should be written out into an output file.
   ///
   /// @author Marcin Nowak <Marcin.Nowak@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 583607 $
   /// $Date: 2014-02-17 12:02:08 +0100 (Mon, 17 Feb 2014) $
   ///
   class AuxSelection {

   public:
      /// Default constructor
      AuxSelection();

      /// Select which variables should be written out
      virtual void selectAux( const std::set< std::string >& attributes );
      /// Return which variables were selected to be written out
      virtual const SG::auxid_set_t&
      getSelectedAuxIDs( const SG::auxid_set_t& fullset ) const;

   protected:
      /// Properties following the variable selection convention
      std::set< std::string > m_names;
      /// Auxiliary IDs of the variables that were selected from the last list
      mutable SG::auxid_set_t m_auxids;

   }; // class AuxSelection

} // namespace xAOD

#endif // XAODCORE_AUXSELECTION_H
