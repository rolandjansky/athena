/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IOHelperFcns.cxx 693573 2015-09-07 19:15:49Z wsfreund $
#include "RingerSelectorTools/tools/IOHelperFcns.icc"

// STL includes:
#include <stdexcept>
#include <string>
#include <sstream>
#include <algorithm>
#include <functional>
#include <locale>
#include <memory>

// ROOT includes:
#include <TClass.h>
#include <TSystem.h>

namespace Ringer { namespace IOHelperFcns {

namespace {

// =============================================================================
/**
 * @brief Fctor for checking if string is digit or comma
 **/
//struct is_digit_or_comma {
//  bool operator ()(int c){
//    if ( std::isdigit(c) || c != ',' ){
//      return true;
//    } else {
//      return false;
//    }
//  }
//};

// =============================================================================
/**
 * @brief Filter index string from
 **/
std::string& filterIdxStr(std::string &str)
{
  std::string oldStr = str;
  // Check if string is in correct format:
  bool isCorrectFormat = true;
  std::stringstream ss(str);
  // We want to discard everything until @
  std::getline ( ss, str, '@' );
  std::getline ( ss, str, '@' );
  if ( str.empty() || str.front() != '(' || str.back() != ')'){
    isCorrectFormat = false;
  }
  // Strip first and last charater
  str = str.substr(1, str.size()-2 );
  if ( str.find_first_not_of( "0123456789," ) != std::string::npos ){
    isCorrectFormat = false;
  }
  if ( !isCorrectFormat ){
    throw std::runtime_error(std::string("Could not extract index string "
          "from ") + oldStr );
  }
  // Ok, it is on correct format:
  return str;
}
} //private namespace

// =============================================================================
std::string makeIdxStr(unsigned int idxV)
{
  std::stringstream ss;
  ss << "@(" << idxV << ")";
  return ss.str();
}

// =============================================================================
std::string makeIdxStr(std::vector<unsigned int> &idxV)
{
  std::stringstream ss;
  ss << "@(";
  for ( size_t count = 0; count < idxV.size(); ++count ){
    ss << idxV[count];
    if ( count + 1 < idxV.size() ) {
      ss << ",";
    }
  }
  ss << ")";
  return ss.str();
}

// =============================================================================
void getIdxVecFromStr(const char* cstr, std::vector<unsigned int> &vec)
{
  // Clear vector so that we can fill it with idx array
  vec.clear();
  //  Retrieve idxStr from cstr and prepare to loop over indexes
  std::string str(cstr);
  std::stringstream ss(filterIdxStr(str));
  //  Fill vector
  while ( ss.good() ) {
    std::getline(ss,str,',');
    vec.push_back(std::stoul(str));
  }
}

// =============================================================================
unsigned int getIdxFromStr(const char* cstr, unsigned int n)
{
  std::string str(cstr);
  std::stringstream ss(filterIdxStr(str));
  // Ignore any number until we get into our requested index:
  for ( size_t counter = 0; counter < n ; ++counter ){
    std::getline(ss,str,',');
  }
  return std::stoul(str);
}

// =============================================================================
bool startsWith(const char *cStrStart, const char *cStr)
{
  std::string strStart(cStrStart);
  std::string str(cStr);
  if(str.substr(0, strStart.size()) == strStart) {
    return true;
  }
  return false;
}

// =============================================================================
unsigned version()
{

#ifndef RINGER_SELECTOR_PACKAGE_VERSION
# error "RINGER_SELECTOR_PACKAGE_VERSION macro is undefined."
#endif

  std::string versionStr = RINGER_SELECTOR_PACKAGE_VERSION;

  versionStr.erase(std::remove_if( versionStr.begin(),
      versionStr.end(),
      [] (int c) { return !std::isdigit(c); }),
      versionStr.end());

  return std::stoul(versionStr);
}

// =============================================================================
void checkFile(const TFile& file){
  if (file.IsZombie()){
    throw std::runtime_error(std::string("TFile \"") +
        file.GetName() + "\"is not in good status");
  }
}

// =============================================================================
bool fileExist(const char* fileName) {
  Int_t prevMessageLevel = gErrorIgnoreLevel;
  gErrorIgnoreLevel = kBreak;
  TFile file(fileName);
  gErrorIgnoreLevel = prevMessageLevel;
  try {
    file.IsZombie();
    return true;
  } catch (const std::runtime_error &e){
    return false;
  }
}

// =============================================================================
int deleteFile(const char* fileName)
{
  if (fileExist(fileName)){
    return gSystem->Exec((std::string("rm ") + fileName).c_str() );
  } else {
    return -1;
  }
}

// =============================================================================
void checkDir(TDirectory *dir)
{
  if (dir == nullptr)
  {
    throw std::runtime_error(std::string("TDirectory points to void."));
  }
}

// =============================================================================
void writeVersion(TDirectory* dir)
{
  // Add package version in which we have written this configurable
  unsigned v = version();
  IOHelperFcns::writeVar(dir, "writtenOnPkgVersion", v );
}

// =============================================================================
TDirectory *makeDir(TDirectory* baseDir, const char *name)
{
  checkDir(baseDir);
  TDirectory* newDir = baseDir->mkdir(name);
  try {
    checkDir(newDir);
  } catch ( std::runtime_error & ) {
    throw std::runtime_error(std::string("Could not create a directory "
          "on directory named ") + baseDir->GetName() );
  }
  writeVersion(newDir);
  return newDir;
}

// =============================================================================
std::shared_ptr<THashList> getDirList(TDirectory* dir)
{
  checkDir(dir);
  THashList* list = static_cast<THashList*>(dir->GetList());
  if ( list->IsEmpty() ) {
    return nullptr;
  }
  // Create new list
  std::shared_ptr<THashList> dirList(
    new THashList( dir->GetListOfKeys()->GetSize() ) );
  // Push only directories to list:
  TIter iter(list);
  while ( TDirectory *dir = static_cast<TDirectory*>(iter()) ){
    //if ( dir->IsA() == TDirectoryFile::Class() ) {}
    if ( dir->InheritsFrom(TDirectory::Class()) ) {
      dirList->Add(dir);
    }
  }
  return dirList;
}

// =============================================================================
unsigned getWrittenVersion(TDirectory *configDir)
{
  unsigned writtenVersion(0);
  readVar(configDir, "writtenOnPkgVersion", writtenVersion);
  return writtenVersion;
}

} // namespace IOHelperFcns

} // namespace Ringer


