!
!> \author M.Virchaux 
!
 REAL(8) FUNCTION RFTTUB(Ident,MboyMdtStatus, &
 RadTrack,                &
 XXXDCA, YYYDCA, ZZZDCA , &
 XTrdir, YTrdir, ZTrdir , &
 XWiCen, YWiCen, ZWiCen , &
 XWidir, YWidir, ZWidir)
 USE M_MB_Control, ONLY : MBPri, RimRTSvc
#if defined(ATLAS_STAND)
 USE M_MB_T0fit
#endif
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Ident
 REAL(8) :: RadTrack
 REAL(8), INTENT(IN) :: XXXDCA, YYYDCA, ZZZDCA   !< coordinates at DCA
 REAL(8), INTENT(IN) :: XTrdir, YTrdir, ZTrdir   !< track direction at DCA
 REAL(8) :: XWiCen, YWiCen, ZWiCen 
 REAL(8) :: XWidir, YWidir, ZWidir
 REAL(8) :: Radius
 INTEGER :: MboyMdtStatus
!
#if defined(ATLAS_STAND)

   CALL RFTTUBCS( Ident, &
                  XXXDCA, YYYDCA, ZZZDCA, &
                  XTrdir, YTrdir, ZTrdir, &
                  XWiCen, YWiCen, ZWiCen, &
                  XWidir, YWidir, ZWidir, &
                  Radius)
   MboyMdtStatus = -1
   IF( modifyT0 ) Radius = Radius + drfromt0
#else

   CALL MUONBOYRFT( RimRTSvc, &
                    Ident, &
                    XXXDCA, YYYDCA, ZZZDCA, &
                    XTrdir, YTrdir, ZTrdir, &
                    XWiCen, YWiCen, ZWiCen, &
                    XWidir, YWidir, ZWidir, &
                    Radius , &
                    MboyMdtStatus)

#endif
!
   IF( Radius < 0.d0 ) THEN
     IF( MBPri >= 1 ) WRITE(*,*) ' Warning RFTTUB with negative radius ',Radius
     Radius = 0.d0
   ENDIF
   RFTTUB = Radius
   
   if (RimRTSvc.ne.0) then
     CALL COMPIMPACT( XXXDCA, YYYDCA, ZZZDCA, &
                      XTrdir, YTrdir, ZTrdir, &
                      XWiCen, YWiCen, ZWiCen, &
                      XWidir, YWidir, ZWidir, &
                      RadTrack)
   endif
!
 END FUNCTION RFTTUB
!
!
!
!
 REAL(8) FUNCTION RFTfast(IT,SS, RadTrack, &
                  XXXDCA, YYYDCA, ZZZDCA , &
                  XTrdir, YTrdir, ZTrdir)
 USE M_MB_Control
 USE M_MB_Digis
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IT
 REAL(8), INTENT(IN) :: SS 
 REAL(8) :: RadTrack
 REAL(8), INTENT(IN) :: XXXDCA, YYYDCA, ZZZDCA  !< coordinates at DCA
 REAL(8), INTENT(IN) :: XTrdir, YTrdir, ZTrdir  !< track direction at DCA
 INTEGER :: MboyMdtStatus, Ident
 REAL(8) :: XWiCen, YWiCen, ZWiCen 
 REAL(8) :: XWidir, YWidir, ZWidir
 REAL(8) :: RFTTUB
!
   IF( LFastRT ) THEN
     IF( RTUBHI(IT) < -0.5D0 .OR. ABS(STUBHI(IT)-SS) > DsFastRT ) THEN
       Ident = ITUBHI(IT)
       RFTfast = RFTTUB(Ident, MboyMdtStatus,  &
                               RadTrack,       &
                               XXXDCA, YYYDCA, ZZZDCA, &
                               XTrdir, YTrdir, ZTrdir, &
                               XWiCen, YWiCen, ZWiCen, &
                               XWidir, YWidir, ZWidir)
       STUBHI(IT) = SS
       IF( RTUBHI(IT) < -0.5d0 ) THEN
         IF( LTUBHI(IT) == 10 ) THEN
           IF( MboyMdtStatus >= 3 ) THEN
             LTUBHI(IT) = MboyMdtStatus
           ELSE
             LTUBHI(IT) = 0
           ENDIF
         ELSE IF( MboyMdtStatus >= 0 ) THEN
           LTUBHI(IT) = MboyMdtStatus
         ENDIF
       ENDIF
       RTUBHI(IT) = RFTfast       
       SaWiXPT(IT) = XWiCen
       SaWiYPT(IT) = YWiCen
       SaWiZPT(IT) = ZWiCen
       SaWiXDI(IT) = XWidir
       SaWiYDI(IT) = YWidir
       SaWiZDI(IT) = ZWidir
     ELSE
       RFTfast = RTUBHI(IT)
       if (RimRTSvc.ne.0) then
         CALL COMPIMPACT( XXXDCA, YYYDCA, ZZZDCA, &
                          XTrdir, YTrdir, ZTrdir, &
                          SaWiXPT(IT), SaWiYPT(IT), SaWiZPT(IT), &
                          SaWiXDI(IT), SaWiYDI(IT), SaWiZDI(IT), &
                          RadTrack)
       endif
     ENDIF
   ELSE
     Ident   = ITUBHI(IT)
     RFTfast =   RFTTUB(Ident, MboyMdtStatus,  &
                               RadTrack,       &
                               XXXDCA, YYYDCA, ZZZDCA, &
                               XTrdir, YTrdir, ZTrdir, &
                               XWiCen, YWiCen, ZWiCen, &
                               XWidir, YWidir, ZWidir)
     IF( LTUBHI(IT) == 10 ) THEN
       IF( MboyMdtStatus >= 3 ) THEN
         LTUBHI(IT) = MboyMdtStatus
       ELSE
         LTUBHI(IT) = 0
       ENDIF
     ELSE IF( MboyMdtStatus >= 0 ) THEN
       LTUBHI(IT) = MboyMdtStatus
     ENDIF
   ENDIF
!
   RETURN
 END FUNCTION RFTfast
!
!
!
!> Compute impact parameter taken into account geometrical wire sag
!
 SUBROUTINE COMPIMPACT( &
                    XXXDCA, YYYDCA, ZZZDCA, &
                    XTrdir, YTrdir, ZTrdir, &
                    XWiCen, YWiCen, ZWiCen, &
                    XWidir, YWidir, ZWidir, &
                    RadTrack)
!
 IMPLICIT NONE
 REAL(8), INTENT(IN)   :: XXXDCA, YYYDCA, ZZZDCA   !< coordinates at DCA
 REAL(8), INTENT(IN)   :: XTrdir, YTrdir, ZTrdir   !< track direction at DCA
 REAL(8), INTENT(IN)   :: XWiCen, YWiCen, ZWiCen 
 REAL(8), INTENT(IN)   :: XWidir, YWidir, ZWidir
 REAL(8) :: RadTrack
 REAL(8) :: vT(3), vW(3), vTW(3), vR(3)
 REAL(8) :: dotTW, lamda, RadTrackNew
!REAL(8) :: ptN(3)
!CHARACTER(3)   :: CHA3 
!

 vT(1:3) = (/ XTrdir , YTrdir , ZTrdir /)
 call NORM3V(vT)
 vW(1:3) = (/XWidir          , YWidir         , ZWidir          /)
 call NORM3V(vW)
 vTW(1:3) = (/XWiCen - XXXDCA, YWiCen - YYYDCA, ZWiCen - ZZZDCA /)

 dotTW = dot_product(vT,vW)
 lamda = dot_product(vTW,vT) - dot_product(vTW,vW) * dotTW
 lamda = lamda / ( 1.d0 - dotTW**2 )

 vR(1) = - lamda * ( vT(2) * vW(3) - vT(3) * vW(2) ) + ( vTW(2) * vW(3) - vTW(3) * vW(2) )
 vR(2) =   lamda * ( vT(1) * vW(3) - vT(3) * vW(1) ) - ( vTW(1) * vW(3) - vTW(3) * vW(1) )
 vR(3) = - lamda * ( vT(1) * vW(2) - vT(2) * vW(1) ) + ( vTW(1) * vW(2) - vTW(2) * vW(1) )
 RadTrackNew = sqrt( dot_product(vR,vR) )
 
!ptN(1) = XXXDCA + lamda * vT(1)
!ptN(2) = YYYDCA + lamda * vT(2)
!ptN(3) = ZZZDCA + lamda * vT(3)
   
   
!CHA3 = '   '
!if ( abs(RadTrack-RadTrackNew).gt.0.0001 ) CHA3 = 'W01'
!if ( abs(RadTrack-RadTrackNew).gt.0.0002 ) CHA3 = 'W02'
!if ( abs(RadTrack-RadTrackNew).gt.0.0002 ) CHA3 = 'W02'
!if ( abs(RadTrack-RadTrackNew).gt.0.0003 ) CHA3 = 'W03'
!if ( abs(RadTrack-RadTrackNew).gt.0.0004 ) CHA3 = 'W04'
!if ( abs(RadTrack-RadTrackNew).gt.0.0005 ) CHA3 = 'W05'
!if ( abs(RadTrack-RadTrackNew).gt.0.0006 ) CHA3 = 'W06'
!if ( abs(RadTrack-RadTrackNew).gt.0.0007 ) CHA3 = 'W07'
!if ( abs(RadTrack-RadTrackNew).gt.0.0008 ) CHA3 = 'W08'
!if ( abs(RadTrack-RadTrackNew).gt.0.0009 ) CHA3 = 'W09'
!if ( abs(RadTrack-RadTrackNew).gt.0.0010 ) CHA3 = 'W10'
!write (*,'(" COMPIMPACT ",3F10.4,A3)')  RadTrack,RadTrackNew,RadTrack-RadTrackNew,CHA3

 RadTrack = RadTrackNew

!
 END SUBROUTINE COMPIMPACT
!
