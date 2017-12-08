/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <vector>
#include "tbb/parallel_reduce.h"
#include "tbb/blocked_range.h"
#include "TestTools/ParallelCallTest.h"
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>


namespace {


class RedirStderr
{
public:
  RedirStderr();
  ~RedirStderr();
  RedirStderr (const RedirStderr&) = delete;
  RedirStderr& operator= (const RedirStderr&) = delete;

private:
  int m_nullfd;
  int m_stderr;
};


// Redirect stderr to /dev/null, remembering the original FD.
RedirStderr::RedirStderr()
{
  if (dlsym (RTLD_DEFAULT, "__ubsan_handle_add_overflow") != NULL) {
    m_nullfd = open ("/dev/null", O_WRONLY);
    m_stderr = dup (2);
    if (m_nullfd >= 0) {
      dup2 (m_nullfd, 2);
    }
  }
  else {
    m_nullfd = m_stderr = -1;
  }
}


// Restore stderr to its original FD.
RedirStderr::~RedirStderr()
{
  if (m_stderr >= 0) {
    dup2 (m_stderr, 2);
    close (m_nullfd);
    close (m_stderr);
  }
}


} // anonymous namespace


bool ParallelCallTest::run( size_t nrepeats ) {    
  firstCall();
  
  return  tbb::parallel_reduce( tbb::blocked_range<int>( 0, nrepeats, 1 ),
				true, // initial value
				[&]( tbb::blocked_range<int>, bool statusSoFar ) -> bool {
				  return callAndCompare() and statusSoFar;
				},
				[]( bool allCallsStatus, bool thisCallStatus ) -> bool { // result accumulation
				  return allCallsStatus and thisCallStatus; 
				} );
}


bool ParallelCallTest::launchTests( size_t nrepeats, std::initializer_list<ParallelCallTest*> testList ) {
  // Suppress undefined behavior warning resulting from a tbb bug.
  // /cvmfs/atlas-nightlies.cern.ch/repo/sw/master/sw/lcg/releases/LCG_88/tbb/44_20160413/x86_64-slc6-gcc62-dbg/include/tbb/parallel_reduce.h:177:32: runtime error: member call on address 0x2aab14047b40 which does not point to an object of type 'task'
  //0x2aab14047b40: note: object has invalid vptr
  //    00 00 00 00  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  00 00 00 00 00 0
  // cf. https://github.com/RcppCore/RcppParallel/issues/36
  RedirStderr redir;

  std::vector<ParallelCallTest*> tests( testList.begin(), testList.end() );
  return  tbb::parallel_reduce( tbb::blocked_range< std::vector<ParallelCallTest*>::iterator >( tests.begin(), tests.end() ),
				true, // initial value
				[&]( tbb::blocked_range< std::vector<ParallelCallTest*>::iterator > groupOfTests, bool statusSoFar ) -> bool {
				  bool success = true;
				  for ( auto test : groupOfTests ) {
				    success = test->run( nrepeats ) and success;
				  }
				  return statusSoFar and success;
				},
				[]( bool allCallsStatus, bool thisCallStatus ) -> bool { // result accumulation
				  return allCallsStatus and thisCallStatus; 
				} );    
}
