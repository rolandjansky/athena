!
!> Input :
!>   Nclus    = number of points
!>   Tclus(i) = abscissa of the ith point
!>   Jclus(i) = when > 0 : weight of the ith point (number of initial points forming the ith point)
!>              when < 0 : - address of the point to which the ith point has been clustered
!>
!> Output :
!>   Same thing after "clusterisation" of size Dist
!>
!> \author Saclay Muon Software Group (SaMuSoG)
!
 SUBROUTINE Mubo_Cluster( Dist,Nclus, Tclus,Jclus )
 IMPLICIT NONE
!
 INTEGER, INTENT(IN)    :: Nclus
 REAL(8), INTENT(IN)    :: Dist
 REAL(8), INTENT(INOUT) :: Tclus(*)
 INTEGER, INTENT(INOUT) :: Jclus(*)
!
 INTEGER :: I,I1,I2,I1min,I2min,Jsum
 REAL(8) :: DD,DDmin
!
100 DDmin = Dist
    I1min = 0
    DO I1=1,Nclus
      IF( Jclus(I1) < 1 )   CYCLE
      DO I2=I1+1,Nclus
        IF( Jclus(I2) < 1 ) CYCLE
        DD = ABS( Tclus(I1) - Tclus(I2) )
        IF( DD >= DDmin )   CYCLE
        DDmin = DD
        I1min = I1
        I2min = I2
      ENDDO
    ENDDO
!
    IF( I1min > 0 ) THEN
      Jsum = Jclus(I1min) + Jclus(I2min)
      Tclus(I1min) = ( Jclus(I1min)*Tclus(I1min) + Jclus(I2min)*Tclus(I2min) ) / Jsum
      Jclus(I1min) =   Jsum
      Jclus(I2min) = - I1min
      GO TO 100
    ENDIF
!
    DO I=1,Nclus
      IF( Jclus(I) > 0 ) CYCLE
      I1 = I
200   I1 = - Jclus(I1)
      IF( Jclus(I1) < 0 ) GO TO 200
      Jclus(I) = - I1
    ENDDO
!
 END SUBROUTINE Mubo_Cluster
!
