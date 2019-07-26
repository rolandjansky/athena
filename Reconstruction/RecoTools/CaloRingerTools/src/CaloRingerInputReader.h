/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingerInputReader.h 667886 2015-05-18 17:26:59Z wsfreund $
#ifndef CALORINGERTOOLS_CALORINGERINPUTREADER_H
#define CALORINGERTOOLS_CALORINGERINPUTREADER_H

// STL includes:
#include <string>

// Base includes:
#include "CaloRingerTools/ICaloRingerInputReader.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// Other interfaces include:
#include "CaloRingerTools/ICaloRingsBuilder.h"

namespace Ringer {

class CaloRingerInputReader : public ::AthAlgTool,
                              virtual public ICaloRingerInputReader
{

  public:

    /// @name CaloRingerInputReader ctors and dtors:
    /// @{
    /**
     * @brief Default constructor
     **/
    CaloRingerInputReader(const std::string& type,
                     const std::string& name,
                     const ::IInterface* parent);

    /**
     * @brief Destructor
     **/
    ~CaloRingerInputReader();
    /// @}

    /// Tool main methods:
    /// @{
    /**
     * @brief initialize method
     **/
    virtual StatusCode initialize() override;
    /**
     * @brief read electrons and populates @name decoMap with them and their
     * respective CaloRings.
     **/
    virtual StatusCode execute() override;
    /**
     * @brief finalize method
     **/
    virtual StatusCode finalize() override;
    /// @}


  protected:

    /// Tool CaloRingerInputReader props (python configurables):
    /// @{
    /**
     * @brief Tool to build CaloRings.
     **/
    PublicToolHandle<ICaloRingsBuilder> m_crBuilder {this,
			"crBuilder", "", "The CaloRingsBuilder Tool"};
    /// @}

    /// Tool CaloRingerInputReader props (non configurables):
    /// @{
    /// If CaloRings builder is available
    Gaudi::Property<bool> m_builderAvailable {this,
			"builderAvailable", false, "Whether builder tool is available."};
    ///  @}

};

} // namespace Ringer

#endif // CALORINGERTOOLS_CALORINGERINPUTREADER_H
