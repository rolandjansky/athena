/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************************
 * Class  : TLumiBlockRange                                                       *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Max Baak <mbaak@cern.ch> - CERN, Switzerland                              *
 **********************************************************************************/

#ifndef __TLumiBlockRange__
#define __TLumiBlockRange__

#include "TObject.h"
#include <vector>

namespace Root {

   class TLumiBlockRange : public TObject {
      
   public:

      TLumiBlockRange();      
      TLumiBlockRange( const Int_t& start, const Int_t& end = 2147483647 );
      virtual ~TLumiBlockRange();

      TLumiBlockRange(const Root::TLumiBlockRange& other) ;
      TLumiBlockRange& operator=(const TLumiBlockRange& other) ;

      const Root::TLumiBlockRange GetOverlapWith(const TLumiBlockRange& other) const ; 
      const std::vector<Root::TLumiBlockRange> GetPartOnlyIn(const TLumiBlockRange& other) const ; 
      const std::vector<Root::TLumiBlockRange> GetPartNotIn(const TLumiBlockRange& other) const ;

      Bool_t Contains( const Int_t& number ) const;

      inline Int_t Begin() const { return m_begin; }
      inline Int_t End() const { return m_end; }
      inline Bool_t IsEmpty() const { return (Begin()>End()); }

      inline void SetBegin(const Int_t& begin) { m_begin=begin; }
      inline void SetEnd(const Int_t& end) { m_end=end; }

      void Summary() const ;
      
   private:

      Int_t m_begin;
      Int_t m_end;

      ClassDef(TLumiBlockRange,1)
   };
}

#endif

