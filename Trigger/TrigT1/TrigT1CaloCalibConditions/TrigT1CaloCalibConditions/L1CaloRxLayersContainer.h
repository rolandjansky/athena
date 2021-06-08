/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALORXLAYERSCONTAINER_H
#define L1CALORXLAYERSCONTAINER_H

#include <map>
#include <string>
#include <vector>

#include "AthenaKernel/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloRxLayers.h"
#include "TrigT1CaloCalibConditions/L1CaloRxCoolChannelId.h"

/**
*  Container of L1CaloRxLayers objects, inherit
*  from the abstract base class AbstractL1CaloConditionContainer.
*  Implement the IL1CaloPersistenceCapable interface.
*
*  @author Peter Faulkner
*/

class L1CaloRxLayersContainer: public DataObject,
                               public AbstractL1CaloPersistentCondition
{
 private:
    enum eAttrSpecification { eNLayers, eName1, eName2, eName3, eName4, eNCells1, eNCells2, eNCells3, eNCells4 };

 public:

    typedef std::map<unsigned int, L1CaloRxLayers> L1CaloRxLayersMap;
    typedef L1CaloRxLayersMap::const_iterator const_iterator;

    enum eCoolFolders { eRxLayers };

    L1CaloRxLayersContainer();
    L1CaloRxLayersContainer(const std::map<L1CaloRxLayersContainer::eCoolFolders, std::string>& folderKeysMap);
    virtual ~L1CaloRxLayersContainer() {};
    virtual DataObject* makePersistent() const;
    
    // Hide from reflex to avoid warning from reflex shadow class.
#ifndef __REFLEX__
    // IL1CaloPersistenceCapable interface
    using AbstractL1CaloPersistentCondition::makeTransient; // to unhide the default implementation of makeTransient
    virtual void makeTransient(const std::map<std::string, CondAttrListCollection*>& condAttrListCollectionMap);
#endif
    
    const L1CaloRxLayers* rxLayers(unsigned int channelId) const;
    const L1CaloRxLayers* rxLayers(const L1CaloRxCoolChannelId& channelId) const;
    void addRxLayers(unsigned int channelId, L1CaloRxLayers&& rxLayers);
    void addRxLayers(const L1CaloRxCoolChannelId& channelId, L1CaloRxLayers&& rxLayers);
    
    inline const_iterator begin() const { return m_mRxLayersMap.begin(); };
    inline const_iterator end()   const { return m_mRxLayersMap.end(); };
    
    virtual std::vector<std::string> coolInputKeys() const;
    virtual std::string coolOutputKey() const;
    
    virtual void clear();
    
 private:
    std::string coolFolderKey(L1CaloRxLayersContainer::eCoolFolders efolder) const;
    
    L1CaloRxLayersMap m_mRxLayersMap;
    std::map<L1CaloRxLayersContainer::eCoolFolders, std::string> m_mCoolFoldersKeysMap;
};

CLASS_DEF( L1CaloRxLayersContainer , 1203741828 , 1 )

#endif
