/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_MEMORYRESCUESVC_H
#define ATHENASERVICES_MEMORYRESCUESVC_H
/** @file MemoryRescueSvc.h
    @brief Svc that shuts Athena down when high-watermark memory usage reached.

*/

#include <new>

#ifndef ATHENABASECOMPS_ATHSERVICE_H
# include "AthenaBaseComps/AthService.h"
#endif

#ifndef KERNEL_STATUSCODES_H
# include "GaudiKernel/StatusCode.h"
#endif

#ifndef GAUDIKERNEL_PROPERTY_H
# include "Gaudi/Property.h"
#endif


/** @class MemoryRescueSvc
    @brief Svc that shuts Athena down when high-watermark memory usage reached.

    @details Service that holds on to a block of memory (of size PAGE_SIZE
    times m_pages, or property 'NumberOfPages'), until the job runs out of
    memory. It then releases the block and exits Athena as cleanly as possible
    by sending std::bad_alloc to the python job.
*/
class MemoryRescueSvc: public AthService {

public:

  /// Allocate the rescue block on Service::initialize
  virtual StatusCode initialize();

  /// Re-allocate the rescue block on Service::reinitialize
  virtual StatusCode reinitialize();

  /// Release the rescue block on Service::finalize
  virtual StatusCode finalize();

  /// Standard Constructor
  MemoryRescueSvc( const std::string& name, ISvcLocator* svc );

  /// Standard Destructor
  virtual ~MemoryRescueSvc();

private:
  /// Block-releasing implementation of the std::new_handler
  static void newHandler();

  /// Holder for the old std::new_handler that is replaced
  std::new_handler m_new;

  /// Pointer to rescue block of memory
  static char* s_block;

private:
  /// @property Size of rescue block of memory, in pages
  IntegerProperty m_pages;
};

#endif // !ATHENASERVICES_MEMORYRESCUESVC_H
