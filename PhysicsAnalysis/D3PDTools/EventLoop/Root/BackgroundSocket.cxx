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


//
// includes
//

#include <EventLoop/BackgroundSocket.h>

#include <EventLoop/BackgroundJob.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <TFile.h>
#include <errno.h>
#include <fcntl.h>
#include <sstream>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

//
// method implementations
//

namespace EL
{
  void BackgroundSocket :: 
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
  }



  BackgroundSocket :: 
  BackgroundSocket (int val_socket)
    : m_socket (val_socket)
  {
    int saved_flags = fcntl (val_socket, F_GETFL);
    if (fcntl (val_socket, F_SETFL, saved_flags | O_NONBLOCK) == -1)
      RCU_THROW_MSG ("failed to set socket to non-blocking");

    RCU_NEW_INVARIANT (this);
  }



  BackgroundSocket ::
  ~BackgroundSocket ()
  {
    RCU_DESTROY_INVARIANT (this);

    close (m_socket);
  }



  void BackgroundSocket ::
  write (Type type, const std::string& payload)
  {
    RCU_CHANGE_INVARIANT (this);

    std::string message;
    message += std::string (reinterpret_cast<const char*>(&type), sizeof (Type));
    std::size_t payloadSize = payload.size();
    message += std::string (reinterpret_cast<const char*>(&payloadSize), sizeof (payloadSize));
    message += payload;
    while (!message.empty())
    {
      fd_set write;
      FD_ZERO (&write);
      FD_SET (m_socket, &write);
      if (select (m_socket + 1, 0, &write, 0, 0) == -1 &&
	  errno != EINTR)
      {
	int myerrno = errno;
	RCU_THROW_MSG ("failed to call select: " + std::string (sys_errlist[myerrno]));
      }

      int bytesWritten = ::write (m_socket, message.data(), message.size());
      if (bytesWritten == -1)
      {
	if (errno != EAGAIN &&
	    errno != EWOULDBLOCK &&
	    errno != EINTR)
	{
	  int myerrno = errno;
	  RCU_THROW_MSG ("failed to write to socket: " + std::string (sys_errlist[myerrno]));
	}
      } else
      {
	message = message.substr (bytesWritten);
      }
    }
  }



  void BackgroundSocket ::
  read (Type& type, std::string& payload)
  {
    RCU_CHANGE_INVARIANT (this);

    const std::size_t headerSize = sizeof(Type) + sizeof(std::size_t);
    while (true)
    {
      if (m_inputBuffer.size() >= headerSize)
      {
	const std::size_t payloadSize =
	  *reinterpret_cast<const std::size_t*>(m_inputBuffer.data() + sizeof(Type));
	if (m_inputBuffer.size() >= headerSize + payloadSize)
	{
	  type = *reinterpret_cast<const Type*>(m_inputBuffer.data());
	  payload = m_inputBuffer.substr (headerSize, payloadSize);
	  m_inputBuffer = m_inputBuffer.substr (headerSize + payloadSize);
	  if (type >= T_INVALID)
	  {
	    type = T_INVALID;
	    RCU_THROW_MSG ("read invalid message type");
	  }
	  return;
	}
      }

      fd_set read;
      FD_ZERO (&read);
      FD_SET (m_socket, &read);
      if (select (m_socket + 1, &read, 0, 0, 0) == -1 &&
	  errno != EINTR)
      {
	int myerrno = errno;
	RCU_THROW_MSG ("failed to call select: " + std::string (sys_errlist[myerrno]));
      }

      char buffer [1024];
      int bytesRead = ::read (m_socket, buffer, sizeof (buffer));
      if (bytesRead == -1)
      {
	if (errno != EAGAIN &&
	    errno != EWOULDBLOCK &&
	    errno != EINTR)
	{
	  int myerrno = errno;
	  RCU_THROW_MSG ("failed to read from socket: " + std::string (sys_errlist[myerrno]));
	}
      } else
      {
	m_inputBuffer += std::string (buffer, bytesRead);
      }

      if (bytesRead == 0 && fcntl(m_socket, F_GETFD) == 0)
	RCU_THROW_MSG ("socket closed");
    }
  }



  void BackgroundSocket ::
  makeSockets (int sockets [2])
  {
    if (socketpair (PF_UNIX, SOCK_STREAM, 0, sockets) < 0)
      RCU_THROW_MSG ("failed to open socket pair");
  }



  std::auto_ptr<BackgroundSocket> BackgroundSocket ::
  startWorker (const BackgroundJob& job)
  {
    int sockets [2] = {0, 0};
    BackgroundSocket::makeSockets (sockets);
    int cpid = 0;

    if ((cpid = fork()) < 0)
      RCU_THROW_MSG ("failed to fork");
    if (cpid == 0)
    {
      close (sockets[1]);
      std::ostringstream str;
      str << "root -l -b -q $ROOTCOREDIR/scripts/load_packages.C $ROOTCOREBIN/user_scripts/EventLoop/background_worker.C'(" << sockets[0] << ")'";
      int flags = fcntl (sockets[0], F_GETFD);
      if (fcntl (sockets[0], F_SETFD, flags & ~FD_CLOEXEC) < 0)
	RCU_THROW_MSG ("failed to set close-on-exec flag");
      if (execlp ("/bin/sh", "/bin/sh", "-c", str.str().c_str(), (char*) 0) < 0)
	RCU_THROW_MSG ("failed to call exec");
    }
    close (sockets[0]);
    std::auto_ptr<BackgroundSocket> result (new BackgroundSocket (sockets[1]));

    char tmpdir [30] = "/tmp/el_bg_job_XXXXXX";
    if (!mkdtemp (tmpdir))
      RCU_THROW_MSG ("failed to create temporary directory");
    {
      std::auto_ptr<TFile> file (TFile::Open ((std::string (tmpdir) + "/job.root").c_str(), "RECREATE"));
      file->WriteObject (&job, "job");
    }
    result->write (BackgroundSocket::T_START_JOB, tmpdir);
    return result;
  }
}
