!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE CscWireLayerModel(Iprint,                          &
                              IAmdcVersion,                    &
                              Ctyp,Jff,Jzz,Job,                &
                              SzzttCc,SzzttCcF,SzzttCcR,       &
                              NberWireLayer,CUFOAMG10,GASGAP,  &
                              ttLayer)
 IMPLICIT NONE
 INTEGER,      INTENT(IN)    :: Iprint
 INTEGER,      INTENT(IN)    :: IAmdcVersion
 CHARACTER(3), INTENT(IN)    :: Ctyp
 INTEGER,      INTENT(IN)    :: Jff,Jzz,Job
 REAL(8),      INTENT(IN)    :: SzzttCc(3)
 REAL(8),      INTENT(IN)    :: SzzttCcF(3)
 REAL(8),      INTENT(IN)    :: SzzttCcR(3)
 INTEGER,      INTENT(IN)    :: NberWireLayer
 REAL(8),      INTENT(IN)    :: CUFOAMG10,GASGAP
 REAL(8),      INTENT(OUT)   :: ttLayer(2,4)
 REAL(8) :: zzCc ,ttCc ,SCc
 REAL(8) :: zzCcF,ttCcF,SCcF
 REAL(8) :: zzCcR,ttCcR,SCcR
 REAL(8) :: TTUTU(4), TTUTUM, SCUFOAMG10, SGASGAP, ShiftLocal
 INTEGER :: L, ikind
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
      SCc  =SzzttCc(1) 
      zzCc =SzzttCc(2) 
      ttCc =SzzttCc(3) 
      SCcF =SzzttCcF(1)
      zzCcF=SzzttCcF(2)
      ttCcF=SzzttCcF(3)
      SCcR =SzzttCcR(1)
      zzCcR=SzzttCcR(2)
      ttCcR=SzzttCcR(3)
!
!--------------------------------------------------------------------
!
      if( Iprint == 1 ) then
       print *,'*************CscWireLayerModel Inputs'
       PRINT *,' Ctyp    ',Ctyp
       PRINT *,' Jff     ',Jff
       PRINT *,' Jzz     ',Jzz
       PRINT *,' Job     ',Job
       print *,'*************'
       PRINT *,' IAmdcVersion ',IAmdcVersion
       print *,'*************'
       PRINT *,' SCc ',SCc  
       PRINT *,' zzCc ',zzCc  
       PRINT *,' ttCc ',ttCc  
       print *,'*************'
       PRINT *,' SCcF ',SCcF  
       PRINT *,' zzCcF ',zzCcF  
       PRINT *,' ttCcF ',ttCcF  
       print *,'*************'
       PRINT *,' SCcR ',SCcR  
       PRINT *,' zzCcR ',zzCcR  
       PRINT *,' ttCcR ',ttCcR  
       print *,'*************'
       PRINT *,' NberWireLayer   ',NberWireLayer  
       print *,'*************'
       PRINT *,' CUFOAMG10   ',CUFOAMG10  
       PRINT *,' GASGAP      ',GASGAP  
       print *,'*************'
      endif
!      
!--------------------------------------------------------------------
!-- Wire Layer Model: ttLayer is the coordinate along tt (after rotation)
!                           of the Wire Layer
!
      IF( IAmdcVersion < 7 ) THEN
       TTUTUM = 0.D0
       DO L=1,NberWireLayer
        TTUTU(L)  = L*( CUFOAMG10 + GASGAP )
        TTUTUM    = TTUTUM + TTUTU(L)
       ENDDO
       TTUTUM    = TTUTUM / NberWireLayer
       DO L=1,NberWireLayer
         TTUTU(L)  = TTUTU(L) - TTUTUM
       ENDDO
       DO L=1,NberWireLayer
        ttLayer(1,L) = ttCc + SIGN( 1.D0 , ttCc ) * TTUTU(L)
        ttLayer(2,L) = ttLayer(1,L)
       ENDDO
      ELSE 
!
       SCUFOAMG10 = CUFOAMG10 * SIGN( 1.D0 , ttCc )
       SGASGAP    = GASGAP    * SIGN( 1.D0 , ttCc )
!
!-- Take into account that GeoModel piles up in the other direction
!-- add that up to know (Thu Oct 19 2006) the thickness of the full volunes
!-- is not equal to the sum of the inner volumes thicknesses
!
       ShiftLocal = ttCcR - ( ttCcF + 5.d0*SCUFOAMG10 + 4.d0*SGASGAP )
!
       ttLayer(1,1) = ttCcF        + SCUFOAMG10 + ShiftLocal
       ttLayer(2,1) = ttLayer(1,1) + SGASGAP
!
       ttLayer(2,2) = ttLayer(2,1) + SCUFOAMG10
       ttLayer(1,2) = ttLayer(2,2) + SGASGAP
!
       ttLayer(1,3) = ttLayer(1,2) + SCUFOAMG10
       ttLayer(2,3) = ttLayer(1,3) + SGASGAP
!
       ttLayer(2,4) = ttLayer(2,3) + SCUFOAMG10
       ttLayer(1,4) = ttLayer(2,4) + SGASGAP
!
      ENDIF
!
!--------------------------------------------------------------------
!
      if (Iprint.eq.1) then
       print *,'*************CscWireLayerModel Outputs'
       PRINT *,' ttLayer '
       Do ikind=1,2
        PRINT *,'*ikind ',ikind
        DO L=1,NberWireLayer
         PRINT *,'  ',L,' ',ttLayer(ikind,L)
        ENDDO
       ENDDO
       print *,'*************'
      endif
!
!--------------------------------------------------------------------
!      
 END SUBROUTINE CscWireLayerModel
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE CscStripValid(IStatus, Ctyp,Jff,Jzz,Job,Jsl,SZflag)
 IMPLICIT NONE
 INTEGER,      INTENT(OUT) :: IStatus
 CHARACTER(3), INTENT(IN)  :: Ctyp
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job,Jsl
 REAL(8),      INTENT(IN)  :: SZflag
 INTEGER :: Jtyp, Jgeo, Jtec, Jsta, Nsl
 INTEGER :: JTROU3
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
      IStatus = 0
      Jtyp = JTROU3(Ctyp,MTYP,CARTYP)
      IF( Jtyp < 1 )               RETURN
      Jgeo = IGEO_V(JTYP_F_Z(Jtyp,Jff,Jzz))
      IF( Jgeo < 1 )               RETURN
      Jtec = ITEC_V(JTYP_G_O(Jtyp,Jgeo,Job))
      IF( CARTEC(Jtec) /= 'CSC' )  RETURN
      Jsta = ISTA_V(JTYP_G_O(Jtyp,Jgeo,Job))
      IF( Jsta < 1 )               RETURN
      Nsl  = NLAZ(Jtec,Jsta)
      IF( Jsl < 1 .OR. Jsl > Nsl ) RETURN
      IStatus = 1
!
 END SUBROUTINE CscStripValid
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE CscPhi2ndCoStripModel(Iprint,                                         &
                                  IAmdcVersion,                                   &
                                  Ctyp,Jff,Jzz,Job,Jsl,JStrip,                    &
                                  SzzttCc,SzzttCcF,SzzttCcR,                      &
                                  S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,  &
                                  PitchStrip,                                     &
                                  ttLayer,                                        &
                                  ShiftStrip,                                     &
                                  SzzttStrip,XYZStrip)
 IMPLICIT NONE
 INTEGER,      INTENT(IN)    :: Iprint
 INTEGER,      INTENT(IN)    :: IAmdcVersion
 CHARACTER(3), INTENT(IN)    :: Ctyp
 INTEGER,      INTENT(IN)    :: Jff,Jzz,Job,Jsl,JStrip
 REAL(8),      INTENT(IN)    :: SzzttCc(3)
 REAL(8),      INTENT(IN)    :: SzzttCcF(3)
 REAL(8),      INTENT(IN)    :: SzzttCcR(3)
 REAL(8),      INTENT(IN)    :: S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap
 REAL(8),      INTENT(IN)    :: Zoop,Ziip,Zaap
 REAL(8),      INTENT(IN)    :: PitchStrip
 REAL(8),      INTENT(IN)    :: ttLayer(2,4)              
 REAL(8),      INTENT(OUT)   :: ShiftStrip
 REAL(8),      INTENT(OUT)   :: SzzttStrip(3,5)
 REAL(8),      INTENT(OUT)   :: XYZStrip(3,5)
 INTEGER :: NbStrips2ndCo
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
      CALL CscPhi2ndCoStripModelBase(Iprint,                                          &
                                     IAmdcVersion,                                    &
                                     Ctyp,Jff,Jzz,Job,Jsl,JStrip,                     &
                                     SzzttCc,SzzttCcF,SzzttCcR,                       &
                                     S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,   &
                                     PitchStrip,                                      &
                                     ttLayer,                                         &
                                     ShiftStrip,                                      &
                                     SzzttStrip,XYZStrip,                             &
                                     NbStrips2ndCo)
!      
 END SUBROUTINE CscPhi2ndCoStripModel
!            
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE CscPhi2ndCoStripModelBase(Iprint,                                         &
                                           IAmdcVersion,                                   &
                                           Ctyp,Jff,Jzz,Job,Jsl,JStrip,                    &
                                           SzzttCc,SzzttCcF,SzzttCcR,                      &
                                           S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,  &
                                           PitchStrip,                                     &
                                           ttLayer,                                        &
                                           ShiftStrip,                                     &
                                           SzzttStrip,XYZStrip,                            &
                                           NbStrips2ndCo)
!
      IMPLICIT REAL(8) (A-H,O-Z)                         
!
      INTEGER,      INTENT(IN)    :: Iprint
      INTEGER,      INTENT(IN)    :: IAmdcVersion
      CHARACTER(3), INTENT(IN)    :: Ctyp
      INTEGER,      INTENT(IN)    :: Jff,Jzz,Job,Jsl,JStrip
      REAL(8),      INTENT(IN)    :: SzzttCc(3)
      REAL(8),      INTENT(IN)    :: SzzttCcF(3)
      REAL(8),      INTENT(IN)    :: SzzttCcR(3)
      REAL(8),      INTENT(IN)    :: S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap
      REAL(8),      INTENT(IN)    :: Zoop,Ziip,Zaap
      REAL(8),      INTENT(IN)    :: PitchStrip
      REAL(8),      INTENT(IN)    :: ttLayer(2,4)              
      REAL(8),      INTENT(OUT)   :: ShiftStrip
      REAL(8),      INTENT(OUT)   :: SzzttStrip(3,5)
      REAL(8),      INTENT(OUT)   :: XYZStrip(3,5)
      INTEGER,      INTENT(OUT)   :: NbStrips2ndCo
!
      REAL(8) :: SStrip,zzStrip,ttStrip
      REAL(8) :: SStripSPzzP,zzStripSPzzP
      REAL(8) :: SStripSPzzM,zzStripSPzzM
      REAL(8) :: SStripSMzzM,zzStripSMzzM
      REAL(8) :: SStripSMzzP,zzStripSMzzP
      REAL(8) :: zzCc ,ttCc ,SCc
      REAL(8) :: zzCcF,ttCcF,SCcF
      REAL(8) :: zzCcR,ttCcR,SCcR
      REAL(8) :: zzStripP
      REAL(8) :: zzStripM
      REAL(8) :: ttPi     
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
      SCc  =SzzttCc(1) 
      zzCc =SzzttCc(2) 
      ttCc =SzzttCc(3) 
      SCcF =SzzttCcF(1)
      zzCcF=SzzttCcF(2)
      ttCcF=SzzttCcF(3)
      SCcR =SzzttCcR(1)
      zzCcR=SzzttCcR(2)
      ttCcR=SzzttCcR(3)
      ttPi=ttLayer(1,JSL)
!
!--------------------------------------------------------------------
!
      if (Iprint.eq.1) then
       print *,'*************CscPhi2ndCoStripModelBase Inputs'
       PRINT *,' Ctyp    ',Ctyp
       PRINT *,' Jff     ',Jff
       PRINT *,' Jzz     ',Jzz
       PRINT *,' Job     ',Job
       PRINT *,' Jsl     ',Jsl
       PRINT *,' JStrip  ',JStrip
       print *,'*************'
       PRINT *,' IAmdcVersion ',IAmdcVersion
       print *,'*************'
       PRINT *,' SCc ',SCc  
       PRINT *,' zzCc ',zzCc  
       PRINT *,' ttCc ',ttCc  
       print *,'*************'
       PRINT *,' SCcF ',SCcF  
       PRINT *,' zzCcF ',zzCcF  
       PRINT *,' ttCcF ',ttCcF  
       print *,'*************'
       PRINT *,' SCcR ',SCcR  
       PRINT *,' zzCcR ',zzCcR  
       PRINT *,' ttCcR ',ttCcR  
       print *,'*************'
       PRINT *,' S1Pp   ',S1Pp  
       PRINT *,' S2Pip  ',S2Pip  
       PRINT *,' S2Po1p ',S2Po1p  
       PRINT *,' S2Po2p ',S2Po2p  
       PRINT *,' S2Pap  ',S2Pap  
       PRINT *,' Ziip   ',Ziip  
       PRINT *,' Zoop   ',Zoop  
       PRINT *,' Zaap   ',Zaap  
       print *,'*************'
       PRINT *,' PitchStrip   ',PitchStrip  
       print *,'*************'
       PRINT *,' ttLayer '
       Do ikind=1,2
        PRINT *,'*ikind ',ikind
        DO L=1,4
         PRINT *,'  ',L,' ',ttLayer(ikind,L)
        ENDDO
       ENDDO
       print *,'*************'
      endif
!      
!--------------------------------------------------------------------
!-- ShiftStrip is the shift along S for Phi-2nd coordinate CSC strip
!
      IF( IAmdcVersion < 7 ) THEN
       WidthToBeMapped = 2.d0*( S1Pp + (S2Pip-S1Pp) * Zoop/2.D0/Ziip )
      ELSE
       WidthToBeMapped = 2.d0 * S2Pip
      ENDIF
!
      NbStrips2ndCo = NINT(WidthToBeMapped/PitchStrip)
!
      IF( IAmdcVersion >= 7 ) THEN
!-- To avoid rounding problems
       NbStrips2ndCo=48 
      ENDIF
!
      WidthMapped = NbStrips2ndCo*PitchStrip
!
      ShiftStrip = SCc - WidthMapped/2.d0 + PitchStrip/2.D0
!
!-- SStrip, zzStrip and ttStrip
!--  are the coordinates of the Phi-2nd coordinate CSC strip JStrip
!-- Coordonnates of strips corners
!--  SStripSPzzP and zzStripSPzzP at max S and max zz
!--  SStripSPzzM and zzStripSPzzM at max S and min zz
!--  SStripSMzzM and zzStripSMzzM at min S and min zz
!--  SStripSMzzP and zzStripSMzzP at min S and max zz
!
      SStrip = ShiftStrip + real((JStrip-1),8)*PitchStrip
!
      IF( IAmdcVersion < 7 ) THEN
!
       zzStrip = zzCc
!
       zzInput = zzStrip - zzCc
!
       SInput= SStrip - SCc + PitchStrip/2.d0
       ZRelocated =  Zoop/2.d0
       call CscGeoNonParallel(Ctyp,                                           &
                              ZRelocated,                                     &
                              S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,  &
                              SInput,zzInput,                                 &
                              RelocatedS,Relocatedzz )
        SStripSPzzP=SCc+RelocatedS
       zzStripSPzzP=zzCc+Relocatedzz
!
       SInput= SStrip - SCc + PitchStrip/2.d0
       ZRelocated = -Zoop/2.d0
       call CscGeoNonParallel(Ctyp,                                           &
                              ZRelocated,                                     &
                              S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,  &
                              SInput,zzInput,                                 &
                              RelocatedS,Relocatedzz )
        SStripSPzzM=SCc+RelocatedS
       zzStripSPzzM=zzCc+Relocatedzz
!
       SInput= SStrip - SCc - PitchStrip/2.d0
       ZRelocated = -Zoop/2.d0
       call CscGeoNonParallel(Ctyp,                                           &
                              ZRelocated,                                     &
                              S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,  &
                              SInput,zzInput,                                 &
                              RelocatedS,Relocatedzz)
        SStripSMzzM=SCc+RelocatedS
       zzStripSMzzM=zzCc+Relocatedzz
!
       SInput= SStrip - SCc - PitchStrip/2.d0
       ZRelocated = Zoop/2.d0
       call CscGeoNonParallel(Ctyp,                                           &
                              ZRelocated,                                     &
                              S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,  &
                              SInput,zzInput,                                 &
                              RelocatedS,Relocatedzz)
        SStripSMzzP=SCc+RelocatedS
       zzStripSMzzP=zzCc+Relocatedzz
!
      ELSE
!
       SInput  = SStrip - SCc
       call CscGeoTRange(Ctyp,                                            &
                         S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,   &
                         SInput,                                          &
                         zzStripP,zzStripM)
       zzStrip = zzCc + ( zzStripP + zzStripM )/2.d0
!
        SStripSPzzP=SStrip+PitchStrip/2.d0
       zzStripSPzzP=zzCc+zzStripP
!
        SStripSPzzM=SStrip+PitchStrip/2.d0
       zzStripSPzzM=zzCc+zzStripM
!
        SStripSMzzM=SStrip-PitchStrip/2.d0
       zzStripSMzzM=zzCc+zzStripM
!
        SStripSMzzP=SStrip-PitchStrip/2.d0
       zzStripSMzzP=zzCc+zzStripP
!
      ENDIF
!      
      ttStrip = ttPi
!
!-- Center
      SzzttStrip(1,5) = SStrip
      SzzttStrip(2,5) = zzStrip
      SzzttStrip(3,5) = ttStrip
!
!-- Point A, B, C and D
!    DA  41
!    CB  32
!
      SzzttStrip(1,1) =  SStripSPzzP
      SzzttStrip(2,1) = zzStripSPzzP
      SzzttStrip(3,1) = ttStrip
!
      SzzttStrip(1,2) =  SStripSPzzM
      SzzttStrip(2,2) = zzStripSPzzM
      SzzttStrip(3,2) = ttStrip
!
      SzzttStrip(1,3) =  SStripSMzzM
      SzzttStrip(2,3) = zzStripSMzzM
      SzzttStrip(3,3) = ttStrip
!
      SzzttStrip(1,4) =  SStripSMzzP
      SzzttStrip(2,4) = zzStripSMzzP
      SzzttStrip(3,4) = ttStrip
!
!-- Global coordinates
!
      DO J=1,5
       call CSCLocalToGlobal( Ctyp,Jff,Jzz,Job,Jsl, SzzttStrip(1,J), XYZStrip(1,J) )
      ENDDO
! 
!--------------------------------------------------------------------
!
      if (Iprint.eq.1) then
       print *,'*************CscPhi2ndCoStripModelBase Outputs'
       PRINT *,' WidthToBeMapped ',WidthToBeMapped
       PRINT *,' NbStrips2ndCo ',NINT(WidthToBeMapped/PitchStrip)
       PRINT *,' NbStrips2ndCo ',NbStrips2ndCo
       PRINT *,' WidthMapped ',WidthMapped
       PRINT *,' ShiftStrip ',ShiftStrip
       PRINT *,' SStrip  ',SStrip
       PRINT *,' zzStrip ',zzStrip
       PRINT *,' ttStrip ',ttStrip
       do i1=1,4
        PRINT *,' point ',i1
        do i2=1,3
         PRINT *,'       ',SzzttStrip(i2,i1)-SzzttStrip(i2,5)
        enddo
       enddo
       i1=5
        PRINT *,' point ',i1
        do i2=1,3
         PRINT *,'       ',SzzttStrip(i2,i1)
        enddo
       print *,'*************'
      endif
!
      END SUBROUTINE CscPhi2ndCoStripModelBase
!            
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE CSCLocalToGlobal( Ctyp,Jff,Jzz,Job,Jsl, SZT, XYZ )
!
      IMPLICIT NONE
      CHARACTER(3), INTENT(IN)    :: Ctyp
      INTEGER,      INTENT(IN)    :: Jff,Jzz,Job,Jsl
      REAL(8),      INTENT(INOUT) :: SZT(3), XYZ(3)
!
!     REAL(8) :: SZT_Test(3) 
!
!--------------------------------------------------------------------
! Perform  Local  to Global coordinates transformation
!
! Ctyp,Jff,Jzz,Job  are the AMDC_SIMREC indexes defining the CSC multilayer
!  Jsl   is the CSC strips monolayer index (1 to 3 or 4)
!
! SZT are Local coordinates
! XYZ are Global coordinates
!
!--------------------------------------------------------------------
!
      call CSCGlobalAndLocal( 1, Ctyp,Jff,Jzz,Job,Jsl, SZT, XYZ )
!
!     print *,'TEST CSCLocalToGlobal ==========================='
!     call CSCGlobalToLocal( Ctyp,Jff,Jzz,Job,Jsl, SZT_Test, XYZ )
!     print *,'TEST CSCLocalToGlobal 1 ',&
!     ' ',SZT(1),' ',SZT_Test(1),' ',SZT_Test(1)-SZT(1)
!     print *,'TEST CSCLocalToGlobal 2 ',&
!     ' ',SZT(2),' ',SZT_Test(2),' ',SZT_Test(2)-SZT(2)
!     print *,'TEST CSCLocalToGlobal 3 ',&
!     ' ',SZT(3),' ',SZT_Test(3),' ',SZT_Test(3)-SZT(3)
!     if ( abs(SZT_Test(1)-SZT(1)).ge.0.0000001 &
!      .or.abs(SZT_Test(2)-SZT(2)).ge.0.0000001 &
!      .or.abs(SZT_Test(3)-SZT(3)).ge.0.0000001 ) then
!       print *,'TEST CSCLocalToGlobal ARRRRGH ' 
!     endif
!
      END SUBROUTINE CSCLocalToGlobal
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE CSCGlobalToLocal( Ctyp,Jff,Jzz,Job,Jsl, SZT, XYZ )
! 
      IMPLICIT NONE
!
      CHARACTER(3), INTENT(IN) :: Ctyp
      INTEGER,      INTENT(IN) :: Jff,Jzz,Job,Jsl
      REAL(8)                  :: SZT(3), XYZ(3)
!--------------------------------------------------------------------
! Perform Global to Local  coordinates transformation
!
! Ctyp,Jff,Jzz,Job  are the AMDC_SIMREC indexes defining the CSC multilayer
!  Jsl   is the CSC strips monolayer index (1 to 3 or 4)
!
! SZT are Local coordinates
! XYZ are Global coordinates
!--------------------------------------------------------------------
!
      call CSCGlobalAndLocal( 2, Ctyp,Jff,Jzz,Job,Jsl, SZT, XYZ )
!   
      END SUBROUTINE CSCGlobalToLocal
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE CSCGlobalAndLocal( ILocalToGlobal, Ctyp,Jff,Jzz,Job,Jsl, SZT, XYZ )
!
      IMPLICIT NONE
!
      INTEGER,      INTENT(IN) :: ILocalToGlobal
      CHARACTER(3), INTENT(IN) :: Ctyp
      INTEGER,      INTENT(IN) :: Jff,Jzz,Job,Jsl
      REAL(8)                  :: SZT(3), XYZ(3)
!
      INTEGER :: Iprint
      INTEGER :: IAmdcVersion
      REAL(8) :: SZTFixed(3),SZTCc(3)
      REAL(8) :: SzzttCc(3),SzzttCcF(3),SzzttCcR(3)
      REAL(8) :: S1Pb,S2Pib,S2Po1b,S2Po2b,S2Pab      
      REAL(8) :: Ziib,Zoob,Zaab
      REAL(8) :: S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap
      REAL(8) :: Ziip,Zoop,Zaap
      REAL(8) :: PitchEta1stStrip,PitchPhi2ndStrip
      INTEGER :: NberWireLayer
      REAL(8) :: CUFOAMG10,GASGAP
      REAL(8) :: Gama0,FOs 
      REAL(8) :: COSAA,SINAA, DCF,DSF
      REAL(8) :: GdSZT(3) 
      REAL(8) :: SZTin(3)   , SZTout(3)
!--------------------------------------------------------------------
! ILocalToGlobal: if equal to 1, perform Local  to Global coordinates transformation
!                 otherwise      perform Global to Local  coordinates transformation
!
! Ctyp,Jff,Jzz,Job  are the AMDC_SIMREC indexes defining the CSC multilayer
!  Jsl   is the CSC strips monolayer index (1 to 3 or 4)
!
! SZT are Local coordinates
! XYZ are Global coordinates
!
!JFL vendredi 26 juin 2009, 18:42:46 (UTC+0200)
! Here local means ss/zz/tt system
!--------------------------------------------------------------------
!
      Iprint = 0
      if( Iprint == 1 ) then
        PRINT 1000,Ctyp,Jff,Jzz,Job,Jsl
1000    format(/'     /Name/Jff/Jzz/ Job/Jsl/Jtube/',A3,5I4)
      endif
!
!--Basics of Csc Geometry
      call CscGeoBasics(Iprint, Ctyp,Jff,Jzz,Job, IAmdcVersion,          &     
                        SZTFixed,SZTCc,  SzzttCc,SzzttCcF,SzzttCcR,      &
                        S1Pb,S2Pib,S2Po1b,S2Po2b,S2Pab,Ziib,Zoob,Zaab,   &
                        S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,   &
                        PitchEta1stStrip,PitchPhi2ndStrip,               &
                        NberWireLayer,CUFOAMG10,GASGAP,                  &
                        Gama0,FOs)
!
!--Coordinates transformation
!--Gama0 is gamma for EC>0 and -gamma for EC<0, i.e for nominal CSC always negative
      COSAA = COS(Gama0)
      SINAA = SIN(Gama0)
      DCF   = COS(FOs)
      DSF   = SIN(FOs)
!
!-- Local  to Global coordinates transformation
      if( ILocalToGlobal == 1 ) then
! 
!----- SZTin are coordinates in a system where nominal CSC layers are at  SZTin(3)=cte
!----- SZTin is indirect; this is the Mboy system
        SZTin(1:3) = SZT(1:3)
!
        CALL FPerformICscDirLocDisplacAndDeform(Ctyp,Jff,Jzz,Job,Jsl,SZTin,SZTout)
!
!----- GdSZT are coordinates in the SZT Amdc system (fig 1 Amdc manual)
        GdSZT(1) =         SZTout(1)
        GdSZT(2) = SINAA * SZTout(2) + COSAA * SZTout(3) 
        GdSZT(3) = COSAA * SZTout(2) - SINAA * SZTout(3)
!
        XYZ(1) = - DSF * GdSZT(1) + DCF * GdSZT(3)
        XYZ(2) =   DSF * GdSZT(3) + DCF * GdSZT(1)
        XYZ(3) =         GdSZT(2)
!
!-- Global to Local  coordinates transformation
      else
!        
!----- GdSZT are coordinates in the SZT Amdc system (fig 1 Amdc manual)
        GdSZT(1) = - DSF * XYZ(1) + DCF * XYZ(2)
        GdSZT(2) =         XYZ(3)
        GdSZT(3) =   DCF * XYZ(1) + DSF * XYZ(2)
!        
!----- SZTin are coordinates in a system where nominal CSC layers are at  SZTin(3)=cte
!----- SZTin is indirect; this is the Mboy system
        SZTin(1) =         GdSZT(1)
        SZTin(2) = SINAA * GdSZT(2) + COSAA * GdSZT(3) 
        SZTin(3) = COSAA * GdSZT(2) - SINAA * GdSZT(3)
!
        CALL FPerformICscBakLocDisplacAndDeform(Ctyp,Jff,Jzz,Job,Jsl,SZTin,SZTout)
!
        SZT(1:3) = SZTout(1:3)
!
      endif
!    
      END SUBROUTINE CSCGlobalAndLocal
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE CscGeoTRange(Ctyp, S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,   &
                              Ziip,Zoop,Zaap, Sstrip,                 &
                              TstripP,TstripM)
!
      IMPLICIT NONE
      CHARACTER(3), INTENT(IN)  :: Ctyp
      REAL(8),      INTENT(IN)  :: S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap
      REAL(8),      INTENT(IN)  :: Zoop,Ziip,Zaap
      REAL(8),      INTENT(IN)  :: Sstrip
      REAL(8),      INTENT(OUT) :: TstripP,TstripM
!
      REAL(8) :: aSstrip, CornerLow, CornerUp 
      REAL(8) :: CornerMed, cotanal, cotanbe
!--------------------------------------------------------------------
!TstripP and TstripM are the Max and Min T value
! reached at Sstrip S value for a CSC chamber 
! the shape of which is defined by S1Pp,S2Pip,S2Po1p,Zoop,Ziip
!--------------------------------------------------------------------
!
      aSstrip = dabs(Sstrip)
!
      CornerLow = -Zoop/2.d0
      CornerUp  =  Zoop/2.d0
      CornerMed =  Ziip-Zoop/2.d0
      cotanal = Ziip/(S2Pip-S1Pp)
!
      TstripP = CornerUp
      IF( CTYP(3:3) == 'L' ) then
        cotanbe = (Ziip-Zoop)/(S2Pip-S2Po1p)
        if( aSstrip >= S2Po1p .and. aSstrip <= S2Pip ) TstripP = CornerUp + cotanbe*(aSstrip-S2Po1p)
      endif
      if( aSstrip >= S2Pip ) TstripP = CornerMed
!
      TstripM = CornerLow
      if( aSstrip >= S1Pp .and. aSstrip <= S2Pip ) TstripM = CornerLow + cotanal*(aSstrip-S1Pp)
      if( aSstrip >= S2Pip ) TstripM = CornerMed
!
      END SUBROUTINE CscGeoTRange
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE CscGeoSRange( Ctyp,                                           &
                               S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,  &
                               Tstrip,SstripP,SstripM)
!
      IMPLICIT NONE
      CHARACTER(3), INTENT(IN)  :: Ctyp
      REAL(8),      INTENT(IN)  :: S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap
      REAL(8),      INTENT(IN)  :: Zoop,Ziip,Zaap
      REAL(8),      INTENT(IN)  :: Tstrip
      REAL(8),      INTENT(OUT) :: SstripP,SstripM
!
      REAL(8) :: CornerLow, CornerUp, CornerMed, tanal, tanbe
!--------------------------------------------------------------------
!SstripP and SstripM are the Max and Min S value
! reached at Tstrip T value for a CSC chamber 
! the shape of which is defined by S1Pp,S2Pip,S2Po1p,Zoop,Ziip
!--------------------------------------------------------------------
!
      CornerLow = -Zoop/2.d0
      CornerUp  =  Zoop/2.d0
      CornerMed = Ziip-Zoop/2.d0
      tanal = (S2Pip-S1Pp)/Ziip
!
      SstripP = S1Pp
      IF( CTYP(3:3) == 'L' ) then
        tanbe = (S2Po1p-S2Pip)/(Zoop-Ziip)
        if( Tstrip >= CornerLow .and. Tstrip <= CornerMed ) SstripP = S1Pp + (Tstrip-CornerLow)*tanal
        if( Tstrip >= CornerMed .and. Tstrip <= CornerUp )  SstripP = S2Pip + (Tstrip-CornerMed)*tanbe
      else
        if( Tstrip >= CornerLow .and. Tstrip <= CornerUp )  SstripP = S1Pp+  (Tstrip-CornerLow)*tanal
      endif
      if( Tstrip >= CornerUp ) SstripP = S2Po1p
!
      SstripM = -SstripP
!
      END SUBROUTINE CscGeoSRange
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE CscGeoNonParallel(Ctyp,                                           &
                                   ZRelocated,                                     &
                                   S1Pb,S2Pib,S2Po1b,S2Po2b,S2Pab,Ziib,Zoob,Zaab,  &
                                   SInput,zzInput,                                 &
                                   RelocatedS,Relocatedzz)
!
      IMPLICIT REAL(8) (A-H,O-Z)                                 
      CHARACTER(3), INTENT(IN)  :: Ctyp
      REAL(8),      INTENT(IN)  :: ZRelocated
      REAL(8),      INTENT(IN)  :: S1Pb,S2Pib,S2Po1b,S2Po2b,S2Pab
      REAL(8),      INTENT(IN)  :: Ziib,Zoob,Zaab
      REAL(8),      INTENT(IN)  :: SInput
      REAL(8),      INTENT(IN)  :: zzInput
      REAL(8),      INTENT(OUT) :: RelocatedS
      REAL(8),      INTENT(OUT) :: Relocatedzz
!
      REAL(8) :: SZT(3),SZTRelocated(3)
      REAL(8) :: AA,AM1,AP1
!--------------------------------------------------------------------
!For a CSC chamber the shae of which is defined by 
! S1Pb,S2Pib,S2Po1b,Zoob,Ziib
! RelocatedS,Relocatedzz are the S and zz 
! coordinates of the point transfered parallely
! from the point SInput,zzInput
!
!--------------------------------------------------------------------
!
      SZT(1)=SInput
      SZT(2)=zzInput
!
      S1P =   S1Pb
      S1N = - S1Pb
      S2P =   S2Po2b
      S2N = - S2Po2b
      DZZ = Zoob/2.d0
!
      SCc = 0.d0
      AA  = (S2P-S2N) / (S1P-S1N)
      AM1 = AA - 1.D0
      AP1 = AA + 1.D0
!
!--Coordinates transformation
!      
      SZTRelocated(2) = ZRelocated
      SZTRelocated(1) =  SZT(1)                                                      &
                      + (SZT(2)-SZTRelocated(2)) * (AA*(S2P-S1P)+AM1*(SZT(1)-S2P))   &
                                                / (-AM1*SZT(2)-AP1*DZZ)
!
      if (ZRelocated.ge.(Ziib-DZZ)) then
       S3Pb=S2Pib
       IF (CTYP(3:3).eq.'L') S3Pb = S3Pb + (ZRelocated-(Ziib-DZZ))*(S2Po1b-S2Pib)/(Zoob-Ziib)
       S3P=SCc+S3Pb
       S3N=SCc-S3Pb
       if (SZTRelocated(1).gt.S3P) then
        Slope0=0.
        IF (CTYP(3:3).eq.'L') Slope0=(S2Po1b-S2Pib)/(Zoob-Ziib)
        Vinte0=SCc+S2Pib
       endif
       if (SZTRelocated(1).lt.S3N) then
        Slope0=0.
        IF (CTYP(3:3).eq.'L') Slope0=-(S2Po1b-S2Pib)/(Zoob-Ziib)
        Vinte0=SCc-S2Pib
       endif
       if( SZTRelocated(1) > S3P .or. SZTRelocated(1) < S3N ) then
        Origi0=Ziib-DZZ
        Slope1=0.
        if( ZRelocated.ne.SZT(2) ) Slope1=(SZTRelocated(1)-SZT(1))/(ZRelocated-SZT(2))
        Vinte1=SZT(1)
        Origi1=SZT(2)
        Zint=0.
        if (Slope0.ne.Slope1) then
         Zint=(Vinte1-Origi1*Slope1)-(Vinte0-Origi0*Slope0)
         Zint=Zint/(Slope0-Slope1)
        endif
        SZTRelocated(2) = Zint
        SZTRelocated(1) =  SZT(1)                                                     &
                        + (SZT(2)-SZTRelocated(2)) * (AA*(S2P-S1P)+AM1*(SZT(1)-S2P))  &
                                                  / (-AM1*SZT(2)-AP1*DZZ)
       endif
      endif
!
      RelocatedS =SZTRelocated(1)
      Relocatedzz=SZTRelocated(2)
!
      END SUBROUTINE CscGeoNonParallel
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE CscGeoBasics(Iprint,                                        &
                              Ctyp,Jff,Jzz,Job,                              &
                              IAmdcVersion,                                  &
                              SZTFixed,SZTCc,                                &
                              SzzttCc,SzzttCcF,SzzttCcR,                     &
                              S1Pb,S2Pib,S2Po1b,S2Po2b,S2Pab,Ziib,Zoob,Zaab, &
                              S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap, &
                              PitchEta1stStrip,PitchPhi2ndStrip,             &
                              NberWireLayer,CUFOAMG10,GASGAP,                &
                              Gama0,FOs)                  
!
      IMPLICIT REAL(8) (A-H,O-Z)                      
      INTEGER,      INTENT(IN)    :: Iprint
      CHARACTER(3), INTENT(IN)    :: Ctyp
      INTEGER,      INTENT(IN)    :: Jff,Jzz,Job
      INTEGER,      INTENT(OUT)   :: IAmdcVersion
      REAL(8),      INTENT(OUT)   :: SZTFixed(3)
      REAL(8),      INTENT(OUT)   :: SZTCc(3)
      REAL(8),      INTENT(OUT)   :: SzzttCc(3)
      REAL(8),      INTENT(OUT)   :: SzzttCcF(3)
      REAL(8),      INTENT(OUT)   :: SzzttCcR(3)
      REAL(8),      INTENT(OUT)   :: S1Pb,S2Pib,S2Po1b,S2Po2b,S2Pab
      REAL(8),      INTENT(OUT)   :: Ziib,Zoob,Zaab
      REAL(8),      INTENT(OUT)   :: S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap
      REAL(8),      INTENT(OUT)   :: Zoop,Ziip,Zaap
      REAL(8),      INTENT(OUT)   :: PitchEta1stStrip
      REAL(8),      INTENT(OUT)   :: PitchPhi2ndStrip
      INTEGER,      INTENT(OUT)   :: NberWireLayer
      REAL(8),      INTENT(OUT)   :: CUFOAMG10,GASGAP
      REAL(8),      INTENT(OUT)   :: Gama0,FOs                  
!
      REAL(8) :: zzCc ,ttCc ,SCc
      REAL(8) :: zzCcF,ttCcF,SCcF
      REAL(8) :: zzCcR,ttCcR,SCcR
      REAL(8) :: FreeSpace
      REAL(8) :: SFreeSpaceLower
      REAL(8) :: SFreeSpaceUpper
      REAL(8) :: TFreeSpaceLower
      REAL(8) :: TFreeSpaceUpper
      REAL(8) :: COSAA
      REAL(8) :: SINAA
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
#include "AmdcStand/copipi.inc"
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
!--------------------------------------------------------------------
!
      if (Iprint.eq.1) then
       print *,'*************CscGeoBasics Inputs'
       PRINT *,' Ctyp    ',Ctyp
       PRINT *,' Jff     ',Jff
       PRINT *,' Jzz     ',Jzz
       PRINT *,' Job     ',Job
       print *,'*************'
      endif
!      
!--------------------------------------------------------------------
!
      Jtyp = JTROU3(Ctyp,MTYP,CARTYP)
      Jgeo = IGEO(Jtyp,Jff,Jzz)
      Jtec = ITEC(Jtyp,Jgeo,Job)
      Jsta = ISTA(Jtyp,Jgeo,Job)
!
!--------------------------------------------------------------------
!
      ONE    = 1.D0
!
!--------------------------------------------------------------------
!
      IAmdcVersion = IVERSION_AMDC
!
!--------------------------------------------------------------------
!FOs, ZOs, TOs and SOs give the Station Origin
!
      FOs = Pos_Phi(JTYP,JFF,JZZ)
      ZOs = Pos_Z(JTYP,JFF,JZZ)
      TOs = Pos_R(JTYP,JFF,JZZ)
      SOs = Pos_S(JTYP,JFF,JZZ)
!
!--------------------------------------------------------------------
!Signs:
! SIZOs   = -1/1  Z<0   /Z>0
! SBAREL  = -1/1  Barrel/EndCap
! SISOs   = -1/1  S.s<0 /S.s<0 after IAmdcVersion=6 (i.e from IAmdcVersion=7)
!             1  otherwise                  
!
      SIZOs = SIGN( ONE , ZOs )
!
      SBAREL = ONE
      IF( IBAREL(JTYP,JFF,JZZ) < 1 ) SBAREL = -ONE
!
      SISOs = ONE
      IF( IBAREL(JTYP,JFF,JZZ) < 1 .AND. IAmdcVersion > 6 .AND. JZZ > 0 ) SISOs = -ONE
!
!--------------------------------------------------------------------
!
      call Dim_Station(JTYP,JFF,JZZ,                      &
                       SC_Station,ZC_Station,TC_Station,  &
                       DS_Station,DZ_Station,DT_Station)
      SC_Station          = SC_Station * 10.d0
      ZC_Station          = ZC_Station * 10.d0
      TC_Station          = TC_Station * 10.d0
      DS_Station          = DS_Station * 10.d0
      DZ_Station          = DZ_Station * 10.d0
      DT_Station          = DT_Station * 10.d0
      call Get_CSC_SZdim(JTYP,JFF,JZZ,JOB,              &
                         S1Pb, S2Pib, S2Po1b,S2Po2b,    &
                         S2Pab, Ziib,Zoob,Zaab )
      S1Pb                = S1Pb       * 10.d0
      S2Pib               = S2Pib      * 10.d0
      S2Po1b              = S2Po1b     * 10.d0
      S2Po2b              = S2Po2b     * 10.d0
      S2Pab               = S2Pab      * 10.d0
      Ziib                = Ziib       * 10.d0
      Zoob                = Zoob       * 10.d0
      Zaab                = Zaab       * 10.d0
!
!--------------------------------------------------------------------
!Gama0 is the resigned gama angle 
!
      Gama0 = SIZOs * Pos_Gama(JTYP,JFF,JZZ) * PIS180
      COSAA = COS(Gama0)
      SINAA = SIN(Gama0)
!
!--------------------------------------------------------------------
!ZCs and TCs are the coordinates of the CSC Station Center
!before gamma rotation
!
      ZCs = ZOs + SIZOs * TC_Station
      TCs = TOs +         ZC_Station
!
!--------------------------------------------------------------------
!Rotation Model: ZFixed and TFixed are the coordinates of the point
!                fixed under the rotation by al/be/ga
!
      IF( IAmdcVersion < 7 ) THEN
        ZFixed = ZCs
        TFixed = TCs
      ELSE
        ZFixed = ZOs
        TFixed = TOs
      ENDIF
!
      SZTFixed(1) = 0.d0
      SZTFixed(2) = ZFixed
      SZTFixed(3) = TFixed
!
!--------------------------------------------------------------------
!ZCc, TCc and SCc are the coordinates of the CSC chamber Center
!before gamma rotation
!
      ZCc = ZOs + SIZOs * Geo_dz(JTYP,JGEO,JOB) + SIZOs * STAEE(JTEC,JSTA) / 2.D0
      TCc = TOs + Geo_dy(JTYP,JGEO,JOB) + Zoob/2.D0
      IF(ABS(Pos_Alfa(JTYP,JFF,JZZ)-180.D0) < 0.1D0 .AND. IAmdcVersion < 7 ) THEN
        SCc = SOs - SBAREL*Geo_dx(JTYP,JGEO,JOB)
      ELSE
        IF( IAmdcVersion < 7 ) THEN
          SCc = SOs + SBAREL*Geo_dx(JTYP,JGEO,JOB)
        ELSE
          SCc = SOs +  SISOs*Geo_dx(JTYP,JGEO,JOB)
        ENDIF
      ENDIF
!
      SZTCc(1) = SCc
      SZTCc(2) = ZCc
      SZTCc(3) = TCc
!
!zzCc and ttCc are the coordinates of the CSC chamber Center
!after gamma rotation
!
      zzCc = SINAA*ZFixed + COSAA*TFixed - TFixed + TCc
      ttCc = COSAA*ZFixed - SINAA*TFixed - ZFixed + ZCc
!
      SzzttCc(1) = SCc
      SzzttCc(2) = zzCc
      SzzttCc(3) = ttCc
!
!--------------------------------------------------------------------
!ZCcF,TCcF and SCcF are the coordinates of the center of the Front of the CSC chamber 
!before gamma rotation
!
      ZCcF = ZCc - SIZOs * STAEE(JTEC,JSTA)     /2.D0
      TCcF = TCc
      SCcF = SCc
!
!zzCcF and ttCcF are the coordinates of the center of the Front of the CSC chamber 
!after gamma rotation
!
      zzCcF = SINAA*ZFixed + COSAA*TFixed - TFixed + TCcF
      ttCcF = COSAA*ZFixed - SINAA*TFixed - ZFixed + ZCcF
!
      SzzttCcF(1) = SCcF
      SzzttCcF(2) = zzCcF
      SzzttCcF(3) = ttCcF
!
!--------------------------------------------------------------------
!ZCcR, TCc Rand SCcR are the coordinates of the center of the Rear of the CSC chamber 
!before gamma rotation
!
      ZCcR = ZCc + SIZOs * STAEE(JTEC,JSTA) / 2.D0
      TCcR = TCc
      SCcR = SCc
!
!zzCcR and ttCcR are the coordinates of the center of the Rear of the CSC chamber 
!after gamma rotation
!
      zzCcR = SINAA*ZFixed + COSAA*TFixed - TFixed + TCcR
      ttCcR = COSAA*ZFixed - SINAA*TFixed - ZFixed + ZCcR
!
      SzzttCcR(1) = SCcR
      SzzttCcR(2) = zzCcR
      SzzttCcR(3) = ttCcR
!
!--------------------------------------------------------------------
!FreeSpace is the space free of strips ond the
! perimeter of the Csc chamber
!SFreeSpaceLower and SFreeSpaceUpper are
! the space free of strips on the mounting edges projected on S 
! for the lower and upper part of the Csc chamber respectively
!
      IF( IAmdcVersion < 7 ) THEN
        FreeSpace = STATT(JTEC,JSTA,11)
      ELSE
        FreeSpace = STATT(JTEC,JSTA,17) + STAOO(JTEC,JSTA,2) + STAOO(JTEC,JSTA,3)
      ENDIF
      SFreeSpaceLower = FreeSpace * sqrt(1.d0+((S2Pib-S1Pb)/Ziib)**2)
      TFreeSpaceLower = FreeSpace * sqrt(1.d0+(Ziib/(S2Pib-S1Pb))**2)
      SFreeSpaceUpper = 0.d0
      TFreeSpaceUpper = FreeSpace
      if(Zoob .ne. Ziib ) then
        SFreeSpaceUpper = FreeSpace * sqrt(1.d0+((S2Pib-S2Po1b)/(Zoob-Ziib))**2)
        TFreeSpaceUpper = FreeSpace * sqrt(1.d0+((Zoob-Ziib)/(S2Pib-S2Po1b))**2)
      endif
!
!--------------------------------------------------------------------
!S1Pp, S2Pip, S2Po1p, S2Po2p, S2Pap, Zaap, Zoop and Ziip
! defined the active part of the Csc chamber 
!
      IF (CTYP(3:3).eq.'L') then
       tanal = (S2Pib-S1Pb)/Ziib
       tanbe = (S2Pib-S2Po1b)/(Zoob-Ziib)
       Zoop=Zoob-2.d0*FreeSpace
       S1Pp  =S1Pb  +tanal*FreeSpace-SFreeSpaceLower
       S2Po1p=S2Po1b+tanbe*FreeSpace-SFreeSpaceUpper
       Ziip=tanbe*Zoop+S2Po1p-S1Pp
       Ziip=Ziip/(tanal+tanbe)
       S2Pip=S1Pp+tanal*Ziip
       Zaap   = Ziip + (Zoop-Ziip)*S2Pip/(S2Pip-S2Po1p)
      else
       tanal = (S2Pib-S1Pb)/Ziib
       Zoop=Zoob-2.d0*FreeSpace
       Ziip=Zoop
       S1Pp =S1Pb+tanal*FreeSpace-SFreeSpaceLower
       S2Pip=S1Pp+tanal*Ziip
       S2Po1p=S2Pip
       Zaap   = Ziip 
      endif
      S2Po2p = S1Pp + (S2Pip-S1Pp)*Zoop/Ziip
      S2Pap  = S1Pp + (S2Pip-S1Pp)*Zaap/Ziip
!
!--------------------------------------------------------------------
!PitchPhi2ndStrip is the Strip pitch along the "second coordinate" 
!
      PitchPhi2ndStrip  = STAOO(JTEC,JSTA,4)
      IF( PitchPhi2ndStrip < 0.01D0 ) PitchPhi2ndStrip = 20.0D0
!
!--------------------------------------------------------------------
!PitchEta1stStrip is the Cathode readout pitch along the "precise coordinate"
!
      PitchEta1stStrip  = STATT(JTEC,JSTA,6)
!      
!--------------------------------------------------------------------
!NberWireLayer is the Number of sensitive layers in one CSC multilayer
!
      NberWireLayer = NLAZ(JTEC,JSTA)
!
!--------------------------------------------------------------------
!CUFOAMG10 is the thickness of Cu, Foam and G10
!GASGAP    is the thickness of Gas Gap
!
      CUFOAMG10 = STATT(JTEC,JSTA,2) + 2.D0*STATT(JTEC,JSTA,3)
      GASGAP    = 2.D0*STATT(JTEC,JSTA,5)
!      
!--------------------------------------------------------------------
!
      if (Iprint.eq.1) then
       print *,'*************CscGeoBasics Outputs'
       PRINT *,' IAmdcVersion ',IAmdcVersion
       print *,'*************'
       PRINT *,'*FOs ',FOs
       PRINT *,' ZOs ',ZOs
       PRINT *,' TOs ',TOs
       PRINT *,' SOs ',SOs
       print *,'*************'
       PRINT *,' S1Pb   ',S1Pb  
       PRINT *,' S2Pib  ',S2Pib 
       PRINT *,' S2Po1b ',S2Po1b
       PRINT *,' S2Po2b ',S2Po2b
       PRINT *,' S2Pab  ',S2Pab 
       PRINT *,' Ziib   ',Ziib  
       PRINT *,' Zoob   ',Zoob  
       PRINT *,' Zaab   ',Zaab  
       print *,'*************'
       PRINT *,' Gama0   ',Gama0/PIS180 
       print *,'*************'
       PRINT *,' ZCs   ',ZCs  
       PRINT *,' TCs   ',TCs  
       print *,'*************'
       PRINT *,' ZFixed   ',ZFixed  
       PRINT *,' TFixed   ',TFixed  
       print *,'*************'
       PRINT *,' ZCc ',ZCc  
       PRINT *,' TCc ',TCc  
       PRINT *,' SCc ',SCc  
       PRINT *,' zzCc ',zzCc  
       PRINT *,' ttCc ',ttCc  
       print *,'*************'
       PRINT *,' ZCcF ',ZCcF  
       PRINT *,' TCcF ',TCcF  
       PRINT *,' SCcF ',SCcF  
       PRINT *,' zzCcF ',zzCcF  
       PRINT *,' ttCcF ',ttCcF  
       print *,'*************'
       PRINT *,' ZCcR ',ZCcR  
       PRINT *,' TCcR ',TCcR  
       PRINT *,' SCcR ',SCcR  
       PRINT *,' zzCcR ',zzCcR  
       PRINT *,' ttCcR ',ttCcR  
       print *,'*************'
       PRINT *,' FreeSpace ',FreeSpace  
       PRINT *,' SFreeSpaceLower ',SFreeSpaceLower  
       PRINT *,' SFreeSpaceUpper ',SFreeSpaceUpper  
       PRINT *,' TFreeSpaceLower ',TFreeSpaceLower  
       PRINT *,' TFreeSpaceUpper ',TFreeSpaceUpper  
       print *,'*************'
       PRINT *,' S1Pp   ',S1Pp  
       PRINT *,' S2Pip  ',S2Pip  
       PRINT *,' S2Po1p ',S2Po1p  
       PRINT *,' S2Po2p ',S2Po2p  
       PRINT *,' S2Pap  ',S2Pap  
       PRINT *,' Ziip   ',Ziip  
       PRINT *,' Zoop   ',Zoop  
       PRINT *,' Zaap   ',Zaap  
       print *,'*************'
       PRINT *,' PitchPhi2ndStrip   ',PitchPhi2ndStrip  
       print *,'*************'
       PRINT *,' PitchEta1stStrip   ',PitchEta1stStrip  
       print *,'*************'
       PRINT *,' NberWireLayer   ',NberWireLayer  
       print *,'*************'
       PRINT *,' CUFOAMG10   ',CUFOAMG10  
       PRINT *,' GASGAP      ',GASGAP  
       print *,'*************'
      endif
!      
!--------------------------------------------------------------------
!
      END SUBROUTINE CscGeoBasics
!            
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE CscEta1stCoStripModel(Iprint,                                         &
                                  IAmdcVersion,                                   &
                                  Ctyp,Jff,Jzz,Job,Jsl,JStrip,                    &
                                  SzzttCc,SzzttCcF,SzzttCcR,                      &
                                  S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,  &
                                  PitchStrip,                                     &
                                  ttLayer,                                        &
                                  ShiftStrip,                                     &
                                  SzzttStrip,XYZStrip)
 IMPLICIT NONE
 INTEGER,      INTENT(IN)    :: Iprint
 INTEGER,      INTENT(IN)    :: IAmdcVersion
 CHARACTER(3), INTENT(IN)    :: Ctyp
 INTEGER,      INTENT(IN)    :: Jff,Jzz,Job,Jsl,JStrip
 REAL(8),      INTENT(IN)    :: SzzttCc(3)
 REAL(8),      INTENT(IN)    :: SzzttCcF(3)
 REAL(8),      INTENT(IN)    :: SzzttCcR(3)
 REAL(8),      INTENT(IN)    :: S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap
 REAL(8),      INTENT(IN)    :: Zoop,Ziip,Zaap
 REAL(8),      INTENT(IN)    :: PitchStrip
 REAL(8),      INTENT(IN)    :: ttLayer(2,4)              
 REAL(8),      INTENT(OUT)   :: ShiftStrip
 REAL(8),      INTENT(OUT)   :: SzzttStrip(3,5)
 REAL(8),      INTENT(OUT)   :: XYZStrip(3,5)
 INTEGER :: NbStrips1stCo
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
    CALL CscEta1stCoStripModelBase(Iprint,                                          &
                                   IAmdcVersion,                                    &
                                   Ctyp,Jff,Jzz,Job,Jsl,JStrip,                     &
                                   SzzttCc,SzzttCcF,SzzttCcR,                       &
                                   S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,   &
                                   PitchStrip,                                      &
                                   ttLayer,                                         &
                                   ShiftStrip,                                      &
                                   SzzttStrip,XYZStrip,                             &
                                   NbStrips1stCo)
!
 END SUBROUTINE CscEta1stCoStripModel
!            
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE CscEta1stCoStripModelBase(Iprint,                                         &
                                           IAmdcVersion,                                   &
                                           Ctyp,Jff,Jzz,Job,Jsl,JStrip,                    &
                                           SzzttCc,SzzttCcF,SzzttCcR,                      &
                                           S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,  &
                                           PitchStrip,                                     &
                                           ttLayer,                                        &
                                           ShiftStrip,                                     &
                                           SzzttStrip,XYZStrip,                            &
                                           NbStrips1stCo)
!
      IMPLICIT REAL(8) (A-H,O-Z)                              
!
      INTEGER,      INTENT(IN)    :: Iprint
      INTEGER,      INTENT(IN)    :: IAmdcVersion
      CHARACTER(3), INTENT(IN)    :: Ctyp
      INTEGER,      INTENT(IN)    :: Jff,Jzz,Job,Jsl,JStrip
      REAL(8),      INTENT(IN)    :: SzzttCc(3)
      REAL(8),      INTENT(IN)    :: SzzttCcF(3)
      REAL(8),      INTENT(IN)    :: SzzttCcR(3)
      REAL(8),      INTENT(IN)    :: S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap
      REAL(8),      INTENT(IN)    :: Zoop,Ziip,Zaap
      REAL(8),      INTENT(IN)    :: PitchStrip
      REAL(8),      INTENT(IN)    :: ttLayer(2,4)              
      REAL(8),      INTENT(OUT)   :: ShiftStrip
      REAL(8),      INTENT(OUT)   :: SzzttStrip(3,5)
      REAL(8),      INTENT(OUT)   :: XYZStrip(3,5)
      INTEGER,      INTENT(OUT)   :: NbStrips1stCo
!
      REAL(8) :: SStrip,zzStrip,ttStrip
      REAL(8) :: SStripSPzzP,zzStripSPzzP
      REAL(8) :: SStripSPzzM,zzStripSPzzM
      REAL(8) :: SStripSMzzM,zzStripSMzzM
      REAL(8) :: SStripSMzzP,zzStripSMzzP
      REAL(8) :: zzCc ,ttCc ,SCc
      REAL(8) :: zzCcF,ttCcF,SCcF
      REAL(8) :: zzCcR,ttCcR,SCcR
      REAL(8) :: SStripP
      REAL(8) :: SStripM
      REAL(8) :: ttPi     
!--------------------------------------------------------------------
!
      SCc  =SzzttCc(1) 
      zzCc =SzzttCc(2) 
      ttCc =SzzttCc(3) 
      SCcF =SzzttCcF(1)
      zzCcF=SzzttCcF(2)
      ttCcF=SzzttCcF(3)
      SCcR =SzzttCcR(1)
      zzCcR=SzzttCcR(2)
      ttCcR=SzzttCcR(3)
      ttPi=ttLayer(2,JSL)
!
!--------------------------------------------------------------------
!
      if (Iprint.eq.1) then
       print *,'*************CscEta1stCoStripModelBase Inputs'
       PRINT *,' Ctyp    ',Ctyp
       PRINT *,' Jff     ',Jff
       PRINT *,' Jzz     ',Jzz
       PRINT *,' Job     ',Job
       PRINT *,' Jsl     ',Jsl
       PRINT *,' JStrip  ',JStrip
       print *,'*************'
       PRINT *,' IAmdcVersion ',IAmdcVersion
       print *,'*************'
       PRINT *,' SCc ',SCc  
       PRINT *,' zzCc ',zzCc  
       PRINT *,' ttCc ',ttCc  
       print *,'*************'
       PRINT *,' SCcF ',SCcF  
       PRINT *,' zzCcF ',zzCcF  
       PRINT *,' ttCcF ',ttCcF  
       print *,'*************'
       PRINT *,' SCcR ',SCcR  
       PRINT *,' zzCcR ',zzCcR  
       PRINT *,' ttCcR ',ttCcR  
       print *,'*************'
       PRINT *,' S1Pp   ',S1Pp  
       PRINT *,' S2Pip  ',S2Pip  
       PRINT *,' S2Po1p ',S2Po1p  
       PRINT *,' S2Po2p ',S2Po2p  
       PRINT *,' S2Pap  ',S2Pap  
       PRINT *,' Ziip   ',Ziip  
       PRINT *,' Zoop   ',Zoop  
       PRINT *,' Zaap   ',Zaap  
       print *,'*************'
       PRINT *,' PitchStrip   ',PitchStrip  
       print *,'*************'
       PRINT *,' ttLayer '
       Do ikind=1,2
        PRINT *,'*ikind ',ikind
        DO L=1,4
         PRINT *,'  ',L,' ',ttLayer(ikind,L)
        ENDDO
       ENDDO
       print *,'*************'
      endif
!      
!--------------------------------------------------------------------
!ShiftStrip is the shift along Z for Eta-1st coordinate CSC strip
!
      WidthToBeMapped = Zoop
!
      NbStrips1stCo = NINT(WidthToBeMapped/PitchStrip)
!
      IF( IAmdcVersion >= 7 ) THEN
!To avoid rounding problems
       NbStrips1stCo=192
      ENDIF
!
      WidthMapped= NbStrips1stCo * PitchStrip
!
      ShiftStrip = zzCc - WidthMapped/2.d0 + PitchStrip/2.d0
!
!SStrip, zzStrip and ttStrip
! are the coordinates of the Eta-1st coordinate CSC strip Jstrip
!Coordonnates of strips corners
! SStripSPzzP and zzStripSPzzP at max S and max zz
! SStripSPzzM and zzStripSPzzM at max S and min zz
! SStripSMzzM and zzStripSMzzM at min S and min zz
! SStripSMzzP and zzStripSMzzP at min S and max zz
!
      zzStrip = ShiftStrip + real((JStrip-1),8)*PitchStrip
!
      Tstrip = zzStrip - zzCc
      call CscGeoSRange(Ctyp,                                            &
                        S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,   &
                        Tstrip,SStripP,SStripM)
      SStrip = SCc + ( SStripP + SStripM )/2.d0
!
      IF( IAmdcVersion < 7 ) THEN
! 
       Tstrip = zzStrip - zzCc + PitchStrip/2.d0
       call CscGeoSRange(Ctyp,                                           &
                         S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,  &
                         Tstrip,SStripP,SStripM)
        SStripSPzzP=SCc+SStripP
       zzStripSPzzP=zzStrip+PitchStrip/2.d0
! 
       Tstrip = zzStrip - zzCc - PitchStrip/2.d0
       call CscGeoSRange(Ctyp,                                           &
                         S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,  &
                         Tstrip,SStripP,SStripM)
        SStripSPzzM=SCc+SStripP
       zzStripSPzzM=zzStrip-PitchStrip/2.d0
! 
       Tstrip = zzStrip - zzCc - PitchStrip/2.d0
       call CscGeoSRange(Ctyp,                                           &
                         S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,  &
                         Tstrip,SStripP,SStripM)
        SStripSMzzM=SCc+SStripM
       zzStripSMzzM=zzStrip-PitchStrip/2.d0
! 
       Tstrip = zzStrip - zzCc + PitchStrip/2.d0
       call CscGeoSRange(Ctyp,                                           &
                         S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,  &
                         Tstrip,SStripP,SStripM)
        SStripSMzzP=SCc+SStripM
       zzStripSMzzP=zzStrip+PitchStrip/2.d0
! 
      ELSE
!
        SStripSPzzP=SCc+SStripP
       zzStripSPzzP=zzStrip+PitchStrip/2.d0
!
        SStripSPzzM=SCc+SStripP
       zzStripSPzzM=zzStrip-PitchStrip/2.d0
!
        SStripSMzzM=SCc+SStripM
       zzStripSMzzM=zzStrip-PitchStrip/2.d0
!
        SStripSMzzP=SCc+SStripM
       zzStripSMzzP=zzStrip+PitchStrip/2.d0
!
      ENDIF
!      
      ttStrip = ttPi
!
!>>Center
      SzzttStrip(1,5) = SStrip
      SzzttStrip(2,5) = zzStrip
      SzzttStrip(3,5) = ttStrip
!
!>>Point A, B, C and D
!    DA  41
!    CB  32
!
      SzzttStrip(1,1) =  SStripSPzzP
      SzzttStrip(2,1) = zzStripSPzzP
      SzzttStrip(3,1) = ttStrip
!
      SzzttStrip(1,2) =  SStripSPzzM
      SzzttStrip(2,2) = zzStripSPzzM
      SzzttStrip(3,2) = ttStrip
!
      SzzttStrip(1,3) =  SStripSMzzM
      SzzttStrip(2,3) = zzStripSMzzM
      SzzttStrip(3,3) = ttStrip
!
      SzzttStrip(1,4) =  SStripSMzzP
      SzzttStrip(2,4) = zzStripSMzzP
      SzzttStrip(3,4) = ttStrip
!
!>> Global coordinates
!
      DO J=1,5
       call CSCLocalToGlobal( Ctyp,Jff,Jzz,Job,Jsl, SzzttStrip(1,J), XYZStrip(1,J) )
      ENDDO
!
!--------------------------------------------------------------------
!
      if (Iprint.eq.1) then
       print *,'*************CscEta1stCoStripModelBase Outputs'
       PRINT *,' WidthToBeMapped ',WidthToBeMapped
       PRINT *,' NbStrips1stCo ',NINT(WidthToBeMapped/PitchStrip)
       PRINT *,' NbStrips1stCo ',NbStrips1stCo
       PRINT *,' WidthMapped ',WidthMapped
       PRINT *,' ShiftStrip ',ShiftStrip
       PRINT *,' SStrip  ',SStrip
       PRINT *,' zzStrip ',zzStrip
       PRINT *,' ttStrip ',ttStrip
       do i1=1,4
        PRINT *,' point ',i1
        do i2=1,3
         PRINT *,'       ',SzzttStrip(i2,i1)-SzzttStrip(i2,5)
        enddo
       enddo
       i1=5
        PRINT *,' point ',i1
        do i2=1,3
         PRINT *,'       ',SzzttStrip(i2,i1)
        enddo
       print *,'*************'
      endif
!
      END SUBROUTINE CscEta1stCoStripModelBase
!            
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE SETICSC
!            
!--------------------------------------------------------------------
!    
!  The effect of I line is:
!    li_out = Ri * li_in + Ti
!    
!  where li is the local vector in a special local system shifted by some lc in the true local system
!  i.e the fixed point by Ri is lc
!   
!  One has:
!     l = lc + li
!  
!  So 
!     l_out = Ri * l_in + Ti + ( 1 - Ri )* Ti
!       
!--------------------------------------------------------------------
!            
 IMPLICIT NONE
!            
#include "AmdcStand/amdcsim_com.inc"
!
 INTEGER            :: J1,Icode
 INTEGER            :: JTYP,JFF,JZZ,JOB,JLAY
!
 INTEGER      :: Iprint
 CHARACTER(3) :: Ctyp
 INTEGER :: IAmdcVersion
 REAL(8) :: SZTFixed(3),SZTCc(3)
 REAL(8) :: SzzttCc(3),SzzttCcF(3),SzzttCcR(3)
 REAL(8) :: S1Pb,S2Pib,S2Po1b,S2Po2b,S2Pab
 REAL(8) :: Ziib,Zoob,Zaab
 REAL(8) :: S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap
 REAL(8) :: Ziip,Zoop,Zaap
 REAL(8) :: PitchEta1stStrip,PitchPhi2ndStrip
 INTEGER :: NberWireLayer
 REAL(8) :: CUFOAMG10,GASGAP
 REAL(8) :: Gama0,FOs
 REAL(8) :: ttLayer(2,4)
!
 REAL(8) :: Alfa(3),ILineFixPoint(3)
 REAL(8) :: OCenter(3)
 REAL(8) :: RotaSZT(3,3)
 REAL(8) :: RotS(3,3),RotZ(3,3),RotT(3,3),RotX(3,3)
 REAL(8) :: COSA1,SINA1
 REAL(8) :: COSA2,SINA2
 REAL(8) :: COSA3,SINA3
!
 REAL(8) :: COSAA,SINAA
!
      Iprint = 0

!      print *,'*********************************SETICSC'
      DO J1=1,NBdwnlay
        Icode = JTYPFZOLdwnlay(J1)
        CALL DECO_JTYP_F_Z_O_L(Icode, JTYP,JFF,JZZ,JOB,JLAY)
        ctyp = CARTYP(JTYP)
!--Basics of Csc Geometry
        call CscGeoBasics(Iprint,                                         &
                         Ctyp,Jff,Jzz,Job,                                &
                         IAmdcVersion,                                    &
                         SZTFixed,SZTCc,                                  &
                         SzzttCc,SzzttCcF,SzzttCcR,                       &
                         S1Pb,S2Pib,S2Po1b,S2Po2b,S2Pab,Ziib,Zoob,Zaab,   &
                         S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,   &
                         PitchEta1stStrip,PitchPhi2ndStrip,               &
                         NberWireLayer,CUFOAMG10,GASGAP,                  &
                         Gama0,FOs)
!
!--Wire Layer Model
        call CscWireLayerModel(Iprint,                           &
                              IAmdcVersion,                      &
                              Ctyp,Jff,Jzz,Job,                  &
                              SzzttCc,SzzttCcF,SzzttCcR,         &
                              NberWireLayer,CUFOAMG10,GASGAP,    &
                              ttLayer)
 
        ILineFixPoint(1) = 0.D0

        ILineFixPoint(2) = SZTCc(3) - SZTFixed(3)

        COSAA = COS(Gama0)
        SINAA = SIN(Gama0)

        ILineFixPoint(3) = (ttLayer(1,Jlay)+ttLayer(2,Jlay))/2.d0 &
                  - ( COSAA * SZTFixed(2) - SINAA * SZTFixed(3) )
        if (jzz<0) ILineFixPoint(3) = -ILineFixPoint(3)
        
        Alfa( 1:3 ) =  SZTrotdwnlay( 1:3,J1)
        
        COSA1 = COS(Alfa(1))
        SINA1 = SIN(Alfa(1))
        COSA2 = COS(Alfa(2))
        SINA2 = SIN(Alfa(2))
        COSA3 = COS(Alfa(3))
        SINA3 = SIN(Alfa(3))

        RotS(1:3,1:3) =   0.D0
        RotZ(1:3,1:3) =   0.D0
        RotT(1:3,1:3) =   0.D0

        RotS( 1 , 1 ) =   1.D0
        RotS( 2 , 2 ) =   COSA1
        RotS( 3 , 2 ) =   SINA1
        RotS( 2 , 3 ) = - SINA1
        RotS( 3 , 3 ) =   COSA1
        
        RotZ( 2 , 2 ) =   1.D0
        RotZ( 1 , 1 ) =   COSA2
        RotZ( 3 , 1 ) = - SINA2
        RotZ( 1 , 3 ) =   SINA2
        RotZ( 3 , 3 ) =   COSA2
        
        RotT( 3 , 3 ) =   1.D0
        RotT( 1 , 1 ) =   COSA3
        RotT( 2 , 1 ) =   SINA3
        RotT( 1 , 2 ) = - SINA3
        RotT( 2 , 2 ) =   COSA3

        CALL MULT3M3M( RotT,RotZ, RotX )
        CALL MULT3M3M( RotX,RotS, RotaSZT )

        SZTtradwnlayShifted(1:3,J1) = SZTtradwnlay(1:3,J1) +     &
                                             ILineFixPoint(1:3)  &
                          - RotaSZT(1:3,1) * ILineFixPoint(1)    &
                          - RotaSZT(1:3,2) * ILineFixPoint(2)    &
                          - RotaSZT(1:3,3) * ILineFixPoint(3)

        OCenter(1) = SZTFixed(1)
        OCenter(2) = SINAA*SZTFixed(2) + COSAA*SZTFixed(3) 
        OCenter(3) = COSAA*SZTFixed(2) - SINAA*SZTFixed(3)

        SZTtradwnlayCentred(1:3,J1) = ILineFixPoint(1:3) + OCenter(1:3)

!        print *,'*',J1
!        print *,' ',SZTCc(3),' ',SZTFixed(3)
!        print *,' ',ttLayer(1,Jlay),' ',ttLayer(2,Jlay),' ',&
!        ( COSAA * SZTFixed(2) - SINAA * SZTFixed(3) )
!        print *,' ',SZT(1),' ',SZTtradwnlay(1,J1),' ',SZTtradwnlayShifted(1,J1)
!        print *,' ',SZT(2),' ',SZTtradwnlay(2,J1),' ',SZTtradwnlayShifted(2,J1)
!        print *,' ',SZT(3),' ',SZTtradwnlay(3,J1),' ',SZTtradwnlayShifted(3,J1)
      ENDDO
!      print *,'*********************************'
      END SUBROUTINE SETICSC
!            
