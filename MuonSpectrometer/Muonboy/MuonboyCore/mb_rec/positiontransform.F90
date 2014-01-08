!
!> give position in local from global SZT frame
!
 SUBROUTINE Pos_Ideal_from_True(Itra, Strue,Ztrue,Ttrue, Sidea,Zidea,Tidea)
 USE M_MB_TransGeo
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Itra
 REAL(8), INTENT(IN)  :: Strue, Ztrue, Ttrue
 REAL(8), INTENT(OUT) :: Sidea, Zidea, Tidea
 REAL(8) ::  S, Z, T 
 REAL(8) :: dS,dZ,dT
!
   IF( Itra >= 1 .AND. Itra <= NBtransGeo ) THEN
     S = TransGeoBak(1,1,Itra)*Strue + TransGeoBak(1,2,Itra)*Ztrue &
       + TransGeoBak(1,3,Itra)*Ttrue + TransGeoBak(1,4,Itra)
     Z = TransGeoBak(2,1,Itra)*Strue + TransGeoBak(2,2,Itra)*Ztrue &
       + TransGeoBak(2,3,Itra)*Ttrue + TransGeoBak(2,4,Itra)
     T = TransGeoBak(3,1,Itra)*Strue + TransGeoBak(3,2,Itra)*Ztrue &
       + TransGeoBak(3,3,Itra)*Ttrue + TransGeoBak(3,4,Itra)
     CALL getDeformation( Itra, S,Z,T, dS,dZ,dT )
     Sidea = S - dS
     Zidea = Z - dZ
     Tidea = T - dT
   ELSE
     Sidea = Strue
     Zidea = Ztrue
     Tidea = Ttrue
   ENDIF
!
 END SUBROUTINE Pos_Ideal_from_True
!
!
!> give position in global SZT frame from local 
!
 SUBROUTINE Pos_True_from_Ideal(Itra, Sidea,Zidea,Tidea, Strue,Ztrue,Ttrue)
 USE M_MB_TransGeo
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: ITRA
 REAL(8), INTENT(IN)  :: Sidea, Zidea, Tidea
 REAL(8), INTENT(OUT) :: Strue, Ztrue, Ttrue
 REAL(8) ::  S, Z, T 
 REAL(8) :: dS,dZ,dT
!
   IF( Itra >= 1 .AND. Itra <= NBtransGeo ) THEN
     CALL getDeformation( Itra, Sidea,Zidea,Tidea, dS,dZ,dT )
     S   = Sidea + dS
     Z   = Zidea + dZ
     T   = Tidea + dT
     Strue = TransGeoDir(1,1,Itra)*S + TransGeoDir(1,2,Itra)*Z &
           + TransGeoDir(1,3,Itra)*T + TransGeoDir(1,4,Itra)
     Ztrue = TransGeoDir(2,1,Itra)*S + TransGeoDir(2,2,Itra)*Z &
           + TransGeoDir(2,3,Itra)*T + TransGeoDir(2,4,Itra)
     Ttrue = TransGeoDir(3,1,Itra)*S + TransGeoDir(3,2,Itra)*Z &
           + TransGeoDir(3,3,Itra)*T + TransGeoDir(3,4,Itra)
   ELSE
     Strue = Sidea
     Ztrue = Zidea
     Ttrue = Tidea
   ENDIF
!
 END SUBROUTINE Pos_True_from_Ideal
! 
!
!> give S-Z-T in layer coordinate from S-Z-T in station coordinate for CSC
!
 SUBROUTINE Pos_Layer_from_Csc(Jcsc,Jlay, Scsc,Zcsc,Tcsc, Slayer,Zlayer,Tlayer)
 USE M_MB_Mugeom
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Jcsc, Jlay
 REAL(8), INTENT(IN)  :: Scsc,   Zcsc,   Tcsc
 REAL(8), INTENT(OUT) :: Slayer, Zlayer, Tlayer
!
   IF( Jcsc >= 1 ) THEN
     Slayer = transCSC(1,Jlay,Jcsc)*(Scsc-transCSC(3,Jlay,Jcsc)) - transCSC(2,Jlay,Jcsc)*(Zcsc-transCSC(4,Jlay,Jcsc))
     Zlayer = transCSC(2,Jlay,Jcsc)*(Scsc-transCSC(3,Jlay,Jcsc)) + transCSC(1,Jlay,Jcsc)*(Zcsc-transCSC(4,Jlay,Jcsc))
   ELSE
     Slayer = Scsc
     Zlayer = Zcsc
   ENDIF
   Tlayer = Tcsc
!
 END SUBROUTINE Pos_Layer_from_Csc
!
!
!> give S-Z-T in station coordinate from S-Z-T in layer coordinate for CSC
!
 SUBROUTINE Pos_Csc_from_Layer(Jcsc,Jlay, Slayer,Zlayer,Tlayer, Scsc,Zcsc,Tcsc)
 USE M_MB_Mugeom
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Jcsc, Jlay
 REAL(8), INTENT(IN)  :: Slayer, Zlayer, Tlayer
 REAL(8), INTENT(OUT) :: Scsc,   Zcsc,   Tcsc
!
   IF( Jcsc >= 1 ) THEN
     Scsc =   transCSC(1,Jlay,Jcsc)*Slayer + transCSC(2,Jlay,Jcsc)*Zlayer + transCSC(3,Jlay,Jcsc)
     Zcsc = - transCSC(2,Jlay,Jcsc)*Slayer + transCSC(1,Jlay,Jcsc)*Zlayer + transCSC(4,Jlay,Jcsc)
   ELSE
     Scsc = Slayer
     Zcsc = Zlayer
   ENDIF
   Tcsc = Tlayer
!
 END SUBROUTINE Pos_Csc_from_Layer
!
!
!> give position and direction in local from global SZT frame
!
 SUBROUTINE SZTWU_Ideal_from_True(Itra, SStrue,ZZtrue,TTtrue,VStrue,VZtrue,VTtrue, &
                                        SSidea,ZZidea,TTidea,WZidea,WTidea,UZTidea,USTidea )
 USE M_MB_TransGeo
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Itra
 REAL(8), INTENT(IN)  :: SStrue, ZZtrue, TTtrue
 REAL(8), INTENT(IN)  :: VStrue, VZtrue, VTtrue
 REAL(8), INTENT(OUT) :: SSidea, ZZidea, TTidea
 REAL(8), INTENT(OUT) :: WZidea, WTidea, UZTidea, USTidea
 REAL(8) ::   S,   Z,   T 
 REAL(8) ::  VS,  VZ,  VT
 REAL(8) ::  dS,  dZ,  dT  
 REAL(8) :: ddS, ddZ, ddT 
 REAL(8) :: AAZT, VSidea, VZidea, VTidea
!
   IF( Itra >= 1 .AND. Itra <= NBtransGeo ) THEN
     S  = TransGeoBak(1,1,Itra)*SStrue + TransGeoBak(1,2,Itra)*ZZtrue  &
        + TransGeoBak(1,3,Itra)*TTtrue + TransGeoBak(1,4,Itra)
     Z  = TransGeoBak(2,1,Itra)*SStrue + TransGeoBak(2,2,Itra)*ZZtrue  &
        + TransGeoBak(2,3,Itra)*TTtrue + TransGeoBak(2,4,Itra)
     T  = TransGeoBak(3,1,Itra)*SStrue + TransGeoBak(3,2,Itra)*ZZtrue  &
        + TransGeoBak(3,3,Itra)*TTtrue + TransGeoBak(3,4,Itra)
     VS = TransGeoBak(1,1,Itra)*VStrue + TransGeoBak(1,2,Itra)*VZtrue &
        + TransGeoBak(1,3,Itra)*VTtrue
     VZ = TransGeoBak(2,1,Itra)*VStrue + TransGeoBak(2,2,Itra)*VZtrue &
        + TransGeoBak(2,3,Itra)*VTtrue
     VT = TransGeoBak(3,1,Itra)*VStrue + TransGeoBak(3,2,Itra)*VZtrue &
        + TransGeoBak(3,3,Itra)*VTtrue
     CALL getDeformation( Itra, S   ,Z   ,T   ,  dS, dZ, dT )
     CALL getDeformation( Itra, S+VS,Z+VZ,T+VT, ddS,ddZ,ddT )
     SSidea = S - dS
     ZZidea = Z - dZ
     TTidea = T - dT
     VSidea = VS - ( ddS - dS )
     VZidea = VZ - ( ddZ - dZ )
     VTidea = VT - ( ddT - dT )
     AAZT    = SQRT( VZidea**2 + VTidea**2 )
     WZidea  = VZidea / AAZT
     WTidea  = VTidea / AAZT
     UZTidea = VZidea / VTidea
     USTidea = VSidea / VTidea
   ELSE
     SSidea  = SStrue
     ZZidea  = ZZtrue
     TTidea  = TTtrue
     AAZT    = SQRT( VZtrue**2 + VTtrue**2 )
     WZidea  = VZtrue / AAZT
     WTidea  = VTtrue / AAZT
     UZTidea = VZtrue / VTtrue
     USTidea = VStrue / VTtrue
   ENDIF
!
 END SUBROUTINE SZTWU_Ideal_from_True
!
