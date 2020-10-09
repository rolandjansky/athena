/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <EventLoop/KubernetesDriver.h>

#include <fstream>
#include <sstream>
#include <TSystem.h>
#include <AsgMessaging/StatusCode.h>
#include <EventLoop/Job.h>
#include <EventLoop/ManagerData.h>
#include <EventLoop/MessageCheck.h>
#include <PathResolver/PathResolver.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ShellExec.h>
#include <RootCoreUtils/StringUtil.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

ClassImp(EL::KubernetesDriver)

namespace EL
{
  void KubernetesDriver ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  KubernetesDriver ::
  KubernetesDriver ()
  {
    RCU_NEW_INVARIANT (this);
  }



  ::StatusCode KubernetesDriver ::
  doManagerStep (Detail::ManagerData& data) const
  {
    RCU_READ_INVARIANT (this);
    using namespace msgEventLoop;
    ANA_CHECK (BatchDriver::doManagerStep (data));
    switch (data.step)
    {
    case Detail::ManagerStep::batchScriptVar:
      {
        data.batchSkipReleaseSetup = true;
      }
      break;

    case Detail::ManagerStep::submitJob:
    case Detail::ManagerStep::doResubmit:
      {
        const std::string dockerImage {
          data.options.castString(Job::optDockerImage)};

        const std::string dockerOptions {
          data.options.castString(Job::optDockerOptions)};
        if (!dockerOptions.empty())
        {
          ANA_MSG_WARNING ("you specified docker options for kubernetes driver");
          ANA_MSG_WARNING ("this is not supported in this way");
          ANA_MSG_WARNING ("instead you need to provide your own kubernetes config file");
        }

        /// \brief the setup file we use as a template
        const std::string batchSetupFile {
          data.options.castString(Job::optBatchSetupFile, "EventLoop/kubernetes_setup.yml")};


        /// \brief the config file we use as a template
        const std::string batchConfigFile {
          data.options.castString(Job::optBatchConfigFile, "EventLoop/kubernetes_job.yml")};
        std::string baseConfig;
        {
          std::ifstream file (PathResolverFindDataFile (batchConfigFile).c_str());
          baseConfig = std::string (std::istreambuf_iterator<char>(file),
                                    std::istreambuf_iterator<char>() );
        }
        baseConfig = RCU::substitute (baseConfig, "%%DOCKERIMAGE%%", dockerImage);
        baseConfig = RCU::substitute (baseConfig, "%%SUBMITDIR%%", data.submitDir);

        std::ostringstream basedirName;
        basedirName << data.submitDir << "/tmp";
        if (!data.resubmit)
        {
          if (gSystem->MakeDirectory (basedirName.str().c_str()) != 0)
            RCU_THROW_MSG ("failed to create directory " + basedirName.str());
        }

        const std::string jobFilePath {data.submitDir + "/job.yml"};
        {
          bool first {true};
          std::ofstream jobFile (jobFilePath.c_str());
          if (!batchSetupFile.empty())
          {
            std::ifstream file (PathResolverFindDataFile (batchSetupFile).c_str());
            std::string setupConfig {std::istreambuf_iterator<char>(file),
                std::istreambuf_iterator<char>()};
            setupConfig = RCU::substitute (setupConfig, "%%DOCKERIMAGE%%", dockerImage);
            setupConfig = RCU::substitute (setupConfig, "%%SUBMITDIR%%", data.submitDir);
            jobFile << setupConfig;
            first = false;
          }

          for (std::size_t jobIndex : data.batchJobIndices)
          {
            std::ostringstream dirName;
            dirName << basedirName.str() << "/" << jobIndex;
            if (gSystem->MakeDirectory (dirName.str().c_str()) != 0)
              RCU_THROW_MSG ("failed to create directory " + dirName.str());

            if (first)
              first = false;
            else
              jobFile << "---\n";

            std::string myConfig = baseConfig;
            myConfig = RCU::substitute (myConfig, "%%JOBINDEX%%", std::to_string (jobIndex));
            std::ostringstream command;
            command << data.submitDir << "/submit/run " << jobIndex;
            myConfig = RCU::substitute (myConfig, "%%COMMAND%%", command.str());

            jobFile << myConfig << "\n";
          }
        }

        std::ostringstream cmd;
        cmd << "kubectl create -f " << jobFilePath;
        RCU::Shell::exec (cmd.str());
        data.submitted = true;
      }
      break;

    default:
      break;
    }
    return ::StatusCode::SUCCESS;
  }
}
