/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// AthLegacySequence.h
// Header file for class AthLegacySequence
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHLEGACYEQUENCE_H
#define ATHENABASECOMPS_ATHLEGACYEQUENCE_H 1

#pragma once


// Framework includes
#include "AthenaBaseComps/AthCommonDataStore.h"
#include "AthenaBaseComps/AthCommonMsg.h"
#include "Gaudi/Sequence.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "CxxUtils/checker_macros.h"

namespace Athena {
  namespace details {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
    class AthLegacySequenceAdapter
      : public AthCommonDataStore<AthCommonMsg< Gaudi::Sequence >>
    { 
    public: 
      
      /// Constructor with parameters: 
      AthLegacySequenceAdapter(const std::string& name, 
                               ISvcLocator* pSvcLocator);
      
      virtual StatusCode execute() = 0;
      
      const EventContext& getContext() const { return Gaudi::Hive::currentContext(); }

      virtual const DataObjIDColl& extraOutputDeps() const override;
      
    protected:
      bool isReEntrant() const override final { return false; }
      
    private:
      StatusCode execute( const EventContext& ) const override final
      {
        AthLegacySequenceAdapter* seq ATLAS_THREAD_SAFE =
          const_cast<AthLegacySequenceAdapter*>( this );
        return seq->execute();
      }
      
      DataObjIDColl m_extendedExtraObjects;
      
    }; 
#pragma GCC diagnostic pop
  }
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
/// Alias for backward compatibility
class AthLegacySequence : public Athena::details::AthLegacySequenceAdapter
{
public:
  using Athena::details::AthLegacySequenceAdapter::AthLegacySequenceAdapter;
  // this is to explicitly hide the other signature of `execute`
  StatusCode execute() override = 0;
};
#pragma GCC diagnostic pop


#endif //> !ATHENABASECOMPS_ATHLEGACYSEQUENCE_H
