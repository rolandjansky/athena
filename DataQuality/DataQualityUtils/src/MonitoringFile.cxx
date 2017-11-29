/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: MonitoringFile.cxx 756119 2016-06-19 06:44:00Z ponyisi $
// **********************************************************************

#include "DataQualityUtils/MonitoringFile.h"
#include "DataQualityInterfaces/HanApp.h"
#include "DataQualityInterfaces/HanUtils.h"

#include <cmath>
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <vector>
#include <map>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <TObject.h>
#include <TSystem.h>

#include <TDirectory.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <TIterator.h>
#include <TKey.h>
#include <TROOT.h>
#include <TTree.h>

ClassImp(dqutils::MonitoringFile)


namespace dqutils {

bool histOKToMerge(TH1* h) {
  Long_t arrsize, computedsize;
  switch (h->GetDimension()) {
  case 1:
    computedsize = h->GetNbinsX()+2;
    break;
  case 2:
    computedsize = (h->GetNbinsX()+2)*(h->GetNbinsY()+2);
    break;
  case 3:
    computedsize = (h->GetNbinsX()+2)*(h->GetNbinsY()+2)*(h->GetNbinsZ()+2);
    break;
  default:
    std::cerr << "Unable to handle dimension of " << h->GetName() << "; refusing to merge";
    return false;
  }
  TArray* arrptr = dynamic_cast<TArray*>(h);
  if (! arrptr) {
    std::cerr << "Unable to cast TH1 to TArray for " << h->GetName() << " - saying it's OK and hoping for the best" << std::endl;
    return true;
  }
  arrsize = arrptr->GetSize();
  if (computedsize != arrsize) {
    std::cout << "Sizes: computed " << computedsize << " array " << arrsize << std::endl;
  }
  return computedsize == arrsize;
}

// *********************************************************************
// Public Methods
// *********************************************************************

MonitoringFile::OutputMetadata::
OutputMetadata( TTree* metadata )
	: m_charArrSize(100)
	, m_metadata(metadata)
	, m_nameData(0)
	, m_intervalData(0)
	, m_chainData(0)
  , m_mergeData(0)
{
  m_nameData = new char[m_charArrSize];
  m_intervalData = new char[m_charArrSize];
  m_chainData = new char[m_charArrSize];
  m_mergeData = new char[m_charArrSize];
  adjustAddresses( "Name", m_nameData, "Name/C" );
  adjustAddresses( "Interval", m_intervalData, "Interval/C" );
  adjustAddresses( "TriggerChain", m_chainData, "TriggerChain/C" );
  adjustAddresses( "MergeMethod", m_mergeData, "MergeMethod/C" );
}

void
MonitoringFile::OutputMetadata::
adjustAddresses(const char* branchName, void* ptr, const char* branchstr) 
{
  if (m_metadata->GetBranch(branchName)) {
    m_metadata->SetBranchAddress(branchName, ptr);
  } else {
    m_metadata->Branch(branchName, ptr, branchstr);
  }
}


MonitoringFile::OutputMetadata::
~OutputMetadata()
{
  delete [] m_mergeData;
	delete [] m_chainData;
	delete [] m_intervalData;
	delete [] m_nameData;
}


void
MonitoringFile::OutputMetadata::
fill( std::string name,
      std::string interval,
      std::string chain,
      std::string merge )
{
	copyString( m_nameData, name );
	copyString( m_intervalData, interval );
	copyString( m_chainData, chain );
	copyString( m_mergeData, merge );
	m_metadata->Fill();
}


void
MonitoringFile::OutputMetadata::
copyString( char* to, const std::string& from )
{
	int i = 0;
	const char* f = from.c_str();
	while( ++i < m_charArrSize && (*to++ = *f++) != 0 )
	  ;
	if( i == m_charArrSize ) {
		*to = 0;
	}
}


MonitoringFile::
MonitoringFile()
  : m_file(0),m_mergeMatchHistoRE(0),m_mergeMatchDirRE(0),
    m_mergeMatchHistoREString(".*"),m_mergeMatchDirREString(".*")
{
  m_debugLevel=0;
  m_fileCompressionLevel=1;
  clearData();
}


MonitoringFile::
MonitoringFile( std::string fileName )
  : m_file(0),m_mergeMatchHistoRE(0),m_mergeMatchDirRE(0),
    m_mergeMatchHistoREString(".*"),m_mergeMatchDirREString(".*")
{
  m_debugLevel=0;
  m_fileCompressionLevel=1;
  clearData();
  setFile( fileName );
}


MonitoringFile::
~MonitoringFile()
{
  dqi::DisableMustClean disabled;
  //  bool useRecursiveDelete = gROOT->MustClean();
  //  gROOT->SetMustClean(false);
  
  delete m_file;
  delete m_mergeMatchDirRE;
  delete m_mergeMatchHistoRE;
  //  gROOT->SetMustClean(useRecursiveDelete);
}

bool MonitoringFile::setHistogramRegEx(const std::string& re){
  if(re.empty()){
    std::cerr<<__PRETTY_FUNCTION__<<"Warning empty regular expression string is given. Old RegEx \""<<m_mergeMatchHistoREString<<"\" is not changed"<<std::endl;
    std::cerr<<__PRETTY_FUNCTION__<<"See http://www.boost.org/doc/libs/1_42_0/libs/regex/doc/html/boost_regex/syntax.html for allowed regular expression syntax"<<std::endl;
    return false;
  }
  boost::regex *reNew=0;
  try{
    reNew=new boost::regex(re);
    // this should fail if there are any problems with re!
    std::string test("Test String");
    boost::regex_match(test,*reNew);
  }catch(std::exception& e){
    std::cerr<<__PRETTY_FUNCTION__<<"Invalid RegEx string \""<<re<<"\". Old RegEx \""<<m_mergeMatchHistoREString<<"\" is not changed"<<std::endl;
    std::cerr<<__PRETTY_FUNCTION__<<"See http://www.boost.org/doc/libs/1_42_0/libs/regex/doc/html/boost_regex/syntax.html for allowed regular expression syntax"<<std::endl;
    delete reNew;
    return false;
  }
  delete m_mergeMatchHistoRE;
  m_mergeMatchHistoREString=re;
  m_mergeMatchHistoRE=reNew;
  //delete reNew;
  m_useRE=true;
  return true;
}

bool MonitoringFile::setDirectoryRegEx(const std::string& re){
  if(re.empty()){
    std::cerr<<__PRETTY_FUNCTION__<<"Warning empty regular expression string is given. Old RegEx \""<<m_mergeMatchDirREString<<"\" is not changed"<<std::endl;
    std::cerr<<__PRETTY_FUNCTION__<<"See http://www.boost.org/doc/libs/1_42_0/libs/regex/doc/html/boost_regex/syntax.html for allowed regular expression syntax"<<std::endl;
    return false;
  }
  boost::regex *reNew=0;
  try{
    reNew=new boost::regex(re);
    boost::regex_match("Test string",*reNew);
  }catch(std::exception& e){
    std::cerr<<__PRETTY_FUNCTION__<<"Invalid RegEx string \""<<re<<"\". Old RegEx \""<<m_mergeMatchDirREString<<"\" is not changed"<<std::endl;
    std::cerr<<__PRETTY_FUNCTION__<<"See http://www.boost.org/doc/libs/1_42_0/libs/regex/doc/html/boost_regex/syntax.html for allowed regular expression syntax"<<std::endl;
    delete reNew;
    return false;
  }
  delete m_mergeMatchDirRE;
  m_mergeMatchDirREString=re;
  m_mergeMatchDirRE=reNew;
  //delete reNew;
  m_useRE=true;
  return true;
}

void
MonitoringFile::
getAllDirs( DirMap_t& dirmap, TDirectory* dir, std::string dirName )
{
  if( dir == 0 )
    return;
  
  if( dirName != "" ) {
    DirMap_t::value_type dirmapVal( dirName, dir );
    dirmap.insert( dirmapVal );
  }
  
  TIter next( dir->GetListOfKeys() );
  TKey* key;
  while( (key = dynamic_cast<TKey*>( next() )) != 0 ) {
    // don't delete TDirectories
    TObject* obj = key->ReadObj();
    TDirectory* subdir = dynamic_cast<TDirectory*>( obj );
    if( subdir != 0 ) {
      std::string subdirName( subdir->GetName() );
      std::string fName("");
      if( dirName != "" ) {
        fName += dirName;
        fName += "/";
      }
      fName += subdirName;
      getAllDirs( dirmap, subdir, fName );
    }
    else {
      delete obj;
    }
  }
}


TDirectory*
MonitoringFile::
createDir( DirMap_t& dirmap, TDirectory* dir, std::string parent, std::string path )
{
  if( dir == 0 )
    return 0;
  
  TDirectory* subdir(0);
  DirMap_t::const_iterator diter;
  std::string::size_type i = path.find_first_of('/');
  std::string fName("");
  if( parent != "" ) {
    fName += parent;
    fName += "/";
  }
  
  if( i != std::string::npos ) {
    std::string dName( path, 0, i );
		std::string pName( path, i+1, std::string::npos );
		fName += dName;
    if( dName != "" ) {
      diter = dirmap.find( fName );
      if( diter != dirmap.end() ) {
        subdir = diter->second;
      }
      else {
        subdir = dir->mkdir( dName.c_str() );
        DirMap_t::value_type dirmapVal( fName, subdir );
        dirmap.insert( dirmapVal );
      }
		}
    else {
      subdir = dir;
    }
    return createDir( dirmap, subdir, fName, pName );
  }
	
  fName += path;
    
  diter = dirmap.find( fName );
  if( diter != dirmap.end() ) {
    return diter->second;
  }
  
  subdir = dir->mkdir( path.c_str() );
  DirMap_t::value_type dirmapVal( fName, subdir );
  dirmap.insert( dirmapVal );
  return subdir;
}


TKey*
MonitoringFile::
getObjKey( TDirectory* dir, std::string path )
{
  if( dir == 0 )
    return 0;
	
  TKey* key(0);
	
	std::string::size_type i = path.find_first_of('/');
	if( i != std::string::npos ) {
		std::string dName( path, 0, i );
		std::string pName( path, i+1, std::string::npos );
		if( dName != "" ) {
			key = dir->FindKey( dName.c_str() );
			if( key != 0 ) {
				TDirectory* subDir = dynamic_cast<TDirectory*>( key->ReadObj() );
				if (subDir) {
				  return getObjKey( subDir, pName );
				} // else fall through
			}
			return 0;
		}
		return getObjKey( dir, pName );
	}
	
	return dir->FindKey( path.c_str() );

}

std::string getInputDirectory(std::string outputDirName, TFile* input, bool has_multiple_runs, std::map< TFile*, std::string >* prefixes) {
  if (! has_multiple_runs) {
    return outputDirName;
  } else {
    std::string retval(outputDirName);
    std::string::size_type sepi = retval.find("run_multiple");
    if (sepi != std::string::npos && prefixes->find(input) != prefixes->end()) {
      retval.replace(sepi, 12, (*prefixes)[input]);
    }
    return retval;
  }
}

std::string getOutputDirectory(std::string inputDirName, TFile* input, bool has_multiple_runs, std::map< TFile*, std::string >* prefixes) {
  if (! has_multiple_runs) {
    return inputDirName;
  } else {
    std::string retval(inputDirName);
    if (prefixes->find(input) == prefixes->end()) {
      return retval;
    }
    std::string::size_type sepi = retval.find((*prefixes)[input]);
    if (sepi != std::string::npos) {
      retval.replace(sepi, (*prefixes)[input].length(), "run_multiple");
    }
    return retval;
  }
}

std::string getOutputDirectory(TKey* key, TFile* input, bool has_multiple_runs, std::map< TFile*, std::string >* prefixes) {
  if (TClass::GetClass(key->GetClassName())->InheritsFrom("TDirectory")) {
    return getOutputDirectory(key->GetName(), input, has_multiple_runs, prefixes);
  } else {
    return key->GetName();
  }
}
  
// This function will populate a mapping between key names and vectors of cycles
void populateKeyMapping(TDirectory* dir, keycyclemap& kcmap) {
   TIter nextKey( dir->GetListOfKeys() );
   TKey* key;
   std::string keyName("");
   while( (key = dynamic_cast<TKey*>( nextKey() )) != 0 ) {
      kcmap[key->GetName()].push_back(key->GetCycle());
   }
}

// This function will merge objects given a name, a vector of cycles, 
// a directory, a merge type, and optionally an object to merge into.  
// Return value is a merged object.
TObject* MonitoringFile::mergeObjsMultiCycles(const std::string& keyname, 
					      const std::vector<int>& cycles,
					      TDirectory* dir,
					      std::string & mergeType,
					      TObject* obj) {
   if (cycles.size() == 0) {
      return obj;
   }
   int start_idx = 0;
   if (obj == 0) {
      TKey* key(dir->GetKey(keyname.c_str(), cycles[0]));
      obj = key->ReadObj();
      start_idx = 1;
      TH1* h = dynamic_cast<TH1*>(obj);
      if (h && !histOKToMerge(h)) {
	// histogram is damaged goods
	std::cerr << "WARNING: HISTOGRAM " << h->GetName() << " IS INTERNALLY INCONSISTENT, NOT MERGING" << std::endl;
	return obj;
      }
   }
   for (std::vector<int>::size_type idx = start_idx; 
	idx < cycles.size(); ++idx) {
      TKey* nextKey = dir->GetKey(keyname.c_str(), cycles[idx]);
      if (nextKey != 0) {
	 std::auto_ptr<TObject> nextObj(nextKey->ReadObj());
	 if (nextObj.get() == 0) {
	    std::cerr << "MonitoringFile::mergeObjsMultiCycles(): "
		      << "In directory \"" << dir->GetPath() << "\",\n"
		      << "  Object \"" << keyname << "\" cannot be read from file: skipping\n";
	    continue;
	 }
	 TH1* h = dynamic_cast<TH1*>(nextObj.get());
	 if (h && !histOKToMerge(h)) {
	   // histogram is damaged goods; even deleting it may be dangerous
	   std::cerr << "WARNING: HISTOGRAM " << h->GetName() << " IS INTERNALLY INCONSISTENT, NOT MERGING" << std::endl;
	   nextObj.release();
	   continue;
	 }
         if (h && (obj->IsA() !=  h->IsA())) {
           // problem: class types have changed ...
	   std::cerr << "WARNING: CHANGE OF CLASS TYPES FOR " << h->GetName() << ", NOT MERGING" << std::endl;
	   nextObj.release();
	   continue;
	 }
	 MonitoringFile::mergeObjs(obj, nextObj.get(), mergeType,m_debugLevel>VERBOSE?VERBOSE: (dqutils::MonitoringFile::debugLevel_t)m_debugLevel);
      } else {
	 std::cerr << "MonitoringFile::mergeObjsMultiCycles(): NULL KEY; corrupt file?" << std::endl;
      }
   }
   return obj;
}


void getListOfKeysWithName(TDirectory* dir, const std::string& keyname,
			   TCollection* target) 
{
   target->Clear();
   TIter keyit(dir->GetListOfKeys());
   TKey* key;
   while ( (key = dynamic_cast<TKey*>(keyit())) != 0 ) {
      if (keyname == key->GetName()) {
	 target->Add(key);
      }
   }
}

void populateCycleVector(const TCollection& source, std::vector<int>& target)
{
   TKey* key;
   TIter keyit(&source);
   while ( (key = dynamic_cast<TKey*>(keyit())) != 0 ) {
      target.push_back(key->GetCycle());
   }
}


// 'mergeDirectory(...)' is based on 'MergeRootfile(...)' in the hadd.cxx utility in ROOT, using
// the same algorithm for looping over files, directories and histograms.

void
MonitoringFile::
mergeDirectory( TDirectory* outputDir, const std::vector<TFile*>& inputFiles, bool has_multiple_runs, std::map< TFile*, std::string >* prefixes )
{
   typedef std::set< std::string >            ObjNameSet_t;
   typedef std::map< std::string, MetaData >  MetaDataMap_t;

   if( outputDir == 0 ) {
     return;
   }

   gROOT->cd();

   std::string outputDirPath( outputDir->GetPath() );
   std::string outputDirName( outputDirPath );
   std::string::size_type sepi = outputDirPath.find_last_of(':');
   if( sepi != std::string::npos ) {
      outputDirName = std::string( outputDirPath, sepi+2, std::string::npos );
   }
   //std::cout << outputDirName << std::endl;
   bool metadataInDir = false;
   bool targetDir=!m_useRE;
   if(m_useRE && (boost::regex_search(outputDirName,*m_mergeMatchDirRE))){
     //std::cout<<"Found target dir \""<<outputDirName<<"\""<<std::endl;
     targetDir=true;
   }
   TTree* mdTree = new TTree( "metadata", "Monitoring Metadata" );
   mdTree->SetDirectory(0);
   OutputMetadata outputmd( mdTree );

   ObjNameSet_t  mergedObjs;
   MetaDataMap_t mdMap;

   std::vector<TFile*>::const_iterator inputFilesEnd = inputFiles.end();
   for( std::vector<TFile*>::const_iterator i = inputFiles.begin(); i != inputFilesEnd; ++i ) {
      TFile* inputFile = *i;
      TDirectory* inputDir = inputFile->GetDirectory( getInputDirectory(outputDirName, inputFile, has_multiple_runs, prefixes).c_str() );
      if( inputDir == 0 ) {
         // Directory is not present in this file; try next file
         continue;
      }
      //std::cout<<"inputdir=\""<<inputDir->GetPath()<<"\""<<std::endl;
      if( inputDir == outputDir ) {
         continue;
      }

      inputDir->cd();
      fillMetaDataMap( mdMap, inputDir );
      gROOT->cd();

      TIter nextKey( inputDir->GetListOfKeys() );
      TKey* key;
      std::string keyName("");
      keycyclemap kcmap;
      populateKeyMapping(inputDir, kcmap);
                
      for (keycyclemap::iterator kcit = kcmap.begin();
	   kcit != kcmap.end();
	   ++kcit) {
	key = inputDir->GetKey(kcit->first.c_str(), kcit->second.back());
	if (! key) {
	  std::cout << "Key " << kcit->first.c_str() << ";" << kcit->second.back() << " not readable" << std::endl;
	  continue;
	}
	kcit->second.pop_back();
	keyName = getOutputDirectory(key, inputFile, has_multiple_runs, prefixes);
         ObjNameSet_t::iterator obji = mergedObjs.find( keyName );
         if( obji != mergedObjs.end() ) {
            // Object has already been processed
            continue;
         }
	 // we only need to read the non-dir objects below target directory. skip the rest
	 bool isDir=false;
	 if(m_useRE && key->IsFolder()){
	   //std::cout<<"Key "<<keyName<<" is a folder"<<std::endl;
	   if(keyName!="metadata")isDir=true; //metadata is not going to be merged, treat it as a regular obj
	 }
	 if(!targetDir && !isDir){
	   //std::cerr<<"Skipping keyname "<keyname<<std::endl;
	   continue; //skip everything except directories 
	 }
	 std::auto_ptr<TObject> obj(key->ReadObj());
         if (obj.get() == 0) {
            std::cerr << "MonitoringFile::mergeDirectory(): "
               << "In directory \"" << inputDir->GetPath() << "\",\n"
                  << "  Object \"" << key->GetName() << "\" cannot be read from file: skipping\n";
            continue;
         }

	 // we actually have an object, and it hasn't been done before 
         mergedObjs.insert( keyName );

         TH1* h(0);
         TGraph* g(0);
         TDirectory* d(0);
         TTree* t(0);
         TObject* targetObj(0);
	 //moved inside if to speedup
//          h = dynamic_cast<TH1*>( obj.get() );
//          d = dynamic_cast<TDirectory*>( obj.get() );
//          g = dynamic_cast<TGraph*>( obj.get() );
//          t = dynamic_cast<TTree*>( obj.get() );

         if((targetDir)&&((h = dynamic_cast<TH1*>( obj.get() )) ||  //merge only objects below target directory
	     (g = dynamic_cast<TGraph*>( obj.get() ))|| 
		((keyName != "metadata") && (t = dynamic_cast<TTree*>( obj.get() )))))  {
	   //skip cases where regexp doesn't match object name, all directories are processed by default
	   if(m_useRE){
	     if(!boost::regex_search(keyName,*m_mergeMatchHistoRE)){
	       //std::cerr<<" skipping   keyName=\""<<outputDirName+"/"+keyName<<"\""<<std::endl;
	       continue; // not the histogram we want
	     }
	   }
	   std::string mergeType("<default>");
	   MetaDataMap_t::iterator mdi = mdMap.find( keyName );
	   if( mdi != mdMap.end() ) {
               metadataInDir = true;
               const MetaData& md = mdi->second;
               if (has_multiple_runs && 
		   (md.interval != "run" && 
		    md.interval != "fill" && 
		    md.interval != "all" && 
		    md.interval != "file")){
		 continue;
               }
               outputmd.fill( md.name, md.interval, md.chain, md.merge );
               mergeType = md.merge;
               if( g && (md.merge != "<default>") ) {
                  std::cerr << "MonitoringFile::mergeDirectory(): "
                     << "In directory \"" << inputDir->GetPath() << "\",\n"
                        << "  TGraph \"" << keyName << "\" requests merging type " << mergeType
                        << " but only default merging implemented for TGraphs\n";
               }
               if( t && (md.merge != "<default>") ) {
                  std::cerr << "MonitoringFile::mergeDirectory(): "
                     << "In directory \"" << inputDir->GetPath() << "\",\n"
                        << "  TTree \"" << keyName << "\" requests merging type " << mergeType
                        << " but only default merging implemented for TTrees\n";
               }
            }else {
               std::cerr << "MonitoringFile::mergeDirectory(): "
                  << "In directory \"" << inputDir->GetPath() << "\",\n"
                     << "  object \"" << keyName << "\" has no metadata\n";
            }
 	    if(t){
	      TDirectory* currentDir = gDirectory;
	      outputDir->cd();
	      TTree* t2 = t->CloneTree();
	      targetObj = t2;
	      currentDir->cd();
	    } else {
	      targetObj = obj.get();
	    }
	    mergeObjsMultiCycles(keyName, kcit->second, inputDir,
				 mergeType, targetObj);
            for( std::vector<TFile*>::const_iterator j = i+1; j!= inputFilesEnd; ++j ) {
               TFile* nextInputFile = *j;
               TDirectory* nextInputDir = nextInputFile->GetDirectory( getInputDirectory(outputDirName, *j, has_multiple_runs, prefixes).c_str() );
               if( nextInputDir == 0 ) {
                  // Directory is not present in this file; try next file
                  continue;
               }

	       TObjArray tl; std::vector<int> nextCycles;
	       getListOfKeysWithName(nextInputDir, kcit->first, &tl);
	       populateCycleVector(tl, nextCycles);
		 
	       mergeObjsMultiCycles(kcit->first, nextCycles, 
	       			    nextInputDir, mergeType, targetObj);

            }
            outputDir->cd();
	    if (h) {
	      h->SetDirectory(outputDir);
	    }

	    targetObj->Write();
         }else if( (targetDir) && (t = dynamic_cast<TTree*>(obj.get())) ) {
	    // do not merge metadata
	 }else if((d = dynamic_cast<TDirectory*>( obj.get() ))) {
	   // Do not run the multicycle merge on directories;
	   // haven't seen one yet that has multiple keys...
	   // Merge TDirectory
	   outputDir->cd();
	   std::string outputSubDirName(getOutputDirectory(d->GetName(), inputFile, has_multiple_runs, prefixes));
	   TDirectory* outputSubDir = outputDir->mkdir( outputSubDirName.c_str(), d->GetTitle() );
	   mergeDirectory( outputSubDir, inputFiles, has_multiple_runs, prefixes );
         }else {
	   std::cout << "MonitoringFile::mergeDirectory(): "
		     << "In directory \"" << inputDir->GetPath() << "\",\n"
		     << "  Object \"" << key->GetName() << "\" will not be merged\n";
         }

         //delete obj;
      }
   }

   // Write metadata TTree to output directory
   if( metadataInDir ) {
      outputDir->cd();
      mdTree->SetDirectory(outputDir);
      mdTree->Write();
   }
   delete mdTree;
}


void
MonitoringFile::
fillMetaDataMap( std::map<std::string,dqutils::MonitoringFile::MetaData>& mdMap, TDirectory* dir )
{
  if (dir == 0) return;
  //  TKey *mdKey = dynamic_cast<TKey*>(dir->GetListOfKeys()->FindObject("metadata"));
  //if (mdKey == 0) return;
  //TTree *md = dynamic_cast<TTree*>(mdKey->ReadObj());
  TTree *md = dynamic_cast<TTree*>(dir->Get("metadata"));
  if (md == 0) return;

  char* i_name = new char[100];
  char* i_interval = new char[100];
  char* i_chain = new char[100];
  char* i_merge = new char[100];

  md->SetBranchStatus( "Name", 1 );
  md->SetBranchAddress( "Name", i_name );
  md->SetBranchStatus( "Interval", 1 );
  md->SetBranchAddress( "Interval", i_interval );
  md->SetBranchStatus( "TriggerChain", 1 );
  md->SetBranchAddress( "TriggerChain", i_chain );
  md->SetBranchStatus( "MergeMethod", 1 );
  md->SetBranchAddress( "MergeMethod", i_merge );
  int counter = 0;
  int nEntries = int( md->GetEntries() );

  while( counter < nEntries ) {
    try {
      md->GetEntry(counter);
    }
    catch( const std::exception& e ) {
      std::cerr << "Exception: \"" << e.what() << "\" in directory \""
                << dir->GetName() << "\"\n" << std::flush;
      return;
    }

    std::string nameStr(i_name);
    if( mdMap.find(nameStr) == mdMap.end() ) {
      MetaData md( i_name, i_interval, i_chain, i_merge );
      std::map<std::string,MetaData>::value_type mdVal( nameStr, md );
      mdMap.insert( mdVal );
    }
    ++counter;
  }
  delete [] i_name;
  delete [] i_interval;
  delete [] i_chain;
  delete [] i_merge;
  delete md;
}


// 'mergeFiles(...)' is based on the hadd.cxx utility distributed with ROOT

void
MonitoringFile::
mergeFiles( std::string outFileName, const std::vector<std::string>& files )
{
  dqi::DisableMustClean disabled;
  TH1::AddDirectory(false);
  if(m_useRE){
    std::cout<<" ========== Using regular expressions for selective merging ========== "<<std::endl;
    std::cout<<" Directory selection RE=\""<<m_mergeMatchDirREString<<"\""<<std::endl;
    std::cout<<" Object selection RE=\""<<m_mergeMatchHistoREString<<"\""<<std::endl;
  }
  typedef std::vector<TFile*>  TFileList_t;
  typedef std::map<TFile*, std::string> PrefixIgnore_t;
  
  std::auto_ptr<TFile> outfile( TFile::Open(outFileName.c_str(),"RECREATE",outFileName.c_str(),m_fileCompressionLevel) );
  if( outfile.get() == 0 ) {
    std::cerr << "MonitoringFile::mergeFiles(): "
              << "Output file not opened\n";
    return;
  }
  
  TFileList_t tfiles;
  PrefixIgnore_t prefix_ignore;
  
  std::vector< std::string >::const_iterator filesEnd = files.end();
  std::vector< std::string >::const_iterator fi;
  for( fi = files.begin(); fi != filesEnd; ++fi ) {
    std::cout << "Opening file: " << *fi << "\n";
    TFile* f = TFile::Open( fi->c_str() );
    if( f == 0 || ! f->IsOpen()) {
      std::cerr << "MonitoringFile::mergeFiles(): "
                << "Input file not opened\n";
      delete f;
      continue;
    }
    TList* keys = f->GetListOfKeys();
    if (keys->GetSize() == 0) {
      std::cerr << "MonitoringFile::mergeFiles(): "
		<< "Input file " << *fi << " has no keys!" 
		<< std::endl;
      delete f;
      continue;
    } 
    std::vector< std::string > runkeys;
    TIter keyitr(keys);
    TKey* key;
    
    while ( (key = dynamic_cast<TKey*>(keyitr())) != 0 ) {
      std::string keyname(key->GetName());
      if (keyname.substr(0,4) == "run_") {
	runkeys.push_back(keyname);
      }
    }
    if (runkeys.size() > 1) {
      std::cerr << "MonitoringFile::mergeFiles():\n"
		<< "   Input root file " << *fi << " has multiple top-level run_* keys\n"
		<< "   Assuming " << runkeys[0] << " is the run key" 
		<< std::endl;
    }
    if (runkeys.size() > 0) {
      prefix_ignore[f] = runkeys[0];
    }
    tfiles.push_back(f);
  }

  // check to see if all run keys are the same
  bool has_multiple_runs = false;
  std::string prev_key_name("");

  for (PrefixIgnore_t::const_iterator pi_it = prefix_ignore.begin();
       pi_it != prefix_ignore.end();
       ++pi_it) {
    if (prev_key_name != pi_it->second) {
      if (prev_key_name == "") {
	prev_key_name = pi_it->second;
      } else {
	has_multiple_runs = true;
	break;
      }
    }
  }

  if (has_multiple_runs) {
    std::cout << "Multiple runs detected in input files.\n"
	      << "Will merge ONLY RUN, FILL, FILE, or ALL histograms to run_multiple directory.\n"
	      << "(That is, no event block, lumi block, lowStat, medStat,\n"
	      << "or higStat histograms will be in the output.)"
	      << std::endl;
  }
  
  std::cout << "Writing file: " << outFileName << "\n";
  std::cout << "\nWarning messages from merging follow:\n\n";
  
  mergeDirectory( outfile.get(), tfiles, has_multiple_runs, &prefix_ignore );
  
  outfile->Close();
  
  TFileList_t::const_iterator tfilesEnd = tfiles.end();
  TFileList_t::const_iterator tfi;
  for( tfi = tfiles.begin(); tfi != tfilesEnd; ++tfi ) {
    TFile* tf = *tfi;
    tf->Delete("");
    tf->Close();
    delete tf;
  }

  std::cout << "\n";
  std::cout << "****************************************\n\n";
}


void
MonitoringFile::
mergeFiles( std::string outFileName, std::string listFileName )
{
  typedef std::vector< std::string >  FileList_t;
  
  const unsigned int nFilesAtOnce = 50;
  
  FileList_t allFiles;
  bool success = setListFromFile( allFiles, listFileName );
  if( !success ) {
    std::cerr << "MonitoringFile::mergeFiles(): Cannot merge files\n";
    return;
  }
  
  if( allFiles.size() <= nFilesAtOnce ) {
    mergeFiles( outFileName, allFiles );
    return;
  }
  
  FileList_t procFiles, tmpIntermediateFiles;

  
  FileList_t::const_iterator filesEnd = allFiles.end();
  FileList_t::const_iterator fi       = allFiles.begin();
  
  unsigned int counter = 0;
  std::string tmpInputFile("");
  std::string tmpOutputFile("");
  
  // new logic: merge intermediately, then merge intermediate files
  while( fi != filesEnd ) {
    
    procFiles.push_back(*fi);
    ++counter; ++fi;
    if ( counter % nFilesAtOnce == 0 || fi == filesEnd ) {
      std::ostringstream nameStream;
      nameStream << "tmp_merge_" << counter << ".root";
      tmpOutputFile = nameStream.str();
      tmpIntermediateFiles.push_back(tmpOutputFile);
      mergeFiles( tmpOutputFile, procFiles );
      procFiles.clear();
      }
    /*    
    if( counter == 0 ) {
      tmpOutputFile = *fi;
    }
    
    ++counter;
    ++fi;
    
    if( counter % nFilesAtOnce == 0 || fi == filesEnd ) {
      tmpInputFile = tmpOutputFile;
      procFiles.push_back( tmpInputFile );
      std::ostringstream nameStream;
      nameStream << "tmp_merge_" << counter << ".root";
      tmpOutputFile = nameStream.str();
      mergeFiles( tmpOutputFile, procFiles );
      if( counter > nFilesAtOnce ) {
        std::remove( tmpInputFile.c_str() );
      }
      procFiles.clear();
    }
    
    if( fi != filesEnd ) {
      procFiles.push_back( *fi );
    }
    */ 
  }

  /*  
  if( std::rename(tmpOutputFile.c_str(),outFileName.c_str()) == 0 ) {
    std::cout << "Renaming " << tmpOutputFile << " as " << outFileName << "\n";
  }
  else {
    std::cerr << "Cannot rename " << tmpOutputFile << " as " << outFileName << "\n";
  }
  */
  mergeFiles(outFileName, tmpIntermediateFiles);
  for (const auto& tmpFile : tmpIntermediateFiles) {
    std::remove(tmpFile.c_str());
    }
}



bool
MonitoringFile::
setFile( std::string fileName )
{
  clearData();
  m_file = TFile::Open( fileName.c_str() );
  if( m_file != 0 )
    return true;
  
  return false;
}


void
MonitoringFile::
printDirectories() const
{
  if( m_file == 0 ) {
    std::cerr << "MonitoringFile::printDirectories(): "
              << "No input file is open\n";
    return;
  }
  
  DirMap_t indirmap;
  
  getAllDirs( indirmap, m_file, "" );
  
  DirMap_t::const_iterator idirend = indirmap.end();
  for( DirMap_t::const_iterator idir = indirmap.begin(); idir != idirend; ++idir ) {
    std::string idirName = idir->first;
    std::cout << idirName << "\n";
  }
}


void
MonitoringFile::
printStatistics() const
{
  if( m_file == 0 ) {
    std::cerr << "MonitoringFile::printStatistics(): "
              << "No input file is open\n";
    return;
  }
  
  DirMap_t indirmap;
  
  getAllDirs( indirmap, m_file, "" );
  
  DirMap_t::const_iterator idirend = indirmap.end();
  for( DirMap_t::const_iterator idir = indirmap.begin(); idir != idirend; ++idir ) {
    std::string idirName = idir->first;
    
    GatherStatistics stat_shift( idirName );
    GatherStatistics stat_all( idirName );
    
    loopOnHistogramsInMetadata( stat_shift, idir->second );
    loopOnHistograms( stat_all, idir->second );
    
    std::cout.setf(std::ios_base::left,std::ios_base::adjustfield);
    std::cout.width(80);
    std::cout << idirName << "  ";
    
    std::cout.setf(std::ios_base::right,std::ios_base::adjustfield);
    std::cout << "  shift: ";
    std::cout.width(3);
    std::cout << stat_shift.m_nHist1D << " ";
    std::cout.width(5);
    std::cout << stat_shift.m_nHist1DBins << " ";
    std::cout.width(3);
    std::cout << stat_shift.m_nHist2D << " ";
    std::cout.width(7);
    std::cout << stat_shift.m_nHist2DBins << " ";
    std::cout.width(3);
    std::cout << stat_shift.m_nGraph  << " ";
    std::cout.width(5);
    std::cout << stat_shift.m_nGraphPoints << "   ";
    
    
    std::cout << "  all: ";
    std::cout.width(3);
    std::cout << stat_all.m_nHist1D << " ";
    std::cout.width(5);
    std::cout << stat_all.m_nHist1DBins << " ";
    std::cout.width(3);
    std::cout << stat_all.m_nHist2D << " ";
    std::cout.width(7);
    std::cout << stat_all.m_nHist2DBins << " ";
    std::cout.width(3);
    std::cout << stat_all.m_nGraph  << " ";
    std::cout.width(5);
    std::cout << stat_all.m_nGraphPoints << "\n";
    
    std::cout << std::flush;
  }
}


bool
MonitoringFile::
copyHistograms( std::string outFileName, std::string dirName )
{
  dqi::DisableMustClean disabled;
  //  bool useRecursiveDelete = gROOT->MustClean();
  //  gROOT->SetMustClean(false);
  
  if( m_file == 0 ) {
    std::cerr << "MonitoringFile::copyHistograms(): "
              << "No input file is open\n";
    return false;
  }
  
  DirMap_t indirmap;
  DirMap_t reducedmap;
  DirMap_t outdirmap;
  
  if( dirName != "all" ) {
    TKey* dkey = getObjKey( m_file, dirName );
    if( dkey == 0 ) {
      std::cerr << "MonitoringFile::copyHistograms(): "
                << "Directory \'" << dirName << "\' not found in input file\n";
      return false;
    }
    
    TDirectory* fromDir = dynamic_cast<TDirectory*>(dkey->ReadObj());
    
    DirMap_t::value_type dirmapVal( dirName, fromDir );
    indirmap.insert( dirmapVal );
  }
  else {
    std::cout << "Building list of all TDirectories in file...\n" << std::flush;
    getAllDirs( indirmap, m_file, "" );
  }
  
  DirMap_t::const_iterator idirend = indirmap.end();
  for( DirMap_t::const_iterator idir = indirmap.begin(); idir != idirend; ++idir ) {
    
    std::string idirName = idir->first;
    std::cout << "Checking " << idirName << "\n" << std::flush;
    //std::string::size_type j = idirName.find( "L1Calo/1_PPr_EmFADCTiming" );
    //if( j != std::string::npos ) {
    //  std::cerr << "Skipping directory \"" << idirName << "\"\n";
    //  std::cerr << std::flush;
    //  continue;
    //}
    
    if( !dirHasHistogramsInMetadata(idir->second) ) {
        continue;
    }
    
    reducedmap.insert( *idir );
  }
  
  std::auto_ptr<TFile> outfile( TFile::Open(outFileName.c_str(),"RECREATE",outFileName.c_str(),m_fileCompressionLevel) );
  if( outfile.get() == 0 ) {
    std::cerr << "MonitoringFile::copyHistograms(): "
              << "Output file not opened\n";
    return false;
  }
  
  idirend = reducedmap.end();
  for( DirMap_t::const_iterator idir = reducedmap.begin(); idir != idirend; ++idir ) {
  
    std::string idirName = idir->first;
    std::cout << "Processing " << idirName << "\n" << std::flush;
    
    TDirectory* toDir = createDir( outdirmap, outfile.get(), "", idirName );
    if( toDir == 0 ) {
      std::cerr << "MonitoringFile::copyHistograms(): "
                << "Directory \'" << idirName << "\' not created in output file\n";
      return false;
    }
    
    CopyHistogram copyFcn( toDir, idirName );    
    
    loopOnHistogramsInMetadata( copyFcn, idir->second );
  }
  
  outfile->Write();
  outfile->Close();
  
  //  gROOT->SetMustClean(useRecursiveDelete);
  return true;
}



std::string
MonitoringFile::
getHanResults( std::string hanResultsDir, std::string input,
               std::string hcfg, std::string hcfg_lowStat, std::string hcfg_medStat )
{
  //DisableMustClean disabled;
  
  std::cout << "\nUsing han configurations:\n"
            << "  entire run: " << hcfg << "\n"
            << "  low stat interval: " << hcfg_lowStat << "\n"
            << "  medium stat interval: " << hcfg_medStat << "\n\n" << std::flush;
  
  
  TFile* infile = TFile::Open( input.c_str() );
  if( infile == 0 ) {
    std::cerr << "MonitoringFile::getHanResults(): "
              << "Cannot open input file \"" << input << "\"\n";
    return "";
  }
  
  std::vector<std::string> run_dirs;
  std::vector<std::string> lowStat_dirs;
  std::vector<std::string> medStat_dirs;
  
  TIter next_run( infile->GetListOfKeys() );
  TKey* key_run(0);
  while( (key_run = dynamic_cast<TKey*>( next_run() )) != 0 ) {
    TObject* obj_run = key_run->ReadObj();
    TDirectory* tdir_run = dynamic_cast<TDirectory*>( obj_run );
    if( tdir_run != 0 ) {
      std::string tdir_run_name( tdir_run->GetName() );
      if( tdir_run_name.find("run") != std::string::npos ) {
        run_dirs.push_back( tdir_run_name );
        TIter next_minutes( tdir_run->GetListOfKeys() );
        TKey* key_minutes(0);
        while( (key_minutes = dynamic_cast<TKey*>( next_minutes() )) != 0 ) {
          TObject* obj_minutes = key_minutes->ReadObj();
          TDirectory* tdir_minutes = dynamic_cast<TDirectory*>( obj_minutes );
          if( tdir_minutes != 0 ) {
            std::string tdir_minutes_name( tdir_minutes->GetName() );
            if( tdir_minutes_name.find("lowStat") != std::string::npos ) {
              lowStat_dirs.push_back( tdir_run_name + "/" + tdir_minutes_name );
            }
            else if( tdir_minutes_name.find("medStat") != std::string::npos ) {
              medStat_dirs.push_back( tdir_run_name + "/" + tdir_minutes_name );
            }
          }
          delete obj_minutes;
        }
      }
    }
    delete obj_run;
  }
  
  delete infile;
  
  dqi::HanApp han;
  
  std::string fileList =" ";
  std::vector<std::string>::const_iterator dirs_end;
  std::vector<std::string>::const_iterator dir;
  
  dirs_end = run_dirs.end();
  for( dir = run_dirs.begin(); dir != dirs_end; ++dir ) {
    const std::string& tdir_run_name = *dir;
    std::string han_output_run = hanResultsDir+"/"+tdir_run_name+"_han.root";
    std::cout << "Calling han( " << hcfg << ", " << input << ", " << tdir_run_name
              << ", " << han_output_run << " ):\n" << std::flush;
	  han.Analyze( hcfg, input, han_output_run, tdir_run_name );
    std::cout << "\n";
	  fileList += han_output_run + " " + tdir_run_name + "\n" ;
  }
  
  dirs_end = lowStat_dirs.end();
  for( dir = lowStat_dirs.begin(); dir != dirs_end; ++dir ) {
    const std::string& tdir_minutes_path = *dir;
    
    std::string tdir_minutes_underscore = tdir_minutes_path;
    std::string::size_type tdir_minutes_i = tdir_minutes_underscore.find("/");
    tdir_minutes_underscore.replace( tdir_minutes_i, 1, "_" );
    
    std::string han_output_lowStat = hanResultsDir+"/"+tdir_minutes_underscore+"_han.root";
    std::cout << "Running han, writing to " << han_output_lowStat << ":\n" << std::flush;
	  han.Analyze( hcfg_lowStat, input, han_output_lowStat, tdir_minutes_path );
    std::cout << "\n";
    std::string subdirname( tdir_minutes_path, tdir_minutes_i+1, std::string::npos );
    std::string dirname( tdir_minutes_path, 0, tdir_minutes_i );
	  fileList += han_output_lowStat + " " + subdirname + " " + dirname + " " + subdirname +"\n" ;
  }
  
  dirs_end = medStat_dirs.end();
  for( dir = medStat_dirs.begin(); dir != dirs_end; ++dir ) {
    const std::string& tdir_minutes_path = *dir;
    
    std::string tdir_minutes_underscore = tdir_minutes_path;
    std::string::size_type tdir_minutes_i = tdir_minutes_underscore.find("/");
    tdir_minutes_underscore.replace( tdir_minutes_i, 1, "_" );
    
    std::string han_output_medStat = hanResultsDir+"/"+tdir_minutes_underscore+"_han.root";
    std::cout << "Running han, writing to " << han_output_medStat << ":\n" << std::flush;
	  han.Analyze( hcfg_medStat, input, han_output_medStat, tdir_minutes_path );
    std::cout << "\n";
    std::string subdirname( tdir_minutes_path, tdir_minutes_i+1, std::string::npos );
    std::string dirname( tdir_minutes_path, 0, tdir_minutes_i );
	  fileList += han_output_medStat + " " + subdirname + " " + dirname + " " + subdirname +"\n" ;
  }
  
  return fileList;
}


void
MonitoringFile::
printHanConfig() const
{
  if( m_file == 0 ) {
    std::cerr << "MonitoringFile::printHanConfig(): "
              << "No input file is open\n";
    return;
  }
  
  DirMap_t indirmap;
  
  getAllDirs( indirmap, m_file, "" );

  std::string indent, indent_p,indent_c;
  std::string idirName_p;
  DirMap_t::const_iterator idirend = indirmap.end();
  for( DirMap_t::const_iterator idir = indirmap.begin(); idir != idirend; ++idir ) {
    std::string idirName = idir->first;
    std::string::size_type shortNameIndex = idirName.rfind( '/' );
    std::string shortName = idirName.substr( shortNameIndex+1, std::string::npos );
    
    std::string::size_type fsIndex = idirName.find( '/' );
    std::string shortPath;
    if( fsIndex != shortNameIndex ) 
      shortPath = idirName.substr( fsIndex+1, shortNameIndex );
    else
      shortPath = idirName.substr( fsIndex+1, std::string::npos );
    
    std::cout << idirName << "\n";
    std::cout << shortPath << ", " << shortName << "\n";
    /*
    indent = getIndentation(idirName,"");
    if(int(indent.size())==in_p){
      std::cout << indent << "} \n";
      std::cout << indent << "dir "  << shortName << "  { \n";
      std::cout << indent << "  output " << idirName << "\n";
      std::cout << indent << "  hist all_in_dir  { \n " << indent << " } \n";
    }
    else if (int(indent.size()) > in_p){
      std::cout << indent << "dir "  << shortName << "  { \n";
      std::cout << indent << "  output " << idirName << "\n";
      std::cout << indent << "  hist all_in_dir  { \n " << indent << " } \n";
    }
    else{
      //find common part + number of common "/"
      std::string common = FindCommon(idirName,idirName_p);
      indent_c = getIndentation(common,"");
      int counter = (indent_p.size() - indent_c.size())/2;
      for (int i = counter; i>0; i--){
	std::string temp = indent_c;
	for (int j = 0; j< i; j++){
	  temp+="  ";
	}
	std::cout << temp << "} \n" ;
      }
      std::cout << indent << "} \n";
      std::cout << indent << "dir "  << shortName << "  { \n";
      std::cout << indent << "  output " << idirName << "\n";
      std::cout << indent << "  hist all_in_dir  { \n " << indent << " } \n";
    }
    indent_p = indent;
    in_p = indent_p.size();
    idirName_p = idirName;  
    */
  }  
}



std::string
MonitoringFile:: 
getIndentation( std::string pathName, std::string leadingSpace )
{
  std::string space = leadingSpace;
  std::string::size_type i = pathName.find_first_of('/');
  if( i != std::string::npos ) {
    std::string subPath( pathName, i+1, std::string::npos );
    space += "  ";
    return getIndentation( subPath, space );
  }
  return space;
}

std::string 
MonitoringFile::
FindCommon(std::string name1,std::string name2) const
{
  int length = ( name1.size() < name2.size() ) ? name1.size() : name2.size();
  bool found = true;
  int count = 0;
  while (found==true && count < length){
    if (name1[count]==name2[count]){
      count++;
    }
    else {
      found = false;
    }
  }
  return (name1.substr(0,count));
}



// *********************************************************************
// Protected Methods
// *********************************************************************

MonitoringFile::CopyHistogram::
CopyHistogram( TDirectory* target, std::string dirName )
  : m_target(target)
  , m_dirName(dirName)
  , m_charArrSize(1000)
  , m_metadata(0)
  , m_nameData(0)
  , m_intervalData(0)
  , m_chainData(0)
  , m_mergeData(0)
{
  m_metadata = new TTree( "metadata", "Monitoring Metadata" );
  m_metadata->SetDirectory(0);
  m_nameData = new char[m_charArrSize];
  m_intervalData = new char[m_charArrSize];
  m_chainData = new char[m_charArrSize];
  m_mergeData = new char[m_charArrSize];
  m_metadata->Branch( "Name", m_nameData, "Name/C" );
  m_metadata->Branch( "Interval", m_intervalData, "Interval/C" );
  m_metadata->Branch( "TriggerChain", m_chainData, "TriggerChain/C" );
  m_metadata->Branch( "MergeMethod", m_mergeData, "MergeMethod/C" );
}


MonitoringFile::CopyHistogram::
~CopyHistogram()
{
  m_target->cd();
  m_metadata->SetDirectory(m_target);
  m_metadata->Write();
  delete m_metadata;
  delete [] m_nameData;
  delete [] m_intervalData;
  delete [] m_chainData;
  delete [] m_mergeData;
}


bool
MonitoringFile::CopyHistogram::
execute( TH1* hist )
{
  m_target->cd();
  hist->SetDirectory(m_target);
  hist->Write();
  
  return true;
}


bool
MonitoringFile::CopyHistogram::
execute( TGraph* graph )
{
  m_target->cd();
  graph->Write();
  
  return true;
}


bool
MonitoringFile::CopyHistogram::
executeMD( TH1* hist, const MetaData& md )
{
  m_target->cd();
  hist->SetDirectory(m_target);
  hist->Write();
  
  copyString( m_nameData, md.name );
  copyString( m_intervalData, md.interval );
  copyString( m_chainData, md.chain );
  copyString( m_mergeData, md.merge );
  m_metadata->Fill();
  
  return true;
}


bool
MonitoringFile::CopyHistogram::
executeMD( TGraph* graph, const MetaData& md )
{
  m_target->cd();
  graph->Write();
  
  copyString( m_nameData, md.name );
  copyString( m_intervalData, md.interval );
  copyString( m_chainData, md.chain );
  copyString( m_mergeData, md.merge );
  m_metadata->Fill();
  
  return true;
}


void
MonitoringFile::CopyHistogram::
copyString( char* to, const std::string& from )
{
  int i = 0;
	const char* f = from.c_str();
	while( ++i < m_charArrSize && (*to++ = *f++) != 0 )
	  ;
	if( i == m_charArrSize ) {
		*to = 0;
	}
}


MonitoringFile::GatherStatistics::
GatherStatistics( std::string dirName )
  : m_dirName(dirName)
  , m_nHist1D(0)
  , m_nHist1DBins(0)
  , m_nGraph(0)
  , m_nGraphPoints(0)
  , m_nHist2D(0)
  , m_nHist2DBins(0)
{
}


bool
MonitoringFile::GatherStatistics::
execute( TH1* hist )
{
  TH2* hist2d = dynamic_cast<TH2*>( hist );
  if( hist2d != 0 ) {
    ++m_nHist2D;
    m_nHist2DBins += (hist2d->GetNbinsX() * hist2d->GetNbinsY());
    return true;
  }
  ++m_nHist1D;
  m_nHist1DBins += hist->GetNbinsX();
  return true;
}


bool
MonitoringFile::GatherStatistics::
execute( TGraph* graph )
{
  ++m_nGraph;
  m_nGraphPoints += graph->GetMaxSize();
  return true;
}


MonitoringFile::GatherNames::
GatherNames()
{
}


bool
MonitoringFile::GatherNames::
execute( TH1* hist )
{
  m_names.push_back( std::string(hist->GetName()) );
  return true;
}


bool
MonitoringFile::GatherNames::
execute( TGraph* graph )
{
  m_names.push_back( std::string(graph->GetName()) );
  return true;
}


void
MonitoringFile::
clearData()
{
  dqi::DisableMustClean disabled;
  //  bool useRecursiveDelete = gROOT->MustClean();
  //  gROOT->SetMustClean(false);
  
  delete m_file;
  m_file = 0;
  m_debugLevel=0;
  m_fileCompressionLevel=1;
  delete m_mergeMatchHistoRE;
  delete m_mergeMatchDirRE;
  m_mergeMatchHistoREString=".*";
  m_mergeMatchDirREString=".*";
  m_mergeMatchHistoRE=new boost::regex(m_mergeMatchHistoREString);
  m_mergeMatchDirRE=new boost::regex(m_mergeMatchDirREString);
  m_useRE=false;
  //  gROOT->SetMustClean(useRecursiveDelete);
}


bool
MonitoringFile::
dirHasHistogramsInMetadata( TDirectory* dir )
{
  dir->cd();
  
  TKey* mdKey = dir->FindKey( "metadata" );
  if( mdKey == 0 ) {
    return false;
  }
  
  TTree* md = dynamic_cast<TTree*>( mdKey->ReadObj() );
  if ( md == 0 ) {
    return false;
  }
  
  int counter = 0;
  int nEntries = int( md->GetEntries() );
  
  while( counter < nEntries ) {
    try {
      md->GetEntry(counter);
    }
    catch( const std::exception& e ) {
      std::cerr << "Exception: \"" << e.what() << "\" in directory \""
                << dir->GetName() << "\"\n" << std::flush;
      return false;
    }
    
    return true;
    ++counter;
  }
  
  return false;
}


void
MonitoringFile::
loopOnHistograms( HistogramOperation& fcn, TDirectory* dir )
{
  TIter next( dir->GetListOfKeys() );
  TKey* key;
  while( (key = dynamic_cast<TKey*>( next() )) != 0 ) {
    TObject* obj = key->ReadObj();
    TH1* h = dynamic_cast<TH1*>( obj );
    if( h != 0 ) {
      fcn.execute( h );
    }
    else {
      TGraph* g = dynamic_cast<TGraph*>( obj );
      if( g != 0 ) {
        fcn.execute( g );
      }
    }
    delete obj;
  }
}


bool
MonitoringFile::
loopOnHistogramsInMetadata( HistogramOperation& fcn, TDirectory* dir )
{
  dir->cd();
  TKey* mdKey = dir->FindKey( "metadata" );
  if( mdKey == 0 ) {
    //std::cerr << "MonitoringFile::loopOnHistogramsInMetadata(): "
    //          << "No \'metadata\' object found in directory \"" << dir->GetName() << "\"\n";
    return false;
  }
  
  TTree* md = dynamic_cast<TTree*>( mdKey->ReadObj() );
  if ( md == 0 ) {
    return false;
  }
  
  char* i_name = new char[100];
  char* i_interval = new char[100];
  char* i_chain = new char[100];
  char* i_merge = new char[100];
  TKey* i_key;
  
  md->SetBranchStatus( "Name", 1 );
  md->SetBranchAddress( "Name", i_name );
  md->SetBranchStatus( "Interval", 1 );
  md->SetBranchAddress( "Interval", i_interval );
  md->SetBranchStatus( "TriggerChain", 1 );
  md->SetBranchAddress( "TriggerChain", i_chain );
  md->SetBranchStatus( "MergeMethod", 1 );
  md->SetBranchAddress( "MergeMethod", i_merge );
  
  int counter = 0;
  int nEntries = int( md->GetEntries() );
  
  while( counter < nEntries ) {
    dir->cd();
    try {
      md->GetEntry(counter);
    }
    catch( const std::exception& e ) {
      std::cerr << "Exception: \"" << e.what() << "\" in directory \""
                << dir->GetName() << "\"\n" << std::flush;
      return false;
    }
    
    i_key = dir->FindKey( i_name );
    if( i_key == 0 ) {
      std::cerr << "MonitoringFile::loopOnHistogramsInMetadata(): "
		<< "No \'" << i_name << "\' object found\n";
      return false;
    }
    MetaData md( i_name, i_interval, i_chain, i_merge );
    TObject* obj = i_key->ReadObj();
    TH1* h = dynamic_cast<TH1*>( obj );
    if( h != 0 ) {
      fcn.executeMD( h, md );
    }
    else {
      TGraph* g = dynamic_cast<TGraph*>( obj );
      if( g != 0 ) {
	fcn.executeMD( g, md );
      }
    }
    delete obj; 
    ++counter;
  }
  
  delete [] i_name;
  delete [] i_interval;
  delete [] i_chain;
  delete [] i_merge;
  delete md;
  
  return true;
}


bool
MonitoringFile::
setListFromFile( std::vector<std::string>& filelist, const std::string& listFileName )
{
  using namespace std;
  
  filelist.clear();
  
  ifstream listfile( listFileName.c_str() );
  if(!listfile) {
    cerr << "MonitoringFile::setListFromFile(): "
         << "cannot read from file: " << listFileName << "\n";
    return false;
  }
  
  string line;
  char c;
  string filename;
  while( getline(listfile,line) ) {
    istringstream linestream(line);
    while(linestream.get(c)) {
      if(!isspace(c)) {
        // ignore comments
        if(c == '#') {
          break;
        }
        
        linestream.putback(c);
        linestream >> filename;
        if(!linestream) {
          cerr << "MonitoringFile::setListFromFile(): "
               << "badly formatted line: " << line << "\n";
          break;
        }
        
        filelist.push_back(filename);
      }
    }
  }
  
  return true;
}

int MonitoringFile::mergeObjs(TObject *objTarget, TObject *obj, std::string mergeType, debugLevel_t debugLevel) {
   // merge obj into objTarget based on method specified in mergeType
   // copied in large parts from mergeDirectory 
   std::string name("mergeObjs");
   if( ! objTarget ) {
      std::cerr << name << ": empty target object pointer" << std::endl;
      return -1;
   }
   if( ! obj ) {
      std::cerr << name << ": empty object pointer" << std::endl;
      return -1;
   }

   if( debugLevel >= VERBOSE )
     std::cout << name << ": obj->GetName() = " << obj->GetName() << ", mergeType = " << mergeType << ", class = " << obj->IsA()->GetName() << std::endl;

   TH1 *h=0, *nextH=0;
   TH2 *h2=0, *nextH2=0;
   TGraph *g=0; 
   TTree *t=0;

//    h = dynamic_cast<TH1*>( objTarget );
//    g = dynamic_cast<TGraph*>( objTarget );
//    t = dynamic_cast<TTree*>( objTarget );
   if((h = dynamic_cast<TH1*>( objTarget ))  ) {  // we are dealing with histograms
     /*     if ( debugLevel >= DEBUG )
	    std::cout << " --> " << name << " is a histogram */
     nextH = dynamic_cast<TH1*>( obj );
     if (!nextH) {
       std::cerr << objTarget->GetName() << " is a TH1, but " << obj->GetName() << " is not: skip merging" << std::endl;
       return -1;
     }
     if( mergeType == "effAsPerCent" ) {
       if((h2 = dynamic_cast<TH2*>( objTarget )) && (nextH2 = dynamic_cast<TH2*>( nextH ))){
	 merge_effAsPerCent( *h2, *nextH2 );
       }else{
	 merge_effAsPerCentAlt( *h, *nextH );
       }
     }else if( mergeType == "perBinEffPerCent" ) {
       merge_perBinEffPerCent( *h, *nextH );
     }else if( mergeType == "weightedAverage" ) {
       merge_weightedAverage( *h, *nextH );
     }else if( mergeType == "weightedAverage2D" ) {
       //if( h2 && nextH2 ) merge_weightedAverage( *h2, *nextH2 );
       merge_weightedAverage( *h, *nextH );
     }else if( mergeType == "weightedEff" ) {
       merge_weightedEff( *h, *nextH );
     }else if( mergeType == "mergeRebinned" ) {
       merge_Rebinned( *h, *nextH );
     }else if( mergeType == "eventSample" ) {
       if((h2 = dynamic_cast<TH2*>( objTarget ))&& (nextH2 = dynamic_cast<TH2*>( nextH ))){
	 merge_eventSample( *h2, *nextH2 );
       }
     }else if( mergeType == "mergeRMS" ) {
       merge_RMS( *h, *nextH );
     }else if( mergeType == "RMSpercentDeviation" ) {
       merge_RMSpercentDeviation( *h, *nextH );
     }else if( mergeType == "lowerLB" ) {
       merge_lowerLB( *h, *nextH );
     }else if( mergeType == "merge" ) {
       TList tl; tl.Add(nextH); h->Merge(&tl);
     } else {
       if (!h->Add( nextH )) {
	 std::cerr << "Histogram " << h->GetName() << " should NOT be using Add: needs to specify a merge method (e.g. merge) in its metadata";
       }
     }
   }else if( (g = dynamic_cast<TGraph*>( objTarget )) ) {  // TGraphs
     if( mergeType != "<default>" ) {
       std::cerr << name << ": TGraph " << obj->GetName() << " request mergeType = " << mergeType
		 << " but only default merging implemented for TGraphs\n";              
     }
     TGraph *nextG = dynamic_cast<TGraph*>( obj );
     TList listG;
     listG.Add( nextG );
     g->Merge( &listG );
     listG.Clear();
   }else if ((t = dynamic_cast<TTree*>( objTarget ))) { // TTrees
     if ( debugLevel >= VERBOSE) {
       std::cout << "Merging Tree " << obj->GetName() << std::endl;
     }
     if( mergeType != "<default>" ) {
       std::cerr << name << ": TTree " << obj->GetName() << " request mergeType = " << mergeType
		 << " but only default merging implemented for TTrees\n";            
     }
     TTree *nextT = dynamic_cast<TTree*>( obj );
     TList listT;
     listT.Add( nextT );
     t->Merge( &listT );
     listT.Clear();
   } else {
     std::cerr << name << ": object is not a histogram or graph, merging not implemented" << std::endl;
   }
   return 0;
}

int MonitoringFile::mergeLB_recursiveDirCopy(TDirectory *dir_top_out, TDirectory *dir_out, TDirectory *cwd, 
                                             std::vector<std::string>& v_dirsSub, debugLevel_t& debugLevel) {
   // store in v_dirsSub the paths (relative to dir_top_out) of all subdirectories of cwd
   // copy the subfolder structure from cwd to dir_out 
   std::string name("mergeLB_recursiveDirCopy");
   if( ! cwd ) {
      std::cerr << name << ": empty directory pointer cwd" << std::endl;
      return -1;
   }
   if( ! dir_top_out ) {
      std::cerr << name << ": empty directory pointer dir_top_out" << std::endl;
      return -1;
   }
   if( ! dir_out ) {
      std::cerr << name << ": empty directory pointer dir_out" << std::endl;
      return -1;
   }

   TIter next(cwd->GetListOfKeys());
   TKey *key; 
   while( ( key=(TKey*) next() ) ) {
      std::string keyClassName(key->GetClassName());
      if( (keyClassName.size() > 9) && (keyClassName.substr(0,10) == "TDirectory") ) {
         TDirectory *dir = (TDirectory*) cwd->Get(key->GetName());
         if( !dir ) {
            std::cerr << name << ": could not retrieve directory " << key->GetName() << 
               " from " << cwd->GetPath() << std::endl;
            continue;
         }
         dir_out->cd();
         std::string p_top(dir_top_out->GetPath());
         std::string p(gDirectory->GetPath());
         if( p_top.size() < p.size() ) {
            p = p.substr(p_top.size()+1, p.size()-p_top.size()-1); // strip off the top directory part
            p = p + "/";
         }
         else p = "";

         // is this directory already in the list of subdirectories?
         std::vector<std::string>::iterator it;
         for( it = v_dirsSub.begin(); it != v_dirsSub.end(); it++ ) {
            if( *it == (p + dir->GetName()) ) break;
         }
         if( it == v_dirsSub.end() ) {
            // no
            v_dirsSub.push_back(p + dir->GetName());
         }

         TKey *test_key = dir_out->FindKey(dir->GetName());
         TDirectory *dir_out_new(0);
         if( !test_key ) {   
            if( debugLevel >= DEBUG ) {
               std::cout << name << ": creating subdirectory " << dir->GetName();
               if( p.size() != 0 ) std::cout << " in " << p << std::endl;
               else std::cout << std::endl;
            }
            dir_out_new = (TDirectory*) gDirectory->mkdir(dir->GetName());
            if( ! dir_out_new ) {
               std::cerr << name << ": could not create directory " << dir->GetName() 
                  << " in " << gDirectory->GetPath() << std::endl;
            }
         }
         else {
            if( debugLevel >= DEBUG ) 
               std::cout << name << ": " << dir_out->GetPath() << "/" << dir->GetName() << " exists already" << std::endl;

            dir_out_new = (TDirectory*) dir_out->Get(test_key->GetName()); 
            if( ! dir_out_new ) {
               std::cerr << name << ": could not retrieve directory " << test_key->GetName() 
                  << " from " << dir_out->GetPath() << std::endl;
            }
         }
         // go one level deeper in the directory tree
         if( dir_out_new )
            mergeLB_recursiveDirCopy(dir_top_out, dir_out_new, dir, v_dirsSub, debugLevel);

      }
   }
   return 0;
}

int MonitoringFile::mergeLB_createListOfHistos(TDirectory *dir_top, TDirectory *cwd, std::vector<std::string>& v_histos, debugLevel_t& debugLevel) {
   // get paths relative to dir_top of all histograms and graphs in cwd
   std::string name("mergeLB_createListOfHistos");
   if( ! cwd ) {
      std::cerr << name << ": empty directory pointer" << std::endl;
      return -1;
   }

   TIter next(cwd->GetListOfKeys());
   TKey *key; 
   while( ( key=(TKey*) next() ) ) {
      std::string keyClassName(key->GetClassName());
      if( ( (keyClassName.size() > 2) && ( (keyClassName.substr(0,3) == "TH1") || (keyClassName.substr(0,3) == "TH2")  ) ) ||
          ( (keyClassName.size() > 7) && ( (keyClassName.substr(0,8) == "TProfile") ) ) || 
          ( (keyClassName.size() > 5) && ( (keyClassName.substr(0,6) == "TGraph") ) ) ) {
         if( debugLevel >= VERBOSE )
            std::cout << name << ": found object: " << key->GetName();  

         // add to v_histos if not already in it
         std::string p_top(dir_top->GetPath());
         std::string p(cwd->GetPath());
         if( p_top.size() < p.size() ) {
            p = p.substr(p_top.size()+1, p.size()-p_top.size()-1); // strip off the top directory part
            p = p + "/";
         }
         else p = "";

         std::vector<std::string>::iterator it;
         for( it = v_histos.begin(); it != v_histos.end(); it++ ) {
            if( *it == (p + key->GetName()) ) break;
         }

         if( it == v_histos.end() ) {
            std::string objName(p + key->GetName());
            v_histos.push_back(objName);
            if( debugLevel >= VERBOSE )
               std::cout << ", added to list: " << p + key->GetName() << std::endl;
         }
         else {
            if( debugLevel >= VERBOSE )
               std::cout << ", already in list" << std::endl; 
         }
      }
   }
   return 0;
}

int MonitoringFile::mergeLB_processLBinterval(std::vector<TDirectory*>& v_dirsStat, TDirectory *dir_run, debugLevel_t& debugLevel) {
   // create a directory for merged histograms in dir_run
   // loop over v_dirsStat directories (=different LB intervals) 
   // and merge all histograms and graphs (preserving the subdirectory structure)
   std::string name("mergeLB_processLBinterval");

   if( v_dirsStat.size() == 0 ) return 0;

   if( ! dir_run ) {
      std::cerr << name << ": empty pointer dir_run" << std::endl;
      return -1;
   }

   if( ! v_dirsStat[0] ) {
      std::cerr << name << ": empty first directory pointer" << std::endl;
      return -1;
   }

/*
   // create top-level merged directory
   std::string label((v_dirsStat[0])->GetName());
   label = label.substr(0,3);
   std::string dirMergedName("merged_" + label + "Stat");
   TDirectory *dir_merged = dir_run->mkdir(dirMergedName.c_str());
   if( !dir_merged ) {
      std::cerr << name << ": could not create directory " << dirMergedName << std::endl;
      return -1;
   }
   if( debugLevel >= DEBUG )
      std::cout << name << ": created new directory " << dir_merged->GetPath() << std::endl;
*/
   TDirectory *dir_merged = dir_run;

   // Histograms can be booked on demand in AthenaMonitoring.
   // So a histogram might be present in lowStat_LB21-30 but not
   // in lowStat_LB1-10. Also entire directories might be missing
   // in certain LB intervals.
   // Hence, to cope with on-demand-booking have to loop over all
   // LB intervals to identify all histograms.

   // To limit memory usage, instead of holding all merged histograms in
   // memory, build a list of histogram names (including full path because 
   // can have same name in different subfolders, like Jets/Topo/JetP and
   // Jets/Tower/JetP). 

   // Build this list only per subfolder. Then loop over list, merge the
   // histograms, write them to ROOT file, and delete them from memory.

   // instead of a list, create a map which also holds the merging type
   // (whether simply TH1F::Add() or more complicated function) from metadata.

   std::vector<TDirectory*>::const_iterator i;
   // loop over LB intervals:
   //    - get list of subdirectories
   //    - and create subdirectory structure in merged directory
   std::vector<std::string> v_dirsSub;
   for( i = v_dirsStat.begin(); i != v_dirsStat.end(); i++ ) {
      TDirectory *dirStat = *i;
      if( debugLevel >= DEBUG )
         if( dirStat ) 
            std::cout << name << ": getting input from directory " << dirStat->GetPath() << std::endl;
      mergeLB_recursiveDirCopy(dir_merged, dir_merged, dirStat, v_dirsSub, debugLevel); 
      // v_dirsSub are paths relative to dir_merged
   }

   // loop over subdirectories:
   if( v_dirsSub.size() == 0 ) {
      if( debugLevel >= DEBUG )
         std::cout << name << ": no subdirectories found" << std::endl;
      return 0;
   }
   //   - create list of histograms 
   //        - loop over all LB intervals because of on-demand-booking
   //   - loop over histogram list
   //        - loop over LB intervals
   //            - merge histograms
   //        - write merged histogram to file and delete it from memory
   std::vector<std::string>::const_iterator it_string;
   for( it_string = v_dirsSub.begin(); it_string != v_dirsSub.end(); it_string++ ) {
      std::string dirFullName = *it_string;
      std::vector<std::string> v_histos;
      if( debugLevel >= DEBUG )
         std::cout << name << ": processing " << dirFullName << std::endl;
      for( i = v_dirsStat.begin(); i != v_dirsStat.end(); i++ ) {
         TDirectory *dirStat = *i;
         TDirectory *dir = (TDirectory*) dirStat->Get(dirFullName.c_str());
         if( !dir ) {
            std::cerr << name << ": could not retrieve directory " << dirFullName << 
               " from " << dirStat->GetPath() << std::endl;
            continue;
         }
         if( debugLevel >= DEBUG )
            std::cout << name << ": processing LB folder " << dir->GetPath() << std::endl;
         mergeLB_createListOfHistos(dirStat, dir, v_histos, debugLevel); 
      }

      if( v_histos.size() == 0 ) {
         if( debugLevel >= DEBUG )
            std::cout << name << ": no objects found" << std::endl;
          continue;
      }

      TDirectory *dir_out = (TDirectory*) dir_merged->Get(dirFullName.c_str()); 
      if( !dir_out ) {
         std::cerr << name << ": could not retrieve directory " << dirFullName << 
            " from " << dir_merged->GetPath() << std::endl;
         continue;
      }

      // create new metadata tree for current merged directory if not already extant
      typedef std::map< std::string, MetaData >  MetaDataMap_t;
      TTree* mdTree = dynamic_cast<TTree*>(dir_out->Get("metadata"));
      if (!mdTree) {
	mdTree = new TTree( "metadata", "Monitoring Metadata" );
      }
      mdTree->SetDirectory(0);
      OutputMetadata outputmd( mdTree );
 
      // get metadata from one of the input directories
      // this is for caching, could also read the metadata directly from tree in the
      // current directory when accessing the histogram
      MetaDataMap_t mdMap;
      TDirectory *dir_in = (TDirectory*) v_dirsStat[0]->Get(dirFullName.c_str()); 
      if( !dir_in ) {
         std::cerr << name << ": could not retrieve directory " << dirFullName << 
            " from " << v_dirsStat[0]->GetPath() << std::endl;
         continue;
      }
      fillMetaDataMap( mdMap, dir_in);

      // histogram loop
      std::vector<std::string>::const_iterator it_string2;
      for( it_string2 = v_histos.begin(); it_string2 != v_histos.end(); it_string2++ ) {
         std::string histFullName = *it_string2;  // something like Jets/Tower/JetP
 
         TObject *objMerged(0);
         std::string mergeType("<default>");
          // LB interval loop
	 bool key_checked = false;
         for( i = v_dirsStat.begin(); i != v_dirsStat.end(); i++ ) {
            // retrieve histogram for current LB interval
            TDirectory *dir_current = (*i);
	    std::auto_ptr<TObject> objThis((TObject*) dir_current->Get(histFullName.c_str()));

            if( ! objThis.get() ) { // histogram does not exist in this LB interval
               continue;
            }

	   // test if histogram exists already in dir_out
	   if (! key_checked) {
	     TKey *test_key = dir_out->FindKey(objThis->GetName());
	     if( test_key ) {
	       if( debugLevel >= DEBUG )
		 std::cout << name << ": " << dir_out->GetPath() << "/" << objThis->GetName()
			   << " exists already, not written" << std::endl;
	       break;
	     }
	     key_checked = true;
	   }

            if( ! objMerged ) {
              // clone the current histogram
               objMerged = objThis->Clone();
               // retrieve metadata
               MetaDataMap_t::iterator mdi = mdMap.find(objThis->GetName());
               const MetaData *md(0);                  
               if( mdi != mdMap.end() ) {
                  md = &(mdi->second);
               }
               else {
                  // it could be that we had just been unlucky (on-demand-booking)
                  // and this histogram was not available in v_dirsStat[0]
                  // (or the directory for which we created the map last here).
                  // remake metadatamap
                  mdMap.clear();
                  fillMetaDataMap( mdMap, dir_current );
                  mdi = mdMap.find(objThis->GetName());
                  if( mdi != mdMap.end() ) {
                     md = &(mdi->second);
                  }
               }

               if( ! md ) {
                  std::cerr << name << ": no metadata for " << histFullName << std::endl;
                  std::cerr << name << ": => using default merging" << std::endl;
               }
               else {
                  outputmd.fill( md->name, md->interval, md->chain, md->merge );
                  mergeType = md->merge;
               }
            }
            else {
               // objMerged already exists, merge with objThis
	      mergeObjs(objMerged, objThis.get(), mergeType, debugLevel);
            }
	    //delete objThis;
         }

         dir_out->cd();
         // write histogram to file and delete it from memory
         if( objMerged ) {
	   objMerged->Write();
	   if( debugLevel >= DEBUG )
	     std::cout << name << ": wrote " << dir_out->GetPath() << "/" << objMerged->GetName() << std::endl;
	   delete objMerged;
         }
      }
      // write metadata tree
      dir_out->cd();
      mdTree->SetDirectory(dir_out);
      mdTree->Write(0, kOverwrite);
      delete mdTree;

   } 

   return 0;
}

void MonitoringFile::buildLBToIntervalMap(std::vector<TDirectory*>& v_dirLBs, std::vector<TDirectory*>& v_dirsInterval, map_dir_vdir& mapping, debugLevel_t& debugLevel) {
  std::vector<std::string> v_splits;
  typedef std::vector<std::pair<TDirectory*, std::pair<int, int> > > range_t;
  range_t v_ranges;
  for (std::vector<TDirectory*>::const_iterator dirit = v_dirsInterval.begin();
       dirit != v_dirsInterval.end();
       ++dirit) {
    std::string dirname((*dirit)->GetName());
    std::string corename(dirname.substr(10, std::string::npos));
    boost::split(v_splits, corename, boost::algorithm::is_any_of(std::string("-")));
    if (v_splits.size() != 2) {
      std::cerr << "Unable to properly parse " << (*dirit)->GetName() << std::endl;
      continue;
    }
    if( debugLevel >= DEBUG ) std::cout << "Found " << dirname << " " 
					<< v_splits[0] << " " 
					<< v_splits[1] << std::endl;
    try {
      v_ranges.push_back(std::make_pair(*dirit, std::make_pair(boost::lexical_cast<int>(v_splits[0]), boost::lexical_cast<int>(v_splits[1]))));
    } catch (boost::bad_lexical_cast e) {
      std::cerr << "Unable to cast to integers: " << v_splits[0] << " " 
		<< v_splits[1] << std::endl;
    }
  }
  for (std::vector<TDirectory*>::const_iterator dirit = v_dirLBs.begin();
       dirit != v_dirLBs.end();
       ++dirit) {
    std::string dirname((*dirit)->GetName());
    int lbnum = boost::lexical_cast<int>(dirname.substr(3, std::string::npos));
    for (range_t::const_iterator rangeit = v_ranges.begin(); 
	 rangeit != v_ranges.end(); ++rangeit) {
      if ((*rangeit).second.first <= lbnum && 
	  lbnum <= (*rangeit).second.second) {
	map_dir_vdir::iterator mapit = mapping.find((*rangeit).first);
	(*mapit).second.push_back(*dirit);
      }
    }
  }
}

int MonitoringFile::mergeLB_processLB(std::vector<TDirectory*>& v_dirLBs, std::vector<TDirectory*>& v_dirsInterval, debugLevel_t& debugLevel) {
  std::map<TDirectory*,std::vector<TDirectory*> > mapping;
  for (std::vector<TDirectory*>::const_iterator dirit = v_dirsInterval.begin();
       dirit != v_dirsInterval.end(); ++dirit) {
    mapping[*dirit] = std::vector<TDirectory*>();
  }
  buildLBToIntervalMap(v_dirLBs, v_dirsInterval, mapping, debugLevel);

  for (map_dir_vdir::iterator mapit = mapping.begin();
       mapit != mapping.end(); ++mapit) {
    mergeLB_processLBinterval((*mapit).second, (*mapit).first, debugLevel);
  }

  return 0;
}

int MonitoringFile::mergeLB_processRun(TDirectory *dir_run, debugLevel_t& debugLevel) {
   // get lists of "lowStat_", "medStat_", "higStat_" subdirectories in dir_run
   // call mergeLB_processLBinterval for each of these lists
   std::string name("mergeLB_processRun");
   if( ! dir_run ) {
      std::cerr << name << ": empty pointer to run directory" << std::endl;
      return -1;
   }

   if( debugLevel >= DEBUG )
      std::cout << name << ": processing dir " << dir_run->GetName() << std::endl;
   
   std::vector<TDirectory*> v_dirsLowStat, v_dirsMedStat, v_dirsHigStat,
     v_dirsLB;

   TIter next(dir_run->GetListOfKeys());
   TKey *key; 
   while( ( key=(TKey*) next() ) ) {
      std::string keyClassName(key->GetClassName());
      if( (keyClassName.size() > 9) && (keyClassName.substr(0,10) == "TDirectory") ) {
         TDirectory *dir = (TDirectory*) dir_run->Get(key->GetName());
         if( !dir ) {
            std::cerr << name << ": could not retrieve " << key->GetName() << " from " << dir_run->GetPath() << std::endl;
            continue;
         }
         std::string dirName(dir->GetName()); 
	 if ( dirName.substr(0,3) == "lb_" ) {
	   v_dirsLB.push_back(dir);
	 } else if( dirName.size() > 7 )  {  
            if( dirName.substr(0,8) == "lowStat_" ) {
               v_dirsLowStat.push_back(dir);
            }
            else if( dirName.substr(0,8) == "medStat_" ) {
               v_dirsMedStat.push_back(dir);
            }
            else if( dirName.substr(0,8) == "higStat_" ) {
               v_dirsHigStat.push_back(dir);
            }
         }
      }
   }

   if( debugLevel >= DEBUG ) std::cout << "\n" << name << ": processing LB directories" << std::endl;
   if( v_dirsLB.size() > 0 ) mergeLB_processLB(v_dirsLB, v_dirsLowStat, debugLevel);
   
   if( debugLevel >= DEBUG ) std::cout << "\n" << name << ": processing lowStat directories" << std::endl;
   if( v_dirsLowStat.size() > 0 ) mergeLB_processLBinterval(v_dirsLowStat, dir_run, debugLevel);

   if( debugLevel >= DEBUG ) std::cout << "\n" << name << ": processing medStat directories" << std::endl;
   if( v_dirsMedStat.size() > 0 ) mergeLB_processLBinterval(v_dirsMedStat, dir_run, debugLevel);

   if( debugLevel >= DEBUG ) std::cout << "\n" << name << ": processing higStat directories" << std::endl;
   if( v_dirsHigStat.size() > 0 ) mergeLB_processLBinterval(v_dirsHigStat, dir_run, debugLevel);

   return 0;
}

int MonitoringFile::mergeLBintervals(std::string inFilename, std::string inDebugLevel) {
   // open merged file, call mergeLB_processRun for each "run_" directory
   // in file, close file
   std::string name("mergeLBintervals");

   debugLevel_t debugLevel = none;
   if(inDebugLevel.empty()){
     debugLevel=m_debugLevel>(int)VERBOSE?VERBOSE:(debugLevel_t)m_debugLevel;
   }else{
     if( inDebugLevel == "DEBUG" ) debugLevel = DEBUG;
     else if( inDebugLevel == "VERBOSE" ) debugLevel = VERBOSE;
   }
   
   //TFile &f = *(TFile::Open(inFilename.c_str(), "UPDATE")); // does a copy and leaks memory
   TFile *f = (TFile::Open(inFilename.c_str(), "UPDATE",inFilename.c_str(),m_fileCompressionLevel));

   TIter next(f->GetListOfKeys());
   TKey *key; 
   while( ( key=(TKey*) next() ) ) {
      std::string keyClassName(key->GetClassName());
      if( (keyClassName.size() > 9) && (keyClassName.substr(0,10) == "TDirectory") ) {
         TDirectory *dir = (TDirectory*) f->Get(key->GetName());
         if( !dir ) {
            std::cerr << name << ": could not retrieve " << key->GetName() 
            << " from top level directory of " << inFilename << std::endl;
            continue;
         }
         std::string dirName(dir->GetName()); 
         if( (dirName.size() > 3) && (dirName.substr(0,4) == "run_") ) {
            if( debugLevel == DEBUG )
               std::cout << name << ": found run dir " << dirName << std::endl;
            // process this run
            mergeLB_processRun(dir, debugLevel);
         }
      }
   }

   f->Close();
   return 0;
}

bool
MonitoringFile::
CheckHistogram(TFile* f,const char* HistoName)
{
  std::auto_ptr<TObject> obj(f->Get(HistoName));
  if (! obj.get()) {
    //std::cerr<<"No such histogram \""<< HistoName << "\"\n";
    return false;
  }
  else return true;
}

int MonitoringFile::getDebugLevel(){return m_debugLevel;}
void MonitoringFile::setDebugLevel(int level){m_debugLevel=level;}
int MonitoringFile::m_fileCompressionLevel=1;
int MonitoringFile::m_debugLevel=0;

std::string MonitoringFile::getPath(TDirectory *dir){
  
  std::string path = dir->GetPath();
  if ( path.find(':') != std::string::npos )
    path = path.substr(path.rfind(':')+1);    
  
  return path;
}

} // namespace dqutils

