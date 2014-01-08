!
!> Computation of Segments errors matrix
!> \author J.F.Laporte 
!
 SUBROUTINE ErrorSegment
 USE M_MB_Digis
 USE M_MB_Reco_Segments
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER :: IBINTR, L, NUIN, NUAN
 REAL(8) :: RecSegLoc(3),SegProjLoc(3)
 REAL(8) :: SinThetaYZLoc,SinThetaXZLoc
 REAL(8) :: LamdYZloc(20),LamdXZloc(20)
 REAL(8) :: XmGlo(20),YmGlo(20),ZmGlo(20),RmGlo(20)
 REAL(8) :: XmLoc(20),YmLoc(20),ZmLoc(20)
 REAL(8) :: XmGloO(20),YmGloO(20),ZmGloO(20)
 INTEGER :: IMdtCscLoc(20),IRpcTgcloc(20),I1rst2ndloc(20)
 REAL(8) :: VBidon1,VBidon2,VBidon3
 INTEGER :: IS,IT,IMdtCsc,IRpcTgc,I1rst2nd
 REAL(8) :: AssociatedReso
 REAL(8) :: XYZ1(3),XYZ2(3)
 REAL(8) :: VecNorm(3)
 REAL(8) :: det, covzz, covzt, covtt
 INTEGER :: ThereIsPrecise,ThereIsPasPrecise
 INTEGER :: ThereIsPasPreciseP,ThereIsPasPreciseN
 INTEGER, SAVE :: iprloc = 0 !>> Set print level 0=no print, .ne.0=print
!--
!--  27/09/05 JFL a la louche
!--  13/09/06 JFL CSC a la louche too
!--  10/11/06 JFL Correct angle 2nd coord behavior in case of measurements on one side only
!--  12/10/07 AO  Add alignment error 
!--
!
   IF( iprloc /= 0 ) PRINT 6050,NBINTR
   IF( NBINTR /= 0 ) THEN
     DO IBINTR=1,NBINTR
       NUIN = NUINTR(IBINTR)
       NUAN = NAINTR(IBINTR)
!
!      Compute Segments coordinates in (dx,dy,dz) frame       
       RecSegLoc(1) = VEINTR(1,IBINTR)*DXINTR(1,IBINTR) &
                    + VEINTR(2,IBINTR)*DXINTR(2,IBINTR) &
                    + VEINTR(3,IBINTR)*DXINTR(3,IBINTR) 
       RecSegLoc(2) = VEINTR(1,IBINTR)*DYINTR(1,IBINTR) &
                    + VEINTR(2,IBINTR)*DYINTR(2,IBINTR) &
                    + VEINTR(3,IBINTR)*DYINTR(3,IBINTR) 
       RecSegLoc(3) = VEINTR(1,IBINTR)*DZINTR(1,IBINTR) &
                    + VEINTR(2,IBINTR)*DZINTR(2,IBINTR) &
                    + VEINTR(3,IBINTR)*DZINTR(3,IBINTR) 
       VBidon1 = dsqrt( RecSegLoc(1)**2 &
               +        RecSegLoc(2)**2 &  
               +        RecSegLoc(3)**2 )
       RecSegLoc(1) = RecSegLoc(1)/VBidon1
       RecSegLoc(2) = RecSegLoc(2)/VBidon1
       RecSegLoc(3) = RecSegLoc(3)/VBidon1
!
!      Compute SinThetaYZ
       SegProjLoc(1) = 0.d0
       SegProjLoc(2) = RecSegLoc(2)
       SegProjLoc(3) = RecSegLoc(3)
       VBidon1 = dsqrt( SegProjLoc(1)**2 &
               +        SegProjLoc(2)**2 &  
               +        SegProjLoc(3)**2 )
       SegProjLoc(1) = SegProjLoc(1)/VBidon1
       SegProjLoc(2) = SegProjLoc(2)/VBidon1
       SegProjLoc(3) = SegProjLoc(3)/VBidon1
       SinThetaYZLoc = SegProjLoc(3)
!
!      Compute SinThetaXZ
       SegProjLoc(1) = RecSegLoc(1)
       SegProjLoc(2) = 0.d0
       SegProjLoc(3) = RecSegLoc(3)
       VBidon1 = dsqrt( SegProjLoc(1)**2 &
               +        SegProjLoc(2)**2 &  
               +        SegProjLoc(3)**2 )
       SegProjLoc(1) = SegProjLoc(1)/VBidon1
       SegProjLoc(2) = SegProjLoc(2)/VBidon1
       SegProjLoc(3) = SegProjLoc(3)/VBidon1
       SinThetaXZLoc = SegProjLoc(3)
!
!      Switch off error matrix validity flag
       IERINTR(IBINTR) = 0
!
!      Initialize error matrix
       ERINTR(1:6,IBINTR) = 0.d0
!
!      Initialize auxilary arrays
       DO L=1,NUIN+NUAN
         LamdYZloc(L)  = 0.d0
         LamdXZloc(L)  = 0.d0
         IMdtCscLoc(L)  = 0
         IRpcTgcLoc(L)  = 0
         I1rst2ndLoc(L) = 0
         XmLoc(L)   = 0.d0
         YmLoc(L)   = 0.d0
         ZmLoc(L)   = 0.d0
         XmGlo(L)   = 0.d0
         YmGlo(L)   = 0.d0
         ZmGlo(L)   = 0.d0
         RmGlo(L)   = 0.d0
       ENDDO
!
       IF(CARISC(ISCINTR(IBINTR))(1:2) /= 'CS') THEN
!
!       Initialize error matrix
!WARNING: Set resolutions in case of non second coordinate measurement
         ERINTR(1,IBINTR) = 1.d0 / 60.0d0**2
         ERINTR(2,IBINTR) = 1.d0 / (3.14159d0/dsqrt(12.d0))**2
         ERINTR(3,IBINTR) = 0.d0
!
!       Loop on precise measurements points
         ThereIsPrecise = 0
         DO L=1,NUIN
!
!         Compute coordinates in global frame
           XmGloO(L) = XDINTR(L,IBINTR)
           YmGloO(L) = YDINTR(L,IBINTR)
           ZmGloO(L) = ZDINTR(L,IBINTR)
           XmGlo(L)  = XDINTR(L,IBINTR)
           YmGlo(L)  = YDINTR(L,IBINTR)
           ZmGlo(L)  = ZDINTR(L,IBINTR)
           RmGlo(L)  = RDINTR(L,IBINTR)

!         Compute coordinates in (SegPoint,dX,dY,dZ) frame 
           XmLoc(L) = ( XmGlo(L) - PTINTR(1,IBINTR) ) * DXINTR(1,IBINTR) &
                    + ( YmGlo(L) - PTINTR(2,IBINTR) ) * DXINTR(2,IBINTR) &
                    + ( ZmGlo(L) - PTINTR(3,IBINTR) ) * DXINTR(3,IBINTR) 
           YmLoc(L) = ( XmGlo(L) - PTINTR(1,IBINTR) ) * DYINTR(1,IBINTR) &
                    + ( YmGlo(L) - PTINTR(2,IBINTR) ) * DYINTR(2,IBINTR) &
                    + ( ZmGlo(L) - PTINTR(3,IBINTR) ) * DYINTR(3,IBINTR) 
           ZmLoc(L) = ( XmGlo(L) - PTINTR(1,IBINTR) ) * DZINTR(1,IBINTR) &
                    + ( YmGlo(L) - PTINTR(2,IBINTR) ) * DZINTR(2,IBINTR) &
                    + ( ZmGlo(L) - PTINTR(3,IBINTR) ) * DZINTR(3,IBINTR)
!
!         Compute in (SegPoint,dY,dZ) plane signed distance
           LamdYZloc(L) = dsqrt( YmLoc(L)**2 +  ZmLoc(L)**2)
           if (ZmLoc(L).lt.0.d0) LamdYZloc(L) = -LamdYZloc(L)
!
!         Compute in (SegPoint,dX,dZ) plane signed distance
           LamdXZloc(L) = dsqrt( XmLoc(L)**2 +  ZmLoc(L)**2)
           if (ZmLoc(L).lt.0.d0) LamdXZloc(L) = -LamdXZloc(L)
!
!         Compute errors for (SegPoint,dY,dZ) parameters
           if (RmGlo(L).ne.0.d0) then
             ERINTR(4,IBINTR) = ERINTR(4,IBINTR)                  &
                              + SinThetaYZLoc**2                   / RmGlo(L)**2
             ERINTR(5,IBINTR) = ERINTR(5,IBINTR)                  &
                              +                    LamdYZloc(L)**2 / RmGlo(L)**2
             ERINTR(6,IBINTR) = ERINTR(6,IBINTR)                  &
                              + SinThetaYZLoc    * LamdYZloc(L)    / RmGlo(L)**2
             ThereIsPrecise = 1
           endif
!
         ENDDO
!
!       Loop on pas precise measurements points
         ThereIsPasPrecise = 0
         ThereIsPasPreciseP = 0
         ThereIsPasPreciseN = 0
         DO L=NUIN+1,NUIN+NUAN
!
!         Compute coordinates in global frame
           IS = IUINTR(L,IBINTR)
           call GetStripEnds( &
            IS, IRpcTgc,I1rst2nd, &
            XYZ1,XYZ2,AssociatedReso , &
            VecNorm &
           )
           IRpcTgcLoc(L)  = IRpcTgc
           I1rst2ndLoc(L) = I1rst2nd
           XmGloO(L) = XYZ1(1)
           YmGloO(L) = XYZ1(2)
           ZmGloO(L) = XYZ1(3)
           VBidon1 = ( XmGloO(L) - PTINTR(1,IBINTR) ) * VecNorm(1) &
                   + ( YmGloO(L) - PTINTR(2,IBINTR) ) * VecNorm(2) &
                   + ( ZmGloO(L) - PTINTR(3,IBINTR) ) * VecNorm(3)
           VBidon2 =     VEINTR(1,IBINTR)             * VecNorm(1) &
                   +     VEINTR(2,IBINTR)             * VecNorm(2) &
                   +     VEINTR(3,IBINTR)             * VecNorm(3)
           if (VBidon2.eq.0.d0) CYCLE
           VBidon3 = VBidon1/VBidon2 
           XmGlo(L) = PTINTR(1,IBINTR) + VBidon3 * VEINTR(1,IBINTR)
           YmGlo(L) = PTINTR(2,IBINTR) + VBidon3 * VEINTR(2,IBINTR)
           ZmGlo(L) = PTINTR(3,IBINTR) + VBidon3 * VEINTR(3,IBINTR)
           RmGlo(L) = AssociatedReso
!
!         Compute coordinates in (SegPoint,dX,dY,dZ) frame 
           XmLoc(L) = ( XmGlo(L) - PTINTR(1,IBINTR) ) * DXINTR(1,IBINTR) &
                    + ( YmGlo(L) - PTINTR(2,IBINTR) ) * DXINTR(2,IBINTR) &
                    + ( ZmGlo(L) - PTINTR(3,IBINTR) ) * DXINTR(3,IBINTR) 
           YmLoc(L) = ( XmGlo(L) - PTINTR(1,IBINTR) ) * DYINTR(1,IBINTR) &
                    + ( YmGlo(L) - PTINTR(2,IBINTR) ) * DYINTR(2,IBINTR) &
                    + ( ZmGlo(L) - PTINTR(3,IBINTR) ) * DYINTR(3,IBINTR) 
           ZmLoc(L) = ( XmGlo(L) - PTINTR(1,IBINTR) ) * DZINTR(1,IBINTR) &
                    + ( YmGlo(L) - PTINTR(2,IBINTR) ) * DZINTR(2,IBINTR) &
                    + ( ZmGlo(L) - PTINTR(3,IBINTR) ) * DZINTR(3,IBINTR)
!
!         Compute in (SegPoint,dY,dZ) plane signed distance
           LamdYZloc(L) = dsqrt( YmLoc(L)**2 +  ZmLoc(L)**2)
           if (ZmLoc(L).lt.0.d0) LamdYZloc(L) = -LamdYZloc(L)
!
!         Compute in (SegPoint,dX,dZ) plane signed distance
           LamdXZloc(L) = dsqrt( XmLoc(L)**2 +  ZmLoc(L)**2)
           if (ZmLoc(L).lt.0.d0) LamdXZloc(L) = -LamdXZloc(L)
!
!         Compute errors for (SegPoint,dY,dZ) parameters
!WARNING: Do not take into account variable with of 2nd coordinate TGC strip
           if (I1rst2nd == -1) then
             if (RmGlo(L).ne.0.d0) then
               ERINTR(1,IBINTR) = ERINTR(1,IBINTR)                  &
                                + 1.d0                               / RmGlo(L)**2
               ERINTR(2,IBINTR) = ERINTR(2,IBINTR)                  &
                                + LamdXZloc(L)**2 / SinThetaXZLoc**2 / RmGlo(L)**2
               ERINTR(3,IBINTR) = ERINTR(3,IBINTR)                  &
                                + LamdXZloc(L)    / SinThetaXZLoc    / RmGlo(L)**2
               ThereIsPasPrecise = 1
               if (LamdXZloc(L).gt.0.d0) ThereIsPasPreciseP = 1
               if (LamdXZloc(L).lt.0.d0) ThereIsPasPreciseN = 1
             endif
           endif
!
         ENDDO
!
!       Switch on/off error matrix validity flag (0= no error mat., 1=Ok, 2= no 2nd coordinate)
         IERINTR(IBINTR) = 0
         if (ThereIsPrecise.eq.1) then
           IERINTR(IBINTR) = 1
           if (ThereIsPasPrecise.eq.0) then
             IERINTR(IBINTR) = 2
           else
             if ( (ThereIsPasPreciseP*ThereIsPasPreciseN).eq.0 ) then
               ERINTR(3,IBINTR) = 0.d0
               ERINTR(2,IBINTR) = PTINTR(1,IBINTR)**2 + PTINTR(2,IBINTR)**2 
               ERINTR(2,IBINTR) = ERINTR(2,IBINTR) /20.d0**2
             endif
           endif
         endif

       ELSE
!
!       Initialize error matrix
!WARNING: Set resolutions in case of non second coordinate measurement
         ERINTR(1,IBINTR) = 1.d0 / 60.0d0**2
         ERINTR(2,IBINTR) = 1.d0 / (3.14159d0/dsqrt(12.d0))**2
         ERINTR(3,IBINTR) = 0.d0
!
         ThereIsPrecise = 0
         ThereIsPasPrecise = 0
         DO L=1,NUIN
!         Compute coordinates in global frame
           IT = IUINTR(L,IBINTR)
!WARNING: This geometry looks nominal
!WARNING: Assume explicitly that the strip is paral. to (dX,dY) plane
           Call GetTubeEnds( IT, IMdtCsc,I1rst2nd,XYZ1,XYZ2,AssociatedReso )
           IMdtCscLoc(L)  = IMdtCsc
           I1rst2ndLoc(L) = I1rst2nd
           XmGloO(L) = XYZ1(1)
           YmGloO(L) = XYZ1(2)
           ZmGloO(L) = XYZ1(3)
           VBidon1 = ( XmGloO(L) - PTINTR(1,IBINTR) ) * DZINTR(1,IBINTR) &
                   + ( YmGloO(L) - PTINTR(2,IBINTR) ) * DZINTR(2,IBINTR) &
                   + ( ZmGloO(L) - PTINTR(3,IBINTR) ) * DZINTR(3,IBINTR)
           VBidon2 =   VEINTR(1,IBINTR)             * DZINTR(1,IBINTR) &
                   +   VEINTR(2,IBINTR)             * DZINTR(2,IBINTR) &
                   +   VEINTR(3,IBINTR)             * DZINTR(3,IBINTR)
           if (VBidon2.eq.0.d0) CYCLE
           VBidon3 = VBidon1/VBidon2 
           XmGlo(L) = PTINTR(1,IBINTR) + VBidon3 * VEINTR(1,IBINTR)
           YmGlo(L) = PTINTR(2,IBINTR) + VBidon3 * VEINTR(2,IBINTR)
           ZmGlo(L) = PTINTR(3,IBINTR) + VBidon3 * VEINTR(3,IBINTR)
           RmGlo(L) = AssociatedReso
!
!         Compute coordinates in (SegPoint,dX,dY,dZ) frame 
           XmLoc(L) = ( XmGlo(L) - PTINTR(1,IBINTR) ) * DXINTR(1,IBINTR) &
                    + ( YmGlo(L) - PTINTR(2,IBINTR) ) * DXINTR(2,IBINTR) &
                    + ( ZmGlo(L) - PTINTR(3,IBINTR) ) * DXINTR(3,IBINTR) 
           YmLoc(L) = ( XmGlo(L) - PTINTR(1,IBINTR) ) * DYINTR(1,IBINTR) &
                    + ( YmGlo(L) - PTINTR(2,IBINTR) ) * DYINTR(2,IBINTR) &
                    + ( ZmGlo(L) - PTINTR(3,IBINTR) ) * DYINTR(3,IBINTR) 
           ZmLoc(L) = ( XmGlo(L) - PTINTR(1,IBINTR) ) * DZINTR(1,IBINTR) &
                    + ( YmGlo(L) - PTINTR(2,IBINTR) ) * DZINTR(2,IBINTR) &
                    + ( ZmGlo(L) - PTINTR(3,IBINTR) ) * DZINTR(3,IBINTR)
!
!         Compute in (SegPoint,dY,dZ) plane signed distance
           LamdYZloc(L) = dsqrt( YmLoc(L)**2 +  ZmLoc(L)**2)
           if (ZmLoc(L).lt.0.d0) LamdYZloc(L) = -LamdYZloc(L)
!
!         Compute in (SegPoint,dX,dZ) plane signed distance
           LamdXZloc(L) = dsqrt( XmLoc(L)**2 +  ZmLoc(L)**2)
           if (ZmLoc(L).lt.0.d0) LamdXZloc(L) = -LamdXZloc(L)
!
           if (I1rst2nd.eq.1) then
             if (RmGlo(L).ne.0.d0) then
               ERINTR(4,IBINTR) = ERINTR(4,IBINTR)                  &
                                + 1.d0                               / RmGlo(L)**2
               ERINTR(5,IBINTR) = ERINTR(5,IBINTR)                  &
                                + LamdYZloc(L)**2 / SinThetaYZLoc**2 / RmGlo(L)**2
               ERINTR(6,IBINTR) = ERINTR(6,IBINTR)                  &
                                + LamdYZloc(L)    / SinThetaYZLoc    / RmGlo(L)**2
               ThereIsPrecise = 1
             endif
           endif
!
           if (I1rst2nd.eq.-1) then
             if (RmGlo(L).ne.0.d0) then
               ERINTR(1,IBINTR) = ERINTR(1,IBINTR)                  &
                                + 1.d0                               / RmGlo(L)**2
               ERINTR(2,IBINTR) = ERINTR(2,IBINTR)                  &
                                + LamdXZloc(L)**2 / SinThetaXZLoc**2 / RmGlo(L)**2
               ERINTR(3,IBINTR) = ERINTR(3,IBINTR)                  &
                                + LamdXZloc(L)    / SinThetaXZLoc    / RmGlo(L)**2
               ThereIsPasPrecise = 1
             endif
           endif
!
         ENDDO
!
!       Switch on/off error matrix validity flag (0= no error mat., 1=Ok, 2= no 2nd coordinate)
         IERINTR(IBINTR) = 0
         if (ThereIsPrecise.eq.1) then
           IERINTR(IBINTR) = 1
           if (ThereIsPasPrecise.eq.0) then
             IERINTR(IBINTR) = 2
           endif
         endif

       ENDIF
!
! Add error on station alignment (only Z alignment taken here)
!
       IF( AligST(ISCINTR(IBINTR)) > 1.d-4 ) THEN
         det = ERINTR(4,IBINTR)*ERINTR(5,IBINTR) - ERINTR(6,IBINTR)**2
         IF( det /= 0.d0 ) THEN
           covzz =   ERINTR(5,IBINTR) / det + AligST(ISCINTR(IBINTR))**2
           covzt = - ERINTR(6,IBINTR) / det
           covtt =   ERINTR(4,IBINTR) / det
           det = covzz*covtt - covzt**2
           IF( det /= 0.d0 ) THEN
             ERINTR(4,IBINTR) =   covtt / det
             ERINTR(6,IBINTR) = - covzt / det
             ERINTR(5,IBINTR) =   covzz / det
           ENDIF
         ENDIF
       ENDIF
!
       IF (iprloc.eq.0)  CYCLE
       PRINT 6052,IBINTR
       PRINT 6054,CARISC(ISCINTR(IBINTR))
       PRINT 6056,CHINTR(IBINTR)
       PRINT 6057,T0INTR(IBINTR)
       PRINT 6058,IKINTR(IBINTR)
       PRINT 6060,PTINTR(1:3,IBINTR)
       PRINT 6062,VEINTR(1:3,IBINTR)
       PRINT 6706,RecSegLoc(1:3)
       PRINT 6090,DXINTR(1:3,IBINTR)
       PRINT 6092,DYINTR(1:3,IBINTR)
       PRINT 6093,DZINTR(1:3,IBINTR)
       PRINT 6110,SinThetaYZLoc, SinThetaXZLoc
       PRINT 6097,IERINTR(IBINTR)
       if (IERINTR(IBINTR).ne.0) then
         PRINT 6094,1.d0/dsqrt(ERINTR(1,IBINTR))*10.0d0, &
                    1.d0/dsqrt(ERINTR(2,IBINTR))*1000.0d0 , &
                    ERINTR(3,IBINTR)/(dsqrt(ERINTR(1,IBINTR))*dsqrt(ERINTR(2,IBINTR)))
         PRINT 6096,1.d0/dsqrt(ERINTR(4,IBINTR))*10000.0d0, &
                    1.d0/dsqrt(ERINTR(5,IBINTR))*1000000.0d0 , &
                    ERINTR(6,IBINTR)/(dsqrt(ERINTR(4,IBINTR))*dsqrt(ERINTR(5,IBINTR)))
       endif
       PRINT 6063,NUIN
       PRINT 6064,       IUINTR(1:NUIN,IBINTR)
       PRINT 6070,ITUBHI(IUINTR(1:NUIN,IBINTR))
       PRINT 6065,JTUBHI(IUINTR(1:NUIN,IBINTR))
       PRINT 6700,RTUBHI(IUINTR(1:NUIN,IBINTR))
       PRINT 6066,RdigINTR(1:NUIN,IBINTR)
       PRINT 6067,RtraINTR(1:NUIN,IBINTR)
       PRINT 6068,RtraINTR(1:NUIN,IBINTR)-RdigINTR(1:NUIN,IBINTR)
       PRINT 6800,XmLoc(1:NUIN)
       PRINT 6802,YmLoc(1:NUIN)
       PRINT 6804,ZmLoc(1:NUIN)
       PRINT 6100,XmGlo(1:NUIN)
       PRINT 6102,YmGlo(1:NUIN)
       PRINT 6104,ZmGlo(1:NUIN)
       PRINT 6106,RmGlo(1:NUIN)
       PRINT 6200,XmGloO(1:NUIN)
       PRINT 6202,YmGloO(1:NUIN)
       PRINT 6204,ZmGloO(1:NUIN)
       PRINT 6702,IMdtCscLoc(1:NUIN)
       PRINT 6704,I1rst2ndloc(1:NUIN)
       PRINT 6108,LamdYZloc(1:NUIN)
       PRINT 6109,LamdXZloc(1:NUIN)
       PRINT 6080,NUAN
       PRINT 6081,       IUINTR(NUIN+1:NUIN+NUAN,IBINTR)
       PRINT 6082,IBANHI(IUINTR(NUIN+1:NUIN+NUAN,IBINTR))
       PRINT 6083,JBANHI(IUINTR(NUIN+1:NUIN+NUAN,IBINTR))
       PRINT 6810,XmLoc(NUIN+1:NUIN+NUAN)
       PRINT 6812,YmLoc(NUIN+1:NUIN+NUAN)
       PRINT 6814,ZmLoc(NUIN+1:NUIN+NUAN)
       PRINT 6112,XmGlo(NUIN+1:NUIN+NUAN)
       PRINT 6114,YmGlo(NUIN+1:NUIN+NUAN)
       PRINT 6116,ZmGlo(NUIN+1:NUIN+NUAN)
       PRINT 6117,RmGlo(NUIN+1:NUIN+NUAN)
       PRINT 6210,XmGloO(NUIN+1:NUIN+NUAN)
       PRINT 6212,YmGloO(NUIN+1:NUIN+NUAN)
       PRINT 6214,ZmGloO(NUIN+1:NUIN+NUAN)
       PRINT 6118,IRpcTgcloc(NUIN+1:NUIN+NUAN)
       PRINT 6120,I1rst2ndloc(NUIN+1:NUIN+NUAN)
       PRINT 6122,LamdYZloc(NUIN+1:NUIN+NUAN)
       PRINT 6124,LamdXZloc(NUIN+1:NUIN+NUAN)
     ENDDO
   ENDIF
!
6050   FORMAT(/'NContent of Module  M_MB_Reco_Segments  : ',/,' There are ',   &
                I5,' Track segments defined by a Point and a Vector :')
6052   FORMAT('N**> Segment track nb.                :',I5)
6054   FORMAT('N    In station                       :  ',A3)
6056   FORMAT('N    Quality factor                   :',F12.5)
6057   FORMAT('N    Fitted T0                        :',F12.5)
6058   FORMAT('N    ID s of multilayers              :',I7)
6060   FORMAT('N    Coordinates of the Point         :',3F12.5)
6062   FORMAT('N    Coordinates of the Vector        :',3F12.5)
6706   FORMAT('N    Local Coord. of the Vect.()      :',3F12.5)
6090   FORMAT('N    dx                               :',3F12.5)
6092   FORMAT('N    dy                               :',3F12.5)
6093   FORMAT('N    dz                               :',3F12.5)
6110   FORMAT('N    SinThetaYZ, SinThetaXZ           :',2F12.4)
6097   FORMAT('N    error validity                   :',I7)
6094   FORMAT('N    error mat (mm/mrad)              :',3F12.5)
6096   FORMAT('N              (micron/micrad)        :',3F12.5)
6063   FORMAT('N   -Nb. of tube hits used            :',I5)
6064   FORMAT('N    IUINTR=Tube hits numbers         :',12I12)
6070   FORMAT('N    ITUBHI=Tube hits identifiers     :',12I12)
6065   FORMAT('N    JTUBHI=Tube hits flags           :',12I12)
6700   FORMAT('N    RTUBHI=                          :',12F12.4)
6066   FORMAT('N    Tube hits radii                  :',12F12.4)
6067   FORMAT('N    Segment impact parameters        :',12F12.4)
6068   FORMAT('N    Residuals                        :',12F12.4)
6800   FORMAT('N    XmLoc                            :',12F12.4)
6802   FORMAT('N    YmLoc                            :',12F12.4)
6804   FORMAT('N    ZmLoc                            :',12F12.4)
6100   FORMAT('N    XmGlo                            :',12F12.4)
6102   FORMAT('N    YmGlo                            :',12F12.4)
6104   FORMAT('N    ZmGlo                            :',12F12.4)
6106   FORMAT('N    RmGlo                            :',12F12.4)
6200   FORMAT('N    XmGloO                           :',12F12.4)
6202   FORMAT('N    YmGloO                           :',12F12.4)
6204   FORMAT('N    ZmGloO                           :',12F12.4)
6702   FORMAT('N    IMdtCscLoc                       :',12I12)
6704   FORMAT('N    I1rst2nd                         :',12I12)
6108   FORMAT('N    LamdYZLoc                        :',12F12.4)
6109   FORMAT('N    LamdXZLoc                        :',12F12.4)
6080   FORMAT('N   -Nb. of strip hits used           :',I5)
6081   FORMAT('N    IUINTR=Strip hits numbers        :',12I12)
6082   FORMAT('N    IBANHI=Strip hits identifiers    :',12I12)
6083   FORMAT('N    JBANHI=Strip hits flags          :',12I12)
6810   FORMAT('N    XmLoc                            :',12F12.4)
6812   FORMAT('N    YmLoc                            :',12F12.4)
6814   FORMAT('N    ZmLoc                            :',12F12.4)
6112   FORMAT('N    XmGlo                            :',12F12.4)
6114   FORMAT('N    YmGlo                            :',12F12.4)
6116   FORMAT('N    ZmGlo                            :',12F12.4)
6117   FORMAT('N    RmGlo                            :',12F12.4)
6210   FORMAT('N    XmGloO                           :',12F12.4)
6212   FORMAT('N    YmGloO                           :',12F12.4)
6214   FORMAT('N    ZmGloO                           :',12F12.4)
6118   FORMAT('N    IRpcTgc                          :',12I12)
6120   FORMAT('N    I1rst2nd                         :',12I12)
6122   FORMAT('N    LamdYZLoc                        :',12F12.4)
6124   FORMAT('N    LamdXZLoc                        :',12F12.4)
!
 END SUBROUTINE ErrorSegment
!
