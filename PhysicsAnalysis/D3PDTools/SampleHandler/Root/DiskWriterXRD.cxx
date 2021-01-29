/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <SampleHandler/DiskWriterXRD.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ShellExec.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <TFile.h>
#include <TSystem.h>
#include <boost/format.hpp>
#include <boost/functional/hash.hpp>
#include <chrono>
#include <iostream>
#include <random>
#include <sstream>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

//
// method implementations
//

namespace SH
{
  void DiskWriterXRD :: 
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  DiskWriterXRD :: 
  DiskWriterXRD (const std::string& val_path)
    : m_path (val_path)
  {
    RCU_REQUIRE (val_path.find ("root://") == 0);

    const char *tmpdir = getenv ("TMPDIR");
    std::size_t hash {0};
    boost::hash_combine (hash, std::hash<pid_t>() (getpid()));
    std::size_t tries = 0;
    while (m_file == nullptr || !m_file->IsOpen())
    {
      if (++ tries == 10)
        throw std::runtime_error ("infinite loop trying to create tempory file for DiskWriterXRD");

      auto time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
      boost::hash_combine (hash, std::hash<decltype(time)>() (time));
      std::size_t hash16 {hash};
      while (hash16 > 0xffff)
        hash16 = (hash16&0xffff) ^ (hash16 >> 16);

      std::ostringstream str;
      if (tmpdir)
	str << tmpdir;
      else
	str << "/tmp";
      str << "/SH-XRD-" << m_path.substr (m_path.rfind ("/")+1)
          << "-" << (boost::format ("%04x") % hash16).str();
      m_tmp = str.str();
      if (gSystem->AccessPathName (m_tmp.c_str()) != 0)
	m_file.reset (TFile::Open (m_tmp.c_str(), "CREATE"));
    }

    RCU_NEW_INVARIANT (this);
  }



  DiskWriterXRD :: 
  ~DiskWriterXRD ()
  {
    RCU_DESTROY_INVARIANT (this);

    if (m_file != nullptr)
    {
      try
      {
	close ();
      } catch (std::exception& e)
      {
	std::cerr << "exception closing file " << m_path << ": "
		  << e.what() << std::endl;
	
      } catch (...)
      {
	std::cerr << "unknown exception closing file " << m_path << std::endl;
      }
    }
  }



  std::string DiskWriterXRD ::
  getPath () const
  {
    RCU_READ_INVARIANT (this);
    return m_path;
  }



  TFile *DiskWriterXRD ::
  getFile ()
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE2_SOFT (m_file != nullptr, "file already closed");
    return m_file.get();
  }



  void DiskWriterXRD ::
  doClose ()
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE2_SOFT (m_file != nullptr, "file already closed");

    if (m_file->IsOpen())
    {
      if (m_file->Write () < 0)
	RCU_THROW_MSG ("failed to write to file: " + m_path);
      m_file->Close ();
    }

    std::random_device rd;
    std::mt19937 gen (rd());
    bool success = false;
    unsigned tries = 0u;
    while (!success)
    {
      try
      {
        // using the -f flag, because if this copy failed previously
        // we need to force an overwrite.  note that there would be no
        // point in leaving this out on the first try (even though
        // there should be no file there), because it would just fail
        // and retry with the flag set.
        RCU::Shell::exec ("xrdcp -f " + RCU::Shell::quote (m_tmp) + " " + RCU::Shell::quote (m_path));
        success = true;
      } catch (...)
      {
        std::cerr << "encountered error copying files to XRD path: \"" << m_path << "\"" << std::endl;
        if (tries < 10u)
        {
          tries += 1;
          // sleeping for a random period of time, to reduce the
          // chance that the problem is that multiple jobs finishing
          // at the same time keep overloading the server by
          // repeatedly hitting it at the same time.
          unsigned seconds = std::uniform_int_distribution<>(30,60) (gen);
          std::cerr << "sleeping for " << seconds << " seconds before retrying" << std::endl;
          std::this_thread::sleep_for (std::chrono::seconds(seconds));
        } else
        {
          std::cerr << "giving up, leaving file at " << m_tmp << std::endl;
          throw std::runtime_error ("failed to copy file to XRD");
        }
      }
    }
    RCU::Shell::exec ("rm " + RCU::Shell::quote (m_tmp));
    m_file.reset ();
  }
}
