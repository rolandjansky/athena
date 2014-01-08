!
!> RPC dead matter
!
 SUBROUTINE MATRPC
 IMPLICIT NONE
 CHARACTER(4) :: CAR4
 CHARACTER(8) :: CAR8
 REAL(8) :: VV(400),UU(100)
 REAL(8) :: XD(5),ZD(5)
 INTEGER :: JFF, JZZ, JFF16, JGEO, JOB, JSTA, JTEC, JTYP, MATSUP, NSUP
 REAL(8) :: DY, F1, X1, XEPS, Y1, Z1, ZEPS
 REAL(8), PARAMETER :: EPS=0.0100D0
!
#include "AmdcStand/amdcsim_com.inc"
!
!-- Statement functions
#include "AmdcStand/amdcsim_stfunc.inc"
!
    DO JTYP=1,MTYP
      DO JFF=1,8
        JFF16 = 2*JFF
        IF( CARTYP(JTYP)(3:3) == 'L' ) JFF16 = JFF16 - 1
        DO JZZ=-MZZ,MZZ
          JGEO = IGEO(JTYP,JFF,JZZ)
          IF( JGEO <= 0 )               CYCLE
          DO JOB=1,NOBJ(JTYP,JGEO)
            JTEC = ITEC(JTYP,JGEO,JOB)
            JSTA = ISTA(JTYP,JGEO,JOB)
            IF( CARTEC(JTEC) /= 'SUP' ) CYCLE
            X1 = Pos_R(JTYP,JFF,JZZ)/10.D0+Geo_dz(JTYP,JGEO,JOB)/10.D0
            Y1 = Pos_S(JTYP,JFF,JZZ)/10.D0+Geo_dx(JTYP,JGEO,JOB)/10.D0
            DY =                           Geo_Ws(JTYP,JGEO,JOB)/20.D0
            Z1 = Pos_Z(JTYP,JFF,JZZ)/10.D0+Geo_dy(JTYP,JGEO,JOB)/10.D0
            XD(1:4) = STATT(JTEC,JSTA,1:4)/10.D0
            ZD(1:4) = STAOO(JTEC,JSTA,1:4)/10.D0
            IF( JZZ < 0 )  ZD(1:4) = - ZD(1:4)
            XEPS = EPS
            ZEPS = SIGN( EPS , ZD(1) )
            MATSUP = NINT( STAPP(JTEC,JSTA) )
            NSUP = 0
            IF(NLAS(JTEC,JSTA) == 4 .AND. NLAZ(JTEC,JSTA) == 4) THEN
!-- Overlapping RPCs central support
              NSUP = 24
              UU((/  1, 2, 9,10 /)) = X1                        -XEPS
              UU((/  3, 4, 7, 8 /)) = X1-XD(1)
              UU((/  5, 6       /)) = X1      -XD(2)
              UU((/ 11,12       /)) = X1            +XD(3)      +XEPS
              UU((/ 13,14,21,22 /)) = X1            +XD(3)+XD(4)+XEPS
              UU((/ 15,16,19,20 /)) = X1+XD(1)      +XD(3)+XD(4)
              UU((/ 17,18       /)) = X1      +XD(2)+XD(3)+XD(4)
              UU((/ 23,24       /)) = X1                  +XD(4)-XEPS
              VV(1:70:3) = UU(1:24)
              VV(2:71:3) = Y1 + DY
              UU((/  1,24       /)) = Z1                        +ZEPS
              UU((/  2, 3       /)) = Z1      -ZD(2)
              UU((/  4, 5       /)) = Z1-ZD(1)
              UU((/  6, 7       /)) = Z1+ZD(1)            +ZD(4)
              UU((/  8, 9       /)) = Z1      +ZD(2)      +ZD(4)
              UU((/ 10,11       /)) = Z1                  +ZD(4)-ZEPS
              UU((/ 12,13       /)) = Z1            +ZD(3)+ZD(4)-ZEPS
              UU((/ 14,15       /)) = Z1      +ZD(2)+ZD(3)+ZD(4)
              UU((/ 16,17       /)) = Z1+ZD(1)      +ZD(3)+ZD(4)
              UU((/ 18,19       /)) = Z1-ZD(1)      +ZD(3)
              UU((/ 20,21       /)) = Z1      -ZD(2)+ZD(3)
              UU((/ 22,23       /)) = Z1            +ZD(3)      +ZEPS
              VV(3:72:3) = UU(1:24)
            ELSEIF(NLAS(JTEC,JSTA) == 2.AND.NLAZ(JTEC,JSTA) == 1) THEN
!-- Standard RPCs Edges supports
              NSUP = 8
              UU((/  1, 2       /)) = X1                        -XEPS
              UU((/  3, 4       /)) = X1-XD(1)
              UU((/  5, 6       /)) = X1+XD(1)+XD(2)
              UU((/  7, 8       /)) = X1      +XD(2)            +XEPS
              VV(1:22:3) = UU(1:8)
              VV(2:23:3) = Y1 + DY
              UU((/  1, 8       /)) = Z1                        -ZEPS
              UU((/  2, 3, 6, 7 /)) = Z1+ZD(1)
              UU((/  4, 5       /)) = Z1-SIGN(XD(1),ZD(1))
              VV(3:24:3) = UU(1:8)
            ENDIF
            IF( NSUP >= 3 ) THEN
              F1 = Pos_Phi(JTYP,JFF,JZZ)
              WRITE (CAR4,'(I4)') 100*JFF16+MZZ+JZZ
              CAR8 = 'S'//CARTYP(JTYP)//CAR4
              CALL NEWMAT( CAR8, MATSUP, JFF16, MZZ+JZZ )
              CALL PLAMATXX( F1, NSUP, VV )
              VV(2:71:3) = Y1 - DY
              CALL PLAMATXX( F1, NSUP, VV )
              CALL CLOMAT
              CALL ENDMAT
            ENDIF
          ENDDO
        ENDDO
      ENDDO
    ENDDO
!
 END SUBROUTINE MATRPC
!
