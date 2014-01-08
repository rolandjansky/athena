!
!> Create links between trigger (RPC and TGC) stations and  MDT multilayers.
!> It is called from amdcox.F90
!
 SUBROUTINE createTrigLinks
 USE M_MB_MuGeom
 IMPLICIT NONE
 CHARACTER(3) :: CTYP
 REAL(8) :: TOLZ,TOLS, TTT, ZZI, DZZ, ZZZ, EEE, SS0, DSS, SSS, RRR
 REAL(8) :: DTT, DSBUF,DZBUF, TTI, COF, SIF
 INTEGER :: NBPAZ,NBPAS, IC, ISC, ILOHI, INOF,INOFM1,INOFP1, INOZ, IGEOM
 INTEGER :: KORDRE,NORDRE(NTRIMA), ICT, JTYP,JFF,JZZ,JOB, IPAZ
 INTEGER :: KO, ILIN, L, IPAS, IRPC, IDEBUT
 LOGICAL :: normalLink
!
!>> Create links to trigger chambers (RPCs) in barrel MDT multilayers
    TOLZ = 2.0000D0
    TOLS = 0.0000D0
    NBPAZ = 5
    NBPAS = 4
    DO 200 IC=1,NCOUBA
      ISC   = (IC+1)/2
      ILOHI = IC + 2 - 2*ISC
!>> In the general case for Atlas, create links for BM. and BO. stations only !
      IF( CARISC(ISC)(2:2) /= 'M' .AND. CARISC(ISC)(2:2) /= 'O' )  CYCLE
      normalLink = .true.
      DO 180 INOF=1,8
        INOFM1 = MOD(INOF+6,8) + 1
        INOFP1 = MOD(INOF+8,8) + 1
        DO 160 INOZ=1,NZM
          IGEOM = IGEOMA(IC,INOF,INOZ)
          IF( IGEOM <= 0 )                  CYCLE
          TTT = TCECHAV(IGEOM)
          KORDRE = 0
          DO ICT=1,NTRIBA  !<-------------------------------------------------+
            CALL IDBTRI(ICT, JTYP,JFF,JZZ,JOB)                                !
!>> Jphi mdt's <---> Jphi or Jphi-1 or Jphi+1 rpc's                           !
!           IF( JFF /= INOFM1 .AND. JFF /= INOF .AND. JFF /= INOFP1 )  CYCLE  !
            IF( JFF /= INOF )                                          CYCLE  !
!>> General case for Atlas : Bx. mdt's <---> Bx. rpc's                        !
            IF( ISCISC(JTYP) /= ISC )                                  CYCLE  !
            IF( CARISC(ISC) /= 'BOS' .OR. ( JFF /= 6 .AND. JFF /= 7 ) ) THEN  ! do not apply that on BOF/BOG
!>> Lower mdt's <---> Lower rpc's                                             ! because of missing outer RPC
              IF( ILOHI == 1 .AND. TCETRI(ICT) > TTT )                 CYCLE  ! (and also holes in BMF)
!>> Upper mdt's <---> Upper rpc's                                             !
              IF( ILOHI == 2 .AND. TCETRI(ICT) < TTT )                 CYCLE  !
            ENDIF                                                             !
            KORDRE = KORDRE + 1                                               !
            NORDRE(KORDRE) = ICT                                              !
          ENDDO  !>-----------------------------------------------------------+
          ZZI = ZCECHAV(IGEOM) -ZLOCHAV(IGEOM)/2.D0 -TOLZ
          DZZ = (ZLOCHAV(IGEOM)+2.D0*TOLZ) / NBPAZ
          DO 140 IPAZ=0,NBPAZ
            ZZZ = ZZI + IPAZ*DZZ
            EEE = (ZZZ-ZZI)*ZLOCHAV_inv(IGEOM)
            SS0 =   SNICHAV(IGEOM) - TOLS       + EEE*(SNACHAV(IGEOM)-SNICHAV(IGEOM))
            DSS = ( SPICHAV(IGEOM) + TOLS - SS0 + EEE*(SPACHAV(IGEOM)-SPICHAV(IGEOM)) ) / NBPAS
            DO 130 IPAS=0,NBPAS
              SSS = SS0 + IPAS*DSS
              RRR = SQRT( TTT**2 + SSS**2 )
              COF = ( COSCHAV(IGEOM)*TTT - SINCHAV(IGEOM)*SSS) / RRR
              SIF = ( SINCHAV(IGEOM)*TTT + COSCHAV(IGEOM)*SSS) / RRR
              DO KO=1,KORDRE
                CALL addTrigLink(NORDRE(KO),IGEOM,ZZZ,RRR,COF,SIF,normalLink)
              ENDDO
130         ENDDO
140       ENDDO
160     ENDDO
180   ENDDO
200 ENDDO
!
!>> Create links to trigger chambers (TGCs) in end-cap MDT multilayers
    TOLZ = 3.0000D0
    TOLS = 1.5000D0
    NBPAZ = 10
    NBPAS = 10
    DO 400 IC=NCOUBA+1,NCOUCH
      ISC   = (IC+1)/2
      ILOHI = IC + 2 - 2*ISC
      IF( CARISC(ISC)(1:2) /= 'EI' .AND. CARISC(ISC)(1:2) /= 'EM' .AND. CARISC(ISC)(1:2) /= 'EO' )  CYCLE
      normalLink = .true.
      IF( CARISC(ISC)(1:2) == 'EO' ) normalLink = .false.
      IDEBUT = NTRIBA + 1
      IF( CARISC(ISC)(1:2) == 'EM' )  IDEBUT = 1
      DO 380 INOF=1,8
        INOFM1 = MOD(INOF+6,8) + 1
        INOFP1 = MOD(INOF+8,8) + 1
        DO 360 INOZ=1,NZM
          IGEOM = IGEOMA(IC,INOF,INOZ)
          IF( IGEOM <= 0 )               CYCLE
          KORDRE = 0
          ZZZ = TCECHAV(IGEOM)
          TTI = ZCECHAV(IGEOM) - ZLOCHAV(IGEOM)/2.D0 - TOLZ
          DTT =(ZLOCHAV(IGEOM)+2.D0*TOLZ) / NBPAZ
          DO ICT=IDEBUT,NTRITO
            IF( ICT <= NTRIBA ) THEN
              IF( ZZZ*ZCETRI(ICT) < 0.d0 )                            CYCLE
            ELSE
              IF( ZZZ*TCETRI(ICT) < 0.d0 )                            CYCLE
            ENDIF
            CALL IDBTRI(ICT, JTYP,JFF,JZZ,JOB)
            IF( CARISC(ISC)(3:3) == 'L' ) THEN
              IF( JFF /= INOF .AND. JFF /= INOFM1 ) CYCLE
            ELSE
              IF( JFF /= INOF .AND. JFF /= INOFP1 ) CYCLE
            ENDIF
            CALL FGetStationType(JTYP,CTYP)
            IF( CARISC(ISC)(1:2) == 'EI' ) THEN
              IF( ILOHI == 1 .AND. CTYP(1:2) == 'T4')   GO TO 310
            ELSE IF( CARISC(ISC)(1:2) == 'EM' ) THEN
              IF( ILOHI == 1 .AND. CTYP(1:2) == 'BO')   GO TO 310
              IF( ILOHI == 1 .AND. CTYP(1:2) == 'T1')   GO TO 310
              IF( ILOHI == 2 .AND. CTYP(1:2) == 'T2')   GO TO 310
              IF( ILOHI == 2 .AND. CTYP(1:2) == 'T3')   GO TO 310
            ELSE IF( CARISC(ISC)(1:2) == 'EO' ) THEN
              IF( ILOHI == 1 .AND. CTYP(1:2) == 'T2')   GO TO 310
              IF( ILOHI == 1 .AND. CTYP(1:2) == 'T3')   GO TO 310
            ENDIF
            CYCLE
310         KORDRE = KORDRE + 1
            NORDRE(KORDRE) = ICT
          ENDDO
          DO 340 IPAZ=0,NBPAZ
            TTT = TTI + IPAZ*DTT
            EEE = (TTT-TTI)*ZLOCHAV_inv(IGEOM)
            SS0 =   SNICHAV(IGEOM) - TOLS       + EEE*(SNACHAV(IGEOM)-SNICHAV(IGEOM))
            DSS = ( SPICHAV(IGEOM) + TOLS - SS0 + EEE*(SPACHAV(IGEOM)-SPICHAV(IGEOM)) ) / NBPAS
            DO 330 IPAS=0,NBPAS
              SSS = SS0 + IPAS*DSS
              RRR = SQRT( TTT**2 + SSS**2 )
              COF = ( COSCHAV(IGEOM)*TTT - SINCHAV(IGEOM)*SSS) / RRR
              SIF = ( SINCHAV(IGEOM)*TTT + COSCHAV(IGEOM)*SSS) / RRR
              DO KO=1,KORDRE
                CALL addTrigLink(NORDRE(KO),IGEOM,ZZZ,RRR,COF,SIF,normalLink)
              ENDDO
330         ENDDO
340       ENDDO
360     ENDDO
380   ENDDO
400 ENDDO
!
!>> Compute averaged "strips" resolution for each link
    DO ILIN=1,NLINTO
      DSBUF = 0.D0
      DZBUF = 0.D0
      DO L=1,NBLINK(ILIN)
        ICT  = ICTRIG(L,ILIN)
        IRPC = IRPTGC(ICT)
        DSBUF = DSBUF + DSBAND(IRPC)
        DZBUF = DZBUF + DZBAND(IRPC)
      ENDDO
      DSLINK(ILIN) = DSBUF / (2.D0*NBLINK(ILIN))
      DZLINK(ILIN) = DZBUF / (2.D0*NBLINK(ILIN))
    ENDDO
!
 END SUBROUTINE createTrigLinks
!
!
!> add a new link between a MDT (index IGEOM) and a RPC/TGC (index ICT)
!
 SUBROUTINE addTrigLink(ICT,IGEOM,ZZZ,RRR,COFI,SIFI,normalLink)
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(INOUT) :: ICT
 INTEGER, INTENT(IN)    :: IGEOM
 REAL(8), INTENT(IN)    :: ZZZ, RRR, COFI, SIFI
 LOGICAL, INTENT(IN)    :: normalLink
 INTEGER :: ILIN, L
 REAL(8) :: Z, E, T, S, SMIN,SMAX, COSDFI, SINDFI
!
    IF( ICT <= 0 )                   RETURN
    COSDFI = COFI*COSTRI(ICT) + SIFI*SINTRI(ICT)
    IF( COSDFI <= 0.D0 )             RETURN
    IF( ICT <= NTRIBA ) THEN
      Z = TCETRI(ICT) * ZZZ / COSDFI / RRR
      E = (Z-ZCETRI(ICT)) / ZLOTRI(ICT)
      T = TCETRI(ICT)
    ELSE
      T = TCETRI(ICT) * RRR * COSDFI / ZZZ
      IF( T < 0.D0 )                 RETURN
      E = (T-ZCETRI(ICT)) / ZLOTRI(ICT)
    ENDIF
    IF( ABS(E) > 0.5D0 )             RETURN
    SINDFI = SIFI*COSTRI(ICT) - COFI*SINTRI(ICT)
    S = T * SINDFI / COSDFI
    E = E + 0.5D0
    SMIN = SNITRI(ICT) + E * (SNATRI(ICT)-SNITRI(ICT))
    SMAX = SPITRI(ICT) + E * (SPATRI(ICT)-SPITRI(ICT))
    IF( S < SMIN .OR. S > SMAX )     RETURN
    IF( normalLink ) THEN
      IF( ISTLINV(IGEOM) <= 0 ) THEN
        NLINTO = NLINTO + 1
        IF( NLINTO > N1LIMA ) THEN
          PRINT 101,NLINTO
          STOP
        ENDIF
        ILIN = NLINTO
        ISTLINV(IGEOM) = ILIN
      ELSE
        ILIN = ISTLINV(IGEOM)
      ENDIF
      DO L=1,NBLINK(ILIN)
        IF( ICTRIG(L,ILIN) == ICT )    RETURN
      ENDDO
      NBLINK(ILIN) = NBLINK(ILIN) + 1
      IF( NBLINK(ILIN) > N2LIMA ) THEN
        PRINT 102,NBLINK(ILIN)
        STOP
      ENDIF
      ICTRIG(NBLINK(ILIN),ILIN) = ICT
    ELSE
      IF( JSTLINV(IGEOM) <= 0 ) THEN
        MLINTO = MLINTO + 1
        IF( MLINTO > M1LIMA ) THEN
          PRINT 101,MLINTO
          STOP
        ENDIF
        ILIN = MLINTO
        JSTLINV(IGEOM) = ILIN
      ELSE
        ILIN = JSTLINV(IGEOM)
      ENDIF
      DO L=1,MBLINK(ILIN)
        IF( JCTRIG(L,ILIN) == ICT )    RETURN
      ENDDO
      MBLINK(ILIN) = MBLINK(ILIN) + 1
      IF( MBLINK(ILIN) > M2LIMA ) THEN
        PRINT 102,MBLINK(ILIN)
        STOP
      ENDIF
      JCTRIG(MBLINK(ILIN),ILIN) = ICT
    ENDIF
    ICT = 0   ! prevent this trigger station to be tested again
!
101 FORMAT(/' Attempt to create a total number of links to',                    &
            ' trigger chambers >',I5,' ======> STOP in AMDCOX of MUONBOY !!!')
102 FORMAT(/' Attempt to create a local number of links to',                    &
            ' trigger chambers >',I5,' ======> STOP in AMDCOX of MUONBOY !!!')
!
 END SUBROUTINE addTrigLink
!
