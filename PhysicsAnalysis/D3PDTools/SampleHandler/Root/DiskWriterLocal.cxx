/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2013.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/DiskWriterLocal.h>

#include <exception>
#include <iostream>
#include <TFile.h>
#include <TSystem.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

namespace SH
{
  void DiskWriterLocal :: 
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  DiskWriterLocal :: 
  DiskWriterLocal (const std::string& val_path)
    : m_path (val_path)
  {
    RCU_REQUIRE (!val_path.empty());

    std::string dir = gSystem->DirName (val_path.c_str());
    gSystem->mkdir (dir.c_str(), true);
    m_file = new TFile (val_path.c_str(), "RECREATE");

    RCU_NEW_INVARIANT (this);
  }



  DiskWriterLocal :: 
  ~DiskWriterLocal ()
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



  std::string DiskWriterLocal ::
  getPath () const
  {
    RCU_READ_INVARIANT (this);
    return m_path;
  }



  TFile *DiskWriterLocal ::
  getFile ()
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE2_SOFT (m_file != 0, "file already closed");
    return m_file;
  }



  void DiskWriterLocal ::
  doClose ()
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE2_SOFT (m_file != 0, "file already closed");

    if (m_file->Write () < 0)
      RCU_THROW_MSG ("failed to write to file: " + m_path);
    m_file->Close ();
    delete m_file;
    m_file = 0;
  }
}
