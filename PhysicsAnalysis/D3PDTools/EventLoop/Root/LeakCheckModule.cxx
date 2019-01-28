/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack
/// @author Attila Krasznahorkay



//
// includes
//

#include <EventLoop/LeakCheckModule.h>

#include <EventLoop/Job.h>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/ModuleData.h>
#include <SampleHandler/MetaObject.h>
#include <TSystem.h>
#include <TTree.h>
#include <exception>

//
// method implementations
//

namespace EL
{
  namespace Detail
  {
    ::StatusCode LeakCheckModule ::
    postFirstEvent (ModuleData& /*data*/)
    {
      using namespace msgEventLoop;

      // Get the memory usage of the process after initialisation.
      ::ProcInfo_t pinfo;
      if (gSystem->GetProcInfo (&pinfo) != 0) {
        ANA_MSG_ERROR ("Could not get memory usage information");
        return ::StatusCode::FAILURE;
      }
      m_initMemResident = pinfo.fMemResident;
      m_initMemVirtual = pinfo.fMemVirtual;
      ANA_MSG_DEBUG ("starting memory: " << pinfo.fMemResident << " " << pinfo.fMemVirtual);
      return ::StatusCode::SUCCESS;
    }



    ::StatusCode LeakCheckModule ::
    postFinalize (ModuleData& data)
    {
      using namespace msgEventLoop;

      {
        // Get the memory usage of the process after finalisation.
        ::ProcInfo_t pinfo;
        if (gSystem->GetProcInfo (&pinfo) != 0) {
          ANA_MSG_ERROR ("Could not get memory usage information");
          return ::StatusCode::FAILURE;
        }
        m_finMemResident = pinfo.fMemResident;
        m_finMemVirtual = pinfo.fMemVirtual;
        ANA_MSG_DEBUG ("finishing memory: " << pinfo.fMemResident << " " << pinfo.fMemVirtual);

        // Save the memory increase values into the job statistics tree.
        RCU_ASSERT (data.m_jobStats != nullptr);
        Float_t incRes = memIncreaseResident();
        if (! data.m_jobStats->Branch ("memIncreaseResident", &incRes)) {
          ANA_MSG_ERROR ("Failed to create branch memIncreaseResident");
          return ::StatusCode::FAILURE;
        }
        Float_t incVirt = memIncreaseVirtual();
        if (! data.m_jobStats->Branch ("memIncreaseVirtual", &incVirt)) {
          ANA_MSG_ERROR ("Failed to create branch memIncreaseVirtual");
          return ::StatusCode::FAILURE;
        }
      }
      return ::StatusCode::SUCCESS;
    }



    ::StatusCode LeakCheckModule ::
    onWorkerEnd (ModuleData& data)
    {
      using namespace msgEventLoop;

      // Perform a memory leak check in case at least one event was processed.
      if (data.m_eventsProcessed > 0) {

        // Extract the limits for producing an error.
        const int absResidentLimit =
          data.m_metaData->castInteger (Job::optMemResidentIncreaseLimit,
                                   10000);
        const int absVirtualLimit =
          data.m_metaData->castInteger (Job::optMemVirtualIncreaseLimit,
                                   0);
        const int perEvResidentLimit =
          data.m_metaData->castInteger (Job::optMemResidentPerEventIncreaseLimit,
                                   10);
        const int perEvVirtualLimit =
          data.m_metaData->castInteger (Job::optMemVirtualPerEventIncreaseLimit,
                                   0);

        // Calculate and print the memory increase of the job.
        const Long_t resLeak = memIncreaseResident();
        const Double_t resLeakPerEv =
          (static_cast< Double_t > (resLeak) /
           static_cast< Double_t > (data.m_eventsProcessed));
        const Long_t virtLeak = memIncreaseVirtual();
        const Double_t virtLeakPerEv =
          (static_cast< Double_t > (virtLeak) /
           static_cast< Double_t > (data.m_eventsProcessed) );
        ANA_MSG_INFO ("Memory increase/change during the job:");
        ANA_MSG_INFO ("  - resident: " << resLeakPerEv << " kB/event ("
                      << resLeak << " kB total)");
        ANA_MSG_INFO ("  - virtual : " << virtLeakPerEv << " kB/event ("
                      << virtLeak << " kB total)");

        // Decide if this acceptable or not.
        if ((resLeak > absResidentLimit) &&
            (resLeakPerEv > perEvResidentLimit) &&
            (virtLeak > absVirtualLimit) &&
            (virtLeakPerEv > perEvVirtualLimit)) {

          // If not, decide what to do about it.
          if (data.m_metaData->castBool (Job::optMemFailOnLeak, false)) {
            ANA_MSG_ERROR ("A significant memory leak was detected");
            return ::StatusCode::FAILURE;
          } else {
            ANA_MSG_WARNING ("*");
            ANA_MSG_WARNING ("* A significant memory leak was detected");
            ANA_MSG_WARNING ("*");
          }
        }
      }
      return ::StatusCode::SUCCESS;
    }



    Long_t LeakCheckModule ::
    memIncreaseResident () const
    {
      // Check that the user called the function at the correct time.
      if ((m_initMemResident == -1) || (m_finMemResident == -1)) {
        throw std::logic_error ("Function called at incorrect time");
      }
      // Return the resident memory increase.
      return (m_finMemResident - m_initMemResident);
    }



    Long_t LeakCheckModule ::
    memIncreaseVirtual () const
    {
      // Check that the user called the function at the correct time.
      if ((m_initMemVirtual == -1) || (m_finMemVirtual == -1)) {
        throw std::logic_error ("Function called at incorrect time");
      }
      // Return the resident memory increase.
      return (m_finMemVirtual - m_initMemVirtual);
    }
  }
}
