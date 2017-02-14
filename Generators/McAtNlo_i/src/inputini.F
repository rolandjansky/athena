      SUBROUTINE INPUTINI
#include "HERWIG65.INC"
      
      INTEGER I,JPR0,JPR
C QQIN IS THE EVENT FILE 
C String length increased from 50 to 140 chars (CG -- 25.11.09)
      CHARACTER*140 QQIN
      COMMON/VVJIN/QQIN
      REAL*8 TMPLAM,GAMW0,GAMZ0,VGAMMAX,V2GAMMAX
      REAL*8 RGAMMAX
      INTEGER IPDF

      CHARACTER*60 TMPSTR2
      LOGICAL OLDFORM
      DATA OLDFORM/.FALSE./

c
C Open the file with the input parameters
      OPEN (21,FILE='inparmMcAtNlo.dat',STATUS='OLD',ERR=100)
C       WRITE(*,*)'Enter filename for events'
      READ(21,*)QQIN
C       WRITE(*,*)'Enter maximum number of events to generate'
C       WRITE(*,*)'MUST coincide with the number of events on tape'
      READ(21,*)MAXEV
C       WRITE(*,*)'Enter 0 to use Herwig default PDFs'
C       WRITE(*,*)'      1 to use PDFs from library'
      READ(21,*)IPDF
C OUTPUT THE RESULTS AFTER ANY NSTEP EVENTS
C      NSTEP=20000
C---BEAM paRTICLES
C       WRITE(*,*)'Enter colliding particles (PART1, PART2)'
      READ(21,*)PART1,PART2
C---BEAM MOMENTA
C       WRITE(*,*)'Enter beam momenta (PBEAM1, PBEAM2)'
      READ(21,*)PBEAM1,PBEAM2
C---PROCESS
C       WRITE(*,*)'Enter process number (IPROC) (IPROC<0 for Les Houches)'
      READ(21,*)IPROC
C---INITIALISE OTHER COMMON BLOCKS
C      CALL HWIGIN
C---USER CAN RESET PARAMETERS AT
C   THIS POINT, OTHERWISE DEFAULT
C   VALUES IN HWIGIN WILL BE USED.
C
C LHSOFT=.FALSE. SWITCHES UNDERLYING EVENT OFF
C       LHSOFT=IPROC.GT.-10000
C C       IF(IPROC.LT.-10000) IPROC = IPROC + 10000
C       WRITE(*,*)
C       IF (LHSOFT) THEN
C         WRITE(*,*)'Underlying event WILL be generated'
C       ELSE
C          WRITE(*,*)'Underlying event WILL NOT be generated'
C       ENDIF
C       WRITE(*,*)
C
C Select W/Z boson decay modes
C      MODBOS(1)=5
C      MODBOS(2)=5
C
      IF(IPDF.EQ.1)THEN
         DO I=1,2
C            WRITE(*,*)'   Incoming particle # ',I
C            WRITE(*,*)'Enter PDF group name (AUTPDF)'
            READ(21,*)AUTPDF(I)
C            WRITE(*,*)'Enter PDF set number (MODPDF)'
            READ(21,*)MODPDF(I)
         ENDDO
      ENDIF
C      WRITE(*,*)'Enter Lambda_QCD, <0 for Herwig default'
      READ(21,*)TMPLAM
      IF(TMPLAM.GE.0.D0)QCDLAM=TMPLAM
C---CHECK PROCESS CODE
      JPR0=MOD(ABS(IPROC),10000)
      JPR=JPR0/100
      IF (JPR.EQ.13.OR.JPR.EQ.14) THEN
         IF(JPR0.EQ.1396)THEN
C           WRITE(*,*)'Enter M_GAMMA*(INF), M_GAMMA*(SUP)'
           READ(21,*)EMMIN,EMMAX
         ELSEIF(JPR0.EQ.1397)THEN
C           WRITE(*,*)'Enter Z0 mass, width, and GammaX'
           READ(21,*)RMASS(200),GAMZ0,RGAMMAX
           IF(GAMZ0.LT.0.D0)GAMZ=GAMZ0
           IF(RGAMMAX.GE.0.D0) GAMMAX=RGAMMAX
         ELSEIF(JPR0.EQ.1497.OR.JPR0.EQ.1498)THEN
C           WRITE(*,*)'Enter W mass, width, and GammaX'
           READ(21,*)RMASS(198),GAMW0,RGAMMAX
           RMASS(199)=RMASS(198)
           IF(GAMW0.NE.0.D0)GAMW=GAMW0
           IF(RGAMMAX.GE.0.D0) GAMMAX=RGAMMAX
         ELSEIF( (JPR0.GE.1350.AND.JPR0.LE.1356) .OR.
     #           (JPR0.GE.1361.AND.JPR0.LE.1366) )THEN
C           WRITE(*,*)'Enter Z0 mass, width'
           READ(21,*)RMASS(200),GAMZ
C           WRITE(*,*)'Enter GAMMAX, M_Z*(INF), M_Z*(SUP)'
           READ(21,*)RGAMMAX,EMMIN,EMMAX
           IF(RGAMMAX.GE.0.D0) GAMMAX=RGAMMAX
           IF(IPROC.GT.0)THEN
             EMMIN=RMASS(200)-GAMZ*GAMMAX
             EMMAX=RMASS(200)+GAMZ*GAMMAX
           ENDIF
         ELSEIF(JPR0.GE.1371.AND.JPR0.LE.1373)THEN
C           WRITE(*,*)'Enter M_LL(INF), M_LL(SUP)'
           READ(21,*)EMMIN,EMMAX
         ELSEIF( (JPR0.GE.1450.AND.JPR0.LE.1453) .OR.
     #           (JPR0.GE.1461.AND.JPR0.LE.1463) .OR.
     #           (JPR0.GE.1471.AND.JPR0.LE.1473) )THEN
C           WRITE(*,*)'Enter W mass, width'
           READ(21,*)RMASS(198),GAMW
           RMASS(199)=RMASS(198)
C           WRITE(*,*)'Enter GAMMAX, M_W*(INF), M_W*(SUP)'
           READ(21,*)RGAMMAX,EMMIN,EMMAX
           IF(RGAMMAX.GE.0.D0) GAMMAX=RGAMMAX
         ENDIF
      ELSEIF (JPR.EQ.28) THEN
C         WRITE(*,*)'Enter W mass, width'
         READ(21,*)RMASS(198),GAMW0
         RMASS(199)=RMASS(198)
C         WRITE(*,*)'Enter Z mass, width'
         READ(21,*)RMASS(200),GAMZ0
C         WRITE(*,*)'Enter VGAMMAX, V2GAMMAX'
         READ(21,*)VGAMMAX,V2GAMMAX
         IF(GAMW0.NE.0.D0)GAMW=GAMW0
         IF(GAMZ0.NE.0.D0)GAMZ=GAMZ0
         RGAMMAX=MAX(VGAMMAX,V2GAMMAX)
         IF(RGAMMAX.GE.0.D0) GAMMAX=RGAMMAX
      ELSEIF (JPR.EQ.16) THEN
C         WRITE(*,*)'Enter Higgs boson and top masses'
         READ(21,*)RMASS(201),RMASS(6)
      ELSEIF (JPR.EQ.17) THEN
         IF(ABS(IPROC).EQ.1705.OR.ABS(IPROC).EQ.11705)THEN
C           WRITE(*,*)'Enter bottom mass'
           READ(21,*)RMASS(5)
         ELSEIF(ABS(IPROC).EQ.1706.OR.ABS(IPROC).EQ.11706)THEN
C           WRITE(*,*)'Enter top mass, W mass'
CBPK-> 
            READ(21,'(A)') TMPSTR2
            READ(TMPSTR2,*,ERR=616)RMASS(6),RMASS(198)
c            PRINT *,'NEWFORM IN INPUT',RMASS(6),RMASS(198)
            RMASS(199)=RMASS(198)
            GOTO 617
 616        OLDFORM=.TRUE.
            READ(TMPSTR2,*) RMASS(8)
c            PRINT *,'OLDFORM IN INPUT',RMASS(6),TMPSTR2(8:8)
 617        CONTINUE
CBPK-<
         ENDIF
      ELSEIF (JPR.EQ.26) THEN
C         WRITE(*,*)'Enter W mass, width'
         READ(21,*)RMASS(198),GAMW0
         RMASS(199)=RMASS(198)
C         WRITE(*,*)'Enter Higgs boson mass'
         READ(21,*)RMASS(201)
         IF(GAMW0.NE.0.D0)GAMW=GAMW0
      ELSEIF (JPR.EQ.27) THEN
C         WRITE(*,*)'Enter Z mass, width'
         READ(21,*)RMASS(200),GAMZ0
C         WRITE(*,*)'Enter Higgs boson mass'
         READ(21,*)RMASS(201)
         IF(GAMZ0.NE.0.D0)GAMZ=GAMZ0
      ELSEIF (JPR.EQ.20) THEN
c         WRITE(*,*)'Enter top mass, W mass'
         READ(21,*)RMASS(6),RMASS(198)
         RMASS(199)=RMASS(198)
      ELSE
         WRITE(*,*)'   Bad process code IPROC =',IPROC
         CALL HWWARN('HWIGPR',502)
         GOTO 999
      ENDIF
C      WRITE(*,*)'Enter quark (d,u,s,c,b) and gluon masses'
      READ(21,*)RMASS(1),RMASS(2),RMASS(3),
     #         RMASS(4),RMASS(5),RMASS(13)
      DO I=1,5
         RMASS(I+6)=RMASS(I)
      ENDDO
C NO SOFT ME CORRECTIONS (ALREADY INCLUDED IN MC@NLO)
      IF(IPROC.LT.0)THEN
        SOFTME=.FALSE.
      ELSE
        SOFTME=.TRUE.
      ENDIF
C HERWIG RUNS IN WEIGHTED MODE EXCEPT IN THE CASE OF SINGLE TOP IN
C ORDER TO AVOID TOO MANY EVENTS KILLED BY SHOWER
      NOWGT=.FALSE.
      IF(JPR.EQ.20.AND.IPROC.GT.0)NOWGT=.TRUE.
C NEGATIVE WEIGHTS ALLOWED
      NEGWTS=.TRUE.
C      MAXPR=2
C      MAXER=MAXEV/100
      LRSUD=0
      LWSUD=77
C IN THE CASE HERWIG PDFS ARE USED, ADOPT MRST
      NSTRU=8
      PRVTX=.FALSE.
      PTMIN=0.5
      NRN(1)=1973774260
      NRN(2)=1099242306
C THE FOLLOWING SHOULD BE USED ONLY IN WEIGHTED MODE
      IF(.NOT.NOWGT)THEN
        WGTMAX=1.000001D0
        AVABW=1.000001D0
      ENDIF
C FOR TOP PRODUCTION (HARMLESS ELSEWHERE)
CJK remove  RLTIM settings as they don't have any effect according to S.Frixion
CJK      RLTIM(6)=1.D-23
CJK      RLTIM(12)=1.D-23
C---B FRAGMENTATION PARAMETERS (FOR B PRODUCTION ONLY)
      IF(ABS(IPROC).EQ.1705.OR.ABS(IPROC).EQ.11705)THEN
        PSPLT(2)=0.5
      ENDIF
C---USE THE FOLLOWING FOR SINGLE TOP -- AVOIDS TROUBLES WITH ISR
C         IF(JPR.EQ.20)THEN
C           CALL HWUSTA('B+      ')
C           CALL HWUSTA('B-      ')
C           CALL HWUSTA('B_D0    ')
C           CALL HWUSTA('B_DBAR0 ')
C           CALL HWUSTA('B_S0    ')
C           CALL HWUSTA('B_SBAR0 ')
C           CALL HWUSTA('SIGMA_B+')
C           CALL HWUSTA('LMBDA_B0')
C           CALL HWUSTA('SIGMA_B-')
C           CALL HWUSTA('XI_B0   ')
C           CALL HWUSTA('XI_B-   ')
C           CALL HWUSTA('OMEGA_B-')
C           CALL HWUSTA('B_C-    ')
C           CALL HWUSTA('UPSLON1S')
C           CALL HWUSTA('SGM_BBR-')
C           CALL HWUSTA('LMD_BBR0')
C           CALL HWUSTA('SGM_BBR+')
C           CALL HWUSTA('XI_BBAR0')
C           CALL HWUSTA('XI_B+   ')
C           CALL HWUSTA('OMG_BBR+')
C           CALL HWUSTA('B_C+    ')
C           IF(IPROC.LT.0)THEN
C             CALL HWMODK(6,ONE,100,12,-11,5,0,0)
C             CALL HWMODK(-6,ONE,100,-12,11,-5,0,0)
C           ELSE
C             CALL HWMODK(6,ONE,-1,12,-11,5,0,0)
C             CALL HWMODK(-6,ONE,-1,-12,11,-5,0,0)
C           ENDIF
C         ENDIF

      RETURN

 100  WRITE (*,*)
     >   'INITMCATNLO: inparmMcAtNlo.dat file could not be opened!'
      WRITE (*,*)'Please link an *.inparm file to inparmMcAtNlo.dat!'
 999  STOP

      END
      
