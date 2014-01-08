!
 SUBROUTINE readTags(LUNIN)
 USE M_MB_Control
 USE M_MB_EventInfo, ONLY : nevtlu
 USE M_MB_Mat_TagInfo
 USE M_MB_Mat_AGDD2GeoSwitches
 USE M_MB_CondDB
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: LUNIN
 CHARACTER(16)  :: AmdcFileName
 CHARACTER(110) :: CAR110
 CHARACTER(3)   :: CAR3
 CHARACTER(80)  :: ATAB(9)
 INTEGER        :: ITAB(9)
 REAL(8)        :: FTAB(9)
 INTEGER        :: IERROR, i, simset
 CHARACTER(1)   :: CarGeo
!
    ncdbtag = 0
    simset  = 0
    REWIND LUNIN
100 READ(LUNIN,'(A110)') CAR110
    CAR3 = CAR110(1:3)
!
    IF( CAR3 == 'GEO' ) THEN  ! check geometry used by Athena
      CALL GetAmdcName(AmdcFileName)
      CarGeo = ' '
      CALL GENUCA(CAR110,2,0,0,  ATAB,ITAB,FTAB,IERROR)
      IF( ATAB(2)(1:12) == 'ATLAS-CSC-00' )  CarGeo = 'R'
      IF( ATAB(2)(1:12) == 'ATLAS-DC3-03' )  CarGeo = 'R'
      IF( ATAB(2)(1:10) == 'ATLAS-Rome'   )  CarGeo = 'Q'
      IF( ATAB(2)(1:9)  == 'ATLAS-CTB'    )  CarGeo = 'H'
      IF( ATAB(2)(1:2)  == 'X5'           )  CarGeo = 'X'
      IF( CarGeo /= ' ' .AND. CarGeo /= AmdcFileName(1:1) ) THEN
        WRITE(*,*) ' Warning !!! you are not using the right geometry !!! '
        WRITE(*,*) ' This event was processed inside Athena with ',TRIM(ATAB(2))
        WRITE(*,*) ' So you should use a layout ',CarGeo,', while you use ',TRIM(AmdcFileName)
        IF( CarGeo == 'H' .OR. AmdcFileName(1:1) == 'H' )  STOP
      ENDIF
      RpcTimeSchema = 1
      IF( ATAB(2)(1: 9) == 'ATLAS-CSC'   ) RpcTimeSchema = 2
      IF( ATAB(2)(1:12) == 'ATLAS-GEO-02') RpcTimeSchema = 2
      GO TO 100
    ELSE IF( CAR3 == 'SIM' ) THEN
      CALL GENUCA(CAR110,1,1,0,  ATAB,ITAB,FTAB,IERROR)
      IF( ITAB(1) == 0 .OR. ITAB(1) == 1 ) THEN
        IsSimu = ITAB(1)
        simset = 1
      ELSE
        write(*,*) ' Warning !!! unexpected value for SIM ',ITAB(1)
      ENDIF
    ELSE IF( CAR3 == 'TAG' ) THEN
      CALL GENUCA(CAR110,3,0,0,  ATAB,ITAB,FTAB,IERROR)
      IF( IERROR == 0 .AND. TRIM(ATAB(1)) == 'TAGINFO' ) THEN
        NewTagInfoMat = 1
        NewTagInfoRpc = 1
        IF( NberTagInfo < MaxTagInfo ) THEN
          NberTagInfo = NberTagInfo + 1
          ChaTagInfoA(NberTagInfo) = TRIM(ATAB(2))
          ChaTagInfoB(NberTagInfo) = TRIM(ATAB(3))
        ENDIF
        GO TO 100
      ENDIF
    ELSE IF( CAR3 == 'A2G' ) THEN
      CALL GENUCA(CAR110,1,1,0,ATAB,ITAB,FTAB,IERROR)
      IF( IERROR == 0 .AND. TRIM(ATAB(1)) == 'A2G' ) THEN
        NewAGDD2GeoSwitches  = 1
        NberAGDD2GeoSwitches = ITAB(1)
        DO i=1,NberAGDD2GeoSwitches
          READ(LUNIN,'(A110)') CAR110
          CALL GENUCA(CAR110,2,1,0,ATAB,ITAB,FTAB,IERROR)
          IF( IERROR /= 0 .OR. TRIM(ATAB(1)) /= 'A2G' ) THEN
            ChaAGDD2GeoSwitches(i) = ''
          ELSE
            ChaAGDD2GeoSwitches(i) = TRIM(ATAB(2))
          ENDIF
        ENDDO       
      ENDIF
      GO TO 100
    ELSE IF( CAR3 == 'CDB' ) THEN
      CALL GENUCA(CAR110,4,0,0,  ATAB,ITAB,FTAB,IERROR)
      IF( IERROR == 0 .AND. TRIM(ATAB(2)) == 'TAG' ) THEN
        IF( ncdbtag < ncdbmax ) THEN
          ncdbtag = ncdbtag + 1
          cdbfoldername(ncdbtag) = TRIM(ATAB(3))
          cdbtagname(ncdbtag)    = TRIM(ATAB(4))
          if (cdbfoldername(ncdbtag)(1:11) == '/MUONALIGN/' ) then
            ncdbalitag = ncdbalitag + 1
            cdbalifoldername(ncdbalitag) = TRIM(ATAB(3))
            cdbalitagname(ncdbalitag)    = TRIM(ATAB(4))
          endif
        ELSE
          WRITE(*,*) ' Warning !!! ncdbmax too small !!! increase it'
        ENDIF
      ENDIF
      GO TO 100
    ENDIF

!
    REWIND LUNIN
    nevtlu = 0
#if defined(ATLAS_PERSINT)
    IF( simset == 0 ) CALL MboyToPersint('SIMNOTSET')
#endif
!
 END SUBROUTINE readTags
!
