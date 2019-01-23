/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__FILE_EXECUTED_MODULE_H
#define EVENT_LOOP__FILE_EXECUTED_MODULE_H

#include <EventLoop/Global.h>

#include <EventLoop/Module.h>
#include <memory>

class TString;
class TTree;

namespace EL
{
  namespace Detail
  {
    /// \brief a \ref Module recording when FileExecuted was called

    class FileExecutedModule final : public Module
    {
      //
      // public interface
      //

    public:
      virtual ~FileExecutedModule () noexcept;

    public:
      virtual ::StatusCode postInitialize (ModuleData& data) override;

    public:
      virtual ::StatusCode onFileExecute (ModuleData& data) override;

    public:
      virtual ::StatusCode postFinalize (ModuleData& data) override;



      //
      // private interface
      //

      /// \brief the tree containing the list of files for which
      /// fileExecute has been called
    private:
      std::unique_ptr<TTree> m_fileExecutedTree; //!

      /// \brief the name of the file being executed, to be stored
      /// inside \ref m_fileExecutedTree
    private:
      TString *m_fileExecutedName {nullptr}; //!
    };
  }
}

#endif
