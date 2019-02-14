/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/OutputStreamData.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/RootUtils.h>

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    void OutputStreamData ::
    saveOutput ()
    {
      TFile *const file {m_writer->file()};
      RCU_ASSERT (file != nullptr);
      for (std::unique_ptr<TObject>& object : m_output)
      {
        TDirectory *dir {file};
        std::string path = object->GetName();
        std::string name;
        std::string::size_type split = path.rfind ("/");
        if (split == std::string::npos)
        {
          name = path;
        } else
        {
          std::string dirname = path.substr (0, split);
          name = path.substr (split + 1);
          TNamed *named = dynamic_cast<TNamed*>(object.get());
          if (named)
            named->SetName (name.c_str());
          dir = dynamic_cast<TDirectory*>(file->Get (dirname.c_str()));
          if (!dir)
          {
            file->mkdir (dirname.c_str());
            dir = dynamic_cast<TDirectory*>(file->Get (dirname.c_str()));
          }
          RCU_ASSERT (dir != 0);
        }

        if (!RCU::SetDirectory (object.get(), dir))
          dir->WriteObject (object.get(), name.c_str());
        object.release();
      }
      file->Write ();
      m_outputHistMap.clear ();
      m_outputTreeMap.clear ();
      m_output.clear ();
    }
  }
}
