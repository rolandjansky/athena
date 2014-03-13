/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/TSelectionSetArchive.h"
#include <iostream>
#include "TROOT.h"
#include "Riostream.h"
#include "TClass.h"
#include "TObjArray.h"
#include "TObjString.h"


ClassImp(MET::TSelectionSetArchive)


// Constructor
MET::TSelectionSetArchive::TSelectionSetArchive() 
 : TObject()
 , m_currentset(0)
 , m_logger( "TSelectionSetArchive" )
 , m_uptodate(kFALSE)
{
}


// Destructor
MET::TSelectionSetArchive::~TSelectionSetArchive() 
{
  Reset();
}


MET::TSelectionSetArchive&
MET::TSelectionSetArchive::instance() {
  // created on first call, deleted after main()
  static MET::TSelectionSetArchive _instance;
  return _instance;
}


void
MET::TSelectionSetArchive::Reset()
{
  // delete all requested selection sets
  while ( ! m_selectionSets.empty() ) {
    std::map< const TString, MET::TSelectionSet* >::iterator iter = m_selectionSets.begin();
    MET::TSelectionSet *set = (*iter).second;
    m_selectionSets.erase(iter);
    delete set;
  }

  m_currentset=0;
  _gLoose.clear();
  _gMedium.clear();
  _gTight.clear();
}


Bool_t 
MET::TSelectionSetArchive::Contains(const MET::Quality& quality, const Int_t& version) const
{
  Bool_t found(kFALSE);

  switch (quality)
  {
    case MET::Loose:
      if (_gLoose.find(version)!=_gLoose.end()) { found = kTRUE; }
      break;
    case MET::Medium:
      if (_gMedium.find(version)!=_gMedium.end()) { found = kTRUE; }
      break;
    case MET::Tight:
      if (_gTight.find(version)!=_gTight.end()) { found = kTRUE; }
      break;
    default:
      ;
  }

  return found;
}


TString 
MET::TSelectionSetArchive::GetClassName(const MET::Quality& quality, const Int_t& version) const
{
  TString classname;

  switch (quality)
  {
    case MET::Loose:
      if (_gLoose.find(version)!=_gLoose.end()) { classname = _gLoose.find(version)->second; }
      break;
    case MET::Medium:
      if (_gMedium.find(version)!=_gMedium.end()) { classname = _gMedium.find(version)->second; }
      break;
    case MET::Tight:
      if (_gTight.find(version)!=_gTight.end()) { classname = _gTight.find(version)->second; }
      break;
    default:
      ;
  }

  return classname;
}


MET::TSelectionSet* 
MET::TSelectionSetArchive::GetSelectionSet(const Quality& quality, const Int_t& version)
{
  // first check if corresponding classname exists
  TString className = this->GetClassName( quality,version );
  if (className.IsNull()) {
    m_logger << kWARNING 
             << "GetSelectionSet() : Could not find className for: quality = " 
             << quality << ", version = " << version << ". Return 0." 
             << GEndl;
    m_currentset=0;
    return m_currentset;
  }

  // see if class instance already exists
  TSelectionSet* set = this->FindSelectionSet( className );
  if (set!=0) {
    m_currentset=set;
    return m_currentset;
  }
  
  // need to create new class instance
  // get class from available libraries
  TClass* tmp = gROOT->GetClass( className );
  if (tmp && tmp->InheritsFrom( "MET::TSelectionSet" )) {	
    set = (MET::TSelectionSet*)tmp->New();
    if (!set) m_logger << kFATAL 
                       << "GetSelectionSet() : Selection \"" << className << "\" has been found "
                       << "but could not be instantiated. It might be a virtual class: did you "
                       << "implement all virtual functions defined in the \"TSelectionSet\" base class ?"
                       << GEndl;
  }
  
  if (set==0) 
    m_logger << kWARNING << "Parameter \"" << className << "\" is not associated with a selection set. Return 0." << GEndl;
  else {
    m_logger << kDEBUG << "Parameter \"" << className << "\" is now associated with a selection set." << GEndl;
    m_selectionSets[className] = set;
    // now create the TSelectionSet structure
    set->CreateSelection(); 
    // update the parameter input values before use
    set->Update();
  }

  m_currentset=set;
  return m_currentset;
}


MET::TSelectionSet* 
MET::TSelectionSetArchive::FindSelectionSet( const TString& name ) 
{
  std::map<TString, MET::TSelectionSet*>::iterator itr = m_selectionSets.find(name);
  if (itr==m_selectionSets.end()) return 0;
  else return itr->second;
}


void 
MET::TSelectionSetArchive::RemoveSelectionSet(const Quality& quality, const Int_t& version, const TString& setname)
{
  switch (quality)
  {
    case MET::Loose:
      if (_gLoose.find(version) !=_gLoose.end())  { _gLoose.erase(_gLoose.find(version)); }
      break;
    case MET::Medium:
      if (_gMedium.find(version)!=_gMedium.end()) { _gMedium.erase(_gMedium.find(version)); }
      break;
    case MET::Tight:
      if (_gTight.find(version) !=_gTight.end())  { _gTight.erase(_gTight.find(version)); }
      break;
    default:
      ;
  }

  std::map<TString, MET::TSelectionSet*>::iterator itr = m_selectionSets.find(setname);
  if (itr!=m_selectionSets.end()) {
    MET::TSelectionSet* set = itr->second;
    m_selectionSets.erase(itr);
    delete set;
  }
}


Int_t
MET::TSelectionSetArchive::GetLatestVersion(const Quality& quality) const
{
  Int_t version(-1);

  switch (quality)
  {
    case MET::Loose:
      if (!_gLoose.empty())  { version = _gLoose.rbegin()->first; }
      break;
    case MET::Medium:
      if (!_gMedium.empty()) { version = _gMedium.rbegin()->first; }
      break;
    case MET::Tight:
      if (!_gTight.empty())  { version = _gTight.rbegin()->first; }
      break;
    default:
      ;
  }

  return version;
}


void
MET::TSelectionSetArchive::Summary() const
{ 
  std::map<Int_t,TString>::const_iterator itr;

  if (!_gLoose.empty()) {
    m_logger << kINFO << "Registered Loose selection class(es) : " << GEndl;
    for (itr=_gLoose.begin(); itr!=_gLoose.end(); ++itr)
      m_logger << kINFO << itr->first << " : " << itr->second << GEndl;
  }
  if (!_gMedium.empty()) {
    m_logger << kINFO << "Registered Medium selection class(es) : " << GEndl;
    for (itr=_gMedium.begin(); itr!=_gMedium.end(); ++itr) 
      m_logger << kINFO << itr->first << " : " << itr->second << GEndl;
  }
  if (!_gTight.empty()) {
    m_logger << kINFO << "Registered Tight selection class(es) : " << GEndl;
    for (itr=_gTight.begin(); itr!=_gTight.end(); ++itr) 
      m_logger << kINFO << itr->first << " : " << itr->second << GEndl;
  }
}


Bool_t 
MET::TSelectionSetArchive::RegisterSelectionSet(const Quality& quality, const Int_t& version, const TString& setname, TSelectionSet* set)
{
  Bool_t status(kFALSE);

  if (m_selectionSets.find(setname)!=m_selectionSets.end()) {
    m_logger << kWARNING << "Selection set with name <" << setname << "> already registered. Return false." << GEndl;
    return status;
  }

  if (set->GetQuality()!=quality) {
    m_logger << kWARNING << "Specified quality and quality of selection set not identical. Return false." << GEndl;
    return status;
  }

  if (set->GetVersion()!=version) {
    m_logger << kWARNING << "Specified version and version of selection set not identical. Return false." << GEndl;
    return status;
  }

  switch (quality)
  {
    case MET::Loose:
      if (_gLoose.find(version)==_gLoose.end()) {
        _gLoose[version] = setname;
        status = kTRUE;
      }
      break;
    case MET::Medium:
      if (_gMedium.find(version)==_gMedium.end()) {
        _gMedium[version] = setname;
        status = kTRUE;
      }
      break;
    case MET::Tight:
      if (_gTight.find(version)==_gTight.end()) {
        _gTight[version] = setname;
        status = kTRUE;
      }
      break;
    default:
      ;
  }

  if (!status)
    m_logger << kWARNING << "Selection set with quality <" << quality << "> and version <" << version << "> already registered. Return false." << GEndl;
  else {
    m_logger << kDEBUG << "Selection set with quality <" << quality << "> and version <" << version << "> has now been registered." << GEndl;
    m_selectionSets[setname] = set;
  }

  return status;
}


Bool_t 
MET::TSelectionSetArchive::SetLatestSelectionSet(const Quality& quality, TSelectionSet* set)
{
  Int_t version(2147483647); // largest possible int ;)
  TString setName("MET::SelectionSet_Latest_Manual");

  // first clean up existing latest selection set.
  this->RemoveSelectionSet(quality,version,setName);

  // overwrite quality and version set in selection set
  set->SetQuality(quality);
  set->SetVersion(version);

  return this->RegisterSelectionSet(quality,version,setName,set);
}


Bool_t 
MET::TSelectionSetArchive::RegisterSelectionSetClass(const Quality& quality, const Int_t& version, const TString& classname)
{
  Bool_t status(kFALSE);

  switch (quality) 
  {
    case MET::Loose:
      if (_gLoose.find(version)==_gLoose.end()) {
        _gLoose[version] = classname;
        status = kTRUE;
      } 
      break;
    case MET::Medium:
      if (_gMedium.find(version)==_gMedium.end()) {
        _gMedium[version] = classname;
        status = kTRUE;
      } 
      break;
    case MET::Tight:
      if (_gTight.find(version)==_gTight.end()) {
        _gTight[version] = classname;
        status = kTRUE;
      } 
      break;
    default:
      ;
  }

  if (!status)
    m_logger << kWARNING << "Selection class with quality <" << quality << "> and version <" << version << "> already registered. Return false." << GEndl;

  return status;
}


void
MET::TSelectionSetArchive::Update()
{
  // update all selection sets sofar requested
  if (!m_uptodate) {
    std::map<TString,MET::TSelectionSet*>::iterator itr = m_selectionSets.begin();
    for (; itr!=m_selectionSets.end(); ++itr) itr->second->Update();
    m_uptodate = kTRUE;
  }
}


void 
MET::TSelectionSetArchive::SetUptodate( const Bool_t& uptodate )
{
  m_uptodate = uptodate;
  std::map<TString,MET::TSelectionSet*>::iterator itr = m_selectionSets.begin();
  for (; itr!=m_selectionSets.end(); ++itr) itr->second->SetUptodate(uptodate);
}

