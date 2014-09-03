CDECK  ID>, HWDHO3.
*CMZ :-        -17/10/01  10:19:15  by  Peter Richardson
*-- Author :    Ian Knowles & Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE HWDHO3(ID,IM,NPR,MHEP,LHEP,KHEP,CLSAVE)
C-----------------------------------------------------------------------
C   Subroutine to perform the third part of the heavy object decays
C   IE setup the colour connections
C   was part of HWDHOB
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      INTEGER ID,IM,KHEP,LHEP,MHEP,NPR,CLSAVE(2)
      IF (IERROR.NE.0) RETURN
C Colour connections
      IF (ID.EQ.6.OR.ID.EQ.12.OR.(ID.GE.209.AND.ID.LE.212)
     &                       .OR.(ID.GE.215.AND.ID.LE.218)) THEN
        IF ((NPR.EQ.3.AND.NME(IM).EQ.100).OR.
     &      ((SYSPIN.OR.THREEB).AND.NPR.EQ.3.AND.
     &        NME(IM).GE.10000.AND.NME(IM).LE.20000)) THEN
C usual heavy quark decay
          JMOHEP(2,KHEP)=MHEP
          JDAHEP(2,KHEP)=MHEP
          JMOHEP(2,MHEP)=KHEP
          JDAHEP(2,MHEP)=KHEP
          JMOHEP(2,NHEP)=LHEP
          JDAHEP(2,NHEP)=LHEP
        ELSEIF (ABS(IDHEP(MHEP)).EQ.37) THEN
C heavy quark to charged Higgs 2->2
          JMOHEP(2,MHEP)=MHEP
          JDAHEP(2,MHEP)=MHEP
          JMOHEP(2,NHEP)=LHEP
          JDAHEP(2,NHEP)=LHEP
        ELSEIF (ABS(IDHEP(NHEP)).EQ.37) THEN
C heavy quark to charged Higgs 2->2
          JMOHEP(2,MHEP)=LHEP
          JDAHEP(2,MHEP)=LHEP
          JMOHEP(2,NHEP)=NHEP
          JDAHEP(2,NHEP)=NHEP
        ELSE IF (NPR.EQ.3.AND.NME(IM).EQ.200) THEN
C heavy quark to charged Higgs 2->3
          JMOHEP(2,KHEP)=MHEP
          JDAHEP(2,KHEP)=MHEP
          JMOHEP(2,MHEP)=KHEP
          JDAHEP(2,MHEP)=KHEP
          JMOHEP(2,NHEP)=LHEP
          JDAHEP(2,NHEP)=LHEP
        ELSE
          CALL HWWARN('HWDHO3',100)
          GOTO 999
        ENDIF
      ELSEIF(.NOT.RPARTY.AND.NPR.EQ.4.AND.(ID.EQ.429.OR.ID.EQ.435)) THEN
        JMOHEP(2,KHEP) = KHEP 
        JDAHEP(2,KHEP) = KHEP 
        JMOHEP(2,KHEP+1) = KHEP+2 
        JDAHEP(2,KHEP+1) = KHEP+2 
        JMOHEP(2,KHEP+2) = KHEP+1 
        JDAHEP(2,KHEP+2) = KHEP+1 
        JMOHEP(2,KHEP+3) = KHEP+3 
        JDAHEP(2,KHEP+3) = KHEP+3 
      ELSE
        IF(.NOT.RPARTY.AND.
     &     ((NPR.EQ.2.AND.ID.GE.401.AND.ID.LT.448.AND.
     &         IDHW(MHEP).LE.132.AND.IDHW(NHEP).LE.132)
     &     .OR.(NPR.EQ.3.AND.ID.GE.449.AND.ID.LE.457.AND.
     &         IDHW(MHEP).LE.132.AND.IDHW(NHEP).LE.132.AND.
     &         IDHW(MHEP-1).LE.132))) THEN
C R-parity violating SUSY decays
          IF(NPR.EQ.2) THEN
C--Rparity slepton colour connections
            IF(ID.GE.425.AND.ID.LE.448) THEN
              IF(IDHW(MHEP).GT.12) THEN
                JMOHEP(2,MHEP) = MHEP
                JDAHEP(2,MHEP) = MHEP
                JMOHEP(2,NHEP) = NHEP
                JDAHEP(2,NHEP) = NHEP
              ELSE
                JMOHEP(2,MHEP) = NHEP
                JDAHEP(2,MHEP) = NHEP
                JMOHEP(2,NHEP) = MHEP
                JDAHEP(2,NHEP) = MHEP
              ENDIF
C--Rparity squark colour connections
            ELSE
              IF(IDHEP(LHEP).GT.0) THEN
C--LQD decay colour connections
                IF(IDHW(MHEP).GT.12) THEN
                  JMOHEP(2,MHEP) = MHEP
                  JDAHEP(2,MHEP) = MHEP
                  JMOHEP(2,NHEP) = LHEP
                  JDAHEP(2,NHEP) = LHEP
                ELSE
C--UDD decay colour connections
                  HVFCEN = .TRUE.
                  CALL HWDRCL(LHEP,MHEP,CLSAVE)
                ENDIF
              ELSE
C--Antisquark connections
                IF(IDHW(MHEP).GT.12) THEN
                  JMOHEP(2,MHEP) = MHEP
                  JDAHEP(2,MHEP) = MHEP
                  JMOHEP(2,NHEP) = LHEP
                  JDAHEP(2,NHEP) = LHEP
                ELSE
                  HVFCEN = .TRUE.
                 CALL HWDRCL(LHEP,MHEP,CLSAVE)
                ENDIF
              ENDIF
            ENDIF
          ELSE
            IF(ID.GE.450.AND.ID.LE.457) THEN
C--Rparity Neutralino/Chargino colour connection
              IF(IDHW(MHEP-1).LE.12.AND.IDHW(MHEP).LE.12.
     &               AND.IDHW(NHEP).LE.12) THEN
                HVFCEN = .TRUE.
                CALL HWDRCL(LHEP,MHEP,CLSAVE)
              ELSE
                JMOHEP(2,MHEP) = NHEP
                JDAHEP(2,MHEP) = NHEP
                JMOHEP(2,NHEP) = MHEP
                JDAHEP(2,NHEP) = MHEP
              ENDIF
C--Rparity gluino colour connections
            ELSEIF(ID.EQ.449) THEN
              IF(IDHW(MHEP-1).LE.12.AND.IDHW(MHEP).LE.12.
     &               AND.IDHW(NHEP).LE.12) THEN
                HVFCEN = .TRUE.
                CALL HWDRCL(LHEP,MHEP,CLSAVE)
C--Now the lepton number violating decay
              ELSE
                IF(IDHW(MHEP).LE.6) THEN
                  JMOHEP(2,MHEP) = LHEP
                  JDAHEP(2,MHEP) = NHEP
                  JMOHEP(2,NHEP) = MHEP
                  JDAHEP(2,NHEP) = LHEP
                ELSE
                  JMOHEP(2,MHEP) = NHEP
                  JDAHEP(2,MHEP) = LHEP
                  JMOHEP(2,NHEP) = LHEP
                  JDAHEP(2,NHEP) = MHEP
                ENDIF
              ENDIF
            ELSE
              CALL HWWARN('HWDHO3',101)
              GOTO 999
            ENDIF
          ENDIF
        ELSE
C Normal SUSY decays
          IF (ID.LE.448.AND.ID.GT.207) THEN
C Squark (or slepton)
            IF (IDHW(MHEP).EQ.449) THEN
              IF (IDHEP(LHEP).GT.0) THEN
                JMOHEP(2,MHEP)=LHEP
                JDAHEP(2,MHEP)=NHEP
                JMOHEP(2,NHEP)=MHEP
                JDAHEP(2,NHEP)=LHEP
              ELSE
                JMOHEP(2,MHEP)=NHEP
                JDAHEP(2,MHEP)=LHEP
                JMOHEP(2,NHEP)=LHEP
                JDAHEP(2,NHEP)=MHEP
              ENDIF
            ELSE
              IF(NPR.EQ.3.AND.IDHW(MHEP).LE.12) THEN
                JMOHEP(2,MHEP)=NHEP
                JDAHEP(2,MHEP)=NHEP
                JMOHEP(2,NHEP)=MHEP
                JDAHEP(2,NHEP)=MHEP
              ELSE
                JMOHEP(2,MHEP)=MHEP
                JDAHEP(2,MHEP)=MHEP
                JMOHEP(2,NHEP)=LHEP
                JDAHEP(2,NHEP)=LHEP
              ENDIF
            ENDIF
          ELSEIF (ID.EQ.449) THEN
C Gluino
            IF (IDHW(NHEP).EQ.13) THEN
              JMOHEP(2,MHEP)=MHEP
              JDAHEP(2,MHEP)=MHEP
              JMOHEP(2,NHEP)=LHEP
              JDAHEP(2,NHEP)=LHEP
            ELSEIF (IDHEP(MHEP).GT.0) THEN
              JMOHEP(2,MHEP)=LHEP
              JDAHEP(2,MHEP)=NHEP
              JMOHEP(2,NHEP)=MHEP
              JDAHEP(2,NHEP)=LHEP
            ELSE
              JMOHEP(2,MHEP)=NHEP
              JDAHEP(2,MHEP)=LHEP
              JMOHEP(2,NHEP)=LHEP
              JDAHEP(2,NHEP)=MHEP
            ENDIF
          ELSE
C Gaugino or Higgs
            JMOHEP(2,MHEP)=NHEP
            JDAHEP(2,MHEP)=NHEP
            JMOHEP(2,NHEP)=MHEP
            JDAHEP(2,NHEP)=MHEP
          ENDIF
        ENDIF
      ENDIF
 999  RETURN
      END
