!
!> Define ROA based on MDT only by looking for accumulation of hits in theta
!> \author A.Ouraou
!
 SUBROUTINE makeRoaFromMdt(ISC)
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_MuGeom
 USE M_MB_General_CONSTANTS, ONLY : Pi, TwoPi
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: ISC  !< Type of station
 INTEGER :: IFI, I, IC, IW, INOF, INOZ, IGEOM, IMDT, IP, ITDEB, ITFIN, IT, ibin
 INTEGER :: nseg, ib(1), ida, idb, tubcro, tubmin, newroa, iokroa, JNOZ
 REAL(8) :: Tref, Tcent, DIREC, pas, TT, RT, tantet, teta, phi, rrr
 INTEGER, PARAMETER :: nsegmax=2
 INTEGER, PARAMETER :: NBIMA=1000, NBIMA2=2*NBIMA
 INTEGER :: HISTO(NBIMA2,0:2)
!
   IF( CARISC(ISC)(1:2) /= 'BI' .AND. CARISC(ISC)(1:2) /= 'EO' .AND. CARISC(ISC)(1:2) /= 'EM' .AND.  &
       CARISC(ISC)(1:2) /= 'EE' .AND. CARISC(ISC) /= 'BOS'     .AND. CARISC(ISC) /= 'EIL' ) RETURN
   newroa = 0

   DO IFI=1,8
     IF( CARISC(ISC) == 'BOS' .AND. IFI /= 6 .AND. IFI /= 7 ) CYCLE
     tubcro = 0
     HISTO(:,:) = 0
     DO I=1,2
       IC = 2*(ISC-1) + I
       DO IW=1,NCHAHI(IC)
         INOF  = IFCHA(IC,IW)
         IF( INOF /= IFI )       CYCLE
         INOZ  = IZCHA(IC,IW)
         IF( CARISC(ISC)(1:2) == 'EO' .OR. CARISC(ISC)(1:2) == 'EM' ) THEN
           IF( INOZ /= 1 .AND. INOZ /= NZMS2 + 1 )      CYCLE
         ENDIF
         IF( CARISC(ISC) == 'BOS' .AND. INOZ /= NZMS2 ) CYCLE
         IF( CARISC(ISC) == 'EIL' ) THEN
           JNOZ = MOD( INOZ, NZMS2 )
           SELECT CASE ( INOF )
             CASE( 1, 5 ) ; IF( JNOZ /= 3 .AND. JNOZ /= 4 ) CYCLE
             CASE DEFAULT ; IF( JNOZ /= 3 )                 CYCLE
           END SELECT
         ENDIF
         IGEOM = IGEOMA(IC,INOF,INOZ)
         IMDT  = ISTMDTV(IGEOM)
         Tcent = TCECHAV(IGEOM)
         DIREC = 1.d0
         IF( Tcent < 0.d0 ) DIREC = -1.d0
         Tcent = ABS(Tcent)
         IF( tubcro <= 0 ) THEN
           tubcro = 2*NLTUB(IMDT) - 1
           IF( NLTUB(IMDT) >= 4 ) tubcro = tubcro - 1
           tubmin =   NLTUB(IMDT)
           pas    =   PASTUB(IMDT)
           Tref   = Tcent
           phi    = FCECHAV(IGEOM)
           IF( phi > Pi )  phi = phi - TwoPi
         ENDIF
!
         DO IP=1,NLTUB(IMDT)
           TT = Tcent + TLTUB(IMDT,IP,1)
           RT = DIREC * Tref / TT / pas
           ITDEB = NTUBH0(IC,IW,IP) + 1
           ITFIN = NTUBH0(IC,IW,IP) + NTUBHI(IC,IW,IP)
           DO IT=ITDEB,ITFIN
             IF( LTUBHI(IT) /= 1 )  CYCLE
             ibin = int( abs(RT*ZTUBHI(IT)) ) + 1
             IF( RT*ZTUBHI(IT) >= 0.d0 ) THEN
               ibin = NBIMA + ibin
             ELSE
               ibin = NBIMA + 1 - ibin
             ENDIF
             HISTO(ibin-1,I) = HISTO(ibin-1,I) + 1
             HISTO(ibin  ,I) = HISTO(ibin  ,I) + 1
             HISTO(ibin+1,I) = HISTO(ibin+1,I) + 1
           ENDDO
         ENDDO
       ENDDO
     ENDDO
!
     IF( tubcro <= 0 )  CYCLE
     HISTO(:,0) = HISTO(:,1) + HISTO(:,2)
!
     nseg = 0
     DO WHILE ( nseg < nsegmax )
       ib = MAXLOC(HISTO(:,0))
       ibin = ib(1)
       IF( HISTO(ibin,0) < tubcro ) EXIT
       idb = 0
       DO WHILE ( HISTO(ibin+idb+1,0) == HISTO(ibin,0) )
         idb = idb + 1
       ENDDO
       idb   = ( idb + 1 ) / 2
       ibin = ibin + idb

       IF( MBPri >= 3 ) WRITE(*,'("try to create a new Roa with ",2I3," hits for sector",I2)') HISTO(ibin,1:2),IFI
       IF( HISTO(ibin,1) > 0 .AND. HISTO(ibin,2) > 0 ) THEN  ! keep only if hits in both ML
         tantet = (ibin-NBIMA-0.5) * pas / Tref
         rrr = Tref * SQRT(1.+tantet**2)
         IF( ISC <= NCOUBAS2 ) tantet = 1. / tantet
         teta = atan(tantet)
         if( teta < 0.d0 ) teta = teta + Pi
         CALL addAlowMdt(rrr,teta,phi,iokroa)
         IF( iokroa == 1 ) then
           nseg   = nseg   + 1
           newroa = newroa + 1
         ELSE
           IF( MBPri >= 3 ) WRITE(*,'("potential Roa compatible with an existing one -> give up")')
         ENDIF
       ELSE
         IF( MBPri >= 3 ) WRITE(*,'("potential Roa with hits is only one ML -> give up")')
       ENDIF
       HISTO(MAX(1,ibin-idb-5):MIN(NBIMA2,ibin+idb+5),0) = 0
       ida = idb + 6
       IF( ibin - ida >= 1 ) THEN
         DO WHILE ( HISTO(ibin-ida,0) >= tubmin )
           HISTO(ibin-ida,0) = 0
           ida = ida + 1
           IF( ibin - ida < 1 ) EXIT
         ENDDO
       ENDIF
       ida = idb + 6
       IF( ibin + ida <= NBIMA2 ) THEN
         DO WHILE ( HISTO(ibin+ida,0) >= tubmin )
           HISTO(ibin+ida,0) = 0
           ida = ida + 1
           IF( ibin + ida > NBIMA2 ) EXIT
         ENDDO
       ENDIF
     ENDDO
   ENDDO
!
   IF( MBPri >= 3 ) WRITE(*,'(" --- makeRoaFromMdt for ISC=",I2," adds ",I2," ROA",/)') ISC,newroa
!
 END SUBROUTINE makeRoaFromMdt
!
