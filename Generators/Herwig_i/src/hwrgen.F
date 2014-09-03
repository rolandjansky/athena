      function HWRGEN(IDUMMY)
      DOUBLE PRECISION HWRGEN, ATL_HWRGEN, HWRSET, HWRGET
      INTEGER IDUMMY,ISEED(2),JSEED(2)
      HWRGEN=ATL_HWRGEN(IDUMMY)
      RETURN
      ENTRY HWRSET(JSEED)      
C-----------------------------------------------------------------------
C     IF (JSEED(1).EQ.0.OR.JSEED(2).EQ.0) CALL HWWARN('HWRSET',99)
      ISEED(1)=JSEED(1)
      ISEED(2)=JSEED(2)
      HWRSET=0.0D0
      RETURN
C-----------------------------------------------------------------------
      ENTRY HWRGET(JSEED)
C-----------------------------------------------------------------------
      JSEED(1)=ISEED(1)
      JSEED(2)=ISEED(2)
      HWRGET=0.0D0
      RETURN
      END
