*
*
*
*

* Author : Saclay Muon Software Group (SaMuSoG)
***************************************************************************
      SUBROUTINE RBMAGL(LUN,I1MAG,I2MAG,I1FEL,I2FEL,AVER,
     +NVERSION,BINNER,RMAINN,ZMAINN,RTORI,ZTORI,RTORO,ZTORO,EPS0,
     +SECMAG,NRMAG,NFMAG,NZMAG,RMAG ,FMAG ,ZMAG ,BRMAG,BFMAG,BZMAG,
     +SECFEL,NRR  ,NFF  ,NZZ  ,RRPOS,FFPOS,ZZPOS,TABBR,TABBF,TABBZ)
*
*>> Reads general tabulated 3D ATLAS mag. field
*>> from logical unit LUN !
*>> Author : Saclay Muon Software Group (SaMuSoG)
*>> Date   : 28/07/98
*
      IMPLICIT NONE
*
*
      CHARACTER*4 AVER
      INTEGER LUN, I1MAG,I2MAG, I1FEL,I2FEL, NVERSION
      INTEGER NRMAG(*),NFMAG,NZMAG, NRR,NFF,NZZ
      REAL    BINNER, RMAINN, ZMAINN, RTORI, ZTORI, RTORO, ZTORO, EPS0
      REAL    SECFEL, SECMAG, RMAG(*),FMAG(*),ZMAG(*) 
      REAL    BRMAG(*),BFMAG(*),BZMAG(*), RRPOS(*),FFPOS(*),ZZPOS(*)
      REAL    TABBR(*),TABBF(*),TABBZ(*)
*
      INTEGER NSUBVER
      CHARACTER*80 dtbname
      LOGICAL FIRST
      DATA FIRST/.TRUE./
*
      IF( FIRST ) THEN
*****        FIRST = .FALSE.
        write(6,*)" attention temp should be revisted"
        IF(AVER.NE."iron") THEN
          READ(LUN,4000) dtbname(1:7),NSUBVER       
4000      FORMAT(A7,I1)
          AVER(1:2) = dtbname(6:7)
          REWIND LUN
        ENDIF
      ENDIF
*
      IF( AVER(1:1).EQ.'O' .OR. AVER(1:1).EQ.'P' .OR.
     +    AVER(1:1).EQ.'Q' .OR. AVER(1:1).EQ.'R' .OR.
     +    AVER(1:1).EQ.'X'                      ) THEN
        CALL REAMAG(LUN,I1MAG,I2MAG,
     +   SECMAG,NRMAG,NFMAG,NZMAG,RMAG ,FMAG ,ZMAG ,BRMAG,BFMAG,BZMAG)
        CALL RFELIX(LUN,I1FEL,I2FEL,
     +   NVERSION,BINNER,RMAINN,ZMAINN,RTORI,ZTORI,RTORO,ZTORO,EPS0,
     +   SECFEL,NRR  ,NFF  ,NZZ  ,RRPOS,FFPOS,ZZPOS,TABBR,TABBF,TABBZ)
      ELSEIF( AVER(1:1).EQ.'M') THEN
        CALL REAMAG(LUN,I1MAG,I2MAG,
     +   SECMAG,NRMAG,NFMAG,NZMAG,RMAG ,FMAG ,ZMAG ,BRMAG,BFMAG,BZMAG)
        NRR = 0
        NFF = 0
        NZZ = 0
      ELSEIF( dtbname(1:2).EQ.'IC' .OR. AVER(1:4).EQ.'iron' ) THEN 
        CALL RFELIX(LUN,I1FEL,I2FEL,
     +   NVERSION,BINNER,RMAINN,ZMAINN,RTORI,ZTORI,RTORO,ZTORO,EPS0,
     +   SECFEL,NRR  ,NFF  ,NZZ  ,RRPOS,FFPOS,ZZPOS,TABBR,TABBF,TABBZ)
        NFMAG = 0
        NZMAG = 0
      ELSE
        PRINT 1000,dtbname(1:7)
1000    FORMAT(/'  Mag. field file of type ',A7,' cannot be read'
     +         ,' by routine REAMAG  ======>  STOP !')
        STOP
      ENDIF
*
      IF( I2FEL.EQ.1 ) THEN
        WRITE (6,2000) RTORI,ZTORI,RTORO,ZTORO
2000    FORMAT(/' ===> Tabulated 3D mag. field limits set to :'
     +         /' ===> RTORI,ZTORI,RTORO,ZTORO =',4F8.1,' (cm)'/)
      ENDIF
*
*
      RETURN
      END
