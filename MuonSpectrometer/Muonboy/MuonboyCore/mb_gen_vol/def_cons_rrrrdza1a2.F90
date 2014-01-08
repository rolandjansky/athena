!
 SUBROUTINE DEF_CONS_RRRRDZA1A2( Name, Jcolor, RZA, NR )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
!
!>>                 1     2       3     4      5     6       7
!>>     RZA(.)  =  Rmin  Rmax    Rmin  Rmax   dZ   Phimin  Phimax
!>>               | at Zmin  |  | at Zmax  |
!
 CHARACTER(*) :: Name
 INTEGER      :: Jcolor,NR
 REAL(8)      :: RZA(*)
 REAL(8) :: RI(2),RA(2),ZZ(2),AA(2),RDZ(3)
!
   RI(1) = MIN( RZA(1) , RZA(2) )
   RA(1) = MAX( RZA(1) , RZA(2) )
   RI(2) = MIN( RZA(3) , RZA(4) )
   RA(2) = MAX( RZA(3) , RZA(4) )
   ZZ(1) = MIN( -RZA(5)/2.D0 , RZA(5)/2.D0 )
   ZZ(2) = MAX( -RZA(5)/2.D0 , RZA(5)/2.D0 )
   AA(1) = RZA(6)
   AA(2) = RZA(7)
   IF( AA(2) < AA(1) ) AA(2) = AA(2) + 360.D0
   IF( RI(1) < -Eps )                    RETURN
   IF( RI(2) < -Eps )                    RETURN
   IF( RA(1) <  Eps .AND. RA(2) <  Eps ) RETURN
   IF( ZZ(2)-ZZ(1) <  Eps )              RETURN
   IF( AA(2)-AA(1) <  Eps )              RETURN
   IF( AA(2)-AA(1) > 360.D0+Eps )        RETURN
   IF( ABS(AA(2)-AA(1)-360.D0) < Eps ) THEN
     IF(     RA(1) < Eps ) THEN
       RDZ(1) = RA(2)
       RDZ(2) = ZZ(2)
       RDZ(3) = RI(2)
       CALL DEF_CONE_RDZN( Name, Jcolor, RDZ, NR )
       RETURN
     ELSEIF( RA(2) < Eps ) THEN
       RDZ(1) = RA(1)
       RDZ(2) = ZZ(1)
       RDZ(3) = RI(1)
       CALL DEF_CONE_RDZN( Name, Jcolor, RDZ, NR )
       RETURN
     ENDIF
   ENDIF
   CALL NEWVOL(Name,1)
   CALL DEF_CENVOL(Zero,Zero,Zero)
   CALL DEFPL_2DISC_RIRAZZAA( Jcolor, RI,RA,ZZ,AA, NR )
   CALL CLOSE(Jcolor,1)
   CALL ENDVOL_SUV
!
 END SUBROUTINE DEF_CONS_RRRRDZA1A2
!
