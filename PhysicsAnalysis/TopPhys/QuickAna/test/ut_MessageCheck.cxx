/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.

#undef NDEBUG

//
// includes
//

#include <QuickAna/MessageCheck.h>
#include <QuickAna/Global.h>

#include <QuickAna/SelectionChoice.h>
#include <QuickAna/SelectionCut.h>
#include <QuickAna/SelectionData.h>
#include <RootCoreUtils/Assert.h>
#include <PATInterfaces/CorrectionCode.h>
#include <PATInterfaces/SystematicCode.h>
#include <EventLoop/StatusCode.h>

//
// unit test
//

template<typename T,typename T1,typename T2>
void checkTypeWeightSingle (const T& scSuccess, const CP::CorrectionCode& scTest,
			    bool expectedSuccess, bool expectedFinish,
			    bool expectedSet,
			    std::function<bool(T)> successTest)
{
  using namespace ana::msgUserCode;

  bool finish = false;
  T1 weightVar = -7;
  std::function<CP::CorrectionCode (T2& weight)> func
    ([&] (T2& weight) -> CP::CorrectionCode {
      weight = 42; return scTest;});
  T mySC = [&] () -> T {
    ANA_CHECK_SET_TYPE (T);
    QA_CHECK_WEIGHT (T2, weightVar, func (weightVar));
    finish = true;
    return scSuccess;} ();
  RCU_ASSERT (successTest (mySC) == expectedSuccess);
  RCU_ASSERT (finish == expectedFinish);
  if (expectedSet)
    RCU_ASSERT (weightVar == 42);
  else
    RCU_ASSERT (weightVar == -7);
}

template<typename T,typename T1,typename T2>
void checkTypeWeight (const T& scSuccess, std::function<bool(T)> successTest)
{
  checkTypeWeightSingle<T,T1,T2> (scSuccess, CP::CorrectionCode::Ok,
				  true, true, true, successTest);
  checkTypeWeightSingle<T,T1,T2> (scSuccess, CP::CorrectionCode::OutOfValidityRange,
				  true, true, false, successTest);
  checkTypeWeightSingle<T,T1,T2> (scSuccess, CP::CorrectionCode::Error,
				  false, false, false, successTest);
}

template<typename T>
void checkTypeCutSingle (const T& scSuccess, const CP::CorrectionCode& scTest,
			 bool expectedSuccess, bool expectedFinish,
			 ana::SelectionChoice expectedGet,
			 std::function<bool(T)> successTest)
{
  using namespace ana::msgUserCode;

  // manually made cut
  ana::SelectionData selData;
  ana::SelectionCut selCut;
  selData.registerCut (ana::SelectionStep::MANUAL, "test", selCut);

  bool finish = false;
  T mySC = [&] () -> T {
    ANA_CHECK_SET_TYPE (T);
    QA_CHECK_CUT (selCut, scTest);
    finish = true;
    return scSuccess;} ();
  RCU_ASSERT (successTest (mySC) == expectedSuccess);
  RCU_ASSERT (finish == expectedFinish);
  RCU_ASSERT (expectedGet == selCut.get());
}

template<typename T>
void checkTypeCut (const T& scSuccess, std::function<bool(T)> successTest)
{
  checkTypeCutSingle<T> (scSuccess, CP::CorrectionCode::Ok,
			 true, true, ana::SelectionChoice::PASS, successTest);
  checkTypeCutSingle<T> (scSuccess, CP::CorrectionCode::OutOfValidityRange,
			 true, false, ana::SelectionChoice::FAIL, successTest);
  checkTypeCutSingle<T> (scSuccess, CP::CorrectionCode::Error,
			 false, false, ana::SelectionChoice::UNASSIGNED, successTest);
}

template<typename T>
void checkTypeSingle (const T& scSuccess, const T& scTest, bool expectedSuccess,
		      std::function<bool(T)> successTest)
{
  using namespace ana::msgUserCode;

  bool success = false;
  T mySC = [&] () -> T {
    ANA_CHECK_SET_TYPE (T);
    ANA_CHECK (scTest);
    success = true;
    return scSuccess;
  } ();
  RCU_ASSERT (successTest (mySC) == expectedSuccess);
  RCU_ASSERT (success == expectedSuccess);
}

template<typename T>
void checkType (const T& scSuccess, const T& scFailure1, const T& scFailure2,
		std::function<bool(T)> successTest)
{
  checkTypeSingle<T> (scSuccess, scSuccess, true, successTest);
  checkTypeSingle<T> (scSuccess, scFailure1, false, successTest);
  checkTypeSingle<T> (scSuccess, scFailure2, false, successTest);

  checkTypeWeight<T,float,float> (scSuccess, successTest);
  checkTypeWeight<T,float,double> (scSuccess, successTest);
  checkTypeWeight<T,double,float> (scSuccess, successTest);
  checkTypeWeight<T,double,double> (scSuccess, successTest);
}

int main ()
{
  ana::enableStatusCodeFailure ();

  checkType<StatusCode> (StatusCode::SUCCESS, StatusCode::FAILURE, StatusCode::FAILURE, [] (const StatusCode& sc) -> bool {return sc.isSuccess();});
  checkType<xAOD::TReturnCode> (xAOD::TReturnCode::kSuccess, xAOD::TReturnCode::kFailure, xAOD::TReturnCode::kRecoverable, [] (const xAOD::TReturnCode& sc) -> bool {return sc.isSuccess();});
  checkType<EL::StatusCode> (EL::StatusCode::SUCCESS, EL::StatusCode::FAILURE, EL::StatusCode::FAILURE, [] (const EL::StatusCode& sc) -> bool {return sc == EL::StatusCode::SUCCESS;});
  checkType<CP::CorrectionCode> (CP::CorrectionCode::Ok, CP::CorrectionCode::Error, CP::CorrectionCode::OutOfValidityRange, [] (const CP::CorrectionCode& sc) -> bool {return sc == CP::CorrectionCode::Ok;});
  checkType<CP::SystematicCode> (CP::SystematicCode::Ok, CP::SystematicCode::Unsupported, CP::SystematicCode::Unsupported, [] (const CP::SystematicCode& sc) -> bool {return sc == CP::SystematicCode::Ok;});
  checkType<bool> (true, false, false, [] (const bool& sc) -> bool {return sc;});

  return 0;
}
