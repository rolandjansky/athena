!
!> Segment reconstruction in the CSC
!> \author M.Virchaux
!
 SUBROUTINE CHICSC(segcsc, IC,INOF,INOZ, ZZtrue,SStrue, VZtrue,VTtrue,VStrue, NTOT,CTOT, dcapoin)
 USE M_MB_Control
 USE M_MB_Types
 USE M_MB_Digis
 USE M_MB_General_CONSTANTS, ONLY : Zero, One
 USE M_MB_MuGeom
 IMPLICIT NONE
 LOGICAL, INTENT(IN)  :: segcsc
 INTEGER, INTENT(IN)  :: IC, INOF, INOZ
 REAL(8), INTENT(IN)  :: ZZtrue, SStrue, VZtrue, VTtrue, VStrue
 INTEGER, INTENT(OUT) :: NTOT
 REAL(8), INTENT(OUT) :: CTOT
 TYPE(DcaPoint), INTENT(OUT) :: dcapoin
 INTEGER :: IW, IGEOM, IMDT, IP, ITDEB, ITFIN, IT, Itrans, i, Jcsc
 REAL(8) :: ZZ, SS, VZ, VT, TT, TTtrue, DIREC
 REAL(8) :: UZT, UST, DT, STUT, ZTUT, TTUT, STAT, ZTAT, TTAT 
 REAL(8) :: CCUR, CHICUR, fact
 REAL(8), PARAMETER :: CHIMAZ=80.d0, CHIMAS=40.d0
 REAL(8), PARAMETER :: seuil=1.73205d0, transition=0.2d0  ! seuil = sqrt(3.)
 REAL(8) :: A0,SinA0,CosA0
 REAL(8) :: SiFFi,CoFFi
 REAL(8) :: Zul,Tul, ZBt,TBt
 REAL(8) :: Z1DCA, T1DCA, S1DCA
 REAL(8) :: dirx, diry, dirz
 INTEGER :: intpos
!
! segcsc is true when called from filicsc (segment maker, in "ideal" coordinate)
! otherwise called from chizzz (track maker, in "true" coordinate)
!
     IGEOM = IGEOMA(IC,INOF,INOZ)
     IMDT  = ISTMDTV(IGEOM)
!
     NTOT = 0
     CTOT = Zero
     CALL ResetDCA( dcapoin )
!
     IW        = IWCHA (IC,INOF,INOZ)
     DIREC     = SIGN( One, TCECHAV(IGEOM) )
     Itrans = ItransCHA(IGEOM)
     Jcsc   = intpos(IGEOM,IGEOMCSC,NChaCSC)
     TTtrue = TCECHAV(IGEOM)
     IF( segcsc .OR. Itrans <= 0 ) THEN
       TT = TTtrue
       SS = SStrue
       ZZ = ZZtrue
       VT = VTtrue
       UZT = VZtrue / VTtrue
       UST = VStrue / VTtrue
     ELSE
       CALL SZTWU_Ideal_from_True(Itrans, SStrue,ZZtrue,TTtrue,VStrue,VZtrue,VTtrue, SS,ZZ,TT,VZ,VT,UZT,UST)
     ENDIF
!
     A0    = GCECHAV(IGEOM)
     SinA0 = SIN(A0)
     CosA0 = COS(A0)
     SiFFi = SINCHAV(IGEOM)
     CoFFi = COSCHAV(IGEOM)
     diry =  CosA0 * VZtrue + SinA0 * VTtrue
     dirz = -SinA0 * VZtrue + CosA0 * VTtrue
     dirx = CoFFi * diry - SiFFi * VStrue
     diry = SiFFi * diry + CoFFi * VStrue
     IF( VT < 0.d0) THEN
       dirx = - dirx
       diry = - diry
       dirz = - dirz
     ENDIF
     dirx = dirx * DIREC
     diry = diry * DIREC
     dirz = dirz * DIREC
!
bip: DO IP=1,NLTUB(IMDT)
       IF( ndeadChan(IGEOM) > 0 ) THEN
         DO i=ideadChan(IGEOM)+1,ideadChan(IGEOM)+ndeadChan(IGEOM)
           IF( ldeadChan(i) == IP ) CYCLE bip
         ENDDO
       ENDIF
!
       DT = TTtrue + DIREC * ( TLTUB(IMDT,IP,1) + TLTUB(IMDT,IP,2) ) / 2.d0 - TT
!     
       ZTAT = ZZ + DT*UZT
       STAT = SS + DT*UST
       TTAT = TT + DT
       CALL Pos_Layer_from_Csc(Jcsc,IP,STAT,ZTAT,TTAT,STUT,ZTUT,TTUT)
       dcapoin%CCC(IP,1:2) = 9999999.d0
       dcapoin%RTRA(IP,1 ) = ZTUT
       dcapoin%RTRA(IP,2 ) = STUT
       IF( saveDcaSeg .OR. SaveDcaTrk ) THEN
         CALL Pos_True_from_Ideal( Itrans, STAT,ZTAT,TTAT, S1DCA,ZBt,TBt )
         Zul   = TBt
         Tul   = ZBt
         T1DCA =  CosA0 * Tul + SinA0 * Zul
         Z1DCA = -SinA0 * Tul + CosA0 * Zul
         dcapoin%XDCA(IP,1:2) = CoFFi*T1DCA - SiFFi*S1DCA
         dcapoin%YDCA(IP,1:2) = SiFFi*T1DCA + CoFFi*S1DCA
         dcapoin%ZDCA(IP,1:2) = Z1DCA
         dcapoin%XDIR(IP,1:2) = dirx
         dcapoin%YDIR(IP,1:2) = diry
         dcapoin%ZDIR(IP,1:2) = dirz
         dcapoin%SDCA(IP, 1 ) = STUT
         dcapoin%SDCA(IP, 2 ) = ZTUT
         dcapoin%EDCA(IP,1:2) = 99999.d0
         dcapoin%IDE(IP,1:2)  = 0
       ENDIF
!
       ITDEB = NTUBH0(IC,IW,IP) + 1
       ITFIN = NTUBH0(IC,IW,IP) + NTUBHI(IC,IW,IP)
       DO IT=ITDEB,ITFIN
         IF( IT <= NTUBHM .OR. IT > NTUBHT ) THEN  ! useless test to avoid a fpe on lxplus
           WRITE(*,*) ' Problem MuonboyCore/chicsc it n0 ni ntubhm ntubht ',it,ntubh0(ic,iw,ip),ntubhi(ic,iw,ip),ntubhm,ntubht
         ENDIF
         IF( RTUBHI(IT) < -10.D0 ) THEN
!-- S reconstructed position
           IF( ABS(STUBHI(IT)-STUT) > 5000.d0*DTUBHI(IT) )  CYCLE
           CCUR = ( (STUBHI(IT)-STUT) / DTUBHI(IT) )**2
           fact = 1.d0
           IF( LTUBHI(IT) /= 0 .AND. LTUBHI(IT) /= 10 ) THEN ! error is in fact width/sqrt(12)
             fact = ( abs(STUBHI(IT)-STUT) - seuil*DTUBHI(IT) ) / transition
             IF( fact > 20.d0 ) THEN
               fact = 3.d0
             ELSE IF( fact > -20.d0 ) THEN
               fact = 2.d0 + TANH(REAL(fact,10))   ! differences between slc5 and slc6 in 17.2.7.4 
             ELSE
               fact = 1.d0
             ENDIF
             CCUR = CCUR * fact
           ENDIF
           IF( CCUR < dcapoin%CCC(IP,2) ) THEN
             dcapoin%CCC(IP,2)  = CCUR
             dcapoin%JJJ(IP,2)  = JTUBHI(IT)
             dcapoin%III(IP,2)  = IT
             dcapoin%RHIT(IP,2) = STUBHI(IT)
             dcapoin%RDCA(IP,2) = DTUBHI(IT)
             dcapoin%scal(IP,2) = fact
             dcapoin%dRodZ(IP,2) = 0.d0
             dcapoin%dRodR(IP,2) = 0.d0
             dcapoin%d2RodZdR(IP,2) = 0.d0
             dcapoin%d2RodRdR(IP,2) = 0.d0
           ENDIF
         ELSEIF( RTUBHI(IT) < 0.D0 ) THEN
!-- Z reconstructed position
           IF( ABS(ZTUBHI(IT)-ZTUT) > 5000.d0*DTUBHI(IT) )  CYCLE
           CCUR = ( (ZTUBHI(IT)-ZTUT) / DTUBHI(IT) )**2
           fact = 1.d0
           IF( LTUBHI(IT) /= 0 .AND. LTUBHI(IT) /= 10 ) THEN ! error is in fact width/sqrt(12)
             fact = ( abs(ZTUBHI(IT)-ZTUT) - seuil*DTUBHI(IT) ) / transition
             IF( fact > 20.d0 ) THEN
               fact = 3.d0
             ELSE IF( fact > -20.d0 ) THEN
               fact = 2.d0 + TANH(REAL(fact,10))   ! differences between slc5 and slc6 in 17.2.7.4
             ELSE
               fact = 1.d0
             ENDIF
             CCUR = CCUR * fact
           ENDIF
           IF( CCUR < dcapoin%CCC(IP,1) ) THEN
             dcapoin%CCC(IP,1)  = CCUR
             dcapoin%JJJ(IP,1)  = JTUBHI(IT)
             dcapoin%III(IP,1)  = IT
             dcapoin%RHIT(IP,1) = ZTUBHI(IT)
             dcapoin%RDCA(IP,1) = DTUBHI(IT)
             dcapoin%scal(IP,1) = fact
             dcapoin%dRodZ(IP,1) = 1.d0
             dcapoin%dRodR(IP,1) = DT * (1+UZT**2)
             dcapoin%d2RodZdR(IP,1) = 0.d0
             dcapoin%d2RodRdR(IP,1) = 2.d0 * DT * UZT * (1+UZT**2)
           ENDIF
         ENDIF
       ENDDO
!
       IF( dcapoin%CCC(IP,1) > CHIMAZ ) THEN
         CCUR = dcapoin%CCC(IP,1)
         IF( CCUR > 2.0d0*CHIMAZ ) THEN
           dcapoin%JJJ(IP,1) = -20000000
           dcapoin%III(IP,1) = 0
           dcapoin%IDE(IP,1) = IGEOM + 100000*IP + 1000000
         ENDIF
         dcapoin%CCC(IP,1) = CHICUR( CCUR , CHIMAZ )
         IF( dcapoin%III(IP,1) > 0 ) dcapoin%scal(IP,1) = dcapoin%scal(IP,1) * dcapoin%CCC(IP,1) / CCUR
       ENDIF
!
       IF( dcapoin%CCC(IP,2) > CHIMAS ) THEN
         IF( dcapoin%CCC(IP,2) > 2.0d0*CHIMAS ) THEN
           dcapoin%JJJ(IP,2) = -20000000
           dcapoin%III(IP,2) = 0
           dcapoin%IDE(IP,2) = IGEOM + 100000*IP + 2000000
         ENDIF
         dcapoin%CCC(IP,2) = CHICUR( dcapoin%CCC(IP,2) , CHIMAS )
       ENDIF
!
       NTOT = NTOT + 2
       CTOT = CTOT + dcapoin%CCC(IP,1) + dcapoin%CCC(IP,2)
     ENDDO bip
!
 END SUBROUTINE CHICSC
!
