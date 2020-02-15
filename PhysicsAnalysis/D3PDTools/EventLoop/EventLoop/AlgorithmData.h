/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__ALGORITHM_DATA_H
#define EVENT_LOOP__ALGORITHM_DATA_H

#include <EventLoop/Global.h>

#include <cstdint>
#include <memory>

namespace EL
{
  namespace Detail
  {
    /// \brief all the data a worker tracks for an individual algorithm
    ///
    /// This is a simple `struct` without accessors, as it is meant to
    /// be strictly internal to the worker and module implementation.

    struct AlgorithmData final
    {
      /// \brief the algorithm we use
      std::unique_ptr<Algorithm> m_algorithm;

      /// \brief the number of times this algorithm has been called
      uint64_t m_executeCount {0};

      /// \brief the number of times this algorithm has asked to skip
      /// this event
      uint64_t m_skipCount {0};


      AlgorithmData () = default;

      AlgorithmData (std::unique_ptr<Algorithm> val_algorithm);

      AlgorithmData (AlgorithmData&& that) = default;

      ~AlgorithmData () noexcept;

      inline Algorithm *operator -> () {return m_algorithm.get();};

      inline const Algorithm *operator -> () const {return m_algorithm.get();};
    };
  }
}

#endif
