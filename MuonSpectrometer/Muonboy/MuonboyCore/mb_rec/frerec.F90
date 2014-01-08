!
!> Effective tube resolution taken during track reconstruction
!
 REAL(8) FUNCTION FREREC(IGEOM,IC,INOF,INOZ, RRR)
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IGEOM,IC,INOF,INOZ
 REAL(8), INTENT(IN) :: RRR
 INTEGER :: IMDT
 REAL(8) :: FREDIG
 LOGICAL :: isMisaligned, isBadlyBuilt, alreadyIncreased
!
   IMDT  = ISTMDTV(IGEOM)
   alreadyIncreased = .false.
   FREREC = FREDIG(RRR) + RESOTU(IMDT)   
   IF( ItypFit /= 3 .AND. ItypFit /= 6 ) THEN
     FREREC = SQRT( FREREC**2 + RESTUB(IMDT)**2 - RESOTU(IMDT)**2 )
   ELSE
     IF( IC <= NCOUBA ) THEN ! increase tube resolution in barrel-endcap overlap if needed
       IF( increaseErrorBarrel ) THEN
         FREREC = SQRT( FREREC**2 + ResBadAli**2 )
         alreadyIncreased = .true.
       ENDIF
     ELSE
       IF( increaseErrorEndcap ) THEN
         FREREC = SQRT( FREREC**2 + ResBadAli**2 )
         alreadyIncreased = .true.
       ENDIF
     ENDIF
   ENDIF
!
   IF( UseT0fit ) THEN  ! increase tube resolution if T0 fit failed
     IF( dT0val(IGEOM) > 900.d0 )  FREREC = SQRT( FREREC**2 + ReTubNoT0**2 )
   ENDIF
!
   IF( .NOT.alreadyIncreased ) THEN
     IF( isMisaligned(IC,INOF,INOZ) ) THEN  ! increase tube resolution if badly aligned station
       FREREC = SQRT( FREREC**2 + ResBadCha(IC)**2 )
     ELSE IF( isBadlyBuilt(IC,INOF,INOZ) ) THEN ! increase tube resolution if badly built
       FREREC = SQRT( FREREC**2 + ResBadTomo**2 )
     ENDIF
   ENDIF
!
 END FUNCTION FREREC
!
