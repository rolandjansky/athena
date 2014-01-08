!
!> Fill the list of dead channels
!
 SUBROUTINE FillDeadChannels
 USE M_MB_Control
 USE M_MB_MuGeom
 IMPLICIT NONE
 CHARACTER(3) :: CTYP
 INTEGER :: ICT, JTYP, JFF, JZZ, JOB
 INTEGER :: I, IC, INOF, INOZ, IGEOM
#if defined(ATLAS_STAND) || defined(ATLAS_PERSINT)
 INTEGER :: KTYP, KFF, KZZ, KOB
#endif
!
! outer BOF/BOG RPC are not cabled
!
  DO ICT=1,NTRIBA
    CALL IDBTRI(ICT,JTYP,JFF,JZZ,JOB)
    IF( JOB < 10 )                          CYCLE ! ... critere pour avoir la RPC externe ...
    CALL FGetStationType(JTYP, CTYP)
    IF( CTYP /= 'BOG' .AND. CTYP /= 'BOF' ) CYCLE
    isOffTrig(ICT) = .true.
    IF( MBPri >= 1 ) WRITE(*,'(" RPC/TGC station ",A3,3I3," is put OFF")') CTYP,JFF,JZZ,JOB
  ENDDO
!
#if defined(ATLAS_STAND) || defined(ATLAS_PERSINT)
! missing EES and EEL stations (done inside athena through dead channel service)
!
  DO IC=NCOUBA+1,NCOUCH
    DO INOF=1,8
      DO INOZ=1,NZM
        IGEOM = IGEOMA(IC,INOF,INOZ)
        IF( IGEOM <= 0 )                     CYCLE
        IF( isOffChav(IGEOM) )               CYCLE
        CALL IDBCHA(IC,INOF,INOZ,KTYP,KFF,KZZ,KOB)
        CALL FGetStationType(KTYP, CTYP)
        IF( CTYP(1:2) /= 'EE' )              CYCLE
        IF( CTYP == 'EEL' ) THEN
          IF( KFF == 1 )               isOffChav(IGEOM) = .true.
          IF( KFF == 2 )               isOffChav(IGEOM) = .true.
          IF( KFF == 4 )               isOffChav(IGEOM) = .true.
          IF( KFF == 5 )               isOffChav(IGEOM) = .true.
          IF( KFF == 6 .AND. KZZ < 0 ) isOffChav(IGEOM) = .true.
          IF( KFF == 8 .AND. KZZ > 0 ) isOffChav(IGEOM) = .true.
        ELSE IF( CTYP == 'EES' ) THEN
          isOffChav(IGEOM) = .true.
        ENDIF
        IF( MBPri >= 1 ) THEN
          IF( isOffChav(IGEOM) ) WRITE(*,'(" MDT/CSC station ",A3,3I3," is put OFF")') CTYP,KFF,KZZ,KOB
        ENDIF
      ENDDO
    ENDDO
  ENDDO
!
#endif
! now list of MDT/CSC/RPC/TGC stations which are OFF
!
  DO I=1,OffNber
    CALL GETIND(OffTyp(I),OffJff(I),OffJzz(I),OffJob(I), IC,INOF,INOZ, ICT)
    IF( ICT <= 0 ) THEN
      IF( IC*INOF*INOZ <= 0 ) CYCLE
      IGEOM = IGEOMA(IC,INOF,INOZ)
      IF( IGEOM <= 0 )        CYCLE
      isOffChav(IGEOM) = .true.
      IF( MBPri >= 1 ) THEN
        CALL FGetStationType(OffTyp(I), CTYP)
        WRITE(*,'(" MDT/CSC station ",A3,3I3," is put OFF")') CTYP,OffJff(I),OffJzz(I),OffJob(I)
      ENDIF
    ELSE IF( ICT <= NTRITO ) THEN
      isOffTrig(ICT) = .true.
      IF( MBPri >= 1 ) THEN
        CALL FGetStationType(OffTyp(I), CTYP)
        WRITE(*,'(" RPC/TGC station ",A3,3I3," is put OFF")') CTYP,OffJff(I),OffJzz(I),OffJob(I)
      ENDIF
    ENDIF
  ENDDO
!
! then add dead channels
  ndead = 0
  ndeadChan(:) =  0
  IF( DeadCscLay ) THEN
    CALL addDeadCscLayer(52,3,1,3,3)  ! the job value for CSC is 3 
    CALL addDeadCscLayer(52,5,1,3,1)
  ENDIF
!
! List dead CSC wire layers
  DO I=1,CSCWireLayerOffNber
    CALL addDeadCscLayer(                       &
                         CSCWireLayerOffTyp (I),&
                         CSCWireLayerOffJff (I),&
                         CSCWireLayerOffJzz (I),&
                         CSCWireLayerOffJob (I),&
                         CSCWireLayerOffJsl (I) &
                         )
!
! Add MDT dead tubes
  call FillDeadTubes
!
  IF( MBPri >= 1 ) CALL WiteDeadTubes
!
  ENDDO
!
 END SUBROUTINE FillDeadChannels 
!
 SUBROUTINE addDeadCscLayer(jtyp, jff, jzz, job, layer)
 USE M_MB_Control
 USE M_MB_MuGeom 
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: jtyp, jff, jzz, job, layer
 INTEGER :: ic,inof,inoz, ict, igeom
 CHARACTER(3) :: ctyp
!
   if( ndead >= NDEDMA )   return
   call GETIND(jtyp,jff,jzz,job, ic,inof,inoz, ict)
   if( ic*inof*inoz <= 0 ) return
   igeom = IGEOMA(ic,inof,inoz)
   if( igeom <= 0 )        return
   if( ndeadChan(igeom) == 0 ) ideadChan(igeom) = ndead
   ndead = ndead + 1
   ndeadChan(igeom) = ndeadChan(igeom) + 1
   ldeadChan(ndead) = layer
   IF( MBPri >= 1 ) THEN
     CALL FGetStationType(jtyp, ctyp)
     WRITE(*,'(" CSC     station ",A3,3I3," has dead layer ",I2)') ctyp,jff,jzz,job,layer
   ENDIF
!
 END SUBROUTINE addDeadCscLayer
!
