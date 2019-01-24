/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__OUTPUT_STREAM_DATA_H
#define EVENT_LOOP__OUTPUT_STREAM_DATA_H

#include <EventLoop/Global.h>

#include <SampleHandler/DiskWriter.h>
#include <TFile.h>
#include <memory>
#include <unordered_map>

class TH1;
class TTree;

namespace EL
{
  namespace Detail
  {
    /// \brief all data needed to manage a given output stream

    struct OutputStreamData final
    {
      /// \brief the file we use (instead of a writer)
      std::unique_ptr<TFile> m_file;

      /// \brief the writer we use
      std::unique_ptr<SH::DiskWriter> m_writer;

      /// \brief the list of objects to write out at the end of job
      std::vector<std::unique_ptr<TObject> > m_output;

      /// \brief the output histogram map
      std::unordered_map<std::string,TH1*> m_outputHistMap;

      /// \brief the output tree map
      std::unordered_map<std::string,TTree*> m_outputTreeMap;


      /// \brief write the list of output objects to disk and clear it
      ///
      /// This is made static and public, because depending on the
      /// implementation it may be called either from the Driver or the
      /// Worker.  however, normal users would have no interest in
      /// calling it.
      ///
      /// \par Guarantee
      ///   basic
      /// \par Failures
      ///   i/o errors
      void saveOutput ();
    };
  }
}

#endif
