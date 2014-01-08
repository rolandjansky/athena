!
!> \author SamuSog
!
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
      SUBROUTINE AmDeProdMatAMatB(MatA,MatB,MatC)
!
      REAL(8), INTENT(IN)  :: MatA(3,3),MatB(3,3)
      REAL(8), INTENT(OUT) :: MatC(3,3)
!
      do i=1,3
       do j=1,3
        MatC(i,j) = 0.d0
        do k=1,3
         MatC(i,j) = MatC(i,j) + MatA(i,k)*MatB(k,j)
        enddo
       enddo
      enddo
!
      END SUBROUTINE AmDeProdMatAMatB
!            
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
      SUBROUTINE AmDeProMatAVecU(MatA,VectU,VectResult)
!
      REAL(8), INTENT(IN)  :: MatA(3,3)
      REAL(8), INTENT(IN)  :: VectU(3)
      REAL(8), INTENT(OUT) :: VectResult(3)
!
       do i=1,3
        VectResult(i) = 0.d0
        do k=1,3
         VectResult(i) = VectResult(i) + MatA(i,k)*VectU(k)
        enddo
       enddo
!
      END SUBROUTINE AmDeProMatAVecU
!            
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
      SUBROUTINE Amdc6Rot2To3Rot(       &
       Gama_1  , Beta_1  , Alfa_1  ,    &
       Gama_2  , Beta_2  , Alfa_2  ,    &
       Gama_out, Beta_out, Alfa_out )
      IMPLICIT NONE
      REAL(8), INTENT(IN) ::  Gama_1  , Beta_1  , Alfa_1 
      REAL(8), INTENT(IN) ::  Gama_2  , Beta_2  , Alfa_2 
      REAL(8), INTENT(OUT) :: Gama_out, Beta_out, Alfa_out
!
      REAL(8) :: RotB1(3,3)
      REAL(8) :: RotB2(3,3)
      REAL(8) :: RotB3(3,3)
      REAL(8) :: RotB4(3,3)
      REAL(8) :: RotaTOT(3,3)
      REAL(8) :: RotS(3,3) ,RotZ(3,3) ,RotT(3,3) 
      REAL(8) :: Alfa(3) 
      REAL(8) :: RotS2(3,3),RotZ2(3,3),RotT2(3,3)
      REAL(8) :: Alfa2(3)
      REAL(8) :: a13, a23, a33, a11, a12
!      
      Alfa(1) = Gama_2
      Alfa(2) = Beta_2
      Alfa(3) = Alfa_2
!
      RotS(1,1:3) = ((/  1.D0,           0.d0,           0.d0 /))
      RotS(2,1:3) = ((/  0.D0,  DCOS(Alfa(1)), -DSIN(Alfa(1)) /))
      RotS(3,1:3) = ((/  0.D0,  DSIN(Alfa(1)),  DCOS(Alfa(1)) /))
!
      RotZ(1,1:3) = ((/  DCOS(Alfa(2)),  0.d0,  DSIN(Alfa(2)) /))
      RotZ(2,1:3) = ((/           0.D0,  1.D0,           0.d0 /))
      RotZ(3,1:3) = ((/ -DSIN(Alfa(2)),  0.d0,  DCOS(Alfa(2)) /))
!
      RotT(1,1:3) = ((/  DCOS(Alfa(3)), -DSIN(Alfa(3)),  0.D0 /))
      RotT(2,1:3) = ((/  DSIN(Alfa(3)),  DCOS(Alfa(3)),  0.D0 /))
      RotT(3,1:3) = ((/           0.D0,           0.d0,  1.D0 /))
!
!
      Alfa2(1) = Gama_1 
      Alfa2(2) = Beta_1
      Alfa2(3) = Alfa_1
!
      RotS2(1,1:3) = ((/  1.D0,            0.d0,            0.d0 /))
      RotS2(2,1:3) = ((/  0.D0,  DCOS(Alfa2(1)), -DSIN(Alfa2(1)) /))
      RotS2(3,1:3) = ((/  0.D0,  DSIN(Alfa2(1)),  DCOS(Alfa2(1)) /))
!
      RotZ2(1,1:3) = ((/  DCOS(Alfa2(2)),  0.d0,  DSIN(Alfa2(2)) /))
      RotZ2(2,1:3) = ((/            0.D0,  1.D0,            0.d0 /))
      RotZ2(3,1:3) = ((/ -DSIN(Alfa2(2)),  0.d0,  DCOS(Alfa2(2)) /))
!
      RotT2(1,1:3) = ((/  DCOS(Alfa2(3)), -DSIN(Alfa2(3)),  0.D0 /))
      RotT2(2,1:3) = ((/  DSIN(Alfa2(3)),  DCOS(Alfa2(3)),  0.D0 /))
      RotT2(3,1:3) = ((/            0.D0,            0.d0,  1.D0 /))
!
!
      call AmDeProdMatAMatB(RotZ ,RotT ,RotB1)
      call AmDeProdMatAMatB(RotS ,RotB1,RotB2)
      call AmDeProdMatAMatB(RotT2,RotB2,RotB3)
      call AmDeProdMatAMatB(RotZ2,RotB3,RotB4)
      call AmDeProdMatAMatB(RotS2,RotB4,RotaTOT)
!
!
      a13 = RotaTOT(1,3)
      a23 = RotaTOT(2,3)
      a33 = RotaTOT(3,3)
      a11 = RotaTOT(1,1)
      a12 = RotaTOT(1,2)
!     
!     
      Beta_out = dasin(a13)
!
      a12 =-a12
      a23 =-a23
!
      Alfa_out = datan2(a12,a11)
      Gama_out = datan2(a23,a33)
!
      END SUBROUTINE Amdc6Rot2To3Rot
!            
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
      SUBROUTINE AmdcA2A(                             &
       JTYP,JFF,JZZ,                                  &
       Pos_Z_Ref   , Pos_R_Ref   , Pos_S_Ref,         &
       Pos_Alfa_Ref, Pos_Beta_Ref, Pos_Gama_Ref,      &
       Ts_Ref,Tz_Ref,Tt_Ref,                          &
       Rs_Ref,Rz_Ref,Rt_Ref,                          &
       Pos_Z_nRef   , Pos_R_nRef   , Pos_S_nRef,      &
       Pos_Alfa_nRef, Pos_Beta_nRef, Pos_Gama_nRef,   &
       Ts_nRef,Tz_nRef,Tt_nRef,                       &
       Rs_nRef,Rz_nRef,Rt_nRef )
!
      implicit none 
      INTEGER, INTENT(IN) :: JTYP,JFF,JZZ
      REAL(8), INTENT(IN) :: Pos_Z_Ref   ,Pos_R_Ref   ,Pos_S_Ref
      REAL(8), INTENT(IN) :: Pos_Alfa_Ref,Pos_Beta_Ref,Pos_Gama_Ref
      REAL(8), INTENT(IN) :: Ts_Ref,Tz_Ref,Tt_Ref
      REAL(8), INTENT(IN) :: Rs_Ref,Rz_Ref,Rt_Ref
      REAL(8), INTENT(IN) :: Pos_Z_nRef   ,Pos_R_nRef   ,Pos_S_nRef
      REAL(8), INTENT(IN) :: Pos_Alfa_nRef,Pos_Beta_nRef,Pos_Gama_nRef
      REAL(8), INTENT(OUT) :: Ts_nRef,Tz_nRef,Tt_nRef
      REAL(8), INTENT(OUT) :: Rs_nRef,Rz_nRef,Rt_nRef
!
      REAL(8) :: R_A_Ref(3,3)
      REAL(8) :: R_P_Ref(3,3)
      REAL(8) :: R_P_nRefI(3,3)
      REAL(8) :: R_PiP(3,3)
      REAL(8) :: R_PiPA(3,3)
      REAL(8) :: Tgl_P_Ref(3)
      REAL(8) :: T_P_Ref(3)
      REAL(8) :: Tgl_P_nRef(3)
      REAL(8) :: T_P_nRef(3)
      REAL(8) :: T_P_d(3)
      REAL(8) :: T_P_d_r(3)
      REAL(8) :: T_A_Ref(3)
      REAL(8) :: T_A_Ref_r(3)
      REAL(8) :: T_A_nRef(3)
      REAL(8) :: MatID(3,3)
!
      call AmdcGetRotMat( 0.d0, 0.d0, 0.d0 , MatID )
!
!
!Compute R_a' = R_p'^-1 * R_p * R_a
      call AmdcGetRotMat(             &
                         Rs_Ref ,     &
                         Rz_Ref ,     &
                         Rt_Ref ,     &
                         R_A_Ref )
  
      call AmdcGetRotMat(                                        &
                         (Pos_Gama_Ref * (dasin(1.d0)/90.d0)),   &
                         (Pos_Beta_Ref * (dasin(1.d0)/90.d0)),   &
                         (Pos_Alfa_Ref * (dasin(1.d0)/90.d0)),   &
                         R_P_Ref  )

      call AmdcGetRotMat(                                        &
                         (Pos_Gama_nRef * (dasin(1.d0)/90.d0)),  &
                         (Pos_Beta_nRef * (dasin(1.d0)/90.d0)),  &
                         (Pos_Alfa_nRef * (dasin(1.d0)/90.d0)),  &
                         R_P_nRefI  )
      R_P_nRefI = TRANSPOSE (R_P_nRefI)

      call AmDeProdMatAMatB(R_P_nRefI ,R_P_Ref   ,R_PiP  )      
      call AmDeProdMatAMatB(R_PiP     ,R_A_Ref   ,R_PiPA )      
!
!
!Compute R_p'^-1 * (T_p-T_p')
      Tgl_P_Ref(1:3)  = (/  Pos_Z_Ref  , Pos_R_Ref  , Pos_S_Ref  /)
      call AmdcZRS2szt( JTYP,JFF,JZZ, Tgl_P_Ref , T_P_Ref  )

      Tgl_P_nRef(1:3) = (/  Pos_Z_nRef , Pos_R_nRef , Pos_S_nRef /)
      call AmdcZRS2szt( JTYP,JFF,JZZ, Tgl_P_nRef, T_P_nRef )
      
      call AmdcGetSumVec(1.d0, T_P_Ref, -1.d0, T_P_nRef, T_P_d)
      
      call AmdcGetVecByRot( R_P_nRefI, T_P_d, T_P_d_r)
!
!     
!Compute R_p'^-1 * R_p * T_a
      T_A_Ref(1:3) = (/  Ts_Ref  , Tz_Ref  , Tt_Ref /)

      call AmdcGetVecByRot ( R_PiP, T_A_Ref, T_A_Ref_r )
!
!
!Compute T_a' = R_p'^-1 * (T_p-T_p') + R_p'^-1 * R_p * T_a
      call AmdcGetSumVec(1.d0, T_P_d_r, 1.d0 , T_A_Ref_r, T_A_nRef)
!
!     
!Get A angles from R_a' = R_p'^-1 * R_p * R_a
      call AmdcGet3AngleFromRotMat(            &
                                   Rs_nRef,    &
                                   Rz_nRef,    &
                                   Rt_nRef ,   &
                                   R_PiPA )
!
!Get A translations from T_a' = R_p'^-1 * (T_p-T_p') + R_p'^-1 * R_p * T_a
      Ts_nRef = T_A_nRef(1)
      Tz_nRef = T_A_nRef(2)
      Tt_nRef = T_A_nRef(3)
!
      END SUBROUTINE AmdcA2A
!            
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
      SUBROUTINE AmdcA2P0nlyZ(                        &
       JTYP,JFF,JZZ,                                  &
       Pos_Z_Ref   , Pos_R_Ref   , Pos_S_Ref,         &
       Pos_Alfa_Ref, Pos_Beta_Ref, Pos_Gama_Ref,      &
       Ts_Ref,Tz_Ref,Tt_Ref,                          &
       Rs_Ref,Rz_Ref,Rt_Ref,                          &
       Pos_Z_nRef   , Pos_R_nRef   , Pos_S_nRef,      &  
       Pos_Alfa_nRef, Pos_Beta_nRef, Pos_Gama_nRef )
!
      implicit none 
      INTEGER, INTENT(IN) :: JTYP,JFF,JZZ
      REAL(8), INTENT(IN) :: Pos_Z_Ref   ,Pos_R_Ref   ,Pos_S_Ref
      REAL(8), INTENT(IN) :: Pos_Alfa_Ref,Pos_Beta_Ref,Pos_Gama_Ref
      REAL(8), INTENT(IN) :: Ts_Ref,Tz_Ref,Tt_Ref
      REAL(8), INTENT(IN) :: Rs_Ref,Rz_Ref,Rt_Ref
      REAL(8), INTENT(OUT) :: Pos_Z_nRef   ,Pos_R_nRef   ,Pos_S_nRef
      REAL(8), INTENT(OUT) :: Pos_Alfa_nRef,Pos_Beta_nRef,Pos_Gama_nRef
!
#include "AmdcStand/amdcsim_com.inc"
!
      CALL AmdcA2P(                                  &
        JTYP,JFF,JZZ,                                &
       Pos_Z_Ref   , Pos_R_Ref   , Pos_S_Ref,        &
       Pos_Alfa_Ref, Pos_Beta_Ref, Pos_Gama_Ref,     &
       Ts_Ref,Tz_Ref,Tt_Ref,                         &
       Rs_Ref,Rz_Ref,Rt_Ref,                         &
       Pos_Z_nRef   , Pos_R_nRef   , Pos_S_nRef,     & 
       Pos_Alfa_nRef, Pos_Beta_nRef, Pos_Gama_nRef )
!      
! 1: Impose : T_p' = T_p + R_P T_A  and  R_P' = R_P
!      Pos_Gama_nRef = Pos_Gama_Ref
!      Pos_Beta_nRef = Pos_Beta_Ref
!      Pos_Alfa_nRef = Pos_Alfa_Ref
!      return
!
! 2: Impose : T_p' = T_p + R_P T_A  and  R_P' = 1
!      Pos_Gama_nRef = 0.d0
!      Pos_Beta_nRef = 0.d0
!      Pos_Alfa_nRef = 0.d0
!      return
!
! 3: Impose :  T_p' = T_p + R_P T_A  and R_P' = R_P R_A
!====> do nothing
!      return
!      
! 4: Impose : T_p' = T_p and  R_P' = R_P
!      Pos_Z_nRef    = Pos_Z_Ref
!      Pos_R_nRef    = Pos_R_Ref
!      Pos_S_nRef    = Pos_S_Ref
!      Pos_Gama_nRef = Pos_Gama_Ref
!      Pos_Beta_nRef = Pos_Beta_Ref
!      Pos_Alfa_nRef = Pos_Alfa_Ref
!      return
!      
! 5: Impose : T_p' = T_p and   R_P' = 1
!      Pos_Z_nRef    = Pos_Z_Ref
!      Pos_R_nRef    = Pos_R_Ref
!      Pos_S_nRef    = Pos_S_Ref
!      Pos_Gama_nRef = 0.d0
!      Pos_Beta_nRef = 0.d0
!      Pos_Alfa_nRef = 0.d0
!      return
!      
! 6: Impose : T_p' = T_p and R_P' = R_P R_A
!      Pos_Z_nRef    = Pos_Z_Ref
!      Pos_R_nRef    = Pos_R_Ref
!      Pos_S_nRef    = Pos_S_Ref
!      return
!
! 7: Impose : pick up part of the angles only
       Pos_Gama_nRef = Pos_Gama_Ref
       if( IBAREL_V(JTYP_F_Z(Jtyp,Jff,Jzz)) > 0 ) then
         Pos_Alfa_nRef = Pos_Alfa_Ref
       else
         Pos_Beta_nRef = Pos_Beta_Ref
       endif
!
      END SUBROUTINE AmdcA2P0nlyZ
!      
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
      SUBROUTINE AmdcA2P(                            &
       JTYP,JFF,JZZ,                                 &
       Pos_Z_Ref   , Pos_R_Ref   , Pos_S_Ref,        &
       Pos_Alfa_Ref, Pos_Beta_Ref, Pos_Gama_Ref,     &
       Ts_Ref,Tz_Ref,Tt_Ref,                         &
       Rs_Ref,Rz_Ref,Rt_Ref,                         &
       Pos_Z_nRef   , Pos_R_nRef   , Pos_S_nRef,     &  
       Pos_Alfa_nRef, Pos_Beta_nRef, Pos_Gama_nRef )
!
      implicit none
      INTEGER, INTENT(IN)  :: JTYP,JFF,JZZ
      REAL(8), INTENT(IN)  :: Pos_Z_Ref   ,Pos_R_Ref   ,Pos_S_Ref
      REAL(8), INTENT(IN)  :: Pos_Alfa_Ref,Pos_Beta_Ref,Pos_Gama_Ref
      REAL(8), INTENT(IN)  :: Ts_Ref,Tz_Ref,Tt_Ref
      REAL(8), INTENT(IN)  :: Rs_Ref,Rz_Ref,Rt_Ref
      REAL(8), INTENT(OUT) :: Pos_Z_nRef   ,Pos_R_nRef   ,Pos_S_nRef
      REAL(8), INTENT(OUT) :: Pos_Alfa_nRef,Pos_Beta_nRef,Pos_Gama_nRef
!     
      REAL(8) :: R_A_Ref(3,3)
      REAL(8) :: R_P_Ref(3,3)
      REAL(8) :: R_PA(3,3)
      REAL(8) :: Tgl_P_Ref(3)
      REAL(8) :: T_A_Ref(3)
      REAL(8) :: T_A_Ref_r(3)
      REAL(8) :: Tgl_A_Ref_r(3)
      REAL(8) :: Tgl_P_nRef(3)
      REAL(8) :: MatID(3,3)
!      
      call AmdcGetRotMat( 0.d0, 0.d0, 0.d0 , MatID )
!
!Compute R_p' = R_p * R_a
      call AmdcGetRotMat(           &
                         Rs_Ref ,   &
                         Rz_Ref ,   &
                         Rt_Ref ,   &
                         R_A_Ref )

      call AmdcGetRotMat(                                       &
                         (Pos_Gama_Ref * (dasin(1.d0)/90.d0)),  &
                         (Pos_Beta_Ref * (dasin(1.d0)/90.d0)),  &
                         (Pos_Alfa_Ref * (dasin(1.d0)/90.d0)),  &
                          R_P_Ref  )

      call AmDeProdMatAMatB(R_P_Ref,R_A_Ref,R_PA)      
!
!Compute T_p
      Tgl_P_Ref(1:3) = (/  Pos_Z_Ref  , Pos_R_Ref  , Pos_S_Ref /)
!     
!     
!Compute R_p * T_a
      T_A_Ref(1:3) = (/  Ts_Ref  , Tz_Ref  , Tt_Ref /)

      call AmdcGetVecByRot ( R_P_Ref, T_A_Ref, T_A_Ref_r )

      call Amdcszt2ZRS( JTYP,JFF,JZZ, T_A_Ref_r , Tgl_A_Ref_r )
!     
!     
!Compute T_p' = T_p + R_p * T_a
      call AmdcGetSumVec(1.d0,Tgl_P_Ref,1.d0,Tgl_A_Ref_r,Tgl_P_nRef)
!     
!     
!Get P angles from R_p' = R_p * R_a
      call AmdcGet3AngleFromRotMat(                &
                                   Pos_Gama_nRef,  &
                                   Pos_Beta_nRef,  &
                                   Pos_Alfa_nRef,  &
                                   R_PA )

      Pos_Alfa_nRef = Pos_Alfa_nRef/(dasin(1.d0)/90.d0)
      Pos_Beta_nRef = Pos_Beta_nRef/(dasin(1.d0)/90.d0)
      Pos_Gama_nRef = Pos_Gama_nRef/(dasin(1.d0)/90.d0)
!     
!     
!Get P translations from T_p' = T_p + R_p * T_a
      Pos_Z_nRef = Tgl_P_nRef(1)
      Pos_R_nRef = Tgl_P_nRef(2)
      Pos_S_nRef = Tgl_P_nRef(3)
!
      END SUBROUTINE AmdcA2P
!      
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
      SUBROUTINE AmdcZRS2szt(   &
       JTYP,JFF,JZZ,            &
       VZRS_in  ,               &
       Vszt_out )
!
      implicit none 
      INTEGER, INTENT(IN)  :: JTYP,JFF,JZZ
      REAL(8), INTENT(IN)  :: VZRS_in(3)
      REAL(8), INTENT(OUT) :: Vszt_out(3)
!
#include "AmdcStand/amdcsim_com.inc"
!
      if( IBAREL_V(JTYP_F_Z(Jtyp,Jff,Jzz)) > 0 ) then
        Vszt_out(1) = VZRS_in(3)
        Vszt_out(2) = VZRS_in(1)
        Vszt_out(3) = VZRS_in(2)
      else
        if ( JZZ > 0 ) then
          Vszt_out(1) = -VZRS_in(3)
          Vszt_out(2) =  VZRS_in(2)
          Vszt_out(3) =  VZRS_in(1)
        endif
        if ( JZZ < 0 ) then
          Vszt_out(1) =   VZRS_in(3)
          Vszt_out(2) =   VZRS_in(2)
          Vszt_out(3) =  -VZRS_in(1)
        endif
      endif
!
      END SUBROUTINE AmdcZRS2szt
!      
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
      SUBROUTINE Amdcszt2ZRS(  &
       JTYP,JFF,JZZ,           &
       Vszt_in ,               &
       VZRS_out )
!
      implicit none 
      INTEGER, INTENT(IN)  :: JTYP,JFF,JZZ
      REAL(8), INTENT(IN)  :: Vszt_in(3)
      REAL(8), INTENT(OUT) :: VZRS_out(3)
!
#include "AmdcStand/amdcsim_com.inc"
!
      if( IBAREL_V(JTYP_F_Z(Jtyp,Jff,Jzz)) > 0 )then
        VZRS_out(1) = Vszt_in(2) 
        VZRS_out(2) = Vszt_in(3)
        VZRS_out(3) = Vszt_in(1)
      else
        if ( JZZ > 0 ) then
          VZRS_out(1) = + Vszt_in(3) 
          VZRS_out(2) = + Vszt_in(2)
          VZRS_out(3) = - Vszt_in(1)
        endif
        if ( JZZ < 0 ) then
          VZRS_out(1) = - Vszt_in(3) 
          VZRS_out(2) = + Vszt_in(2)
          VZRS_out(3) = + Vszt_in(1)
        endif
      endif
!
      END SUBROUTINE Amdcszt2ZRS
!
