!
!> \author SaMuSoG
!
 SUBROUTINE GET_STATION_SZT_TRANSFORMATION( Jtyp,Jff,Jzz,Job, Iok, TransformDir,TransformBak )
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Jtyp,Jff,Jzz,Job
 INTEGER, INTENT(OUT) :: Iok
 REAL(8), INTENT(OUT) :: TransformDir(3,4),TransformBak(3,4)
 REAL(8) :: Translation(3),Rotation(3,3)
 REAL(8) :: RotS(3,3),RotZ(3,3),RotT(3,3),RotA(3,3)
 REAL(8) :: Alfa(3),SZT(3)
 INTEGER :: Ideb, Ifin
 INTEGER :: FindObject
!
#include "AmdcStand/amdcsim_com.inc"
!
!-- Statement functions
#include "AmdcStand/amdcsim_stfunc.inc"
!
      Iok = 0
      TransformDir( 1 ,1:4) = (/ 1.D0, 0.D0, 0.D0, 0.D0 /)
      TransformDir( 2 ,1:4) = (/ 0.D0, 1.D0, 0.D0, 0.D0 /)
      TransformDir( 3 ,1:4) = (/ 0.D0, 0.D0, 1.D0, 0.D0 /)
      TransformBak(1:3,1:4) = TransformDir(1:3,1:4)
!
      Ifin = LastAdjustByType(Jtyp)
      IF( Ifin <= 0 ) RETURN
      Ideb = FirstAdjustByType(Jtyp)
      Iok  = FindObject(Jtyp,Jff,Jzz,Job, Ideb,Ifin, JTYPFZOLadjust) 
      IF( Iok <= 0 )  RETURN
      IF( IBAREL(Jtyp,Jff,Jzz) > 0 ) THEN
        Translation(1:3) =  SZTtraadjust(1:3,Iok)
        Alfa(1:3)        =  SZTrotadjust(1:3,Iok)
      ELSE
        IF( Jzz > 0 ) THEN
          Translation( 1 ) = -SZTtraadjust( 1 ,Iok)
          Translation(2:3) =  SZTtraadjust(2:3,Iok)
          Alfa( 1 )        =  SZTrotadjust( 1 ,Iok)
          Alfa(2:3)        = -SZTrotadjust(2:3,Iok)
        ELSE
          Translation(1:2) =  SZTtraadjust(1:2,Iok)
          Translation( 3 ) = -SZTtraadjust( 3 ,Iok)
          Alfa(1:2)        = -SZTrotadjust(1:2,Iok)
          Alfa( 3 )        =  SZTrotadjust( 3 ,Iok)
        ENDIF
      ENDIF
      RotS(1:3,1:3) =   0.D0
      RotZ(1:3,1:3) =   0.D0
      RotT(1:3,1:3) =   0.D0
      RotS( 1 , 1 ) =   1.D0
      RotS( 2 , 2 ) =   COS(Alfa(1))
      RotS( 3 , 2 ) =   SIN(Alfa(1))
      RotS( 2 , 3 ) = - SIN(Alfa(1))
      RotS( 3 , 3 ) =   COS(Alfa(1))
      RotZ( 2 , 2 ) =   1.D0
      RotZ( 1 , 1 ) =   COS(Alfa(2))
      RotZ( 3 , 1 ) = - SIN(Alfa(2))
      RotZ( 1 , 3 ) =   SIN(Alfa(2))
      RotZ( 3 , 3 ) =   COS(Alfa(2))
      RotT( 3 , 3 ) =   1.D0
      RotT( 1 , 1 ) =   COS(Alfa(3))
      RotT( 2 , 1 ) =   SIN(Alfa(3))
      RotT( 1 , 2 ) = - SIN(Alfa(3))
      RotT( 2 , 2 ) =   COS(Alfa(3))
      CALL MULT3M3M( RotT,RotZ, RotA )
      CALL MULT3M3M( RotA,RotS, Rotation )
      IF( IBAREL(Jtyp,Jff,Jzz) > 0 ) THEN
        SZT(1) = Pos_S(Jtyp,Jff,Jzz)
        SZT(2) = Pos_Z(Jtyp,Jff,Jzz)
        SZT(3) = Pos_R(Jtyp,Jff,Jzz)
      ELSE
        SZT(1) = Pos_S(Jtyp,Jff,Jzz)
        SZT(2) = Pos_R(Jtyp,Jff,Jzz)
        SZT(3) = Pos_Z(Jtyp,Jff,Jzz)
      ENDIF
      TransformDir(1:3,1:3) = Rotation(1:3,1:3)
      TransformDir(1:3, 4 ) = SZT(1:3) + Translation(1:3)  &
                            - Rotation(1:3,1)*SZT(1)       &
                            - Rotation(1:3,2)*SZT(2)       &
                            - Rotation(1:3,3)*SZT(3)
      Rotation = TRANSPOSE(Rotation)                 ! inversion
      TransformBak(1:3,1:3) = Rotation(1:3,1:3)
      TransformBak(1:3, 4 ) = SZT(1:3)                                 &
                            - Rotation(1:3,1)*(SZT(1)+Translation(1))  &
                            - Rotation(1:3,2)*(SZT(2)+Translation(2))  &
                            - Rotation(1:3,3)*(SZT(3)+Translation(3))
!
 END SUBROUTINE GET_STATION_SZT_TRANSFORMATION
!
