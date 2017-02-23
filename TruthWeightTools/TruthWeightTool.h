// $Id$
#ifndef TRUTHWEIGHTTOOLS_TRUTHWEIGHTTOOL_H
#define TRUTHWEIGHTTOOLS_TRUTHWEIGHTTOOL_H

//STL includes
#include <memory>

// Framework include(s):
#include "AsgTools/AsgMetadataTool.h"

// Local include(s):
#include "TruthWeightTools/ITruthWeightTool.h"

#include "xAODTruth/TruthMetaDataContainer.h"
#include "xAODTruth/TruthMetaData.h"

namespace xAOD {

   /// Implementation for the xAOD truth meta data weight tool
   ///
   /// @author Tobias Bisanz  <tobias.bisanz@cern.ch>
   ///
   /// $Revision$
   /// $Date$
   ///
   class TruthWeightTool : public virtual ITruthWeightTool,
                                     public asg::AsgMetadataTool {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS( TruthWeightTool, xAOD::ITruthWeightTool )

   public:
      /// Create a constructor for standalone usage
      TruthWeightTool( const std::string& name = "xAOD::TruthWeightTool");

      /// Default dtor
      ~TruthWeightTool() = default;

      /// @name Function(s) implementing the asg::IAsgTool interface
      /// @{

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// @}

      /// @name Function(s) implementing the ITruthWeightTool interface
      /// @{
      
      /// Create an IndexRetriever 
      virtual std::shared_ptr<IIndexRetriever> spawnIndexRetriever(std::string weightName);

      /// Return weight index
      virtual int getWeightIndex(std::string weightName);

      /// Get a vector with all the currently weight names in meta data
      virtual std::vector<std::string> const & getWeightNames() const;
      /// @}

   protected:
      /// @name Callback function(s) from AsgMetadataTool
      /// @{

      /// Function called when a new input file is opened
      virtual StatusCode beginInputFile();

      /// Function called when a new event is loaded
      virtual StatusCode beginEvent();

      /// @}
      
      virtual void onNewMetaData();

      /// Stores the meta data record name
      std::string m_metaName;
      /// Ptr to the meta data container
      xAOD::TruthMetaDataContainer const * m_metaDataContainer;
      /// Ptr to the currently valid meta data object
      xAOD::TruthMetaData const * m_metaData; 

      /// Vector of all spawned IndexRetrievers
      std::map<std::string, std::weak_ptr<IIndexRetriever> > m_indexRetrievers;
  
      /// Previous MC channel number
      uint32_t m_mcChanNo;

      /// Flag to check if we actaully processed 
      /// a previous event
      bool m_uninitialized;

   };

} // namespace xAOD

#endif
