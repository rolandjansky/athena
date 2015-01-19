!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE APPLY_SZT_DEFORMATION( Param,SS,ZZ,TT, ISBA, SZTin, SZTout )
!
 IMPLICIT NONE
 REAL(8), INTENT(IN)  :: Param(11), SS(4), ZZ(4), TT(4)
 INTEGER, INTENT(IN)  :: ISBA
 REAL(8), INTENT(IN)  :: SZTin(3)
 REAL(8), INTENT(OUT) :: SZTout(3)
!
!
 REAL(8) :: chfb_bz
 REAL(8) :: chfb_bp
 REAL(8) :: chfb_bn
 REAL(8) :: chfb_sp
 REAL(8) :: chfb_sn
 REAL(8) :: chfb_tw
 REAL(8) :: chfb_pg
 REAL(8) :: chfb_tr
 REAL(8) :: chfb_eg
 REAL(8) :: chfb_ep
 REAL(8) :: chfb_en
!
 REAL(8) :: S0A, S0B
 REAL(8) :: SLA, SLB
 REAL(8) :: Z0 , ZL
 REAL(8) :: T0 , TL
!
 REAL(8) ::  S,  Z,  T
 REAL(8) :: dS, dZ, dT
!--------------------------------------------------------------------
! Perform deformation 
!--------------------------------------------------------------------
!
     S = SZTin(1)
     Z = SZTin(2)
     T = SZTin(3)
!
     S0A = SS(1)
     S0B = SS(2)
     SLA = SS(3)
     SLB = SS(4)
     Z0  = ZZ(1)
     ZL  = ZZ(2)
     T0  = TT(1)
     TL  = TT(2)
!
!--  CHFB format
!
     chfb_bz = Param( 1)
     chfb_bp = Param( 2)
     chfb_bn = Param( 3)
     chfb_sp = Param( 4)
     chfb_sn = Param( 5)
     chfb_tw = Param( 6)
     chfb_pg = Param( 7)
     chfb_tr = Param( 8)
     chfb_eg = Param( 9)/1000.d0
     chfb_ep = Param(10)/1000.d0
     chfb_en = Param(11)/1000.d0

!
     call getDeformationCHFB(&
                             chfb_bz     , &
                             chfb_bp     , &
                             chfb_bn     , &
                             chfb_sp     , &
                             chfb_sn     , &
                             chfb_tw     , &
                             chfb_pg     , &
                             chfb_tr     , &
                             chfb_eg     , &
                             chfb_ep     , &
                             chfb_en     , &
                             S0A, S0B    , &
                             SLA, SLB    , &
                             Z0 , ZL     , &
                             T0 , TL     , &
                             ISBA,&
                             S,Z,T, dS,dZ,dT)
!
     SZTout(1) = S + dS 
     SZTout(2) = Z + dZ 
     SZTout(3) = T + dT
!
 END SUBROUTINE APPLY_SZT_DEFORMATION
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE getDeformationCHFB(&
                               chfb_bz     , &
                               chfb_bp     , &
                               chfb_bn     , &
                               chfb_sp     , &
                               chfb_sn     , &
                               chfb_tw     , &
                               chfb_pg     , &
                               chfb_tr     , &
                               chfb_eg     , &
                               chfb_ep     , &
                               chfb_en     , &
                               S0A, S0B    , &
                               SLA, SLB    , &
                               Z0 , ZL     , &
                               T0 , TL     , &
                               ISBA,&
                               S,Z,T, dS,dZ,dT)
!
!
 IMPLICIT NONE
 REAL(8), INTENT(IN)  :: chfb_bz
 REAL(8), INTENT(IN)  :: chfb_bp
 REAL(8), INTENT(IN)  :: chfb_bn
 REAL(8), INTENT(IN)  :: chfb_sp
 REAL(8), INTENT(IN)  :: chfb_sn
 REAL(8), INTENT(IN)  :: chfb_tw
 REAL(8), INTENT(IN)  :: chfb_pg
 REAL(8), INTENT(IN)  :: chfb_tr
 REAL(8), INTENT(IN)  :: chfb_eg
 REAL(8), INTENT(IN)  :: chfb_ep
 REAL(8), INTENT(IN)  :: chfb_en
 REAL(8), INTENT(IN)  :: S0A, S0B
 REAL(8), INTENT(IN)  :: SLA, SLB
 REAL(8), INTENT(IN)  :: Z0 , ZL
 REAL(8), INTENT(IN)  :: T0 , TL
 INTEGER, INTENT(IN)  :: ISBA
 REAL(8), INTENT(IN)  ::  S,  Z,  T
 REAL(8), INTENT(OUT) :: dS, dZ, dT
!
!
 REAL(8) :: chfb_width 
 REAL(8) :: chfb_height
 REAL(8) :: chfb_length
 REAL(8) :: chfb_s
 REAL(8) :: chfb_z
 REAL(8) :: chfb_t
!
 REAL(8) :: EZ, S0 , SL 
!--------------------------------------------------------------------
! Perform deformation 
!--------------------------------------------------------------------
!
!--  One knows that EZ  = ( Z - Z0 ) / ZL varies from -1. to +1.
!--  chfb_z should varies from 0 to chfb_length
     chfb_z      = Z - Z0 + ZL
     chfb_length = 2.d0 * ZL                         
!
!
!--  One knows that ES  = ( S - S0 ) / SL varies from  0. to +1.
!--  chfb_s should varies from -chfb_width/2.  to chfb_width/2.
     EZ  = ( Z - Z0 ) / ZL           ! --> Varies from -1. to +1.
     S0  =   S0A + EZ*S0B
     SL  =   SLA + EZ*SLB
     chfb_s      = S - ( S0 + SL/2.d0 )                                     
     chfb_width  = abs(SL)                                                 
!--  JFL: samedi 15 mai 2010, 19:26:15 (UTC+0200)
!--  SL is negative is the S origin was taken on the S Positive side
!--  in this case one should reverse  chfb_s 
     if ( SL .lt. 0.d0 ) then
       chfb_s      = -chfb_s                                   
     endif
!
!--  chfb_t should varies from 0. to chfb_height
     chfb_t      = T - T0                  
     chfb_height = TL                      
!     print *,' T0 ', T0,' TL ', TL                   
!
!--- Need system change in case of End Cap
!--  Because in End Cap <0, one should inverse the T to go to standard local szt
     if ( ISBA .eq. 0 ) then
       chfb_t = chfb_t * SIGN(1.d0,T)
     endif
!
!     print *,'= ',' chfb_s ',chfb_s, &
!             '//',' chfb_t ',chfb_t, &
!             '//',' chfb_height/2.d0 ',chfb_height/2.d0, &
!             '//',' chfb_z ',chfb_z
     call getDeformationCHFBBase(&
                             chfb_width  , &
                             chfb_height , &
                             chfb_length , &
                             chfb_bz     , &
                             chfb_bp     , &
                             chfb_bn     , &
                             chfb_sp     , &
                             chfb_sn     , &
                             chfb_tw     , &
                             chfb_pg     , &
                             chfb_tr     , &
                             chfb_eg     , &
                             chfb_ep     , &
                             chfb_en     , &
                             chfb_s,chfb_z,chfb_t, dS,dZ,dT)
!
!--  JFL: samedi 15 mai 2010, 19:26:15 (UTC+0200)
!--  SL is negative is the S origin was taken on the S Positive side
!--  in this case one should reverse the S variation
     if ( SL .lt. 0 ) then
       dS      = -dS                                   
     endif
!
!--- Need system change in case of End Cap
!--  Because in End Cap <0, one should inverse the T to go to standard local szt
     if ( ISBA .eq. 0 ) then
       dT =  dT * SIGN(1.d0,T)
     endif
!
 END SUBROUTINE getDeformationCHFB
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE getDeformationCHFBBase(&
                               chfb_width  , &
                               chfb_height , &
                               chfb_length , &
                               chfb_bz     , &
                               chfb_bp     , &
                               chfb_bn     , &
                               chfb_sp     , &
                               chfb_sn     , &
                               chfb_tw     , &
                               chfb_pg     , &
                               chfb_tr     , &
                               chfb_eg     , &
                               chfb_ep     , &
                               chfb_en     , &
                               S,Z,T, dS,dZ,dT)
!
 IMPLICIT NONE
 REAL(8), INTENT(IN)  :: chfb_width 
 REAL(8), INTENT(IN)  :: chfb_height
 REAL(8), INTENT(IN)  :: chfb_length
 REAL(8), INTENT(IN)  :: chfb_bz
 REAL(8), INTENT(IN)  :: chfb_bp
 REAL(8), INTENT(IN)  :: chfb_bn
 REAL(8), INTENT(IN)  :: chfb_sp
 REAL(8), INTENT(IN)  :: chfb_sn
 REAL(8), INTENT(IN)  :: chfb_tw
 REAL(8), INTENT(IN)  :: chfb_pg
 REAL(8), INTENT(IN)  :: chfb_tr
 REAL(8), INTENT(IN)  :: chfb_eg
 REAL(8), INTENT(IN)  :: chfb_ep
 REAL(8), INTENT(IN)  :: chfb_en
 REAL(8), INTENT(IN)  ::  S,  Z,  T
 REAL(8), INTENT(OUT) :: dS, dZ, dT
!
 REAL(8) :: Zrel 
 REAL(8) :: Zrel2M1
 REAL(8) :: Srel
!REAL(8) :: Srel2M1
 REAL(8) :: Trel
!
 REAL(8) :: PhiCor
!REAL(8) :: DltCor
!
!REAL(8) :: Srel_local
!REAL(8) :: dS_local,dT_local,dZ_local
!REAL(8) :: dS_Plus ,dT_Plus ,dZ_Plus
!REAL(8) :: dS_Minus,dT_Minus,dZ_Minus
!INTEGER :: iturn
!
!REAL(8) :: Stef_s0mdt 
 REAL(8) :: Stef_t0mdt
 REAL(8) :: Stef_z0mdt
 REAL(8) :: Stef_phiEP   
 REAL(8) :: Stef_phiEN
!REAL(8) :: Stef_deltaEP 
!REAL(8) :: Stef_deltaEN
!REAL(8) :: Stef_sEP 
 REAL(8) :: Stef_tEP
 REAL(8) :: Stef_zEP
!REAL(8) :: Stef_sEN 
 REAL(8) :: Stef_tEN
 REAL(8) :: Stef_zEN
!REAL(8) :: Stef_delta 
!REAL(8) :: Stef_sNew 
 REAL(8) :: Stef_expanded_srel 
 REAL(8) :: Stef_localT 
 REAL(8) :: Stef_localZ
 REAL(8) :: Stef_localDt 
 REAL(8) :: Stef_localDz
!     
!--------------------------------------------------------------------
! Perform deformation 
!--------------------------------------------------------------------
!
!     print *,' chfb_bz  ',chfb_bz,' chfb_bp  ',chfb_bp
!     print *,' chfb_bn  ',chfb_bn,' chfb_sp  ',chfb_sp
!     print *,' chfb_sn  ',chfb_sn,' chfb_tw  ',chfb_tw
!     print *,' chfb_pg  ',chfb_pg,' chfb_tr  ',chfb_tr
!     print *,' chfb_eg  ',chfb_eg,' chfb_ep  ',chfb_ep
!     print *,' chfb_en  ',chfb_en

!     print *,'= ',' s ',s, &
!             '//',' t ',t, &
!             '//',' z ',z

!CHFB format
     Zrel    = ( Z - chfb_length/2.d0 )/( chfb_length/2.d0 ) ! --> Varies from -1. to +1.
     Zrel2M1 = Zrel**2 - 1.D0                                ! --> Varies from  0. to -1.
!
     Srel    = S / (chfb_width/2.d0)                         ! --> Varies from -1. to +1.
!-not used-     Srel2M1 = Srel**2 - 1.d0                                ! --> Varies from  0. to -1.
!     
     Trel = ( T - chfb_height/2.d0 ) / ( chfb_height/2.d0 )  ! --> Varies from -1. to -1.
!     
     dS = 0.d0
     dZ = 0.d0
     dT = 0.d0
!     
!------!! Do not implement after SS/CA/FB/JFL meeting 01/02/11
!------!! bz
!------!     PhiCor = chfb_bz * Srel2M1
!------!     dZ = dZ + PhiCor 
!     
!------!! Do not implement after SS/CA/FB/JFL meeting 01/02/11
!------!! bp,bn
!------!     PhiCor =          ( chfb_bp + chfb_bn ) * Srel2M1 / 2.d0
!------!     PhiCor = PhiCor + ( chfb_bp - chfb_bn ) * Srel2M1 / 2.d0 * Zrel
!------!     dT = dT + PhiCor 
!     
! sp,sn
     PhiCor =          ( chfb_sp + chfb_sn ) * Zrel2M1 / 2.d0
     PhiCor = PhiCor + ( chfb_sp - chfb_sn ) * Zrel2M1 / 2.d0 * Srel
     dT = dT + PhiCor 
!     
! tw
     PhiCor = - chfb_tw * Srel
     dT = dT + PhiCor * Zrel 
     dZ = dZ - PhiCor * Trel * chfb_height / chfb_length    
!     
!------!! Do not implement after SS/CA/FB/JFL meeting 01/02/11
!------!! pg
!------!     PhiCor = - chfb_pg * Zrel
!------!     dS = dS + PhiCor 
!     
!------!! Do not implement after SS/CA/FB/JFL meeting 01/02/11
!------!! tr
!------!     PhiCor = - chfb_tr * Srel * Zrel
!------!     dS = dS + PhiCor 
!     
! eg
     PhiCor = chfb_eg
!------!! Do not implement after SS/CA/FB/JFL meeting 01/02/11
!------!      dS = dS + PhiCor * S
     dT = dT + PhiCor * T   ! -->WARNING 
     dZ = dZ + PhiCor * Z 
!     print *,'= ',' ds ',ds, &
!             '//',' dt ',dt, &
!             '//',' dz ',dz
!     
! ep,en
!! Compute end points and interpolates linearly between them (CA Ok 21/02/11)
!     do iturn=1,2
!       if (iturn.eq.1) then
!         Srel_local = 1.d0
!       else
!         Srel_local =-1.d0
!       endif
!       DltCor =          ( chfb_ep + chfb_en ) * Srel_local**3  / 6.d0
!       DltCor = DltCor + ( chfb_ep - chfb_en ) * Srel_local**2  / 4.d0
!       PhiCor =          ( chfb_ep + chfb_en ) * Srel_local**2  / 2.d0
!       PhiCor = PhiCor + ( chfb_ep - chfb_en ) * Srel_local     / 2.d0
!       dS_local =              DltCor * chfb_width  / 2.d0  
!       dT_local = PhiCor * T - PhiCor * chfb_height / 2.d0  
!       dZ_local = PhiCor * Z - PhiCor * chfb_length / 2.d0  
!       if (iturn.eq.1) then
!         dS = dS + dS_local * ( 1.d0 + Srel ) / 2.d0
!         dT = dT + dT_local * ( 1.d0 + Srel ) / 2.d0  
!         dZ = dZ + dZ_local * ( 1.d0 + Srel ) / 2.d0  
!       else
!         dS = dS + dS_local * ( 1.d0 - Srel ) / 2.d0
!         dT = dT + dT_local * ( 1.d0 - Srel ) / 2.d0  
!         dZ = dZ + dZ_local * ( 1.d0 - Srel ) / 2.d0  
!       endif
!     enddo


!!JFL Wed Jun 22 12:09:40 CEST 2011
!!Formula from Stefania 
!! Prefered because Christophe and Stefania checked that they agreed     
!!    Stef_s0mdt = S 
     !Stef_t0mdt = T
     !Stef_z0mdt = Z
     
!!AO 12 sept 2011
!!     Stef_phiEP   =  0.5d0 * ( chfb_ep + chfb_en ) &
!!                   + 0.5d0 * ( chfb_ep - chfb_en )    
!!     Stef_phiEN   =  0.5d0 * ( chfb_ep + chfb_en ) &
!!                   - 0.5d0 * ( chfb_ep - chfb_en )
     !Stef_phiEP = chfb_ep
     !Stef_phiEN = chfb_en
!!A0
     
!!AO 20 dec 2011
!!    Stef_deltaEP =  ( chfb_ep + chfb_en ) / 6.d0  &
!!         + 0.25d0 * ( chfb_ep - chfb_en )
!!    Stef_deltaEN = -( chfb_ep + chfb_en ) / 6.d0  &
!!         + 0.25d0 * ( chfb_ep - chfb_en )
!!
!!    Stef_sEP =  0.5d0 * chfb_width &
!!              + 0.5d0 * Stef_deltaEP * chfb_width
!!AO
     !Stef_tEP = Stef_t0mdt * ( 1.d0 + Stef_phiEP ) &
               !- 0.5d0 * Stef_phiEP * chfb_height
     !Stef_zEP = Stef_z0mdt * ( 1.d0 + Stef_phiEP ) &
               !- 0.5d0 * Stef_phiEP * chfb_length 
     
!!    Stef_sEN = -0.5d0 * chfb_width &
!!              + 0.5d0 * Stef_deltaEN * chfb_width 
     !Stef_tEN = Stef_t0mdt * ( 1.d0 + Stef_phiEN ) &
               !- 0.5d0 * Stef_phiEN * chfb_height
     !Stef_zEN = Stef_z0mdt * ( 1.d0 + Stef_phiEN ) &
              !- 0.5d0 * Stef_phiEN * chfb_length

!!     Stef_delta = Srel*Srel*Srel *( chfb_ep + chfb_en ) / 6.d0  &
!!                + Srel*Srel      * 0.25d0 * ( chfb_ep - chfb_en )
!!     
!!     Stef_sNew = Stef_s0mdt &
!!               + 0.5d0 * Stef_delta * chfb_width - Stef_sEN

!!AO  12 sept 2011
!!     Stef_sNew = ( Srel + 1.d0 )*( Stef_sEP - Stef_sEN )/2.d0 
!!     Stef_expanded_srel = Stef_sNew/(Stef_sEP-Stef_sEN)
     !Stef_expanded_srel = ( Srel + 1.d0 ) * 0.5d0
!!AO
     
     !Stef_localT = Stef_tEN + Stef_expanded_srel*(Stef_tEP-Stef_tEN)
     !Stef_localZ = Stef_zEN + Stef_expanded_srel*(Stef_zEP-Stef_zEN)
     
     !Stef_localDt = Stef_localT - Stef_t0mdt 
     !Stef_localDz = Stef_localZ - Stef_z0mdt
     
     !dT = dT + Stef_localDt 
     !dZ = dZ + Stef_localDz 
! PFG 2015-01-16
      PhiCor = 0.5*(chfb_ep+chfb_en)*Srel*Srel + 0.5*(chfb_ep-chfb_en)*Srel;
      dT = dT + PhiCor * (T - 0.5*chfb_height);
      dZ = dZ + PhiCor * (Z - 0.5*chfb_length);
!
 END SUBROUTINE getDeformationCHFBBase
!
