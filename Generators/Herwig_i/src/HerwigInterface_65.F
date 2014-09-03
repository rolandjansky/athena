c
      subroutine herwiginterface(itopd)
c----------------------------------------------------------------------
c   program initialization
c----------------------------------------------------------------------
c
c  HERWIG common blocks (`external` loads in  block data)
c-----------------------------------------------------------------------
#include "HERWIG65.INC"
#include "jimmy.inc"
      EXTERNAL HWUDAT
c-----------------------------------------------------------------------
c  local variables
c-----------------------------------------------------------------------
      INTEGER       ITOPD
      EXTERNAL HWHSCT
      
c------------------------------------------------------------------------------ 
c  set defaults
c------------------------------------------------------------------------------

      CALL HWIGIN                       ! set HERWIG defaults

C(BPK)->CHARYBDIS 1.003 FIX
C---DEFINE BLACK HOLE
      NRES=NRES+1
      RNAME(NRES)='BlacHole'
      IDPDG(NRES)=40

      IF (ITOPD.GT.0) CALL TOPDEC

      CALL JIMMIN

c     Don't write out the sudaov tables by default
      LWSUD         = 0

      RETURN
      END

C
C     The following two dummy routines are needed by Herwig...

      Subroutine Hvcbvi
#include "HERWIG65.INC"

      Write(6,10)
 10   Format(/10x,'HVCBVI called but not linked')
      end


      Subroutine Hwaend
      end

c
      Subroutine cdfreadsusy(filnam,iunit)
      character*(*)  filnam
c      integer status
c      integer sigtlu, sifrlu
      integer iunit
c-----------------------------------------------------------------------
c  open the SUSY particle properties file
c-----------------------------------------------------------------------

c      status        = sigtlu(iunit)
      write(*,*) ' '
      write (*,6660)
 6660 format (' SUSY filename is ')
      write (*,*) filnam
      write (*,6664) iunit
 6664 format ('      and unit number is ',i6)
C
      open(unit=iunit,file=filnam,status='old',err=101
     &     ,FORM='FORMATTED',ACCESS='SEQUENTIAL')
      goto 102
c-----------------------------------------------------------------------
c  error: can't open SUSY file
c-----------------------------------------------------------------------
101   continue
      write(*,*) ' '
      write(*,*) ' ********************************************* '
      write(*,*) ' '
      write(*,*) 
     $     ' Error in opening SUSY file for READING'
      write(*,*)
     $     ' Program will abort.'
      write(*,*) ' '
      write(*,*) ' ********************************************* '
      write(*,*) ' '
102   continue
      end
c
c-----------------------------------------------------------------------
c  dummy hwanal, called from HerwigModule.cxx, can be useful
      subroutine hwanal(ihwcod)
c  input ihwcod = 1, set to zero if want this event skipped
#include "HERWIG65.INC"
      integer ihwcod
c
c
c skip if ierror ne 0
      if(ierror.ne.0) return
c
cc  do whatever here
cc  if want to skip this event, say  ihwcod = 0
c
      end
