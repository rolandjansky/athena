/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MissingETGoodness_TSelectionSetArchive__
#define __MissingETGoodness_TSelectionSetArchive__

#include "MissingETGoodness/EtmissGoodnessQuality.h"
#include "MissingETGoodness/TMsgLogger.h"
#include "MissingETGoodness/TSelectionSet.h"
#include "TObject.h"
#include "TString.h"
#include <map>

namespace MET
{
  class TSelectionSetArchive : public TObject {
    
  public:
    
    // destructor
    virtual ~TSelectionSetArchive();
    // access to the single instance of this class (created statically on first request)
    static TSelectionSetArchive& instance();

    Bool_t Contains(const Quality& quality, const Int_t& version) const;
    Int_t GetLatestVersion(const Quality& quality) const;
    TString GetClassName(const Quality& quality, const Int_t& version) const;
    inline const TString GetLatestClassName(const Quality& quality) { return this->GetClassName(quality,this->GetLatestVersion(quality)); }
    inline TSelectionSet* GetLatestSelectionSet(const Quality& quality) { return this->GetSelectionSet(quality,this->GetLatestVersion(quality)); }
    inline TSelectionSet* GetCurrentSelectionSet() { return m_currentset; }

    TSelectionSet* GetSelectionSet(const Quality& quality, const Int_t& version);

    Bool_t RegisterSelectionSet(const Quality& quality, const Int_t& version, const TString& setname, TSelectionSet* set);
    Bool_t SetLatestSelectionSet(const Quality& quality, TSelectionSet* set);

    Bool_t RegisterSelectionSetClass(const Quality& quality, const Int_t& version, const TString& classname);
    void SetUptodate(const Bool_t& uptodate=kTRUE);

    void Update();
    void Summary() const;
    void Reset();
    
  private:

    // default constructor 
    TSelectionSetArchive();

    TSelectionSet* FindSelectionSet( const TString & setname ) ;
    void RemoveSelectionSet(const Quality& quality, const Int_t& version, const TString& setname);

    TSelectionSet* m_currentset;
    
    std::map< Int_t, TString > _gLoose; 
    std::map< Int_t, TString > _gMedium; 
    std::map< Int_t, TString > _gTight; 

    std::map< TString, TSelectionSet* > m_selectionSets;

    mutable TMsgLogger m_logger; 

    Bool_t m_uptodate;
    
    ClassDef(MET::TSelectionSetArchive,1)
  };
}

#endif

