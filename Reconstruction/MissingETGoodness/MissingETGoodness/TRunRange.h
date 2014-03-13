/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************************
 * Class  : TRunRange                                                       *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Max Baak <mbaak@cern.ch> - CERN, Switzerland                              *
 **********************************************************************************/

#ifndef __MissingETGoodness_TRunRange__
#define __MissingETGoodness_TRunRange__

#include "TObject.h"
#include <vector>

namespace MET {

   class TRunRange : public TObject {
      
   public:

      TRunRange();      
      TRunRange( const Int_t& begin, const Int_t& end = 2147483647 );
      virtual ~TRunRange();

      TRunRange(const MET::TRunRange& other) ;
      TRunRange& operator=(const TRunRange& other) ;

      const MET::TRunRange GetOverlapWith(const TRunRange& other) const ; 
      const std::vector<MET::TRunRange> GetPartOnlyIn(const TRunRange& other) const ; 
      const std::vector<MET::TRunRange> GetPartNotIn(const TRunRange& other) const ;

      Bool_t ContainsRun( const Int_t& number ) const;
      Bool_t ContainsAnyRun() const;

      inline Int_t Begin() const { return m_begin; }
      inline Int_t End() const { return m_end; }
      inline Bool_t IsEmpty() const { return (Begin()>End()); }

      inline void SetBegin(const Int_t& begin) { m_begin=begin; }
      inline void SetEnd(const Int_t& end) { m_end=end; }

      virtual void Summary() const ;
      
   protected:

      Int_t m_begin;
      Int_t m_end;

      ClassDef(TRunRange,0)
   };
}

#endif

