// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHEXSYCL_DEVICESELECTOR_H
#define ATHEXSYCL_DEVICESELECTOR_H

// SYCL include(s).
#include <CL/sycl.hpp>

namespace AthSYCL {

   /// Selector for "viable" SYCL devices
   ///
   /// This selector tries to make sure that only a viable device would be
   /// used during runtime. This mainly just means excluding NVidia OpenCL
   /// devices. :-P
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class DeviceSelector : public cl::sycl::device_selector {

   public:
      /// @name Function(s) inherited from @c cl::sycl::device_selector
      /// @{

      /// Function used for "scoring" the different devices
      virtual int operator()( const cl::sycl::device& device ) const override;

      /// @}

   private:
      /// A default device selector
      cl::sycl::default_selector m_defaultSelector;

   }; // class DeviceSelector

} // namespace AthSYCL

#endif // ATHEXSYCL_DEVICESELECTOR_H
