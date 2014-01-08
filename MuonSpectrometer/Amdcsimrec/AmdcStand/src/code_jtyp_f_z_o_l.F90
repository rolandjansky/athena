 SUBROUTINE CODE_JTYP_F_Z_O_L(Jtyp,Jff,Jzz,Jobj,Jlay, Icode)
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Jtyp, Jff, Jzz, Jobj, Jlay
 INTEGER, INTENT(OUT) :: Icode
!
#include "AmdcStand/amdcsim_param.inc"
!
    Icode = Jlay
    CALL MVBITS(Jobj   , 0, 6, Icode,  6)
    CALL MVBITS(Jzz+MZZ, 0, 6, Icode, 12)
    CALL MVBITS(Jff    , 0, 6, Icode, 18)
    CALL MVBITS(Jtyp   , 0, 8, Icode, 24)
!
 END SUBROUTINE CODE_JTYP_F_Z_O_L
!
 SUBROUTINE DECO_JTYP_F_Z_O_L(Icode, Jtyp,Jff,Jzz,Jobj,Jlay)
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Icode
 INTEGER, INTENT(OUT) :: Jtyp, Jff, Jzz, Jobj, Jlay
!
#include "AmdcStand/amdcsim_param.inc"
!
    Jtyp = IBITS( Icode, 24, 8)
    Jff  = IBITS( Icode, 18, 6)
    Jzz  = IBITS( Icode, 12, 6) - MZZ
    Jobj = IBITS( Icode,  6, 6)
    Jlay = IBITS( Icode,  0, 6)
!
 END SUBROUTINE DECO_JTYP_F_Z_O_L
!
 INTEGER FUNCTION FindObject(Jtyp,Jff,Jzz,Job, Ideb,Ifin, listObj)
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Jtyp,Jff,Jzz,Job, Ideb,Ifin, listObj(*)
 INTEGER :: Icode, Icod1, Iobj1, Iobj2, I
!
    FindObject = 0
    CALL CODE_JTYP_F_Z_O_L(Jtyp,Jff,Jzz,Job,0, Icode)
    Icod1 = IBITS(Icode,12,20)
    Iobj1 = IBITS(Icode, 6, 6)
    DO I=Ideb,Ifin
      IF( IBITS(listObj(I),12,20) /= Icod1 ) CYCLE
      IF( Iobj1 /= 0 ) THEN
        Iobj2 = IBITS( listObj(I), 6, 6)
        IF( Iobj2 /= 0 ) THEN
          IF( Iobj1 /= Iobj2 )               CYCLE
        ENDIF
      ENDIF
      FindObject = I
      EXIT
    ENDDO
!
 END FUNCTION FindObject
!
 INTEGER FUNCTION FindObjectICSC(Jtyp,Jff,Jzz,Job,JLay, Nobj,listObj)
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Jtyp,Jff,Jzz,Job,JLay, Nobj, listObj(*)
 INTEGER :: Icode, Icod1, Ilay1, Iobj1, Iobj2, Ilay2, I
!
    FindObjectICSC = 0
    CALL CODE_JTYP_F_Z_O_L(Jtyp,Jff,Jzz,Job,JLay, Icode)
    Icod1 = IBITS(Icode,12,20)
    Iobj1 = IBITS(Icode, 6, 6)
    Ilay1 = IBITS(Icode, 0, 6)
    DO I=1,Nobj
      IF( IBITS(listObj(I),12,20) /= Icod1 ) CYCLE
      IF( Iobj1 /= 0 ) THEN
        Iobj2 = IBITS( listObj(I), 6, 6)
        IF( Iobj2 /= 0 ) THEN
          IF( Iobj1 /= Iobj2 )               CYCLE
          IF (Ilay1/= 0 ) THEN
            Ilay2 = IBITS( listObj(I), 0, 6)
            IF ( Ilay2 /= 0 ) THEN
              IF( Ilay1 /= Ilay2 )               CYCLE
            ENDIF
          ENDIF
        ENDIF
      ENDIF
      FindObjectICSC = I
      EXIT
    ENDDO
!
 END FUNCTION FindObjectICSC
