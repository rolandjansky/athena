!
!> Input  : TT    = ordered double precision real array
!>          NLONG = dimension of array TT
!>          AA    = one double precision real number
!> Output : JFINDD , such as TT(JFINDD-1) < AA < TT(JFINDD)
!> \author Saclay Muon Software Group (SaMuSoG)  19/01/94
!
 PURE INTEGER FUNCTION JFINDD(TT,NLONG,AA)
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: NLONG
 REAL(8), INTENT(IN) :: TT(*), AA
 INTEGER :: ILO, IME, IHI
!
    JFINDD = 0
    IF( NLONG <= 1 .OR. AA < TT(1) ) RETURN
    IF( AA > TT(NLONG) )             RETURN
!
    ILO = 1
    IME = NLONG / 2
    IHI = NLONG
10  IF( AA < TT(IME) ) THEN
      IF( IME-ILO.LE.1 ) THEN
        JFINDD = IME
        RETURN
      ELSE
        IHI = IME
      ENDIF
    ELSE
      IF( IHI-IME <= 1 ) THEN
        JFINDD = IHI
        RETURN
      ELSE
        ILO = IME
      ENDIF
    ENDIF
    IME = (IHI+ILO) / 2
    GO TO 10
!
 END FUNCTION JFINDD
!
