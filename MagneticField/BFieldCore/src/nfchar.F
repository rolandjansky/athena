*
*
*
*

* Author : Saclay Muon Software Group (SaMuSoG)
***************************************************************************
      INTEGER FUNCTION NFCHAR(L1,L2,A)
*
*>> Input  : L1     = 1 to 4
*>> Input  : L2     = L1 to 4
*>> Input  : A(4)   = array of CHARACTER*1
*>> Output : NFCHAR = integer corresponding to A(L1) to A(L2)
*>> Author : Saclay Muon Software Group (SaMuSoG)
*>> Date   : 14/02/97
*
      IMPLICIT NONE
      INTEGER,      INTENT(IN) :: L1, L2
      CHARACTER(1), INTENT(IN) :: A(4)
*
      INTEGER       :: I, J, L
      INTEGER, SAVE :: IPASS=0, IAAADR(0:255), I70L(0:3)
      CHARACTER(1)  :: AAA(70)
*
      DATA AAA/'0','1','2','3','4','5','6','7','8','9',
     +         'a','b','c','d','e','f','g','h','i','j',
     +         'k','l','m','n','o','p','q','r','s','t',
     +         'u','v','w','x','y','z','+','-','(',')',
     +         'A','B','C','D','E','F','G','H','I','J',
     +         'K','L','M','N','O','P','Q','R','S','T',
     +         'U','V','W','X','Y','Z','.',',',':',';'/
*
      IF(IPASS.LE.0) THEN
        IPASS = 1
        FORALL (I=1:70) IAAADR(ICHAR(AAA(I))) = I
        FORALL (I=0:3)  I70L(I) = 70**I
      ENDIF
*
      NFCHAR = 0
      DO L=L1,L2
        J = ICHAR(A(L))
        I = IAAADR(J)
        IF(I.LT.1) THEN
          PRINT 1000,A(L)
1000      FORMAT(/' =====> Problem in mag. field unpacking :'/
     >    ' =====> Character ',A1,' is unknown !!!!!!'/
     >    ' =====> STOP !!!!!!'/)
          STOP
        ENDIF
        NFCHAR = NFCHAR + (I-1)*I70L(L2-L)
      ENDDO
*
      RETURN
      END
