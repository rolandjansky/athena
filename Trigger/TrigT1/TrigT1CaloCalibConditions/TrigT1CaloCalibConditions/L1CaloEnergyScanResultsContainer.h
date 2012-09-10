/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALOENERGYSCANRESULTSCONTAINER_H
#define L1CALOENERGYSCANRESULTSCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloEnergyScanResults.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

/**
*  Container of L1CaloEnergyScanResults objects, inherit
*  from the abstract base class AbstractL1CaloConditionContainer.
*  Implement the IL1CaloPersistenceCapable interface.
*
*  Copied from L1CaloPprLutContainer and modified.
*
*  @author Veit Scharf <veit.scharf@kip.uni-heidelberg.de>
*/

class L1CaloEnergyScanResultsContainer: public DataObject,
                             public AbstractL1CaloPersistentCondition
{
 private:
    enum eAttrSpecification { eOffset, eSlope, eChi2, eNdf, eBlob, eBlobStrategy, eErrorCode};

 public:

    typedef std::map<unsigned int, L1CaloEnergyScanResults> L1CaloEnergyScanResultsMap;
    typedef L1CaloEnergyScanResultsMap::const_iterator const_iterator;

    enum eCoolFolders { eEnergyScanResults };

    L1CaloEnergyScanResultsContainer();
    L1CaloEnergyScanResultsContainer(const std::map<L1CaloEnergyScanResultsContainer::eCoolFolders, std::string>& folderKeysMap);
    virtual ~L1CaloEnergyScanResultsContainer() {};
    virtual DataObject* makePersistent() const;
    
    // Hide from reflex to avoid warning from reflex shadow class.
#ifndef __REFLEX__
    // IL1CaloPersistenceCapable interface
    using AbstractL1CaloPersistentCondition::makeTransient; // to unhide the default implementation of makeTransient
    virtual void makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap);
#endif
    
    const L1CaloEnergyScanResults* energyScanResults(unsigned int channelId) const;
    const L1CaloEnergyScanResults* energyScanResults(const L1CaloCoolChannelId& channelId) const;
    void addEnergyScanResults(unsigned int channelId, const L1CaloEnergyScanResults& energyScanResults);
    void addEnergyScanResults(const L1CaloCoolChannelId& channelId, const L1CaloEnergyScanResults& energyScanResults);
    
    inline const_iterator begin() const { return m_mEnergyScanResultsMap.begin(); };
    inline const_iterator end()   const { return m_mEnergyScanResultsMap.end(); };
    
    virtual std::vector<std::string> coolInputKeys() const;
    virtual std::string coolOutputKey() const;
    
    virtual void clear();
    
 private:
    std::string coolFolderKey(L1CaloEnergyScanResultsContainer::eCoolFolders efolder) const;
    
    L1CaloEnergyScanResultsMap m_mEnergyScanResultsMap;
    std::map<L1CaloEnergyScanResultsContainer::eCoolFolders, std::string> m_mCoolFoldersKeysMap;
};

CLASS_DEF( L1CaloEnergyScanResultsContainer , 1176586318 , 1 )

#endif
