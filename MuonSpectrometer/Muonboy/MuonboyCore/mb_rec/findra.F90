!
!> Find straight track segments in MDT stations of type ISC
!> \author M.Virchaux
!
 SUBROUTINE FINDRA(ALOCEL,STRICT,SINGML,ISC, MACADO,NDIM1, JFLAGI,JFLAGO, NBDRIT,segm )
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_General_CONSTANTS, ONLY : CP0000
 USE M_MB_MuGeom
 USE M_MB_SSBAN
 USE M_MB_Types
 IMPLICIT NONE
 INTEGER,       INTENT(IN)    :: ISC, MACADO, NDIM1
 LOGICAL,       INTENT(IN)    :: ALOCEL, STRICT, SINGML
 INTEGER,       INTENT(INOUT) :: JFLAGI(*), JFLAGO(*)
 INTEGER,       INTENT(OUT)   :: NBDRIT
 TYPE(Segment), INTENT(OUT)   :: segm(NDIM1)
 TYPE(Segment) :: segcado(MACADO)
 INTEGER :: ISTRIC, IC1,IC2, I, IC, IW, IMDT, IP, ITDEB,ITFIN, IW1, INOF1, INOZ1, IGEOM1, ILIN1
 INTEGER :: IW2, INOF2, INOZ2, IGEOM2, ILIN2, NBDRIT0, LSPAS0, LSPAS1, LSPASS, LS1,LS2,LSS, LSI
 INTEGER :: NBCADO, IK, J, ICI, IWI, INOFI, INOZI, IGEOMI, ILINI, NtubMax, NtubHit
 INTEGER :: ICC(2),IWW(2), JLTUB(2), LLI(2), IMDT1, IMDT2, IMDTI, ICT
 REAL(8) :: F01,T01,Z01, F02,T02,Z02, DSS1,DSS2, DSS,DSSCUT, TZ2S1, TTMOY,ZZMOY, F0I
 REAL(8) :: SSI(2), TTI(2), FracTub
 LOGICAL :: doSSfromROA, SSfromROA, BOG1, BOG2, askROA
 LOGICAL :: BOGcase !< special case for BOG where we apply no requirement on ROA nor on eta strips for strict segments
 INTEGER :: IOKALA
!
    NBDRIT = 0
!
    ISTRIC = 0
    IF( STRICT ) ISTRIC = 1
    IC1 = 2*ISC
    IC2 = 2*ISC - 1
    ICC(1) = IC1
    ICC(2) = IC2
!
!-- Set the flags of the hits to 0 in order to use them for testing wether a hit
!-- has been used or not (and on what side) to form a straight track segment
    DO I=1,2
      IC = ICC(I)
      DO IW=1,NCHAHI(IC)
        IMDT = ISTMDTV( IGEOMA(IC,IFCHA(IC,IW),IZCHA(IC,IW)) )
        DO IP=1,NLTUB(IMDT)
          ITDEB = NTUBH0(IC,IW,IP) + 1
          ITFIN = NTUBH0(IC,IW,IP) + NTUBHI(IC,IW,IP)
          KTUBHI(ITDEB:ITFIN) = 0
        ENDDO
      ENDDO
    ENDDO
!
!----------------------------------------------------------------------------
!-- One starts with straight track segments extending through two
!-- multi-layers
    IF( STRICT) THEN
      FracTub = TubStr2ML
    ELSE
      FracTub = TubLoo2ML
    ENDIF 
    DO 201 IW1=1,NCHAHI(IC1)
      IWW(1)   = IW1
      INOF1    = IFCHA(IC1,IW1)
      INOZ1    = IZCHA(IC1,IW1)
      IGEOM1   = IGEOMA(IC1,INOF1,INOZ1)
      IMDT1    = ISTMDTV(IGEOM1)
      NtubMax  = NINT( FracTub * ZLOCHAV(IGEOM1) * PASTUB_inv(IMDT1) * NLTUB(IMDT1) )
      NtubHit  = SUM( NTUBHI(IC1,IW1,1:NLTUB(IMDT1)) )
      IF( NtubHit > NtubMax ) CYCLE
      F01      = FCECHAV(IGEOM1)
      T01      = TCECHAV(IGEOM1)
      Z01      = ZCECHAV(IGEOM1)
      JLTUB(1) = NLTUB(IMDT1)
      ILIN1    = ISTLINV(IGEOM1)
      DSS1     = DSLINK(ILIN1)
      BOG1     = .false.      
      IF( STRICT ) THEN
        IF( INOF1 == 6 .OR. INOF1 == 7 ) THEN
          IF( isBOG(INOZ1) ) THEN
            IF( CARISC(ISC) == 'BOS' ) THEN
              BOG1 = .true.
            ENDIF
          ENDIF
        ENDIF
      ENDIF
      DO 200 IW2=1,NCHAHI(IC2)
        IWW(2)   = IW2
        INOF2    = IFCHA(IC2,IW2)
        IF( INOF2 /= INOF1 )        CYCLE
        INOZ2    = IZCHA(IC2,IW2)
        IGEOM2   = IGEOMA(IC2,IFCHA(IC2,IW2),INOZ2)
        IMDT2    = ISTMDTV(IGEOM2)
        NtubMax  = NINT( FracTub * ZLOCHAV(IGEOM2) * PASTUB_inv(IMDT2) * NLTUB(IMDT2) )
        NtubHit  = SUM( NTUBHI(IC2,IW2,1:NLTUB(IMDT2)) )
        IF( NtubHit > NtubMax ) CYCLE
        F02      = FCECHAV(IGEOM2)
        T02      = TCECHAV(IGEOM2)
! The two multi-layers must be in the same phi position
        IF( ABS(F01-F02) > 0.01D0 ) CYCLE
! If the two multi-layers are in the end caps, they must be in the same one
        IF( T01*T02 < 0.D0 )        CYCLE
! deal with potentially very displaced stations : do not try segments if
! multilayers too far or if the inner one is farther than the outer one
        IF( ABS(T02-T01) > 150.d0 ) CYCLE
        IF( ABS(T01) < ABS(T02) )   CYCLE
! Allow for one adjacent chamber only along Z (or T)
        IF( IABS(INOZ1-INOZ2) > 1 ) CYCLE
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        Z02      = ZCECHAV(IGEOM2)
        JLTUB(2) = NLTUB(IMDT2)
        ILIN2    = ISTLINV(IGEOM2)
        DSS2     = DSLINK(ILIN2)
        DSS      = 3.0D0*(DSS1+DSS2)
        IF(IC2 <= NCOUBA) THEN
          TZ2S1 = T02 / T01
          TTMOY = (T01+T02)/2.D0
          ZZMOY = (Z01+Z02)/2.D0
        ELSE
          TZ2S1 = Z02 / Z01
          TTMOY = (Z01+Z02)/2.D0
          ZZMOY = (T01+T02)/2.D0
        ENDIF
!--
        BOG2 = .false.
        IF( STRICT .AND. .NOT.BOG1 ) THEN
          IF( INOF2 == 6 .OR. INOF2 == 7 ) THEN
            IF( isBOG(INOZ2) ) THEN
              IF( CARISC(ISC) == 'BOS' ) THEN
                BOG2 = .true.
              ENDIF
            ENDIF
          ENDIF
        ENDIF
        BOGcase = BOG1 .OR. BOG2
        askROA = ALOCEL .AND. .NOT.BOGcase
!-- In strict conditions,
!-- there must be at least one S strip measurement among the two multi-layers
        IF( STRICT .AND. ILIN1 <= 0 .AND. ILIN2 <= 0 )          CYCLE
!-- The two multi-layers must be not too far from an "allowed" (Theta,Phi) cell
        IF( askROA .AND. IOKALA(F01,ZZMOY,TTMOY,CP0000) == 0 )  CYCLE
!
!-- In strict conditions :  One to two loops on S positions according to the
!-- presence of strips in 1 or 2 multi-layers (LSPASS = 2 or 1).
!-- Otherwise :  One to three loops on S positions according to the presence
!-- of strips in 0, 1 or 2 multi-layers (LSPASS = 3, 2 or 1).
        NBDRIT0 = NBDRIT
        LSPAS0 = 1
        IF( ILIN1 <= 0 ) LSPAS0 = LSPAS0 + 1
        IF( ILIN2 <= 0 ) LSPAS0 = LSPAS0 + 1
        doSSfromROA = .false.
        IF( LSPAS0 == 3 ) THEN    ! special case for stations without RPC or TGC
          IF( AtCosm == 0 .AND. .NOT.STRICT ) THEN
            LSPAS0 = 2
            doSSfromROA = .true.
          ENDIF
        ENDIF
        LSPAS1 = 3 - ISTRIC
        DO 170 LSPASS=LSPAS0,LSPAS1
!-- For Test beam (H8)  or  "Cosmic"  data,  skip search without strips when
!-- at least one straight track segment with strip(s) was previously found
          IF( AtCosm /= 0 .AND. LSPASS == 3 .AND. NBDRIT > NBDRIT0) CYCLE
!-- Stores in JFLAGI and JFLAGO the flags of the hits
!-- (telling whether a hit has been used) before looping on S positions
          CALL UNJTUB(2,ICC,IWW,JLTUB, JFLAGI)
          CALL UNJTUB(2,ICC,IWW,JLTUB, JFLAGO)
          TTI(1) = SSBAN(NSBAN1(IC1,IW1)+1)
          TTI(2) = SSBAN(NSBAN1(IC2,IW2)+1)
b151:     DO 151 LS1=NSBAN0(IC1,IW1),NSBAN1(IC1,IW1)
! special case in BOF-BOG where the same strips (inner RPC) may be linked to both ML
! so we skip the dupplicate outer one
            IF( ISC == 5 ) THEN
              IF( LS1 /= NSBAN1(IC1,IW1) ) THEN
                ICT = KBANHI(ISBAN(1,LS1)) / 1000
                DO LS2=NSBAN0(IC2,IW2),NSBAN1(IC2,IW2)-1
                  IF( KBANHI(ISBAN(1,LS2)) / 1000 == ICT ) CYCLE b151
                ENDDO
              ENDIF
            ENDIF
! end of special case
            SSI(1) = SSBAN(LS1)
            LLI(1) = LS1
            IF( LS1 == NSBAN1(IC1,IW1) ) LLI(1) = 0
            DO 150 LS2=NSBAN0(IC2,IW2),NSBAN1(IC2,IW2)
              SSI(2) = SSBAN(LS2)
              LLI(2) = LS2
              IF( LS2 == NSBAN1(IC2,IW2) ) LLI(2) = 0
              LSS = 1 + (LS1-NSBAN0(IC1,IW1)+1)/(NSBAN1(IC1,IW1)-NSBAN0(IC1,IW1)+1)    &
                      + (LS2-NSBAN0(IC2,IW2)+1)/(NSBAN1(IC2,IW2)-NSBAN0(IC2,IW2)+1)
              SSfromROA = .false.
              IF( doSSfromROA ) THEN
                LSS = LSPASS
                IF( LSPASS == 2 ) SSfromROA = .true.
              ENDIF
              IF( LSS /= LSPASS )                                    CYCLE
              IF( LSS == 1 ) THEN       !>> When there are two S strip measurements,
                IF( AtCosm == 0 ) THEN  !>> they must point to vertex if collisions
                  DSSCUT = DSS + ABS(TTI(2)-TTI(1))/50.d0
                  IF( ABS(SSI(2)-SSI(1)*TZ2S1) > DSSCUT )            CYCLE
                ELSE                    !>> or not be too far if cosmics
                  IF( ABS((SSI(2)-SSI(1))/(TTI(2)-TTI(1))) > 2.d0 )  CYCLE
                ENDIF
              ENDIF
!-- All S positions are examined with the same flags
              CALL LOJTUB(2,ICC,IWW,JLTUB, JFLAGI)
!
              CALL DROITU(askROA,STRICT,SSfromROA,BOGcase,ICC,IWW,LLI,SSI,TTI,MACADO,NBCADO, segcado)
!
!-- Stores output of DROITU
              DO 120 IK=1,NBCADO
                NBDRIT = NBDRIT + 1
                segm(NBDRIT) = segcado(IK)
                segm(NBDRIT)%errz = 999.d0
                segm(NBDRIT)%phi0 = F01
                segm(NBDRIT)%alfa = 0.d0
                segm(NBDRIT)%jff  = INOF1
                DO J=1,2
!-- Flag (IWDRIT < 0) the straight track segments obtained without any S measurements
                  IF( SSI(1) > 100000.D0 .AND. SSI(2) > 100000.D0 ) THEN
                    segm(NBDRIT)%iw(J) = - IWW(J)
                  ELSE
                    segm(NBDRIT)%iw(J) =   IWW(J)
                  ENDIF
                  IF(ISC > NCOUBAS2) THEN
                    segm(NBDRIT)%zpos(J) = segcado(IK)%tpos(J)
                    segm(NBDRIT)%tpos(J) = segcado(IK)%zpos(J)
                  ENDIF
                ENDDO
                segm(NBDRIT)%is(1) = LS1
                segm(NBDRIT)%is(2) = LS2
                IF( NBDRIT >= NDIM1 ) GO TO 600
120           ENDDO
!-- Updates JFLAGO by overwriting the flags of the hits that have been used
              CALL UPJTUB(2,ICC,IWW,JLTUB, JFLAGO)
              IF( NBCADO >= MACADO ) GO TO 160
150         ENDDO
151       ENDDO b151
!-- Flags the "used hits" after looping on all S positions
160       CALL LOJTUB(2,ICC,IWW,JLTUB, JFLAGO)
170     ENDDO
!
200   ENDDO
201 ENDDO
!----------------------------------------------------------------------------
!
    IF( .NOT.SINGML ) GO TO 600
!
!----------------------------------------------------------------------------
!-- If, search is to be done in single multi-layers,  one goes on (with the
!-- remaining hits) with straight track segments in single multi-layers
    IF( STRICT) THEN
      FracTub = TubStr1ML
    ELSE
      FracTub = TubLoo1ML
    ENDIF
    DO 401 I=1,2
      IWW(:) = 0
      LLI(:) = 0
      TTI(:) = 0.D0
      SSI(:) = 999999.D0
      ICI = ICC(I)
      DO 400 IWI=1,NCHAHI(ICI)
        IWW(I) = IWI
        INOFI  = IFCHA(ICI,IWI)
        INOZI  = IZCHA(ICI,IWI)
        IGEOMI = IGEOMA(ICI,INOFI,INOZI)
        IMDTI    = ISTMDTV(IGEOMI)
        NtubMax  = NINT( FracTub * ZLOCHAV(IGEOMI) * PASTUB_inv(IMDTI) * NLTUB(IMDTI) )
        NtubHit  = SUM( NTUBHI(ICI,IWI,1:NLTUB(IMDTI)) )
        IF( NtubHit > NtubMax ) CYCLE
        IF( NtubHit <= 1      ) CYCLE
        F0I    = FCECHAV(IGEOMI)
        JLTUB(I) =  NLTUB(IMDTI)
        ILINI  = ISTLINV(IGEOMI)
        IF(ICI <= NCOUBA) THEN
          TTMOY = TCECHAV(IGEOMI)
          ZZMOY = ZCECHAV(IGEOMI)
        ELSE
          TTMOY = ZCECHAV(IGEOMI)
          ZZMOY = TCECHAV(IGEOMI)
        ENDIF
!--
        BOGcase = .false.
        IF( STRICT ) THEN
          IF( IFCHA(ICI,IWI) == 6 .OR. IFCHA(ICI,IWI) == 7 ) THEN
            IF( isBOG(INOZI) ) THEN
              IF( CARISC(ISC) == 'BOS' ) THEN
                BOGcase = .true.
              ENDIF
            ENDIF
          ENDIF
        ENDIF
        askROA = ALOCEL .AND. .NOT.BOGcase
!-- In strict conditions,
!-- there must be at least one S strip measurement in the multi-layer
        IF( STRICT .AND. ILINI <= 0 )                           CYCLE
!-- The multi-layer must be not too far from an "allowed" (Theta,Phi) cell
        IF( askROA .AND. IOKALA(F0I,ZZMOY,TTMOY,CP0000) /= 1 )  CYCLE
!
!-- One to two loops on S positions according to the presence
!-- of strips or not in the multi-layer (LSPASS = 1 or 2).
        NBDRIT0 = NBDRIT
        LSPAS0 = 1
        IF(ILINI <= 0) LSPAS0 = LSPAS0 + 1
        LSPAS1 = 2 - ISTRIC
        DO 370 LSPASS=LSPAS0,LSPAS1
!-- For Test beam (H8)  or  "Cosmic"  data,  skip search without strips when
!-- at least one straight track segment with strip(s) was previously found
          IF( AtCosm /= 0 .AND. LSPASS == 2 .AND. NBDRIT > NBDRIT0)  CYCLE
!-- Stores in JFLAGI and JFLAGO the flags of the hits
!-- (telling whether a hit has been used) before looping on S positions
          CALL UNJTUB(1,ICC(I),IWW(I),JLTUB(I), JFLAGI)
          CALL UNJTUB(1,ICC(I),IWW(I),JLTUB(I), JFLAGO)
          TTI(I) = SSBAN(NSBAN1(ICI,IWI)+1)
          DO 350 LSI=NSBAN0(ICI,IWI),NSBAN1(ICI,IWI)
            SSI(I) = SSBAN(LSI)
            LLI(I) = LSI
            IF( LSI == NSBAN1(ICI,IWI) ) LLI(I) = 0
            LSS = 1 + (LSI-NSBAN0(ICI,IWI)+1)/(NSBAN1(ICI,IWI)-NSBAN0(ICI,IWI)+1)
            IF(LSS /= LSPASS) CYCLE
!-- All S positions are examined with the same flags
            CALL LOJTUB(1,ICC(I),IWW(I),JLTUB(I), JFLAGI)
!
            SSfromROA = .false.
            CALL DROITU(askROA,STRICT,SSfromROA,BOGcase,ICC,IWW,LLI,SSI,TTI,MACADO,NBCADO, segcado)
!
!-- Stores output of DROITU
            DO 320 IK=1,NBCADO
              NBDRIT = NBDRIT + 1
              segm(NBDRIT) = segcado(IK)
              segm(NBDRIT)%errz = 999.d0
              segm(NBDRIT)%phi0 = F0I
              segm(NBDRIT)%alfa = 0.d0
              segm(NBDRIT)%jff  = INOFI
              DO J=1,2
!-- Flag (IWDRIT < 0) the straight track segments obtained without any S measurements
                IF( SSI(I) > 100000.D0 ) THEN
                  segm(NBDRIT)%iw(J) = - IWW(J)
                ELSE
                  segm(NBDRIT)%iw(J) =   IWW(J)
                ENDIF
                IF(ISC > NCOUBAS2) THEN
                  segm(NBDRIT)%zpos(J) = segcado(IK)%tpos(J)
                  segm(NBDRIT)%tpos(J) = segcado(IK)%zpos(J)
                ENDIF
              ENDDO
              segm(NBDRIT)%is(1:2) = 0
              segm(NBDRIT)%is(I)   = LSI
              IF( NBDRIT >= NDIM1 ) GO TO 600
320         ENDDO
!-- Updates JFLAGO by overwriting the flags of the hits that have been used
            CALL UPJTUB(1,ICC(I),IWW(I),JLTUB(I), JFLAGO)
            IF( NBCADO >= MACADO )      GO TO 360
350       ENDDO
!-- Flags the "used hits" after each loop on S positions
360       CALL LOJTUB(1,ICC(I),IWW(I),JLTUB(I), JFLAGO)
370     ENDDO
!
400   ENDDO
401 ENDDO
!----------------------------------------------------------------------------
!
!
600 CONTINUE
!
!-- Cleaning of segments
    CALL cleanSegment(NDIM1, NBDRIT, segm)
!
!-- Reset the flags of the hits to their nominal values
    DO I=1,2
      IC = ICC(I)
      DO IW=1,NCHAHI(IC)
        IMDT = ISTMDTV( IGEOMA(IC,IFCHA(IC,IW),IZCHA(IC,IW)) )
        DO IP=1,NLTUB(IMDT)
          ITDEB = NTUBH0(IC,IW,IP) + 1
          ITFIN = NTUBH0(IC,IW,IP) + NTUBHI(IC,IW,IP)
          KTUBHI(ITDEB:ITFIN) = IC*10000 + IW*10 + IP
        ENDDO
      ENDDO
    ENDDO
!
 END SUBROUTINE FINDRA
!
