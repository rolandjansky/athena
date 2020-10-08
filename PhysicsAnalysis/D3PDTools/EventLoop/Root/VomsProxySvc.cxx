/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoop/VomsProxySvc.h>

#include <AsgMessaging/MessageCheck.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/GridTools.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <fstream>
#include <TSystem.h>

//
// method implementations
//

ClassImp (EL::VomsProxySvc)

namespace EL
{
  const std::string& VomsProxySvc ::
  algServiceName ()
  {
    static std::string result = "VomsProxySvc";
    return result;
  }



  void VomsProxySvc ::
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
  }



  StatusCode VomsProxySvc ::
  setupJob (Job& /*job*/)
  {
    RCU_CHANGE_INVARIANT (this);
    SH::ensureVomsProxy ();

    const char *X509_USER_PROXY = getenv ("X509_USER_PROXY");
    std::string file;
    if (X509_USER_PROXY)
    {
      file = X509_USER_PROXY;
    } else
    {
      std::ostringstream str;
      str << "/tmp/x509up_u" << getuid();
      file = str.str();
    }
    if (file.empty() || gSystem->AccessPathName (file.c_str()) != false)
      RCU_THROW_MSG ("failed to find X509 proxy file: " + file);

    {
      std::ifstream stream (file);
      char ch;
      while (stream.get (ch))
	m_proxyData += ch;
      if (!stream.eof())
	RCU_THROW_MSG ("error reading: " + file);
    }

    return StatusCode::FAILURE;
  }



  const char *VomsProxySvc ::
  GetName () const
  {
    RCU_READ_INVARIANT (this);
    return name().c_str();
  }



  StatusCode VomsProxySvc ::
  histInitialize ()
  {
    RCU_READ_INVARIANT (this);
    if (!SH::checkVomsProxy())
    {
      int fd = open (m_fileName.c_str(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
      if (fd == -1)
      {
	auto myerrno = errno;
	ANA_MSG_ERROR ("failed to create file \"" << m_fileName << "\": " << strerror (myerrno));
	return EL::StatusCode::FAILURE;
      }
      int written = write (fd, &m_proxyData[0], m_proxyData.size());
      if (written == -1)
      {
	auto myerrno = errno;
	ANA_MSG_ERROR ("failed to write to file \"" << m_fileName << "\": " << strerror (myerrno));
	close (fd);
	return EL::StatusCode::FAILURE;
      }
      if (written < int (m_proxyData.size()))
      {
	ANA_MSG_ERROR ("only wrote " << written << " of " << m_proxyData.size() << " bytes to file \"" << m_fileName << "\"");
	close (fd);
	return EL::StatusCode::FAILURE;
      }
      if (close (fd) == -1)
      {
	auto myerrno = errno;
	ANA_MSG_ERROR ("failed to close file \"" << m_fileName << "\": " << strerror (myerrno));
	return EL::StatusCode::FAILURE;
      }
      if (setenv ("X509_USER_PROXY", m_fileName.c_str(), true) == -1)
      {
	auto myerrno = errno;
	ANA_MSG_ERROR ("failed to set X509_USER_PROXY: " << strerror (myerrno));
	return EL::StatusCode::FAILURE;
      }
    }
    return EL::StatusCode::SUCCESS;
  }
}
