// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RootUtils/PyROOTTFilePythonize.h
 * @author Sebastien Binet
 * @date Nov 2009
 * @brief Pythonize the access to @c TFile so they can be used as file-like 
 *        python objects.
 *
 */


#ifndef ROOTUTILS_PYROOTTFILEPYTHONIZE_H
#define ROOTUTILS_PYROOTTFILEPYTHONIZE_H 1

// standard includes
#include <inttypes.h> //> for size_t

// ROOT includes
#include "TFile.h"

// forward declarations


namespace RootUtils {


class PyBytes
{
public:
  /**
   * @brief wrapper around @c char* to explicitly mean bytes.
   * @param buf_sz The size of the buffer (in bytes)
   */
  PyBytes(size_t buf_sz=0);

  ~PyBytes();

  PyBytes(const PyBytes& rhs);

  PyBytes& operator=(const PyBytes& rhs);

  // data members

  /** the size of the buffer of bytes
   */
  size_t sz;

  /** the buffer of bytes
   */
  std::vector<char> buf;

  /** 
   * After we drop py2, we can simplify the code in PyUtils.RootUtils.py
   * by just returning a bytes object from here.
   */
  void* buffer() const { return (void*)buf.data(); }

};

/** @brief read `len` bytes from file `f`
 */
PyBytes _pythonize_read_root_file(TFile* f, Int_t len);

/** @brief tell the current position in file `f`
 */
Long64_t _pythonize_tell_root_file(TFile* f);

} // namespace RootUtils


#endif // not ROOTUTILS_PYROOTTFILEPYTHONIZE_H
