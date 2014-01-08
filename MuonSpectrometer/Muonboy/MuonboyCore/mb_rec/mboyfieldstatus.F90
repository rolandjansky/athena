!-----------------------------------------------------
 SUBROUTINE MboySetFieldStatus (                    &
                                getToroidBarrelOn , &
                                getToroidECTAOn   , &
                                getToroidECTCOn   , &
                                getSolenoidOn     , &
                                getAllToroidOn    , &
                                getFieldStatusOn    &
                               )
 USE M_MB_FieldStatus
 IMPLICIT NONE
 
 INTEGER, INTENT(IN) :: getToroidBarrelOn 
 INTEGER, INTENT(IN) :: getToroidECTAOn  
 INTEGER, INTENT(IN) :: getToroidECTCOn  
 INTEGER, INTENT(IN) :: getSolenoidOn    
 INTEGER, INTENT(IN) :: getAllToroidOn   
 INTEGER, INTENT(IN) :: getFieldStatusOn 

 NewFieldStatus = 1

 ToroidBarrelOn = getToroidBarrelOn 
 ToroidECTAOn   = getToroidECTAOn  
 ToroidECTCOn   = getToroidECTCOn  
 SolenoidOn     = getSolenoidOn    
 AllToroidOn    = getAllToroidOn   
 FieldStatusOn  = getFieldStatusOn 

 END SUBROUTINE MboySetFieldStatus
!-----------------------------------------------------
 SUBROUTINE Initialise_Mubo_Field(alwaysDoIt)
 USE M_MB_Control
 USE M_MB_FieldStatus
 USE M_MB_PRECIS
 IMPLICIT NONE
 LOGICAL, INTENT(IN) :: alwaysDoIt
 REAL(8), PARAMETER :: FieldRef=0.499d0
 REAL(8) :: X0,Y0,Z0, BX0,BY0,BZ0
 INTEGER :: I0
 LOGICAL :: doIt
! 
   doIt = alwaysDoIt .OR. NewFieldStatus == 1
   IF( doIt ) THEN
     IF( MBPri >= 1 ) CALL printFieldStatus
!>> Run with field or not ?
     IF( NewFieldStatus >= 0 ) THEN
       NoField = .NOT. ( ToroidBarrelOn /= 0 .OR. ToroidECTAOn /= 0 .OR. ToroidECTCOn /= 0 )
       NewFieldStatus = 0
     ENDIF
     IF( ABS(FixMo) > 0.1d0 ) THEN  !>> Fix momentum if asked for
       FixMoCur = FixMo
     ELSE IF( NoField ) THEN        !>> Or fix it in case of no field
       FixMoCur = MomNoField
     ELSE
       FixMoCur = 0.d0              !>> Otherwise keep momentum free
     ENDIF
!>> Find sign of magnetic field
     X0 =   10.D0
     Y0 =  650.D0
     Z0 =    0.D0
     CALL BFIELD(X0,Y0,Z0,BX0,BY0,BZ0)
     SFIELD = - SIGN( 1.d0, BX0 )
     RFIELD = SQRT( BX0**2 + BY0**2 + BZ0**2 )
     IF( MBPri >= 1 ) WRITE(*,*) ' Reference value of field ',FieldRef,' and current value ',RFIELD
     RFIELD = RFIELD / FieldRef
!>> Find max.value of |Z| with non-zero mag. field
     X0 =   50.D0
     Y0 =  550.D0
     AZBMIN = 1400.d0
     DO I0=1,100
       Z0 = 1400.D0 + I0*10.D0
       CALL BFIELD(X0,Y0,Z0,BX0,BY0,BZ0)
       IF( SQRT(BX0**2+BY0**2+BZ0**2) > BBMIN )  AZBMIN = Z0 + STEP(4)
       IF( SQRT(BX0**2+BY0**2+BZ0**2) < BBNUL )  EXIT
     ENDDO
     AZBNUL = Z0
     PAMAXB = 300.d0
     IF(MBPri >= 1) THEN
       WRITE(*,'(/," >>> The magnetic field is considered small   for :  Abs(Z) >",F7.1," cm")') AZBMIN
       WRITE(*,'(" >>> The magnetic field is considered as null for :  Abs(Z) >",F7.1," cm",/)') AZBNUL
     ENDIF
   ENDIF
!
 END SUBROUTINE Initialise_Mubo_Field
