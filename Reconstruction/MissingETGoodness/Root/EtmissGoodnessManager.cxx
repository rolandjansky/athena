/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/EtmissGoodnessManager.h"
#include "MissingETGoodness/Goodies.h"

ClassImp(MET::EtmissGoodnessManager)


// Constructor
MET::EtmissGoodnessManager::EtmissGoodnessManager() 
 : TObject()
 , m_archive(MET::TSelectionSetArchive::instance())
 , m_version(-1)
 , m_versionLock(kFALSE)
 , m_versionPrint(kTRUE)
 , m_quality(MET::Medium)
 , m_qualityLock(kFALSE)
 , m_qualityPrint(kTRUE)
 , m_logger("EtmissGoodnessManager")
{
}


// Destructor
MET::EtmissGoodnessManager::~EtmissGoodnessManager() 
{
  Reset();
}


MET::EtmissGoodnessManager&
MET::EtmissGoodnessManager::instance() {
  // created on first call, deleted after main()
  static MET::EtmissGoodnessManager instance;
  return instance;
}


void
MET::EtmissGoodnessManager::Reset()
{
  m_version = -1;
  m_versionLock = kFALSE;
  m_versionPrint = kTRUE;
  m_quality = MET::Medium;
  m_qualityLock = kFALSE;
  m_qualityPrint = kTRUE;
}


void
MET::EtmissGoodnessManager::SetAndLockVersion( const Int_t& version )
{
  if (!m_versionLock) { 
    m_version=version;
    m_logger << kDEBUG << "Etmiss Goodness version set to : " << m_version << " (-1 = latest)" << GEndl;
  } else {
    m_logger << kWARNING << "Etmiss Goodness version already locked to : " << m_version << " (-1 = latest)" << GEndl;
  }
  m_versionLock = kTRUE;
}


void
MET::EtmissGoodnessManager::SetAndLockQuality( const MET::Quality& quality )
{
  if (!m_qualityLock) {
    m_quality=quality;
    m_logger << kDEBUG << "Etmiss Goodness quality set to : " << m_quality << GEndl;
  } else {
    m_logger << kWARNING << "Etmiss Goodness quality already locked to : " << m_quality << GEndl;
  }
  m_qualityLock = kTRUE;
}


Bool_t
MET::EtmissGoodnessManager::execute()
{
  Bool_t status(kTRUE);

  // update all input values for (sofar) requested selections in the archive.
  m_archive.SetUptodate(kFALSE);
  m_archive.Update();

  return status;
}


const MET::TSelectionSet* 
MET::EtmissGoodnessManager::GetSelectionSet( const MET::Quality& quality, const Int_t& version ) const
{
  // quality and version ignored when already locked
  MET::Quality qual = ( this->IsQualityLocked() ? m_quality : quality );
  Int_t vers = ( this->IsVersionLocked() ? m_version : version ); 
  if (vers<0) vers = m_archive.GetLatestVersion(quality);

  const MET::TSelectionSet* set = m_archive.GetSelectionSet(qual,vers);
  return set;
}


Bool_t 
MET::EtmissGoodnessManager::Evaluate( const Quality& quality, const std::vector<TString>& parList, const Int_t& version ) const
{
  // hack: want this function to be const
  (void) this->GetSelectionSet(quality,version); // this call set current selection set
  return ( m_archive.GetCurrentSelectionSet()!=0 ? m_archive.GetCurrentSelectionSet()->Evaluate(parList) : kFALSE ) ;
}


Long64_t
MET::EtmissGoodnessManager::GetEventsBitMask( const Quality& quality, const Int_t& version ) const
{
  // hack: want this function to be const
  (void) this->GetSelectionSet(quality,version); // this call set current selection set
  return ( m_archive.GetCurrentSelectionSet()!=0 ? m_archive.GetCurrentSelectionSet()->GetEventsBitMask() : 0 );
}


const char* 
MET::EtmissGoodnessManager::GetMETKey( const Quality& quality, const Int_t& version ) const
{
  // hack: want this function to be const
  (void) this->GetSelectionSet(quality,version); // this call set current selection set
  return ( m_archive.GetCurrentSelectionSet()!=0 ? m_archive.GetCurrentSelectionSet()->GetMETKey() : 0 );
}


const char* 
MET::EtmissGoodnessManager::GetJetKey( const Quality& quality, const Int_t& version ) const
{
  // hack: want this function to be const
  (void) this->GetSelectionSet(quality,version); // this call set current selection set
  return ( m_archive.GetCurrentSelectionSet()!=0 ? m_archive.GetCurrentSelectionSet()->GetJetKey() : 0 );
}


Bool_t 
MET::EtmissGoodnessManager::IsVersionLocked() const
{
  if (!m_versionPrint) return m_versionLock;
  else {
    m_logger << kINFO << "Version is locked ? " << m_versionLock << " (only printed once)" << GEndl;
    m_versionPrint = kFALSE; 
  }

  return m_versionLock;
}


Bool_t 
MET::EtmissGoodnessManager::IsQualityLocked() const
{
  if (!m_qualityPrint) return m_qualityLock;
  else {
    m_logger << kINFO << "Quality is locked ? " << m_qualityLock << " (only printed once)" << GEndl;
    m_qualityPrint = kFALSE;
  }

  return m_qualityLock;
}


void
MET::EtmissGoodnessManager::Summary(const Bool_t& verbose) const
{ 
  m_logger << kINFO << "Etmiss Goodness quality = " << m_quality << GEndl;
  m_logger << kINFO << "  Quality locked ? " << m_qualityLock << GEndl;
  m_logger << kINFO << "Etmiss Goodness version = " << m_version << " (-1 = latest)" << GEndl;
  m_logger << kINFO << "  Version locked ? " << m_versionLock << GEndl;

  if (verbose)
    if ( m_archive.GetCurrentSelectionSet()!=0 ) 
      m_archive.GetCurrentSelectionSet()->Summary();
}


void 
MET::EtmissGoodnessManager::ParseBitMask( const Int_t& runnr, const Long64_t& bitmask, const Quality& quality, const Int_t& version ) const 
{
  // hack: want this function to be const
  (void) this->GetSelectionSet(quality,version); // this call set current selection set
  if( m_archive.GetCurrentSelectionSet()!=0 ) 
    m_archive.GetCurrentSelectionSet()->ParseBitMask(runnr,bitmask);
}


void 
MET::EtmissGoodnessManager::ClearEvent()
{
  // reset contents of goodies map
  MET::Goodies::instance().clear();
  // reset 'uptodate' status of all selectionset archive
  this->SetUptodate(kFALSE);
  // reset current set's runnumber
  if ( m_archive.GetCurrentSelectionSet()!=0 ) m_archive.GetCurrentSelectionSet()->SetEventsRunNumber(-1);
}


void
MET::EtmissGoodnessManager::Streamer(TBuffer& /*R__b*/)
{
}

