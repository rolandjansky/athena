/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/FileExecutedModule.h>

#include <EventLoop/ModuleData.h>
#include <TString.h>
#include <TTree.h>

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    FileExecutedModule ::
    ~FileExecutedModule () noexcept
    {
      m_fileExecutedTree.release ();
      delete m_fileExecutedName;
    }



    ::StatusCode FileExecutedModule ::
    postInitialize (ModuleData& /*data*/)
    {
      m_fileExecutedTree = std::make_unique<TTree>
        ("EventLoop_FileExecuted", "executed files");
      m_fileExecutedTree->SetDirectory (nullptr);
      m_fileExecutedName = new TString;
      m_fileExecutedTree->Branch ("file", &m_fileExecutedName);
      return ::StatusCode::SUCCESS;
    }



    ::StatusCode FileExecutedModule ::
    onFileExecute (ModuleData& data)
    {
      auto split = data.m_inputFileUrl.rfind ('/');
      if (split != std::string::npos)
        *m_fileExecutedName = data.m_inputFileUrl.substr (split + 1);
      else
        *m_fileExecutedName = data.m_inputFileUrl;
      m_fileExecutedTree->Fill ();
      return ::StatusCode::SUCCESS;
    }



    ::StatusCode FileExecutedModule ::
    postFinalize (ModuleData& data)
    {
      data.addOutput (std::move (m_fileExecutedTree));
      return ::StatusCode::SUCCESS;
    }
  }
}
