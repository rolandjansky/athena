!
 SUBROUTINE CUTAND( NPOL,JPOL,XPOL,YPOL, NCUT,JCUT,XCUT,YCUT, NDIM1,NDIM2, LRR,NRR,JRR,XRR,YRR )
 IMPLICIT NONE
!
!>> Compute the intersection
!>> of the polygon made of the    NPOL  points   XPOL(i),YPOL(i)
!>> with the polygon made of the  NCUT  points   XCUT(i),YCUT(i)
!>> and give the result as
!>> LRR polygons made of the   NRR(LRR) points  XRR(i,LRR),YRR(i,LRR)
!
 INTEGER, PARAMETER :: NDIM3=500
 INTEGER, INTENT(IN) :: NDIM1, NDIM2, NPOL, NCUT
 INTEGER :: JPOL(*),        JCUT(*)
 REAL(8) :: XPOL(*),YPOL(*),XCUT(*),YCUT(*)
 INTEGER :: JPP(NDIM3),           JCC(NDIM3)
 REAL(8) :: XPP(NDIM3),YPP(NDIM3),XCC(NDIM3),YCC(NDIM3)
 REAL(8) :: EE( NDIM3)
 INTEGER :: JSEG(  NDIM3)
 REAL(8) :: XSEG(2,NDIM3),YSEG(2,NDIM3)
 INTEGER :: ISEG(NDIM3)
 INTEGER :: NRR(NDIM2)
 INTEGER :: JRR(NDIM1,NDIM2)
 REAL(8) :: XRR(NDIM1,NDIM2), YRR(NDIM1,NDIM2)
 REAL(8) :: XCUR, YCUR
 INTEGER :: ICUT, IPOL, LRR, I, NPP, IP1, NE, J, NCC, NSEG, ITRY
 LOGICAL :: INSIDE
 REAL(8), PARAMETER :: EPS=0.0010D0
!
      LRR = 0
      NRR(1) = 0
      IF( NPOL <= 0 .OR. NCUT <= 0 ) RETURN
!
!>> Is the cutout (XCUT,YCUT) entirely inside the polygone (XPOL,YPOL) ?
      DO ICUT=1,NCUT
        IF( .NOT.INSIDE(XCUT(ICUT),YCUT(ICUT),NPOL,XPOL,YPOL) ) GO TO 30
      ENDDO
      DO IPOL=1,NPOL
        IF( INSIDE(XPOL(IPOL),YPOL(IPOL),NCUT,XCUT,YCUT) )      GO TO 30
      ENDDO
      LRR       = 1
      NRR(1)    = NCUT
      JRR(1:NCUT,1) = JCUT(1:NCUT)
      XRR(1:NCUT,1) = XCUT(1:NCUT)
      YRR(1:NCUT,1) = YCUT(1:NCUT)
      RETURN
!
!>> Is the polygone (XPOL,YPOL) entirely inside the cutout (XCUT,YCUT) ?
30    CONTINUE
      IF( ICUT >= 2 )                                            GO TO 50
      DO ICUT=2,NCUT
        IF( INSIDE(XCUT(ICUT),YCUT(ICUT),NPOL,XPOL,YPOL) )       GO TO 50
      ENDDO
      DO IPOL=1,NPOL
        IF( .NOT. INSIDE(XPOL(IPOL),YPOL(IPOL),NCUT,XCUT,YCUT) ) GO TO 50
      ENDDO
      LRR       = 1
      NRR(1)    = NPOL
      JRR(1:NPOL,1) = JPOL(1:NPOL)
      XRR(1:NPOL,1) = XPOL(1:NPOL)
      YRR(1:NPOL,1) = YPOL(1:NPOL)
      RETURN
!
!>> General case
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
          IF( NCC >= NDIM3 ) RETURN
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
        IF( INSIDE(XCUR,YCUR,NCUT,XCUT,YCUT) ) THEN
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
b180: DO 180 I=1,NSEG
        IF( ISEG(I) >= 1)  CYCLE
        IF( LRR >= NDIM2 ) RETURN
        ISEG(I) = 1
        LRR = LRR + 1
        NRR(LRR) = 2
        JRR(1,LRR) = JSEG(  I)
        XRR(1,LRR) = XSEG(1,I)
        YRR(1,LRR) = YSEG(1,I)
        XRR(2,LRR) = XSEG(2,I)
        YRR(2,LRR) = YSEG(2,I)
        DO 140 ITRY=1,1000
          DO J=I,NSEG
            IF( ISEG(J) >= 1 )                                CYCLE
            IF( JRR(NRR(LRR)-1,LRR) <= 0 .AND. JSEG(J) <= 0 ) CYCLE
            IF( ABS(XRR(NRR(LRR),LRR)-XSEG(1,J)) < EPS .AND.  &
                ABS(YRR(NRR(LRR),LRR)-YSEG(1,J)) < EPS ) THEN
              ISEG(J) = 1
              JRR(NRR(LRR)  ,LRR) = JSEG(  J)
              XRR(NRR(LRR)+1,LRR) = XSEG(2,J)
              YRR(NRR(LRR)+1,LRR) = YSEG(2,J)
              IF( ABS(XRR(1,LRR)-XSEG(2,J)) < EPS .AND. JSEG(J) >= 1 .AND.   &
                  ABS(YRR(1,LRR)-YSEG(2,J)) < EPS ) CYCLE b180
              IF( NRR(LRR) > NDIM1 )                CYCLE b180
              NRR(LRR) = NRR(LRR) + 1
            ELSE IF( ABS(XRR(NRR(LRR),LRR)-XSEG(2,J)) < EPS .AND.  &
                     ABS(YRR(NRR(LRR),LRR)-YSEG(2,J)) < EPS ) THEN
              ISEG(J) = 1
              JRR(NRR(LRR)  ,LRR) = JSEG(  J)
              XRR(NRR(LRR)+1,LRR) = XSEG(1,J)
              YRR(NRR(LRR)+1,LRR) = YSEG(1,J)
              IF( ABS(XRR(1,LRR)-XSEG(1,J)) < EPS .AND. JSEG(J) >= 1 .AND.  &
                  ABS(YRR(1,LRR)-YSEG(1,J)) < EPS ) CYCLE b180
              IF( NRR(LRR) > NDIM1 )                CYCLE b180
              NRR(LRR) = NRR(LRR) + 1
            ENDIF
          ENDDO
140     ENDDO
180   ENDDO b180
!
 END SUBROUTINE CUTAND
!
