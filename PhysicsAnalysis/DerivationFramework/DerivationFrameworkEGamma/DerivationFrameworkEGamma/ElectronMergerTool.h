/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ElectronMergerTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef DERIVATIONFRAMEWORK_ELECTRONMERGERTOOL_H
#define DERIVATIONFRAMEWORK_ELECTRONMERGERTOOL_H

#include <string>
#include <map>
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/Electron.h"
#include "ExpressionEvaluation/ExpressionParserUser.h"
#include <AthContainers/ConstDataVector.h>

#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"

namespace DerivationFramework {

  /** @brief Class-algorithm for track particle collection merging*/
  class ElectronMergerTool : public AthAlgTool, public IAugmentationTool 
  //class ElectronMergerTool : extends< AthAlgTool, IAugmentationTool > 
    {
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      /** @brief Standard Algotithm methods:                           */
      ///////////////////////////////////////////////////////////////////

      ElectronMergerTool(const std::string& t, const std::string& n, const IInterface* p);
      virtual ~ElectronMergerTool() = default;
      StatusCode initialize() override;
      virtual StatusCode addBranches() const override;
 
    protected:

      ///////////////////////////////////////////////////////////////////
      /** @brief Protected data:                                       */
      ///////////////////////////////////////////////////////////////////
      Gaudi::Property<bool>                            m_createViewCollection{this, "CreateViewCollection", true, "Create a view to avoid deep copy" }; /** Create a view to avoid deep copy. */
      Gaudi::Property<bool>                            m_doRemoval{this, "doRemoval", false, "Remove overlapping electrons from output collection." }; /** Remove overlapping electrons from output collection. */
      Gaudi::Property<float>                           m_ORThreshold{this, "ORThreshold", 0.001, "Delta R threshold for matching in overlap removal." }; /** Delta R threshold for matching in overlap removal. */
      SG::ReadHandleKeyArray<xAOD::ElectronContainer>  m_electronLocation{this, "InCollectionsLocation" , {"Electrons","LRTElectrons"}, "List of input electron collections to merge" }; /** Vector of electron collections to be merged. */
      SG::WriteHandleKey<xAOD::ElectronContainer>      m_outelectronLocation{this, "OutputCollectionName","ElectronsComb", "Name for combined output collection"};  /** Combined electron collection.   */

      ///////////////////////////////////////////////////////////////////
      /** @brief Protected methods:                                    */
      ///////////////////////////////////////////////////////////////////
      
      /** @brief A routine that adds links to shared calo clusters collections. */
      void checkOverlap(const std::vector<const xAOD::ElectronContainer*>& electronCollections,
                        std::set<const xAOD::Electron*> & LRTElectronsToRemove ) const;


      /** @brief A routine that merges the electron collections. */
      void mergeElectron(const xAOD::ElectronContainer* electronCol,
                               xAOD::ElectronContainer* outputCol,
                         const std::set<const xAOD::Electron*>& LRTElectronsToRemove) const;

      /** @brief A routine that merges the electron collections. */
      void mergeElectron(const xAOD::ElectronContainer* electronCol,
                         ConstDataVector<xAOD::ElectronContainer>* outputCol,
                         const std::set<const xAOD::Electron*>& LRTElectronsToRemove) const;

    };
    
}
#endif // DERIVATIONFRAMEWORK_ELECTRONMERGERTOOL_H
