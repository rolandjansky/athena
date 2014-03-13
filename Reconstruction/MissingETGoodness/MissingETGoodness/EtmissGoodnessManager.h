/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MissingETGoodness_EtmissGoodnessManager__
#define __MissingETGoodness_EtmissGoodnessManager__

#include "MissingETGoodness/TSelectionSetArchive.h"
#include "MissingETGoodness/TMsgLogger.h"
#include "TObject.h"

namespace MET
{
  class EtmissGoodnessManager : public TObject {
    
   public:
    
    // destructor
    virtual ~EtmissGoodnessManager();
    // access to the single instance of this class (created statically on first request)
    static EtmissGoodnessManager& instance();

    // needs to be executed for every event
    Bool_t execute();

    void SetAndLockVersion( const Int_t& version = -1 );
    void SetAndLockQuality( const Quality& quality = Medium );
    inline void SetUptodate( Bool_t uptodate=kTRUE ) { m_archive.SetUptodate(uptodate); }
    void ClearEvent();

    const TSelectionSet* GetSelectionSet( const Quality& quality=Medium, const Int_t& version=-1 ) const ;
    Bool_t Evaluate( const Quality& quality=Medium, const std::vector<TString>& parList = std::vector<TString>(), const Int_t& version=-1 ) const ;
    Long64_t GetEventsBitMask( const Quality& quality=Medium, const Int_t& version=-1 ) const ;
    const char* GetMETKey( const Quality& quality=Medium, const Int_t& version=-1 ) const;
    const char* GetJetKey( const Quality& quality=Medium, const Int_t& version=-1 ) const;
    inline Int_t GetVersion() const { return m_version; }
    inline Quality GetQuality() const { return m_quality; }

    Bool_t IsVersionLocked() const; 
    Bool_t IsQualityLocked() const; 

    void ParseBitMask( const Int_t& runnr, const Long64_t& bitmask, const Quality& quality=Medium, const Int_t& version=-1 ) const ;

    void Summary( const Bool_t& verbose=kFALSE ) const;
    void Reset();
    
   private:

    // default constructor 
    EtmissGoodnessManager();

    TSelectionSetArchive& m_archive;

    Int_t   m_version;
    Bool_t  m_versionLock;
    mutable Bool_t  m_versionPrint;
    Quality m_quality;
    Bool_t  m_qualityLock;
    mutable Bool_t  m_qualityPrint;

    mutable TMsgLogger m_logger; 
    
    ClassDef(MET::EtmissGoodnessManager,1)
  };
}

#endif

