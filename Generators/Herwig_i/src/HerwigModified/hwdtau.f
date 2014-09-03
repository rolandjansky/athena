CDECK  ID>, HWDTAU.
*CMZ :-        -17/10/01  09:42:21  by  Peter Richardson
*-- Author :    Peter Richardson
C
C   Modified by Borut Kersevan to adjust for Atlas specific architecture:
C   initialization (IOPT=-1) and finalizing (IOPT=2) are done in Tauola_i     
C
C   Oct.3, 2008:  Vladimir Savinov (V.S.) and Reza Yoosoofmiya (R.Y.) fixed two problems:  
C                 1) on Feb.21, 2008 IFPHOT=0 statement was accidentally deleted 
C                    from this source code - but default value for this variable is set to 1 in 
C                    HERWIG's src/hwsteer/hwigin.F, as the result, src/hwsteer/hwuinc.f reinitializes
C                    PHOTOS with defaults different from ATLAS's defaults which results in less 
C                    FSR at Photos_i/PHOTOS level at a later time. The mistakenly deleted line is put back in. 
C                 2) Introduced a few lines of code to force decays of pi0s produced in tau decays.
C                 Notice that we allow HERWIG to decay taus but FSR will be generated in Photos_i's calls to PHOTOS
C
C-----------------------------------------------------------------------
      SUBROUTINE HWDTAU(IOPT,IHEP,POL)
C-----------------------------------------------------------------------
C     HERWIG-TAUOLA interface to perform tau decays using TAUOLA rather
C     than HERWIG
C     IOPT =-1 initialises
C     IOPT = 1 performs decay
C     IOPT = 2 write outs final TAUOLA information
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'

C     Les Houches accord common block hep-ph/0109068
C...User process event commonblock.
c     Ian Hinchliffe (May 2002)
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &VTIMUP(MAXNUP),SPINUP(MAXNUP)
      SAVE /HEPEUP/
      INTEGER IMOTAU

C     Les Houches accord common block
C     Ian Hinchliffe (May 2002)
C     this one added for atlas use
      INTEGER ATLASCHOICE
      COMMON/LESATLAS/ATLASCHOICE
      SAVE /LESATLAS/

C     Generator Names include
C     while adding a new generator change in GeneratorName.h is needed
C     too
c     E.M. L.12.12.2008
c this one added for atlas use
      INTEGER ALPGEN,MCATNLO,LHAEXT
      PARAMETER (ALPGEN=400000,MCATNLO=900000,LHAEXT=800000) 

      INTEGER IOPT,IHEP,ID,ITAU,I,IMO,NHEPPO
      DOUBLE PRECISION POL
      REAL POL1(4)
      CHARACTER *8 DUMMY
C--common block for PHOTOS
C(JMB)      LOGICAL QEDRAD
C(JMB)      COMMON /PHOQED/ QEDRAD(NMXHEP)
C--common blocks for TAUOLA
      INTEGER NP1,NP2
      COMMON /TAUPOS/ NP1, NP2
      DOUBLE PRECISION Q1(4),Q2(4),P1(4),P2(4),P3(4),P4(4)      
      COMMON / MOMDEC / Q1,Q2,P1,P2,P3,P4
C(JMB)-->
      DOUBLE PRECISION DIST(4),VERTX(4)
C(JMB)--<

C(BPK)-->

C TAUOLA PARAMETER COMMON
      INTEGER IFPOL,IFRADCOR,IFDMOD,IFJAK1,IFJAK2
      REAL FFPHX
      COMMON /FFTAU/IFPOL,IFRADCOR,IFDMOD,IFJAK1,IFJAK2,FFPHX

C(BPK)--<

C--->>>  V.S. and R.Y.  Oct.3, 2008
      IFPHOT = 0
C---<<<  V.S. and R.Y.  Oct.3, 2008


C--initialisation
      IF(IOPT.EQ.-1) THEN
C--initialise TAUOLA
C(BPK)  Done elsewere in Atlas s/w
C       CALL INIETC(JAK1,JAK2,ITDKRC,IFPHOT)
C       CALL INIMAS
C       CALL INIPHX(0.01d0)
C       CALL INITDK

C--generate a decay
      ELSEIF(IOPT.EQ.1) THEN

C(BPK)--> 
         IF (IFDMOD.EQ.1) THEN  ! LEP-LEP
            CALL TAUBRS(1)
         ELSEIF (IFDMOD.EQ.2) THEN ! HAD-HAD
            CALL TAUBRS(2)
         ELSEIF (IFDMOD.EQ.3) THEN ! LEP-HAD
            PRINT *,
     # 'HWDTAU: LEP-HAD MODE NOT POSSIBLE IN INTERNAL HERWIG PROCESSES'
            STOP
         ELSEIF (IFDMOD.EQ.4) THEN ! TAU->PI NU
            CALL TAUBRS(4)
         ELSE                   ! INCLUSIVE DECAY
            CALL TAUBRS(0)
         ENDIF
C(BPK)--<

        ISTHEP(IHEP)=195
        ID = IDHW(IHEP)
        IMO = IHEP
 1      IMO = JMOHEP(1,IMO)
        IF(IDHW(IMO).EQ.ID) GOTO 1
C--id of tau for tauola
        IF(ID.EQ.125) THEN
          ITAU = 2
          NP1 = IHEP
          NP2 = IHEP
        ELSEIF(ID.EQ.131) THEN
          ITAU = 1
          NP1 = IHEP
          NP2 = IHEP
        ELSE
          CALL HWWARN('HWDTAU',501)
        ENDIF
C--set up the tau polarization
        POL1(1) = 0.
        POL1(2) = 0.
        POL1(3) = REAL(POL)
        POL1(4) = 0.

C...=================================================================
C...S.T. 
C...fill polarization if external generator has the spin information.
C...So far, only consider Alpgen.
        IF (ATLASCHOICE.EQ.ALPGEN.OR.ATLASCHOICE.EQ.MCATNLO
     $  .OR.ATLASCHOICE.EQ.LHAEXT) THEN
C          write(*,*) "ST...========================================"
C          write(*,*) "ST...Events/NHEP before Tauola",NEVHEP,NHEP
C          write(*,*) "ST...Tauola",IHEP,IMO,ID,ITAU,ATLASCHOICE,POL,
C     $               IFPOL

          IF (IMO.LT.100) THEN
C...Find mother particle of tau to ask exact mathing of 4-vector.
            IMOTAU = 0
 23         DO I=1,NUP
C              write(*,*) "Check ALL",I,IDUP(I),IDHEP(IMO),
C     $        IMO,JMOHEP(1,IMO),
C     $        PUP(1,I),PHEP(1,IMO)
              IF (IABS(IDUP(I)).GT.22.AND.IABS(IDUP(I)).LT.26
     $            .AND.IDUP(I).EQ.IDHEP(IMO)) THEN
C                IF (ABS(PUP(1,I)-PHEP(1,IMO)).LT.1.0E-3
C     $         .AND.ABS(PUP(2,I)-PHEP(2,IMO)).LT.1.0E-3) THEN
C              write(*,*) "Check 4-vector PHEP:",ISTHEP(IMO),IDHEP(IMO),
C     $        PHEP(1,IMO),PHEP(2,IMO),PHEP(3,IMO),PHEP(4,IMO)
C              write(*,*) "Check 4-vector LHA:",I,IDUP(I),
C     $        PUP(1,I),PUP(2,I),PUP(3,I),PUP(4,I)
                  IMOTAU = I
C                ENDIF
              ENDIF
            ENDDO
            IF (IMOTAU.EQ.0.AND.IMO.NE.6) THEN 
              IMO = JMOHEP(1,IMO)
              GO TO 23
            ENDIF
            IF (IMOTAU.EQ.0.AND.IMO.EQ.6) THEN 
              DO I=1,NUP
                IF (IABS(IDUP(I)).GT.22.AND.IABS(IDUP(I)).LT.26
     $              .AND.IDUP(I).EQ.IDHEP(IMO)) THEN
                  IMOTAU = I
                ENDIF
              ENDDO
            ENDIF

C...Find tau as daughter, so that the spin information is directly filled.
            DO I=1,NUP
               IF(MOTHUP(1,I).GT.0.AND.IMOTAU.GT.0) THEN 
                  IF (IDUP(I).EQ.IDHEP(IHEP)
     $                 .AND.IDUP(MOTHUP(1,I)).EQ.IDUP(IMOTAU)) THEN
C                 write(*,*) "ST...TAU candidate",I,IDUP(I),IDUP(IMOTAU),
C     $           PUP(1,I),PUP(2,I),PUP(3,I),PUP(4,I),SPINUP(I)
                     POL1(3) = SPINUP(I)
                  ENDIF
               ENDIF
            ENDDO

C            write(*,*) "ST...Tauola polarization",IDHEP(IHEP),POL1(3)
          ENDIF
        ENDIF
C...=================================================================


C--tau momentum
C--three components
        DO I=1,3
           IF(ID.EQ.125) THEN
              P1(I) =-PHEP(I,IHEP)
              P2(I) = PHEP(I,IHEP)
           ELSE
              P1(I) = PHEP(I,IHEP)
              P2(I) =-PHEP(I,IHEP)
           ENDIF
C--we measure tau spins in lab frame
          Q1(I) = ZERO
        ENDDO
C--energies
        P1(4)=PHEP(4,IHEP)
        P2(4)=PHEP(4,IHEP)
        Q1(4)=P1(4)+P2(4)
C--perform the decay and generate QED radiation if needed
        NHEPPO=NHEP
        CALL DEXAY(ITAU,POL1)

C--->>> V.S. and R.Y.  Oct.3, 2008
        if ( ID .eq. 1 ) then
          CALL TAUPI0(0,1)
        else
          CALL TAUPI0(0,2)
        endif
C---<<< V.S. and R.Y.  Oct.3, 2008

C(JMB)--> photos calls done elsewhere       
C        IF(IFPHOT.EQ.1) THEN
C          IF(ID.EQ.1) THEN
C            CALL PHOTOS(NP1)
C          ELSE
C            CALL PHOTOS(NP2)
C          ENDIF
C        ENDIF
C(JMB)--> add tau deay vertex info.
        CALL HWUDKL(125,PHEP(1,IHEP),DIST)
        CALL HWVSUM(4,VHEP(1,IHEP),DIST,VERTX)
C(JMB)--<
        IF(NHEPPO.NE.NHEP) THEN
          DO 2 I=NHEPPO+1,NHEP
C(JMB)          CALL HWVEQU(4,VHEP(1,IHEP),VHEP(1,I))
          CALL HWVEQU(4,VERTX,VHEP(1,I))
 2        CALL HWUIDT(1,IDHEP(I),IDHW(I),DUMMY)
        ENDIF


C--write out info at end
      ELSEIF(IOPT.EQ.2) THEN
C(BPK)  Done elsewere in Atlas s/w
C       CALL DEXAY(100,POL1)

C--otherwise issue warning
      ELSE
        CALL HWWARN('HWDTAU',500)
      ENDIF
      END
