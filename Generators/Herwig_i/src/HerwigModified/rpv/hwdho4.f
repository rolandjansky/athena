CDECK  ID>, HWDHO4.
*CMZ :-        -30/09/02  14:05:28  by  Peter Richardson
*-- Author :    Ian Knowles & Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE HWDHO4(IHEP,ID,IM,NPR,MHEP,LHEP,KHEP,PW)
C-----------------------------------------------------------------------
C   Subroutine to perform the fourth part of the heavy object decays
C   IE parton-showers with special treatment for top
C   was part of HWDHOB
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      DOUBLE PRECISION PW(5),PDW(5,3),PDT(5,4),vtau(5)
      INTEGER IHEP,ID,IM,I,KHEP,LHEP,MHEP,NPR,NTRY,WHEP,SHEP,CLSAVE(2)
      DOUBLE COMPLEX RHOIN(2,2)
      SAVE RHOIN
      DATA RHOIN/(0.5D0,0.0D0),(0.0D0,0.0D0),
     &           (0.0D0,0.0D0),(0.5D0,0.0D0)/
      IF (IERROR.NE.0) RETURN
      SHEP = NHEP
C---SPECIAL CASE FOR THREE-BODY TOP DECAYS:
C   RELABEL THEM AS TWO TWO-BODY DECAYS FOR PARTON SHOWERING
      IF ((ID.EQ.6.OR.ID.EQ.12).AND.NPR.EQ.3.AND.
     &     (NME(IM).EQ.100.OR.NME(IM).EQ.200.OR.
     &     (NME(IM).GT.10000.AND.NME(IM).LE.20000.AND.
     &     (SYSPIN.OR.THREEB)))) THEN
C---STORE W/H DECAY PRODUCTS
        CALL HWVEQU(10,PHEP(1,KHEP),PDW)
C---BOOST THEM INTO W/H REST FRAME
        CALL HWULOF(PW,PDW(1,1),PDW(1,3))
C---REPLACE THEM BY W/H
        CALL HWVEQU(5,PW,PHEP(1,KHEP))
        WHEP=KHEP
        IF (NME(IM).EQ.100.OR.(NME(IM).GT.10000.AND.
     &      NME(IM).LE.20000.AND.(SYSPIN.OR.THREEB)))IDHW(KHEP)=198
        IF((NME(IM).EQ.100.OR.(NME(IM).GT.10000.AND.
     &      NME(IM).LE.20000.AND.(SYSPIN.OR.THREEB))).AND.(ID.EQ.12))
     &       IDHW(KHEP)=199
        IF (NME(IM).EQ.200)IDHW(KHEP)=206
        IF((NME(IM).EQ.200).AND.(ID.EQ.12))IDHW(KHEP)=207
        IDHEP(KHEP)=IDPDG(IDHW(KHEP))
        JMOHEP(2,KHEP)=KHEP
        JDAHEP(2,KHEP)=KHEP
        CALL HWVEQU(4,VHEP(1,NHEP),VHEP(1,KHEP))
C---AND MOVE B UP
        CALL HWVEQU(5,PHEP(1,NHEP),PHEP(1,MHEP))
        IDHW(MHEP)=IDHW(NHEP)
        IDHEP(MHEP)=IDHEP(NHEP)
        JDAHEP(2,LHEP)=MHEP
        JMOHEP(2,MHEP)=JMOHEP(2,NHEP)
        JDAHEP(2,MHEP)=JDAHEP(2,NHEP)
        CALL HWVEQU(4,VHEP(1,NHEP),VHEP(1,MHEP))
        NHEP=MHEP
C---DO PARTON SHOWER
        EMSCA=PHEP(5,IHEP)
        CALL HWBGEN
        IF (IERROR.NE.0) RETURN
C---FIND BOOSTED W/H MOMENTUM
        NTRY=0
 41     NTRY=NTRY+1
        IF (NTRY.GT.NHEP.OR.WHEP.LE.0.OR.WHEP.GT.NHEP) THEN
          CALL HWWARN('HWDHO4',100)
          GOTO 999
        ENDIF
        WHEP=JDAHEP(1,WHEP)
        IF (ISTHEP(WHEP).NE.190) GOTO 41
C---AND HENCE ITS CHILDRENS MOMENTA
        CALL HWULOB(PHEP(1,WHEP),PDW(1,3),PHEP(1,NHEP+1))
        CALL HWVDIF(4,PHEP(1,WHEP),PHEP(1,NHEP+1),PHEP(1,NHEP+2))
        PHEP(5,NHEP+2)=PDW(5,2)
C---LABEL THEM
        ISTHEP(WHEP)=195
        DO 51 I=1,2
          IDHW(NHEP+I)=IDKPRD(I,IM)
          IDHEP(NHEP+I)=IDPDG(IDHW(NHEP+I))
          ISTHEP(NHEP+I)=112+I
          JDAHEP(I,WHEP)=NHEP+I
          JMOHEP(1,NHEP+I)=WHEP
          JMOHEP(2,NHEP+I)=NHEP+3-I
          JDAHEP(2,NHEP+I)=NHEP+3-I
 51     CONTINUE
        NHEP=NHEP+2
C---ASSIGN PRODUCTION VERTICES TO 1 AND 2
        IF(NME(IM).EQ.100)CALL HWUDKL(198,PW,VHEP(1,NHEP))
        IF(NME(IM).EQ.200)CALL HWUDKL(206,PW,VHEP(1,NHEP))
        CALL HWVSUM(4,VHEP(1,WHEP),VHEP(1,NHEP),VHEP(1,NHEP))
        CALL HWVEQU(4,VHEP(1,NHEP),VHEP(1,NHEP-1))
C---DO PARTON SHOWERS
        EMSCA=PW(5)
C--modification to use photos in top decays
        IF(ITOPRD.EQ.1) CALL HWPHTP(WHEP)
C--end of modification
        CALL HWBGEN
        IF (IERROR.NE.0) RETURN
C--special for four body tau decays
      ELSEIF((ID.EQ.429.OR.ID.EQ.435).AND.NPR.EQ.4) THEN
C--copy the momenta and id's of the decay products
         CALL HWVZRO(5,PDT(1,4))
         DO I=1,3
            CALL HWVEQU(5,PHEP(1,KHEP-1+I),PDT(1,I))
            CALL HWVSUM(4,PHEP(1,KHEP-1+I),PDT(1,4),PDT(1,4))
         ENDDO
         CALL HWUMAS(PDT(1,4))
C--boost them into the rest frame of the gaugino
         DO I=1,3
            CALL HWULOF(PDT(1,4),PDT(1,I),PDT(1,I))
         ENDDO
C--replace them with the neutralino/chargino
         CALL HWVEQU(5,PDT(1,4),PHEP(1,KHEP))
         WHEP=KHEP
         IF(ID.EQ.429.AND.IDHW(NHEP).EQ.126) THEN
            IDHW(KHEP) = 456
         ELSEIF(ID.EQ.435.AND.IDHW(NHEP).EQ.132) THEN
            IDHW(KHEP) = 454
         ELSE
            IDHW(KHEP) = 450
         ENDIF
         IDHEP(KHEP)=IDPDG(IDHW(KHEP))
         JMOHEP(2,KHEP)=KHEP
         JDAHEP(2,KHEP)=KHEP
         CALL HWVEQU(4,VHEP(1,NHEP),VHEP(1,KHEP))
c--move tau/nu_tau up
         CALL HWVEQU(5,PHEP(1,NHEP),PHEP(1,KHEP+1))
         IDHW(KHEP+1)=IDHW(NHEP)
         IDHEP(KHEP+1)=IDHEP(NHEP)
         JDAHEP(2,KHEP+1)=KHEP+1
         JMOHEP(2,KHEP+1)=KHEP+1
         CALL HWVEQU(4,VHEP(1,NHEP),VHEP(1,KHEP+1))
         NHEP=KHEP+1
         JDAHEP(2,LHEP)=KHEP+1
         ISTHEP(LHEP+1)=113
         ISTHEP(LHEP+2)=114
C--do the parton shower (does nothing but relabels status codes)
         EMSCA=PHEP(5,IHEP)
         CALL HWBGEN
         IF (IERROR.NE.0) RETURN
C--find the boosted gaugino
         NTRY=0
 52      NTRY=NTRY+1
         IF (NTRY.GT.NHEP.OR.WHEP.LE.0.OR.WHEP.GT.NHEP) THEN
            CALL HWWARN('HWDHO4',101)
            GOTO 999
         ENDIF
         WHEP=JDAHEP(1,WHEP)
         IF (ISTHEP(WHEP).NE.190) GOTO 52
C--copy of particle for RPV colour code
         NHEP = NHEP+1
         ISTHEP(NHEP) = 3
         IDHW(NHEP) = 20
         IDHEP(NHEP) = 0
         CALL HWVEQU(5,PHEP(1,WHEP),PHEP(1,NHEP))
         CALL HWVEQU(4,VHEP(1,WHEP),VHEP(1,NHEP))
         JMOHEP(1,NHEP)=JMOHEP(1,WHEP)
         JMOHEP(2,NHEP)=JMOHEP(2,WHEP)
         JDAHEP(1,NHEP)=JDAHEP(1,WHEP)
         JDAHEP(2,NHEP)=JDAHEP(2,WHEP)
C--copy of gaugino
         NHEP=NHEP+1
         ISTHEP(NHEP)=155
         IDHW(NHEP)=IDHW(WHEP)
         IDHEP(NHEP)=IDHEP(WHEP)
         CALL HWVEQU(5,PHEP(1,WHEP),PHEP(1,NHEP))
         CALL HWVEQU(4,VHEP(1,WHEP),VHEP(1,NHEP))
         JMOHEP(1,NHEP)=JMOHEP(1,WHEP)
         JMOHEP(2,NHEP)=JMOHEP(2,WHEP)
C Relabel original track
         IF (ISTHEP(WHEP).NE.120) ISTHEP(WHEP)=3
         JMOHEP(2,WHEP)=JMOHEP(1,WHEP)
         JDAHEP(1,WHEP)=NHEP
         JDAHEP(2,WHEP)=NHEP
         WHEP=NHEP
C---children of the gaugino
         CALL HWUDKL(IDHW(WHEP),PDT(1,4),VTAU)
         CALL HWVSUM(4,VHEP(1,WHEP),VTAU,VTAU)
         DO I=1,3
C--momenta boost back to new frame
            CALL HWULOB(PHEP(1,WHEP),PDT(1,I),PDT(1,I))
            CALL HWVEQU(5,PDT(1,I),PHEP(1,NHEP+I))
            CALL HWVEQU(4,VTAU,VHEP(1,NHEP+I))
            IDHW(NHEP+I)=IDKPRD(I,IM)
            IDHEP(NHEP+I)=IDPDG(IDHW(NHEP+I))
            ISTHEP(NHEP+I)=114
            JMOHEP(1,NHEP+I)=WHEP
         ENDDO
         ISTHEP(NHEP+1)=113
         JDAHEP(1,WHEP)=NHEP+1
         JDAHEP(2,WHEP)=NHEP+3
C--MAKE THE COLOUR CONNECTIONS
         IF(IDHW(NHEP+1).LE.12.AND.IDHW(NHEP+2).LE.12.
     &        AND.IDHW(NHEP+3).LE.12) THEN
            HVFCEN = .TRUE.
            CALL HWDRCL(WHEP,NHEP+2,CLSAVE)
         ELSE
            JMOHEP(2,NHEP+1) = NHEP+1
            JDAHEP(2,NHEP+1) = NHEP+1
            JMOHEP(2,NHEP+2) = NHEP+3
            JDAHEP(2,NHEP+2) = NHEP+3
            JMOHEP(2,NHEP+3) = NHEP+2
            JDAHEP(2,NHEP+3) = NHEP+2
         ENDIF
         NHEP=NHEP+3
C---DO PARTON SHOWERS
         EMSCA=PDT(5,4)
         CALL HWBGEN
         IF (IERROR.NE.0) RETURN     
      ELSE
C Do parton showers
        EMSCA=PHEP(5,IHEP)
        CALL HWBGEN
        IF (IERROR.NE.0) RETURN
C--special for gauge boson decay modes of gauginos and four body higgs
C--call routine to add decay products and generate parton shower
        IF(NME(IM).GT.20000.AND.NME(IM).LT.30000) THEN
          CALL HWDSM3(-1,IHEP,MHEP,SHEP,0,NME(IM)-20000,RHOIN,
     &       ISNHEP(IHEP))
        ELSEIF(NME(IM).GT.40000.AND.NME(IM).LT.50000) THEN
          CALL HWDSM4(2,IHEP,MHEP,SHEP,NME(IM)-40000)
        ENDIF
        IF (IERROR.NE.0) RETURN
      ENDIF
 999  RETURN
      END
