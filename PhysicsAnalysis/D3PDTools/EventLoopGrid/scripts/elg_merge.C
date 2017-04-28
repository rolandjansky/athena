/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <EventLoop/OutputStream.h>
#include <RootCoreUtils/ThrowMsg.h>

void try_exec(const std::string& cmd)
{
  if (gSystem->Exec(cmd.c_str()) != 0) {
    RCU_THROW_MSG("command failed: " + cmd);
  }
}

void elg_merge(const std::string& jobDefFile, 
	       const std::string& output, 
	       const std::string& input)
{
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
      type = os->options()->getString(EL::OutputStream::optType);
      mergeCmd = os->options()->getString(EL::OutputStream::optMergeCmd);
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
