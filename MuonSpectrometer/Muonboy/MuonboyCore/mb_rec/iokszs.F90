!
!> Check that at least one eta strip (RPC or TGC) is compatible with the segment
!> \author M.Virchaux
!
 INTEGER FUNCTION IOKSZS(IC,IW,ZZ,TT,SS,WZ,WT,VST)
 USE M_MB_Digis
 USE M_MB_MuGeom
 USE M_MB_General_CONSTANTS, ONLY : Pi, TwoPi
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IC, IW
 REAL(8), INTENT(IN) :: ZZ, TT, SS, WZ, WT, VST
 INTEGER :: IGEOM, IMDT, ILIN, IL, ICT, IRPC, IP, ISDEB, ISFIN, ISS, JSPLI
 REAL(8) :: DIREC, VZT, TTA,ZZA,VSTA,VZTA, TTB,ZZB, DTB, FIA, FMI, FMA, S00, TolZ
!
   IGEOM = IGEOMA(IC,IFCHA(IC,IW),IZCHA(IC,IW))
   IMDT = ISTMDTV(IGEOM)
   ILIN = ISTLINV(IGEOM)
!
   IOKSZS = 0
   IF( ILIN <= 0 )        RETURN
   IF( NBLINK(ILIN) <= 0) RETURN
   VZT = WZ / WT
!
   IOKSZS = 1
   DO IL=1,NBLINK(ILIN)
     ICT  = ICTRIG(IL,ILIN)
     IRPC = IRPTGC(ICT)
     DIREC = 1.D0
     IF( TCETRI(ICT) < 0.d0 )  DIREC = -1.d0
     TolZ = 0.55D0*(DZBAND(IRPC)+PASTUB(IMDT))
     IF( ( IC <= NCOUBA .AND. ICT <= NTRIBA ) .OR. ( IC > NCOUBA .AND. ICT > NTRIBA ) ) THEN
       ZZA  = ZZ
       TTA  = TT
       VSTA = VST
       VZTA = VZT
     ELSE
       IF( ABS(VZT) < 0.01d0 ) CYCLE
       ZZA  = TT
       TTA  = ZZ
       VZTA =  1.d0/VZT
       VSTA = VST*VZTA
     ENDIF
     IF( ICT > NTRIBA ) THEN ! special case because TGC coverage smaller than MDT in EndCap 
       FIA = FCECHAV(IGEOM) + ASIN(SS/ZZA)
       FMI = FCETRI(ICT) + ASIN(0.5d0*(SNITRI(ICT)+SNATRI(ICT))/ZCETRI(ICT)) - 0.07d0
       FMA = FCETRI(ICT) + ASIN(0.5d0*(SPITRI(ICT)+SPATRI(ICT))/ZCETRI(ICT)) + 0.07d0
       IF( FIA < FMI ) THEN
         FIA = FIA + TwoPi
         IF( FIA > FMA ) CYCLE
       ELSE IF( FIA > FMA ) THEN
         FIA = FIA - TwoPi
         IF( FIA < FMI ) CYCLE
       ENDIF
     ELSE
       S00 = 0.25d0 * ( SPITRI(ICT) + SPATRI(ICT) + SNITRI(ICT) + SNATRI(ICT) )
     ENDIF
     DO IP=N1BAND(IRPC)+1,N2BAND(IRPC)
       TTB = TCETRI(ICT) + DIREC*TBAND(IRPC,IP)
       ZZB = ZZA + VZTA*(TTB-TTA)
       IF( ICT > NTRIBA ) THEN !  approximation of circular TGC
         ZZB = SQRT( ZZB**2 + ( SS + VSTA*(TTB-TTA) )**2 )
       ENDIF
       DTB = 0.02D0*ABS(TTB-TTA) + TolZ
       ISDEB = NBANH0(ICT,IP) + 1
       ISFIN = NBANH0(ICT,IP) + NBANHI(ICT,IP)
       DO ISS=ISDEB,ISFIN
!        IF( rejectStrip(ISS) )                     CYCLE
         IF( ICT <= NTRIBA ) THEN  ! RPC are split in two parts
           JSPLI = MOD( ABS(LBANHI(ISS)) , 10 )
           IF( JSPLI == 1 .OR. JSPLI == 3 ) THEN
             IF( SS + VSTA*(TTB-TTA) > S00 + 8.d0 ) CYCLE
           ELSE IF( JSPLI == 2 .OR. JSPLI == 4 ) THEN
             IF( SS + VSTA*(TTB-TTA) < S00 - 8.d0 ) CYCLE
           ENDIF
         ENDIF
         IF( ABS(SBANHI(ISS)-ZZB) < DTB )  RETURN
       ENDDO
     ENDDO
   ENDDO
!
   IOKSZS = -1
!
 END FUNCTION IOKSZS
!
