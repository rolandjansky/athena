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

#include <EventLoop/Global.h>

#include <EventLoop/BackgroundSocket.h>
#include <RootCoreUtils/Assert.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace EL;

//
// unit test
//

struct Message
{
  BackgroundSocket::Type type;

  std::string payload;
};

const Message messages [] =
{
  {BackgroundSocket::T_END_JOB, ""},
  {BackgroundSocket::T_END_JOB, "asdfasdf"},
  {BackgroundSocket::T_INVALID, ""}
};

void check_write (BackgroundSocket& socket)
{
  for (const Message *message = messages;
       message->type != BackgroundSocket::T_INVALID;
       ++ message)
    socket.write (message->type, message->payload);
}

void check_read (BackgroundSocket& socket)
{
  for (const Message *message = messages;
       message->type != BackgroundSocket::T_INVALID;
       ++ message)
  {
    BackgroundSocket::Type type;
    std::string payload;
    socket.read (type, payload);
    RCU_ASSERT (type == message->type);
    RCU_ASSERT (payload == message->payload);
  }
}

int main ()
{
  int sock [2] = {0, 0}, cpid = 0;
  BackgroundSocket::makeSockets (sock);

  switch ((cpid = fork()))
  {
  case -1:
    RCU_ASSERT0 ("failed to fork");
    break; //compiler dummy
  case 0:
    {
      close (sock[1]);
      BackgroundSocket socket (sock[0]);
      check_write (socket);
      check_read (socket);
      close (sock[0]);
    }
    break;
  default:
    {
      close (sock[0]);
      BackgroundSocket socket (sock[1]);
      check_read (socket);
      check_write (socket);
      close (sock[1]);

      int status = 0;
      if (wait (&status) == -1)
	RCU_ASSERT0 ("process wait failed");
      if (WIFSIGNALED (status) || WEXITSTATUS (status) != 0)
	RCU_ASSERT0 ("subprocess failed");
    }
    break;
  }

  return 0;
}
