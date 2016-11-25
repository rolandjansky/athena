/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_MonitoringcArrayUtilities_h
#define SCT_MonitoringcArrayUtilities_h

/**    @file cArrayUtilities.cxx
 *
 *
 *  @author  Shaun Roe
 *  Header amnd implementation for utiities dealing with
 *  C-style arrays
 */


namespace SCT_Monitoring {
  // give the total size of a 3D C-style array
  template<class T, int Size1, int Size2, int Size3>
  int
  extent(T (&parray)[Size1][Size2][Size3]) {
    return Size1 * Size2 * Size3;
  }

  // give the total size of a 2D C-style array
  template<class T, int Size1, int Size2>
  int
  extent(T (&parray)[Size1][Size2]) {
    return Size1 * Size2;
  }

  // give beyond-the-end pointer for 2D C-style array
  template<class T, int Size1, int Size2>
  T *
  endof(T (&parray)[Size1][Size2]) {
    return &(parray[0][0]) + extent(parray);
  }

  // give the byte size of a 3D C-style array
  template<class T, int Size1, int Size2, int Size3>
  int
  byte_extent3D(T (&)[Size1][Size2][Size3]) {
    return Size1 * Size2 * Size3 * sizeof(T);
  }

  // give the byte size of a 2D C-style array
  template<class T, int Size1, int Size2>
  int
  byte_extent(T (&)[Size1][Size2]) {
    return Size1 * Size2 * sizeof(T);
  }

  // give the byte size of a 1D C-style array
  template<class T, int Size1>
  int
  byte_extent1D(T (&)[Size1]) {
    return Size1 * sizeof(T);
  }

  template <class T, int Size1, int Size2, int Size3>
  void
  clear3D(T (&parray)[Size1][Size2][Size3]) {
    memset(parray, 0, byte_extent3D(parray));
  }

  template <class T, int Size1, int Size2>
  void
  clear(T (&parray)[Size1][Size2]) {
    memset(parray, 0, byte_extent(parray));
  }

  // for 1-D arrays
  template <class T, int Size1>
  void
  clear1D(T (&parray)[Size1]) {
    memset(parray, 0, byte_extent1D(parray));
  }
}
#endif
