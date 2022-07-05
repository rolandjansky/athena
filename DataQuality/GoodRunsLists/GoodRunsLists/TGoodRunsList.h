/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************************
 * Class  : TGoodRunsList                                                       *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Max Baak <mbaak@cern.ch> - CERN, Switzerland                              *
 **********************************************************************************/

#ifndef __TGoodRunsList__
#define __TGoodRunsList__

#include "GoodRunsLists/TGoodRun.h"
#include "CxxUtils/checker_macros.h"
#include "TNamed.h"
#include "TString.h"
#include <map>
#include <vector>
#include <string>

namespace Root {

   class TGoodRunsList : public std::map< Int_t, TGoodRun >, public TNamed {
      
   public:

      TGoodRunsList();
      TGoodRunsList(const char* name);      
      virtual ~TGoodRunsList();

      void AddGRL(const TGoodRunsList& other);
      const Root::TGoodRunsList GetOverlapWith(const TGoodRunsList& other) const ;
      const Root::TGoodRunsList GetSumWith(const TGoodRunsList& other) const ;
      const Root::TGoodRunsList GetPartOnlyIn(const TGoodRunsList& other) const ;
      const Root::TGoodRunsList GetPartNotIn(const TGoodRunsList& other) const ;
      
      Bool_t HasTriggerInfo() const;
      Bool_t HasRun( Int_t runnr )  const;
      Bool_t HasRunLumiBlock( Int_t runnr, Int_t lumiblocknr ) const ;
      Bool_t HasSameGRLInfo( const TGoodRunsList& other ) const;
      Bool_t HasOverlapWith( const TGoodRunsList& other, bool verb=false ) const;

      void AddRunLumiBlock( Int_t runnr, Int_t lumiblocknr );
      inline void SetVersion(const TString& version) { m_version = version; }
      inline void AddMetaData(const TString& key, const TString& value) { m_metadata[key] = value; }
      inline void SetMetaData(const std::map<TString,TString>& metadata) { m_metadata = metadata; }
      inline void SetCheckGRLInfo(Bool_t check=kTRUE) { m_checkGRLInfo=check; }

      inline const Bool_t& GetCheckGRLInfo() const { return m_checkGRLInfo; }
      inline const TString& GetVersion() const { return m_version; }
      inline const std::map<TString,TString>& GetMetaData() const { return m_metadata; }
      inline unsigned int GetMetaDataSize() const { return m_metadata.size(); }

      void Summary(Bool_t verbose = kFALSE) const;
      Bool_t IsEmpty() const;

      std::vector<int> GetRunlist() const;
      std::vector<Root::TGoodRun> GetGoodRuns() const;
      std::vector<std::string> GetTriggerList() const;
      std::vector<std::string> GetStreamList() const;

      TString GetSuggestedName() const;
      void Compress();

   private:

      TString m_version; 
      std::map<TString,TString> m_metadata;
      bool m_checkGRLInfo{false};

      ClassDef(TGoodRunsList,1)
   };
}

#endif

