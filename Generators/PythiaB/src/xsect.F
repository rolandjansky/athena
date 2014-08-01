      SUBROUTINE XSECT(IOUT)
c
c --- Print out cross section taken from PYTHIA XSEC(0,3)
c --- and corrected to multiple hadronisation
c
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      INTEGER PYK,PYCHGE,PYCOMP
      COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
      COMMON/bevent/ihard
      common/repeat/NTB
            
	    
          WRITE(IOUT,5300)
          WRITE(IOUT,5400)

      xc=xsec(0,3)/ntb
      WRITE(IOUT,1)
      WRITE(IOUT,2) ntb
      WRITE(IOUT,3) XSEC(0,3)
      WRITE(IOUT,4) XC
      WRITE(IOUT,5) NGEN(0,3)
      if (keve.ne.0) then
      WRITE(IOUT,6)
      WRITE(IOUT,7) float(NGEN(0,3))/ihard

      endif

        WRITE(IOUT,5400)
        WRITE(IOUT,5300)



 1    FORMAT(8X,'CROSS SECTION CORRECTED FOR REPEATED HADRONISATION')
 2    FORMAT(8X,'Hadronization was repeated ',i6,' times')
 3    FORMAT(8X,'Cross section given by PYTHIA was: ',E10.4,'mbarn')
 4    FORMAT(8X,'Corrected cross section is:     ',E10.4,'mbarn')
 5    FORMAT(8X,'Number of accepted events is:   ',I7)
 6    FORMAT(8X,'AVERAGE NUM. OF ACCEPTED EVTS WITH THE SAME')
 7    FORMAT(8X,'HARD PROCESS',f10.5)



 5300 FORMAT(78('='))
 5400 FORMAT(78(' '))
      RETURN
      END
	    
