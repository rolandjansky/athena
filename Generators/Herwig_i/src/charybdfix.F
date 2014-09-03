      subroutine charybdfix()

c----------------------------------------------------------------------
c   program initialization
c----------------------------------------------------------------------
c
c  HERWIG common blocks (`external` loads in  block data)
c-----------------------------------------------------------------------
#include "HERWIG65.INC"
#include "jimmy.inc"
      external hwudat
C--RELABEL HARD AS BLACK HOLE
      IDHW(6)=NRES
      IDHEP(6)=40

      end
      
      
