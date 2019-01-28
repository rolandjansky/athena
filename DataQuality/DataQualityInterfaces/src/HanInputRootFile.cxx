/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataQualityInterfaces/HanInputRootFile.h"
#include "TClass.h"
#include "TH1.h"
#include "TGraph.h"
#include "TDirectoryFile.h"
#include "TEfficiency.h"
#include <iostream>
#include <cstring>

namespace {

TKey* getObjKeyFromTDir( TDirectory* dir, std::string path );
TDirectory* changeInputDir( TDirectory* dir, std::string path );

}

namespace dqi {

HanInputRootFile::
HanInputRootFile( std::string& rootFileName, std::string path )
  : dqm_core::InputRootFile::InputRootFile(rootFileName)
  , m_rootFile( TFile::Open(rootFileName.c_str()) )
  , m_basedir(0)
  , m_histNamesBuilt(false)
{
	if( m_rootFile.get() == 0 ) {
    // FIXME: Should throw a dqm_core::Exception
    std::cerr << "HanInputRootFile -> "<< rootFileName << " is not readable.\n";
    exit(-1);
	}

  if( path != "" ) {
    std::cout << "Using path \"" << path << "\" in input file\n";
    std::string pathForSearch = path;
    pathForSearch += "/dummyName";
    m_basedir = changeInputDir( m_rootFile.get(), pathForSearch );
    if( m_basedir == 0 ) {
      std::cerr << "Cannot find \"" << path << "\" in input file\n";
    }
  }
  
  if( m_basedir == 0 )
    m_basedir = m_rootFile.get();
}


HanInputRootFile::
~HanInputRootFile()
{
}

void
HanInputRootFile::
dolsr(const TDirectory* dir, std::vector<std::string>& hists, const TDirectory* topdir) 
{
	// permit calling with two arguments
	if (topdir == NULL) {
		topdir = dir;
	}
  TIter keys(dir->GetListOfKeys());
  TKey* key;
  std::string fullpath(dir->GetPath());
  std::string toppath(topdir->GetPath());
  std::string::size_type toppathlen = toppath.length();
  while ((key = dynamic_cast<TKey*>(keys())) != NULL) {
    if (TClass::GetClass(key->GetClassName())->InheritsFrom("TDirectory")) {
      TDirectory* newdir = dynamic_cast<TDirectory*>(key->ReadObj());
      if (!newdir) {
	std::cerr << "WARNING: cannot read directory " 
		  << fullpath << "/" << key->GetName()
		  << "; skipping" << std::endl;
      } else {
	dolsr(newdir, hists, topdir);
      }
      delete newdir;
    } else {
      std::string path;
      if (fullpath.substr(0, toppathlen) == toppath) {
	int extra = 1;
	if (toppath[toppathlen-1] == '/') extra = 0;
      	path = fullpath.substr(toppathlen + extra, std::string::npos);
      } else {
      	path = fullpath;
      }
      hists.push_back(path+"/"+std::string(key->GetName()));
    }      
  }
}


void
HanInputRootFile::
addListener( const boost::regex& regex, dqm_core::InputListener* listener )
{
  // avoid building directory until actually demanded
  if (! m_histNamesBuilt) {
    dolsr(m_basedir, m_histNames);
    m_histNamesBuilt = true;
  }

  for (std::vector<std::string>::const_iterator str = m_histNames.begin();
       str != m_histNames.end();
       ++str) {
    try {
      if (boost::regex_match(*str, regex)) {
        // is this actually a histogram/graph?
        TObject* temp = m_basedir->Get(str->c_str());
        if (dynamic_cast<TH1*>(temp) || dynamic_cast<TGraph*>(temp) || dynamic_cast<TEfficiency*>(temp) ) {
          std::cout << "Regular expression " << regex << " matches " << *str << std::endl;
          addListener(*str, listener);
        }
        delete temp;
      }
    } catch (std::exception &ex) { // should be std::runtime_error, but Coverity is not happy with that
      std::cout << "Warning: Matching of regular expression " << regex << " failed: " << ex.what() << std::endl;
    }
    
  }
}


void
HanInputRootFile::
addListener( const std::vector<std::string>& names, dqm_core::InputListener* listener )
{
  dqm_core::InputRootFile::addListener( names, listener );
}


void
HanInputRootFile::
addListener(const std::string& name, dqm_core::InputListener *listener)
{
  // TKeys are owned by the TFile
  TKey* key = getObjKeyFromTDir( m_basedir, name );
  if( key == 0 ) {
    // reduce verbosity
    //std::cerr << "Warning: Histogram \"" << name << "\" was not found.  Ignoring...\n";
    return;
  }
  m_monObjects.push_back( MonBundle(name,key,listener) );
}


TFile*
HanInputRootFile::
file() const
{
  return m_rootFile.get();
}

const TDirectory*
HanInputRootFile::
getBasedir() const
{
	return m_basedir;
}

} // end namespace dqi

namespace {

// This method is copy-pasted from InputRootFile (it was private)
TKey* getObjKeyFromTDir( TDirectory* dir, std::string path ) {
  if( dir == 0 )
    return 0;

  std::string::size_type i = path.find_first_of('/');
  if( i != std::string::npos ) {
    std::string dName( path, 0, i );
    std::string pName( path, i+1, std::string::npos );
    if( dName != "" ) {
      TDirectory* subDir = dir->GetDirectory(dName.c_str());
      if (subDir != dir) {
	return getObjKeyFromTDir( subDir, pName );
      }
      // TKey* key = dir->FindKey( dName.c_str() );
      // if( key != 0 ) {
      // 	TObject* obj = key->ReadObj();
      //   TDirectory* subDir = dynamic_cast<TDirectory*>( obj );
      //   TKey* rv = getObjKeyFromTDir( subDir, pName );
      // 	delete obj;
      // 	return rv;
      // }
      return 0;
    }
    return getObjKeyFromTDir( dir, pName );
  }

  return dir->FindKey( path.c_str() );
}


TDirectory* changeInputDir( TDirectory* dir, std::string path )
{
  if( dir == 0 )
    return 0;

  std::string::size_type i = path.find_first_of('/');
  if( i != std::string::npos ) {
    std::string dName( path, 0, i );
    std::string pName( path, i+1, std::string::npos );
    if( dName != "" ) {
      TDirectory* subDir = dir->GetDirectory(dName.c_str());
      if (subDir != dir) {
	return changeInputDir( subDir, pName );
      }
      return 0;
      // TDirectory* subDir(0);
      // TKey* key = dir->FindKey( dName.c_str() );
      // if( key != 0 ) {
      //   subDir = dynamic_cast<TDirectory*>( key->ReadObj() );
      // }
      // else {
      //   return 0;
      // }
      // return changeInputDir( subDir, pName );
    }
    return changeInputDir( dir, pName );
  }
  
  return dir;
}

} // end unnamed namespace
