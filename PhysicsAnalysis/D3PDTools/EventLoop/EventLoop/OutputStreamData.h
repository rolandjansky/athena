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
class StatusCode;
class TTree;

namespace EL
{
  namespace Detail
  {
    /// \brief all data needed to manage a given output stream

    class OutputStreamData final
    {
      //
      // public interface
      //

      /// \brief test the invariant of this object
    public:
      void testInvariant () const;


      /// \brief open the given file and create an output stream for it
      /// \par Guarantee
      ///   strong
      /// \par Failures
      ///   out of memory II
    public:
      explicit OutputStreamData (const std::string& val_fileName, const std::string& mode);


      /// \brief create this output stream for a pre-opened file
      /// \par Guarantee
      ///   strong
      /// \par Failures
      ///   out of memory II
    public:
      explicit OutputStreamData (std::unique_ptr<TFile> val_file);


      /// \brief create this output stream for a custom writer
      /// \par Guarantee
      ///   strong
      /// \par Failures
      ///   out of memory II
    public:
      explicit OutputStreamData (std::unique_ptr<SH::DiskWriter> val_writer);


      /// \brief the file we are writing to
      /// \par Guarantee
      ///   no-fail
      /// \post result != nullptr
    public:
      TFile *file () const noexcept;


      /// \brief write the list of output objects to disk and clear it
      ///
      /// \par Guarantee
      ///   basic
      /// \par Failures
      ///   i/o errors
    public:
      void saveOutput ();


      /// \brief close this file
      /// \par Guarantee
      ///   basic
      /// \par Failures
      ///   i/o errors
    public:
      void close ();


      /// \brief the final path of the file created
      /// \par Guarantee
      ///   strong
      /// \par Failures
      ///   out of memory II
    public:
      std::string finalFileName () const;


      /// \brief add the given output object to this stream
      ///
      /// While the caller transfers ownership to this object, he may
      /// retain a reference to the object until \ref saveOutput or
      /// \ref close is called.
      ///
      /// \par Guarantee
      ///   basic
      /// \par Failures
      ///   out of memory II
    public:
      void addOutput (std::unique_ptr<TObject> outputObject);


      /// \brief add a clone of the given object to the output
      ///
      /// \par Guarantee
      ///   basic
      /// \par Failures
      ///   cloning failures\n
      ///   out of memory II
    public:
      void addClone (const TObject& prototypeObject);


      /// \brief make the directory for the object of the given name
      ///
      /// This will make sure that we pick the proper sub-directory if
      /// needed.
      ///
      /// \par Guarantee
      ///   basic
      /// \par Failures
      ///   directory creation failures
    public:
      TDirectory *makeDirectoryFor (std::string& name);


      /// \brief get the output histogram with the given name, or
      /// nullptr if there is no histogam with such a name
      ///
      /// \par Guarantee
      ///   no-fail
    public:
      TH1 *getOutputHist (const std::string& name) const noexcept;


      /// \brief get the output tree with the given name, or nullptr
      /// if there is no tree with such a name
      ///
      /// \par Guarantee
      ///   no-fail
    public:
      TTree *getOutputTree (const std::string& name) const noexcept;



      //
      // private interface
      //

      /// \brief the writer we use
    private:
      std::unique_ptr<SH::DiskWriter> m_writer;

      /// \brief the list of objects to write out at the end of job
    private:
      std::vector<std::unique_ptr<TObject> > m_output;

      /// \brief the output histogram map
    private:
      std::unordered_map<std::string,TH1*> m_outputHistMap;

      /// \brief the output tree map
    private:
      std::unordered_map<std::string,TTree*> m_outputTreeMap;
    };
  }
}

#endif
