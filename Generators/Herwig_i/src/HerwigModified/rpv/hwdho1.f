CDECK  ID>, HWDHO1.
*CMZ :-        -17/10/01  10:19:15  by  Peter Richardson
*-- Author :    Ian Knowles & Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE HWDHO1(IHEP,ID,IM,NPR,LHEP,MHEP)
C-----------------------------------------------------------------------
C   Subroutine to perform the first part of the heavy object decays
C   IE to select the decay mode
C   was part of HWDHOB
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      DOUBLE PRECISION HWUMBW,HWRGEN,SDKM,RN,BF
      INTEGER IST(4),IHEP,ID,IM,I,JHEP,LHEP,MHEP,NPR,MTRY,NTRY,IS
      EXTERNAL HWRGEN
      SAVE IST
      DATA IST/113,114,114,114/
      IF (IERROR.NE.0) RETURN
      IF(.NOT.RPARTY) THEN
        NHEP = NHEP+1
        ISTHEP(NHEP) = 3
        IDHW(NHEP) = 20
        IDHEP(NHEP) = 0
        CALL HWVEQU(5,PHEP(1,IHEP),PHEP(1,NHEP))
        CALL HWVEQU(4,VHEP(1,IHEP),VHEP(1,NHEP))
        JMOHEP(1,NHEP)=JMOHEP(1,IHEP)
        JMOHEP(2,NHEP)=JMOHEP(2,IHEP)
        JDAHEP(1,NHEP)=JDAHEP(1,IHEP)
        JDAHEP(2,NHEP)=JDAHEP(2,IHEP)
      ENDIF
C Make a copy of decaying object
      NHEP=NHEP+1
      ISTHEP(NHEP)=155
      IDHW(NHEP)=IDHW(IHEP)
      IDHEP(NHEP)=IDHEP(IHEP)
      CALL HWVEQU(5,PHEP(1,IHEP),PHEP(1,NHEP))
      CALL HWVEQU(4,VHEP(1,IHEP),VHEP(1,NHEP))
      JMOHEP(1,NHEP)=JMOHEP(1,IHEP)
      JMOHEP(2,NHEP)=JMOHEP(2,IHEP)
C--copy the location of the particle in the spin block
      IF(SYSPIN.AND.NSPN.NE.0) THEN
         IF(ISNHEP(IHEP).EQ.0) THEN
           IS = IHEP
           MTRY = 0
 5         MTRY = MTRY+1
           IS = JMOHEP(1,IS)
           IF(ISNHEP(IS).EQ.0.AND.MTRY.LE.NETRY) GOTO 5
           IF(MTRY.GT.NETRY) THEN
             CALL HWWARN('HWDHO1',102)
             GOTO 999
           ENDIF
           ISNHEP(IHEP) = ISNHEP(IS)
         ENDIF
         ISNHEP(NHEP) = ISNHEP(JMOHEP(1,NHEP))
      ENDIF
      MTRY=0
 15   MTRY=MTRY+1
C Select decay mode
      RN=HWRGEN(0)
      BF=0.
      IM=LSTRT(ID)
      DO 20 I=1,NMODES(ID)
      BF=BF+BRFRAC(IM)
      IF (BF.GE.RN) GOTO 30
  20  IM=LNEXT(IM)
      CALL HWWARN('HWDHO1',50)
  30  IF (NHEP+5.GT.NMXHEP) THEN
        CALL HWWARN('HWDHO1',100)
        GOTO 999
      ENDIF
      NPR=NPRODS(IM)
      JDAHEP(1,NHEP)=NHEP+1
      JDAHEP(2,NHEP)=NHEP+NPR
C Reset colour pointers (if set)
      JHEP=JMOHEP(2,IHEP)
      IF (JHEP.GT.0) THEN
        IF (JDAHEP(2,JHEP).EQ.IHEP) JDAHEP(2,JHEP)=NHEP
        IF(.NOT.RPARTY.AND.ISTHEP(JHEP).EQ.155
     &    .AND.ABS(IDHEP(JHEP)).GT.1000000
     &    .AND.JDAHEP(2,JHEP-1).EQ.IHEP) JDAHEP(2,JHEP-1) = NHEP
      ENDIF
      JHEP=JDAHEP(2,IHEP)
      IF (JHEP.GT.0) THEN
        IF (JMOHEP(2,JHEP).EQ.IHEP) JMOHEP(2,JHEP)=NHEP
        IF(.NOT.RPARTY.AND.ISTHEP(JHEP).EQ.155
     &    .AND.ABS(IDHEP(JHEP)).GT.1000000
     &    .AND.JMOHEP(2,JHEP-1).EQ.IHEP) JMOHEP(2,JHEP-1) = NHEP
      ENDIF
C--Reset colour pointers if baryon number violated
      IF(.NOT.RPARTY) THEN
        DO JHEP=1,NHEP
          IF(ISTHEP(JHEP).EQ.155
     &       .AND.ABS(IDHEP(JHEP)).GT.1000000.AND.
     &       JDAHEP(2,JHEP-1).EQ.IHEP) JDAHEP(2,JHEP-1)= NHEP
          IF(JDAHEP(2,JHEP).EQ.IHEP) JDAHEP(2,JHEP)=NHEP
          IF(JMOHEP(2,JHEP).EQ.IHEP) JMOHEP(2,JHEP)=NHEP
        ENDDO
        IF(HRDCOL(1,1).EQ.IHEP) HRDCOL(1,1)=NHEP
      ENDIF
C Relabel original track
      IF (ISTHEP(IHEP).NE.120) ISTHEP(IHEP)=3
      JMOHEP(2,IHEP)=JMOHEP(1,IHEP)
      JDAHEP(1,IHEP)=NHEP
      JDAHEP(2,IHEP)=NHEP
C Label decay products and choose masses
      LHEP=NHEP
      MHEP=LHEP+1
      NTRY=0
 35   NTRY=NTRY+1
      SDKM=PHEP(5,NHEP)
      DO 40 I=1,NPR
      NHEP=NHEP+1
      IDHW(NHEP)=IDKPRD(I,IM)
      IDHEP(NHEP)=IDPDG(IDKPRD(I,IM))
      ISTHEP(NHEP)=IST(I)
      JMOHEP(1,NHEP)=LHEP
      JDAHEP(1,NHEP)=0
      PHEP(5,NHEP)=HWUMBW(IDKPRD(I,IM))
 40   SDKM=SDKM-PHEP(5,NHEP)
      IF (SDKM.LT.ZERO) THEN
        NHEP=NHEP-NPR
        IF (NTRY.LE.NETRY) GO TO 35
        CALL HWWARN('HWDHO1',1)
        IF (MTRY.LE.NETRY) GO TO 15
        CALL HWWARN('HWDHO1',101)
        GOTO 999
      ENDIF
C Assign production vertices to decay products
      CALL HWUDKL(ID,PHEP(1,IHEP),VHEP(1,MHEP))
      CALL HWVSUM(4,VHEP(1,IHEP),VHEP(1,MHEP),VHEP(1,MHEP))
      CALL HWVEQU(4,VHEP(1,MHEP),VHEP(1,NHEP))
 999  RETURN
      END
