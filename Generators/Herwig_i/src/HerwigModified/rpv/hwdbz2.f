CDECK  ID>, HWDBZ2.
*CMZ :-        -02/04/01  12.11.55  by  Peter Richardson
*-- Author :    Peter Richardson based on Mike Seymour's HWDBOZ
C-----------------------------------------------------------------------
      SUBROUTINE HWDBZ2(IDBOS,IFER,IANT,CV,CA,BR,IOPT,MASS)
C-----------------------------------------------------------------------
C     CHOOSE DECAY MODE OF BOSON
C     IOPT=2 TO RESET COUNTERS, 1 FOR BOSON PAIR, 0 FOR ANY OTHERS
C     IDENTICAL TO HWDBOZ BUT REQUIRES DECAY MODE ACCESSIBLE FOR GIVEN
C     MASS
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      DOUBLE PRECISION HWRGEN,BRMODE(12,3),CV,CA,BR,BRLST,BRCOM,FACZ,
     & FACW,MSMODE(12,3),MASS
      INTEGER HWRINT,IDBOS,IDEC,IDMODE(2,12,3),IFER,IANT,IOPT,I1,I2,
     & I1LST,I2LST,NWGLST,NUMDEC,NPAIR,MODTMP,JFER,NTRY
      LOGICAL GENLST
      EXTERNAL HWRGEN,HWRINT
      SAVE FACW,FACZ,MSMODE,NWGLST,GENLST,NUMDEC,NPAIR,I1LST,I2LST,BRLST
      SAVE IDMODE,BRMODE
      DATA NWGLST,GENLST,NPAIR/-1,.FALSE.,0/
C---STORE THE DECAY MODES (FERMION FIRST)
      DATA IDMODE/  2,  7,  4,  9,  6, 11,  2,  9,  4,  7,
     &            122,127,124,129,126,131,8*0,
     &              1,  8,  3, 10,  5, 12,  3,  8,  1, 10,
     &            121,128,123,130,125,132,8*0,
     &              1,  7,  2,  8,  3,  9,  4, 10,  5, 11,  6, 12,
     &            121,127,123,129,125,131,122,128,124,130,126,132/
C---STORE THE BRANCHING RATIOS TO THESE MODES
      DATA BRMODE/0.321D0,0.321D0,0.000D0,0.017D0,0.017D0,0.108D0,
     &            0.108D0,0.108D0,4*0.0D0,
     &            0.321D0,0.321D0,0.000D0,0.017D0,0.017D0,0.108D0,
     &            0.108D0,0.108D0,4*0.0D0,
     &            0.154D0,0.120D0,0.154D0,0.120D0,0.152D0,0.000D0,
     &            0.033D0,0.033D0,0.033D0,0.067D0,0.067D0,0.067D0/
      DATA MSMODE/36*0.0D0/
C---FACTORS FOR CV AND CA FOR W AND Z
      DATA FACW,FACZ/2*0.0D0/
      IF (FACZ.EQ.ZERO) FACZ=SQRT(SWEIN)
      IF (FACW.EQ.ZERO) FACW=0.5/SQRT(2D0)
      IF (IDBOS.LT.198.OR.IDBOS.GT.200) THEN
        CALL HWWARN('HWDBZ2',101)
        GOTO 999
      ENDIF
      IF(MSMODE(1,1).EQ.ZERO) THEN
        DO I1=1,12
          DO I2=1,3
            MSMODE(I1,I2)=RMASS(IDMODE(1,I1,I2))+RMASS(IDMODE(2,I1,I2))
          ENDDO
        ENDDO
      ENDIF
C---IF THIS IS A NEW EVENT SINCE LAST TIME, ZERO COUNTERS
      IF (NWGTS.NE.NWGLST .OR.(GENEV.NEQV.GENLST).OR. IOPT.EQ.2) THEN
        NPAIR=0
        NUMDEC=0
        NWGLST=NWGTS
        GENLST=GENEV
        IF (IOPT.EQ.2) RETURN
      ENDIF
      NUMDEC=NUMDEC+1
      IF (NUMDEC.GT.MODMAX) THEN
        CALL HWWARN('HWDBZ2',102)
        GOTO 999
      ENDIF
C---IF PAIR OPTION SPECIFIED FOR THE FIRST TIME, MAKE CHOICE
      IF (IOPT.EQ.1) THEN
        IF (NUMDEC.GT.MODMAX-1) THEN
          CALL HWWARN('HWDBZ2',103)
          GOTO 999
        ENDIF
        IF (NPAIR.EQ.0) THEN
          IF (HWRGEN(1).GT.HALF) THEN
            MODTMP=MODBOS(NUMDEC+1)
            MODBOS(NUMDEC+1)=MODBOS(NUMDEC)
            MODBOS(NUMDEC)=MODTMP
          ENDIF
          NPAIR=NUMDEC
        ELSE
          NPAIR=0
        ENDIF
      ENDIF
C---SELECT USER'S CHOICE
      IF (IDBOS.EQ.200) THEN
        IF (MODBOS(NUMDEC).EQ.1) THEN
          I1=1
          I2=6
        ELSEIF (MODBOS(NUMDEC).EQ.2) THEN
          I1=7
          I2=7
        ELSEIF (MODBOS(NUMDEC).EQ.3) THEN
          I1=8
          I2=8
        ELSEIF (MODBOS(NUMDEC).EQ.4) THEN
          I1=9
          I2=9
        ELSEIF (MODBOS(NUMDEC).EQ.5) THEN
          I1=7
          I2=8
        ELSEIF (MODBOS(NUMDEC).EQ.6) THEN
          I1=10
          I2=12
        ELSEIF (MODBOS(NUMDEC).EQ.7) THEN
          I1=5
          I2=5
        ELSE
          I1=1
          I2=12
        ENDIF
      ELSE
        IF (MODBOS(NUMDEC).EQ.1) THEN
          I1=1
          I2=5
        ELSEIF (MODBOS(NUMDEC).EQ.2) THEN
          I1=6
          I2=6
        ELSEIF (MODBOS(NUMDEC).EQ.3) THEN
          I1=7
          I2=7
        ELSEIF (MODBOS(NUMDEC).EQ.4) THEN
          I1=8
          I2=8
        ELSEIF (MODBOS(NUMDEC).EQ.5) THEN
          I1=6
          I2=7
        ELSE
          I1=1
          I2=8
        ENDIF
      ENDIF
      NTRY = 0
 10   IDEC=HWRINT(I1,I2)
      NTRY = NTRY+1
      IF (HWRGEN(0).GT.BRMODE(IDEC,IDBOS-197).AND.I1.NE.I2) GOTO 10
      IF(MASS.LT.MSMODE(IDEC,IDBOS-197).AND.NTRY.LT.NBTRY) GOTO 10
      IF(NTRY.GE.NBTRY) THEN
        BR = ZERO
        RETURN
      ENDIF
      IFER=IDMODE(1,IDEC,IDBOS-197)
      IANT=IDMODE(2,IDEC,IDBOS-197)
C---CALCULATE BRANCHING RATIO
C   (RESULT IS NOT WELL-DEFINED AFTER THE FIRST CALL OF A PAIR)
      BR=0
      DO 20 IDEC=I1,I2
 20     IF(MSMODE(IDEC,IDBOS-197).LT.MASS) BR=BR+BRMODE(IDEC,IDBOS-197)
      IF (IOPT.EQ.1) THEN
        IF (NPAIR.NE.0) THEN
          I1LST=I1
          I2LST=I2
          BRLST=BR
        ELSE
          BRCOM=0
          DO 30 IDEC=MAX(I1,I1LST),MIN(I2,I2LST)
 30         IF(MSMODE(IDEC,IDBOS-197).LT.MASS)
     &            BRCOM=BRCOM+BRMODE(IDEC,IDBOS-197)
          BR=2*BR*BRLST - BRCOM**2
        ENDIF
      ENDIF
C---SET UP VECTOR AND AXIAL VECTOR COUPLINGS (NORMALIZED TO THE
C   CONVENTION WHERE THE WEAK CURRENT IS G*(CV-CA*GAM5) )
      IF (IDBOS.EQ.200) THEN
        IF (IFER.LE.6) THEN
C Quark couplings
           CV=VFCH(IFER,1)
           CA=AFCH(IFER,1)
        ELSE
C lepton couplings
           JFER=IFER-110
           CV=VFCH(JFER,1)
           CA=AFCH(JFER,1)
        ENDIF
        CV=CV * FACZ
        CA=CA * FACZ
      ELSE
        CV=FACW
        CA=FACW
      ENDIF
 999  RETURN
      END
