!
!> Return the values of MuonBoy REAL(8) parameters and variables
!> \author M.Virchaux
!
 REAL(8) FUNCTION RMuboVar( Name, I1,I2,I3 )
 IMPLICIT NONE
 CHARACTER (len=*), INTENT(IN) :: Name
 INTEGER,           INTENT(IN) :: I1,I2,I3
 REAL(8), EXTERNAL    :: RMuboVarBaz
 INTEGER :: Istop, IStatus
!
      Istop = 1
      RMuboVar = RMuboVarBaz( Istop, IStatus, Name, I1,I2,I3 )
!
 END FUNCTION RMuboVar
!
!
!> Return the values of MuonBoy REAL(8) parameters and variables
!> \author M.Virchaux
!
 REAL(8) FUNCTION RMuboVarBaz( Istop, IStatus, Name, I1,I2,I3 )
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
!
      IStatus = 1
!
      SELECT CASE( Name )
      CASE('SSTRAKDO')
         RMuboVarBaz = SSTRAKDO(I1,I2,I3)
         GO TO 930
      CASE('ZZTRAKDO')
         RMuboVarBaz = ZZTRAKDO(I1,I2,I3)
         GO TO 930
      CASE('TTTRAKDO')
         RMuboVarBaz = TTTRAKDO(I1,I2,I3)
         GO TO 930
      CASE('CCDRAT')
         RMuboVarBaz = CCDRAT(I1,I2)
         GO TO 920
      CASE('FFDRAT')
         RMuboVarBaz = FFDRAT(I1,I2)
         GO TO 920
      CASE('GGDRAT')
         RMuboVarBaz = GGDRAT(I1,I2)
         GO TO 920
      CASE('EZDRAT')
         RMuboVarBaz = EZDRAT(I1,I2)
         GO TO 920
      CASE('SSDRAT')
         RMuboVarBaz = SSDRAT(I1,I2,I3)
         GO TO 930
      CASE('ZZDRAT')
         RMuboVarBaz = ZZDRAT(I1,I2,I3)
         GO TO 930
      CASE('TTDRAT')
         RMuboVarBaz = TTDRAT(I1,I2,I3)
         GO TO 930
      CASE('PASTUB')
         RMuboVarBaz = PASTUB(I1)
         GO TO 910
      CASE('RESTUB')
         RMuboVarBaz = RESTUB(I1)
         GO TO 910
      CASE('GCECHAV')
         RMuboVarBaz = GCECHAV(I1)
         GO TO 910
      CASE('TCECHAV')
         RMuboVarBaz = TCECHAV(I1)
         GO TO 910
      CASE('FCECHAV')
         RMuboVarBaz = FCECHAV(I1)
         GO TO 910
      CASE('CHICAN')
         RMuboVarBaz = CHICAN(I1)
         GO TO 910
      CASE('CXXCAN')
         RMuboVarBaz = CXXCAN(I1,I2)
         GO TO 920
      CASE('VMUCAN')
         RMuboVarBaz = VMUCAN(I1,I2)
         GO TO 920
      CASE('COXCAN')
         RMuboVarBaz = COXCAN(I1,I2,I3)
         GO TO 930
      CASE('PAXCAN')
         RMuboVarBaz = PAXCAN(I1,I2)
         GO TO 920
      CASE('VXXCAN')
         RMuboVarBaz = VXXCAN(I1,I2)
         GO TO 920
      CASE('COYCAN')
         RMuboVarBaz = COYCAN(I1,I2,I3)
         GO TO 930
      CASE('PAYCAN')
         RMuboVarBaz = PAYCAN(I1,I2)
         GO TO 920
      CASE('VYYCAN')
         RMuboVarBaz = VYYCAN(I1,I2)
         GO TO 920
      CASE('COZCAN')
         RMuboVarBaz = COZCAN(I1,I2,I3)
         GO TO 930
      CASE('PAZCAN')
         RMuboVarBaz = PAZCAN(I1,I2)
         GO TO 920
      CASE('VZZCAN')
         RMuboVarBaz = VZZCAN(I1,I2)
         GO TO 920
      CASE('RZXCAN')
         RMuboVarBaz = RZXCAN(I1,I2)
         GO TO 920
      CASE('RZICAN')
         RMuboVarBaz = RZICAN(I1,I2)
         GO TO 920
      CASE('RZISIM')
         RMuboVarBaz = RZISIM(I1,I2)
         GO TO 920
      CASE('EZIFIT')
         RMuboVarBaz = EZIFIT(I1,I2)
         GO TO 920
      CASE('RSXCAN')
         RMuboVarBaz = SBANHI(ISXCAN(I1,I2))
         GO TO 920
      CASE('RSISIM')
         RMuboVarBaz = RSISIM(I1,I2)
         GO TO 920
      CASE('ESIFIT')
         RMuboVarBaz = ESIFIT(I1,I2)
         GO TO 920
      CASE('RRMSXX')
         RMuboVarBaz = RRMSXX(I1,I2)
         GO TO 920
      CASE('X0MSXX')
         RMuboVarBaz = X0MSXX(I1,I2)
         GO TO 920
      CASE('S1MSXX')
         RMuboVarBaz = S1MSXX(I1,I2)
         GO TO 920
      CASE('S2MSXX')
         RMuboVarBaz = S2MSXX(I1,I2)
         GO TO 920
      CASE('THMSXX')
         RMuboVarBaz = THMSXX(I1,I2)
         GO TO 920
      CASE('ENMSXX')
         RMuboVarBaz = ENMSXX(I1)
         GO TO 910
      CASE('EAMSXX')
         RMuboVarBaz = EAMSXX(I1,I2)
         GO TO 920
      CASE('EBMSXX')
         RMuboVarBaz = EBMSXX(I1,I2)
         GO TO 920
      CASE('VinVouXX')
         RMuboVarBaz = VinVouXX(I1,I2,I3)
         GO TO 930
      CASE('VinVouXX2')
         RMuboVarBaz = VinVouXX2(I1,I2,I3)
         GO TO 930
      CASE('PAFIDR')
         RMuboVarBaz = PAFIDR(I1,I2)
         GO TO 920
      CASE('VMFIDR')
         RMuboVarBaz = VMFIDR(I1,I2)
         GO TO 920
      CASE('CIFIDR')
         RMuboVarBaz = CIFIDR(I1)
         GO TO 910
      CASE('PTINTR')
         RMuboVarBaz = PTINTR(I1,I2)
         GO TO 920
      CASE('VEINTR')
         RMuboVarBaz = VEINTR(I1,I2)
         GO TO 920
      CASE('CHINTR')
         RMuboVarBaz = CHINTR(I1)
         GO TO 910
      CASE('T0INTR')
         RMuboVarBaz = T0INTR(I1)
         GO TO 910
      CASE('ET0INTR')
         RMuboVarBaz = ET0INTR(I1)
         GO TO 910
      CASE('RdigINTR')
         RMuboVarBaz = RdigINTR(I1,I2)
         GO TO 920
      CASE('RtraINTR')
         RMuboVarBaz = RtraINTR(I1,I2)
         GO TO 920
      CASE('Z1')
         RMuboVarBaz = Z1
         GO TO 900
      CASE('T1')
         RMuboVarBaz = T1
         GO TO 900
      CASE('S1')
         RMuboVarBaz = S1
         GO TO 900
      CASE('X1')
         RMuboVarBaz = X1
         GO TO 900
      CASE('Y1')
         RMuboVarBaz = Y1
         GO TO 900
      CASE('Z2')
         RMuboVarBaz = Z2
         GO TO 900
      CASE('T2')
         RMuboVarBaz = T2
         GO TO 900
      CASE('S2')
         RMuboVarBaz = S2
         GO TO 900
      CASE('X2')
         RMuboVarBaz = X2
         GO TO 900
      CASE('Y2')
         RMuboVarBaz = Y2
         GO TO 900
      CASE('SFA')
         RMuboVarBaz = SFA
         GO TO 900
      CASE('CFA')
         RMuboVarBaz = CFA
         GO TO 900
      CASE('SFI')
         RMuboVarBaz = SFI
         GO TO 900
      CASE('CFI')
         RMuboVarBaz = CFI
         GO TO 900
      CASE('XSeve')
         RMuboVarBaz = XSeve
         GO TO 900
      CASE('DeTub')
         RMuboVarBaz = DeTub
         GO TO 900
      CASE('ReTub')
         RMuboVarBaz = ReTub
         GO TO 900
      CASE('ResBadBEE')
         RMuboVarBaz = ResBadBEE
         GO TO 900
      CASE('ResBadBIS')
         RMuboVarBaz = ResBadBIS
         GO TO 900
      CASE('ResBadEIL')
         RMuboVarBaz = ResBadEIL
         GO TO 900
      CASE('ResBadEEL')
         RMuboVarBaz = ResBadEEL
         GO TO 900
      CASE('ResBadAli')
         RMuboVarBaz = ResBadAli
         GO TO 900
      CASE('ResBadTomo')
         RMuboVarBaz = ResBadTomo
         GO TO 900
      CASE('ReTubNoT0')
         RMuboVarBaz = ReTubNoT0
         GO TO 900
      CASE('StepT0Fit')
         RMuboVarBaz = StepT0Fit
         GO TO 900
      CASE('TubEf')
         RMuboVarBaz = TubEf
         GO TO 900
      CASE('StrEf')
         RMuboVarBaz = StrEf
         GO TO 900
      CASE('AerML')
         RMuboVarBaz = AerML
         GO TO 900
      CASE('CerML')
         RMuboVarBaz = CerML
         GO TO 900
      CASE('AligStBa')
         RMuboVarBaz = AligStBa
         GO TO 900
      CASE('AligStEc')
         RMuboVarBaz = AligStEc
         GO TO 900
      CASE('AligStCs')
         RMuboVarBaz = AligStCs
         GO TO 900
      CASE('AligBaEc')
         RMuboVarBaz = AligBaEc
         GO TO 900
      CASE('AligRPCeta')
         RMuboVarBaz = AligRPCeta
         GO TO 900
      CASE('AligRPCphi')
         RMuboVarBaz = AligRPCphi
         GO TO 900
      CASE('AligTGCeta')
         RMuboVarBaz = AligTGCeta
         GO TO 900
      CASE('AligTGCphi')
         RMuboVarBaz = AligTGCphi
         GO TO 900
      CASE('ErrZMsIdBa')
         RMuboVarBaz = ErrZMsIdBa
         GO TO 900
      CASE('ErrZMsIdEc')
         RMuboVarBaz = ErrZMsIdEc
         GO TO 900
      CASE('ResAngBa')
         RMuboVarBaz = ResAngBa
         GO TO 900
      CASE('ResAngEc')
         RMuboVarBaz = ResAngEc
         GO TO 900
      CASE('FixMo')
         RMuboVarBaz = FixMo
         GO TO 900
      CASE('MomNoField')
         RMuboVarBaz = MomNoField
         GO TO 900
      CASE('DsFastRT')
         RMuboVarBaz = DsFastRT
         GO TO 900
      CASE('FinalCut')
         RMuboVarBaz = FinalCut
         GO TO 900
      CASE('TStripMin')
         RMuboVarBaz = TStripMin
         GO TO 900
      CASE('TStripMax')
         RMuboVarBaz = TStripMax
         GO TO 900
      CASE('TubStr2ML')
         RMuboVarBaz = TubStr2ML
         GO TO 900
      CASE('TubStr1ML')
         RMuboVarBaz = TubStr1ML
         GO TO 900
      CASE('TubLoo2ML')
         RMuboVarBaz = TubLoo2ML
         GO TO 900
      CASE('TubLoo1ML')
         RMuboVarBaz = TubLoo1ML
         GO TO 900
      CASE('MdtCut')
         RMuboVarBaz = MdtCut
         GO TO 900
      CASE('MdtAdc')
         RMuboVarBaz = MdtAdc
         GO TO 900
      CASE('SegAdc')
         RMuboVarBaz = SegAdc
         GO TO 900
      CASE('RCYLIN')
         RMuboVarBaz = RCYLIN(I1)
         GO TO 910
      CASE('ZCYLIN')
         RMuboVarBaz = ZCYLIN(I1)
         GO TO 910
      CASE('RCYLI2')
         RMuboVarBaz = RCYLI2(I1)
         GO TO 910
      CASE('VDEB' )
         RMuboVarBaz = VDEB(I1,I2)
         GO TO 920
      CASE('DZT1')
         RMuboVarBaz = DZT1
         GO TO 900
      CASE('DS1')
         RMuboVarBaz = DS1
         GO TO 900
      CASE('DZT2')
         RMuboVarBaz = DZT2
         GO TO 900
      CASE('DS2')
         RMuboVarBaz = DS2
         GO TO 900
      CASE('SMU')
         RMuboVarBaz = SMU
         GO TO 900
      CASE('RCHAMA')
         RMuboVarBaz = RCHAMA
         GO TO 900
      CASE('RCHAMI')
         RMuboVarBaz = RCHAMI
         GO TO 900
      CASE('ZCHAMA')
         RMuboVarBaz = ZCHAMA
         GO TO 900
      CASE('ZCHAMI')
         RMuboVarBaz = ZCHAMI
         GO TO 900
      CASE('VTHETAPHI')
         RMuboVarBaz = VTHETAPHI(I1)
         GO TO 910
      CASE('VPERIGEE')
         RMuboVarBaz = VPERIGEE(I1)
         GO TO 910
      CASE('VEND')
         RMuboVarBaz = VEND(I1)
         GO TO 910
      CASE('RRMUSC')
         RMuboVarBaz = RRMUSC(I1)
         GO TO 910
      CASE('RRMASC')
         RMuboVarBaz = RRMASC(I1)
         GO TO 910
      CASE('XposMASC')
         RMuboVarBaz = XposMASC(I1)
         GO TO 910
      CASE('YposMASC')
         RMuboVarBaz = YposMASC(I1)
         GO TO 910
      CASE('ZposMASC')
         RMuboVarBaz = ZposMASC(I1)
         GO TO 910
      CASE('XdirMASC')
         RMuboVarBaz = XdirMASC(I1)
         GO TO 910
      CASE('YdirMASC')
         RMuboVarBaz = YdirMASC(I1)
         GO TO 910
      CASE('ZdirMASC')
         RMuboVarBaz = ZdirMASC(I1)
         GO TO 910
      CASE('PmomMASC')
         RMuboVarBaz = PmomMASC(I1)
         GO TO 910
      CASE('ChgeMASC')
         RMuboVarBaz = ChgeMASC(I1)
         GO TO 910
      CASE('X0MASC')
         RMuboVarBaz = X0MASC(I1)
         GO TO 910
      CASE('S1MASC')
         RMuboVarBaz = S1MASC(I1)
         GO TO 910
      CASE('S2MASC')
         RMuboVarBaz = S2MASC(I1)
         GO TO 910
      CASE('THMASC')
         RMuboVarBaz = THMASC(I1)
         GO TO 910
      CASE('ALMASC')
         RMuboVarBaz = ALMASC(I1)
         GO TO 910
      CASE('BLMASC')
         RMuboVarBaz = BLMASC(I1)
         GO TO 910
      CASE('VinVouMASC')
         RMuboVarBaz = VinVouMASC(I1,I2)
         GO TO 920
      CASE('VinVou2MASC')
         RMuboVarBaz = VinVou2MASC(I1,I2)
         GO TO 920
      CASE('ENMASC')
         RMuboVarBaz = ENMASC
         GO TO 900
      CASE('SIGENMASC')
         RMuboVarBaz = SIGENMASC
         GO TO 900
      CASE('RRMESU')
         RMuboVarBaz = RRMESU(I1)
         GO TO 910
      CASE('DDMESU')
         RMuboVarBaz = DDMESU(I1)
         GO TO 910
      CASE('FFMESU')
         RMuboVarBaz = FFMESU(I1)
         GO TO 910
      CASE('TRECEVT' )
         RMuboVarBaz = TRECEVT
         GO TO 900
      CASE('RCHAMImi' )
         RMuboVarBaz = RCHAMImi
         GO TO 900
      CASE('RCHAMAma' )
         RMuboVarBaz = RCHAMAma
         GO TO 900
      CASE('XDCCAN')
         RMuboVarBaz = XDCCAN(I1,I2)
         GO TO 920
      CASE('YDCCAN')
         RMuboVarBaz = YDCCAN(I1,I2)
         GO TO 920
      CASE('ZDCCAN')
         RMuboVarBaz = ZDCCAN(I1,I2)
         GO TO 920
       CASE('XDCdCAN')
         RMuboVarBaz = XDCdCAN(I1,I2)
         GO TO 920
      CASE('YDCdCAN')
         RMuboVarBaz = YDCdCAN(I1,I2)
         GO TO 920
      CASE('ZDCdCAN')
         RMuboVarBaz = ZDCdCAN(I1,I2)
         GO TO 920
     CASE('ENCCAN')
         RMuboVarBaz = ENCCAN(I1,I2)
         GO TO 920
      CASE('RDCCAN')
         RMuboVarBaz = RDCCAN(I1,I2)
         GO TO 920
      CASE('SDCCAN')
         RMuboVarBaz = SDCCAN(I1,I2)
         GO TO 920
      CASE('XSDCCAN')
         RMuboVarBaz = XSDCCAN(I1,I2)
         GO TO 920
      CASE('YSDCCAN')
         RMuboVarBaz = YSDCCAN(I1,I2)
         GO TO 920
      CASE('ZSDCCAN')
         RMuboVarBaz = ZSDCCAN(I1,I2)
         GO TO 920
      CASE('XSDCdCAN')
         RMuboVarBaz = XSDCdCAN(I1,I2)
         GO TO 920
      CASE('YSDCdCAN')
         RMuboVarBaz = YSDCdCAN(I1,I2)
         GO TO 920
      CASE('ZSDCdCAN')
         RMuboVarBaz = ZSDCdCAN(I1,I2)
         GO TO 920
      CASE('ESNCCAN')
         RMuboVarBaz = ESNCCAN(I1,I2)
         GO TO 920
      CASE('RSDCCAN')
         RMuboVarBaz = RSDCCAN(I1,I2)
         GO TO 920
      CASE('ENHCAN')
         RMuboVarBaz = ENHCAN(I1,I2)
         GO TO 920
      CASE('XHACAN')
         RMuboVarBaz = XHACAN(I1,I2)
         GO TO 920
      CASE('YHACAN')
         RMuboVarBaz = YHACAN(I1,I2)
         GO TO 920
      CASE('ZHACAN')
         RMuboVarBaz = ZHACAN(I1,I2)
         GO TO 920
      CASE('XHdCAN')
         RMuboVarBaz = XHdCAN(I1,I2)
         GO TO 920
      CASE('YHdCAN')
         RMuboVarBaz = YHdCAN(I1,I2)
         GO TO 920
      CASE('ZHdCAN')
         RMuboVarBaz = ZHdCAN(I1,I2)
         GO TO 920
      CASE('XDINTR')
         RMuboVarBaz = XDINTR(I1,I2)
         GO TO 920
      CASE('YDINTR')
         RMuboVarBaz = YDINTR(I1,I2)
         GO TO 920
      CASE('ZDINTR')
         RMuboVarBaz = ZDINTR(I1,I2)
         GO TO 920
      CASE('XDdINTR')
         RMuboVarBaz = XDdINTR(I1,I2)
         GO TO 920
      CASE('YDdINTR')
         RMuboVarBaz = YDdINTR(I1,I2)
         GO TO 920
      CASE('ZDdINTR')
         RMuboVarBaz = ZDdINTR(I1,I2)
         GO TO 920
      CASE('RDINTR')
         RMuboVarBaz = RDINTR(I1,I2)
         GO TO 920
      CASE('DXINTR')
         RMuboVarBaz = DXINTR(I1,I2)
         GO TO 920
      CASE('DYINTR')
         RMuboVarBaz = DYINTR(I1,I2)
         GO TO 920
      CASE('DZINTR')
         RMuboVarBaz = DZINTR(I1,I2)
         GO TO 920
      CASE('ERINTR')
         RMuboVarBaz = ERINTR(I1,I2)
         GO TO 920
      CASE('XHINTR')
         RMuboVarBaz = XHINTR(I1,I2)
         GO TO 920
      CASE('YHINTR')
         RMuboVarBaz = YHINTR(I1,I2)
         GO TO 920
      CASE('ZHINTR')
         RMuboVarBaz = ZHINTR(I1,I2)
         GO TO 920
      CASE('XHdINTR')
         RMuboVarBaz = XHdINTR(I1,I2)
         GO TO 920
      CASE('YHdINTR')
         RMuboVarBaz = YHdINTR(I1,I2)
         GO TO 920
      CASE('ZHdINTR')
         RMuboVarBaz = ZHdINTR(I1,I2)
         GO TO 920
      END SELECT
!
890   CONTINUE
      IStatus = 0
      if (Istop.eq.1) then
        PRINT 1000,TRIM(Name),I1,I2,I3
1000    FORMAT(/' MuonBoy function RMuboVarBaz was called with the following arguments :   ',A,3I6,'  =====>  STOP !')
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
 END FUNCTION RMuboVarBaz
!
!
!> (Re)Set the values of MuonBoy REAL(8) parameters and variables
!> \author M.Virchaux
!
 SUBROUTINE SetRMuboVar( Name, I1,I2,I3, Rvar )
 IMPLICIT NONE
 CHARACTER (len=*), INTENT(IN) :: Name
 INTEGER,           INTENT(IN) :: I1,I2,I3
 REAL(8),           INTENT(IN) :: Rvar
 INTEGER :: Istop, IStatus
!
      Istop = 1
      CALL SetRMuboVarBaz( Istop, IStatus, Name, I1,I2,I3, Rvar )
!
 END SUBROUTINE SetRMuboVar
!
!
!> (Re)Set the values of MuonBoy REAL(8) parameters and variables
!> \author : M.Virchaux
!
 SUBROUTINE SetRMuboVarBaz( Istop, IStatus, Name, I1,I2,I3, Rvar )
 USE M_MB_Control
 USE M_MB_Reco_Tracks
 USE M_MB_Reco_Segments
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER,           INTENT(IN)  :: Istop
 INTEGER,           INTENT(OUT) :: IStatus
 CHARACTER (len=*), INTENT(IN)  :: Name
 INTEGER,           INTENT(IN)  :: I1,I2,I3
 REAL(8),           INTENT(IN)  :: Rvar
!
      IStatus = 1
!
      SELECT CASE( Name )
      CASE('CHICAN')
         CHICAN(I1) = Rvar
         GO TO 910
      CASE('CXXCAN')
         CXXCAN(I1,I2) = Rvar
         GO TO 920
      CASE('VMUCAN')
         VMUCAN(I1,I2) = Rvar
         GO TO 920
      CASE('COXCAN')
         COXCAN(I1,I2,I3) = Rvar
         GO TO 930
      CASE('PAXCAN')
         PAXCAN(I1,I2) = Rvar
         GO TO 920
      CASE('VXXCAN')
         VXXCAN(I1,I2) = Rvar
         GO TO 920
      CASE('COYCAN')
         COYCAN(I1,I2,I3) = Rvar
         GO TO 930
      CASE('PAYCAN')
         PAYCAN(I1,I2) = Rvar
         GO TO 920
      CASE('VYYCAN')
         VYYCAN(I1,I2) = Rvar
         GO TO 920
      CASE('COZCAN')
         COZCAN(I1,I2,I3) = Rvar
         GO TO 930
      CASE('PAZCAN')
         PAZCAN(I1,I2) = Rvar
         GO TO 920
      CASE('VZZCAN')
         VZZCAN(I1,I2) = Rvar
         GO TO 920
      CASE('RZXCAN')
         RZXCAN(I1,I2) = Rvar
         GO TO 920
      CASE('RZICAN')
         RZICAN(I1,I2) = Rvar
         GO TO 920
      CASE('RZISIM')
         RZISIM(I1,I2) = Rvar
         GO TO 920
      CASE('EZIFIT')
         EZIFIT(I1,I2) = Rvar
         GO TO 920
      CASE('RSXCAN')
         WRITE(*,*) ' What are you trying to do with RSXCAN ??? '
         GO TO 920
      CASE('RSISIM')
         RSISIM(I1,I2) = Rvar
         GO TO 920
      CASE('ESIFIT')
         ESIFIT(I1,I2) = Rvar
         GO TO 920
      CASE('RRMSXX')
         RRMSXX(I1,I2) = Rvar
         GO TO 920
      CASE('X0MSXX')
         X0MSXX(I1,I2) = Rvar
         GO TO 920
      CASE('S1MSXX')
         S1MSXX(I1,I2) = Rvar
         GO TO 920
      CASE('S2MSXX')
         S2MSXX(I1,I2) = Rvar
         GO TO 920
      CASE('THMSXX')
         THMSXX(I1,I2) = Rvar
         GO TO 920
      CASE('ENMSXX')
         ENMSXX(I1) = Rvar
         GO TO 910
      CASE('EAMSXX')
         EAMSXX(I1,I2) = Rvar
         GO TO 920
      CASE('EBMSXX')
         EBMSXX(I1,I2) = Rvar
         GO TO 920
      CASE('VinVouXX')
         VinVouXX(I1,I2,I3) = Rvar
         GO TO 930
      CASE('VinVouXX2')
         VinVouXX2(I1,I2,I3) = Rvar
         GO TO 930
      CASE('PAFIDR')
         PAFIDR(I1,I2) = Rvar
         GO TO 920
      CASE('VMFIDR')
         VMFIDR(I1,I2) = Rvar
         GO TO 920
      CASE('CIFIDR')
         CIFIDR(I1) = Rvar
         GO TO 910
      CASE('PTINTR')
         PTINTR(I1,I2) = Rvar
         GO TO 920
      CASE('VEINTR')
         VEINTR(I1,I2) = Rvar
         GO TO 920
      CASE('CHINTR')
         CHINTR(I1) = Rvar
         GO TO 910
      CASE('T0INTR')
         T0INTR(I1) = Rvar
         GO TO 910
      CASE('ET0INTR')
         ET0INTR(I1) = Rvar
         GO TO 910
      CASE('RdigINTR')
         RdigINTR(I1,I2) = Rvar
         GO TO 920
      CASE('RtraINTR')
         RtraINTR(I1,I2) = Rvar
         GO TO 920
      CASE('Z1')
         Z1 = Rvar
         GO TO 900
      CASE('T1')
         T1 = Rvar
         GO TO 900
      CASE('S1')
         S1 = Rvar
         GO TO 900
      CASE('X1')
         X1 = Rvar
         GO TO 900
      CASE('Y1')
         Y1 = Rvar
         GO TO 900
      CASE('Z2')
         Z2 = Rvar
         GO TO 900
      CASE('T2')
         T2 = Rvar
         GO TO 900
      CASE('S2')
         S2 = Rvar
         GO TO 900
      CASE('X2')
         X2 = Rvar
         GO TO 900
      CASE('Y2')
         Y2 = Rvar
         GO TO 900
      CASE('SFA')
         SFA = Rvar
         GO TO 900
      CASE('CFA')
         CFA = Rvar
         GO TO 900
      CASE('SFI')
         SFI = Rvar
         GO TO 900
      CASE('CFI')
         CFI = Rvar
         GO TO 900
      CASE('XSeve')
         XSeve = Rvar
         GO TO 900
      CASE('DeTub')
         DeTub = Rvar
         GO TO 900
      CASE('ReTub')
         ReTub = Rvar
         GO TO 900
      CASE('ResBadBEE')
         ResBadBEE = Rvar
         GO TO 900
      CASE('ResBadBIS')
         ResBadBIS = Rvar
         GO TO 900
      CASE('ResBadEIL')
         ResBadEIL = Rvar
         GO TO 900
      CASE('ResBadEEL')
         ResBadEEL = Rvar
         GO TO 900
      CASE('ResBadAli')
         ResBadAli = Rvar
         GO TO 900
      CASE('ResBadTomo')
         ResBadTomo = Rvar
         GO TO 900
      CASE('ReTubNoT0')
         ReTubNoT0 = Rvar
         GO TO 900
      CASE('StepT0Fit')
         StepT0Fit = Rvar
         GO TO 900
      CASE('TubEf')
         TubEf = Rvar
         GO TO 900
      CASE('StrEf')
         StrEf = Rvar
         GO TO 900
      CASE('AerSt')
         AligStBa = Rvar
         AligStEc = Rvar
         GO TO 900
      CASE('AerML')
         AerML = Rvar
         GO TO 900
      CASE('CerSt')
         AligStCs = Rvar
         GO TO 900
      CASE('CerML')
         CerML = Rvar
         GO TO 900
      CASE('AligStBa')
         AligStBa = Rvar
         GO TO 900
      CASE('AligStEc')
         AligStEc = Rvar
         GO TO 900
      CASE('AligStCs')
         AligStCs = Rvar
         GO TO 900
      CASE('AligBaEc')
         AligBaEc = Rvar
         GO TO 900
      CASE('AligRPCeta')
         AligRPCeta = Rvar
         GO TO 900
      CASE('AligRPCphi')
         AligRPCphi = Rvar
         GO TO 900
      CASE('AligTGCeta')
         AligTGCeta = Rvar
         GO TO 900
      CASE('AligTGCphi')
         AligTGCphi = Rvar
         GO TO 900
      CASE('ErrZMsIdBa')
         ErrZMsIdBa = Rvar
         GO TO 900
      CASE('ErrZMsIdEc')
         ErrZMsIdEc = Rvar
         GO TO 900
      CASE('ResAngBa')
         ResAngBa = Rvar
         GO TO 900
      CASE('ResAngEc')
         ResAngEc = Rvar
         GO TO 900
      CASE('FixMo')
         FixMo = Rvar
         GO TO 900
      CASE('MomNoField')
         MomNoField = Rvar
         GO TO 900
      CASE('DsFastRT')
         DsFastRT = Rvar
         GO TO 900
      CASE('FinalCut')
         FinalCut = Rvar
         GO TO 900
      CASE('TStripMin')
         TStripMin = Rvar
         GO TO 900
      CASE('TStripMax')
         TStripMax = Rvar
         GO TO 900
      CASE('TubStr2ML')
         TubStr2ML = Rvar
         GO TO 900
      CASE('TubStr1ML')
         TubStr1ML = Rvar
         GO TO 900
      CASE('TubLoo2ML')
         TubLoo2ML = Rvar
         GO TO 900
      CASE('TubLoo1ML')
         TubLoo1ML = Rvar
         GO TO 900
      CASE('MdtCut')
         MdtCut = Rvar
         GO TO 900
      CASE('MdtAdc')
         MdtAdc = Rvar
         GO TO 900
      CASE('SegAdc')
         SegAdc = Rvar
         GO TO 900
      CASE('RCYLIN')
         RCYLIN(I1) = Rvar
         GO TO 910
      CASE('ZCYLIN')
         ZCYLIN(I1) = Rvar
         GO TO 910
      CASE('RCYLI2')
         RCYLI2(I1) = Rvar
         GO TO 910
      CASE('VDEB' )
         VDEB(I1,I2) = Rvar
         GO TO 920
      CASE('DZT1')
         DZT1 = Rvar
         GO TO 900
      CASE('DS1')
         DS1 = Rvar
         GO TO 900
      CASE('DZT2')
         DZT2 = Rvar
         GO TO 900
      CASE('DS2')
         DS2 = Rvar
         GO TO 900
      CASE('SMU')
         SMU = Rvar
         GO TO 900
      CASE('RCHAMA')
         RCHAMA = Rvar
         GO TO 900
      CASE('RCHAMI')
         RCHAMI = Rvar
         GO TO 900
      CASE('ZCHAMA')
         ZCHAMA = Rvar
         GO TO 900
      CASE('ZCHAMI')
         ZCHAMI = Rvar
         GO TO 900
      CASE('VTHETAPHI')
         VTHETAPHI(I1) = Rvar
         GO TO 910
      CASE('VPERIGEE')
         VPERIGEE(I1) = Rvar
         GO TO 910
      CASE('RCHAMImi')
         RCHAMImi = Rvar
         GO TO 900
      CASE('RCHAMAma')
         RCHAMAma = Rvar
         GO TO 900
      CASE('XDCCAN')
         XDCCAN(I1,I2) = Rvar
         GO TO 920
      CASE('YDCCAN')
         YDCCAN(I1,I2) = Rvar
         GO TO 920
      CASE('ZDCCAN')
         ZDCCAN(I1,I2) = Rvar
         GO TO 920
      CASE('XDCdCAN')
         XDCdCAN(I1,I2) = Rvar
         GO TO 920
      CASE('YDCdCAN')
         YDCdCAN(I1,I2) = Rvar
         GO TO 920
      CASE('ZDCdCAN')
         ZDCdCAN(I1,I2) = Rvar
         GO TO 920
      CASE('ENCCAN')
         ENCCAN(I1,I2) = Rvar
         GO TO 920
      CASE('RDCCAN')
         RDCCAN(I1,I2) = Rvar
         GO TO 920
      CASE('SDCCAN')
         SDCCAN(I1,I2) = Rvar
         GO TO 920
      CASE('XSDCCAN')
         XSDCCAN(I1,I2) = Rvar
         GO TO 920
      CASE('YSDCCAN')
         YSDCCAN(I1,I2) = Rvar
         GO TO 920
      CASE('ZSDCCAN')
         ZSDCCAN(I1,I2) = Rvar
         GO TO 920
      CASE('XSDCdCAN')
         XSDCdCAN(I1,I2) = Rvar
         GO TO 920
      CASE('YSDCdCAN')
         YSDCdCAN(I1,I2) = Rvar
         GO TO 920
      CASE('ZSDCdCAN')
         ZSDCdCAN(I1,I2) = Rvar
         GO TO 920
      CASE('ESNCCAN')
         ESNCCAN(I1,I2) = Rvar
         GO TO 920
      CASE('RSDCCAN')
         RSDCCAN(I1,I2) = Rvar
         GO TO 920
      CASE('ENHCAN')
         ENHCAN(I1,I2) = Rvar
         GO TO 920
      CASE('XHACAN')
         XHACAN(I1,I2) = Rvar
         GO TO 920
      CASE('YHACAN')
         YHACAN(I1,I2) = Rvar
         GO TO 920
      CASE('ZHACAN')
         ZHACAN(I1,I2) = Rvar
         GO TO 920
      CASE('XHdCAN')
         XHdCAN(I1,I2) = Rvar
         GO TO 920
      CASE('YHdCAN')
         YHdCAN(I1,I2) = Rvar
         GO TO 920
      CASE('ZHdCAN')
         ZHdCAN(I1,I2) = Rvar
         GO TO 920
      CASE('XDINTR')
         XDINTR(I1,I2) = Rvar
         GO TO 920
      CASE('YDINTR')
         YDINTR(I1,I2) = Rvar
         GO TO 920
      CASE('ZDINTR')
         ZDINTR(I1,I2) = Rvar
         GO TO 920
      CASE('XDdINTR')
         XDdINTR(I1,I2) = Rvar
         GO TO 920
      CASE('YDdINTR')
         YDdINTR(I1,I2) = Rvar
         GO TO 920
      CASE('ZDdINTR')
         ZDdINTR(I1,I2) = Rvar
         GO TO 920
      CASE('RDINTR')
         RDINTR(I1,I2) = Rvar
         GO TO 920
      CASE('DXINTR')
         DXINTR(I1,I2) = Rvar
         GO TO 920
      CASE('DYINTR')
         DYINTR(I1,I2) = Rvar
         GO TO 920
      CASE('DZINTR')
         DZINTR(I1,I2) = Rvar
         GO TO 920
      CASE('ERINTR')
         ERINTR(I1,I2) = Rvar
         GO TO 920
      CASE('XHINTR')
         XHINTR(I1,I2) = Rvar
         GO TO 920
      CASE('YHINTR')
         YHINTR(I1,I2) = Rvar
         GO TO 920
      CASE('ZHINTR')
         ZHINTR(I1,I2) = Rvar
         GO TO 920
      CASE('XHdINTR')
         XHdINTR(I1,I2) = Rvar
         GO TO 920
      CASE('YHdINTR')
         YHdINTR(I1,I2) = Rvar
         GO TO 920
      CASE('ZHdINTR')
         ZHdINTR(I1,I2) = Rvar
         GO TO 920
      END SELECT
!
890   CONTINUE
      IStatus = 0
      if (Istop.eq.1) then
        PRINT 1000,TRIM(Name),I1,I2,I3
1000    FORMAT(/' MuonBoy routine SetRMuboVarBaz was called with the following arguments :   ',A,3I6,'  =====>  STOP !')
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
 END SUBROUTINE SetRMuboVarBaz
!
!
!
 SUBROUTINE F1GetRMuboVar( Long, Name1, I1, I2, I3, Rvar )
 IMPLICIT NONE
 CHARACTER (Len=1), INTENT(IN)  :: Name1(*)
 INTEGER,           INTENT(IN)  :: Long,I1,I2,I3
 REAL(8),           INTENT(OUT) :: Rvar
 REAL(8), EXTERNAL    :: RMuboVar
 CHARACTER (len=Long) :: VarName
 INTEGER              :: I
!
    DO I=1,Long
      VarName(I:I) = Name1(I)
    ENDDO
    Rvar = RMuboVar( VarName, I1,I2,I3 )
!
 END SUBROUTINE F1GetRMuboVar
!
!
!
 SUBROUTINE F1GetRMuboVarBaz( Istop, IStatus, Long, Name1, I1, I2, I3, Rvar )
 IMPLICIT NONE
 INTEGER,           INTENT(IN)  :: Istop
 INTEGER,           INTENT(OUT) :: IStatus
 CHARACTER (Len=1), INTENT(IN)  :: Name1(*)
 INTEGER,           INTENT(IN)  :: Long,I1,I2,I3
 REAL(8),           INTENT(OUT) :: Rvar
 REAL(8), EXTERNAL    :: RMuboVarBaz
 CHARACTER (len=Long) :: VarName
 INTEGER              :: I
!
    DO I=1,Long
      VarName(I:I) = Name1(I)
    ENDDO
    Rvar = RMuboVarBaz( Istop, IStatus,  VarName, I1,I2,I3 )
!
 END SUBROUTINE F1GetRMuboVarBaz
!
!
!
 SUBROUTINE F1SetRMuboVar( Long, Name1, I1, I2, I3, Rvar )
 IMPLICIT NONE
 CHARACTER (Len=1), INTENT(IN) :: Name1(*)
 INTEGER,           INTENT(IN) :: Long,I1,I2,I3
 REAL(8),           INTENT(IN) :: Rvar
 CHARACTER (len=Long) :: VarName
 INTEGER              :: I
!
    DO I=1,Long
     VarName(I:I) = Name1(I)
    ENDDO
    CALL SetRMuboVar( VarName, I1,I2,I3, Rvar )
!
 END SUBROUTINE F1SetRMuboVar
!
!
!
 SUBROUTINE F1SetRMuboVarBaz( Istop, IStatus, Long, Name1, I1, I2, I3, Rvar )
 IMPLICIT NONE
 INTEGER,           INTENT(IN)  :: Istop
 INTEGER,           INTENT(OUT) :: IStatus
 CHARACTER (Len=1), INTENT(IN) :: Name1(*)
 INTEGER,           INTENT(IN) :: Long,I1,I2,I3
 REAL(8),           INTENT(IN) :: Rvar
 CHARACTER (len=Long) :: VarName
 INTEGER              :: I
!
    DO I=1,Long
      VarName(I:I) = Name1(I)
    ENDDO
    CALL SetRMuboVarBaz( Istop, IStatus, VarName, I1,I2,I3, Rvar )
!
 END SUBROUTINE F1SetRMuboVarBaz
!
