/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <SampleHandler/DiskWriterXRD.h>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <TFile.h>
#include <TSystem.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ShellExec.h>
#include <RootCoreUtils/ThrowMsg.h>

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
    : m_path (val_path), m_file (0)
  {
    RCU_REQUIRE (val_path.find ("root://") == 0);

    const char *tmpdir = getenv ("TMPDIR");
    unsigned index = 0;
    while (m_file == 0 || !m_file->IsOpen())
    {
      std::ostringstream str;
      if (tmpdir)
	str << tmpdir;
      else
	str << "/tmp";
      str << "/SH-XRD-" << getpid() << "-" << index << "-";
      str << m_path.substr (m_path.rfind ("/")+1);
      m_tmp = str.str();
      if (gSystem->AccessPathName (m_tmp.c_str()) != 0)
	m_file = new TFile (m_tmp.c_str(), "CREATE");
    }

    RCU_NEW_INVARIANT (this);
  }



  DiskWriterXRD :: 
  ~DiskWriterXRD ()
  {
    RCU_DESTROY_INVARIANT (this);

    if (m_file != 0)
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
    RCU_REQUIRE2_SOFT (m_file != 0, "file already closed");
    return m_file;
  }



  void DiskWriterXRD ::
  doClose ()
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE2_SOFT (m_file != 0, "file already closed");

    if (m_file->IsOpen())
    {
      if (m_file->Write () < 0)
	RCU_THROW_MSG ("failed to write to file: " + m_path);
      m_file->Close ();
    }
    RCU::Shell::exec ("xrdcp " + RCU::Shell::quote (m_tmp) + " " + RCU::Shell::quote (m_path));
    RCU::Shell::exec ("rm " + RCU::Shell::quote (m_tmp));
    delete m_file;
    m_file = 0;
  }
}
