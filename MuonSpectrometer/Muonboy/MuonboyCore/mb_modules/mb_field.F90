!
 MODULE M_MB_FieldStatus
    IMPLICIT NONE
!
!>> Tell if the current information fursnished or not and if fursnished, if it has been already used or not
!>>  -1 no information furnished
!>>   0 This information has been already used
!>>   1 This information has not been already used
    INTEGER, SAVE :: NewFieldStatus = -1
!
    INTEGER, SAVE :: ToroidBarrelOn 
    INTEGER, SAVE :: ToroidECTAOn  
    INTEGER, SAVE :: ToroidECTCOn  
    INTEGER, SAVE :: SolenoidOn    
    INTEGER, SAVE :: AllToroidOn   
    INTEGER, SAVE :: FieldStatusOn
  CONTAINS
    SUBROUTINE printFieldStatus
      WRITE(*,*)
      IF( NewFieldStatus == -1 ) THEN
        WRITE(*,'(" >>> MB_FieldStatus data NOT furnished")')
      ELSE
        WRITE(*,'(" >>> MB_FieldStatus data furnished")')
        IF( NewFieldStatus == 0 ) THEN
          WRITE(*,'("     AND used already ")')
        ELSE
          WRITE(*,'("     AND not yet used ")')
        ENDIF
        WRITE(*,'("  ToroidBarrelOn = ",I3)') ToroidBarrelOn
        WRITE(*,'("  ToroidECTAOn   = ",I3)') ToroidECTAOn  
        WRITE(*,'("  ToroidECTCOn   = ",I3)') ToroidECTCOn  
        WRITE(*,'("  SolenoidOn     = ",I3)') SolenoidOn    
        WRITE(*,'("  AllToroidOn    = ",I3)') AllToroidOn   
        WRITE(*,'("  FieldStatusOn  = ",I3)') FieldStatusOn 
      ENDIF
    END SUBROUTINE printFieldStatus
!
 END MODULE M_MB_FieldStatus
!
 MODULE M_MB_PRECIS
  IMPLICIT NONE
  REAL(8), SAVE :: STEP(4), BBNUL, BBMIN, DBMAX, DBMIN
  REAL(8), SAVE :: FACREF, FACMIN, FACMAX
  REAL(8), SAVE :: PASMIN, AZBNUL, AZBMIN, PAMAXB
  REAL(8), SAVE :: SFIELD       ! sign of field
  REAL(8), SAVE :: RFIELD=1.d0  ! value of field ratio
 END MODULE M_MB_PRECIS
!
