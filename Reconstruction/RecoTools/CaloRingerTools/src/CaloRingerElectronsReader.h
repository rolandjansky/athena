/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingerElectronsReader.h 668867 2015-05-20 20:23:22Z wsfreund $
#ifndef CALORINGERTOOLS_CALORINGERELECTRONSREADER_H
#define CALORINGERTOOLS_CALORINGERELECTRONSREADER_H

// STL includes:
#include <string>

// Base includes:
#include "CaloRingerInputReader.h"
#include "CaloRingerTools/ICaloRingerElectronsReader.h"
#include "CaloRingerReaderUtils.h"

// xAOD includes:
#include "xAODEgamma/ElectronContainer.h"

// Asg selectors include:
#include "RingerSelectorTools/IAsgElectronRingerSelector.h"

// StoreGate includes:
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKeyArray.h"

namespace Ringer {

class CaloRingerElectronsReader : public CaloRingerInputReader,
                                  public ICaloRingerElectronsReader
{

  public:

    /// @name CaloRingerElectronsReader ctors and dtors:
    /// @{
    /**
     * @brief Default constructor
     **/
    CaloRingerElectronsReader(const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

    /**
     * @brief Destructor
     **/
    ~CaloRingerElectronsReader();
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


  private:

    /// @name CaloRingerElectronsReader private methods:
    /// @{
    /** Retrieve Electron Asg Selector */
    StatusCode retrieveSelectors();
    /// @}

    /** Add decorations for a given selector */
    StatusCode addSelectorDeco(const std::string &contName, const std::string &selName);

    /// Tool CaloRingerElectronsReader props (python configurables):
    /// @{
    /**
     * @brief Electron selectors.
     **/
    PublicToolHandleArray<IAsgElectronRingerSelector> m_ringerSelectors {this,
	"ElectronSelectors", {}, "The ASG Electron Selectors."};

    /** @brief electron collection input name*/
    SG::ReadHandleKey<xAOD::ElectronContainer> m_inputElectronContainerKey {this,
      "inputKey",
      "Electrons",
      "Name of the input electron container"};
    /// @}

    /// Tool CaloRingerElectronsReader props (non configurables):
    /// @{

    /// The WriteDecorHandleKeyArrays for the selectors. 
    /// Need to be properties for dependency propagation.
    SG::WriteDecorHandleKeyArray<xAOD::ElectronContainer, char> m_selKeys {this,
       "DoNotSet_selKeys", {}, "dummy property"};

    SG::WriteDecorHandleKeyArray<xAOD::ElectronContainer, unsigned int> m_isEMKeys {this,
       "DoNotSet_isEMKeys", {}, "dummy property"};

    SG::WriteDecorHandleKeyArray<xAOD::ElectronContainer, float> m_lhoodKeys {this,
       "DoNotSet_lhoodKeys", {}, "dummy property"};

    /// The CaloRings Builder functor:
    BuildCaloRingsFctor<xAOD::ElectronContainer> *m_clRingsBuilderElectronFctor;

    /// Whether selectors are available
    Gaudi::Property<bool> m_selectorsAvailable {this,
	"selectorsAvailable", false, "Whether Selector Tool is available."};
    /// @}

};

} // namespace Ringer

#endif
