/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/AlgorithmData.h>

#include <AnaAlgorithm/IAlgorithmWrapper.h>

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    AlgorithmData ::
    AlgorithmData (std::unique_ptr<IAlgorithmWrapper> val_algorithm)
      : m_algorithm (std::move (val_algorithm))
    {}

    AlgorithmData ::
    ~AlgorithmData () noexcept
    {
      // made explicit to reduce include dependencies
    }
  }
}
