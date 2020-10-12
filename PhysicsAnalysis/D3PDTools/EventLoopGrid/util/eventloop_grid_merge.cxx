/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <EventLoop/OutputStream.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <xAODRootAccess/Init.h>
#include <AsgMessaging/MessageCheck.h>
#include <TList.h>
#include <TSystem.h>
#include <TFile.h>
#include <regex>

void try_exec(const std::string& cmd)
{
  if (gSystem->Exec(cmd.c_str()) != 0) {
    RCU_THROW_MSG("command failed: " + cmd);
  }
}

int main (int argc, char **argv)
{
  using namespace asg::msgUserCode;
  ANA_CHECK_SET_TYPE (int);

  ANA_CHECK (xAOD::Init ());

  if (argc != 4)
  {
    ANA_MSG_ERROR ("invalid number of arguments");
    return -1;
  }

  std::string jobDefFile = argv[1];
  std::string output = argv[2];
  std::string input = argv[3];

  std::string type = "DEFAULT";
  std::string mergeCmd = "";
  
  TFile jd(jobDefFile.c_str());

  std::unique_ptr<TList> outs{dynamic_cast<TList*>(jd.Get("outputs"))};
  if (outs.get() == nullptr) {
    RCU_THROW_MSG("Unable to get list of output streams");
  }
  TIter itr(outs.get());
  TObject* obj;
  while ((obj = itr())) {
    EL::OutputStream* os = dynamic_cast<EL::OutputStream*>(obj);
    if (std::regex_match(output, std::regex(os->label() + "\\.root.*")) ||
	std::regex_match(output, std::regex(".*\\." + os->label() + "\\.root.*")))
    {
      type = os->options()->castString(EL::OutputStream::optType, "", SH::MetaObject::CAST_NOCAST_DEFAULT);
      mergeCmd = os->options()->castString(EL::OutputStream::optMergeCmd, "", SH::MetaObject::CAST_NOCAST_DEFAULT);
      break;
    }
  }

  if (mergeCmd.empty()) {
    if (type == "xAOD") {
      mergeCmd = "xAODMerge -s";
    } else if (type == "xAODNoMeta") {
      mergeCmd = "xAODMerge";
    } else {
      mergeCmd = "hadd -f";
    }  
  }
  mergeCmd += " " + output + " " + input;
  try_exec(mergeCmd);
}
