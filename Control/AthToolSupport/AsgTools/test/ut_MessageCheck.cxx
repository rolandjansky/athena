//        
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <AsgTools/MessageCheck.h>

#include <iostream>
#include <assert.h>
#include <functional>

//
// unit test
//

template<typename T,typename T2>
void checkTypeSingle (const T2& scSuccess, const T2& scTest, bool expectedSuccess,
		      std::function<bool(const T&)> successTest)
{
  using namespace asg::msgUserCode;

  bool success = false;
  T mySC = [&] () -> T {
    ANA_CHECK_SET_TYPE (T);
    ANA_CHECK (T (scTest));
    success = true;
    return scSuccess;
  } ();
  assert (successTest (mySC) == expectedSuccess);
  assert (success == expectedSuccess);
}

template<typename T,typename T2>
void checkType (const T2& scSuccess, const T2& scFailure1, const T2& scFailure2,
		std::function<bool(const T&)> successTest)
{
  checkTypeSingle<T> (scSuccess, scSuccess, true, successTest);
  checkTypeSingle<T> (scSuccess, scFailure1, false, successTest);
  checkTypeSingle<T> (scSuccess, scFailure2, false, successTest);
}

int main ()
{
  StatusCode::enableFailure ();
  xAOD::TReturnCode::enableFailure ();

  // checkType<StatusCode> (StatusCode::SUCCESS, StatusCode::FAILURE, StatusCode::FAILURE, [] (const StatusCode& sc) -> bool {return sc.isSuccess();});
  // checkType<xAOD::TReturnCode> (xAOD::TReturnCode::kSuccess, xAOD::TReturnCode::kFailure, xAOD::TReturnCode::kRecoverable, [] (const xAOD::TReturnCode& sc) -> bool {return sc.isSuccess();});
  checkType<bool> (true, false, false, [] (const bool& sc) -> bool {return sc;});

  return 0;
}
