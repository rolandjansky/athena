/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingerPhotonsReader.h 752568 2016-06-03 16:03:21Z ssnyder $
#ifndef CALORINGERTOOLS_CALORINGERPHOTONSREADER_H
#define CALORINGERTOOLS_CALORINGERPHOTONSREADER_H

// STL includes:
#include <string>

// Base includes:
#include "CaloRingerInputReader.h"
#include "CaloRingerTools/ICaloRingerPhotonsReader.h"
#include "CaloRingerReaderUtils.h"

//CxxUtils for override final  
#include "CxxUtils/final.h"
#include "CxxUtils/override.h"

// xAOD includes:
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Electron.h"

// Asg selectors include:
#include "RingerSelectorTools/IAsgElectronRingerSelector.h"


namespace Ringer {

class CaloRingerPhotonsReader : public CaloRingerInputReader, 
                                public ICaloRingerPhotonsReader
{

  public:

    /// @name CaloRingerPhotonsReader ctors and dtors:
    /// @{
    /** 
     * @brief Default constructor
     **/
    CaloRingerPhotonsReader(const std::string& type,
                     const std::string& name,
                     const ::IInterface* parent);

    /** 
     * @brief Destructor
     **/
    ~CaloRingerPhotonsReader();
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

    /// Tool CaloRingerElectronsReader props (python configurables):
    /// @{
    /** 
     * @brief Electron selectors.
     * TODO Change it to Photon Selector
     **/
    PublicToolHandleArray<IAsgElectronRingerSelector> m_ringerSelectors {this,
	"PhotonSelectors", {}, "The ASG Photon Selectors."};

    /** 
     * @brief Hold selectors result names.
     **/
    Gaudi::Property<std::vector<std::string> > m_ringerSelectorResultNames {this,
	"ResultNames", {}, "The ASG Selectors result names."};
    /// @}


    /// Tool CaloRingerPhotonsReader props (non configurables):
    /// @{
    /// The Photon container
    xAOD::PhotonContainer* m_container;

    /// The CaloRings Builder functor:
    BuildCaloRingsFctor<xAOD::Photon> *m_clRingsBuilderPhotonFctor;

    /// Whether selectors are available
    //bool m_selectorAvailable;
    /// @}

};

}

#endif
