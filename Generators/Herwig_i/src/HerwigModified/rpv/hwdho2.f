CDECK  ID>, HWDH02.
*CMZ :-        -30/09/02  14:05:28  by  Peter Richardson
*-- Author :    Ian Knowles & Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE HWDHO2(IHEP,IM,NPR,MHEP,LHEP,KHEP,PW)
C-----------------------------------------------------------------------
C   Subroutine to perform the second part of the heavy object decays
C   IE generate the kinematics for the decay
C   was part of HWDHOB
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      COMMON/FFS/TB,BT
      COMMON/SFF/IT1,IB1,IT2,IB2
      DOUBLE PRECISION TB,BT
      INTEGER IT1,IB1,IT2,IB2,ISP
      DOUBLE PRECISION GAMHPM
      DOUBLE PRECISION HWUPCM,HWRGEN,PCM,
     & EMMX,EMWSQ,GMWSQ,EMLIM,PW(5),EMTST,HWDPWT,HWDWWT,HWULDO,HWDHWT
      DOUBLE COMPLEX RHOIN(2,2,2)
      INTEGER IHEP,IM,KHEP,LHEP,MHEP,NPR,RHEP
      EXTERNAL HWRGEN,HWDPWT,HWDWWT,HWDHWT
      SAVE RHOIN
      DATA RHOIN/(1.0D0,0.0D0),(0.0D0,0.0D0),
     &           (0.0D0,0.0D0),(0.0D0,0.0D0),
     &           (0.5D0,0.0D0),(0.0D0,0.0D0),
     &           (0.0D0,0.0D0),(0.5D0,0.0D0)/
      ISP = INT(2*RSPIN(IDHW(IHEP)))+1
      IF (IERROR.NE.0) RETURN
      IF (NPR.EQ.2) THEN
C Two body decay: LHEP -> MHEP + NHEP
        IF(NME(IM).GT.20000.AND.NME(IM).LT.30000) THEN
C--generate a two body decay to a gauge boson as a three body decay
          CALL HWDSM3(2,IHEP,MHEP,NHEP,0,NME(IM)-20000,RHOIN(1,1,ISP),1)
C--generate a two body decay of a Higgs to two gauge bosons
        ELSEIF(NME(IM).GT.40000.AND.NME(IM).LT.50000) THEN
          CALL HWDSM4(1,IHEP,MHEP,NHEP,NME(IM)-40000)
C--if spin correlations call the routine to set-up the matrix element
        ELSEIF(SYSPIN.AND.NME(IM).GE.30000.AND.NME(IM).LE.40000) THEN
          CALL HWDSM2(IHEP,MHEP,NHEP,NME(IM)-30000,RHOIN(1,1,ISP),1)
        ELSE
          PCM=HWUPCM(PHEP(5,IHEP),PHEP(5,MHEP),PHEP(5,NHEP))
          CALL HWDTWO(PHEP(1,IHEP),PHEP(1,MHEP),
     &                PHEP(1,NHEP),PCM,TWO,.FALSE.)
        ENDIF
      ELSEIF (NPR.EQ.3) THEN
C Three body decay: LHEP -> KHEP + MHEP + NHEP
        KHEP=MHEP
        MHEP=MHEP+1
C Provisional colour self-connection of KHEP
        JMOHEP(2,KHEP)=KHEP
        JDAHEP(2,KHEP)=KHEP
        IF (NME(IM).EQ.100) THEN
C Generate decay momenta using full (V-A)*(V-A) matrix element
          EMMX=PHEP(5,IHEP)-PHEP(5,NHEP)
          EMWSQ=RMASS(198)**2
          GMWSQ=(RMASS(198)*GAMW)**2
          EMLIM=GMWSQ
          IF (EMMX.LT.RMASS(198)) EMLIM=EMLIM+(EMWSQ-EMMX**2)**2
  50      CALL HWDTHR(PHEP(1,IHEP),PHEP(1,MHEP),
     &                PHEP(1,KHEP),PHEP(1,NHEP),HWDWWT)
          CALL HWVSUM(4,PHEP(1,KHEP),PHEP(1,MHEP),PW)
          PW(5)=HWULDO(PW,PW)
          EMTST=(EMWSQ-PW(5))**2
          IF ((EMTST+GMWSQ)*HWRGEN(1).GT.EMLIM) GOTO 50
          PW(5)=SQRT(PW(5))
C Assign production vertices to 1 and 2
          CALL HWUDKL(198,PW,VHEP(1,KHEP))
          CALL HWVSUM(4,VHEP(1,NHEP),VHEP(1,KHEP),VHEP(1,KHEP))
        ELSE IF (NME(IM).EQ.200) THEN
C Generate decay momenta using full
C ((V-A)*TB1+(V+A)*CT1)*((V-A)*TB2+(V+A)*CT2)) matrix element
          GAMHPM=RMASS(206)/DKLTM(206)
C sort tan(beta)
          IF((IDK(IM).EQ.  2).OR.(IDK(IM).EQ.  4).OR.
     &       (IDK(IM).EQ.  6).OR.(IDK(IM).EQ.  8).OR.
     &       (IDK(IM).EQ. 10).OR.(IDK(IM).EQ. 12).OR.
     &       (IDK(IM).EQ.122).OR.(IDK(IM).EQ.124).OR.
     &       (IDK(IM).EQ.126).OR.(IDK(IM).EQ.128).OR.
     &       (IDK(IM).EQ.130).OR.(IDK(IM).EQ.132))THEN
            TB=TANB
          ELSE
            TB=1./TANB
          END IF
          IF((IDKPRD(1,IM).EQ.  2).OR.(IDKPRD(1,IM).EQ.  4).OR.
     &       (IDKPRD(1,IM).EQ.  6).OR.(IDKPRD(1,IM).EQ.  8).OR.
     &       (IDKPRD(1,IM).EQ. 10).OR.(IDKPRD(1,IM).EQ. 12).OR.
     &       (IDKPRD(1,IM).EQ.122).OR.(IDKPRD(1,IM).EQ.124).OR.
     &       (IDKPRD(1,IM).EQ.126).OR.(IDKPRD(1,IM).EQ.128).OR.
     &       (IDKPRD(1,IM).EQ.130).OR.(IDKPRD(1,IM).EQ.132))THEN
            BT=TANB
          ELSE
            BT=1./TANB
          END IF
          IT1=IDK(IM)
          IB1=IDKPRD(3,IM)
          IT2=IDKPRD(1,IM)
          IB2=IDKPRD(2,IM)
          EMMX=PHEP(5,IHEP)-PHEP(5,NHEP)
          EMWSQ=RMASS(206)**2
          GMWSQ=(RMASS(206)*GAMHPM)**2
          EMLIM=GMWSQ
          IF (EMMX.LT.RMASS(206)) EMLIM=EMLIM+(EMWSQ-EMMX**2)**2
  55      CALL HWDTHR(PHEP(1,IHEP),PHEP(1,NHEP),
     &                PHEP(1,KHEP),PHEP(1,MHEP),HWDHWT)
          CALL HWVSUM(4,PHEP(1,KHEP),PHEP(1,MHEP),PW)
          PW(5)=HWULDO(PW,PW)
          EMTST=(EMWSQ-PW(5))**2
          IF ((EMTST+GMWSQ)*HWRGEN(2).GT.EMLIM) GOTO 55
          PW(5)=SQRT(PW(5))
C Assign production vertices to 1 and 2
          CALL HWUDKL(206,PW,VHEP(1,KHEP))
          CALL HWVSUM(4,VHEP(1,NHEP),VHEP(1,KHEP),VHEP(1,KHEP))
        ELSEIF(NME(IM).EQ.300) THEN
C Generate momenta using 3-body RPV matrix element
          CALL HWDRME(LHEP,KHEP)
C--Three body SUSY decay
        ELSEIF(NME(IM).GE.10000.AND.NME(IM).LT.20000) THEN
          CALL HWDSM3(3,IHEP,MHEP,KHEP,NHEP,NME(IM)-10000,
     &                RHOIN(1,1,ISP),1)
C--special for top decay
          IF(ABS(IDHEP(IHEP)).EQ.6) THEN
            CALL HWVSUM(4,PHEP(1,KHEP),PHEP(1,MHEP),PW)
            CALL HWUMAS(PW)
          ENDIF
        ELSE
C Three body phase space decay
          CALL HWDTHR(PHEP(1,IHEP),PHEP(1,MHEP),
     &                PHEP(1,KHEP),PHEP(1,NHEP),HWDPWT)
        ENDIF
        CALL HWVEQU(4,VHEP(1,KHEP),VHEP(1,MHEP))
      ELSEIF(NPR.EQ.4) THEN
C Four body decay: LHEP -> KHEP + RHEP + MHEP + NHEP
        KHEP = MHEP
        RHEP = MHEP+1
        MHEP = MHEP+2
        ISTHEP(NHEP) = 114
C Provisional colour connections of KHEP and RHEP
        JMOHEP(2,KHEP)=RHEP
        JDAHEP(2,KHEP)=RHEP
        JMOHEP(2,RHEP)=KHEP
        JDAHEP(2,RHEP)=KHEP
C Four body RPV decay
        IF(NME(IM).EQ.1000) THEN
           CALL HWDRPD(IHEP,KHEP,NHEP)
C Four body phase space decay
        ELSE
           CALL HWDFOR(PHEP(1,IHEP),PHEP(1,KHEP),PHEP(1,RHEP),
     &          PHEP(1,MHEP),PHEP(1,NHEP))
        ENDIF
        CALL HWVEQU(4,VHEP(1,KHEP),VHEP(1,RHEP))
        CALL HWVEQU(4,VHEP(1,KHEP),VHEP(1,MHEP))
      ELSE
        CALL HWWARN('HWDHO2',100)
      ENDIF
      END
