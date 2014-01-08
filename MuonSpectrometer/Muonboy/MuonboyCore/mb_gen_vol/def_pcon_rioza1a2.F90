!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE DEF_PCON_RIOZA1A2( Name, Jcolor, NRZA, RZA, NR )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
!
!>>           1     2    3     4     5    6        3*n-2 3*n-1 3*n  3*n+1  3*n+2
!>> RZA(.) = Ri(1) Ro(1) Z(1) Ri(2) Ro(2) Z(2) ... Ri(n) Ro(n) Z(n) PhiMin PhiMax
!
 CHARACTER (len=*) :: Name
 INTEGER           :: Jcolor,NRZA,NR
 REAL(8)           :: RZA(*)
 INTEGER :: Nsection,N,Iop1,Iop2
 REAL(8) :: RI(2),RA(2),ZZ(2),AA(2)
!
   AA(1) = RZA(NRZA-1)
   AA(2) = RZA(NRZA)
   Nsection = (NRZA-5) / 3
   IF( RZA(2) < Eps .AND. RZA(5) < Eps ) RETURN
   IF( ABS(RZA(3)-RZA(6)) < Eps )        RETURN
   IF( ABS(AA(2)-AA(1)) <  Eps )         RETURN
   IF( AA(2) < AA(1) ) AA(2) = AA(2) + 360.D0
   IF( Nsection < 1 )                    RETURN
!
   CALL NEWVOL(Name,1)
   CALL DEF_CENVOL(Zero,Zero,Zero)
   DO N=1,Nsection
     RI(1) = RZA(3*N-2)
     RA(1) = RZA(3*N-1)
     ZZ(1) = RZA(3*N  )
     RI(2) = RZA(3*N+1)
     RA(2) = RZA(3*N+2)
     ZZ(2) = RZA(3*N+3)
     CALL DEFPL_2DISC_RIRAZZAA( Jcolor, RI,RA,ZZ,AA, NR )
     Iop1 = 0
     Iop2 = 0
     IF( N == 1        ) Iop1 = 1
     IF( N == Nsection ) Iop2 = 1
     CALL CLOSE_OPEN(Jcolor, Iop1,Iop2 ,1)
   ENDDO
   CALL ENDVOL_SUV
!
 END
