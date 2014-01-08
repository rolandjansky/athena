!
 SUBROUTINE checkTRABAZ
 USE M_MB_General_CONSTANTS, ONLY : Pi, TwoPi
 USE M_MB_CTRABAZ
 USE M_MB_Fit
 use m_mb_precis
 IMPLICIT NONE
 INTEGER, PARAMETER :: NP = 100, NT = 100, NM = 8, N1 = 2, N2 = 2, N3 = 2, N4=2
 INTEGER :: IP, IT, IM, IB
 REAL(8) :: PHI, COSPHI, SINPHI, TET, TANTET, COSTET, SINTET, ETA, PHIFIN
 REAL(8) :: VINI(8), VPOS(3), VDIR(3), VDIF(3)
 REAL(8) :: ecart, ecartmax(50), phimax(50), tetmax(50), etamax(50), mommax(50)
 REAL(8) :: MOM(NM) = (/ 2.d0, 5.d0, 10.d0, 20.d0, 50.d0, 100.d0, 300.d0, 800.d0 /)
 REAL(8) :: BNU(0:N1) = (/0.00005d0, 0.00050d0, 0.00100d0 /)
 REAL(8) :: BMI(0:N2) = (/0.01000d0, 0.00500d0, 0.02000d0 /)
 REAL(8) :: DBA(0:N3) = (/0.30d0, 0.35d0, 0.25d0 /)
 REAL(8) :: DBI(0:N4) = (/0.12d0, 0.10d0, 0.15d0 /)
 CHARACTER(21) :: filename
 INTEGER, SAVE :: IPASS = 0
!
!>> Set common CTRABAZ
   NZTPLA = 9
   ICCPLA(1:NZTPLA) = (/     -1,     -1,      -1,        1,       1,       1,       1,        1,        1 /)
   TT0PLA(1:NZTPLA) = (/ 500.d0, 750.d0, 1000.d0,   700.d0, 1400.d0, 2100.d0, -700.d0, -1400.d0, -2100.d0 /)
   CA0PLA(1:NZTPLA) = 1.d0
   SA0PLA(1:NZTPLA) = 0.d0
!
   RCHAMI = RCYLIN(3)
   ZCHAMI = ZCYLIN(3)
   RCHAMA = TT0PLA(3) + 10.d0
   ZCHAMA = TT0PLA(6) + 10.d0
!
   ecartmax(:) = -1.d0
!!$   IPASS = IPASS + 1
!!$   WRITE(filename,'(A18,I3.3)') 'pointSortieSpectro',IPASS
!!$   OPEN( unit=41, file=filename )
!!$   write(41,*) 'mom0/F:phi0/F:teta0/F:eta0/F:xfinal/F:yfinal/F:zfinal/F'
!!$   WRITE(filename,'(A18,I3.3)') 'ecartEntreeSpectro',IPASS
!!$   OPEN( unit=42, file=filename )
!!$   write(42,*) 'mom0/F:phi0/F:teta0/F:eta0/F:ecart0/F:phiFinal/F'
   DO IP=0,NP
     PHI = IP*Pi/NP - Pi/2.d0
     COSPHI = COS(PHI)
     SINPHI = SIN(PHI)
     CF0PLA(1:NZTPLA) = COSPHI
     SF0PLA(1:NZTPLA) = SINPHI
     DO IT = 0, NT
       ETA    = IT*5.4d0/NT - 2.7d0
       TANTET = 1.d0 / SINH(ETA)
       TET    = ATAN(TANTET)
       IF( TET < 0.d0 ) TET = TET + Pi
       COSTET = COS(TET)
       SINTET = SIN(TET)
       IF( ABS(RCHAMI/TANTET) < ZCHAMI ) THEN
         VINI(1) = RCHAMI * COSPHI
         VINI(2) = RCHAMI * SINPHI
         VINI(3) = RCHAMI / TANTET
       ELSE
         VINI(1) = ZCHAMI * ABS(TANTET) * COSPHI
         VINI(2) = ZCHAMI * ABS(TANTET) * SINPHI
         VINI(3) = ZCHAMI * SIGN(1.d0, TANTET)
       ENDIF
       VINI(4) = SINTET * COSPHI
       VINI(5) = SINTET * SINPHI
       VINI(6) = COSTET
       VINI(8) = 1.d0
!
       DO IM=1,NM
         VINI(7) = MOM(IM)
         VDEB(1:8,1) = VINI(1:8)
!
         bbnul = BNU(0)
         bbmin = BMI(0)
         dbmax = DBA(0)
         dbmin = DBI(0)
         CALL TRABAZ(1,1,0)  !>> Track and catch the crossings away from the vertex
!!$         write(41,*) mom(im),phi,tet,eta,vfin(1:3)
!!$         PHIFIN = ATAN2( VFIN(2), VFIN(1) )
!!$         VDEB(1:3,1) =   VFIN(1:3)
!!$         VDEB(4:6,1) = - VFIN(4:6)
!!$         VDEB( 7 ,1) =   VFIN(7)
!!$         VDEB( 8 ,1) = - VFIN(8)
!!$         RCHAMA = TT0PLA(3) + 200.d0
!!$         ZCHAMA = TT0PLA(6) + 200.d0
         vpos(1:3) = VFIN(1:3)
         vdir(1:3) = VFIN(4:6) / sqrt(sum(VFIN(4:6)**2))
         DO IB=1,N1+N2+N3+N4+1
           bbnul = BNU(0)
           bbmin = BMI(0)
           dbmax = DBA(0)
           dbmin = DBI(0)
           IF( IB <= N1 ) THEN
             bbnul = BNU(IB)
           ELSE IF( IB <= N1 + N2 ) THEN
             bbmin = BMI(IB-N1)
           ELSE IF( IB <= N1 + N2 + N3 ) THEN
             dbmax = DBA(IB-N1-N2)
           ELSE IF( IB <= N1 + N2 + N3 + N4 ) THEN
             dbmin = DBI(IB-N1-N2-N3)
           ENDIF
           CALL TRABAZ(1,1,0)  !>> Track and catch the crossings towards the vertex
           vdif(1:3) = vfin(1:3) - vpos(1:3)
           ecart = sqrt( sum(vdif(1:3)**2) - (sum(vdif(1:3)*vdir(1:3)))**2 )
!!$           write(42,*) mom(im),phi,tet,eta,ecart,phifin
           IF( ecart > ecartmax(ib) ) THEN
             ecartmax(ib) = ecart
             phimax(ib) = PHI
             tetmax(ib) = TET
             etamax(ib) = ETA
             mommax(ib) = MOM(IM)
           ENDIF
         ENDDO
       ENDDO
     ENDDO
   ENDDO
!
!!$   CLOSE(41)
!!$   CLOSE(42)
!
   WRITE(filename,'(A8,I3.3)') 'ecartMax',IPASS
   OPEN( unit=41, file=filename )
   DO IB=1,N1
     phimax(ib) = mod( phimax(ib)+pi , pi/4.D0 ) * 180.d0 / pi
     WRITE(41,*) ' pour bbnul = ',bnu(ib),' on a ecartmax ',ecartmax(ib),' lorsque phi teta eta mom '  &
                                 ,phimax(ib),tetmax(ib),etamax(ib),mommax(ib)
   ENDDO
   WRITE(41,*)
   DO IB=N1+1,N1+N2
     phimax(ib) = mod( phimax(ib)+pi , pi/4.D0 ) * 180.d0 / pi
     WRITE(41,*) ' pour bbmin = ',bmi(ib-N1),' on a ecartmax ',ecartmax(ib),' lorsque phi teta eta mom '  &
                                 ,phimax(ib),tetmax(ib),etamax(ib),mommax(ib)
   ENDDO
   WRITE(41,*)
   DO IB=N1+N2+1,N1+N2+N3
     phimax(ib) = mod( phimax(ib)+pi , pi/4.D0 ) * 180.d0 / pi
     WRITE(41,*) ' pour dbmax = ',dba(ib-N1-N2),' on a ecartmax ',ecartmax(ib),' lorsque phi teta eta mom '  &
                                 ,phimax(ib),tetmax(ib),etamax(ib),mommax(ib)
   ENDDO
   WRITE(41,*)
   DO IB=N1+N2+N3+1,N1+N2+N3+N4
     phimax(ib) = mod( phimax(ib)+pi , pi/4.D0 ) * 180.d0 / pi
     WRITE(41,*) ' pour dbmin = ',dbi(ib-N1-N2-N3),' on a ecartmax ',ecartmax(ib),' lorsque phi teta eta mom '  &
                                 ,phimax(ib),tetmax(ib),etamax(ib),mommax(ib)
   ENDDO
   WRITE(41,*)
   IB=N1+N2+N3+N4+1
   phimax(ib) = mod( phimax(ib)+pi , pi/4.D0 ) * 180.d0 / pi
   WRITE(41,*) '      verif     on a ecartmax ',ecartmax(ib),' lorsque phi teta eta mom '  &
                               ,phimax(ib),tetmax(ib),etamax(ib),mommax(ib)
   CLOSE(41)
!
 END SUBROUTINE checkTRABAZ
!
