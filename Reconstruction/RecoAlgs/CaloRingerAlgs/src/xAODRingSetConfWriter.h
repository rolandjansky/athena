/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingSetConfWriter.h 713521 2015-12-09 08:53:41Z wsfreund $

#ifndef CALORINGERTOOLS_XAODCALORINGSSCONFWRITER_H
#define CALORINGERTOOLS_XAODCALORINGSSCONFWRITER_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "SGTools/ClassID_traits.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

// EDM include(s):
#include "xAODCaloRings/RingSetConfContainer.h"

namespace Ringer {

class ICaloRingsBuilder;

/**
*  @short Algorithm used to write the RingSets configuration
*
*         This algorithm needs to be scheduled in jobs that write xAOD files in
*         Athena in order to write the RingSet configuration into the output
*         file.
*
*         It assembles the RingSet configuration that is written into
*         the metadata TTree of the xAOD file at the end of the job.
*
* This algorithm is based on TrigConfxAOD from Attila Krasznahorkay.
*
* @author Werner Freund <wsfreund@cern.ch>
*
* $Revision: 713521 $
* $Date: 2015-12-09 09:53:41 +0100 (Wed, 09 Dec 2015) $
*/

class xAODRingSetConfWriter : public ::AthAlgorithm 
{

  public:
    /// Regular Algorithm constructor
    xAODRingSetConfWriter( const std::string& name, ::ISvcLocator* svcLoc );

    /// Method initialising the algorithm
    StatusCode initialize();

    /// Method executing the algorithm
    StatusCode execute();

    /// Method executing the algorithm
    StatusCode finalize();

  private:
    /// Private properties (python configurables):
    /// @{
    /// Connection to the (output)metadata store
    ServiceHandle< StoreGateSvc > m_metaStore;
    /// Connection to the inputMetadata store
    ServiceHandle< StoreGateSvc > m_inputMetaStore;
    /// Connection to the trigger configuration service
    ToolHandleArray< Ringer::ICaloRingsBuilder > m_crBuilderTools;
    /// StoreGate keys for the RingSet configurations 
    std::vector<std::string> m_rsMetaNames;
    /// @}

    /// Private properties (non python configurables):
    /// @{
    /// The CaloRings configuration container
    std::vector< xAOD::RingSetConfContainer* > m_rsConfContVec;
    /// @}

    /// Private methods:
    /// @{
    /**
     * @brief Retrieve CaloRingsBuilder tools
     **/
    StatusCode copyInputMetaStore();

    /**
     * @brief Auxiliary method called by copyInputMetaStore
     **/
    template< class auxT, class T >
    StatusCode searchAndCopyCLID( const ClassID_traits< T >& classID );

    /**
     * Retrieve a key and returns a copy of it to outputmeta
     **/ 
    template < class auxT, class T >
    StatusCode copyKeyToStore( const std::string &key );

    /**
     * @brief Retrieve CaloRingsBuilder tools
     **/
    StatusCode retrieveCaloRingsBuilders();

    /**
     * @brief Prepare containers to retrieve configurations
     **/
    StatusCode allocateContainers();

    /**
     * @brief Fill the containers with each CaloRingsBuilder tool configuration
     **/
    StatusCode fillConfigurations();
    /// @}

}; // class xAODRingSetsConfWriter

} // namespace Ringer

#endif // CALORINGERTOOLS_XAODCALORINGSSCONFWRITER_H
