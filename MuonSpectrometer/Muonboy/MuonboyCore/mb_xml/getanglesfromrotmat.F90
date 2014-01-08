!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE GetAnglesFromRotMat( Rot, Iok, AngX,AngY,AngZ )
 IMPLICIT NONE
!
 REAL(8), INTENT(IN)  :: Rot(3,3)
 INTEGER, INTENT(OUT) :: Iok
 REAL(8), INTENT(OUT) :: AngX,AngY,AngZ
!
 REAL(8), PARAMETER :: EpsLoc = 0.000001D0
 REAL(8)            :: SiX,SiY,SiZ,CoX,CoY,CoZ,ACoY1,ACoY2
 REAL(8)            :: AngXplus,AngYplus,AngZplus,AngXminus,AngYminus,AngZminus
!
    Iok  = 0
    AngX = 0.D0
    AngY = 0.D0
    AngZ = 0.D0
!
    SiY   = - Rot(1,3)
    ACoY1 = SQRT( Rot(1,1)**2 + Rot(1,2)**2 )
    ACoY2 = SQRT( Rot(2,3)**2 + Rot(3,3)**2 )
    IF( ABS(ACoY1-ACoY2) > EpsLoc ) RETURN
    CoY   = (ACoY1+ACoY2) / 2.D0
    IF( CoY < EpsLoc ) THEN
      IF( ABS(1.D0-ABS(SiY)) > EpsLoc .OR.   &
          ABS(Rot(3,1)-SiY*Rot(2,2)) > EpsLoc .OR. ABS(Rot(2,1)+SiY*Rot(3,2)) > EpsLoc ) RETURN
      SiZ  = (SiY*Rot(3,2)-Rot(2,1)) / 2.D0
      CoZ  = (SiY*Rot(3,1)+Rot(2,2)) / 2.D0
      Iok  = 1
      AngX = 0.D0
      AngY = ATAN2( SiY , CoY )
      AngZ = ATAN2( SiZ , CoZ )
    ELSE
      SiX = Rot(2,3) / CoY
      CoX = Rot(3,3) / CoY
      SiZ = Rot(1,2) / CoY
      CoZ = Rot(1,1) / CoY
      IF( ABS(CoZ*SiX*SiY-SiZ*CoX-Rot(2,1)) > EpsLoc .OR. ABS(CoZ*CoX*SiY+SiZ*SiX-Rot(3,1)) > EpsLoc .OR.  &
          ABS(SiZ*SiX*SiY+CoZ*CoX-Rot(2,2)) > EpsLoc .OR. ABS(SiZ*CoX*SiY-CoZ*SiX-Rot(3,2)) > EpsLoc ) RETURN
      Iok = 1
      AngXplus  = ATAN2(  SiX ,  CoX )
      AngYplus  = ATAN2(  SiY ,  CoY )
      AngZplus  = ATAN2(  SiZ ,  CoZ )
      AngXminus = ATAN2( -SiX , -CoX )
      AngYminus = ATAN2(  SiY , -CoY )
      AngZminus = ATAN2( -SiZ , -CoZ )
      IF( ABS(AngXplus)+ABS(AngYplus)+ABS(AngZplus) < ABS(AngXminus)+ABS(AngYminus)+ABS(AngZminus) ) THEN
        AngX = AngXplus
        AngY = AngYplus
        AngZ = AngZplus
      ELSE
        AngX = AngXminus
        AngY = AngYminus
        AngZ = AngZminus
      ENDIF
    ENDIF
!
 END
