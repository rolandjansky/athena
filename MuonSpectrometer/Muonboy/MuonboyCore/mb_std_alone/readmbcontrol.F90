!
!> read MUONBOY control parameters when in standAlone
!> \author : J.F.Laporte   20/06/95
!
 SUBROUTINE ReadMBcontrol(LunCards)
 USE M_MB_Control
 USE M_MB_StandAlone
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: LunCards
 CHARACTER(110) :: CHA110
 CHARACTER(80)  :: ATAB(nchamax),CHAkey
 INTEGER        :: ITAB(101),Ilist,Ierr,I
 REAL(8)        :: FTAB(9)
!
   IF( LunCards < 1 ) RETURN
!
!>> If LunCards > 0 , read the data cards for MUONBOY on unit LunCards
   Ilist = 0
#if !defined(ATLAS_PERSINT)
   Nchaf = 0
#endif
   DO
     READ (LunCards,1000,END=10) CHA110
1000 FORMAT(A110)
     IF( Ilist > 0 ) PRINT 1001,TRIM(CHA110)
1001 FORMAT('ReadMBcontrol >>> ',A)
     CALL GENUCA(CHA110,1,0,0,  ATAB,ITAB,FTAB,Ierr)
     CHAkey = ATAB(1)
     IF( CHAkey == 'SEVE' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       XSeve = FTAB(1)
     ELSEIF( CHAkey == 'DTUB' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       DeTub = FTAB(1)
     ELSEIF( CHAkey == 'RTUB' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       ReTub = FTAB(1) / 1.D4
     ELSEIF( CHAkey == 'RNOT' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       ReTubNoT0 = FTAB(1) / 1.D4
     ELSEIF( CHAkey == 'ETUB' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       TubEf = FTAB(1) / 100.D0
     ELSEIF( CHAkey == 'ESTR' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       StrEf = FTAB(1) / 100.D0
     ELSEIF( CHAkey == 'ALST' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       AligStBa = FTAB(1) / 1.D4
       AligStEc = FTAB(1) / 1.D4
     ELSEIF( CHAkey == 'ALML' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       AerML = FTAB(1) / 1.D4
     ELSEIF( CHAkey == 'ALCS' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       AligStCs = FTAB(1) / 1.D4
     ELSEIF( CHAkey == 'FIXM' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       FixMo = FTAB(1)
     ELSEIF( CHAkey == 'BNUL' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       NoField = ( ITAB(1) == 1 )
     ELSEIF( CHAkey == 'MAST' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       MaxConStr = ITAB(1)
     ELSEIF( CHAkey == 'NMDT' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       MaxMdtTubes = ITAB(1)
     ELSEIF( CHAkey == 'ASEG' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       UseAllSeg = ( ITAB(1) == 1 )
     ELSEIF( CHAkey == 'MROA' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
        UseRoaMdt = ( ITAB(1) == 1 )
     ELSEIF( CHAkey == 'NOCO' ) THEN
        CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
        IF( Ierr > 1 ) GO TO 900
        NoCutOut = ( ITAB(1) == 1 )
     ELSEIF( CHAkey == 'MSMA' ) THEN
        CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
        IF( Ierr > 1 ) GO TO 900
        MaxNberMS = ITAB(1)
     ELSEIF( CHAkey == 'RMIN' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       RCHAMImi = FTAB(1)
     ELSEIF( CHAkey == 'RMAX' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       RCHAMAma = FTAB(1)
     ELSEIF( CHAkey == 'FASR' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       DsFastRT = FTAB(1)
     ELSEIF( CHAkey == 'FCUT' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       FinalCut = FTAB(1)
     ELSEIF( CHAkey == 'IPRI' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       MBPri = ITAB(1)
     ELSEIF( CHAkey == 'CHDI' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       MBChkDig = ITAB(1)
     ELSEIF( CHAkey == 'CHCS' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       MBChkCsc = ITAB(1)
     ELSEIF( CHAkey == 'DUMP' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       MBDum = ITAB(1)
     ELSEIF( CHAkey == 'ACCE' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       MBAccRec = MAX( 0 , MIN( 1 , MOD(ITAB(1),10) ) )
       MBAccTrk = MAX( 0 , MIN( 1 ,     ITAB(1)/10  ) )
     ELSEIF( CHAkey == 'ISOF' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       MBSto = MAX( -2 , MIN( 3 , ITAB(1) ) )
     ELSEIF( CHAkey == 'UROA' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       UseRoA = ITAB(1)
     ELSEIF( CHAkey == 'MDTN' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       MDTnu = MAX( -1 , MIN( 1 , ITAB(1) ) )
     ELSEIF( CHAkey == 'MADC' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       MdtAdc = FTAB(1)
     ELSEIF( CHAkey == 'SADC' ) THEN
       CALL GENUCA(CHA110,1,0,1,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       SegAdc = FTAB(1)
     ELSEIF( CHAkey == 'CSCP' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       CSCpa = MAX( 0 , MIN( 1 , ITAB(1) ) )
     ELSEIF( CHAkey == 'MELO' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       MEloss = ITAB(1)
     ELSEIF( CHAkey == 'SBUG' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       SimBug = ITAB(1)
     ELSEIF( CHAkey == 'SIMU' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       IsSimu = ITAB(1)
     ELSEIF( CHAkey == 'ATCO' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       AtCosm = ITAB(1) 
     ELSEIF( CHAkey == 'LGEO' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       LunGeo = ITAB(1)
     ELSEIF( CHAkey == 'LDSE' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       LunDumSeg = ITAB(1)
     ELSEIF( CHAkey == 'LDTR' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       LunDumTra = ITAB(1)
     ELSEIF( CHAkey == 'EVTR' ) THEN
       CALL GENUCA(CHA110,1,3,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       MBevt = ITAB(1)
       MBev0 = ITAB(2)
       MBev1 = ITAB(3)
     ELSEIF( CHAkey == 'EVTS' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       MBidn = ITAB(1)
       CALL GENUCA(CHA110,1,MBidn+1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       DO I=1,MBidn
         MBide(I) = ITAB(I+1)
       ENDDO
     ELSEIF( CHAkey == 'LIST' ) THEN
       Ilist = 1
       PRINT 1001,TRIM(CHA110)
     ELSEIF( CHAkey == 'EXCL' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       ExternalClust = ITAB(1)
     ELSEIF( CHAkey == 'CHKT' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       CheckTracking = ITAB(1) >= 1
#if !defined(ATLAS_PERSINT)
     ELSEIF( CHAkey == 'CHAF' ) THEN
       ATAB(:) = '    '
       CALL GENUCA(CHA110,nchamax,0,0,  ATAB,ITAB,FTAB,Ierr)
       DO I=2,nchamax
         IF( ATAB(I) == '    ' ) EXIT
         Nchaf = Nchaf + 1
         Cchaf(Nchaf) = ATAB(I)
       ENDDO
#endif
     ELSEIF( CHAkey == 'DERI' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       checkBderiv = ITAB(1) >= 1
     ELSEIF( CHAkey == 'RIRT' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       RimRTSvc = ITAB(1)
     ELSEIF( CHAkey == 'T0FI' ) THEN
       CALL GENUCA(CHA110,1,1,0,  ATAB,ITAB,FTAB,Ierr)
       IF( Ierr > 1 ) GO TO 900
       DoT0fit   = ITAB(1) >= 1
       DoT0Print = ITAB(1) >= 2
     ELSEIF( CHAkey == 'STOP' ) THEN
       GO TO 10
     ELSEIF( CHAkey == 'FNOI' .OR. CHAkey == 'DED2' .OR. CHAkey == 'EFF2' &
                              .OR. CHAkey == 'RES2' .OR. CHAkey == 'AFT2' ) THEN
       WRITE(*,'(" In ReadMBcontrol, data card : ",A4," is obsolete ")') CHAkey
!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     ELSEIF( CHAkey(1:1) /= 'C' .AND. CHAkey(1:1) /= 'c' ) THEN
       GO TO 900
     ENDIF
   ENDDO
10 CONTINUE
   RETURN
!
900  PRINT 2000,TRIM(CHA110)
2000 FORMAT(/' In ReadMBcontrol, data card :',/,A,/' is un-understandable  =====>  STOP !')
   STOP
 END SUBROUTINE ReadMBcontrol
!
