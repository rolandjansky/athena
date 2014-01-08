!
!> Interface with the toms611 package
!
 SUBROUTINE minimizeFcn(n,maxcall,err)
 USE M_MB_Control
 USE M_MB_Fit
 USE M_MB_COMUSC
 IMPLICIT NONE
 INTEGER, PARAMETER :: liv=60, lv=220, npmax=30
 INTEGER, INTENT(IN)    :: n, maxcall
 REAL(8), INTENT(IN)    :: err(n)
 INTEGER, SAVE :: iv(liv)
 REAL(8), SAVE :: v(lv)
 INTEGER :: np, i, uip(1)
 REAL(8) :: x(npmax), d(npmax), urp(1)
 REAL(8) :: DsFastRTref
 REAL(8), PARAMETER :: bigDs=99999999.d0
 LOGICAL, SAVE :: first=.true.
 EXTERNAL :: qdrtf, qdrtg
!
   IF( LFastRT ) THEN   ! keep RT fixed during fit to improve convergence
     DsFastRTref = DsFastRT
     DsFastRT    = bigDs
   ENDIF
   saveDcaTrk = .false.
!
   IF( first ) THEN
     first = .false.
     call deflt( 2, iv, liv, lv, v)
     v(31) = 1.0d-5    ! afctol absolute tol on f
!    v(32) = 1.0d-8    ! rfctol relative tol on f
     v(33) = 1.0d-6    ! xctol
     v(34) = 1.0d-7   ! xftol
     IF( MBPri <= 3 ) iv(21) = 0
   ENDIF
   IF( ItypFit == 2 .OR. ItypFit == 5 ) THEN
     v(32) = 1.0d-3    ! rfctol absolute tol on f
!   ELSE IF( ItypFit == 1 .OR. ItypFit == 4 ) THEN
!     v(32) = 1.0d-4    ! rfctol absolute tol on f
   ELSE
     v(32) = 1.0d-4    ! rfctol absolute tol on f
   ENDIF
   iv( 1) = 12
   iv(17) = maxcall * 2  ! mxfcal max # of function call
   iv(18) = maxcall      ! mxiter max # of gradient call
   np    = 0
   do i=1,n
     if( err(i) > 1.d-9 ) then
       doderiv(i) = .true.
       np = np + 1
       if( i == 1 ) then
         x(np) = dzt1
         d(np) = 100.d0
       else if( i == 2 ) then
         x(np) = ds1
         d(np) = 1.d0
       else if( i == 3 ) then
         x(np) = dzt2
         d(np) = 100.d0
       else if( i == 4 ) then
         x(np) = ds2
         d(np) = 1.d0
       else if( i == 5 ) then
         x(np) = smu
         d(np) = 5.d0
       else
         x(np) = s1masc(i-5)
         d(np) = 1.d0
       endif
     else
       doderiv(i) = .false.
     endif
   enddo
   uip(1) = n
   urp(1) = 0.d0
   call sumsl(np, d(1:np), x(1:np), qdrtf, qdrtg, iv, liv, lv, v, uip, urp, qdrtf)
   fmin = v(10)
   np = 0
   do i=1,n
     if( .NOT.doderiv(i) ) cycle
     np = np + 1
     if( i == 1 ) then
       dzt1 = x(np)
     else if( i == 2 ) then
       ds1  = x(np)
     else if( i == 3 ) then
       dzt2 = x(np)
     else if( i == 4 ) then
       ds2  = x(np)
     else if( i == 5 ) then
       smu  = x(np)
     else
       s1masc(i-5)  = x(np)
     endif
   enddo
!
   IF( LFastRT ) DsFastRT = DsFastRTref
   saveDcaTrk = .true.
!
 END SUBROUTINE minimizeFcn
!
!
!
 SUBROUTINE qdrtf(n, x, nf, f, uip, urp, ufp)
 USE M_MB_Fit
 USE M_MB_COMUSC
 IMPLICIT NONE
 INTEGER :: n, nf, uip(*)
 INTEGER :: iflag, na, i, k
 REAL(8) :: urp(*), a(20), g(20), x(n), f
 EXTERNAL :: ufp
!
   iflag = 1
   na    = uip(1)
   k     = 0
   do i=1,na
     if( doderiv(i) ) then
       k = k + 1
       a(i) = x(k)
     else
       if( i == 1 ) then
         a(i) = dzt1
       else if( i == 2 ) then
         a(i) = ds1
       else if( i == 3 ) then
         a(i) = dzt2
       else if( i == 4 ) then
         a(i) = ds2
       else if( i == 5 ) then
         a(i) = smu
       else
         a(i) = s1masc(i-5)
       endif
     endif
   enddo
   call fcnmu(na,g,f,a,iflag)
!
 END SUBROUTINE qdrtf
!
 SUBROUTINE qdrtg(n, x, nf, g, uip, urp, ufp)
 USE M_MB_Fit
 USE M_MB_COMUSC
 IMPLICIT NONE
 INTEGER :: n, nf, uip(*)
 INTEGER :: iflag, na, i, k
 REAL(8) :: urp(*), a(20), gg(20), f, x(n), g(n)
 EXTERNAL :: ufp
!
   iflag = 2
   na    = uip(1)
   k     = 0
   do i=1,na
     if( doderiv(i) ) then
       k = k + 1
       a(i) = x(k)
     else
       if( i == 1 ) then
         a(i) = dzt1
       else if( i == 2 ) then
         a(i) = ds1
       else if( i == 3 ) then
         a(i) = dzt2
       else if( i == 4 ) then
         a(i) = ds2
       else if( i == 5 ) then
         a(i) = smu
       else
         a(i) = s1masc(i-5)
       endif
     endif
   enddo
   call fcnmu(na,gg,f,a,iflag)
   k = 0
   do i=1,na
     if( .NOT.doderiv(i) ) cycle
     k = k + 1
     g(k) = gg(i)
   enddo
!
 END SUBROUTINE qdrtg
!
!> Compute the chi2 to be minimized during track fit
!> IFLAG=1 = compute the function
!> IFLAG=2 = compute the derivatives
!
 SUBROUTINE FCNMU(NA,G,F,A,IFLAG)
 USE M_MB_Control
 USE M_MB_FAX
 USE M_MB_PACONS
 USE M_MB_COMUSC
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER :: NA, IFLAG
 REAL(8) :: F
 REAL(8) :: G(*), A(*)
 INTEGER :: IMS, L, NFX, NN, NVDEB, IA
 REAL(8) :: DERIV(KVDEB)
 REAL(8) :: C0, X0, C, X, CP, SigMS
 REAL(8), PARAMETER :: DERIV_ZZZ = 0.000010D0,   DERIV_SSS = 0.001000D0
 REAL(8), PARAMETER :: DERIV_SMU = 0.00001000D0, DERIV_THE = 0.00000050D0
 REAL(8), PARAMETER :: DERIV_PHI = 0.00005000D0, DERIV_S1M = 0.0000500D0
!
   F = 0.D0
   G(1:NA) = 0.D0
!
   C0 =  0.D0
   X0 = -5.D0   ! 5 parameters to be fitted
!
   NVDEB = 1
   inderiv(1) = 0
   IF( ItypFit == 1 .OR. ItypFit == 2 ) THEN
     DZT1 = A(1)
     DS1  = A(2)
     DZT2 = A(3)
     DS2  = A(4)
     SMU  = A(5)
     CALL GIVDEB1(1)
     IF( IFLAG == 2 ) THEN
       DERIV(1) = DERIV_ZZZ
       DERIV(2) = DERIV_SSS
       DERIV(3) = DERIV_ZZZ
       DERIV(4) = DERIV_SSS
       DERIV(5) = A(5)*DERIV_SMU
       IF( doderiv(1) ) THEN
         DZT1 = A(1) + DERIV(1)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) = 1
         DZT1 = A(1) - DERIV(1)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) = -1
         DZT1 = A(1)
       ENDIF
       IF( doderiv(2) ) THEN
         DS1  = A(2) + DERIV(2)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) = 2
         DS1  = A(2) - DERIV(2)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) = -2
         DS1  = A(2)
       ENDIF
       IF( doderiv(3) ) THEN
         DZT2 = A(3) + DERIV(3)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) = 3
         DZT2 = A(3) - DERIV(3)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) = -3
         DZT2 = A(3)
       ENDIF
       IF( doderiv(4) ) THEN
         DS2  = A(4) + DERIV(4)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) = 4
         DS2  = A(4) - DERIV(4)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) = -4
         DS2  = A(4)
       ENDIF
       IF( doderiv(5) ) THEN
         SMU  = A(5) + DERIV(5)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) = 5
         SMU  = A(5) - DERIV(5)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) = -5
         SMU  = A(5)
       ENDIF
     ENDIF
     IF( ItypFit == 1 ) THEN
       CALL TRAKTU(NVDEB,0)
     ELSE
       CALL TRAKDO(NVDEB,0)
     ENDIF
   ELSEIF( ItypFit == 3 ) THEN
     VTHETAPHI(1:5) = A(1:5)
     S1MASC(1:NBMASC)  = A(6:5+NBMASC)
     DS1MASC(1:NBMASC) = 0.D0
     CALL GIVDEB2(1)
     IF( IFLAG == 2 ) THEN
       DERIV(1)          = DERIV_THE
       DERIV(2)          = DERIV_PHI
       DERIV(3)          = DERIV_THE
       DERIV(4)          = DERIV_PHI
       DERIV(5)          = A(5)*DERIV_SMU
       DERIV(6:5+NBMASC) = DERIV_S1M
       DO L=1,5
         VTHETAPHI(L) = A(L) + DERIV(L)
         CALL GIVDEB2(2*L)
         VTHETAPHI(L) = A(L) - DERIV(L)
         CALL GIVDEB2(2*L+1)
         VTHETAPHI(L) = A(L)
       ENDDO
       DO IMS=1,NBMASC
         DS1MASC(IMS) = DERIV(5+IMS)
         CALL GIVDEB2(11+2*IMS-1)
         CALL GIVDEB2(11+2*IMS  )
       ENDDO
       NVDEB = 11 + 2*NBMASC
       DO L=2,NVDEB
         IF( MOD(L,2) == 0 ) THEN
           inderiv(L) =  L/2
         ELSE
           inderiv(L) = -L/2
         ENDIF
       ENDDO
     ENDIF
     CALL TRAFAX(NVDEB,0)
   ELSE IF( ItypFit >= 4 ) THEN
     DZT1 = A(1)
     DS1  = A(2)
     DZT2 = A(3)
     DS2  = A(4)
     SMU  = A(5)
     S1MASC(1:NBMASC)  = A(6:5+NBMASC)
     DS1MASC(1:NBMASC) = 0.D0
     CALL GIVDEB1(1)
     IF( IFLAG == 2 ) THEN
       DERIV(1) = DERIV_ZZZ
       DERIV(2) = DERIV_SSS
       DERIV(3) = DERIV_ZZZ
       DERIV(4) = DERIV_SSS
       DERIV(5) = A(5)*DERIV_SMU
       DERIV(6:5+NBMASC) = DERIV_S1M/ABS(SMU)
       IF( doderiv(1) ) THEN
         DZT1 = A(1) + DERIV(1)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) =  1
         DZT1 = A(1) - DERIV(1)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) = -1
         DZT1 = A(1)
       ENDIF
       IF( doderiv(2) ) THEN
         DS1  = A(2) + DERIV(2)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) =  2
         DS1  = A(2) - DERIV(2)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) = -2
         DS1  = A(2)
       ENDIF
       IF( doderiv(3) ) THEN
         DZT2 = A(3) + DERIV(3)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) =  3
         DZT2 = A(3) - DERIV(3)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) = -3
         DZT2 = A(3)
       ENDIF
       IF( doderiv(4) ) THEN
         DS2  = A(4) + DERIV(4)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) =  4
         DS2  = A(4) - DERIV(4)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) = -4
         DS2  = A(4)
       ENDIF
       IF( doderiv(5) ) THEN
         SMU  = A(5) + DERIV(5)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) =  5
         SMU  = A(5) - DERIV(5)
         NVDEB = NVDEB + 1
         CALL GIVDEB1(NVDEB)
         inderiv(NVDEB) = -5
         SMU  = A(5)
       ENDIF
       DO IMS=1,NBMASC
         IF( doderiv(5+IMS) ) THEN
           DS1MASC(IMS) = DERIV(5+IMS)
           NVDEB = NVDEB + 1
           CALL GIVDEB1(NVDEB)
           inderiv(NVDEB) =  5+IMS
           NVDEB = NVDEB + 1
           CALL GIVDEB1(NVDEB)
           inderiv(NVDEB) = -5-IMS
         ENDIF
       ENDDO
     ENDIF
     IF( ItypFit == 4 ) THEN
       CALL TRAKTU(NVDEB,0)
     ELSE IF( ItypFit == 5 ) THEN
       CALL TRAKDO(NVDEB,0)
     ELSE
       CALL TRAFAX(NVDEB,0)
     ENDIF
   ELSE
     RETURN
   ENDIF
   IF( NVDEB > KVDEB ) THEN
     WRITE(*,*) ' MuonboyCore/mb_rec/fcnmu FATAL ERROR   NVDEB KVDEB ',NVDEB,KVDEB
     STOP
   ENDIF
   NFX = MAX( NBFIXE , NBFAX )
!
   DO L=1,NVDEB
     C = C0
     X = X0
     IF( ItypFit >= 3 ) THEN
       DO IMS=1,NBMASC
         IF( inderiv(L) == 5+IMS ) THEN
           SigMS = S1MASC(IMS) + DS1MASC(IMS)
         ELSE IF( inderiv(L) == -5-IMS ) THEN
           SigMS = S1MASC(IMS) - DS1MASC(IMS)
         ELSE
           SigMS = S1MASC(IMS)
         ENDIF
         C = C + SigMS**2
         IF( ItypFit == 5 ) THEN
           IF( ABS(SigMS) > SigMaxFit5 + 1.d0 ) THEN
             C = C + (ABS(SigMS)-SigMaxFit5-1.d0)**4
           ENDIF
         ENDIF
       ENDDO
     ENDIF
     DO NN=1,NFX
       C = C + DZFIXE(NN,1,L) + DZFIXE(NN,2,L) + DSFIXE(NN,1,L)       + DSFIXE(NN,2,L)
       X = X + NZFIXE(NN,1,L) + NZFIXE(NN,2,L) + NSFIXE(NN,1,L)*FacSZ + NSFIXE(NN,2,L)*FacSZ
     ENDDO
     IF( NCFAX > 0 ) THEN
       NN = NBFAX + 1
       C = C + DSFIXE(NN,1,L)       + DSFIXE(NN,2,L)
       X = X + NSFIXE(NN,1,L)*FacSZ + NSFIXE(NN,2,L)*FacSZ
     ENDIF
     DO NN=1,NBLIBR
       C = C + DZLIBR(NN,1,L) + DZLIBR(NN,2,L) + DSLIBR(NN,1,L)       + DSLIBR(NN,2,L)
       X = X + NZLIBR(NN,1,L) + NZLIBR(NN,2,L) + NSLIBR(NN,1,L)*FacSZ + NSLIBR(NN,2,L)*FacSZ
     ENDDO
     IF( AtCosm == 0 ) THEN
       X = X + 2.d0
       C = C + (REXTRA(L)/DREXTR(L))**2 + (ZEXTRA(L)/DZEXTR(L))**2
     ENDIF
     X = MAX( X , 1.D0 )
     C = C / X
!
     IF( inderiv(L) == 0 ) THEN
       F      =   C
     ELSEIF( inderiv(L) > 0 ) THEN
       CP     =   C
     ELSE
       IA = -inderiv(L)
       G(IA) = ( CP - C ) / 2.D0 / DERIV(IA)
     ENDIF
   ENDDO
!
 END SUBROUTINE FCNMU
!
