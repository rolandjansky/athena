/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_VOMS_PROXY_SVC_H
#define EVENT_LOOP_VOMS_PROXY_SVC_H

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


#include <EventLoop/Global.h>

#include <EventLoop/Algorithm.h>

namespace EL
{
  class VomsProxySvc : public Algorithm
  {
    //
    // public interface
    //

    /// description: the name of the service
  public:
    static const std::string& algServiceName ();


    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;

    /// \brief the name of the file to be created
  public:
    std::string m_fileName;



    //
    // interface inherited from Algorithm
    //

  public:
    virtual const char *GetName () const override;

  private:
    virtual StatusCode setupJob (Job& job) override;

  private:
    virtual StatusCode histInitialize () override;



    //
    // private interface
    //

    /// \brief the data to place inside the proxy file
  private:
    std::string m_proxyData;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(VomsProxySvc, 1);
#pragma GCC diagnostic pop
  };
}

#endif
