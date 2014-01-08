!
!> \author SaMuSoG
!
 SUBROUTINE Bool_Creat_Poly( Ndim_gon,Ndim_poly, Ioverf, Nseg,Vseg,  Lpol,Jpol,Vpol )
 USE M_MB_General_CONSTANTS, ONLY : EPSBOO, IproblemBOO
!
!>>
!>> Create  Lpol  polygons made of the  Jpol(0,Lpol)  points  Vpol(i,2,Lpol)
!>>                                                                  |
!>> from the  Nseg  segments  Vseg(j,2,2)                            |
!>>                                  | |                             |
!>>                                  | |                             |
!>>      beginning or end <----------/ \----------> x or y <---------/
!>>
!
 IMPLICIT NONE
 INTEGER :: Ndim_gon,Ndim_poly,Ioverf
 INTEGER :: Nseg
 REAL(8) :: Vseg(Ndim_gon,2,2)
 INTEGER :: Lpol
 INTEGER :: Jpol(0:Ndim_gon,  Ndim_poly)
 REAL(8) :: Vpol(  Ndim_gon,2,Ndim_poly)
 INTEGER :: I,J,Nt,Ipass,JDEB,JFIN,JPAS,ITRY,Iproblem,LpolNew
 INTEGER :: Iseg(Ndim_gon)
 REAL(8) :: Epsi
!
   Ipass = 0
!
10 Ipass = Ipass + 1
   IF(     Ipass == 1 ) THEN
     JDEB =    1
     JFIN = Nseg
     JPAS =    1
     Epsi = EPSBOO
   ELSEIF( Ipass == 2 ) THEN
     JDEB = Nseg
     JFIN =    1
     JPAS =   -1
     Epsi = EPSBOO
   ELSEIF( Ipass == 3 ) THEN
     JDEB =    1
     JFIN = Nseg
     JPAS =    1
     Epsi =   5.D0*EPSBOO
   ELSEIF( Ipass == 4 ) THEN
     JDEB =    1
     JFIN = Nseg
     JPAS =    1
     Epsi =  30.D0*EPSBOO
   ELSEIF( Ipass == 5 ) THEN
     JDEB =    1
     JFIN = Nseg
     JPAS =    1
     Epsi = 100.D0*EPSBOO
   ELSE
     GO TO 20
   ENDIF
!
   Lpol         = 0
   Jpol(0,1)    = 0
   Iseg(1:Nseg) = 0
   Iproblem     = 0
   DO I=1,Nseg
     IF( ABS(Vseg(I,1,1)-Vseg(I,2,1)) < Epsi/2.D0   .AND.   &
         ABS(Vseg(I,1,2)-Vseg(I,2,2)) < Epsi/2.D0 )  Iseg(I) = 1
   ENDDO
!
!---------------------------------------
   DO I=1,Nseg
     IF( Iseg(I) >= 1 ) CYCLE
     Iseg(I) = 1
     CALL Bool_Incr(Lpol,Ndim_poly,'Ndim_poly',Ioverf)
     IF( Ioverf /= 0 ) GO TO 94
     Nt = 2
     Vpol(1,1:2,Lpol) = Vseg(I,1,1:2)
     Vpol(2,1:2,Lpol) = Vseg(I,2,1:2)
!
     DO ITRY=1,1000
       DO J=JDEB,JFIN,JPAS
         IF( Iseg(J) >= 1 ) CYCLE
         IF( ABS(Vpol(Nt,1,Lpol)-Vseg(J,1,1)) < Epsi .AND.  &
             ABS(Vpol(Nt,2,Lpol)-Vseg(J,1,2)) < Epsi ) THEN
           Iseg(J) = 1
           IF( ABS(Vpol(1,1,Lpol)-Vseg(J,2,1)) < Epsi .AND.  &
               ABS(Vpol(1,2,Lpol)-Vseg(J,2,2)) < Epsi ) GO TO 100
           CALL Bool_Incr(Nt,Ndim_gon,'Ndim_gon',Ioverf)
           IF( Ioverf /= 0 ) GO TO 95
           Vpol(Nt,1:2,Lpol) = Vseg(J,2,1:2)
         ELSEIF(ABS(Vpol(Nt,1,Lpol)-Vseg(J,2,1)) < Epsi .AND.  &
                ABS(Vpol(Nt,2,Lpol)-Vseg(J,2,2)) < Epsi ) THEN
           Iseg(J) = 1
           IF(  ABS(Vpol(1,1,Lpol)-Vseg(J,1,1)) < Epsi .AND.  &
                ABS(Vpol(1,2,Lpol)-Vseg(J,1,2)) < Epsi ) GO TO 100
           CALL Bool_Incr(Nt,Ndim_gon,'Ndim_gon',Ioverf)
           IF( Ioverf /= 0 ) GO TO 95
           Vpol(Nt,1:2,Lpol) = Vseg(J,1,1:2)
         ENDIF
       ENDDO
     ENDDO
!
     Iproblem = Iproblem + 1
100  IF( Nt <= 2 ) Iproblem = Iproblem + 1000
     Jpol(  0 ,Lpol) = Nt
     Jpol(1:Nt,Lpol) = 1
!
   ENDDO
!---------------------------------------
!
   IF( Iproblem > 0 ) GO TO 10
!
20 IF( Iproblem > 0 ) THEN
     IproblemBOO = IproblemBOO + 1
     WRITE(*,'(" In Bool_Creat_Poly :  Ipass,Iproblem =",2I6)') Ipass,Iproblem
     LpolNew = 0
     DO I=1,Lpol
       IF( Jpol(0,I) < 3 ) CYCLE
       LpolNew = LpolNew + 1
       IF( I == LpolNew ) CYCLE
       Nt = Jpol(0,I)
       Jpol(  0 ,    LpolNew) = Nt
       Jpol(1:Nt,    LpolNew) = Jpol(1:Nt,I)
       Vpol(1:Nt,1:2,LpolNew) = Vpol(1:Nt,1:2,I)
     ENDDO
     Lpol = LpolNew
   ENDIF
!############################################################################
!###      IF( Iproblem > 0 .OR. Ipass > 1 ) THEN
!###        PRINT 1000,Ipass,Iproblem
!###        PRINT 7770,Nseg
!###7770    FORMAT(' Nseg',I5)
!###        DO I=1,Nseg
!###         PRINT 7771, I, Vseg(I,1,1), Vseg(I,2,1),
!###     +                  Vseg(I,1,2), Vseg(I,2,2)
!###7771     FORMAT(' Seg ',I5,F12.4,F12.4/
!###     +                 10X,F12.4,F12.4)
!###        ENDDO
!###        PRINT 7772,Lpol
!###7772    FORMAT(' Lpol',I5)
!###        DO I=1,Lpol
!###         PRINT 7773,I,Jpol(0,I)
!###7773     FORMAT(' Pol ',I5,' Npt ',I5)
!###         PRINT 7774,Vpol(1:Jpol(0,I),1,I)
!###         PRINT 7774,Vpol(1:Jpol(0,I),2,I)
!###7774     FORMAT(12F12.4)
!###        ENDDO
!###      ENDIF
!############################################################################
   RETURN
!
94 Ioverf = 4
   RETURN
!
95 Ioverf = 5
   RETURN
!
 END SUBROUTINE Bool_Creat_Poly
!
