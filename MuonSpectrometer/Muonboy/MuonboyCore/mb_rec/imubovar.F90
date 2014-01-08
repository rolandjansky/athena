!
!> Return the values of MuonBoy INTEGER parameters and variables
!> \author M.Virchaux
!
 INTEGER FUNCTION IMuboVar( Name, I1,I2,I3 )
 IMPLICIT NONE
 CHARACTER (len=*), INTENT(IN) :: Name
 INTEGER,           INTENT(IN) :: I1,I2,I3
 INTEGER, EXTERNAL    :: IMuboVarBaz
 INTEGER :: Istop, IStatus
!
      Istop = 1
      IMuboVar = IMuboVarBaz( Istop, IStatus, Name, I1,I2,I3 )
!
 END FUNCTION IMuboVar
!
!
!> Return the values of MuonBoy INTEGER parameters and variables
!> \author M.Virchaux
!
 INTEGER FUNCTION IMuboVarBaz( Istop, IStatus, Name, I1,I2,I3 )
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_Reco_Tracks
 USE M_MB_Reco_Segments
 USE M_MB_COMUSC
 USE M_MB_Fit
 USE M_MB_MuGeom
 USE M_MB_EventInfo
 USE M_MB_CODRAT
 IMPLICIT NONE
 INTEGER,           INTENT(IN)  :: Istop
 INTEGER,           INTENT(OUT) :: IStatus
 CHARACTER (len=*), INTENT(IN)  :: Name
 INTEGER,           INTENT(IN)  :: I1,I2,I3
 INTEGER :: Iw1,Iw2,NbMl
!
      IStatus = 1
!
      SELECT CASE( Name )
      CASE('NDRAMA')
         IMuboVarBaz = NDRAMA
         GO TO 900
      CASE('NMATMA')
         IMuboVarBaz = NMATMA
         GO TO 900
      CASE('NBDRAT')
         IMuboVarBaz = NBDRAT(I1)
         GO TO 910
      CASE('IWDRAT')
         IMuboVarBaz = IWDRAT(I1,I2,I3)
         GO TO 930
      CASE('JFDRAT')
         IMuboVarBaz = JFDRAT(I1,I2)
         GO TO 920
      CASE('KOUNTERDRA')
         IMuboVarBaz = KOUNTERDRA(I1,I2)
         GO TO 920
      CASE('NBFIXE' )
         IMuboVarBaz = NBFIXE
         GO TO 900
      CASE('NMATTR' )
         IMuboVarBaz = NMATTR
         GO TO 900
      CASE('NCOUBAS2' )
         IMuboVarBaz = NCOUBAS2
         GO TO 900
      CASE('timestamp' )
         IMuboVarBaz = timestamp
         GO TO 900
      CASE('NLAYMA')
         IMuboVarBaz = NLAYMA
         GO TO 900
      CASE('NBCAN')
         IMuboVarBaz = NBCAN
         GO TO 900
      CASE('NFICAN')
         IMuboVarBaz = NFICAN
         GO TO 900
      CASE('IDFIXE')
         IMuboVarBaz = IDFIXE(I1)
         GO TO 910
      CASE('ISCFIXE')
         IMuboVarBaz = ISCFIXE(I1)
         GO TO 910
      CASE('ISTMDTV')
         IMuboVarBaz = ISTMDTV(I1)
         GO TO 910
      CASE('ISCISC')
         IMuboVarBaz = ISCISC(I1)
         GO TO 910
      CASE('NDFCAN')
         IMuboVarBaz = NDFCAN(I1)
         GO TO 910
      CASE('FLGCAN')
         IMuboVarBaz = FLGCAN(I1)
         GO TO 910
      CASE('ISECAN')
         IMuboVarBaz = ISECAN(I1)
         GO TO 910
      CASE('ITICAN')
         IMuboVarBaz = ITICAN(I1,I2)
         GO TO 920
      CASE('ITXCAN')
         IMuboVarBaz = ITXCAN(I1,I2)
         GO TO 920
      CASE('NZXCAN')
         IMuboVarBaz = NZXCAN(I1)
         GO TO 910
      CASE('IZXCAN')
         IMuboVarBaz = IZXCAN(I1,I2)
         GO TO 920
      CASE('IZXCANext')
         IMuboVarBaz = ITUBHI(IZXCAN(I1,I2))
         GO TO 920
      CASE('NSXCAN')
         IMuboVarBaz = NSXCAN(I1)
         GO TO 910
      CASE('ISXCAN')
         IMuboVarBaz = ISXCAN(I1,I2)
         GO TO 920
      CASE('ISXCANext')
         IMuboVarBaz = IBANHI(ISXCAN(I1,I2))
         GO TO 920
      CASE('NZOCAN')
         IMuboVarBaz = NZOCAN(I1)
         GO TO 910
      CASE('NHXCAN')
         IMuboVarBaz = NHXCAN(I1)
         GO TO 910
      CASE('IHXCAN')
         IMuboVarBaz = IHXCAN(I1,I2)
         GO TO 920
      CASE('TYHCAN')
         IMuboVarBaz = TYHCAN(I1,I2)
         GO TO 920
      CASE('NBMSXX')
         IMuboVarBaz = NBMSXX(I1)
         GO TO 910
      CASE('LLMSXX')
         IMuboVarBaz = LLMSXX(I1,I2)
         GO TO 920
      CASE('NCFIDR')
         IMuboVarBaz = NCFIDR
         GO TO 900
      CASE('ITFIDR')
         IMuboVarBaz = ITFIDR(I1,I2)
         GO TO 920
      CASE('IKINTR')
         IMuboVarBaz = IKINTR(I1)
         GO TO 910
      CASE('NbMlINTR' )
         Iw1 = ABS( IKINTR(I1) ) / 1000
         Iw2 = ABS( IKINTR(I1) ) - 1000*Iw1
         NbMl = 2
         IF( Iw1*Iw2 == 0 .OR. MBSto < 0 ) NbMl = 1
         IF( IKINTR(I1) < 0 )              NbMl = - NbMl
         IMuboVarBaz = NbMl
         GO TO 910
      CASE('NUINTR')
         IMuboVarBaz = NUINTR(I1)
         GO TO 910
      CASE('NHINTR')
         IMuboVarBaz = NHINTR(I1)
         GO TO 910
      CASE('NAINTR')
         IMuboVarBaz = NAINTR(I1)
         GO TO 910
      CASE('NOINTR')
         IMuboVarBaz = NOINTR(I1)
         GO TO 910
      CASE('IUINTR')
         IMuboVarBaz = IUINTR(I1,I2)
         GO TO 920
      CASE('IUINTRext')
         IF( I1 <= NUINTR(I2) ) THEN
            IMuboVarBaz = ITUBHI(IUINTR(I1,I2))
         ELSE
            IMuboVarBaz = IBANHI(IUINTR(I1,I2))
         ENDIF
         GO TO 920
      CASE('IHINTR')
         IMuboVarBaz = IHINTR(I1,I2)
         GO TO 920
      CASE('TYHINTR')
         IMuboVarBaz = TYHINTR(I1,I2)
         GO TO 920
      CASE('NBINTR')
         IMuboVarBaz = NBINTR
         GO TO 900
      CASE('MBPri')
         IMuboVarBaz = MBPri
         GO TO 900
      CASE('MBDum')
         IMuboVarBaz = MBDum
         GO TO 900
      CASE('MBAcc')
         IMuboVarBaz = MBAccRec + 10*MBAccTrk
         GO TO 900
      CASE('MBSto')
         IMuboVarBaz = MBSto
         GO TO 900
      CASE('UseRoA')
         IMuboVarBaz = UseRoA
         GO TO 900
      CASE('MBChkDig')
         IMuboVarBaz =  MBChkDig
         GO TO 900
      CASE('MBChkCsc')
         IMuboVarBaz =  MBChkCsc
         GO TO 900
      CASE('EXCL')
         IMuboVarBaz =  ExternalClust
         GO TO 900
      CASE('MDTnu')
         IMuboVarBaz = MDTnu
         GO TO 900
      CASE('CSCpa')
         IMuboVarBaz = CSCpa
         GO TO 900
      CASE('MEloss')
         IMuboVarBaz = MEloss
         GO TO 900
      CASE('SimBug')
         IMuboVarBaz = SimBug
         GO TO 900
      CASE('IsSimu')
         IMuboVarBaz = IsSimu
         GO TO 900
      CASE('AtCosm')
         IMuboVarBaz = AtCosm
         GO TO 900
      CASE('MBideMAX')
         IMuboVarBaz = 100
         GO TO 900
      CASE('NBEROFCYLINDERS')
         IMuboVarBaz = 4
         GO TO 900
      CASE('ISCI')
         IMuboVarBaz = ISCI
         GO TO 900
      CASE('ISENFI')
         IMuboVarBaz = ISENFI
         GO TO 900
      CASE('IFAST')
         IMuboVarBaz = IFAST
         GO TO 900
      CASE('NBMUSC')
         IMuboVarBaz = NBMUSC
         GO TO 900
      CASE('RimRTSvc')
         IMuboVarBaz = RimRTSvc
         GO TO 900
      CASE('JVMUSC')
         IMuboVarBaz = JVMUSC(I1)
         GO TO 910
      CASE('NBMASC')
         IMuboVarBaz = NBMASC
         GO TO 900
      CASE('JVMASC')
         IMuboVarBaz = JVMASC(I1)
         GO TO 910
      CASE('LLMASC')
         IMuboVarBaz = LLMASC(I1)
         GO TO 910
      CASE('IDMASC')
         IMuboVarBaz = IDMASC(I1)
         GO TO 910
      CASE('IValMASC')
         IMuboVarBaz = IValMASC(I1)
         GO TO 910
      CASE('NBMESU')
         IMuboVarBaz = NBMESU
         GO TO 900
      CASE('ItypMboyAmdc')
         IMuboVarBaz = ItypMboyAmdc( ISCINTR(I1) )
         GO TO 910
      CASE('IERINTR')
         IMuboVarBaz = IERINTR(I1)
         GO TO 910
      CASE('DeadCscLay')
         IMuboVarBaz = 0
         IF( DeadCscLay ) IMuboVarBaz = 1
         GO TO 900
      CASE('RecovDead')
         IMuboVarBaz = 0
         IF( RecovDead ) IMuboVarBaz = 1
         GO TO 900
      CASE('DoAligFit')
         IMuboVarBaz = DoAligFit
         GO TO 900
      CASE('DoT0Fit')
         IMuboVarBaz = 0
         IF( DoT0Fit ) THEN
           IMuboVarBaz = 1
           IF( DoT0Print ) IMuboVarBaz = 2
         ENDIF
         GO TO 900
      CASE( 'UseT0Fit' )
         IMuboVarBaz = 0
         IF( UseT0Fit ) IMuboVarBaz = 1
         GO TO 900         
      CASE('NsteT0Fit')
         IMuboVarBaz = NsteT0Fit
         GO TO 900
      CASE('MaxConStr')
         IMuboVarBaz = MaxConStr
         GO TO 900
      CASE('UseAllSeg')
         IMuboVarBaz = 0
         IF( UseAllSeg ) IMuboVarBaz = 1
         GO TO 900
      CASE('UseRoaMdt')
         IMuboVarBaz = 0
         IF( UseRoaMdt ) IMuboVarBaz = 1
         GO TO 900
      CASE('MinuiFOut')
         IMuboVarBaz = MinuiFOut
         GO TO 900
      CASE('NoCutOut')
         IMuboVarBaz = 0
         IF( NoCutOut ) IMuboVarBaz = 1
         GO TO 900
      CASE('LunGeo')
         IMuboVarBaz = LunGeo
         GO TO 900
      CASE('MaxNberMS')
         IMuboVarBaz = MaxNberMS
         GO TO 900
      CASE('IWCHA')
         IMuboVarBaz = IWCHA(I1,I2,I3)
         GO TO 930
      CASE('IGEOMA')
         IMuboVarBaz = IGEOMA(I1,I2,I3)
         GO TO 930
      END SELECT
!
890   CONTINUE
      IStatus = 0
      if (Istop.eq.1) then
        PRINT 1000,TRIM(Name),I1,I2,I3
1000    FORMAT(/' MuonBoy function IMuboVarBaz was called with the following arguments :   ',A,3I6,'  =====>  STOP !')
        STOP
      endif
      return
!
900   IF( I1 /= 0 .OR. I2 /= 0 .OR. I3 /= 0 ) GO TO 890
      RETURN
!
910   IF( I1 < 1  .OR. I2 /= 0 .OR. I3 /= 0 ) GO TO 890
      RETURN
!
920   IF( I1 < 1  .OR. I2 < 1  .OR. I3 /= 0 ) GO TO 890
      RETURN
!
930   IF( I1 < 1  .OR. I2 < 1  .OR. I3 < 1  ) GO TO 890
      RETURN
!
 END FUNCTION IMuboVarBaz
!
!
!> (Re)Set the values of MuonBoy INTEGER parameters and variables
!> \author M.Virchaux
!
 SUBROUTINE SetIMuboVar( Name, I1,I2,I3, Ivar )
 IMPLICIT NONE
 CHARACTER (len=*), INTENT(IN) :: Name
 INTEGER,           INTENT(IN) :: I1,I2,I3,Ivar
 INTEGER :: Istop, IStatus
!
      Istop = 1
      CALL SetIMuboVarBaz( Istop, IStatus, Name, I1,I2,I3, Ivar )
!
 END SUBROUTINE SetIMuboVar
!
!
!> (Re)Set the values of MuonBoy INTEGER parameters and variables
!> \author M.Virchaux
!
 SUBROUTINE SetIMuboVarBaz( Istop, IStatus, Name, I1,I2,I3, Ivar )
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_Reco_Tracks
 USE M_MB_Reco_Segments
 USE M_MB_COMUSC
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER,           INTENT(IN)  :: Istop
 INTEGER,           INTENT(OUT) :: IStatus
 CHARACTER (len=*), INTENT(IN)  :: Name
 INTEGER,           INTENT(IN)  :: I1,I2,I3,Ivar
!
      IStatus = 1
!
      SELECT CASE( Name )
      CASE('NBCAN')
         NBCAN = Ivar
         GO TO 900
      CASE('NFICAN')
         NFICAN = Ivar
         GO TO 900
      CASE('NDFCAN')
         NDFCAN(I1) = Ivar
         GO TO 910
      CASE('FLGCAN')
         FLGCAN(I1) = Ivar
         GO TO 910
      CASE('ISECAN')
         ISECAN(I1) = Ivar
         GO TO 910
      CASE('ITICAN')
         ITICAN(I1,I2) = Ivar
         GO TO 920
      CASE('ITXCAN')
         ITXCAN(I1,I2) = Ivar
         GO TO 920
      CASE('NZXCAN')
         NZXCAN(I1) = Ivar
         GO TO 910
      CASE('IZXCAN')
         IZXCAN(I1,I2) = Ivar
         GO TO 920
      CASE('IZXCANext')
         ITUBHI(IZXCAN(I1,I2)) = Ivar
         GO TO 920
      CASE('NSXCAN')
         NSXCAN(I1) = Ivar
         GO TO 910
      CASE('ISXCAN')
         ISXCAN(I1,I2) = Ivar
         GO TO 920
      CASE('ISXCANext')
         IBANHI(ISXCAN(I1,I2)) = Ivar
         GO TO 920
      CASE('NZOCAN')
         NZOCAN(I1) = Ivar
         GO TO 910
      CASE('NHXCAN')
         NHXCAN(I1) = Ivar
         GO TO 910
      CASE('IHXCAN')
         IHXCAN(I1,I2) = Ivar
         GO TO 920
      CASE('TYHCAN')
         TYHCAN(I1,I2) = Ivar
         GO TO 920
      CASE('NBMSXX')
         NBMSXX(I1) = Ivar
         GO TO 910
      CASE('LLMSXX')
         LLMSXX(I1,I2) = Ivar
         GO TO 920
      CASE('NCFIDR')
         NCFIDR = Ivar
         GO TO 900
      CASE('ITFIDR')
         ITFIDR(I1,I2) = Ivar
         GO TO 920
      CASE('IKINTR')
         IKINTR(I1) = Ivar
         GO TO 910
      CASE('NUINTR')
         NUINTR(I1) = Ivar
         GO TO 910
      CASE('NHINTR')
         NHINTR(I1) = Ivar
         GO TO 910
      CASE('NAINTR')
         NAINTR(I1) = Ivar
         GO TO 910
      CASE('NOINTR')
         NOINTR(I1) = Ivar
         GO TO 910
      CASE('IUINTR')
         IUINTR(I1,I2) = Ivar
         GO TO 920
      CASE('IUINTRext')
         IF( I1 <= NUINTR(I2) ) THEN
            ITUBHI(IUINTR(I1,I2)) = Ivar
         ELSE
            IBANHI(IUINTR(I1,I2)) = Ivar
         ENDIF
         GO TO 920
      CASE('IHINTR')
         IHINTR(I1,I2) = Ivar
         GO TO 920
      CASE('TYHINTR')
         TYHINTR(I1,I2) = Ivar
         GO TO 920
      CASE('NBINTR')
         NBINTR = Ivar
         GO TO 900
      CASE('MBPri')
         MBPri = Ivar
         GO TO 900
      CASE('MBDum')
         MBDum = Ivar
         GO TO 900
      CASE('MBAcc')
         MBAccRec = MAX( 0 , MIN( 1 , MOD(Ivar,10) ) )
         MBAccTrk = MAX( 0 , MIN( 1 ,     Ivar/10  ) )
         GO TO 900
      CASE('MBSto')
         MBSto = Ivar
         GO TO 900
      CASE('UseRoA')
         UseRoA = Ivar
         GO TO 900
      CASE('MBChkCsc')
         MBChkCsc = Ivar 
         GO TO 900
      CASE('MBChkDig')
         MBChkDig = Ivar 
         GO TO 900
      CASE('EXCL')
         ExternalClust = Ivar
         GO TO 900
      CASE('MDTnu')
         MDTnu = Ivar
         GO TO 900
      CASE('CSCpa')
         CSCpa = Ivar
         GO TO 900
      CASE('MEloss')
         MEloss = Ivar
         GO TO 900
      CASE('SimBug')
         SimBug = Ivar
         GO TO 900
      CASE('IsSimu')
         IsSimu = Ivar
         GO TO 900
      CASE('AtCosm')
         AtCosm = Ivar
         GO TO 900
      CASE('ISCI')
         ISCI = Ivar
         GO TO 900
      CASE('ISENFI')
         ISENFI = Ivar
         GO TO 900
      CASE('IFAST') 
         IFAST = Ivar
         GO TO 900
      CASE('NBMUSC') 
         NBMUSC = Ivar
         GO TO 900
      CASE('NBMASC') 
         NBMASC = Ivar
         GO TO 900
      CASE('NBMESU') 
         NBMESU = Ivar
         GO TO 900
      CASE('IERINTR')
         IERINTR(I1) = Ivar
         GO TO 910
      CASE('DeadCscLay')
         DeadCscLay = ( Ivar >= 1 )
         GO TO 900
      CASE('RecovDead')
         RecovDead = ( Ivar >= 1 )
         GO TO 900
      CASE('DoAligFit')
         DoAligFit = Ivar
         GO TO 900
      CASE('DoT0Fit')
         DoT0Fit   = ( Ivar >= 1 )
         DoT0Print = ( Ivar >= 2 )
         GO TO 900
      CASE('UseT0Fit')
         UseT0Fit   = ( Ivar >= 1 )
         GO TO 900
      CASE('NsteT0Fit')
         NsteT0Fit = Ivar
         GO TO 900
      CASE('MaxConStr')
         MaxConStr = Ivar
         GO TO 900
      CASE('RimRTSvc')
         RimRTSvc = Ivar
         GO TO 900
      CASE('UseAllSeg')
         UseAllSeg = ( Ivar >= 1 )
         GO TO 900
      CASE('UseRoaMdt')
         UseRoaMdt = ( Ivar >= 1 )
         GO TO 900
      CASE('MinuiFOut')
         MinuiFOut = Ivar
         GO TO 900
      CASE('NoCutOut')
         NoCutOut = ( Ivar >= 1 )
         GO TO 900
      CASE('LunGeo')
         LunGeo = Ivar
         GO TO 900
      CASE('MaxNberMS')
         MaxNberMS = Ivar
         GO TO 900
      END SELECT
!
890   CONTINUE
      IStatus = 0
      if (Istop.eq.1) then
        PRINT 1000,TRIM(Name),I1,I2,I3
1000    FORMAT(/' MuonBoy routine SetIMuboVarBaz was called with the following arguments :   ',A,3I6,'  =====>  STOP !')
        STOP
      endif
      return
!
900   IF( I1 /= 0 .OR. I2 /= 0 .OR. I3 /= 0 ) GO TO 890
      RETURN
!
910   IF( I1 < 1  .OR. I2 /= 0 .OR. I3 /= 0 ) GO TO 890
      RETURN
!
920   IF( I1 < 1  .OR. I2 < 1  .OR. I3 /= 0 ) GO TO 890
      RETURN
!
 END SUBROUTINE SetIMuboVarBaz
!
!
!
 SUBROUTINE F1GetIMuboVar( Long, Name1, I1, I2, I3, Ivar )
 IMPLICIT NONE
 CHARACTER (Len=1), INTENT(IN)  :: Name1(*)
 INTEGER,           INTENT(IN)  :: Long,I1,I2,I3
 INTEGER,           INTENT(OUT) :: Ivar
 INTEGER, EXTERNAL    :: IMuboVar
 CHARACTER (len=Long) :: VarName
 INTEGER              :: I
!
    DO I=1,Long
      VarName(I:I) = Name1(I)
    ENDDO
    Ivar = IMuboVar( VarName, I1,I2,I3 )
!
 END SUBROUTINE F1GetIMuboVar
!
!
!
 SUBROUTINE F1GetIMuboVarBaz( Istop, IStatus, Long, Name1, I1, I2, I3, Ivar )
 IMPLICIT NONE
 INTEGER,           INTENT(IN)  :: Istop
 INTEGER,           INTENT(OUT) :: IStatus
 CHARACTER (Len=1), INTENT(IN)  :: Name1(*)
 INTEGER,           INTENT(IN)  :: Long,I1,I2,I3
 INTEGER,           INTENT(OUT) :: Ivar
 INTEGER, EXTERNAL    :: IMuboVarBaz
 CHARACTER (len=Long) :: VarName
 INTEGER              :: I
!
    DO I=1,Long
      VarName(I:I) = Name1(I)
    ENDDO
    Ivar = IMuboVarBaz( Istop, IStatus, VarName, I1,I2,I3 )
!
 END SUBROUTINE F1GetIMuboVarBaz
!
!
!
 SUBROUTINE F1SetIMuboVar( Long, Name1, I1, I2, I3, Ivar )
 IMPLICIT NONE
 CHARACTER (Len=1), INTENT(IN)  :: Name1(*)
 INTEGER,           INTENT(IN)  :: Long,I1,I2,I3,Ivar
 CHARACTER (len=Long) :: VarName
 INTEGER              :: I
!
    DO I=1,Long
      VarName(I:I) = Name1(I)
    ENDDO
    CALL SetIMuboVar( VarName, I1,I2,I3, Ivar )
!
 END SUBROUTINE F1SetIMuboVar
!
!
!
 SUBROUTINE F1SetIMuboVarBaz( Istop, IStatus, Long, Name1, I1, I2, I3, Ivar )
 IMPLICIT NONE
 INTEGER,           INTENT(IN)  :: Istop
 INTEGER,           INTENT(OUT) :: IStatus
 CHARACTER (Len=1), INTENT(IN)  :: Name1(*)
 INTEGER,           INTENT(IN)  :: Long,I1,I2,I3,Ivar
 CHARACTER (len=Long) :: VarName
 INTEGER              :: I
!
    DO I=1,Long
      VarName(I:I) = Name1(I)
    ENDDO
    CALL SetIMuboVarBaz( Istop, IStatus, VarName, I1,I2,I3, Ivar )
!
 END SUBROUTINE F1SetIMuboVarBaz
!
