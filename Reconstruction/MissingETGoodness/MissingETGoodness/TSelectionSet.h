/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************************
 * Class  : TSelectionSet                                                       *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Max Baak <mbaak@cern.ch> - CERN, Switzerland                              *
 **********************************************************************************/

#ifndef __TSelectionSet__
#define __TSelectionSet__

#include "MissingETGoodness/TSelection.h"
#include "MissingETGoodness/EtmissGoodnessQuality.h"
#include "MissingETGoodness/TMsgLogger.h"
#include "TObject.h"
#include <vector>

namespace MET {

  class TSelectionSet : public std::vector< TSelection >, public TObject {
     
  public:

    TSelectionSet( const Quality& quality = Medium, const Int_t& version = -1 ) ;
    virtual ~TSelectionSet();

    TSelectionSet(const MET::TSelectionSet& other) ;
    TSelectionSet& operator=(const TSelectionSet& other) ;

    virtual Bool_t CreateSelection() { return kTRUE; }

    Bool_t ContainsRun( const Int_t& runnr ) const;
    Bool_t ContainsAnyRun() const;
    Bool_t ContainsThisRun();

    std::vector< MET::TSelection >::iterator Find( const Int_t& runnr );
    std::vector< MET::TSelection >::const_iterator Find( const Int_t& runnr ) const;

    void AddSelection ( const TSelection& selection );
    void SetMETKey( const TString& metKey );
    void SetJetKey( const TString& jetKey );
    inline void SetEventsRunNumber( const Int_t& runnr ) { m_eventsrunnr = runnr; }
    inline void SetQuality( const Quality& quality ) { m_quality=quality; }
    inline void SetVersion( const Int_t& version ) { m_version=version; }
    void SetUptodate( const Bool_t& uptodate=kTRUE );

    const char* GetMETKey() const; 
    const char* GetJetKey() const; 
    Long64_t GetEventsBitMask();
    Int_t GetEventsRunNumber();
    inline Quality GetQuality() const { return m_quality; }
    inline Int_t GetVersion() const { return m_version; }

    Bool_t Evaluate(const std::vector<TString>& parList = std::vector<TString>() );
    void Update();
    void UpdateRunNumber();

    void Sort();
    void Summary() const ;
    void PrintEvent();

    void ParseBitMask(const Int_t& runnr, const Long64_t bitmask) const;

  protected:

    std::vector<TRunRange> GetCompressedRunRange() const;

    // sorter function for lumiblock ranges
    struct SorterL2H {
      SorterL2H () {}
      bool operator() (const TSelection& p1, const TSelection& p2) {
        return (p1.Begin()<p2.Begin());
      }
    };

    Quality m_quality;
    Int_t m_version;
    Int_t m_eventsrunnr;

    mutable TMsgLogger m_logger; 

    Bool_t m_uptodate;
    
    ClassDef(TSelectionSet,0)
  };
}

#endif

