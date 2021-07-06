/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALOHVCORRECTIONSCONTAINER_H
#define L1CALOHVCORRECTIONSCONTAINER_H

#include <map>
#include <string>
#include <vector>

#include "AthenaKernel/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloHVCorrections.h"
#include "TrigT1CaloCalibConditions/L1CaloRxCoolChannelId.h"

/**
*  Container of L1CaloHVCorrections objects, inherit
*  from the abstract base class AbstractL1CaloConditionContainer.
*  Implement the IL1CaloPersistenceCapable interface.
*
*  @author Peter Faulkner
*/

class L1CaloHVCorrectionsContainer: public DataObject,
                                    public AbstractL1CaloPersistentCondition
{
 private:
    enum eAttrSpecification { eRxMean, eAffectedCells1, eAffectedCells2, eAffectedCells3, eAffectedCells4,
                                       eLayerMean1, eLayerMean2, eLayerMean3, eLayerMean4 };

 public:

    typedef std::map<unsigned int, L1CaloHVCorrections> L1CaloHVCorrectionsMap;
    typedef L1CaloHVCorrectionsMap::const_iterator const_iterator;

    enum eCoolFolders { eHVCorrections };

    L1CaloHVCorrectionsContainer();
    L1CaloHVCorrectionsContainer(const std::map<L1CaloHVCorrectionsContainer::eCoolFolders, std::string>& folderKeysMap);
    virtual ~L1CaloHVCorrectionsContainer() {};
    virtual DataObject* makePersistent() const;
    
    // Hide from reflex to avoid warning from reflex shadow class.
#ifndef __REFLEX__
    // IL1CaloPersistenceCapable interface
    using AbstractL1CaloPersistentCondition::makeTransient; // to unhide the default implementation of makeTransient
    virtual void makeTransient(const std::map<std::string, CondAttrListCollection*>& condAttrListCollectionMap);
#endif
    
    const L1CaloHVCorrections* hvCorrections(unsigned int channelId) const;
    const L1CaloHVCorrections* hvCorrections(const L1CaloRxCoolChannelId& channelId) const;
    void addHVCorrections(unsigned int channelId, L1CaloHVCorrections&& hvCorrections);
    void addHVCorrections(const L1CaloRxCoolChannelId& channelId, L1CaloHVCorrections&& hvCorrections);
    
    inline const_iterator begin() const { return m_mHVCorrectionsMap.begin(); };
    inline const_iterator end()   const { return m_mHVCorrectionsMap.end(); };
    
    virtual std::vector<std::string> coolInputKeys() const;
    virtual std::string coolOutputKey() const;
    
    virtual void clear();
    
 private:
    std::string coolFolderKey(L1CaloHVCorrectionsContainer::eCoolFolders efolder) const;
    
    L1CaloHVCorrectionsMap m_mHVCorrectionsMap;
    std::map<L1CaloHVCorrectionsContainer::eCoolFolders, std::string> m_mCoolFoldersKeysMap;
};

CLASS_DEF( L1CaloHVCorrectionsContainer , 1106607448 , 1 )

#endif
