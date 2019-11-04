/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/SubmitDirManager.h>

#include <AsgMessaging/StatusCode.h>
#include <EventLoop/Job.h>
#include <EventLoop/ManagerData.h>
#include <EventLoop/ManagerOrder.h>
#include <EventLoop/MessageCheck.h>
#include <RootCoreUtils/Assert.h>
#include <TSystem.h>
#include <boost/format.hpp>
#include <boost/functional/hash.hpp>
#include <fcntl.h>
#include <regex>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


using namespace EL::msgEventLoop;

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    namespace
    {
      void reportErrno (int myerrno)
      {
        char error [160];
        strerror_r (myerrno, error, sizeof (error));
        ANA_MSG_ERROR ("encountered system error: " << error);
      }

      void reportErrno ()
      {
        reportErrno (errno);
      }
    }


 
    std::pair<Detail::ManagerOrder,std::string> SubmitDirManager ::
    getManagerOrder () const noexcept
    {
      return std::make_pair (ManagerOrder::SUBMIT_DIR, "");
    }



    ::StatusCode SubmitDirManager ::
    doManagerStep (Detail::ManagerData& data) const
    {
      switch (data.step)
      {
      case ManagerStep::updateSubmitDir:
        {
          std::smatch match;

          // make sure directory is absolute
          if (data.submitDir[0] != '/')
            data.submitDir = gSystem->WorkingDirectory () + ("/" + data.submitDir);

          // make sure we don't end in "/", "/." or "/.."
          const std::regex identityEndExpr {"(/$)|(/\\.$)"};
          while (std::regex_search (data.submitDir, match, identityEndExpr))
            data.submitDir.replace (match.position(0), match.length(0), "");
          const std::regex relativeEndExpr {"/\\.\\.$"};
          if (std::regex_search (data.submitDir, match, relativeEndExpr))
          {
            ANA_MSG_ERROR ("submit directory can not end in \"..\": " + data.submitDir);
            return ::StatusCode::FAILURE;
          }

          // make sure we don't include any '..' in our path.  this is
          // mainly relevant for Ganga Tasks which may crash
          // otherwise.  this specifically doesn't use any of the
          // "normal" regularization functions because those resolve
          // symlinks as well, and depending on the driver we may not
          // want that to happen.
          const std::regex identityExpr {"(/\\./)|(//)|(^/\\.\\./)"};
          while (std::regex_search (data.submitDir, match, identityExpr))
            data.submitDir.replace (match.position(0), match.length(0), "/");
          const std::regex relativeExpr {"/[^/]+/\\.\\./"};
          while (std::regex_search (data.submitDir, match, relativeExpr))
            data.submitDir.replace (match.position(0), match.length(0), "/");

          if (data.submitDir.find ("/pnfs/") == 0)
          {
            ANA_MSG_ERROR ("can not place submit directory on pnfs: " + data.submitDir);
            return ::StatusCode::FAILURE;
          }

          ANA_MSG_DEBUG ("changed submit-dir to " << data.submitDir);
        }
        break;

      case ManagerStep::extractOptions:
        {
          std::string mode = data.options.castString (Job::optSubmitDirMode);
          if (!mode.empty())
          {
            if (mode == "no-clobber")
              data.submitDirMode = SubmitDirMode::NO_CLOBBER;
            else if (mode == "overwrite")
              data.submitDirMode = SubmitDirMode::OVERWRITE;
            else if (mode == "unique")
              data.submitDirMode = SubmitDirMode::UNIQUE;
            else if (mode == "unique-link")
              data.submitDirMode = SubmitDirMode::UNIQUE_LINK;
            else
            {
              ANA_MSG_ERROR ("unknown submit-dir mode: " << mode);
              ANA_MSG_ERROR ("known modes: no-clobber, overwrite, unique, unique-link");
              return ::StatusCode::FAILURE;
            }
          }

          if (data.options.castBool (Job::optRemoveSubmitDir, false))
          {
            if (!mode.empty())
            {
              ANA_MSG_ERROR ("can't specify both an explicit submit-dir mode and optRemoveSubmitDir");
              return ::StatusCode::FAILURE;
            }
            data.submitDirMode = SubmitDirMode::OVERWRITE;
          }
        }
        break;

      case ManagerStep::createSubmitDir:
        {
          ANA_MSG_DEBUG ("using submit-dir mode: " << unsigned (data.submitDirMode));

          bool success {false};
          unsigned tries {0};
          std::string submitDir;
          std::size_t hash {0};
          while (success == false && tries < 10)
          {
            tries += 1;

            switch (data.submitDirMode)
            {
            case SubmitDirMode::NO_CLOBBER:
            case SubmitDirMode::OVERWRITE:
              submitDir = data.submitDir;
              break;

            case SubmitDirMode::UNIQUE:
            case SubmitDirMode::UNIQUE_LINK:
              {
                timeval tv;
                tm tvSplit;
                if (gettimeofday (&tv, nullptr) == -1 ||
                    localtime_r (&tv.tv_sec, &tvSplit) == nullptr)
                {
                  reportErrno ();
                  ANA_MSG_ERROR ("failed to get time of day???");
                  return ::StatusCode::FAILURE;
                }
                const std::string uniqueDateFormat {
                  data.options.castString (Job::optUniqueDateFormat,
                                           "-%Y-%m-%d-%H%M-")};
                char timeString [160];
                strftime (timeString, sizeof (timeString),
                          uniqueDateFormat.c_str(), &tvSplit);

                // make a hash value and reduce it to 16 bits
                boost::hash_combine (hash, std::hash<pid_t>() (getpid()));
                boost::hash_combine (hash, std::hash<suseconds_t>() (tv.tv_usec));
                std::size_t hash16 {hash};
                while (hash16 > 0xffff)
                  hash16 = (hash16&0xffff) ^ (hash16 >> 16);

                // we are appending both a date and a unique hash
                // here, in most cases the date should be sufficient
                // to make it unique, but for unit tests a lot of jobs
                // may be submitted in rapid succcession, so having
                // some random suffix should help avoid clashes.  the
                // date is purposely put first because in some cases
                // that can provide a useful ordering of output
                // directories, i.e. the latest results will be listed
                // last.
                submitDir = data.submitDir + timeString +
                  (boost::format ("%04x") % hash16).str();
                ANA_MSG_DEBUG ("unique submit-dir: " << submitDir);
              }
              break;
            }

            if (::mkdir (submitDir.c_str(), 0777) == 0)
            {
              success = true;
            } else
            {
              const int myerrno {errno};
              if (myerrno == EEXIST)
              {
                switch (data.submitDirMode)
                {
                case SubmitDirMode::NO_CLOBBER:
                  ANA_MSG_ERROR ("cowardly refusing to overwrite " << submitDir);
                  ANA_MSG_ERROR ("change the name or remove file/directory already there");
                  return ::StatusCode::FAILURE;
                case SubmitDirMode::OVERWRITE:
                  if (tries > 1)
                  {
                    ANA_MSG_ERROR ("failed to remove directory " << submitDir);
                    ANA_MSG_ERROR ("please try to remove it manually");
                    return ::StatusCode::FAILURE;
                  }
                  ANA_MSG_DEBUG ("removing directory " << submitDir);
                  gSystem->Exec (("rm -rf " + submitDir).c_str());
                  break;
                case SubmitDirMode::UNIQUE:
                case SubmitDirMode::UNIQUE_LINK:
                  // just pass through, try again with a new directory
                  // name next time
                  break;
                }
              } else
              {
                reportErrno (myerrno);
                ANA_MSG_ERROR ("failed to create directory: " << submitDir);
                return ::StatusCode::FAILURE;
              }
            }
          }

          if (success)
          {
            ANA_MSG_INFO ("created submission directory " + submitDir);
            switch (data.submitDirMode)
            {
            case SubmitDirMode::OVERWRITE:
            case SubmitDirMode::NO_CLOBBER:
            case SubmitDirMode::UNIQUE:
              // no-op
              break;

            case SubmitDirMode::UNIQUE_LINK:
              {
                if (unlink (data.submitDir.c_str()) == -1 && errno != ENOENT)
                {
                  reportErrno ();
                  ANA_MSG_ERROR ("failed to remove: " << data.submitDir);
                  return ::StatusCode::FAILURE;
                }

                std::string file = submitDir.substr (submitDir.rfind ('/')+1);
                if (symlink (file.c_str(), data.submitDir.c_str()) == -1)
                {
                  reportErrno ();
                  ANA_MSG_ERROR ("failed to create symlink at: " << data.submitDir);
                  return ::StatusCode::FAILURE;
                }
                ANA_MSG_INFO ("created sym-link at: " << data.submitDir);
              }
              break;
            }
            data.submitDir = submitDir;
          } else
          {
            ANA_MSG_ERROR ("tried " << tries << " times to create directory and failed: " << data.submitDir);
            ANA_MSG_ERROR ("try removing existing directory manually");
            return ::StatusCode::FAILURE;
          }
        }
        break;

      default:
        // no-op
        break;
      }
      return ::StatusCode::SUCCESS;
    }
  }
}
