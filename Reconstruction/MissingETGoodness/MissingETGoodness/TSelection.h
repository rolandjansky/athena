/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************************
 * Class  : TSelection                                                       *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Max Baak <mbaak@cern.ch> - CERN, Switzerland                              *
 **********************************************************************************/

#ifndef __MissingETGoodness_TSelection__
#define __MissingETGoodness_TSelection__

#include "MissingETGoodness/TEasyFormula.h"
#include "MissingETGoodness/TRunRange.h"

namespace MET {

  class TSelection : public TRunRange, public std::vector< TEasyFormula > {
     
  public:

    TSelection();
    TSelection( const Int_t& begin, const Int_t& end = 2147483647 );
    virtual ~TSelection();

    TSelection(const TSelection& other) ;
    TSelection& operator=(const TSelection& other) ;

    inline void SetMETKey( const TString& metKey ) { m_metKey = metKey; }
    inline void SetJetKey( const TString& jetKey ) { m_jetKey = jetKey; }
    inline void SetUptodate(const Bool_t& uptodate=kTRUE) { m_uptodate=uptodate; }
    void AddCut( const TString& cut, const TString& cutname="" );

    inline const char* GetMETKey() const { return m_metKey.Data(); }
    inline const char* GetJetKey() const { return m_jetKey.Data(); }
    Long64_t GetEventsBitMask();

    Bool_t Evaluate(const std::vector<TString>& parList = std::vector<TString>() );
    void Update();

    std::vector< MET::TEasyFormula >::iterator Find( const TString& name );
    std::vector< MET::TEasyFormula >::const_iterator Find( const TString& name ) const;

    void Summary() const ;
    void PrintEvent();

    void ParseBitMask(const Long64_t bitmask) const;

  protected:

    Long64_t m_eventsbitmask;
    TString m_metKey;
    TString m_jetKey;

    Bool_t m_uptodate;
    
    ClassDef(TSelection,0)
  };
}

#endif

