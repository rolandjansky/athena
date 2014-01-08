!
!> Cuts the polygon made of the  NPOL  points   XPOL(i),YPOL(i)
!> by the polygon made of the    NCUT  points   XCUT(i),YCUT(i)
!> and give the result as
!> LRR polygons made of the   NRR(LRR) points  XRR(i,LRR),YRR(i,LRR)
!> \author SaMuSoG
!
      SUBROUTINE CUTOUT( NPOL,JPOL,XPOL,YPOL, NCUT,JCUT,XCUT,YCUT,   &
                         NDIM1,NDIM2,   LRR,NRR,JRR,XRR,YRR )
      IMPLICIT REAL(8) (A-H,O-Z)
!
#include "AmdcStand/coforc.inc"
!
      PARAMETER (NDIM3=500)
      DIMENSION JPOL(*),        JCUT(*)
      DIMENSION XPOL(*),YPOL(*),XCUT(*),YCUT(*)
      DIMENSION JPP(NDIM3),           JCC(NDIM3)
      DIMENSION XPP(NDIM3),YPP(NDIM3),XCC(NDIM3),YCC(NDIM3)
      DIMENSION EE(NDIM3)
      DIMENSION JSEG(  NDIM3)
      DIMENSION XSEG(2,NDIM3),YSEG(2,NDIM3)
      DIMENSION ISEG(NDIM3)
      DIMENSION NRR(NDIM2)
      DIMENSION JRR(NDIM1,NDIM2)
      DIMENSION XRR(NDIM1,NDIM2),YRR(NDIM1,NDIM2)
!
      LOGICAL INSIDE
      DATA EPS/ 0.0010D0 /
!
      LRR = 0
      NRR(1) = 0
      IF( NPOL.LE.0 ) RETURN
      IF( NCUT.LE.0 ) THEN
        LRR       = 1
        NRR(1)    = NPOL
        JRR(1:NPOL,1) = JPOL(1:NPOL)
        XRR(1:NPOL,1) = XPOL(1:NPOL)
        YRR(1:NPOL,1) = YPOL(1:NPOL)
        RETURN
      ENDIF
!
!-- Is the cutout (XCUT,YCUT) entirely inside the polygone (XPOL,YPOL) ?
      DO ICUT=1,NCUT
        IF(.NOT. INSIDE(XCUT(ICUT),YCUT(ICUT),NPOL,XPOL,YPOL) ) GO TO 30
      ENDDO
      DO IPOL=1,NPOL
        IF( INSIDE(XPOL(IPOL),YPOL(IPOL),NCUT,XCUT,YCUT) )      GO TO 30
      ENDDO
      IF( IPFORC >= 1 .AND. IPFORC <= NPOL .AND. ICFORC >= 1 .AND. ICFORC <= NCUT ) THEN
        IP0 = IPFORC
        IC0 = ICFORC
      ELSE
        IP0 = 1
        IC0 = 1
        DD0 = (XPOL(1)-XCUT(1))**2 + (YPOL(1)-YCUT(1))**2
        DO IP=1,NPOL
          DO IC=1,NCUT
            DD  = (XPOL(IP)-XCUT(IC))**2 + (YPOL(IP)-YCUT(IC))**2
            IF( DD.GE.DD0 ) CYCLE
            IP0 = IP
            IC0 = IC
            DD0 = DD
          ENDDO
        ENDDO
      ENDIF
      LRR = 1
      NRR(LRR) = 1
      JRR(NRR(LRR),LRR)  = 0
      XRR(NRR(LRR),LRR)  = XPOL(IP0)
      YRR(NRR(LRR),LRR)  = YPOL(IP0)
      DO J=IC0,NCUT+IC0-1
        I = MOD(J-1,NCUT) + 1
        NRR(LRR) = NRR(LRR) + 1
        JRR(NRR(LRR),LRR) = JCUT(I)
        XRR(NRR(LRR),LRR) = XCUT(I)
        YRR(NRR(LRR),LRR) = YCUT(I)
      ENDDO
      NRR(LRR) = NRR(LRR) + 1
      JRR(NRR(LRR),LRR)  = 0
      XRR(NRR(LRR),LRR)  = XCUT(IC0)
      YRR(NRR(LRR),LRR)  = YCUT(IC0)
      DO J=IP0,NPOL+IP0-1
        I = MOD(J-1,NPOL) + 1
        NRR(LRR) = NRR(LRR) + 1
        JRR(NRR(LRR),LRR) = JPOL(I)
        XRR(NRR(LRR),LRR) = XPOL(I)
        YRR(NRR(LRR),LRR) = YPOL(I)
      ENDDO
      RETURN
!
!-- Is the polygone (XPOL,YPOL) entirely inside the cutout (XCUT,YCUT) ?
30    CONTINUE
      IF( ICUT.GE.2 )                                           GO TO 50
      DO ICUT=2,NCUT
        IF( INSIDE(XCUT(ICUT),YCUT(ICUT),NPOL,XPOL,YPOL) )      GO TO 50
      ENDDO
      DO IPOL=1,NPOL
        IF(.NOT. INSIDE(XPOL(IPOL),YPOL(IPOL),NCUT,XCUT,YCUT) ) GO TO 50
      ENDDO
      RETURN
!
!-- General case
50    CONTINUE
      NPP = 0
      DO 60 I=1,NPOL
        IP1 = MOD(I,NPOL) + 1
        CALL INTERS(NCUT,XCUT,YCUT,XPOL(I),YPOL(I),XPOL(IP1),YPOL(IP1),NDIM3, NE,EE)
        NPP = NPP + 1
        IF( NPP >= NDIM3 ) RETURN
        JPP(NPP) = JPOL(I)
        XPP(NPP) = XPOL(I)
        YPP(NPP) = YPOL(I)
        DO J=1,NE
          NPP = NPP + 1
          IF(NPP.GE.NDIM3) RETURN
          JPP(NPP) = JPOL(I)
          XPP(NPP) = XPOL(I) + EE(J)*(XPOL(IP1)-XPOL(I))
          YPP(NPP) = YPOL(I) + EE(J)*(YPOL(IP1)-YPOL(I))
        ENDDO
60    ENDDO
!
      NCC = 0
      DO 70 I=1,NCUT
        IP1 = MOD(I,NCUT) + 1
        CALL INTERS(NPOL,XPOL,YPOL,XCUT(I),YCUT(I),XCUT(IP1),YCUT(IP1),NDIM3, NE,EE)
        NCC = NCC + 1
        IF( NCC >= NDIM3 ) RETURN
        JCC(NCC) = JCUT(I)
        XCC(NCC) = XCUT(I)
        YCC(NCC) = YCUT(I)
        DO J=1,NE
          NCC = NCC + 1
          IF(NCC.GE.NDIM3) RETURN
          JCC(NCC) = JCUT(I)
          XCC(NCC) = XCUT(I) + EE(J)*(XCUT(IP1)-XCUT(I))
          YCC(NCC) = YCUT(I) + EE(J)*(YCUT(IP1)-YCUT(I))
        ENDDO
70    ENDDO
!
      NSEG = 0
      DO I=1,NPP
        IP1 = MOD(I,NPP) + 1
        XCUR = (XPP(I)+XPP(IP1))/2.D0
        YCUR = (YPP(I)+YPP(IP1))/2.D0
        IF( .NOT. INSIDE(XCUR,YCUR,NCUT,XCUT,YCUT) ) THEN
          IF( NSEG >= NDIM3 ) RETURN
          NSEG = NSEG + 1
          JSEG(  NSEG) = JPP(I)
          XSEG(1,NSEG) = XPP(I)
          YSEG(1,NSEG) = YPP(I)
          XSEG(2,NSEG) = XPP(IP1)
          YSEG(2,NSEG) = YPP(IP1)
        ENDIF
      ENDDO
      DO I=1,NCC
        IP1 = MOD(I,NCC) + 1
        XCUR = (XCC(I)+XCC(IP1))/2.D0
        YCUR = (YCC(I)+YCC(IP1))/2.D0
        IF( INSIDE(XCUR,YCUR,NPOL,XPOL,YPOL) ) THEN
          IF( NSEG >= NDIM3 ) RETURN
          NSEG = NSEG + 1
          JSEG(  NSEG) = JCC(I)
          XSEG(1,NSEG) = XCC(I)
          YSEG(1,NSEG) = YCC(I)
          XSEG(2,NSEG) = XCC(IP1)
          YSEG(2,NSEG) = YCC(IP1)
        ENDIF
      ENDDO
!
      ISEG(1:NSEG) = 0
      DO 180 I=1,NSEG
         IF( ISEG(I) >= 1 ) CYCLE
         IF( LRR >= NDIM2 ) RETURN
         ISEG(I) = 1
         LRR = LRR + 1
         NRR(LRR) = 2
         JRR(1,LRR) = JSEG(  I)
         XRR(1,LRR) = XSEG(1,I)
         YRR(1,LRR) = YSEG(1,I)
         XRR(2,LRR) = XSEG(2,I)
         YRR(2,LRR) = YSEG(2,I)
         DO 141 ITRY=1,1000
           DO 140 J=I,NSEG
             IF( ISEG(J).GE.1 )                                CYCLE
             IF( JRR(NRR(LRR)-1,LRR).LE.0 .AND. JSEG(J).LE.0 ) CYCLE
             IF( ABS(XRR(NRR(LRR),LRR)-XSEG(1,J)).LT.EPS .AND.    &
                 ABS(YRR(NRR(LRR),LRR)-YSEG(1,J)).LT.EPS ) THEN
               ISEG(J) = 1
               JRR(NRR(LRR)  ,LRR) = JSEG(  J)
               XRR(NRR(LRR)+1,LRR) = XSEG(2,J)
               YRR(NRR(LRR)+1,LRR) = YSEG(2,J)
               IF( ABS(XRR(1,LRR)-XSEG(2,J)).LT.EPS .AND. JSEG(J).GE.1 .AND.  &
                   ABS(YRR(1,LRR)-YSEG(2,J)).LT.EPS ) GO TO 180
               IF(NRR(LRR).GT.NDIM1)                  GO TO 180
               NRR(LRR) = NRR(LRR) + 1
             ELSEIF( ABS(XRR(NRR(LRR),LRR)-XSEG(2,J)).LT.EPS .AND.   &
                     ABS(YRR(NRR(LRR),LRR)-YSEG(2,J)).LT.EPS ) THEN
               ISEG(J) = 1
               JRR(NRR(LRR)  ,LRR) = JSEG(  J)
               XRR(NRR(LRR)+1,LRR) = XSEG(1,J)
               YRR(NRR(LRR)+1,LRR) = YSEG(1,J)
               IF( ABS(XRR(1,LRR)-XSEG(1,J)).LT.EPS .AND. JSEG(J).GE.1 .AND.   &
                   ABS(YRR(1,LRR)-YSEG(1,J)).LT.EPS ) GO TO 180
               IF(NRR(LRR).GT.NDIM1)                  GO TO 180
               NRR(LRR) = NRR(LRR) + 1
             ENDIF
140        ENDDO
141      ENDDO
180   CONTINUE
!
      END SUBROUTINE CUTOUT
!
