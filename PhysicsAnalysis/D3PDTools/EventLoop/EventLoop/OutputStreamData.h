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
    };
  }
}

#endif
