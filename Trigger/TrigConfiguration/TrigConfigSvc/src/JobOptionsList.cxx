/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JobOptionsList.cpp: implementation of the JobOptionsList class.
//
//////////////////////////////////////////////////////////////////////

#include "TrigConfigSvc/JobOptionsList.h"
#include "GaudiKernel/Property.h"

TrigConf::JobOptionsList::~JobOptionsList() {
	// Delete the Properties held by this catalogue
	for( std::vector<const Property*>::iterator iter = m_properties.begin();
       iter != m_properties.end(); iter++ ) {
		delete *iter;
	}
}

TrigConf::JobOptionsList::JobOptionsList( const std::string& myAlg, const Property* const& myOpt ){
	m_algorithmName = myAlg;
	this->addOption( myOpt );
}

const std::string&
TrigConf::JobOptionsList::algorithmName() const {
	return m_algorithmName;
}

void
TrigConf::JobOptionsList::addOption( const Property* const& myOpt ){
	m_properties.insert( m_properties.end(), myOpt );
}

const std::vector<const Property*>*
TrigConf::JobOptionsList::properties() const {
	return &m_properties;
}
