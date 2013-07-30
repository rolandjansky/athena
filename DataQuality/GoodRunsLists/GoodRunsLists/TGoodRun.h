/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************************
 * Class  : TGoodRun                                                       *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Max Baak <mbaak@cern.ch> - CERN, Switzerland                              *
 **********************************************************************************/

#ifndef __TGoodRun__
#define __TGoodRun__

#include "GoodRunsLists/TLumiBlockRange.h"
#include "TObject.h"
#include <vector>

namespace Root {

   class TGoodRun : public std::vector< TLumiBlockRange >, public TObject {
      
   public:

      TGoodRun() ;
      TGoodRun( const Int_t& runnr );      
      virtual ~TGoodRun();

      TGoodRun(const Root::TGoodRun& other) ;
      TGoodRun& operator=(const TGoodRun& other) ;

      const Root::TGoodRun GetOverlapWith(const TGoodRun& other) const ;
      const Root::TGoodRun GetSumWith(const TGoodRun& other) const ;
      const Root::TGoodRun GetPartOnlyIn(const TGoodRun& other) const ;
      const Root::TGoodRun GetPartNotIn(const TGoodRun& other) const ;

      Bool_t IsEmpty() const;      
      Bool_t HasLB( const Int_t& lumiblocknr )  const;
      std::vector<Root::TLumiBlockRange>::iterator Find( const Int_t& lumiblocknr );
      std::vector< Root::TLumiBlockRange >::const_iterator Find( const Int_t& lumiblocknr ) const;
      inline Int_t GetRunNumber() const { return m_runnr; }
      inline void SetRunNumber( const Int_t& runnr ) { m_runnr=runnr; } 

      void Summary() const ;

      void Sort();
      void Compress();
      void AddLB( const Int_t& lumiblocknr );
      
   private:

      Int_t m_runnr;

      // sorter function for lumiblock ranges
      struct SorterL2H {
        SorterL2H () {}
        bool operator() (const TLumiBlockRange& p1, const TLumiBlockRange& p2) {
          return (p1.Begin()<p2.Begin());
        }
      };

      ClassDef(TGoodRun,1)
   };
}

#endif

