/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack
/// @author Attila Krasznahorkay



#ifndef EVENT_LOOP__LEAK_CHECK_MODULE_H
#define EVENT_LOOP__LEAK_CHECK_MODULE_H

#include <EventLoop/Global.h>

#include <EventLoop/Module.h>
#include <Rtypes.h>

namespace EL
{
  namespace Detail
  {
    /// \brief a \ref Module implementing a simple leak checker

    class LeakCheckModule final : public Module
    {
      //
      // public interface
      //

    public:
      virtual ::StatusCode postFirstEvent (ModuleData& data) override;

    public:
      virtual ::StatusCode onWorkerEnd (ModuleData& data) override;

    public:
      virtual ::StatusCode postFinalize (ModuleData& data) override;



      //
      // private interface
      //

      /// \brief Amount of resident memory used after initialisation in kB
    private:
      Long_t m_initMemResident = -1; //!

      /// \brief Amount of virtual memory used after initialisation in kB
    private:
      Long_t m_initMemVirtual = -1; //!

      /// \brief Amount of resident memory used after finalisation in kB
    private:
      Long_t m_finMemResident = -1; //!

      /// \brief Amount of virtual memory used after finalisation in kB
    private:
      Long_t m_finMemVirtual = -1; //!


      /// \brief Resident memory leak/increase during the job
      ///
      /// The idea here is that the worker captures the amount of memory used by
      /// the job during initialisation and finalisation, so that a rough estimate
      /// could be made about the memory leak behaviour of the user's code.
      ///
      /// This function returns the number of kilobytes by which the used resident
      /// memory changed between initialisation and finalisation. If the function
      /// is called at an incorrect point in time, it throws an exception.
      ///
    private:
      Long_t memIncreaseResident () const;


      /// \brief Virtual memory leak/increase during the job
      ///
      /// The idea here is that the worker captures the amount of memory used by
      /// the job during initialisation and finalisation, so that a rough estimate
      /// could be made about the memory leak behaviour of the user's code.
      ///
      /// This function returns the number of kilobytes by which the used virtual
      /// memory changed between initialisation and finalisation. If the function
      /// is called at an incorrect point in time, it throws an exception.
      ///
    private:
      Long_t memIncreaseVirtual () const;
    };
  }
}

#endif
