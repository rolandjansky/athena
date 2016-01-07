/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJetHypo/TrigHLTJetHypoUtils/CleanerMatcherFactory.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/IMatcherFactory.h"

CleanerMatcherFactory::CleanerMatcherFactory(const CleanerFactory& cf,
					     const std::shared_ptr<IMatcherFactory>& mf):

  m_cleanerFactory(cf), m_matcherFactory(mf){
}

CleanerMatcher CleanerMatcherFactory::make() const {
  return CleanerMatcher(m_cleanerFactory.make(),
			m_matcherFactory->make());
}
