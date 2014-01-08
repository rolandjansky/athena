!
!> Dump the geometry as used by Muonboy
!
 SUBROUTINE printGeom(CARTYP)
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_General_CONSTANTS, ONLY : PiS180
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN) :: CARTYP(*)
 INTEGER :: JMDT, JRPC, L, IC, IPASSF, INOF, IPASSZ, INOZ, ISC, ILOHI, IGEOM
 INTEGER :: JSTCUT, LL, K, JTYPO, JFFO, ICT, ILIN , JTYP, JFF, JZZ, JOB
 INTEGER :: KTYP, KFF, KZZ, KOB
!
    IF( LunGeo <= 0 )  RETURN
    IF( LunGeo /= 6 )  OPEN( unit=LunGeo, status='unknown' )
!
    WRITE(LunGeo,*)
    WRITE(LunGeo,900)
    DO JMDT=1,NMDTMA
      IF( NLTUB(JMDT) <= 0 )  CYCLE
      WRITE(LunGeo,901) JMDT,JrtTUB(JMDT),NLTUB(JMDT),PASTUB(JMDT),RAYTUB(JMDT),DEDTUB(JMDT),RESTUB(JMDT)           &
                            ,TLTUB(JMDT,1,1:2),OLTUB(JMDT,1,1:2),DRITUB(JMDT),RADTUB(JMDT),ACUTUB(JMDT)
      DO L=2,NLTUB(JMDT)
        WRITE(LunGeo,902) TLTUB(JMDT,L,1:2),OLTUB(JMDT,L,1:2)
      ENDDO
    ENDDO
    WRITE(LunGeo,*)
    WRITE(LunGeo,*)
!
    WRITE(LunGeo,907)
    DO JRPC=0,NRPCMA
      IF(JRPC >= 1 .AND. N2BAND(JRPC) <= 0)  CYCLE
      WRITE(LunGeo,908) JRPC,N1BAND(JRPC),N2BAND(JRPC)-N1BAND(JRPC),DSBAND(JRPC),DZBAND(JRPC),TBAND(JRPC,1)    &
                            ,OBAND (JRPC,1,1),OBAND (JRPC,1,2),ISZBAND(JRPC,1,1),ISZBAND(JRPC,1,2)
      DO L=2,N2BAND(JRPC)
        WRITE(LunGeo,909) TBAND(JRPC,L),OBAND(JRPC,L,1),OBAND(JRPC,L,2),ISZBAND(JRPC,L,1),ISZBAND(JRPC,L,2)
      ENDDO
    ENDDO
!
    WRITE(LunGeo,915) NGEOMT
    DO IC=1,NCOCSC
      IPASSF = 0
      DO INOF=1,8
        IPASSZ = 0
        DO INOZ=1,NZM
          IF( IGEOMA(IC,INOF,INOZ) <= 0 )  CYCLE
          CALL IDBCHA(IC,INOF,INOZ, KTYP,KFF,KZZ,KOB)
          IPASSF = IPASSF + 1
          IPASSZ = IPASSZ + 1
          IF(IPASSF == 1 .AND. MOD(IC,2) /= 0)  WRITE(LunGeo,905)
          IF(IPASSZ == 1)  WRITE(LunGeo,*)
          ISC = (IC+1)/2
          ILOHI = IC + 2 - 2*ISC
          IGEOM = IGEOMA(IC,INOF,INOZ)
          WRITE(LunGeo,904) IC,CARISC(ISC),CALOHI(ILOHI),INOF,INOZ,IGEOM,ISTMDTV(IGEOM)                       &
                              ,ISTLINV(IGEOM),ISTCUTV(IGEOM),ItransCHA(IGEOM),ISingle(IGEOM)                  &
                              ,FCECHAV(IGEOM),GCECHAV(IGEOM),TCECHAV(IGEOM),ZCECHAV(IGEOM),ZLOCHAV(IGEOM)     &
                              ,SNICHAV(IGEOM),SPICHAV(IGEOM),SNACHAV(IGEOM),SPACHAV(IGEOM)                    &
                              ,CARTYP(KTYP),KFF,KZZ,KOB,STACHAV(IGEOM)
          JSTCUT = ISTCUTV(IGEOM)
          IF(JSTCUT > 0) THEN
            LL = 1
            DO K=1,NBCUT(JSTCUT)
              LL = LL + 1
              IF(LL <= 1) THEN
                WRITE(LunGeo,906) K,SSCUT(1:4,JSTCUT,K),CARTYP(KTYP),KFF,KZZ,KOB
              ELSE
                WRITE(LunGeo,906) K,SSCUT(1:4,JSTCUT,K)
              ENDIF
              LL = LL + 1
              IF(LL <= 1) THEN
                WRITE(LunGeo,916) ANCUT(JSTCUT,K)/PiS180,ZTCUT(1:4,JSTCUT,K),CARTYP(KTYP),KFF,KZZ,KOB
              ELSE
                WRITE(LunGeo,916) ANCUT(JSTCUT,K)/PiS180,ZTCUT(1:4,JSTCUT,K)
              ENDIF
            ENDDO
            LL = LL + 1
            IF(LL <= 1) WRITE(LunGeo,914) CARTYP(KTYP),KFF,KZZ,KOB
          ENDIF
        ENDDO
      ENDDO
    ENDDO
!
    WRITE(LunGeo,*)
    WRITE(LunGeo,*)
    JTYPO = 0
    JFFO  = 0
    DO ICT=1,NTRITO
      CALL IDBTRI(ICT, JTYP,JFF,JZZ,JOB)
      IF(JTYP <= 0)    CYCLE
      IF(JTYP /= JTYPO .OR. JFF /= JFFO) WRITE(LunGeo,910)
      JTYPO = JTYP
      JFFO  = JFF
      WRITE(LunGeo,911) ICT,CARTYP(JTYP),JFF,JZZ,JOB,IRPTGC(ICT),ITRCUT(ICT),FCETRI(ICT),TCETRI(ICT)   &
                           ,ZCETRI(ICT),ZLOTRI(ICT),SNITRI(ICT),SPITRI(ICT),SNATRI(ICT),SPATRI(ICT)
    ENDDO
!
    WRITE(LunGeo,*)
    WRITE(LunGeo,*)
    WRITE(LunGeo,912)
    DO ILIN=0,NLINTO
      WRITE(LunGeo,913) ILIN,NBLINK(ILIN),DSLINK(ILIN),DZLINK(ILIN),(ICTRIG(L,ILIN),L=1,NBLINK(ILIN))
    ENDDO
!
    IF( LunGeo /= 6 )  CLOSE(LunGeo)
!
900 FORMAT(/'  Imdt : RT Ntub   PAStub    RAYtub    DEDtub',                     &
            '    REStub     Tltub               Oltub              DRItub',      &
            '    RADtub    ACUtub  RelRTtub')
901 FORMAT(/I5,I6,I4,2F10.3,F10.1,5F10.3,F10.1,2F10.4)
902 FORMAT(     55X,            4F10.3)
904 FORMAT(I3,2X,A3,1X,A5,I4,I5,I6,I4,I5,I5,I5,I5,2F10.6,2F9.3,5F8.3,3X,A3,I3,2I4,F8.1)
914 FORMAT(109X,A3,I3,2I4)
915 FORMAT(//'              NgeomT = ',I6)
905 FORMAT(/' IC    Type    InoF InoZ Igeo Imdt Ilin Icut Iszt Isin   ',       &
            'FCEcha    GCEcha   TCEcha   ZCEcha  ZLOcha  SNIcha  SPIcha  ',    &
            'SNAcha  SPAcha  Jtyp Jff Jzz Job  STAcha')
906 FORMAT(38X,I5,42X,          'SS :',4F8.3,3X,A3,I3,2I4)
916 FORMAT(38X,'Angle',F8.2,34X,'ZT :',4F8.2,3X,A3,I3,2I4)
907 FORMAT(/'  Irpc :   NSband N','Zband  DSband    DZband','     Tband     Oband              ISZband')
908 FORMAT(/I5,I10,I7,5F10.3,I10,I6)
909 FORMAT( 42X,      3F10.3,I10,I6)
910 FORMAT(/' ICT  Type   Jff  Jzz  Job      Irpc Icut   FCEtri   TCEtri',     &
            '   ZCEtri   ZLOtri   SNItri   SPItri   SNAtri   SPAtri'/)
911 FORMAT(I4,3X,A3,3I5,5X,2I5,F10.6,7F9.3)
912 FORMAT(/' Ilin  NbLin  dS    dZ      ICTs ......'/)
913 FORMAT(I5,I6,2F6.2,2X,20I5/25X,20I5/25X,20I5)
!
 END SUBROUTINE printGeom
!
