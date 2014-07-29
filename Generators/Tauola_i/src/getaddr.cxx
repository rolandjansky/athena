////////////////////////////////////////////////////////////////////////
// Routine meant to be called from FORTRAN which simply returns the
// address of the (FORTRAN) argument. This should be 64bit-safe.
//
// 1999/01/08 Chris Green (Purdue University)
// 1999/04/29 CG * altered to use void* from long*
////////////////////////////////////////////////////////////////////////

extern "C" {

  void* getaddr_(void* arg);

}

void* getaddr_(void* arg) {

  return(arg);

}
