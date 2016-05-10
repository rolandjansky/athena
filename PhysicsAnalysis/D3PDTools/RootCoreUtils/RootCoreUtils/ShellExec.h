#ifndef ROOT_CORE_UTILS__SHELL_EXEC_H
#define ROOT_CORE_UTILS__SHELL_EXEC_H

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <RootCoreUtils/Global.h>

#include <string>

namespace RCU
{
  namespace Shell
  {
    /// effects: execute the given command
    /// guarantee: strong
    /// failures: out of memory II
    /// failures: system failure
    /// failures: command failure
    void exec (const std::string& cmd);


    /// effects: execute the given command and return the output
    /// returns: the output of the command
    /// guarantee: strong
    /// failures: out of memory III
    /// failures: system failure
    /// failures: command failure
    std::string exec_read (const std::string& cmd);


    /// effects: execute the given command and return the output
    /// returns: the output of the command
    /// guarantee: strong
    /// failures: out of memory III
    /// failures: system failure
    /// failures: command failure
    std::string exec_read (const std::string& cmd, int& rc);


    /// effects: quote the given name to protect it from the shell
    /// returns: the quoted name
    /// guarantee: strong
    /// failures: out of memory II
    std::string quote (const std::string& name);
  }
}

#endif
