/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RootUtils/src/pyroot/PyROOTTFilePythonize.cxx
 * @author Sebastien Binet
 * @date Nov 2009
 * @brief pythonize access to (raw) TFile
 */

// stdlib includes
#include <stdlib.h>
#include <stdio.h>

#include "TFile.h"

// local includes
#include "RootUtils/PyROOTTFilePythonize.h"

namespace RootUtils {

PyBytes::PyBytes(size_t buf_sz) : 
  sz(buf_sz>0?buf_sz:0), 
  buf(sz)
{
}

PyBytes::~PyBytes() 
{
}

PyBytes::PyBytes(const PyBytes& rhs) : 
  sz(rhs.sz), 
  buf(rhs.buf)
{
}

PyBytes&
PyBytes::operator=(const PyBytes& rhs)
{
  if (this != &rhs) {
    this->sz = rhs.sz;
    this->buf = rhs.buf;
  }
  return *this;
}

Long64_t
_pythonize_tell_root_file(TFile* f)
{
  if (f) {
    return f->GetRelOffset();
  }
  fprintf (stderr, 
           "::RootUtils::_pythonize_tell_root_file got a NULL TFile ptr!\n");
  return 0;
}

PyBytes
_pythonize_read_root_file(TFile* f, Int_t len)
{
  if (!f) {
    //err
    return PyBytes(0);
  }

  Long64_t totsz  = f->GetSize();
  Long64_t curpos = _pythonize_tell_root_file(f);
  Long64_t remain = totsz - curpos;
  remain = (remain>0) ? remain : 0;

  len = (len>remain) ? remain : len;
  PyBytes buf(len);
  if (f->ReadBuffer((char*)buf.buf.data(), buf.sz)) {
    //err
    return PyBytes(0);
  }

  f->Seek(buf.sz + curpos);
  return buf;
}

} //> namespace RootUtils
