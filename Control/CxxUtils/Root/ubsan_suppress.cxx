/**
 * @file CxxUtils/Root/ubsan_suppress.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2017
 * @brief Helper for suppressing ubsan warnings.
 */


#include "CxxUtils/ubsan_suppress.h"
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>


namespace CxxUtils {


class RedirStderr
{
public:
  RedirStderr();
  ~RedirStderr();
  RedirStderr (const RedirStderr&) = delete;
  RedirStderr& operator= (const RedirStderr&) = delete;

private:
  int m_nullfd;
  int m_stderr;
};


// Redirect stderr to /dev/null, remembering the original FD.
RedirStderr::RedirStderr()
{
  m_nullfd = open ("/dev/null", O_WRONLY);
  m_stderr = dup (2);
  if (m_nullfd >= 0) {
    dup2 (m_nullfd, 2);
  }
}


// Restore stderr to its original FD.
RedirStderr::~RedirStderr()
{
  dup2 (m_stderr, 2);
  close (m_nullfd);
  close (m_stderr);
}


/**
 * @brief Helper for suppressing ubsan warnings.
 * @param func Function to call (may be a lambda).
 *
 * If ubsan is running, temporarily redirect stderr to /dev/null.
 * Then call @c func.
 *
 * For example, we sometimes get a bogus ubsan warning from
 * cling initialization.  This can be suppressed by adding something like:
 *
 *@code
 *  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
 @endcode
 */
void ubsan_suppress (void (*func)())
{
  if (dlsym (RTLD_DEFAULT, "__ubsan_handle_add_overflow") != NULL)
  {
    RedirStderr redir;
    func();
  }
  else {
    func();
  }
}


} // namespace CxxUtils
