!
!> \author SaMuSoG
!
!--------------------------------------------------------------------
!
!   The general transformation formula between local vector l_in and global vector g_out is:
!        g_out = Rp [ Ra l_in + Ta ] + Tp
!       
!   where Rp and Ra are rotation matrices defined by the P and A lines
!   and Tp and Ta are translations vector defined by the same lines
!  
!   Mboy does not use local vector but instead considers global vector before  transformation.
!   A choice of hat is a global vector before  transformation depends is to be made.
!   Mboy does the same choice for all stations but for the CSC
!  
!  
!   For all stations but CSC, the global vector g_in before the transformation is taken as
!        g_in = l_in + Tp
!     
!   So
!        g_out = Ro * g_in + To
!
!   with
!        Ro = Rp * Ra and To =  Tp + Rp * Ta - Ro * Tp
!        
!        
!   For the CSC, there is 2 points
!   1. the global vector g_in before  transformation is
!        g_in = Rg * l_in + Tp
!
!      where Rg is the rotation by the gamma angle
!     
!   2. the routine has arguments defined in a special global frame such that
!        G = Rg^-1 * g
!       
!
!   The consequences of the first point are that we have :
!       g_out = Ro * R^-1 * g_in + To + Ro * [1 -Rg^-1 ] * Tp
!   
!   and from here the consequences of the second point are that we have:
!       G_out = Rcsc * G_in  + Tcsc
!
!   where
!       Rcsc = Rg^-1 * Ro * Rg^-1 * Rg * G_in   
!   and Tcsc = Rg^-1 * [ To + Ro * [ 1 -Rg^-1 ] * Tp ]   
!
!--------------------------------------------------------------------
      SUBROUTINE GET_STATION_CSC_ABG_SZT_TRANSFORM( Jtyp,Jff,Jzz,Job, Iok, TransformDir,TransformBak  )
      IMPLICIT NONE
      INTEGER, INTENT(IN)  :: Jtyp,Jff,Jzz,Job
      INTEGER, INTENT(OUT) :: Iok
      REAL(8), INTENT(OUT) :: TransformDir(3,4),TransformBak(3,4)
      REAL(8) :: Gama0 
      REAL(8) :: Caa,Saa 
      REAL(8) :: RotTransformDir (3,3)
      REAL(8) :: RotTransformBak (3,3)
      REAL(8) :: SZTCSC(3),SZTCSC_B(3)
      REAL(8) :: RotCSC(3,3)
      REAL(8) :: New_RotTransformDir (3,3)
      REAL(8) :: New_RotTransformBak (3,3)
      REAL(8) :: Trans (3)
!
#include "AmdcStand/copipi.inc"
!
#include "AmdcStand/amdcsim_com.inc"
!
!-- Statement functions
#include "AmdcStand/amdcsim_stfunc.inc"
!
      CALL GET_STATION_ABG_SZT_TRANSFORM( Jtyp,Jff,Jzz,Job, Iok, TransformDir,TransformBak )
      IF( Iok < 1 ) RETURN
!
      IF( ICSCCH(Jtyp,Jff,Jzz) /= 1 ) RETURN
!      
      Gama0 = Pos_Gama(Jtyp,Jff,Jzz) * PIS180
      IF( Jzz < 0 ) Gama0 = -Gama0
      Caa = COS(Gama0)
      Saa = SIN(Gama0)
!
      SZTCSC(1) = Pos_S(Jtyp,Jff,Jzz) 
      SZTCSC(2) = Pos_R(JTYP,JFF,JZZ) 
      SZTCSC(3) = Pos_Z(JTYP,JFF,JZZ)
!
!========
      RotCSC( 1 ,1:3) = (/ 1.D0, 0.D0, 0.D0 /)
      RotCSC( 2 ,1:3) = (/ 0.D0,  Caa,  Saa /)
      RotCSC( 3 ,1:3) = (/ 0.D0, -Saa,  Caa /)
!
      SZTCSC_B(1:3) = SZTCSC(1:3)               &  
                    - RotCSC(1:3,1)*SZTCSC(1)   &
                    - RotCSC(1:3,2)*SZTCSC(2)   &
                    - RotCSC(1:3,3)*SZTCSC(3)
      TransformDir(1:3, 4 ) = TransformDir(1:3, 4 )           &
                          + TransformDir(1:3,1)*SZTCSC_B(1)   &
                          + TransformDir(1:3,2)*SZTCSC_B(2)   &
                          + TransformDir(1:3,3)*SZTCSC_B(3)
!
      RotTransformDir(1:3,1:3) = TransformDir(1:3,1:3)
      CALL MULT3M3M( RotCSC, RotTransformDir, New_RotTransformDir )
      TransformDir(1:3,1:3) = New_RotTransformDir(1:3,1:3)
!
!========
      RotCSC( 1 ,1:3) = (/ 1.D0, 0.D0, 0.D0 /)
      RotCSC( 2 ,1:3) = (/ 0.D0, -Saa,  Caa /)
      RotCSC( 3 ,1:3) = (/ 0.D0,  Caa,  Saa /)
!
      RotTransformDir(1:3,1:3) = TransformDir(1:3,1:3)
      CALL MULT3M3M( RotCSC, RotTransformDir, New_RotTransformDir )
      TransformDir(1:3,1:3) = New_RotTransformDir(1:3,1:3)
!
      RotTransformDir(1:3,1:3) = TransformDir(1:3,1:3)
      CALL MULT3M3M(RotTransformDir, RotCSC , New_RotTransformDir )
      TransformDir(1:3,1:3) = New_RotTransformDir(1:3,1:3)
!
      Trans(1:3) = TransformDir(1:3, 4 )
      TransformDir(1:3, 4 ) = RotCSC(1:3,1)*Trans(1)  &
                            + RotCSC(1:3,2)*Trans(2)  &
                            + RotCSC(1:3,3)*Trans(3)
!
!========
      RotCSC( 1 ,1:3) = (/ 1.D0, 0.D0, 0.D0 /)
      RotCSC( 2 ,1:3) = (/ 0.D0, 0.D0, 1.D0 /)
      RotCSC( 3 ,1:3) = (/ 0.D0, 1.D0, 0.D0 /)
!
      RotTransformDir(1:3,1:3) = TransformDir(1:3,1:3)
      CALL MULT3M3M( RotCSC, RotTransformDir, New_RotTransformDir )
      TransformDir(1:3,1:3) = New_RotTransformDir(1:3,1:3)
!
      RotTransformDir(1:3,1:3) = TransformDir(1:3,1:3)
      CALL MULT3M3M(RotTransformDir, RotCSC , New_RotTransformDir )
      TransformDir(1:3,1:3) = New_RotTransformDir(1:3,1:3)
!
      Trans(1:3) = TransformDir(1:3, 4 )
      TransformDir(1:3, 4 ) = RotCSC(1:3,1)*Trans(1)  &
                            + RotCSC(1:3,2)*Trans(2)  &
                            + RotCSC(1:3,3)*Trans(3)
!
!========
      RotCSC( 1 ,1:3) = (/ 1.D0, 0.D0, 0.D0 /)
      RotCSC( 2 ,1:3) = (/ 0.D0,  Caa, -Saa /)
      RotCSC( 3 ,1:3) = (/ 0.D0,  Saa,  Caa /)
!
      SZTCSC_B(1:3) = SZTCSC(1:3)               &  
                    - RotCSC(1:3,1)*SZTCSC(1)   &
                    - RotCSC(1:3,2)*SZTCSC(2)   &
                    - RotCSC(1:3,3)*SZTCSC(3)
      Trans(1:3) = TransformBak(1:3, 4 )
      TransformBak(1:3, 4 ) = SZTCSC_B(1:3)           &
                            + RotCSC(1:3,1)*Trans(1)  &
                            + RotCSC(1:3,2)*Trans(2)  &
                            + RotCSC(1:3,3)*Trans(3)
!
      RotTransformBak(1:3,1:3) = TransformBak(1:3,1:3)
      CALL MULT3M3M( RotTransformBak, RotCSC, New_RotTransformBak )
      TransformBak(1:3,1:3) = New_RotTransformBak(1:3,1:3)
!
!========
      RotCSC( 1 ,1:3) = (/ 1.D0, 0.D0, 0.D0 /)
      RotCSC( 2 ,1:3) = (/ 0.D0, -Saa,  Caa /)
      RotCSC( 3 ,1:3) = (/ 0.D0,  Caa,  Saa /)
!
      RotTransformBak(1:3,1:3) = TransformBak(1:3,1:3)
      CALL MULT3M3M( RotCSC, RotTransformBak, New_RotTransformBak )
      TransformBak(1:3,1:3) = New_RotTransformBak(1:3,1:3)
! 
      RotTransformBak(1:3,1:3) = TransformBak(1:3,1:3)
      CALL MULT3M3M(RotTransformBak, RotCSC , New_RotTransformBak )
      TransformBak(1:3,1:3) = New_RotTransformBak(1:3,1:3)
!
      Trans(1:3) = TransformBak(1:3, 4 )
      TransformBak(1:3, 4 ) = RotCSC(1:3,1)*Trans(1)   &
                            + RotCSC(1:3,2)*Trans(2)   &
                            + RotCSC(1:3,3)*Trans(3)
!
!========
      RotCSC( 1 ,1:3) = (/ 1.D0, 0.D0, 0.D0 /)
      RotCSC( 2 ,1:3) = (/ 0.D0, 0.D0, 1.D0 /)
      RotCSC( 3 ,1:3) = (/ 0.D0, 1.D0, 0.D0 /)
!
      RotTransformBak(1:3,1:3) = TransformBak(1:3,1:3)
      CALL MULT3M3M( RotCSC, RotTransformBak, New_RotTransformBak )
      TransformBak(1:3,1:3) = New_RotTransformBak(1:3,1:3)
!
      RotTransformBak(1:3,1:3) = TransformBak(1:3,1:3)
      CALL MULT3M3M(RotTransformBak, RotCSC , New_RotTransformBak )
      TransformBak(1:3,1:3) = New_RotTransformBak(1:3,1:3)
!
      Trans(1:3) = TransformBak(1:3, 4 )
      TransformBak(1:3, 4 ) = RotCSC(1:3,1)*Trans(1)   &
                            + RotCSC(1:3,2)*Trans(2)   &
                            + RotCSC(1:3,3)*Trans(3)
!
      END SUBROUTINE GET_STATION_CSC_ABG_SZT_TRANSFORM
!
