/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: HanOutput.cxx,v 1.19 2009-03-30 12:52:21 ponyisi Exp $
// **********************************************************************

#include "DataQualityInterfaces/HanOutput.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <map>

#include <string.h>	// strncmp()

#include <TDirectory.h>
#include <TFile.h>
#include <TKey.h>
#include <TList.h>
#include <TObjArray.h>
#include <TNamed.h>
#include <TObject.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TH1.h>
#include <TGraph.h>

#include "dqm_core/exceptions.h"
#include "dqm_core/OutputListener.h"
#include "dqm_core/Parameter.h"
#include "dqm_core/Region.h"
#include "dqm_core/Result.h"

#include "DataQualityInterfaces/HanConfig.h"
#include "DataQualityInterfaces/HanUtils.h"

#include <boost/algorithm/string/replace.hpp>

namespace {

std::string StatusToStr( const dqm_core::Result::Status& status );
}


namespace dqi {

bool setNameGeneral(TObject* obj, const std::string& name) {
  if (obj != 0) {
    // some special cases, to avoid interpreter
    if (TH1* h=dynamic_cast<TH1*>(obj)) {
      h->SetName(name.c_str());
      return true;
    } else if (TObjArray* a=dynamic_cast<TObjArray*>(obj)) {
      a->SetName(name.c_str());
      return true;
    } else if (TGraph* g=dynamic_cast<TGraph*>(obj)) {
      g->SetName(name.c_str());
      return true;
    } else {
      TClass* kl = obj->IsA();
      TMethod* klm = kl->GetMethod("SetName", "\"Reference\"");
      if (! klm) {
	std::cerr << "Error: attempt to change object name to " << name << " failed as its name is not settable" << std::endl;
      } else {
	std::cout << "Manually doing cast for " << kl->GetName() << std::endl;
	obj->Execute("SetName", ("\""+name+"\"").c_str());
	return true;
      }
    }
  }
  return false;
}


const int HanOutput::Result::charArrSize = 10;


// *********************************************************************
// Public Methods
// *********************************************************************

HanOutput::
HanOutput( std::string rootFileName, DQOutputMap_t *outMap, TSeqCollection *outList )
  : m_fileName(rootFileName)
  , m_file( TFile::Open(rootFileName.c_str(),"RECREATE") )
  , m_retainUnpubData(false)
  , m_config(0)
  , m_input(0)
{
  m_outputMap = outMap;
  m_outputList = outList;
  if( m_file.get() == 0 ) {
    std::cerr << "File not writable: " << rootFileName << "\n";
  }
}


HanOutput::
~HanOutput()
{
  DQResultMap_t::const_iterator rend = m_dqResults.end();
  for( DQResultMap_t::const_iterator i = m_dqResults.begin(); i != rend; ++i ) {
    dqm_core::Result* result = i->second;
    delete result;
  }
  m_file->Close();
}


void
HanOutput::
addListener( const std::string& name, dqm_core::OutputListener* listener )
  throw (dqm_core::Exception)
{
  dqm_core::Region* region = dynamic_cast<dqm_core::Region*>( listener );
  if( region == 0 )
    return;

  DQParMap_t::const_iterator i = m_dqPars.find(name);
  if (i != m_dqPars.end()) {
    if (i->second != region) {
      std::cerr << "Attempt to add " << name << " twice; ignoring" << std::endl;
    } else {
      return;
    }
  }

  DQParMap_t::value_type dqParVal( name, region );
  m_dqPars.insert( dqParVal );
  
  DQRegMap_t::value_type dqRegVal( region, name );
  m_dqRegs.insert( dqRegVal );
 
  m_unpublishedDQPars.insert( name );
}


void
HanOutput::
addListener( const dqm_core::Parameter& parameter, dqm_core::OutputListener* listener )
{
  addListener( parameter.getName(), listener );
}


void
HanOutput::
publishResult( const std::string& name, const dqm_core::Result& result )
  throw (dqm_core::Exception)
{
//  std::cout << "Publish " << name << std::endl;
  delete m_dqResults[name];
  m_dqResults[name] = result.clone();

  DQParSet_t::iterator dqpari = m_unpublishedDQPars.find( name );
  if( dqpari != m_unpublishedDQPars.end() && !m_retainUnpubData ) {
  	m_unpublishedDQPars.erase( dqpari );
  }

  if (m_retainUnpubData) {
    DQParMap_t::const_iterator i = m_dqPars.find( name );
    if( i != m_dqPars.end() ) {
      dqm_core::Region* parent = i->second;
      dqm_core::OutputListener* plistener = parent;
      plistener->handleResult(name,result);
    }
  }
}
 
void
HanOutput::
flushResults()
{
	// store regex lists
	DQOutputMap_t tmpRegex;
	DQParSet_t::const_iterator regexEnd = m_regexlist.end();
	for (DQParSet_t::const_iterator regex = m_regexlist.begin(); regex != regexEnd; ++regex) {
		TSeqCollection *resultList = (*m_outputMap)[*regex];
		if (resultList == 0) {
			std::cerr << "Can't find original list for regex???" << std::endl;
		} else {
			tmpRegex.insert(DQOutputMap_t::value_type(*regex, resultList));
			(*m_outputMap).erase(*regex);
		  DQParMap_t::const_iterator i = m_dqPars.find( *regex );
		  if( i != m_dqPars.end() ) {
		    dqm_core::Region* parent = i->second;

		    std::string parentName = parent->getName();
		    // just remove, don't delete
		    dynamic_cast<TSeqCollection*>((*m_outputMap)[parentName])->Remove(resultList);
		  }
		}
	}
	
  // replay results
	DQResultMap_t::const_iterator rEnd = m_dqResults.end();
  for (DQResultMap_t::const_iterator r = m_dqResults.begin();
       r != rEnd;
       ++r) {
    const std::string name(r->first);
    const dqm_core::Result& result(*(r->second));

    std::string parentName("top_level");
    DQParMap_t::const_iterator i = m_dqPars.find( name );

    if( i != m_dqPars.end() ) {
    	dqm_core::Region* parent = i->second;
    	parentName = parent->getName();
    }
    // copy-paste ENDS here

    TSeqCollection *resultList = (*m_outputMap)[name];
    std::string parname(name);
    std::string storename(name);

    if (resultList == NULL) {
    	// is regex?
    	bool isRegex = false;
	std::string extra;
		  for (DQParSet_t::const_iterator regex = m_regexlist.begin(); regex != regexEnd; ++regex) {
		  	std::string::size_type regexlen = regex->length();
		  	if (*regex + "_" == name.substr(0, regexlen+1)) {
		  		isRegex = true;
		  		parname = *regex;
				std::string::size_type atsign = regex->rfind("@");
				if (atsign != std::string::npos) {
				  extra = regex->substr(atsign, std::string::npos);
				}
		  		storename = name.substr(regexlen+1, std::string::npos);
		  		break;
		  	}
		  }
		  if (isRegex) {
		  	if (m_input == NULL) {
		  		std::cerr << "WARNING: setInput() has not been set; cannot publish regex results" << std::endl;
		  		continue;
		  	}
		  	resultList = dynamic_cast<TSeqCollection*>(tmpRegex[parname]->Clone());
		  	(*m_outputMap)[storename+extra] = resultList;
			DQParMap_t::const_iterator i = m_dqPars.find( parname );
			parentName = i->second->getName();
			bool use_full_name = false;
			if (m_config) {
			  std::auto_ptr<const HanConfigAssessor> a(m_config->GetAssessor(parentName, parname));
			  if (a.get()) {
			    std::string store_using_path;
			    const HanConfigParMap* hcpm = a->GetAnnotation("store_using_path");
			    if (hcpm) {
			      store_using_path.assign(hcpm->GetValue());
			    }
			    boost::algorithm::to_lower(store_using_path);
			    if (store_using_path == "1" || store_using_path == "yes" || store_using_path == "true") {
			      use_full_name = true;
			    }
			  }
			}
			if (use_full_name) {
			  resultList->SetName((boost::algorithm::replace_all_copy(storename, "/", "_") + extra + "_").c_str());
			} else {
			  resultList->SetName((storename + extra + "_").c_str());
			}
		  	dynamic_cast<TSeqCollection*>((*m_outputMap)[parentName])->Add(resultList);
		  	TKey* key = getObjKey(m_input, storename);
		  	if( key != 0 ) {
		  		const char* className = key->GetClassName();
		  		if( (strncmp(className, "TH", 2) == 0) || (strncmp(className, "TGraph", 6) == 0) || (strncmp(className, "TProfile", 8) == 0) ) {
				  TNamed* transobj = dynamic_cast<TNamed*>(key->ReadObj());
				  if (transobj != NULL) {
				    HanHistogramLink* hhl = new HanHistogramLink(m_input, storename);
				    if (use_full_name) {
				      hhl->SetName((boost::algorithm::replace_all_copy(storename, "/", "_") + extra).c_str());
				    } else {
				      hhl->SetName((std::string(transobj->GetName()) + extra).c_str());
				    }
				    //transobj->SetName((std::string(transobj->GetName()) + extra).c_str());
				    //resultList->Add(transobj);
				    resultList->Add(hhl);
				  } else {
				    std::cerr << "TNamed* cast failed for "
					      << storename << std::endl;
				  }
		  		}
		  	} else {
		  		std::cout << "key is NULL" << std::endl;
		  	}
		  } else {
		  	std::cerr << "WARNING: Unable to find mapping for " << name << std::endl;
		  	continue;
		  }
	  }
	
	  resultList = dynamic_cast<TSeqCollection*>(resultList->FindObject("Results"));
	 
	  if (resultList == 0) {
	    std::cerr << "Warning: no result list found associated with '" << name << "'\n";
	    continue;
	  }
	  resultList->Add( newTObjArray("Status", new TObjString( StatusToStr(result.status_).c_str() ), 1) );
	
	  // iterate through the tags
	  std::map<std::string, double>::const_iterator iter = result.tags_.begin();
	  for (; iter != result.tags_.end(); ++iter) {
	    std::ostringstream tagval;
	    tagval << std::setprecision(4) << iter->second;
	    resultList->Add( newTObjArray(iter->first.c_str(),new TObjString( tagval.str().c_str() ), 1) );
		}
	  
	  // if there's an output object from the algorithm, include a clone
	  TObject* resultobj = result.getObject();
	  if (resultobj != 0) {
	    TObject* resultobjclone = resultobj->Clone();
	    if (setNameGeneral(resultobj, "ResultObject")) {
	      resultList->Add(resultobjclone);
	    } else {
	      std::cerr << "Discarding result object " << result.getObject()->GetName() << std::endl;
	      delete resultobjclone;
	    }
	  }
	  
	  if( m_config != 0 ) {
	    TObject* ref = m_config->GetReference(parentName,parname);
	    if (ref) {
	      if (setNameGeneral(ref, "Reference")) {
		resultList->Add(ref);
	      } else {
		std::cerr << "Discarding reference object " << ref->GetName() << std::endl;
	      }
	    }
	  }
  }
}


void
HanOutput::
activate()
{
  gROOT->cd();
}

void
HanOutput::
setInput(TDirectory* input)
{
	m_input = input;
}

static void WriteListToDirectory(TDirectory *dir, TSeqCollection *list, TFile* file, int level)
{
  TIter nextElem(list);
  TObject *obj;
  TSeqCollection *tmpList;

  while( (obj = nextElem()) != 0 ) {
    bool delete_when_done = false;
    HanHistogramLink* hhl = dynamic_cast<HanHistogramLink*>(obj);
    if (hhl != 0) { 
      obj = hhl->getObject();
      if (! obj) continue;
      delete_when_done = true;
      setNameGeneral(obj, hhl->GetName());
    }
    if (strncmp(obj->GetName(),"Reference",9) == 0 ||
	strncmp(obj->GetName(),"ResultObject",12) == 0) {
      dir->WriteTObject(obj);
      if (delete_when_done) delete obj;
      continue;
    }
    tmpList = dynamic_cast<TSeqCollection *>(obj);
    if (tmpList != 0) {
      char *str = (char *)tmpList->GetName(), *tmp = strtok((char *)str, "/");
      while(tmp != 0) {
        str = tmp;
        tmp = strtok(0, "/");
      }
      TDirectory* daughter;
      if (!dir->FindKey(str)) {
	daughter = dir->mkdir(str);
      }
      else{ 
	std::cout << "Failed to make " << std::endl;
	continue; 
      }
      WriteListToDirectory(daughter, tmpList, file, level-1);
      if (level > 0) { file->Write(); delete daughter; }
    }
    else if ((strncmp(obj->ClassName(), "TH", 2) == 0)
	     || (strncmp(obj->ClassName(), "TGraph", 6) == 0)
	     || (strncmp(obj->ClassName(), "TProfile", 8) ==0)) {
      dir->GetMotherDir()->WriteTObject(obj);
    } else {
      // anything else put it in current directory
      dir->WriteTObject(obj);
    }
    if (delete_when_done) delete obj;
  }
}


void
HanOutput::
deactivate()
{
  flushResults();
  m_file->SetBit(TFile::kDevNull);
  WriteListToDirectory(m_file.get(), dynamic_cast<TSeqCollection *>(m_outputList->First()), m_file.get(), 4);
  m_file->Write();
  m_file->Flush();
}


void
HanOutput::
setConfig( HanConfig* config )
{
  m_config = config;
  config->GetRegexList(m_regexlist);
}


void
HanOutput::
publishMissingDQPars()
{
  m_retainUnpubData = true;
  
  DQRegMap_t::const_iterator dqRegIter = m_dqRegs.begin();
  DQRegMap_t::const_iterator dqRegEnd  = m_dqRegs.end();
  for( ; dqRegIter != dqRegEnd; ++dqRegIter ) {
    std::string regname = dqRegIter->first->getName();
    m_unpublishedDQPars.erase( regname );
  }
  
  DQParSet_t::const_iterator regexItr = m_regexlist.begin();
  DQParSet_t::const_iterator regexEnd = m_regexlist.end();
  for ( ; regexItr != regexEnd ; ++regexItr ) {
  	m_unpublishedDQPars.erase( *regexItr );
  }
  
  dqm_core::Result::Status status( dqm_core::Result::Undefined );
  dqm_core::Result result( status );
  
  DQParSet_t::const_iterator unpubIter = m_unpublishedDQPars.begin();
  DQParSet_t::const_iterator unpubEnd  = m_unpublishedDQPars.end();
  for( ; unpubIter != unpubEnd; ++unpubIter ) {
    const std::string& name = *unpubIter;
    //reduce verbosity
    //std::cout << "--> Publishing missing object: \"" << name << "\"\n";
    publishResult( name, result );
  }
  
  m_retainUnpubData = false;
}


// *********************************************************************
// Protected Methods
// *********************************************************************


HanOutput::Result::
Result( TDirectory* dir )
  : m_result( new TTree("result","Assessment Result") )
  , m_status( new char[charArrSize] )
{
  m_result->SetDirectory( dir );
  m_result->Branch( "Status", m_status, "Status/C" );
}


HanOutput::Result::
~Result()
{
  delete [] m_status;
}


void
HanOutput::Result::
fill( const dqm_core::Result& result )
{
  copyString( m_status, StatusToStr(result.status_) );
  m_result->Fill();
}


void
HanOutput::Result::
write()
{
  m_result->Write();
}


void
HanOutput::Result::
copyString( char* to, const std::string& from )
{
  int i = 0;
  const char* f = from.c_str();
  while( ++i < charArrSize && (*to++ = *f++) != 0 )
    ;
  if( i == charArrSize ) {
  *to = 0;
  }
}


bool
HanOutput::RegionNameComp::
operator()( const dqm_core::Region* a, const dqm_core::Region* b )
{
  return (a->getName() < b->getName());
}

// *********************************************************************
// Private Methods
// *********************************************************************

HanOutput::
HanOutput() :
    m_retainUnpubData(false),
    m_outputMap(0),
    m_outputList(0),
    m_config(0),
    m_input(0)
{
}


} // namespace dqi


namespace {

std::string StatusToStr( const dqm_core::Result::Status& status )
{
  switch( status ) {
  case dqm_core::Result::Red:
    return "Red";
  case dqm_core::Result::Yellow:
    return "Yellow";
  case dqm_core::Result::Green:
    return "Green";
  case dqm_core::Result::Disabled:
    return "Disabled";
  case dqm_core::Result::Undefined:
  default:
    return "Undefined";
  }
}

}
