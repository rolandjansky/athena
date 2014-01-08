!
!> \author M.Virchaux
!
 SUBROUTINE Shift_to_True_Seg(Ic1,Iw1, Sin1,Zin1,Tin1,               &
                              Ic2,Iw2, Sin2,Zin2,Tin2, F12,          &
                              XuM,YuM,ZuM, XuD,YuD,ZuD,              &
                              dxaxisX,dxaxisY,dxaxisZ,dyaxisX,dyaxisY,dyaxisZ,dzaxisX,dzaxisY,dzaxisZ )
 USE M_MB_Digis
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Ic1,Iw1, Ic2,Iw2
 REAL(8), INTENT(IN)  :: Sin1,Zin1,Tin1, Sin2,Zin2,Tin2, F12
 REAL(8), INTENT(OUT) :: XuM,YuM,ZuM, XuD,YuD,ZuD
 REAL(8), INTENT(OUT) :: dxaxisX,dxaxisY,dxaxisZ,dyaxisX,dyaxisY,dyaxisZ,dzaxisX,dzaxisY,dzaxisZ
 INTEGER :: Igeom1, Igeom2, Itran1, Itran2
 REAL(8) :: TZ01, TZ02, ZTin1, TZin1, ZTin2, TZin2
 REAL(8) :: S1a, ZT1a, TZ1a, S2a, ZT2a, TZ2a
 REAL(8) :: S1b, ZT1b, TZ1b, S2b, ZT2b, TZ2b
 REAL(8) :: SuL,ZuL,TuL, CFF, SFF, Xu1, Yu1, Zu1, Tu1, Xu2, Yu2, Zu2, Tu2
 REAL(8) :: Su1, ZTu1, TZu1, Su2, ZTu2, TZu2, OuD
 REAL(8) :: A0,SinA0,CosA0
 REAL(8) :: Tu_N,Zu_N, Stmp,Ztmp,Ttmp
!
   IF( Iw1 <= 0 ) THEN
     Igeom2 = IGEOMA(Ic2,IFCHA(Ic2,Iw2),IZCHA(Ic2,Iw2))
     Igeom1 = Igeom2
     TZ01 = TCECHAV(Igeom2) + 5.D0
     TZ02 = TCECHAV(Igeom2) - 5.D0
   ELSEIF( Iw2 <= 0 ) THEN
     Igeom1 = IGEOMA(Ic1,IFCHA(Ic1,Iw1),IZCHA(Ic1,Iw1))
     Igeom2 = Igeom1
     TZ01 = TCECHAV(Igeom1) + 5.D0
     TZ02 = TCECHAV(Igeom1) - 5.D0
   ELSE
     Igeom1 = IGEOMA(Ic1,IFCHA(Ic1,Iw1),IZCHA(Ic1,Iw1))
     Igeom2 = IGEOMA(Ic2,IFCHA(Ic2,Iw2),IZCHA(Ic2,Iw2))
     TZ01 = TCECHAV(Igeom1)
     TZ02 = TCECHAV(Igeom2)
   ENDIF
   Itran1 = ItransCHA( Igeom1 )
   Itran2 = ItransCHA( Igeom2 )

   IF( Ic1 <= NCOUBA ) THEN
     ZTin1 = Zin1
     TZin1 = Tin1
     ZTin2 = Zin2
     TZin2 = Tin2
   ELSE
     A0    = GCECHAV(Igeom1)
     SinA0 = SIN(A0)
     CosA0 = COS(A0)
     ZTin1 = CosA0 * Tin1 - SinA0 * Zin1
     TZin1 = SinA0 * Tin1 + CosA0 * Zin1
     ZTin2 = CosA0 * Tin2 - SinA0 * Zin2
     TZin2 = SinA0 * Tin2 + CosA0 * Zin2  
   ENDIF
   CALL Pos_Ideal_from_True(Itran1, Sin1,ZTin1,TZin1, S1a,ZT1a,TZ1a)
   CALL Pos_Ideal_from_True(Itran2, Sin2,ZTin2,TZin2, S2a,ZT2a,TZ2a)
   CALL Pos_Ideal_from_True(Itran1, Sin2,ZTin2,TZin2, Stmp,Ztmp,Ttmp)
   SuL  = Stmp -S1a
   ZuL  = Ztmp-ZT1a
   TuL  = Ttmp-TZ1a
   S1b  = S1a  + (TZ01-TZ1a) * SuL/TuL
   ZT1b = ZT1a + (TZ01-TZ1a) * ZuL/TuL
   TZ1b =         TZ01
   CALL Pos_Ideal_from_True(Itran2, Sin1,ZTin1,TZin1, Stmp,Ztmp,Ttmp)
   SuL  = S2a -Stmp
   ZuL  = ZT2a-Ztmp
   TuL  = TZ2a-Ttmp
   S2b  = S2a  + (TZ02-TZ2a) * SuL/TuL
   ZT2b = ZT2a + (TZ02-TZ2a) * ZuL/TuL
   TZ2b =         TZ02
   CALL Pos_True_from_Ideal(Itran1, S1b,ZT1b,TZ1b, Su1,ZTu1,TZu1)
   CALL Pos_True_from_Ideal(Itran2, S2b,ZT2b,TZ2b, Su2,ZTu2,TZu2)
   IF( Ic1 <= NCOUBA ) THEN
     Zu1 = ZTu1
     Tu1 = TZu1
     Zu2 = ZTu2
     Tu2 = TZu2
   ELSE
     Zu1 = TZu1
     Tu1 = ZTu1
     Zu2 = TZu2
     Tu2 = ZTu2
     Tu_N =  CosA0 * Tu1 + SinA0 * Zu1
     Zu_N = -SinA0 * Tu1 + CosA0 * Zu1
     Tu1 = Tu_N
     Zu1 = Zu_N
     Tu_N =  CosA0 * Tu2 + SinA0 * Zu2
     Zu_N = -SinA0 * Tu2 + CosA0 * Zu2
     Tu2 = Tu_N
     Zu2 = Zu_N
   ENDIF
!
   CFF = COS(F12)
   SFF = SIN(F12)
   Xu1 = CFF * Tu1 - SFF * Su1
   Yu1 = SFF * Tu1 + CFF * Su1
   Xu2 = CFF * Tu2 - SFF * Su2
   Yu2 = SFF * Tu2 + CFF * Su2
   XuM = (Xu1+Xu2)/2.D0
   YuM = (Yu1+Yu2)/2.D0
   ZuM = (Zu1+Zu2)/2.D0
   XuD =  Xu2-Xu1
   YuD =  Yu2-Yu1
   ZuD =  Zu2-Zu1
   OuD = SQRT( XuD**2 + YuD**2 + ZuD**2 )
   IF( XuM*XuD+YuM*YuD < 0.D0 ) OuD = -OuD
   XuD = XuD / OuD
   YuD = YuD / OuD
   ZuD = ZuD / OuD
!
!This is the cheap way to do. It will work only for nominal geometry. 
   dxaxisX = -SFF
   dxaxisY =  CFF
   dxaxisZ =    0
   IF( Ic1 <= NCOUBA ) THEN
    dyaxisX =   0.d0 
    dyaxisY =   0.d0
    dyaxisZ =   1.d0
   ELSE
    dyaxisX =   CosA0 * CFF
    dyaxisY =   CosA0 * SFF
    dyaxisZ = - SinA0 * 1.d0
   ENDIF
   IF( Ic1 > NCOUBA ) THEN
    if ( TZ01 > 0. ) THEN
       dxaxisX = -dxaxisX
       dxaxisY = -dxaxisY
       dxaxisZ = -dxaxisZ
    ENDIF
   ENDIF
   dzaxisX =  dxaxisY * dyaxisZ - dxaxisZ * dyaxisY
   dzaxisY =-(dxaxisX * dyaxisZ - dxaxisZ * dyaxisX)
   dzaxisZ =  dxaxisX * dyaxisY - dxaxisY * dyaxisX
!
 END SUBROUTINE Shift_to_True_Seg
!
