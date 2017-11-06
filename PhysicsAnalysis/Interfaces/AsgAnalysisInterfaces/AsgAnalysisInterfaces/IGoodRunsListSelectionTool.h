/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GoodRunsLists_IGoodRunsListSelectionTool_H
#define GoodRunsLists_IGoodRunsListSelectionTool_H

// System include(s):
#include <vector>
#include <string>

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

// Forward declaration(s):
namespace Root {
   class TGRLCollection;
}

///Main tools implementing this interface:
///  GoodRunsListSelectionTool   (source code: DataQuality/GoodRunsLists)

class IGoodRunsListSelectionTool : virtual public asg::IAsgTool {

   ASG_TOOL_INTERFACE( IGoodRunsListSelectionTool )

public:
   /// Check if the current event passes the selection
   virtual bool passRunLB( const std::vector< std::string >& grlnameVec =
                              std::vector< std::string >(),
                           const std::vector< std::string >& brlnameVec =
                              std::vector< std::string >() ) const = 0;

   /// Check if the event specified passes the selection
   virtual bool passRunLB( const xAOD::EventInfo& event,
                           const std::vector< std::string >& grlnameVec =
                              std::vector< std::string >(),
                           const std::vector< std::string >& brlnameVec =
                              std::vector< std::string >() ) const = 0;

   /// Check if the event specified passes the selection
   virtual bool passRunLB( int runNumber, int lumiBlockNr,
                           const std::vector< std::string >& grlnameVec =
                              std::vector< std::string >(),
                           const std::vector< std::string >& brlnameVec =
                              std::vector< std::string >() ) const = 0;

   /// Get the good runs list used by the tool
   virtual const Root::TGRLCollection& getGRLCollection() const = 0;

   /// Get the bad runs list used by the tool
   virtual const Root::TGRLCollection& getBRLCollection() const = 0;

}; // class IGoodRunsListSelectionTool

#endif // GoodRunsLists_IGoodRunsListSelectionTool_H
