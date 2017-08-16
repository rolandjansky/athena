// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_MSGSTREAMEMBER_H
#define ATHENAKERNEL_MSGSTREAMEMBER_H 1
/** @file MsgStreamMember.h
 *  @brief MsgStreamMember is designed to be a data member of a Gaudi component
 *         that sets up and give access safely and efficiently to a 
 *         MsgStream object
 *
 *  $Id: MsgStreamMember.h,v 1.1 2008-07-14 22:10:14 calaf Exp $
 *  @author Paolo Calafiura - Atlas collaboration
   */
#include <string>

//strictly speaking this could be in the cxx file, by putting it here we allow
//clients to use MsgStreamMember as a self-contained replacement for MsgStream
#include "GaudiKernel/MsgStream.h"

#include "AthenaKernel/getMessageSvc.h" /* IMessageSvcHolder */
class MsgStream;
namespace Athena {
  

  /** @class MsgStreamMember
   *  @brief designed to be a data member of a Gaudi component
   *         that sets up and give access safely and efficiently to a 
   *         MsgStream object. The main advantage over a plain MsgStream
   *         is that MsgStreamMember can delay the instantiation of the
   *         MsgStream (and of IMessageSvc) till the last possible moment
   *         and will manage IMessageSvc ref count for you.
   *
   */
  class MsgStreamMember {
  public:
    /// standard constructor (looks like MsgStream's)
    /// In 
    /// We take ownership (addRef) ims here, and release it in the destructor
    /// This makes it work with e.g. Algorithm::msgSvc which is propertly
    /// released in ~Algorithm. Notice that m_stream is set lazily in get()
    /// @param label: the label to be printed by the stream (usually name())
    MsgStreamMember(IMessageSvc *ims, const std::string& label) :
      m_ims(ims), m_label(label), m_stream(0) {}

    /// this constructor will take IMessageSvc* from getMessageSvc()
    /// slow, meant to be used from a class without access to IMessageSvc*
    /// m_ims and m_stream set lazily in get()
    /// @param label: the label to be printed by the stream (usually name())
    MsgStreamMember(const std::string& label) :
      m_ims(), m_label(label), m_stream(0) {}

    /// allows to create the MsgStream immediately
    /// @param o: if o is @c Athena::Options::Eager it will create a MsgStream
    /// @param label: the label to be printed by the stream (usually name())
    /// @c MsgStream instance there and then.
    MsgStreamMember(const Options::CreateOptions o, const std::string& label);

    MsgStreamMember() : m_ims(), m_label(""), m_stream(0) {}
    MsgStreamMember(const MsgStreamMember& rhs) :
      m_ims(rhs.m_ims), m_label(rhs.m_label), m_stream(0) {}

    MsgStreamMember& operator= (const MsgStreamMember& rhs);

    /// we own the stream
    ~MsgStreamMember();

    /// upon first access sets m_ims as needed
    MsgStream& get();

  private:
    IMessageSvcHolder m_ims;
    /// the label to be printed by the stream (usually name())
    std::string m_label;
    MsgStream* m_stream;
  };
}
template <typename T>
MsgStream& operator << (Athena::MsgStreamMember& stream, const T& rhs) {
  return (stream.get() << rhs);
}

#endif
