!
!> \author SaMuSoG
!
 SUBROUTINE Bool_Ort_Nor_Sys( Ndim, Nedge,Ptxyz, Sys)
 IMPLICIT  NONE
 INTEGER, INTENT(IN)  :: Ndim, Nedge
 REAL(8), INTENT(IN)  :: Ptxyz(3,Ndim)
 REAL(8), INTENT(OUT) :: Sys(3,0:3)
 INTEGER :: I1, I2, I3
 REAL(8) :: AV3, AV3max
 REAL(8) :: V1(3), V2(3), V3(3)
 REAL(8) :: ANOR3V
!
   AV3max = 0.D0
   DO I1=1,Nedge
     I2 = MOD(I1,Nedge) + 1
     I3 = MOD(I2,Nedge) + 1
     V1(1:3) = Ptxyz(1:3,I1) - Ptxyz(1:3,I2)
     V2(1:3) = Ptxyz(1:3,I3) - Ptxyz(1:3,I2)
     CALL PVEC3V( V1,V2, V3 )
     AV3 = ANOR3V( V3 ) 
     IF( AV3 <= AV3max ) CYCLE
     AV3max = AV3
     Sys(1:3,0) = Ptxyz(1:3,I2)
     Sys(1:3,1) =    V1(1:3)
     Sys(1:3,3) =    V3(1:3)
   ENDDO
!
   CALL NORM3V( Sys(1,1) )
   CALL NORM3V( Sys(1,3) )
   CALL PVEC3V( Sys(1,3),Sys(1,1), Sys(1,2) )
!
 END SUBROUTINE Bool_Ort_Nor_Sys
!
