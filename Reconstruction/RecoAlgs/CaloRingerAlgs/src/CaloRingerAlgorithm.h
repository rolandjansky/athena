/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingerAlgorithm.h 667872 2015-05-18 17:15:32Z wsfreund $
#ifndef CALORINGERALGS_CALORINGERDECORATOR_H
#define CALORINGERALGS_CALORINGERDECORATOR_H

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "CaloRingerTools/ICaloRingerInputReader.h"

namespace Ringer {


/**
 *  @class CaloRingerAlgorithm
 *  @brief Algorithm responsable for CaloRinger execution.
 *
 *  @author: Werner S. Freund <wsfreund@cern.ch>
 *  @date: Dec 2014
 *
 *  Based on egammaRec and IsolationAlgs.
 **/
class CaloRingerAlgorithm : public ::AthAlgorithm
{ 

  public: 

    /// @name CaloRingerAlgorithm ctors and dtors:
    /// @{
    /**
     * @brief AthAlgorithm ctor:
     **/
    CaloRingerAlgorithm( const std::string& name, ::ISvcLocator* pSvcLocator );
    /**
     * @brief default dtor:
     **/
    virtual ~CaloRingerAlgorithm(); 
    /// @}

    /// Athena algorithm's Hooks:
    /// @{
    virtual StatusCode  initialize();
    virtual StatusCode  execute();
    virtual StatusCode  finalize();
    /// @}

  private: 

    /// @name CaloRingerAlgorithm private methods:
    /// @{
    /** Retrieve Input Reader Tools */
    StatusCode retrieveReaders();
    /// @}

    /// @name CaloRingerAlgorithm props (python configurables):
    /// @{
    /** 
     * @brief Tools for building CaloRings 
     **/
    PublicToolHandleArray< Ringer::ICaloRingerInputReader > m_inputReaders {this,
	"inputReaderTools", {}, "Sequence of reader tools to execute."};
    /// @}

}; 

} // namespace Ringer

#endif // CALORINGERALGS_CALORINGERDECORATOR_H
