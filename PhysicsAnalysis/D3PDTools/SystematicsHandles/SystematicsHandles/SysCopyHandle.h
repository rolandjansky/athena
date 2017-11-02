#ifndef SYSTEMATICS_HANDLES__SYS_COPY_HANDLE_H
#define SYSTEMATICS_HANDLES__SYS_COPY_HANDLE_H

//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <PATInterfaces/SystematicSet.h>
#include <string>
#include <tuple>
#include <unordered_map>

#ifdef ROOTCORE
#include <AsgTools/SgTEvent.h>
#else
#include <StoreGate/StoreGateSvc.h>
#endif

class StatusCode;

namespace CP
{
  class SystematicSet;
}

namespace EL
{
  /// \brief a data handle for copying systematics varied input data

  template<typename T> class SysCopyHandle final
  {
    //
    // public interface
    //

    /// \brief standard constructor
  public:
    template<typename T2>
    SysCopyHandle (T2 *owner, const std::string& propertyName,
                   const std::string& propertyValue,
                   const std::string& propertyDescription);


    /// \brief retrieve the object for the given name
  public:
    ::StatusCode getCopy (T*& object,
                          const CP::SystematicSet& sys) const;



    //
    // private interface
    //

    /// \brief the input name we use
  private:
    std::string m_inputName;

    /// \brief the output name we use
  private:
    std::string m_outputName;

    /// \brief the cache of names we use
  private:
    mutable std::unordered_map<CP::SystematicSet,std::tuple<std::string,std::string,std::string> > m_nameCache;

#ifdef ROOTCORE
    /// \brief the event store we use
  private:
    mutable asg::SgTEvent *m_evtStore = nullptr;

    /// \brief the function to retrieve the event store
  private:
    std::function<asg::SgTEvent*()> m_evtStoreGetter;
#else
    /// \brief the event store we use
  private:
    mutable StoreGateSvc *m_evtStore = nullptr;

    /// \brief the function to retrieve the event store
  private:
    std::function<StoreGateSvc*()> m_evtStoreGetter;
#endif
  };
}

#include "SysCopyHandle.icc"

#endif
