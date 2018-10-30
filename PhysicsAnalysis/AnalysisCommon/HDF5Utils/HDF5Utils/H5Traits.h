// this is -*- C++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef H5_TRAITS_H
#define H5_TRAITS_H

/**
 * HDF5 Traits
 *
 * This is a collection of tools to make HDF5 strongly typed
 *
 **/


namespace H5 {
  class DataType;
}

namespace H5Utils {

  /// @brief Internal clssses and code
  /// @{

  namespace internal {

    /** @brief data_buffer_t
     *
     * This buffer element is used by the HDF5 library to store data
     * which is about to be written to disk.
     **/
    union data_buffer_t
    {
      int _int;
      long long _llong;
      unsigned int _uint;
      unsigned char _uchar;
      float _float;
      double _double;
      bool _bool;
    };

    /**
     * We have lots of code to get around HDF5's rather weak typing. Some
     * of this is filled out more in the cxx file.
     **/
    template <typename T> struct H5Traits;
    template <> struct H5Traits<int> {
      static const H5::DataType type;
      static int& ref(data_buffer_t& buf) { return buf._int; }
    };
    template <> struct H5Traits<long long> {
      static const H5::DataType type;
      static long long& ref(data_buffer_t& buf) { return buf._llong; }
    };
    template <> struct H5Traits<unsigned int> {
      static const H5::DataType type;
      static unsigned int& ref(data_buffer_t& buf) { return buf._uint; }
    };
    template <> struct H5Traits<unsigned char> {
      static const H5::DataType type;
      static unsigned char& ref(data_buffer_t& buf) { return buf._uchar; }
    };
    template <> struct H5Traits<float> {
      static const H5::DataType type;
      static float& ref(data_buffer_t& buf) { return buf._float; }
    };
    template <> struct H5Traits<double> {
      static const H5::DataType type;
      static double& ref(data_buffer_t& buf) { return buf._double; }
    };
    template <> struct H5Traits<bool> {
      static const H5::DataType type;
      static bool& ref(data_buffer_t& buf) { return buf._bool; }
    };

  }
}

#endif
