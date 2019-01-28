/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoop/Worker.h>

#include <EventLoop/AlgorithmStateModule.h>
#include <EventLoop/EventCountModule.h>
#include <EventLoop/EventRange.h>
#include <EventLoop/FileExecutedModule.h>
#include <EventLoop/Job.h>
#include <EventLoop/LeakCheckModule.h>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/StopwatchModule.h>
#include <EventLoop/TEventSvc.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/RootUtils.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskWriter.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/ToolsOther.h>
#include <TFile.h>
#include <TH1.h>
#include <TTree.h>

//
// method implementations
//

namespace EL
{
  namespace
  {
    struct MyWriter : public SH::DiskWriter
    {
      /// description: this is a custom writer for the old-school
      ///   drivers that don't use an actual writer

    public:
      std::unique_ptr<TFile> m_file;

      explicit MyWriter (std::unique_ptr<TFile> val_file)
	: m_file (std::move (val_file))
      {}

      ~MyWriter ()
      {
	if (m_file != 0)
	  close();
      }

      std::string getPath () const
      {
	return "";
      }

      TFile *getFile ()
      {
	RCU_REQUIRE2_SOFT (m_file != 0, "file already closed");
	return m_file.get();
      }

      void doClose ()
      {
	RCU_REQUIRE2_SOFT (m_file != 0, "file already closed");
	m_file->Write ();
	m_file->Close ();
	m_file = 0;
      }
    };

    /// Helper class for managing the current ROOT directory during operations
    class DirectoryReset {
    public:
       /// Constructor, capturing which directory we need to return to
       DirectoryReset() : m_dir( *gDirectory ) {}
       /// Destructor, returning to the original directory
       ~DirectoryReset() { m_dir.cd(); }
    private:
       /// The directory we need to return to
       TDirectory& m_dir;
    };
  }



  void Worker ::
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
    RCU_INVARIANT (m_metaData != nullptr);
    RCU_INVARIANT (m_output != nullptr);
    for (std::size_t iter = 0, end = m_algs.size(); iter != end; ++ iter)
    {
      RCU_INVARIANT (m_algs[iter].m_algorithm != nullptr);
      RCU_INVARIANT (m_algs[iter]->wk() == this);
    }
    for (outputFilesIter iter = m_outputFiles.begin(),
	   end = m_outputFiles.end(); iter != end; ++ iter)
    {
      RCU_INVARIANT (iter->second != nullptr);
    }
  }



  Worker ::
  ~Worker ()
  {
    using namespace msgEventLoop;

    RCU_DESTROY_INVARIANT (this);

    for (outputFilesIter iter = m_outputFiles.begin(),
	   end = m_outputFiles.end(); iter != end; ++ iter)
    {
      delete iter->second;
    }

    ANA_MSG_INFO ("worker finished");
  }



  void Worker ::
  addOutput (TObject *output_swallow)
  {
    std::auto_ptr<TObject> output (output_swallow);

    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (output_swallow != 0);

    RCU::SetDirectory (output_swallow, 0);
    m_output->Add (output.release());

    TH1 *hist = dynamic_cast<TH1*> (output_swallow);
    if (hist)
      m_outputHistMap[hist->GetName()] = hist;
  }



  void Worker ::
  addOutputList (const std::string& name, TObject *output_swallow)
  {
    std::auto_ptr<TObject> output (output_swallow);

    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (output_swallow != 0);

    RCU::SetDirectory (output_swallow, 0);
    std::auto_ptr<TList> list (new TList);
    list->SetName (name.c_str());
    list->Add (output.release());
    addOutput (list.release());
  }



  TH1 *Worker ::
  getOutputHist (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);

    OutputHistMapIter iter = m_outputHistMap.find (name);
    if (iter == m_outputHistMap.end())
      RCU_THROW_MSG ("unknown output histogram: " + name);
    RCU_ASSERT (iter->second != 0);
    return iter->second;
  }



  TFile *Worker ::
  getOutputFile (const std::string& label) const
  {
    RCU_READ_INVARIANT (this);
    TFile *result = getOutputFileNull (label);
    if (result == 0)
      RCU_THROW_MSG ("no output dataset defined with label: " + label);
    return result;
  }



  TFile *Worker ::
  getOutputFileNull (const std::string& label) const
  {
    RCU_READ_INVARIANT (this);
    outputFilesIter iter = m_outputFiles.find (label);
    if (iter == m_outputFiles.end())
      return 0;
    return iter->second->file();
  }



  ::StatusCode Worker::
  addTree( const TTree& tree, const std::string& stream )
  {
     RCU_READ_INVARIANT( this );

     // Do not change the user's "current directory" during any of the
     // following...
     DirectoryReset dirReset;

     // Access the output file with this stream/label name.
     TFile* file = getOutputFileNull( stream );
     if( ! file ) {
        RCU_ERROR_MSG( "No output file with stream name \"" + stream +
                       "\" found" );
        return ::StatusCode::FAILURE;
     }

     // Make a clone of the tree, and make the output file own it from here
     // on out.
     file->cd();
     TTree* clone = dynamic_cast< TTree* >( tree.Clone() );
     if( ! clone ) {
        RCU_ERROR_MSG( "Unexpected error while cloning TTree" );
        return ::StatusCode::FAILURE;
     }
     clone->SetDirectory( file );

     // Hold on to the pointer of the tree in our internal cache.
     m_outputTreeMap[ std::make_pair( stream,
                                      std::string( clone->GetName() ) ) ] = clone;

     // Return gracefully:
     return ::StatusCode::SUCCESS;
  }



  TTree *Worker::
  getOutputTree( const std::string& name, const std::string& stream ) const
  {
     RCU_READ_INVARIANT( this );
     auto key = std::make_pair( stream, name );
     auto itr = m_outputTreeMap.find( key );
     if( itr == m_outputTreeMap.end() ) {
        RCU_THROW_MSG( "No tree with name \"" + name + "\" in stream \"" +
                       stream + "\"" );
     }
     RCU_ASSERT( itr->second != nullptr );
     return itr->second;
  }



  const SH::MetaObject *Worker ::
  metaData () const
  {
    RCU_READ_INVARIANT (this);
    return m_metaData;
  }



  TTree *Worker ::
  tree () const
  {
    RCU_READ_INVARIANT (this);
    return m_inputTree;
  }



  Long64_t Worker ::
  treeEntry () const
  {
    RCU_READ_INVARIANT (this);
    return m_inputTreeEntry;
  }



  TFile *Worker ::
  inputFile () const
  {
    RCU_READ_INVARIANT (this);
    return m_inputFile.get();
  }



  std::string Worker ::
  inputFileName () const
  {
    // no invariant used
    std::string path = inputFile()->GetName();
    auto split = path.rfind ('/');
    if (split != std::string::npos)
      return path.substr (split + 1);
    else
      return path;
  }



  TTree *Worker ::
  triggerConfig () const
  {
    RCU_READ_INVARIANT (this);
    return dynamic_cast<TTree*>(inputFile()->Get("physicsMeta/TrigConfTree"));
  }



  xAOD::TEvent *Worker ::
  xaodEvent () const
  {
    RCU_READ_INVARIANT (this);

    TEventSvc *const svc
      = dynamic_cast<TEventSvc*>(getAlg (TEventSvc::name));
    if (svc == 0)
      RCU_THROW_MSG ("Job not configured for xAOD support");
    return svc->event();
  }



  xAOD::TStore *Worker ::
  xaodStore () const
  {
    RCU_READ_INVARIANT (this);

    TEventSvc *const svc
      = dynamic_cast<TEventSvc*>(getAlg (TEventSvc::name));
    if (svc == 0)
      RCU_THROW_MSG ("Job not configured for xAOD support");
    return svc->store();
  }



  Algorithm *Worker ::
  getAlg (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);
    for (auto& alg : m_algs)
    {
      if (alg->hasName (name))
	return alg.m_algorithm.get();
    }
    return 0;
  }



  void Worker ::
  skipEvent ()
  {
    RCU_CHANGE_INVARIANT (this);
    m_skipEvent = true;
  }



  bool Worker ::
  filterPassed () const noexcept
  {
    RCU_READ_INVARIANT (this);
    return !m_skipEvent;
  }



  void Worker ::
  setFilterPassed (bool val_filterPassed) noexcept
  {
    RCU_CHANGE_INVARIANT (this);
    m_skipEvent = !val_filterPassed;
  }



  Worker ::
  Worker (const SH::MetaObject *val_metaData, TList *output)
  {
    RCU_REQUIRE (val_metaData != 0);
    RCU_REQUIRE (output != 0);

    m_metaData = val_metaData;
    m_output = output;

    RCU_NEW_INVARIANT (this);
  }



  void Worker ::
  setJobConfig (JobConfig&& jobConfig)
  {
    RCU_CHANGE_INVARIANT (this);
    for (std::unique_ptr<Algorithm>& alg : jobConfig.extractAlgorithms())
    {
      alg->m_wk = this;
      m_algs.push_back (std::move (alg));
    }
  }



  ::StatusCode Worker ::
  initialize ()
  {
    using namespace msgEventLoop;
    RCU_CHANGE_INVARIANT (this);

    m_modules.push_back (std::make_unique<Detail::LeakCheckModule> ());
    m_modules.push_back (std::make_unique<Detail::StopwatchModule> ());
    m_modules.push_back (std::make_unique<Detail::FileExecutedModule> ());
    m_modules.push_back (std::make_unique<Detail::EventCountModule> ());
    m_modules.push_back (std::make_unique<Detail::AlgorithmStateModule> ());

    m_jobStats = std::make_unique<TTree>
      ("EventLoop_JobStats", "EventLoop job statistics");
    m_jobStats->SetDirectory (nullptr);

    for (auto& module : m_modules)
      ANA_CHECK (module->preInitialize (*this));

    for (auto& module : m_modules)
      ANA_CHECK (module->onInitialize (*this));
    for (auto& module : m_modules)
      ANA_CHECK (module->postInitialize (*this));
    
    return ::StatusCode::SUCCESS;
  }



  ::StatusCode Worker ::
  finalize ()
  {
    using namespace msgEventLoop;

    RCU_CHANGE_INVARIANT (this);

    ANA_CHECK (openInputFile (""));
    for (auto& module : m_modules)
      ANA_CHECK (module->onFinalize (*this));
    for (outputFilesIter iter = m_outputFiles.begin(),
           end = m_outputFiles.end(); iter != end; ++ iter)
    {
      iter->second->close ();
      std::string path = iter->second->path ();
      if (!path.empty())
        addOutputList ("EventLoop_OutputStream_" + iter->first, new TObjString (path.c_str()));
    }
    for (auto& module : m_modules)
      ANA_CHECK (module->postFinalize (*this));
    if (m_jobStats->GetListOfBranches()->GetEntries() > 0)
    {
      if (m_jobStats->Fill() <= 0)
      {
        ANA_MSG_ERROR ("failed to fill the job statistics tree");
        return ::StatusCode::FAILURE;
      }
      m_output->Add (m_jobStats.release());
    }
    for (auto& module : m_modules)
      ANA_CHECK (module->onWorkerEnd (*this));
    return ::StatusCode::SUCCESS;
  }



  ::StatusCode Worker ::
  processEvents (EventRange& eventRange)
  {
    using namespace msgEventLoop;

    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (!eventRange.m_url.empty());
    RCU_REQUIRE (eventRange.m_beginEvent >= 0);
    RCU_REQUIRE (eventRange.m_endEvent == EventRange::eof || eventRange.m_endEvent >= eventRange.m_beginEvent);

    ANA_CHECK (openInputFile (eventRange.m_url));

    if (eventRange.m_beginEvent > inputFileNumEntries())
    {
      ANA_MSG_ERROR ("first event (" << eventRange.m_beginEvent << ") points beyond last event in file (" << inputFileNumEntries() << ")");
      return ::StatusCode::FAILURE;
    }
    if (eventRange.m_endEvent == EventRange::eof)
    {
      eventRange.m_endEvent = inputFileNumEntries();
    } else if (eventRange.m_endEvent > inputFileNumEntries())
    {
      ANA_MSG_ERROR ("end event (" << eventRange.m_endEvent << ") points beyond last event in file (" << inputFileNumEntries() << ")");
      return ::StatusCode::FAILURE;
    }

    m_inputTreeEntry = eventRange.m_beginEvent;
    if (m_newInputFile)
    {
      m_newInputFile = false;
      for (auto& module : m_modules)
        ANA_CHECK (module->onNewInputFile (*this));
    }

    if (eventRange.m_beginEvent == 0)
    {
      for (auto& module : m_modules)
        ANA_CHECK (module->onFileExecute (*this));
    }

    ANA_MSG_INFO ("Processing events " << eventRange.m_beginEvent << "-" << eventRange.m_endEvent << " in file " << eventRange.m_url);

    for (Long64_t event = eventRange.m_beginEvent;
         event != eventRange.m_endEvent;
         ++ event)
    {
      m_inputTreeEntry = event;
      ANA_CHECK (algsExecute ());
      m_eventsProcessed += 1;
      if (m_eventsProcessed % 10000 == 0)
        ANA_MSG_INFO ("Processed " << m_eventsProcessed << " events");
    }
    return ::StatusCode::SUCCESS;
  }



  ::StatusCode Worker ::
  openInputFile (std::string inputFileUrl)
  {
    using namespace msgEventLoop;

    RCU_CHANGE_INVARIANT (this);

    if (m_inputFileUrl == inputFileUrl)
      return ::StatusCode::SUCCESS;

    if (!m_inputFileUrl.empty())
    {
      if (m_newInputFile == false)
      {
        for (auto& module : m_modules)
          ANA_CHECK (module->onCloseInputFile (*this));
      }
      m_newInputFile = false;
      m_inputTree = nullptr;
      m_inputFile.reset ();
      m_inputFileUrl.clear ();
    }

    if (inputFileUrl.empty())
      return ::StatusCode::SUCCESS;

    ANA_MSG_INFO ("Opening file " << inputFileUrl);
    std::unique_ptr<TFile> inputFile;
    try
    {
      inputFile = SH::openFile (inputFileUrl, *metaData());
    } catch (...)
    {
      Detail::report_exception ();
    }
    if (inputFile.get() == 0)
    {
      ANA_MSG_ERROR ("failed to open file " << inputFileUrl);
      for (auto& module : m_modules)
        module->reportInputFailure (*this);
      return ::StatusCode::FAILURE;
    }
    if (inputFile->IsZombie())
    {
      ANA_MSG_ERROR ("input file is a zombie: " << inputFileUrl);
      for (auto& module : m_modules)
        module->reportInputFailure (*this);
      return ::StatusCode::FAILURE;
    }

    TTree *tree = 0;
    const std::string treeName
      = m_metaData->castString (SH::MetaFields::treeName, SH::MetaFields::treeName_default);
    tree = dynamic_cast<TTree*>(inputFile->Get (treeName.c_str()));
    if (tree == nullptr)
    {
      ANA_MSG_INFO ("tree " << treeName << " not found in input file: " << inputFileUrl);
      ANA_MSG_INFO ("treating this like a tree with no events");
    }

    m_newInputFile = true;
    m_inputTree = tree;
    m_inputFile = std::move (inputFile);
    m_inputFileUrl = std::move (inputFileUrl);

    return ::StatusCode::SUCCESS;
  }



  ::StatusCode Worker ::
  addOutputFile (const std::string& label, std::unique_ptr<TFile> file)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (file != 0);

    return addOutputWriter (label, std::make_unique<MyWriter> (std::move (file)));
  }



  ::StatusCode Worker ::
  addOutputWriter (const std::string& label,
                   std::unique_ptr<SH::DiskWriter> writer)
  {
    using namespace msgEventLoop;
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (writer != 0);

    if (m_outputFiles.find (label) != m_outputFiles.end())
    {
      ANA_MSG_ERROR ("output file already defined for label: " + label);
      return ::StatusCode::FAILURE;
    }
    m_outputFiles[label] = writer.get();
    writer.release();
    return ::StatusCode::SUCCESS;
  }



  ::StatusCode Worker ::
  algsExecute ()
  {
    using namespace msgEventLoop;
    RCU_CHANGE_INVARIANT (this);

    m_skipEvent = false;
    auto iter = m_algs.begin();
    try
    {
      for (auto end = m_algs.end();
           iter != end; ++ iter)
      {
        iter->m_executeCount += 1;
        if (iter->m_algorithm->execute() == StatusCode::FAILURE)
        {
          ANA_MSG_ERROR ("while calling execute() on algorithm " << iter->m_algorithm->GetName());
          return ::StatusCode::FAILURE;
        }

        if (m_skipEvent)
        {
          iter->m_skipCount += 1;
          return ::StatusCode::SUCCESS;
        }
      }
    } catch (...)
    {
      Detail::report_exception ();
      ANA_MSG_ERROR ("while calling execute() on algorithm " << iter->m_algorithm->GetName());
      return ::StatusCode::FAILURE;
    }

    /// rationale: this will make sure that the post-processing runs
    ///   for all algorithms for which the regular processing was run
    try
    {
      for (auto jter = m_algs.begin(), end = iter;
           jter != end && !m_skipEvent; ++ jter)
      {
        if (jter->m_algorithm->postExecute() == StatusCode::FAILURE)
        {
          ANA_MSG_ERROR ("while calling postExecute() on algorithm " << iter->m_algorithm->GetName());
          return ::StatusCode::FAILURE;
        }
      }
    } catch (...)
    {
      Detail::report_exception ();
      ANA_MSG_ERROR ("while calling postExecute() on algorithm " << iter->m_algorithm->GetName());
      return ::StatusCode::FAILURE;
    }

    if (!m_algs.empty() &&
        m_algs.back().m_executeCount == m_algs.back().m_skipCount + 1)
    {
      for (auto& module : m_modules)
        ANA_CHECK (module->postFirstEvent (*this));
    }

    return ::StatusCode::SUCCESS;
  }



  Long64_t Worker ::
  inputFileNumEntries () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (inputFile() != 0);

    if (m_inputTree != 0)
      return m_inputTree->GetEntries();
    else
      return 0;
  }



  uint64_t Worker ::
  eventsProcessed () const noexcept
  {
    RCU_READ_INVARIANT (this);
    return m_eventsProcessed;
  }



  void Worker ::
  addModule (std::unique_ptr<Detail::Module> module)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (module != nullptr);
    m_modules.push_back (std::move (module));
  }
}
