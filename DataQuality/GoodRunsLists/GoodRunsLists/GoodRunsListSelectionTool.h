/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
// Dear emacs, this is -*- c++ -*-
#ifndef GoodRunsLists_GoodRunsListSelectionTool_H
#define GoodRunsLists_GoodRunsListSelectionTool_H

// System include(s):
#include <vector>
#include <string>

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Local include(s):
#include "AsgAnalysisInterfaces/IGoodRunsListSelectionTool.h"
#include "GoodRunsLists/TGRLCollection.h"

/// Tool implementing the GRL selection
///
/// This tool needs to be used to select good lumiblocks for a physics
/// or performance analysis.
///
/// $Revision$
/// $Date$
///
class GoodRunsListSelectionTool : virtual public IGoodRunsListSelectionTool,
      public asg::AsgTool {

   // Declare the proper constructor(s) for Athena:
   ASG_TOOL_CLASS( GoodRunsListSelectionTool, IGoodRunsListSelectionTool )

public:
   GoodRunsListSelectionTool( const std::string& name =
         "GoodRunsListsSelectionTool" );

   /// Initialize AlgTool
   virtual StatusCode initialize();

   /// @name Interface inherited from IGoodRunsListSelectionTool
   /// @{

   /// Check if the current event passes the selection
   virtual bool passRunLB( const std::vector< std::string >& grlnameVec =
                              std::vector< std::string >(),
                           const std::vector< std::string >& brlnameVec =
                              std::vector< std::string >() ) const;

   /// Check if the event specified passes the selection
   virtual bool passRunLB( const xAOD::EventInfo& event,
                           const std::vector< std::string >& grlnameVec =
                              std::vector< std::string >(),
                           const std::vector< std::string >& brlnameVec =
                              std::vector< std::string >() ) const;

   /// Check if the event specified passes the selection
   virtual bool passRunLB( int runNumber, int lumiBlockNr,
                           const std::vector< std::string >& grlnameVec =
                              std::vector< std::string >(),
                           const std::vector< std::string >& brlnameVec =
                              std::vector< std::string >() ) const;

   /// Get the good runs list used by the tool
   virtual const Root::TGRLCollection& getGRLCollection() const {
      return m_grlcollection;
   }

   /// Get the bad runs list used by the tool
   virtual const Root::TGRLCollection& getBRLCollection() const {
      return m_brlcollection;
   }

   /// @}

protected:
   /// Helper function reading in the specified files into a GRL object
   StatusCode readXMLs( Root::TGRLCollection& grl,
                        const std::vector< std::string >& files );

   std::vector< std::string > m_goodrunslistVec;
   std::vector< std::string > m_blackrunslistVec;

   Root::TGRLCollection m_grlcollection;
   Root::TGRLCollection m_brlcollection;

   int  m_boolop;
   bool m_passthrough;
   bool m_rejectanybrl;

}; // class GoodRunsListSelectionTool

#endif // GoodRunsLists_GoodRunsListSelectionTool_H
