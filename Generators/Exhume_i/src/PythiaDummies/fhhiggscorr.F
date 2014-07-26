
C*********************************************************************

C...FHHIGGSCORR
C...Dummy function, to be removed when FEYNHIGGS is to be linked.

      SUBROUTINE FHHIGGSCORR(IERR, RMHIGG, SAEFF, UHIGGS)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

C...FeynHiggs variables
      DOUBLE PRECISION RMHIGG(4), SAEFF, UHIGGS(3,3) 
C...Commonblocks.
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      SAVE /PYDAT1/
 
C...Stop program if this routine is ever called.
      WRITE(MSTU(11),5000)
      IF(PYR(0).LT.10D0) STOP

      IERR = IERR ! dummy line to avoid compiler warnings
      do i=1,4
        RMHIGG(i) = RMHIGG(i) ! dummy line to avoid compiler warnings
      enddo
      SAEFF = SAEFF ! dummy line to avoid compiler warnings
      do 10 i=1,3
       do 11 j=1,3
         UHIGGS(i,j) = UHIGGS(i,j)  ! dummy line to avoid compiler warnings
 11    continue
 10   continue
 
C...Format for error printout.
 5000 FORMAT(1X,'Error: you did not link FEYNHIGGS correctly.'/
     &1X,'Dummy routine FHSETPARA in PYTHIA file called instead.'/
     &1X,'Execution stopped!')
      RETURN
      END
