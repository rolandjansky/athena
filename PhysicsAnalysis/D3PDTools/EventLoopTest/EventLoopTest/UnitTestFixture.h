/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP__UNIT_TEST_FIXTURE_H
#define EVENT_LOOP__UNIT_TEST_FIXTURE_H

//        Copyright Iowa State University 2016.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <EventLoopTest/Global.h>

class TH1;

#include <EventLoop/Job.h>
#include <gtest/gtest.h>
#include <map>
#include <memory>

namespace EL
{
  class UnitTestFixture : public ::testing::TestWithParam<UnitTestConfig>
  {
  public:
    std::string getJob ();

    SH::SamplePtr getSample (const std::string& sameName);

    SH::SampleHandler getSH ();

    unsigned eventCount (const std::string& sampleName);

    TObject *getTObject (const std::string& sampleName,
			 const std::string& objectName,
			 bool isMandatory);

    template<typename T>
    T *getHist (const std::string& sampleName,
		const std::string& objectName,
		bool isMandatory);

    TH1 *getCallbacks (const std::string& sampleName);

    void checkFileExecuted (const std::string& sampleName);

  private:
    static std::map<std::shared_ptr<Driver>,std::string> m_jobs;

  private:
    std::string makeFile (const std::vector<unsigned>& entries);
  };
}

#include "UnitTestFixture.ihh"

#endif
