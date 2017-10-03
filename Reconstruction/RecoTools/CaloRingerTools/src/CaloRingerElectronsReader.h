/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

//CxxUtils for override final  
#include "CxxUtils/final.h"
#include "CxxUtils/override.h"

// xAOD includes:
#include "xAODEgamma/ElectronContainer.h"

// Asg selectors include:
#include "RingerSelectorTools/IAsgElectronRingerSelector.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"

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
    virtual StatusCode initialize() ATH_OVERRIDE;
    /** 
     * @brief read electrons and populates @name decoMap with them and their
     * respective CaloRings.
     **/
    virtual StatusCode execute() ATH_OVERRIDE;
    /** 
     * @brief finalize method
     **/
    virtual StatusCode finalize() ATH_OVERRIDE;
    /// @}


  private:

    /// @name CaloRingerElectronsReader private methods:
    /// @{
    /** Retrieve Electron Asg Selector */
    StatusCode retrieveSelectors();
    /// @}

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

    /** @brief helper class to contain write docoration handle keys */
    template<class T> struct writeDecorHandleKeys {
      StatusCode initializeDecorKeys(const std::string &name); // note, not constructor

      std::vector<SG::WriteDecorHandleKey<T> > keys;
    };
    
    /** @brief helper class to contain write decoration handles */
    template<class T> struct writeDecorHandles {
     writeDecorHandles(const writeDecorHandleKeys<T>& keys); // constructor
      
      std::vector<SG::WriteDecorHandle<T, char> > selection;
      std::vector<SG::WriteDecorHandle<T, unsigned int> > isEM;
      std::vector<SG::WriteDecorHandle<T, float> > lhood;

    };

    /// The CaloRings Builder functor:
    BuildCaloRingsFctor<const xAOD::Electron> *m_clRingsBuilderElectronFctor;

    /// Whether selectors are available
    Gaudi::Property<bool> m_selectorsAvailable {this, 
	"selectorsAvailable", false, "Whether Selector Tool is available."};
    /// @}

};

} // namespace Ringer

#endif
