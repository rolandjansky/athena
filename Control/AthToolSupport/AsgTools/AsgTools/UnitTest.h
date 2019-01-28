// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//
#ifndef ASG_TOOLS__UNIT_TEST_H
#define ASG_TOOLS__UNIT_TEST_H

//        
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.

#include <AsgTools/MessageCheck.h>
#include <AsgTools/MessagePrinterErrorCollect.h>
#include <AsgTools/MessagePrinterOverlay.h>
#include <string>
#include <exception>
#include <gtest/gtest.h>

namespace asg
{
  namespace detail
  {
    /// \brief return whether str matches regex
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   expression error
    bool matchesRegex (const std::string& regex, const std::string& str);
  }

  template<typename T> struct CheckHelper;
}

#define ASSERT_SUCCESS(x)					\
  ASSERT_EQ (asg::CheckHelper<decltype(x)>::successCode(), x)

#define EXPECT_SUCCESS(x)					\
  EXPECT_EQ (asg::CheckHelper<decltype(x)>::successCode(), x)

#define ASSERT_FAILURE(x)					\
  ASSERT_EQ (asg::CheckHelper<decltype(x)>::failureCode(), x)

#define EXPECT_FAILURE(x)					\
  EXPECT_EQ (asg::CheckHelper<decltype(x)>::failureCode(), x)


#ifndef XAOD_STANDALONE
#define ASSERT_FAILURE_REGEX(x,regex)                           \
  ASSERT_FAILURE(x)
#define EXPECT_FAILURE_REGEX(x,regex)                           \
  EXPECT_FAILURE(x)
#else
#define ASSERT_FAILURE_REGEX(x,regex)                                   \
  { MessagePrinterErrorCollect errorPrinter;                            \
    MessagePrinterOverlay overlayPrinter (&errorPrinter);               \
    auto code = (x);                                                    \
    if (asg::CheckHelper<decltype(x)>::isSuccess (code)) {              \
      FAIL () << "command didn't fail as expected: " << #x;             \
    } else if (errorPrinter.empty()) {                                  \
      FAIL () << "command printed no error message on failure: " << #x; \
    } else if (!errorPrinter.matchesRegex (regex)) {                    \
      FAIL () << "invalid error message for: " << #x             \
              << "\nexpected:\n  " << regex << "\nfound:\n"             \
              << errorPrinter.asString ("  ");                          \
    } }
#define EXPECT_FAILURE_REGEX(x,regex)                                   \
  { MessagePrinterErrorCollect errorPrinter;                            \
    MessagePrinterOverlay overlayPrinter (&errorPrinter);               \
    auto code = (x);                                                    \
    if (asg::CheckHelper<decltype(x)>::isSuccess (code)) {              \
      ADD_FAILURE () << "command didn't fail as expected: " << #x;      \
    } else if (errorPrinter.empty()) {                                  \
      ADD_FAILURE () << "command printed no error message on failure: " << #x; \
    } else if (!errorPrinter.matchesRegex (regex)) {                    \
      ADD_FAILURE () << "invalid error message for: " << #x             \
                     << "\nexpected:\n  " << regex << "\nfound:\n"      \
                     << errorPrinter.asString ("  ");                   \
    } }
#endif

#define ASSERT_THROW_REGEX(x,regex)					\
  { std::string internalTestMessage; try {				\
      x; internalTestMessage = std::string ("expected statement ") + #x " to throw, but it didn't"; \
    } catch (std::exception& e) {					\
      if (!::asg::detail::matchesRegex ((regex), e.what())) {		\
	internalTestMessage = std::string ("expected statement ") + #x " to throw message matching " + (regex) + ", but actual message didn't match: " + e.what(); \
      } } catch (...) {							\
      internalTestMessage = std::string ("statement ") + #x " threw an exception that didn't derive from std::exception"; } \
    if (!internalTestMessage.empty())					\
      FAIL() << internalTestMessage;					\
    else SUCCEED(); }

#define EXPECT_THROW_REGEX(x,regex)					\
  { std::string internalTestMessage; try {				\
      x; internalTestMessage = std::string ("expected statement ") + #x " to throw, but it didn't"; \
    } catch (std::exception& e) {					\
      if (!::asg::detail::matchesRegex ((regex), e.what())) {		\
	internalTestMessage = std::string ("expected statement ") + #x " to throw message matching " + (regex) + ", but actual message didn't match: " + e.what(); \
      } } catch (...) {							\
      internalTestMessage = std::string ("statement ") + #x " threw an exception that didn't derive from std::exception"; } \
    if (!internalTestMessage.empty())					\
      ADD_FAILURE() << internalTestMessage;				\
    else SUCCEED(); }

#define ASSERT_MATCH_REGEX(reg,str)		\
  ASSERT_PRED2 (::asg::detail::matchesRegex, reg, str)

#define EXPECT_MATCH_REGEX(reg,str)		\
  EXPECT_PRED2 (::asg::detail::matchesRegex, reg, str)



#ifndef ASGTOOL_STANDALONE

#define ATLAS_GOOGLE_TEST_MAIN			\
  int main (int argc, char **argv)		\
  {						\
    ::testing::InitGoogleTest (&argc, argv);	\
    return RUN_ALL_TESTS();			\
  }

#else

#include <xAODRootAccess/Init.h>

#define ATLAS_GOOGLE_TEST_MAIN			\
  int main (int argc, char **argv)		\
  {						\
    using namespace asg::msgUserCode;		\
    ANA_CHECK_SET_TYPE (int);			\
    ::StatusCode::enableFailure();		\
    ANA_CHECK (xAOD::Init ());			\
    ::testing::InitGoogleTest (&argc, argv);	\
    return RUN_ALL_TESTS();			\
  }

#endif

#endif
