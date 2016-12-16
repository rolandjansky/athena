/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JobOptionsCatalogue.cxx,v 1.1 2007-07-29 13:25:49 stelzer Exp $
// JobOptionsCatalogue.cpp: implementation of the JobOptionsCatalogue class.
//
//////////////////////////////////////////////////////////////////////

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "TrigConfigSvc/JobOptionsCatalogue.h"

#include <string.h>

//----------------------------------------------------------------------------
TrigConf::JobOptionsCatalogue::~JobOptionsCatalogue() {
	// Delete the JobOptionsLists created by this catalogue
	for( algorithmsIterator iter = m_algorithmoptions.begin();
       iter != m_algorithmoptions.end(); iter++ ) {
		delete *iter;
	}
}

//----------------------------------------------------------------------------
StatusCode
TrigConf::JobOptionsCatalogue::findAlgorithm( const std::string& myAlg, 
                                               JobOptionsList*& myList ) const {
  std::vector<JobOptionsList*>::const_iterator iter;
	for( iter = m_algorithmoptions.begin();
     	 iter != m_algorithmoptions.end();
       iter++ ) {
    std::string ss = ((*iter)->algorithmName());
                if ( (*iter)->algorithmName().compare(myAlg) == 0 ) {
			myList = *iter;
			return StatusCode::SUCCESS;
		}
	}
	return StatusCode::FAILURE;
}

//----------------------------------------------------------------------------
void
TrigConf::JobOptionsCatalogue::addOption( const std::string& myAlg, 
                                          const Property* const& myOpt ) {
	JobOptionsList* myList;
	StatusCode sc = this->findAlgorithm( myAlg, myList );
	if( sc.isSuccess() ) {
		myList->addOption( myOpt );
	}
	else {
	// If algorithm was not found, create a new list
		JobOptionsList* j = new JobOptionsList(myAlg, myOpt);
		m_algorithmoptions.insert( m_algorithmoptions.end(), j );
	}
}

 
//---------------------------------------------------------------------------- 
StatusCode
TrigConf::JobOptionsCatalogue::optionsOf( const std::string& myAlg,
                                          std::vector<const Property*>*& myOpts) const {
	JobOptionsList* myList;
	StatusCode sc = this->findAlgorithm( myAlg, myList );
	if( sc.isSuccess() ) {
	  myOpts 
	    = const_cast<std::vector<const Property*>*>(myList->properties() );
	  return StatusCode::SUCCESS;
	}
	else {
		return StatusCode::FAILURE; // Algorithm has no options
	}
}

//----------------------------------------------------------------------------
const Property* 
TrigConf::JobOptionsCatalogue::getOption(const std::string& which) {
  size_t loc = which.find('.');
  if ( loc != std::string::npos )   {
    std::vector<const Property*>* opts = 0;
    StatusCode sc = optionsOf( which.substr(0, loc), opts);
    if ( sc.isSuccess() )   {
      std::string propName = which.substr(loc+1);
      std::vector<const Property*>::const_iterator itr;
      for ( itr = opts->begin(); itr != opts->end(); itr++ )    {
        if ( (*itr)->name() == propName )    {
          return *itr;
        }
      }
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
void
TrigConf::JobOptionsCatalogue::printUnresolvedOptions(MsgStream& msgStream) {
	for( algorithmsIterator iter = m_algorithmoptions.begin();
       iter != m_algorithmoptions.end();
       iter++ ) {
    std::vector<const Property*>* opts =
      const_cast<std::vector<const Property*>*>((*iter)->properties());
    std::vector<const Property*>::const_iterator itr;
    for ( itr = opts->begin(); itr != opts->end(); itr++ )    {
      Property* p = const_cast<Property*>(*itr);
      StringArrayProperty* sap = dynamic_cast<StringArrayProperty*> (p);
      if ( sap != 0 )    {
        typedef std::vector<std::string> OptsArray;
        OptsArray& res = const_cast<OptsArray&>(sap->value());
        for ( unsigned int i = 0; i != res.size(); i++ )  {
          std::string& s = res[i];
          if ( s.length() > 0 && s[0] == '@' )    {
            msgStream << MSG::FATAL << "Found problematic option:" 
                      << (*iter)->algorithmName() << "." << p->name() 
                      << " Value[" << i << "] = " << s << endmsg;
          }
        }
      }
      else    {
        StringProperty* sp = dynamic_cast<StringProperty*> (p);
        if ( sp != 0 )    {
          const std::string& s = sp->value();
          if ( s.length() > 0 && s[0] == '@' )
            msgStream << MSG::FATAL << "Found problematic option:" 
                      << (*iter)->algorithmName() << "." << p->name() 
                      << " Value = " << s << endmsg;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------
StatusCode
TrigConf::JobOptionsCatalogue::resolveOptions(MsgStream& log) {
  long loops = 10000, missing_syms = 0;

  // loop over all algorithms in the catalogue
	for( algorithmsIterator iter = m_algorithmoptions.begin();
       iter != m_algorithmoptions.end();
       iter++ ) {

    bool restart_flag = false;

    // for each algorithm get the list of properties
    std::vector<const Property*>* opts =
      const_cast<std::vector<const Property*>*>((*iter)->properties());
    std::vector<const Property*>::const_iterator itr;

    // loop over the properties
    for ( itr = opts->begin(); itr != opts->end(); itr++ )    {

      Property* p = const_cast<Property*>(*itr);

      StringArrayProperty* sap = dynamic_cast<StringArrayProperty*> (p);

      // property is an array, that means the value of the property is a vector of strings
      if ( sap != 0 ) {
        typedef std::vector<std::string> OptsArray;
        OptsArray& res = const_cast<OptsArray&>(sap->value());
        OptsArray new_opts;
        // loop over the string values
        for ( OptsArray::iterator i = res.begin(); i != res.end(); i++ )  {
          std::string& s = *i;
          if ( s.length() > 0 && s[0] == '@' )   {
            Property* prop = const_cast<Property*>(getOption( s.substr(1) ));
            if ( prop != p ) { // Self-references are not allowed!!!
              StringArrayProperty* sp =
                dynamic_cast<StringArrayProperty*> (prop);
              if ( 0 != sp )     {
                OptsArray rep = sp->value();
                for ( OptsArray::iterator j = rep.begin(); j != rep.end(); j++){
                  new_opts.push_back(*j);
                }
                restart_flag = true;
              }
              else    {
                new_opts.push_back(s);
                missing_syms++;
              }
            }
            else    {
              log << endmsg
                  << MSG::FATAL << "Self references to options are not allowed!"
                  << (*iter)->algorithmName() << "." << p->name() 
                  << " Value = " << s << endmsg;
              return StatusCode::FAILURE;
            }
          }
          else    {
            new_opts.push_back(s);
          }
        }
        res = new_opts;
      }
      else    {
        StringProperty* sp = dynamic_cast<StringProperty*> (p);
        if ( sp != 0 )    {
          const std::string& s = sp->value();
          if ( s.length() > 0 && s[0] == '@' )   {
            const Property* prop = getOption(s.substr(1));
            const StringProperty* sprp =
              dynamic_cast<const StringProperty*> (prop);
            if ( 0 != sprp )     {
              sp->set(sprp->value());
              restart_flag = true;
            }
            else    {
              missing_syms++;
            }
          }
        }
      }
    }
    if ( restart_flag )   {
      if ( loops-- > 0 )   {
        iter = m_algorithmoptions.begin();
        missing_syms = 0;
      }
    }
  }
  if ( loops <= 0 || missing_syms > 0 )   {
    log << MSG::FATAL
        << "Job option symbols cannot be resolved. Bad symbols found."
        << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------
std::vector<std::string>
TrigConf::JobOptionsCatalogue::getClients() const {
  std::vector<std::string> clients;
  std::vector<JobOptionsList*>::const_iterator iter;

	for( iter = m_algorithmoptions.begin();
     					   iter != m_algorithmoptions.end(); iter++ ) {
		clients.push_back((*iter)->algorithmName());
	}
  return clients;
}
