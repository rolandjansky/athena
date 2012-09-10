/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCONDITIONSCONTAINER_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCONDITIONSCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditions.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

#include <map>
#include <string>
#include <vector>

/**
*  Container of L1CaloPprConditions objects, inherit
*  from the abstract base class AbstractL1CaloConditionContainer.
*  Implement the IL1CaloPersistenceCapable interface.
*
*  @author Peter Faulkner
*/


class L1CaloPprConditionsContainer: public DataObject, public AbstractL1CaloPersistentCondition {

private:
	enum eAttrSpecification { eExtBcidThreshold, eSatBcidThreshLow, eSatBcidThreshHigh, eSatBcidLevel, eBcidEnergyRangeLow,
	                          eBcidEnergyRangeHigh, eFirStartBit,
	                          eBcidDecision1, eSatOverride1, eBcidDecision2, eSatOverride2, eBcidDecision3, eSatOverride3,
	                          ePeakFinderCond, eDecisionSource, eFirCoeff1, eFirCoeff2, eFirCoeff3, eFirCoeff4, eFirCoeff5,
				  eLutStrategy, eLutOffset, eLutSlope, eLutNoiseCut, ePedValue, ePedMean };

public:
	enum eCoolFolders { ePprChanDefaults, ePprChanCalib };

	L1CaloPprConditionsContainer();
	L1CaloPprConditionsContainer(const std::map<L1CaloPprConditionsContainer::eCoolFolders, std::string>& folderKeysMap);
	virtual ~L1CaloPprConditionsContainer();
	virtual DataObject* makePersistent() const;

	// IL1CaloPersistenceCapable interface
	using AbstractL1CaloPersistentCondition::makeTransient; // to unhide the default implementation of makeTransient
	virtual void makeTransient(const std::map<std::string, CondAttrListCollection*> condAttrListCollectionMap);

	const L1CaloPprConditions* pprConditions(unsigned int channelId) const;
	const L1CaloPprConditions* pprConditions(const L1CaloCoolChannelId& channelId) const;

	inline unsigned int bcidDecision1()  const { return m_bcidDecision1; }
	inline unsigned int satOverride1()   const { return m_satOverride1; }
	inline unsigned int bcidDecision2()  const { return m_bcidDecision2; }
	inline unsigned int satOverride2()   const { return m_satOverride2; }
	inline unsigned int bcidDecision3()  const { return m_bcidDecision3; }
	inline unsigned int satOverride3()   const { return m_satOverride3; }
	inline unsigned int peakFinderCond() const { return m_peakFinderCond; }
	inline unsigned int decisionSource() const { return m_decisionSource; }

	virtual std::vector<std::string> coolInputKeys() const;
	virtual std::string coolOutputKey() const;

	virtual void clear();

	void dump() const;

private:
        typedef std::vector<L1CaloPprConditions*> L1CaloPprConditionsVec;

	std::string coolFolderKey(L1CaloPprConditionsContainer::eCoolFolders efolder) const;

	L1CaloPprConditionsVec m_mPprConditionsVec;
	std::map<L1CaloPprConditionsContainer::eCoolFolders, std::string> m_mCoolFoldersKeysMap;

	unsigned int m_bcidDecision1;
	unsigned int m_satOverride1;
	unsigned int m_bcidDecision2;
	unsigned int m_satOverride2;
	unsigned int m_bcidDecision3;
	unsigned int m_satOverride3;
	unsigned int m_peakFinderCond;
	unsigned int m_decisionSource;

	static const unsigned int s_vectorSize = 8192;

};

CLASS_DEF(L1CaloPprConditionsContainer ,  1190452919 , 1 )

#endif
