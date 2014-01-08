!
!> \author M.Virchaux
!
 SUBROUTINE PATINI
 USE M_MB_Control
 USE M_MB_PACONS
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER :: I1, I2, I3, IFLAMA
!
!>> Print Muonboy Version
    I1 =            Muonboy_Version / 10000
    I2 = Mod(Muonboy_Version,10000) / 100
    I3 = Mod(Muonboy_Version,100)
    PRINT 100,I1,I2,I3
100 FORMAT(//'  +-------------------------------+',/                 &
             '  |                               |',/                 &
             '  |   ATLAS Muon Reconstruction   |',/                 &
             '  |                               |',/                 &
             '  |   MUONBOY Version ',I2,'.',I2.2,'.',I2.2,'    |',/ &
             '  |                  **********   |',/                 &
             '  |                               |',/                 &
             '  +-------------------------------+',//)
!=============================
    IF( IsSimu < 0 ) THEN  ! for backward compatibility
      IF( AtCosm == 0 ) THEN
        IsSimu = 1
      ELSE
        IsSimu = 0
      ENDIF
    ENDIF
!
    IF( MBPri >= 1 ) WRITE(*,*) ' Muonboy settings : IsSimu AtCosm ',IsSimu,AtCosm
!=============================
!
!>> Pointing errors for roughly projective track search
!>> as a function of the fit type  ( ItypFit = 1 to 6 )
    DRVERT(2) = 12.0000D0
    DZVERT(2) = 20.0000D0
    DRVERT(1) =  5.0000D0
    DZVERT(1) = 14.0000D0
    DRVERT(3:6) =  2.0000D0
    DZVERT(3:6) = 12.0000D0
!
!>> Initialisation of MUONBOY Tracking (Module M_MB_PRECIS) <<<<<<<<<<<<<<<
    CALL Initialise_Mubo_Tracking
    CALL Initialise_Mubo_Field(.true.)
!
!>> Initialisation of Module  M_MB_COMATI  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
!>> i.e. of extra matter (on top of the one from the detectors)
!>> to be taken into account for multiple scattering, i.e. matter
!>> from :   *----------> Dead matter of RPCs (+MDTs ?)
!>>          |*---------> Alignement stuff
!>>          ||*--------> Inner detectors + Solenoid
!>>          |||*-------> Liq. argon calorimeters
!>>          ||||*------> Had. "tile" calorimeters
!>>          |||||*-----> Shieldings
!>>          ||||||*----> Feet
!>>          |||||||*---> End-cap toroids
!>>          ||||||||*--> Barrel toroid
!*  IFLAMA = 000001111
    IFLAMA = 001111111
    CALL MATIERE(IFLAMA, 1)
!
!>> Define in which sequence of stations to perform the pattern recog.
!>>   1     2     3     4     5     6     7     8
!>>  BIS   BIL   BMS   BML   BOS   BOL   BIR   BEE
!>>   9    10    11    12    13    14    15    16    17    18
!>>  EIS   EIL   EES   EEL   EMS   EML   EOS   EOL   FOS   FOL
!>>  19    20
!>>  CSS   CSL
    NSEARC = 0
!----------------------------------------------------------------------------
!>> BOL -> BML -> BIL or BIR
    NSEARC = NSEARC + 1
    LSEARC(NSEARC) = 4
    IFLECH(NSEARC) = 0
    IBORDU(NSEARC) = 0
    ISCEMI(1:4,NSEARC) = (/ 6, 4, 2, 7 /)
!----------------------------------------------------------------------------
!>> BOS -> BMS -> BIS
    NSEARC = NSEARC + 1
    LSEARC(NSEARC) = 3
    IFLECH(NSEARC) = 0
    IBORDU(NSEARC) = 0
    ISCEMI(1:3,NSEARC) = (/ 5, 3, 1 /)
!----------------------------------------------------------------------------
!>> EML -> EOL -> EIL or EEL or CSL
    NSEARC = NSEARC + 1
    LSEARC(NSEARC) = 5
    IFLECH(NSEARC) = 1
    IF( AtCosm /= 0 ) IFLECH(NSEARC) = 0
    IBORDU(NSEARC) = 0
    ISCEMI(1:5,NSEARC) = (/ 14, 16, 10, 12, 20 /)
!----------------------------------------------------------------------------
!>> EMS -> EOS -> EES or EIS or CSS
    NSEARC = NSEARC + 1
    LSEARC(NSEARC) = 5
    IFLECH(NSEARC) = 0
    IBORDU(NSEARC) = 0
    ISCEMI(1:5,NSEARC) = (/ 13, 15, 11, 9, 19 /)
!----------------------------------------------------------------------------
!>> EML -> EIL -> EEL or BIL or BIR
    NSEARC = NSEARC + 1
    LSEARC(NSEARC) = 5
    IFLECH(NSEARC) = 0
    IBORDU(NSEARC) = 0
    ISCEMI(1:5,NSEARC) = (/ 14, 10, 12, 2, 7 /)
!----------------------------------------------------------------------------
!>> EMS -> EIS -> EES or BIS
    NSEARC = NSEARC + 1
    LSEARC(NSEARC) = 4
    IFLECH(NSEARC) = 0
    IBORDU(NSEARC) = 0
    ISCEMI(1:4,NSEARC) = (/ 13, 9, 11, 1 /)
!----------------------------------------------------------------------------
!>> EMS -> BMS -> EES or BIS or BOS
    NSEARC = NSEARC + 1
    LSEARC(NSEARC) = 5
    IFLECH(NSEARC) = 0
    IBORDU(NSEARC) = 0
    ISCEMI(1:5,NSEARC) = (/ 13,  3, 11, 1, 5 /)
!----------------------------------------------------------------------------
!>> EML -> BML -> EEL or BIL or BIR or BOL
    NSEARC = NSEARC + 1
    LSEARC(NSEARC) = 6
    IFLECH(NSEARC) = 0
    IBORDU(NSEARC) = 0
    ISCEMI(1:6,NSEARC) = (/ 14, 4, 12, 2, 7, 6 /)
!----------------------------------------------------------------------------
    IF( AtCosm == 0 ) THEN
!>> EML -> EOL -> EIL or EEL or CSL
!>> ===>  recover possible track that would have been found with Iflech = 0
      NSEARC = NSEARC + 1
      LSEARC(NSEARC) = 5
      IFLECH(NSEARC) = -1
      IBORDU(NSEARC) = 0
      ISCEMI(1:5,NSEARC) = (/ 14, 16, 10, 12, 20 /)   
    ENDIF
!----------------------------------------------------------------------------
!>> BOL -> EIL -> BIL or BIR or  EEL or EML
    NSEARC = NSEARC + 1
    LSEARC(NSEARC) = 6
    IFLECH(NSEARC) = 0
    IBORDU(NSEARC) = 0
    ISCEMI(1:6,NSEARC) = (/ 6, 10, 2, 7, 12, 14 /)
!----------------------------------------------------------------------------
!>> BOS -> EIS -> BIS or EES or EMS
    NSEARC = NSEARC + 1
    LSEARC(NSEARC) = 5
    IFLECH(NSEARC) = 0
    IBORDU(NSEARC) = 0
    ISCEMI(1:5,NSEARC) = (/ 5, 9, 1, 11, 13 /)
!--------------- Now L - S transitions --------------------------------------
!>> BOS -> BML -> BIL or BIR
    NSEARC = NSEARC + 1
    LSEARC(NSEARC) = 4
    IFLECH(NSEARC) = 0
    IBORDU(NSEARC) = 1
    ISCEMI(1:4,NSEARC) = (/ 5, 4, 2, 7 /)
!----------------------------------------------------------------------------
!>> BOL -> BMS -> BIS
    NSEARC = NSEARC + 1
    LSEARC(NSEARC) = 3
    IFLECH(NSEARC) = 0
    IBORDU(NSEARC) = 1
    ISCEMI(1:3,NSEARC) = (/ 6, 3, 1 /)
!----------------------------------------------------------------------------
!>> EML -> EIS -> BIS or EEL or CSS
    NSEARC = NSEARC + 1
    LSEARC(NSEARC) = 5
    IFLECH(NSEARC) = 0
    IBORDU(NSEARC) = 1
    ISCEMI(1:5,NSEARC) = (/ 14, 9, 1, 12, 19 /)
!>> EMS -> EIL -> BIL or BIR or EES or CSL
    NSEARC = NSEARC + 1
    LSEARC(NSEARC) = 6
    IFLECH(NSEARC) = 0
    IBORDU(NSEARC) = 1
    ISCEMI(1:6,NSEARC) = (/ 13, 10, 2, 7, 11, 20 /)
!----------------------------------------------------------------------------
    NSCSC = NSEARC + 1
!----------------------------------------------------------------------------
    IF( AtCosm /= 1 .AND. .NOT.UseAllSeg ) THEN
!>> ===> recover the CSC region not covered by TGC
!>> EML -> CSL -> EOL    with loose segments
      NSEARC = NSEARC + 1
      LSEARC(NSEARC) = 3
      IFLECH(NSEARC) = 1
      IBORDU(NSEARC) = 0
      ISCEMI(1:3,NSEARC) = (/ 14, 20, 16 /)
!----------------------------------------------------------------------------
!>> EMS -> CSS -> EOS    with loose segments
      NSEARC = NSEARC + 1
      LSEARC(NSEARC) = 3
      IFLECH(NSEARC) = 1
      IBORDU(NSEARC) = 0
      ISCEMI(1:3,NSEARC) = (/ 13, 19, 15 /)
    ENDIF
!----------------------------------------------------------------------------
    IF(NSEARC > NSMAX) THEN
      WRITE(*,'("Augmenter NSMAX dans module M_MB_PACONS a ",I3)') NSEARC
      STOP 'Augmenter NSMAX dans module M_MB_PACONS'
    ENDIF
!----------------------------------------------------------------------------
    IF( AtCosm /= 0 ) IBORDU(1:NSEARC) = 0
!----------------------------------------------------------------------------
!
 END SUBROUTINE PATINI
!
