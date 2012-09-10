/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALOENERGYSCANRUNINFOCONTAINER_H
#define L1CALOENERGYSCANRUNINFOCONTAINER_H

#include <map>
#include <string>

#include "CLIDSvc/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"

/**
*  Container of L1CaloEnergyScanRunInfo metadata, inherit
*  from the abstract base class AbstractL1CaloConditionContainer.
*  Implement the IL1CaloPersistenceCapable interface.
*
*  Copied from L1CaloPprLutContainer and modified.
*
*/

class L1CaloEnergyScanRunInfoContainer: public DataObject,
                             public AbstractL1CaloPersistentCondition
{
 private:
    enum eAttrSpecification { eRunNumber, eGainStrategy };

 public:

    enum eCoolFolders { eEnergyScanRunInfo };

    L1CaloEnergyScanRunInfoContainer();
    L1CaloEnergyScanRunInfoContainer(const std::map<L1CaloEnergyScanRunInfoContainer::eCoolFolders, std::string>& folderKeysMap);
    virtual ~L1CaloEnergyScanRunInfoContainer() {};
    virtual DataObject* makePersistent() const;
    
    // Hide from reflex to avoid warning from reflex shadow class.
#ifndef __REFLEX__
    // IL1CaloPersistenceCapable interface
    using AbstractL1CaloPersistentCondition::makeTransient; // to unhide the default implementation of makeTransient
    virtual void makeTransient(const std::map<std::string, AthenaAttributeList*> athenaAttributeListMap);
#endif
    
    unsigned int runNumber()   const    { return m_runNumber; }
    std::string gainStrategy() const    { return m_gainStrategy; }
    void setRunNumber(unsigned int run) { m_runNumber = run; }
    void setGainStrategy(const std::string& strategy) { m_gainStrategy = strategy; }
    
    virtual std::vector<std::string> coolInputKeys() const;
    virtual std::string coolOutputKey() const;
    
 private:
    std::string coolFolderKey(L1CaloEnergyScanRunInfoContainer::eCoolFolders efolder) const;
    
    std::map<L1CaloEnergyScanRunInfoContainer::eCoolFolders, std::string> m_mCoolFoldersKeysMap;
    unsigned int m_runNumber;
    std::string m_gainStrategy;
};

CLASS_DEF( L1CaloEnergyScanRunInfoContainer , 1193529625 , 1 )

#endif
