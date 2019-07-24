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
#include <EventLoop/Job.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ShellExec.h>
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



  std::string KubernetesDriver ::
  batchReleaseSetup (bool /*sharedFileSystem*/) const
  {
    RCU_READ_INVARIANT (this);
    return "";
  }



  void KubernetesDriver ::
  batchSubmit (const std::string& location, const SH::MetaObject& options,
               const std::vector<std::size_t>& jobIndices, bool resubmit)
    const
  {
    RCU_READ_INVARIANT (this);

    const std::string dockerImage {
      options.castString(Job::optDockerImage)};

    /// \todo need to include this into my configuration file, once I
    /// figure out how to do this
    // const std::string dockerOptions {
    //   options.castString(Job::optDockerOptions)};

    /// \brief the data directory, if we have one
    const std::string dataDirectory {
      options.castString(Job::optDataDirectory)};

    std::ostringstream basedirName;
    basedirName << location << "/tmp";
    if (!resubmit)
    {
      if (gSystem->MakeDirectory (basedirName.str().c_str()) != 0)
        RCU_THROW_MSG ("failed to create directory " + basedirName.str());
    }

    const std::string jobFilePath {location + "/job.yml"};
    {
      bool first {true};
      std::ofstream jobFile (jobFilePath.c_str());
      for (std::size_t jobIndex : jobIndices)
      {
        std::ostringstream dirName;
        dirName << basedirName.str() << "/" << jobIndex;
        if (gSystem->MakeDirectory (dirName.str().c_str()) != 0)
          RCU_THROW_MSG ("failed to create directory " + dirName.str());

        if (first)
          first = false;
        else
          jobFile << "---\n";

        jobFile << "apiVersion: batch/v1\n";
        jobFile << "kind: Job\n";
        jobFile << "metadata:\n";

        //use this when there is a fixed name
        // jobFile << "  name: eventloop-" << jobIndex << "\n";

        jobFile << "  generateName: eventloop-" << jobIndex << "-\n";
        jobFile << "spec:\n";
        jobFile << "  backoffLimit: 0\n";
        jobFile << "  template:\n";
        jobFile << "    spec:\n";
        jobFile << "      restartPolicy: Never\n";
        jobFile << "      containers:\n";
        jobFile << "      - name: job\n";
        jobFile << "        image: " << dockerImage << "\n";
        jobFile << "        command:\n";
        jobFile << "        - bash\n";
        jobFile << "        - -c \n";
        jobFile << "        - |\n";
        jobFile << "          echo world\n";
        jobFile << "          echo my job index is $JOB_INDEX\n";
        jobFile << "          " << location << "/submit/run " << jobIndex << "\n";
        jobFile << "        env:\n";
        jobFile << "        - name: JOB_INDEX\n";
        jobFile << "          value: \"" << jobIndex << "\"\n";
        jobFile << "        volumeMounts:\n";
        jobFile << "        - mountPath: " << location << "\n";
        jobFile << "          name: submit-dir\n";
        if (!dataDirectory.empty())
        {
          jobFile << "        - mountPath: " << dataDirectory << "\n";
          jobFile << "          name: data-dir\n";
        }
        jobFile << "      volumes:\n";
        jobFile << "      - name: submit-dir\n";
        jobFile << "        hostPath:\n";
        jobFile << "          # directory location on host\n";
        jobFile << "          path: " << location << "\n";
        jobFile << "          # this field is optional\n";
        jobFile << "          type: Directory\n";
        if (!dataDirectory.empty())
        {
          jobFile << "      - name: data-dir\n";
          jobFile << "        hostPath:\n";
          jobFile << "          # directory location on host\n";
          jobFile << "          path: " << dataDirectory << "\n";
          jobFile << "          # this field is optional\n";
          jobFile << "          type: Directory\n";
        }
      }
    }

    std::ostringstream cmd;
    cmd << "kubectl create -f " << jobFilePath;
    RCU::Shell::exec (cmd.str());
  }
}
