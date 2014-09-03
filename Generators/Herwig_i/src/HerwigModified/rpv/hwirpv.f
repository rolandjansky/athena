
C----------------------------------------------------------------------
      SUBROUTINE HWIRPV
C----------------------------------------------------------------------
c     SUBROUTINE TO INITIALISE THE NEW FOUR BODY RPV MATRIX ELEMENTS
C----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      DOUBLE PRECISION EPS,TOTAL,OLD,NEW
      INTEGER III,JJJ,KKK,I
      PARAMETER(EPS=1.D-10)
      LOGICAL FOUND,LLEFND(5,3,3,3),LQDFND(8,3,3,3),UDDFND(4,3,3,3),
     &     NEUT,CHAR
C--common block for the parameters for the new modes
      DOUBLE PRECISION LLEMAX,LLEAVR,LQDMAX,LQDAVR,UDDMAX,UDDAVR
      COMMON /HWDRP4/ LLEMAX(5,3,3,3),LLEAVR(5,3,3,3),
     &                LQDMAX(8,3,3,3),LQDAVR(8,3,3,3),
     &                UDDMAX(4,3,3,3),UDDAVR(4,3,3,3)
C--look at the stau decay modes
      NEUT=.FALSE.
      CHAR=.TRUE.
      DO I=1,NDKYS
         IF(IDK(I).EQ.429) THEN
C--check two body stau -> neutralino tau not present 
            IF((IDKPRD(1,I).EQ.450.AND.IDKPRD(2,I).EQ.125).OR.
     &         (IDKPRD(2,I).EQ.450.AND.IDKPRD(1,I).EQ.125)) NEUT=.TRUE.
C--check two body stau -> chargino nu_tau not present
            IF((IDKPRD(1,I).EQ.456.AND.IDKPRD(2,I).EQ.125).OR.
     &         (IDKPRD(2,I).EQ.456.AND.IDKPRD(1,I).EQ.125)) CHAR=.TRUE.
         ENDIF
      ENDDO
      NEUT=.false.
C--zero the partial widths
      DO 50 III=1,3
      DO 50 JJJ=1,3
      DO 50 KKK=1,3
         DO 51 I=1,5
         LLEFND(I,III,JJJ,KKK)=.FALSE.
         LLEAVR(I,III,JJJ,KKK)=0.0D0
 51      LLEMAX(I,III,JJJ,KKK)=0.0D0
         DO 52 I=1,8
         LQDFND(I,III,JJJ,KKK)=.FALSE.
         LQDAVR(I,III,JJJ,KKK)=0.0D0
 52      LQDMAX(I,III,JJJ,KKK)=0.0D0
         DO 53 I=1,4
         UDDFND(I,III,JJJ,KKK)=.FALSE.
         UDDAVR(I,III,JJJ,KKK)=0.0D0
 53      UDDMAX(I,III,JJJ,KKK)=0.0D0
 50   CONTINUE
      FOUND = .FALSE.
C--ensure copuling initialisation
      CALL HWISPC
C--calculate all the modes we need
C--first the LLE neutralino modes
      IF(.NOT.NEUT) THEN 
         DO 100 III=1,3
         DO 100 JJJ=1,3
         DO 100 KKK=1,3
            IF(ABS(LAMDA1(III,JJJ,KKK)).GT.EPS.AND.
     &           RMASS(429).GT.(RMASS(125+2*III)+RMASS(126+2*JJJ)+
     &           RMASS(119+2*KKK)+RMASS(125      ))) THEN
               FOUND = .TRUE.
               LLEFND(1,III,JJJ,KKK)=.TRUE.
C--calculate the partial width
C-- stau- -> l_i+ nu_bar_j l_k- tau-
               CALL HWDRMX(429,125+2*III,126+2*JJJ,119+2*KKK,125,
     &              LLEMAX(1,III,JJJ,KKK),LLEAVR(1,III,JJJ,KKK))
C-- stau- -> l_i- nu_j l_k+ tau-
               CALL HWDRMX(429,119+2*III,120+2*JJJ,125+2*KKK,125,
     &              LLEMAX(2,III,JJJ,KKK),LLEAVR(2,III,JJJ,KKK))
C-- stau+ -> l_i- nu_j l_k+ tau+
               CALL HWDRMX(435,119+2*III,120+2*JJJ,125+2*KKK,131,
     &              LLEMAX(1,III,JJJ,KKK),LLEAVR(1,III,JJJ,KKK))
C-- stau+ -> l_i+ nu_bar_j l_k- tau+
               CALL HWDRMX(435,125+2*III,126+2*JJJ,119+2*KKK,131,
     &              LLEMAX(2,III,JJJ,KKK),LLEAVR(2,III,JJJ,KKK))
C--average
               LLEAVR(1,III,JJJ,KKK) = 0.5D0*LLEAVR(1,III,JJJ,KKK)
               LLEAVR(2,III,JJJ,KKK) = 0.5D0*LLEAVR(2,III,JJJ,KKK)
            ENDIF
 100     CONTINUE
      ENDIF
C-- LLE chargino modes
C-- first mode
      IF(.NOT.CHAR) THEN
         DO 110 III=1,3
         DO 110 JJJ=1,3
         DO 110 KKK=1,3
            IF(ABS(LAMDA1(III,JJJ,KKK)).GT.EPS.AND.
     &           RMASS(429).GT.(RMASS(126+2*III)+RMASS(125+2*JJJ)+
     &           RMASS(126+2*III)+RMASS(126))) THEN
               FOUND = .TRUE.
               LLEFND(2,III,JJJ,KKK)=.TRUE.
C--calculate the partial width
C-- stau- -> nu_i l_j- nu_bar_k nu_tau
               CALL HWDRMX(429,120+2*III,119+2*JJJ,126+2*KKK,126,
     &              LLEMAX(3,III,JJJ,KKK),LLEAVR(3,III,JJJ,KKK))
C-- stau+ -> nu_bar_i l_j+ nu_k nu_bar_tau
               CALL HWDRMX(435,126+2*III,125+2*JJJ,120+2*KKK,132,
     &              LLEMAX(3,III,JJJ,KKK),LLEAVR(3,III,JJJ,KKK))
C--average
               LLEAVR(3,III,JJJ,KKK) = 0.5*LLEAVR(3,III,JJJ,KKK)
            ENDIF
 110     CONTINUE
      ENDIF
C--second mode
      IF(.NOT.CHAR) THEN
         DO 120 III=1,3
         DO 120 JJJ=1,3
         DO 120 KKK=1,3
            IF(ABS(LAMDA1(III,JJJ,KKK)).GT.EPS.AND.
     &           RMASS(429).GT.(RMASS(126+2*III)+RMASS(126+2*JJJ)+
     &           RMASS(125+2*III)+RMASS(126))) THEN
               FOUND = .TRUE.
               LLEFND(3,III,JJJ,KKK)=.TRUE.
C--calculate the partial width
C-- stau- -> nu_i l_j- nu_bar_k nu_tau
               CALL HWDRMX(429,126+2*III,126+2*JJJ,119+2*KKK,126,
     &              LLEMAX(4,III,JJJ,KKK),LLEAVR(4,III,JJJ,KKK))
C-- stau+ -> nu_bar_i l_j+ nu_k nu_bar_tau
               CALL HWDRMX(435,120+2*III,120+2*JJJ,125+2*KKK,132,
     &              LLEMAX(4,III,JJJ,KKK),LLEAVR(4,III,JJJ,KKK))
C--average
               LLEAVR(4,III,JJJ,KKK) = 0.5*LLEAVR(4,III,JJJ,KKK)
            ENDIF
 120     CONTINUE
      ENDIF
C--third mode
      IF(.NOT.CHAR) THEN
         DO 130 III=1,3
         DO 130 JJJ=1,3
         DO 130 KKK=1,3
            IF(ABS(LAMDA1(III,JJJ,KKK)).GT.EPS.AND.
     &           RMASS(429).GT.(RMASS(125+2*III)+RMASS(125+2*JJJ)+
     &           RMASS(125+2*III)+RMASS(126))) THEN
               FOUND = .TRUE.
               LLEFND(4,III,JJJ,KKK)=.TRUE.
C--calculate the partial width
C-- stau- -> nu_i l_j- nu_bar_k nu_tau
               CALL HWDRMX(429,119+2*III,119+2*JJJ,125+2*KKK,126,
     &              LLEMAX(5,III,JJJ,KKK),LLEAVR(5,III,JJJ,KKK))
C-- stau+ -> nu_bar_i l_j+ nu_k nu_bar_tau
               CALL HWDRMX(435,125+2*III,125+2*JJJ,119+2*KKK,132,
     &              LLEMAX(5,III,JJJ,KKK),LLEAVR(5,III,JJJ,KKK))
C--average
               LLEAVR(5,III,JJJ,KKK) = 0.5*LLEAVR(5,III,JJJ,KKK)
            ENDIF
 130     CONTINUE
      ENDIF
C--LQD neutralino modes 
C--first mode
      If(.NOT.NEUT) THEN
         DO 200 III=1,3
         DO 200 JJJ=1,3
         DO 200 KKK=1,3
            IF(ABS(LAMDA2(III,JJJ,KKK)).GT.EPS.AND.
     &           RMASS(429).GT.(RMASS(120+2*III)+RMASS(2*JJJ-1)+
     &           RMASS(2*KKK+5)+RMASS(125))) THEN
               FOUND = .TRUE.
               LQDFND(1,III,JJJ,KKK)=.TRUE.
C--calculate the partial width
C-- stau- -> nu_i d_j dbar_k tau-
               CALL HWDRMX(429,120+2*III,2*JJJ-1,2*KKK+5,125,
     &              LQDMAX(1,III,JJJ,KKK),LQDAVR(1,III,JJJ,KKK))
C-- stau- -> nu_bar_i dbar_j d_k tau-
               CALL HWDRMX(429,126+2*III,2*JJJ+5,2*KKK-1,125,
     &              LQDMAX(3,III,JJJ,KKK),LQDAVR(3,III,JJJ,KKK))
C-- stau+ -> nu_bar_i dbar_j d_k tau+
               CALL HWDRMX(435,126+2*III,2*JJJ+5,2*KKK-1,131,
     &              LQDMAX(1,III,JJJ,KKK),LQDAVR(1,III,JJJ,KKK))
C-- stau+ -> nu_i d_j dbar_k tau+
               CALL HWDRMX(435,120+2*III,2*JJJ-1,2*KKK+5,131,
     &              LQDMAX(3,III,JJJ,KKK),LQDAVR(3,III,JJJ,KKK))
C--average
               LQDAVR(1,III,JJJ,KKK) = 0.5d0*LQDAVR(1,III,JJJ,KKK)
               LQDAVR(3,III,JJJ,KKK) = 0.5d0*LQDAVR(3,III,JJJ,KKK)
            ENDIF
 200     CONTINUE
      ENDIF
C--second mode
      IF(.NOT.NEUT) THEN
         DO 210 III=1,3
         DO 210 JJJ=1,3
         DO 210 KKK=1,3
            IF(ABS(LAMDA2(III,JJJ,KKK)).GT.EPS.AND.
     &           RMASS(429).GT.(RMASS(119+2*III)+RMASS(2*JJJ)+
     &           RMASS( 5 +2*KKK)+RMASS( 125) ).AND.
     &           JJJ.NE.3) THEN
               FOUND = .TRUE.
               LQDFND(2,III,JJJ,KKK)=.TRUE.
C--calculate the partial width
C-- stau- -> l_i- u_j dbar_k tau-
               CALL HWDRMX(429,119+2*III,2*JJJ,5+2*KKK,125,
     &              LQDMAX(2,III,JJJ,KKK),LQDAVR(2,III,JJJ,KKK))
C-- stau- -> l_i+ ubar_j d_k tau-
               CALL HWDRMX(429,125+2*III,6+2*JJJ,2*KKK-1,125,
     &              LQDMAX(4,III,JJJ,KKK),LQDAVR(4,III,JJJ,KKK))
C-- stau+ -> l_i+ ubar_j d_k tau+
               CALL HWDRMX(435,125+2*III,6+2*JJJ,2*KKK-1,131,
     &              LQDMAX(2,III,JJJ,KKK),LQDAVR(2,III,JJJ,KKK))
C-- stau+ -> l_i- u_j dbar_k tau+
               CALL HWDRMX(435,119+2*III,2*JJJ,5+2*KKK,131,
     &              LQDMAX(4,III,JJJ,KKK),LQDAVR(4,III,JJJ,KKK))
C--store the average (this is the partial width)
               LQDAVR(2,III,JJJ,KKK) = 0.5d0*LQDAVR(2,III,JJJ,KKK)
               LQDAVR(4,III,JJJ,KKK) = 0.5d0*LQDAVR(4,III,JJJ,KKK)
            ENDIF
 210     CONTINUE
      ENDIF
C--chargino modes
C--first mode
      IF(.NOT.CHAR) THEN
         DO 220 III=1,3
         DO 220 JJJ=1,3
         DO 220 KKK=1,3
            IF(ABS(LAMDA2(III,JJJ,KKK)).GT.EPS.AND.
     &           RMASS(429).GT.(RMASS(119+2*III)+RMASS(2*JJJ-1)+
     &           RMASS(2*KKK-1)+RMASS( 126) )) THEN
               FOUND = .TRUE.
               LQDFND(3,III,JJJ,KKK)=.TRUE.
C--calculate the partial width
C-- stau- -> l_i- d_j dbar_k nu_tau
               CALL HWDRMX(429,119+2*III,2*JJJ-1,5+2*KKK,126,
     &              LQDMAX(5,III,JJJ,KKK),LQDAVR(5,III,JJJ,KKK))
C--  stau+ -> l_i+ dbar_j d_k nu_tau_bar
               CALL HWDRMX(435,125+2*III,2*JJJ+5,2*KKK-1,132,
     &              LQDMAX(5,III,JJJ,KKK),LQDAVR(5,III,JJJ,KKK))
C--store the average (this is the partial width)
               LQDAVR(5,III,JJJ,KKK) = 0.5d0*LQDAVR(5,III,JJJ,KKK)
            ENDIF
 220     CONTINUE
      ENDIF
C--second mode
      IF(.NOT.CHAR) THEN
         DO 230 III=1,3
         DO 230 JJJ=1,3
         DO 230 KKK=1,3
            IF(ABS(LAMDA2(III,JJJ,KKK)).GT.EPS.AND.
     &           RMASS(429).GT.(RMASS(120+2*III)+RMASS(2*JJJ-1)+
     &           RMASS(2*KKK)+RMASS( 126) ).AND.
     &           JJJ.NE.3) THEN
               FOUND = .TRUE.
               LQDFND(4,III,JJJ,KKK)=.TRUE.
C--calculate the partial width
C-- stau- -> nu_bar_i ubar_j d_k nu_tau
               CALL HWDRMX(429,126+2*III,6+2*JJJ,2*KKK-1,126,
     &              LQDMAX(6,III,JJJ,KKK),LQDAVR(6,III,JJJ,KKK))
C--  stau+ -> nu_i u_j dbar_k nu_tau_bar
               CALL HWDRMX(435,120+2*III,2*JJJ,2*KKK+5,132,
     &              LQDMAX(6,III,JJJ,KKK),LQDAVR(6,III,JJJ,KKK))
C--store the average (this is the partial width)
               LQDAVR(6,III,JJJ,KKK) = 0.5d0*LQDAVR(6,III,JJJ,KKK)
            ENDIF
 230     CONTINUE
      ENDIF
C--third mode
      IF(.NOT.CHAR) THEN
         DO 240 III=1,3
         DO 240 JJJ=1,3
         DO 240 KKK=1,3
            IF(ABS(LAMDA2(III,JJJ,KKK)).GT.EPS.AND.
     &           RMASS(429).GT.(RMASS(120+2*III)+RMASS(2*JJJ-1)+
     &           RMASS(2*KKK)+RMASS( 126) ).AND.
     &           KKK.NE.3) THEN
               FOUND = .TRUE.
               LQDFND(5,III,JJJ,KKK)=.TRUE.
C--calculate the partial width
C-- stau- -> nu_bar_i ubar_j d_k nu_tau
               CALL HWDRMX(429,120+2*III,2*JJJ-1,2*KKK+6,126,
     &              LQDMAX(7,III,JJJ,KKK),LQDAVR(7,III,JJJ,KKK))
C--  stau+ -> nu_i u_j dbar_k nu_tau_bar
               CALL HWDRMX(435,126+2*III,2*JJJ+5,2*KKK,132,
     &              LQDMAX(7,III,JJJ,KKK),LQDAVR(7,III,JJJ,KKK))
C--store the average (this is the partial width)
               LQDAVR(7,III,JJJ,KKK) = 0.5d0*LQDAVR(7,III,JJJ,KKK)
            ENDIF
 240     CONTINUE
      ENDIF
C--fourth mode
      IF(.NOT.CHAR) THEN
         DO 250 III=1,3
         DO 250 JJJ=1,3
         DO 250 KKK=1,3
            IF(ABS(LAMDA2(III,JJJ,KKK)).GT.EPS.AND.
     &           RMASS(429).GT.(RMASS(119+2*III)+RMASS(2*JJJ)+
     &           RMASS(2*KKK)+RMASS( 126) ).AND.
     &           JJJ.NE.3.AND.KKK.NE.3) THEN
               FOUND = .TRUE.
               LQDFND(6,III,JJJ,KKK)=.TRUE.
C--calculate the partial width
C-- stau- -> nu_bar_i ubar_j d_k nu_tau
               CALL HWDRMX(429,119+2*III,2*JJJ,2*KKK+6,126,
     &              LQDMAX(8,III,JJJ,KKK),LQDAVR(8,III,JJJ,KKK))
C--  stau+ -> nu_i u_j dbar_k nu_tau_bar
               CALL HWDRMX(435,125+2*III,2*JJJ+6,2*KKK,132,
     &              LQDMAX(8,III,JJJ,KKK),LQDAVR(8,III,JJJ,KKK))
C--store the average (this is the partial width)
               LQDAVR(8,III,JJJ,KKK) = 0.5d0*LQDAVR(8,III,JJJ,KKK)
            ENDIF
 250     CONTINUE
      ENDIF
C--UDD Neutralino mode
      IF(.NOT.NEUT) THEN
         DO 300 III=1,3
         DO 300 JJJ=1,3
         DO 300 KKK=JJJ,3
            IF(ABS(LAMDA3(III,JJJ,KKK)).GT.EPS.AND.III.NE.3.AND.
     &           RMASS(429).GT.(RMASS(2*III  )+RMASS(2*JJJ-1)+
     &           RMASS(2*KKK-1)+RMASS(125    ))) THEN
               FOUND = .TRUE.
               UDDFND(1,III,JJJ,KKK)=.TRUE.
C--calculate the partial width
C-- stau- -> u_i d_j d_k tau-
               CALL HWDRMX(429,2*III,2*JJJ-1,2*KKK-1,125,
     &              UDDMAX(1,III,JJJ,KKK),UDDAVR(1,III,JJJ,KKK))
C-- stau- -> ubar_i dbar_j dbar_k tau-
               CALL HWDRMX(429,2*III+6,2*JJJ+5,2*KKK+5,125,
     &              UDDMAX(2,III,JJJ,KKK),UDDAVR(2,III,JJJ,KKK))
C-- stau+ -> ubar_i dbar_j dbar_k tau+
               CALL HWDRMX(435,2*III+6,2*JJJ+5,2*KKK+5,131,
     &              UDDMAX(1,III,JJJ,KKK),UDDAVR(1,III,JJJ,KKK))
C-- stau+ -> u_i d_j d_k tau+
               CALL HWDRMX(435,2*III,2*JJJ-1,2*KKK-1,131,
     &              UDDMAX(2,III,JJJ,KKK),UDDAVR(2,III,JJJ,KKK))
C--average
               UDDAVR(1,III,JJJ,KKK) = 0.5*UDDAVR(1,III,JJJ,KKK)
               UDDAVR(2,III,JJJ,KKK) = 0.5*UDDAVR(2,III,JJJ,KKK)
            ENDIF
 300     CONTINUE
      ENDIF
C--first chargino modes
      IF(.NOT.CHAR) THEN
         DO 310 III=1,3
         DO 310 JJJ=III,3
         DO 310 KKK=1,3
            IF((ABS(LAMDA3(III,JJJ,KKK)).GT.EPS.OR.
     &           ABS(LAMDA3(JJJ,KKK,III)).GT.EPS).AND.
     &           RMASS(429).GT.(RMASS(2*III  )+RMASS(2*JJJ)+
     &           RMASS(2*KKK-1)+RMASS(126  )).AND.
     &           III.NE.3.AND.JJJ.NE.3) THEN
               FOUND = .TRUE.
               UDDFND(2,III,JJJ,KKK)=.TRUE.
C--calculate the partial width
C-- stau+ -> u_i u_j d_k nu_tau
               CALL HWDRMX(435,2*III  ,2*JJJ  ,2*KKK-1,132,
     &              UDDMAX(3,III,JJJ,KKK),UDDAVR(3,III,JJJ,KKK))
C-- stau- -> ubar_i ubar_j dbar_k nu_taubar
               CALL HWDRMX(429,2*III+6,2*JJJ+6,2*KKK+5,126,
     &              UDDMAX(3,III,JJJ,KKK),UDDAVR(3,III,JJJ,KKK))
C--average
               UDDAVR(3,III,JJJ,KKK) = 0.5*UDDAVR(3,III,JJJ,KKK)
            ENDIF
 310     CONTINUE
      ENDIF
c--second chargino modes
      IF(.NOT.CHAR) THEN
         DO 320 III=1,3
         DO 320 JJJ=III,3
         DO 320 KKK=JJJ,3
            IF((ABS(LAMDA3(III,JJJ,KKK)).GT.EPS.OR.
     &           ABS(LAMDA3(JJJ,III,KKK)).GT.EPS.OR.
     &           ABS(LAMDA3(KKK,JJJ,III)).GT.EPS).AND.
     &           RMASS(429).GT.(RMASS(2*III-1)+RMASS(2*JJJ-1)+
     &           RMASS(2*KKK-1)+RMASS(126    ))) THEN
               FOUND = .TRUE.
               UDDFND(3,III,JJJ,KKK)=.TRUE.
C--calculate the partial width
C-- stau- -> d_i d_j d_k nu_tau
               CALL HWDRMX(429,2*III-1,2*JJJ-1,2*KKK-1,126,
     &              UDDMAX(4,III,JJJ,KKK),UDDAVR(4,III,JJJ,KKK))
C-- stau+ -> ubar_i ubar_j dbar_k nu_taubar
               CALL HWDRMX(435,2*III+5,2*JJJ+5,2*KKK+5,132,
     &              UDDMAX(4,III,JJJ,KKK),UDDAVR(4,III,JJJ,KKK))
C--average
               UDDAVR(4,III,JJJ,KKK) = 0.5*UDDAVR(4,III,JJJ,KKK)
            ENDIF
 320     CONTINUE
      ENDIF
C--return if no new modes found
      IF(.NOT.FOUND) RETURN
C--calculate the total partial width for the new modes
      TOTAL=0.0D0
      DO 400 III=1,3
      DO 400 JJJ=1,3
      DO 400 KKK=1,3
         DO 401 I=1,5
 401     TOTAL = TOTAL+LLEAVR(I,III,JJJ,KKK)
         DO 402 I=1,8
 402     TOTAL = TOTAL+LQDAVR(I,III,JJJ,KKK)
         DO 403 I=1,4
 403     TOTAL = TOTAL+UDDAVR(I,III,JJJ,KKK)
 400  CONTINUE
C-total partial width of the old modes
      OLD = HBAR/RLTIM(429)
c--NEW TOTAL AND RESET THE LIFETIME
      NEW = OLD+TOTAL
      RLTIM(429) = HBAR/NEW
      RLTIM(435) = HBAR/NEW
c--RENORMALISE THE OLD MODES
      DO I=1,NDKYS
         IF(IDK(I).EQ.429.OR.IDK(I).EQ.435) 
     &        BRFRAC(I)=BRFRAC(I)*OLD/NEW
      ENDDO
C--now add the decay modes to the table this is quite complicated
C--add the LLE modes
      DO 500 III=1,3
      DO 500 JJJ=1,3
      DO 500 KKK=1,3
c-- first neutralino mode
         IF(LLEFND(1,III,JJJ,KKK)) THEN
C--stau- 
            CALL HWDRAD(429,125+2*III,126+2*JJJ,119+2*KKK,125,
     &           LLEAVR(1,III,JJJ,KKK)/NEW)
C--stau+
            CALL HWDRAD(435,119+2*III,120+2*JJJ,125+2*KKK,131,
     &           LLEAVR(1,III,JJJ,KKK)/NEW)
c-- second neutralino mode
C--stau-
            CALL HWDRAD(429,119+2*III,120+2*JJJ,125+2*KKK,125,
     &           LLEAVR(2,III,JJJ,KKK)/NEW)
C--stau+
            CALL HWDRAD(435,125+2*III,126+2*JJJ,119+2*KKK,131,
     &           LLEAVR(2,III,JJJ,KKK)/NEW)
         ENDIF
C-- first chargino mode
         IF(LLEFND(2,III,JJJ,KKK)) THEN
C--stau-
            CALL HWDRAD(429,120+2*III,119+2*JJJ,126+2*KKK,126,
     &           LLEAVR(3,III,JJJ,KKK)/NEW)
C--stau+
            CALL HWDRAD(435,126+2*III,125+2*JJJ,120+2*KKK,136,
     &           LLEAVR(3,III,JJJ,KKK)/NEW)
         ENDIF
C-- second chargino mode
         IF(LLEFND(3,III,JJJ,KKK)) THEN
C--stau-
            CALL HWDRAD(429,126+2*III,126+2*JJJ,119+2*KKK,126,
     &           LLEAVR(4,III,JJJ,KKK)/NEW)
C--stau+
            CALL HWDRAD(435,120+2*III,120+2*JJJ,125+2*KKK,132,
     &           LLEAVR(4,III,JJJ,KKK)/NEW)
         ENDIF
C-- second chargino mode
         IF(LLEFND(4,III,JJJ,KKK)) THEN
C--stau-
            CALL HWDRAD(429,119+2*III,119+2*JJJ,125+2*KKK,126,
     &           LLEAVR(5,III,JJJ,KKK)/NEW)
C--stau+
            CALL HWDRAD(435,125+2*III,125+2*JJJ,119+2*KKK,132,
     &           LLEAVR(5,III,JJJ,KKK)/NEW)
         ENDIF
 500  CONTINUE
C--add the LQD modes
      DO 600 III=1,3
      DO 600 JJJ=1,3
      DO 600 KKK=1,3
         IF(LQDFND(1,III,JJJ,KKK)) THEN
C--first mode
c--stau-
            CALL HWDRAD(429,120+2*III,2*JJJ-1,2*KKK+5,125,
     &           LQDAVR(1,III,JJJ,KKK)/NEW)
c--stau+
            CALL HWDRAD(435,126+2*III,2*JJJ+5,2*KKK-1,131,
     &           LQDAVR(1,III,JJJ,KKK)/NEW)
C--second mode
c--stau-
            CALL HWDRAD(429,126+2*III,2*JJJ+5,2*KKK-1,125,
     &           LQDAVR(3,III,JJJ,KKK)/NEW)
c--stau+
            CALL HWDRAD(435,120+2*III,2*JJJ-1,2*KKK+5,131,
     &           LQDAVR(3,III,JJJ,KKK)/NEW)
         ENDIF
C--third mode
         IF(LQDFND(2,III,JJJ,KKK)) THEN
c--stau-
            CALL HWDRAD(429,119+2*III,2*JJJ  ,2*KKK+5,125,
     &           LQDAVR(2,III,JJJ,KKK)/NEW)
c--stau+
            CALL HWDRAD(435,125+2*III,2*JJJ+6,2*KKK-1,131,
     &           LQDAVR(2,III,JJJ,KKK)/NEW)
C--fourth mode
c--stau-
            CALL HWDRAD(429,125+2*III,2*JJJ+6,2*KKK-1,125,
     &           LQDAVR(4,III,JJJ,KKK)/NEW)
c--stau+
            CALL HWDRAD(435,119+2*III,2*JJJ  ,2*KKK+5,131,
     &           LQDAVR(4,III,JJJ,KKK)/NEW)
         ENDIF
c--chargino modes
C--first mode
         IF(LQDFND(3,III,JJJ,KKK)) THEN
c--stau-
            CALL HWDRAD(429,119+2*III,2*JJJ-1,2*KKK+5,126,
     &           LQDAVR(5,III,JJJ,KKK)/NEW)
c--stau+
            CALL HWDRAD(435,125+2*III,2*JJJ+5,2*KKK-1,132,
     &           LQDAVR(5,III,JJJ,KKK)/NEW)
         ENDIF
C--second mode
         IF(LQDFND(4,III,JJJ,KKK)) THEN
c--stau-
            CALL HWDRAD(429,126+2*III,2*JJJ+6,2*KKK-1,126,
     &           LQDAVR(6,III,JJJ,KKK)/NEW)
c--stau+
            CALL HWDRAD(435,120+2*III,2*JJJ  ,2*KKK+5,132,
     &           LQDAVR(6,III,JJJ,KKK)/NEW)
         ENDIF
C--third mode
         IF(LQDFND(5,III,JJJ,KKK)) THEN
            CALL HWDRAD(429,120+2*III,2*JJJ-1,2*KKK+6,126,
     &           LQDAVR(7,III,JJJ,KKK)/NEW)
            CALL HWDRAD(435,126+2*III,2*JJJ+5,2*KKK  ,132,
     &           LQDAVR(7,III,JJJ,KKK)/NEW)

         ENDIF
C--fourth mode
         IF(LQDFND(6,III,JJJ,KKK)) THEN
c--stau-
            CALL HWDRAD(429,119+2*III,2*JJJ  ,2*KKK+6,126,
     &           LQDAVR(8,III,JJJ,KKK)/NEW)
c--stau+
            CALL HWDRAD(435,125+2*III,2*JJJ+6,2*KKK  ,132,
     &           LQDAVR(8,III,JJJ,KKK)/NEW)
         ENDIF
 600  CONTINUE
C--add the UDD modes
      DO 700 III=1,3
      DO 700 JJJ=1,3
      DO 700 KKK=1,3
C--neutralino modes
         IF(UDDFND(1,III,JJJ,KKK)) THEN
C--first mode
C--stau-
            CALL HWDRAD(429,2*III,2*JJJ-1,2*KKK-1,125,
     &           UDDAVR(1,III,JJJ,KKK)/NEW)
C--stau+
            CALL HWDRAD(435,2*III+6,2*JJJ+5,2*KKK+5,131,
     &           UDDAVR(1,III,JJJ,KKK)/NEW)
C--second mode
C--stau-
            CALL HWDRAD(429,2*III+6,2*JJJ+5,2*KKK+5,125,
     &           UDDAVR(2,III,JJJ,KKK)/NEW)
C--stau+
            CALL HWDRAD(435,2*III,2*JJJ-1,2*KKK-1,131,
     &           UDDAVR(2,III,JJJ,KKK)/NEW)
         ENDIF
C--first chargino mode
         IF(UDDFND(2,III,JJJ,KKK)) THEN
            CALL HWDRAD(429,2*III+6,2*JJJ+6,2*KKK+5,126,
     &           UDDAVR(3,III,JJJ,KKK)/NEW)
            CALL HWDRAD(435,2*III  ,2*JJJ  ,2*KKK-1,132,
     &           UDDAVR(3,III,JJJ,KKK)/NEW)
         ENDIF
C--second chargino mode
         IF(UDDFND(3,III,JJJ,KKK)) THEN
            CALL HWDRAD(429,2*III-1,2*JJJ-1,2*KKK-1,126,
     &           UDDAVR(4,III,JJJ,KKK)/NEW)
            CALL HWDRAD(435,2*III+5,2*JJJ+5,2*KKK+5,132,
     &           UDDAVR(4,III,JJJ,KKK)/NEW)
         ENDIF
 700  CONTINUE
C--format statements for the output
C AB      FORMAT('CALCULATING DECAY RATE FOR ',A8,' --> ',
C AB    &     A8,' ',A8,' ',A8,' ',A8)
C AB     FORMAT('PARTIAL WIDTH IS ',5E15.5,' +/- ',5E15.5)
      END
C----------------------------------------------------------------------
      SUBROUTINE HWDRPM(WGT,IDIN,PIN,ID1,ID2,ID3,ID4,POUT,GEN)
C----------------------------------------------------------------------
C     subroutine to calculate the weight and momentum for a
C     four body RPV decay
C----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      DOUBLE PRECISION WGT,PIN(5),POUT(5,4),MCHIL2,MCHIU2,MCHI2,RHO,
     &     HWRUNI,MCHI,PCM4,PCM1,PCM2,HWUPCM,PCHI(5),P23(5),FJAC,
     &     F1,F2,M23,MIN,A(2,6),B(2,6),C(2,6),PCHIM(5),m122,M232,M132,
     &     HWULDO,COLFAC,MAXWGT
      INTEGER IDIN,ID1,ID2,ID3,ID4,I,J,O(2),IDINT(6),IG
      DATA O/2,1/
      EXTERNAL HWRUNI,HWUPCM,HWULDO
C--H.E. functions
      DOUBLE COMPLEX S,D,FCHI(2,2,8,8),FCHIM(2,2,8,8),ME(2,2,2,2),
     &     PROPS(3,2)
      INTEGER P1,P2,P3,P4,METYPE
      COMMON/HWHEWS/S(8,8,2),D(8,8)
      DOUBLE PRECISION XMASS,PLAB,PRW,PCM
      COMMON/HWHEWP/XMASS(10),PLAB(5,10),PRW(5,2),PCM(5,10)
      DOUBLE COMPLEX MESQ,II
      PARAMETER(II=(0.0d0,1.0d0))
C--common block for the couplings
      DOUBLE PRECISION SW,CW,TW,E,G,RT,ORT,MW,MZ,AFN(2,12,2,4),
     &     AFG(2,6,2),AFC(2,12,2,2),OIJ(2,4,2),OIJP(2,2,2),OIJPP(2,4,4),
     &     HNN(2,3,4,4),HCC(2,3,2,2),HNC(2,4,2),HFF(2,4,12),HWW(2),
     &     HZZ(2),ZAB(12,2,2),HHB(2,3),GS
      COMMON /HWSPNC/ SW,CW,TW,E,G,RT,ORT,MW,MZ,GS,AFN,AFG,AFC,OIJ,OIJP,
     &               OIJPP,HNN,HCC,HNC,HFF,HWW,HZZ,ZAB,HHB
C--variables to be saved after initilisation
      LOGICAL FIRST,CHRINO,GEN
      DOUBLE PRECISION MN(2),MN2(2),GN(2),GMN(2)
      DATA FIRST /.TRUE./
      SAVE FIRST,MN,MN2,GN,GMN
C--initialisation
      IF(FIRST) THEN
         FIRST = .FALSE.
         HBAR=1.D-15*SQRT(GEV2NB/10.)/CSPEED
         MN(1)  = RMASS(450)
         GN(1)  = HBAR/RLTIM(450)
         MN2(1) = MN(1)**2
         GMN(1) = MN(1)*GN(1)
         MN(2)  = RMASS(454)
         GN(2)  = HBAR/RLTIM(454)
         MN2(2) = MN(2)**2
         GMN(2) = MN(2)*GN(2)
      ENDIF
C--work out if neutralino or chargino mode
      CHRINO=ID4.EQ.126.OR.ID4.EQ.132
C--mass of the incoming particle
      MIN=PIN(5)
C--check the kinematics
      IF(MIN.LT.RMASS(ID1)+RMASS(ID2)+RMASS(ID3)+RMASS(ID4)) THEN
         WGT = 0.0D0
         RETURN
      ENDIF
C--generate the mass of the off-shell gaugino
C--limits on the mass
      MCHIU2 = (MIN-RMASS(ID4))**2
      MCHIL2 = (RMASS(ID1)+RMASS(ID2)+RMASS(ID3))**2
      IF(CHRINO) THEN
         IG=2
      ELSE
         IG=1
      ENDIF
C--limits in transformaed variable
      MCHIU2 = ATAN((MCHIU2-MN2(IG))/GMN(IG))
      MCHIL2 = ATAN((MCHIL2-MN2(IG))/GMN(IG))
C--generate the value of rho
      RHO  = HWRUNI(0,MCHIL2,MCHIU2)
C--convert to a mass
      MCHI2 = GMN(IG)*TAN(RHO)+MN2(IG)
      MCHI = SQRT(MCHI2)
C--jacobian factor 
      FJAC = (MCHIU2-MCHIL2)/GMN(IG)
C--perform the first decay
      PCHI(5) = MCHI
      POUT(5,4) = RMASS(ID4)
      PCM4 = HWUPCM(MIN,MCHI,POUT(5,4))
      CALL HWDTWO(PIN,PCHI,POUT(1,4),PCM4,2.0D0,.TRUE.)
C--weight for this piece
      F1 = PCM4/8./PIFAC/MIN**2
C--mass of the 23 system
      M23 = SQRT(HWRUNI(2,(RMASS(ID2)+RMASS(ID3))**2,
     &     (MCHI-RMASS(ID1))**2))
      F2=((MCHI-RMASS(ID1))**2-(RMASS(ID2)+RMASS(ID3))**2)
C--perform this decay
      POUT(5,1) = RMASS(ID1)
      P23(5) = M23
      PCM1 = HWUPCM(MCHI,M23,POUT(5,1))
      CALL HWDTWO(PCHI,P23,POUT(1,1),PCM1,2.0D0,.TRUE.)
c--perform the last decay
      POUT(5,2) = RMASS(ID2)
      POUT(5,3) = RMASS(ID3)
      PCM2 = HWUPCM(M23,POUT(5,2),POUT(5,3))
      CALL HWDTWO(P23,POUT(1,2),POUT(1,3),PCM2,2.0D0,.TRUE.)
C--weight for this piece
      F2 = F2*PCM1*PCM2/64./PIFAC**4/MCHI/M23
C--put the phase space weight all together
      WGT = F1*FJAC*F2
C--colour factor
      IF(ID3.GT.12) THEN
         COLFAC = 1.0D0
      ELSEIF(ID1.GT.12) THEN
         COLFAC = 3.0D0
      ELSE 
         COLFAC = 6.0D0
      ENDIF
C--couplings of the upper vertex
      CALL HWDRTC(IDIN,CHRINO,A)
C--neutralinos
      IF(.NOT.CHRINO) THEN
C--LLE decays
         IF(ID3.GT.12) THEN
            CALL HWDRN1(ID1,ID2,ID3,ID4,B,C,METYPE,IDINT,MAXWGT)
C--LQD decays
         ELSEIF(ID1.GT.12) THEN
            CALL HWDRN2(ID1,ID2,ID3,ID4,B,C,METYPE,IDINT,MAXWGT)
C--UDD decays
         ELSE
            CALL HWDRN3(ID1,ID2,ID3,ID4,B,C,METYPE,IDINT,MAXWGT)
         ENDIF
C--chargino modes
      ELSE
C--LLE modes
         IF(ID3.GT.12) THEN
            CALL  HWDRC1(ID1,ID2,ID3,ID4,B,C,METYPE,IDINT,MAXWGT)
C--LQD mode
         ELSEIF(ID1.GT.12) THEN
            CALL  HWDRC2(ID1,ID2,ID3,ID4,B,C,METYPE,IDINT,MAXWGT)
C--UDD mode
         ELSE
            CALL  HWDRC3(ID1,ID2,ID3,ID4,B,C,METYPE,IDINT,MAXWGT)
         ENDIF
      ENDIF
C--now we have the couplings, masses and momenta we can calculate the matrix element
C--reorder the momenta for the H.E. code
C--outgoing particles
      DO 6 I=1,4
      PCM(1,I)=POUT(3,I)
      PCM(2,I)=POUT(1,I)
      PCM(3,I)=POUT(2,I)
      PCM(4,I)=POUT(4,I)
 6    PCM(5,I)=POUT(5,I)
C-- intermediate neutralino
      PCHIM(1) = PCHI(3)
      PCHIM(2) = PCHI(1)
      PCHIM(3) = PCHI(2)
      PCHIM(4) = PCHI(4)
      PCHIM(5) = PCHI(5)
C--compute the S functions
      CALL HWHEW2(4,PCM(1,1),S(1,1,2),S(1,1,1),D)
      DO 7 I=1,8
      DO 7 J=1,8
      S(I,J,2) = -S(I,J,2)
 7    D(I,J)   = TWO*D(I,J)
C--compute the one 5 function we need
      CALL HWH2F3(4,FCHI ,PCHIM, MN(IG))
      CALL HWH2F3(4,FCHIM,PCHIM,-MN(IG))
c--COMPUTE THE INVARIANTs
      M122 = 2.0D0*HWULDO(POUT(1,1),POUT(1,2))
      M232 = 2.0D0*HWULDO(POUT(1,2),POUT(1,3))
      M132 = 2.0D0*HWULDO(POUT(1,1),POUT(1,3))
C--propagators
      DO J=1,2
         IF(METYPE.LE.4.OR.METYPE.EQ.9.OR.METYPE.EQ.14) THEN
            PROPS(1,J) = 1./(M232-RMASS(IDINT(J))**2
     &           +II*RMASS(IDINT(J  ))*HBAR/RLTIM(IDINT(J  )))
            PROPS(2,J) = 1./(M132-RMASS(IDINT(J+2))**2
     &           +II*RMASS(IDINT(J+2))*HBAR/RLTIM(IDINT(J+2)))
            PROPS(3,J) = 1./(M122-RMASS(IDINT(J+4))**2
     &           +II*RMASS(IDINT(J+4))*HBAR/RLTIM(IDINT(J+4)))
         ELSEIF(METYPE.EQ.5.OR.METYPE.EQ.10) THEN
            PROPS(1,J) = 1./(M122-RMASS(IDINT(J  ))**2
     &           +II*RMASS(IDINT(J  ))*HBAR/RLTIM(IDINT(J  )))
         ELSE
            PROPS(1,J) = 1./(M232-RMASS(IDINT(J))**2
     &           +II*RMASS(IDINT(J  ))*HBAR/RLTIM(IDINT(J  )))
            PROPS(2,J) = 1./(M132-RMASS(IDINT(J+2))**2
     &           +II*RMASS(IDINT(J+2))*HBAR/RLTIM(IDINT(J+2)))
         ENDIF
      ENDDO
c--COMPUTE THE MATRIX ELEMENT
C--matrix element for stau decay
      DO 100 P1=1,2
      DO 100 P2=1,2
      DO 100 P3=1,2
      DO 100 P4=1,2
         ME(P1,P2,P3,P4) = 0.0d0
         DO J=1,2
C--propagators
C--LLE AND LQD NEUTRALINO 1
            IF(METYPE.EQ.1) THEN
C--   FIRST DIAGRAM
               IF(P2.EQ.O(P3))  ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              +PROPS(1,J)*A(P4,J  )*B(O(P1),J  )*C(O(P2),J  )*
     &              S(2,3,P2)*FCHI(P1,P4,1,4)
C--   SECOND DIAGRAM
               IF(P1.EQ.O(P3)) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)+
     &              (-PROPS(2,J))*A(P4,J+2)*B(O(P2),j+2)*C(O(P1),j+2)*
     &              S(1,3,p1)*FCHI(P2,P4,2,4)
C--   THIRD DIAGRAM
               IF(P1.EQ.P2) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              -(PROPS(3,J)*A(P4,J+4)*B(P3,j+4)*C(O(P1),j+4)
     &              *S(2,1,P2)*FCHIM(o(P4),p3,4,3))
C--UDD NEUTRALINO 1
            ELSEIF(METYPE.EQ.2) THEN
C--   FIRST  DIAGRAM
               IF(P2.EQ.P3) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              -PROPS(1,J)*A(O(P4),J  )*B(O(P1),J  )*C(O(P2),J  )*
     &              S(2,3,P2)*FCHI(P1,O(P4),1,4)
C--   SECOND DIAGRAM
               IF(P1.EQ.P3) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              +PROPS(2,J)*A(O(P4),J+2)*B(O(P2),J+2)*C(O(P3),J+2)*
     &              s(1,3,P1)*FCHI(P2,O(P4),2,4)
C--   THIRD  DIAGRAM
               IF(P1.EQ.P2) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              -(PROPS(3,J)*A(O(P4),J+4)*B(O(P3),J+4)*C(O(P2),J+4)*
     &              S(1,2,P1)*FCHI(P3,O(P4),3,4))
C--LLE AND LQD NEUTRALINO 2
            ELSEIF(METYPE.EQ.3) THEN
C--FIRST DIAGRAM
               IF(P2.EQ.O(P3)) ME(P1,P2,P3,P4)= ME(P1,P2,P3,P4)
     &              -(PROPS(1,J)*A(O(P4),J  )*B(  P1 ,J  )*C(  P2 ,J  )*
     &              S(3,2,O(P2))*FCHIM(P4,P1,4,1))
C--SECOND DIAGRAM
               IF(P1.EQ.O(P3)) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              +PROPS(2,J)*A(O(P4),J+2)*B(  P2 ,j+2)*C(  P1 ,J+2)*
     &              S(3,1,O(P1))*FCHIM(P4,P2,4,2)
C--THIRD DIAGRAM
               IF(P1.EQ.P2) ME(P1,P2,P3,P4)=ME(P1,P2,P3,P4)
     &              +PROPS(3,J)*A(O(P4),J+4)*B(O(P3),j+4)*C(  P1 ,J+4)
     &              *s(1,2,O(P2))*FCHI(P3,O(P4),3,4)
C--UDD
            ELSEIF(METYPE.EQ.4) THEN
C--FIRST DIAGRAM
               IF(P2.EQ.P3) ME(P1,P2,P3,P4)=ME(P1,P2,P3,P4)
     &              +PROPS(1,J)*A(O(P4),J  )*B(P1,J  )*C(P2,J  )*
     &              S(2,3,O(P2))*FCHIM(P4,P1,4,1)
C--SECOND DIAGRAM
               IF(P1.EQ.P3) ME(P1,P2,P3,P4)=ME(P1,P2,P3,P4)
     &              +PROPS(2,J)*A(O(P4),J+2)*B(P2,J+2)*C(P1,J+2)*
     &              S(3,1,O(P3))*FCHIM(P4,P2,4,2)
C--THIRD DIAGRAM
               IF(P1.EQ.P2) ME(P1,P2,P3,P4)=ME(P1,P2,P3,P4)
     &              -(PROPS(3,J)*A(O(P4),J+4)*B(P3,J+4)*C(P1,J+4)*
     &              S(2,1,O(P1))*FCHIM(P4,P3,4,3))
C--LLE AND LQD CHARGINO 1
            ELSEIF(METYPE.EQ.5) THEN
               IF(P1.EQ.P2) ME(P1,P2,P3,P4)=ME(P1,P2,P3,P4)
     &              +PROPS(1,J)*A(O(P4),J  )*B(P3,J  )*C(O(P1),J)*
     &              S(2,1, P2 )*FCHIM(P4,P3,4,3)
C--LLE AND LQD CHARGINO 2
            ELSEIF(METYPE.EQ.6) THEN
C--FIRST DIAGRAM
               IF(P2.EQ.O(P3)) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              +PROPS(1,J)*A(P4,J  )*B(O(P1),J  )*C(O(P2),J  )*
     &              S(2,3,P2)*FCHI(P1,P4,1,4)
C--SECOND DIAGRAM
               IF(P1.EQ.O(P3)) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              +PROPS(2,J)*A(P4,J+2)*B(O(P2),J+2)*C(P3,J+2)*
     &              S(1,3,P1)*FCHI(P2,P4,2,4)
C--LLE AND LQD CHARGINO 3
            ELSEIF(METYPE.EQ.7) THEN
C--FIRST DIAGRAM
               IF(P2.EQ.O(P3)) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              +PROPS(1,J)*A(O(P4),J  )*B(O(P1),J  )*C(P3,J  )*
     &              S(2,3,P2)*FCHIM(P4,O(P1),4,1)
C--SECOND DIAGRAM
               IF(P1.EQ.O(P3)) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              +PROPS(2,J)*A(O(P4),J+2)*B(O(P2),J+2)*C(P3,J+2)*
     &              S(1,3,P1)*FCHIM(P4,O(P2),4,2)
C--UDD CHARGINO 1
            ELSEIF(METYPE.EQ.8) THEN
C--FIRST DIAGRAM
               IF(P2.EQ.P3) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              +PROPS(1,J)*A(P4,J  )*B(O(P1),J  )*C(O(P3),J  )*
     &              S(2,3,P2)*FCHI(P1,P4,1,4)
C--SECOND DIAGRAM
               IF(P1.EQ.P3) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              +PROPS(2,J)*A(P4,J+2)*B(O(P2),J+2)*C(O(P3),J+2)*
     &              S(1,3,P1)*FCHI(P2,P4,2,4)
C--UDD CHARGINO 2
            ELSEIF(METYPE.EQ.9) THEN
C--FIRST DIAGRAM
               IF(P2.EQ.P3) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              -(PROPS(1,J)*A(O(P4),J  )*B(O(P1),J  )*C(O(P2),J  )*
     &              S(3,2,P3)*FCHIM(P4,O(P1),4,1))
C--SECOND DIAGRAM
               IF(P1.EQ.P3) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              -(PROPS(2,J)*A(O(P4),J+2)*B(O(P2),J+2)*C(O(P1),J+2)*
     &              S(3,1,P3)*FCHIM(P4,O(P2),4,2))
C--THIRD DIAGRAM
               IF(P1.EQ.P2) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              -(PROPS(3,J)*A(O(P4),J+4)*B(O(P3),J+4)*C(O(P2),J+4)*
     &              S(1,2,P1)*FCHIM(P4,O(P3),4,3))
C--LLE AND LQD CHARGINO 4
            ELSEIF(METYPE.EQ.10) THEN
               IF(P1.EQ.P2) ME(P1,P2,P3,P4)=ME(P1,P2,P3,P4)
     &              +PROPS(1,J)*A(P4,J)*B(O(P3),J)*C(P1,J)*
     &              S(1,2,O(P2))*FCHI(P3,P4,3,4)
C--LLE AND LQD CHARGINO 5
            ELSEIF(METYPE.EQ.11) THEN
C--FIRST DIAGRAM
               IF(P2.EQ.O(P3)) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              -(PROPS(1,J)*A(O(P4),J  )*B(P1,J  )*C(P2,J  )*
     &              S(3,2,P3)*FCHIM(P4,P1,4,1))
C--SECOND DIAGRAM
               IF(P1.EQ.O(P3)) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              -(PROPS(2,J)*A(O(P4),J+2)*B(P2,J+2)*C(P1,J+2)*
     &              S(3,1,P3)*FCHIM(P4,P2,4,2))
C--LLE AND LQD CHARGINO 6
            ELSEIF(METYPE.EQ.12) THEN
C--FIRST DIAGRAM
               IF(P2.EQ.O(P3)) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              +PROPS(1,J)*A(P4,J  )*B(P1,J  )*C(P2,J  )*
     &              S(3,2,O(P2))*FCHI(O(P1),P4,1,4)
C--SECOND DIAGRAM
               IF(P1.EQ.O(P3)) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              +PROPS(2,J)*A(P4,J+2)*B(P2,J+2)*C(P1,J+2)*
     &              S(3,1,O(P1))*FCHI(O(P2),P4,2,4)
C--UDD CHARGINO 3
            ELSEIF(METYPE.EQ.13) THEN
C--FIRST DIAGRAM
               IF(P2.EQ.P3) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              -(PROPS(1,J)*A(O(P4),J  )*B(P1,J  )*C(P2,J  )*
     &              FCHIM(P4,P4,4,1)*S(3,2,O(P2)))
C--SECOND DIAGRAM
               IF(P1.EQ.P3) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              -(PROPS(2,J)*A(O(P4),J+2)*B(P2,J+2)*C(P1,J+2)*
     &              FCHIM(P4,P2,4,2)*S(3,1,O(P3)))
C--UDD CHARGINO 4
            ELSEIF(METYPE.EQ.14) THEN
C--FIRST DIAGRAM
               IF(P2.EQ.P3) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              +PROPS(1,J)*A(P4,J  )*B(P1,J  )*C(P3,J  )*
     &              S(2,3,O(P2))*FCHI(O(P1),P4,1,4)
C--SECOND DIAGRAM
               IF(P1.EQ.P3) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              +PROPS(2,J)*A(P4,J+2)*B(P2,J+2)*C(P3,J+2)*
     &              S(1,3,O(P1))*FCHI(O(P2),P4,2,4)
C--THIRD DIAGRAM
               IF(P1.EQ.P2) ME(P1,P2,P3,P4) = ME(P1,P2,P3,P4)
     &              +PROPS(3,J)*A(P4,J+4)*B(P3,J+4)*C(P1,J+4)*
     &              S(2,1,O(P2))*FCHI(O(P3),P4,3,4)
            ENDIF
         ENDDO
 100  CONTINUE
C-- combine to get the matrix element squared
      MESQ=0.0D0
      DO 200 P1=1,2
      DO 200 P2=1,2
      DO 200 P3=1,2
      DO 200 P4=1,2
 200  MESQ=MESQ+ME(P1,P2,P3,P4)*DCONJG(ME(P1,P2,P3,P4))
C--identical particle symmetry factor
      IF(ID1.EQ.ID2.OR.ID1.EQ.ID3.OR.ID2.EQ.ID3) WGT=0.5D0*WGT
C--final piece of the weight
      WGT = WGT*DBLE(MESQ)*COLFAC
C--if generating decays divide by maximum
      IF(GEN) WGT=WGT/MAXWGT
      END
C-----------------------------------------------------------------------
      SUBROUTINE HWDRPD(IHEP,MHEP,LHEP)
C-----------------------------------------------------------------------
C     four body rpv decay generator
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      INTEGER IHEP,MHEP,LHEP,ID1,ID2,ID3,ID4,IDIN,NPOINT,NTRY,I
      PARAMETER(NPOINT=1000)
      DOUBLE PRECISION WGT,POUT(5,4),HWRGEN
      EXTERNAL HWRGEN
c--ID'S OF THE PARTICLES
      IDIN = IDHW(IHEP)
      ID1  = IDHW(MHEP)
      ID2  = IDHW(MHEP+1)
      ID3  = IDHW(MHEP+2)
      ID4  = IDHW(LHEP)
c--GENERATE THE DECAY MODE
      NTRY = 0 
 10   CALL HWDRPM(WGT,IDIN,PHEP(1,IHEP),ID1,ID2,ID3,ID4,POUT,.TRUE.)
      IF(WGT.GT.HWRGEN(0)) then
         DO I=1,4
            CALL HWVEQU(5,POUT(1,I),PHEP(1,MHEP-1+I))
         ENDDO
      ELSEIF(NTRY.LT.NPOINT) THEN
         NTRY=NTRY+1
         GOTO 10
      ELSE
         CALL HWWARN('HWDRPD',100)
         GOTO 999
      ENDIF
 999  END
C-----------------------------------------------------------------------
      SUBROUTINE HWDRAD(IDIN,ID1,ID2,ID3,ID4,BR)
C-----------------------------------------------------------------------
C     ADD A FOUR BODY STAU DECAY TO THE DECAY TABLES
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      INTEGER IDIN,ID1,ID2,ID3,ID4
      DOUBLE PRECISION BR
      IDK(NDKYS+1) = IDIN
      NME(NDKYS+1) = 1000
      IDKPRD(1,NDKYS+1) = ID1
      IDKPRD(2,NDKYS+1) = ID2
      IDKPRD(3,NDKYS+1) = ID3
      IDKPRD(4,NDKYS+1) = ID4
      IDKPRD(5,NDKYS+1) = 0
      BRFRAC(NDKYS+1)   = BR
      NDKYS=NDKYS+1
      END
C-----------------------------------------------------------------------
      SUBROUTINE HWDRMX(IDIN,ID1,ID2,ID3,ID4,MAXWGT,WIDTH)
C-----------------------------------------------------------------------
C     PERFORM THE INITIAL SEARCH FOR THE MAXIMUM WEIGHT AND CALCULATION
c     OF THE PARTIAL WIDTH FOR A FOUR BODY RPV STAU DECAY
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      INTEGER IDIN,ID1,ID2,ID3,ID4,NPOINT,I
      DOUBLE PRECISION WGTSM,WGTSQ,WGT,PIN(5),MAXWGT,WIDTH,POUT(5,4)
      PARAMETER(NPOINT=10000)
C--Output the mode being calculated
      IF(IPRINT.EQ.2) WRITE(*,10) RNAME(IDIN),RNAME(ID1),RNAME(ID2),
     &     RNAME(ID3),RNAME(ID4)
C--momentum of the decaying particle
      DO I=1,3
         PIN(I) = 0.0D0
      ENDDO
      PIN(4) = RMASS(IDIN)
      PIN(5) = RMASS(IDIN)
C--zero sum and sum squared
      WGTSM = 0.0D0
      WGTSQ = 0.0D0
      DO I=1,NPOINT
         CALL HWDRPM(WGT,IDIN,PIN,ID1,ID2,ID3,ID4,POUT,.FALSE.)
         MAXWGT=MAX(MAXWGT,WGT)
         WGTSM=WGTSM+WGT
         WGTSQ=WGTSQ+WGT**2
      ENDDO
      WGTSM=WGTSM/DBLE(NPOINT)
      WGTSQ=MAX(0.,WGTSQ/DBLE(NPOINT)-WGTSM**2)
      WGTSQ = SQRT(WGTSQ/DBLE(NPOINT))
      IF(IPRINT.EQ.2) WRITE(*,20) WGTSM,WGTSQ
      WIDTH=WIDTH+WGTSM
      RETURN
C--format statements for the output
 10   FORMAT('CALCULATING DECAY RATE FOR ',A8,' --> ',
     &     A8,' ',A8,' ',A8,' ',A8)
 20   FORMAT('PARTIAL WIDTH IS ',5E15.5,' +/- ',5E15.5)
      END
C-----------------------------------------------------------------------
      SUBROUTINE HWDRTC(IDIN,CHRINO,A)
C-----------------------------------------------------------------------
C     CALCULATE THE COUPLINGS FOR THE STAU COUPLING FOR RPV 4 BODY STAU
C     DECAYS
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      INTEGER IDIN,J
      DOUBLE PRECISION A(2,6)
      LOGICAL CHRINO
C--common block for the couplings
      DOUBLE PRECISION SW,CW,TW,E,G,RT,ORT,MW,MZ,AFN(2,12,2,4),
     &     AFG(2,6,2),AFC(2,12,2,2),OIJ(2,4,2),OIJP(2,2,2),OIJPP(2,4,4),
     &     HNN(2,3,4,4),HCC(2,3,2,2),HNC(2,4,2),HFF(2,4,12),HWW(2),
     &     HZZ(2),ZAB(12,2,2),HHB(2,3),GS
      COMMON /HWSPNC/ SW,CW,TW,E,G,RT,ORT,MW,MZ,GS,AFN,AFG,AFC,OIJ,OIJP,
     &               OIJPP,HNN,HCC,HNC,HFF,HWW,HZZ,ZAB,HHB
C--neutralino case
      IF(.NOT.CHRINO) THEN 
         IF(IDIN.EQ.435) THEN
            DO J=1,6
               A(1,J) =  AFN(2,11,1,1)
               A(2,J) =  AFN(1,11,1,1)
            ENDDO
         ELSE
            DO J=1,6
               A(1,J) =  AFN(1,11,1,1)
               A(2,J) =  AFN(2,11,1,1)
            ENDDO
         ENDIF
C--chargino case
      ELSE
         IF(IDIN.EQ.435) THEN
            DO J=1,6
               A(1,J) =  AFC(2,11,1,1)
               A(2,J) =  AFC(1,11,1,1)
            ENDDO
         ELSE
            DO J=1,6
               A(1,J) =  AFC(1,11,1,1)
               A(2,J) =  AFC(2,11,1,1)
            ENDDO
         ENDIF
      ENDIF
      END
C-----------------------------------------------------------------------
      SUBROUTINE HWDRN1(ID1,ID2,ID3,ID4,B,C,METYPE,IDINT,MAXWGT)
C-----------------------------------------------------------------------
C     CALCULATE THE COUPLINGS FOR THE NEUTRALINO PART OF A FOUR BODY
C     STAU DECAY FOR LLE COUPLINGS
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      INTEGER ID1,ID2,ID3,ID4,J,K,METYPE,III,JJJ,KKK,O(2),IDINT(6)
      DOUBLE PRECISION B(2,6),C(2,6),MAXWGT
      DATA O/2,1/
C--common block for the couplings
      DOUBLE PRECISION SW,CW,TW,E,G,RT,ORT,MW,MZ,AFN(2,12,2,4),
     &     AFG(2,6,2),AFC(2,12,2,2),OIJ(2,4,2),OIJP(2,2,2),OIJPP(2,4,4),
     &     HNN(2,3,4,4),HCC(2,3,2,2),HNC(2,4,2),HFF(2,4,12),HWW(2),
     &     HZZ(2),ZAB(12,2,2),HHB(2,3),GS
      COMMON /HWSPNC/ SW,CW,TW,E,G,RT,ORT,MW,MZ,GS,AFN,AFG,AFC,OIJ,OIJP,
     &               OIJPP,HNN,HCC,HNC,HFF,HWW,HZZ,ZAB,HHB
C--common block for the parameters for the new modes
      DOUBLE PRECISION LLEMAX,LLEAVR,LQDMAX,LQDAVR,UDDMAX,UDDAVR
      COMMON /HWDRP4/ LLEMAX(5,3,3,3),LLEAVR(5,3,3,3),
     &                LQDMAX(8,3,3,3),LQDAVR(8,3,3,3),
     &                UDDMAX(4,3,3,3),UDDAVR(4,3,3,3)
c-- chi^0_1 -> l_i- nu_j e_k+
      IF(IDPDG(ID1).GT.0) THEN
         METYPE=1
C--indices for the couplings
         III = (ID1-119)/2
         JJJ = (ID2-120)/2
         KKK = (ID3-125)/2
         DO J=1,2
            DO K=1,2
C--RPC neutralino decay couplings
               B(K,J  ) = AFN(  K ,5+2*III,J,1)
               B(K,J+2) = AFN(  K ,6+2*JJJ,J,1)
               B(K,J+4) = AFN(O(K),5+2*KKK,J,1)
            ENDDO
C--the RPV couplings
            C(1,J  ) = LMIXSS(2*III-1,1,J)*LAMDA1(III,JJJ,KKK)
            C(2,J  ) = 0.0D0
            C(1,J+2) = LAMDA1(III,JJJ,KKK)
            C(2,J+2) = 0.0D0
            C(1,J+4) = LMIXSS(2*KKK-1,2,J)*LAMDA1(III,JJJ,KKK)
            C(2,J+4) = 0.0D0
C--the intermediate particles
            IDINT(J  ) = 423+2*III+(J-1)*12
            IDINT(J+2) = 424+2*JJJ+(J-1)*12
            IDINT(J+4) = 423+2*KKK+(J-1)*12
         ENDDO
         C(1,4) = 0.0D0
C--set the maximum weight
         IF(ID4.EQ.125) THEN
            MAXWGT = LLEMAX(2,III,JJJ,KKK)
         ELSE
            MAXWGT = LLEMAX(1,III,JJJ,KKK)
         ENDIF
      ELSE
c-- chi^0_1 -> l_i+ nubar_j e_k-
         METYPE=3
C--indices for the couplings
         III = (ID1-125)/2
         JJJ = (ID2-126)/2
         KKK = (ID3-119)/2
C--   RPC neutralino decay couplings
         DO J=1,2
            DO K=1,2
               B(K,J  ) = AFN(O(K),5+2*III,J,1)
               B(K,J+2) = AFN(O(K),6+2*JJJ,J,1)
               B(K,J+4) = AFN(  K ,5+2*KKK,J,1)
            ENDDO
C--the RPV couplings
            C(1,J  ) = 0.0D0
            C(2,J  ) =  LMIXSS(2*III-1,1,J)*LAMDA1(III,JJJ,KKK)
            C(1,J+2) = 0.0D0
            C(2,J+2) =  LAMDA1(III,JJJ,KKK)
            C(1,J+4) = 0.0D0
            C(2,J+4) =  LMIXSS(2*KKK-1,2,J)*LAMDA1(III,JJJ,KKK)
C--the intermediate particles
            IDINT(J  ) = 423+2*III+(J-1)*12
            IDINT(J+2) = 424+2*JJJ+(J-1)*12
            IDINT(J+4) = 423+2*KKK+(J-1)*12
         ENDDO
         C(2,4) = 0.0D0
C--set the maximum weight
         IF(ID4.EQ.125) THEN
            MAXWGT = LLEMAX(1,III,JJJ,KKK)
         ELSE
            MAXWGT = LLEMAX(2,III,JJJ,KKK)
         ENDIF
      ENDIF
      END
C-----------------------------------------------------------------------
      SUBROUTINE HWDRN2(ID1,ID2,ID3,ID4,B,C,METYPE,IDINT,MAXWGT)
C-----------------------------------------------------------------------
C     CALCULATE THE COUPLINGS FOR THE NEUTRALINO PART OF A FOUR BODY
C     STAU DECAY FOR LQD COUPLINGS
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      INTEGER ID1,ID2,ID3,ID4,J,K,METYPE,III,JJJ,KKK,O(2),IDINT(6)
      DOUBLE PRECISION B(2,6),C(2,6),MAXWGT
      DATA O/2,1/
C--common block for the couplings
      DOUBLE PRECISION SW,CW,TW,E,G,RT,ORT,MW,MZ,AFN(2,12,2,4),
     &     AFG(2,6,2),AFC(2,12,2,2),OIJ(2,4,2),OIJP(2,2,2),OIJPP(2,4,4),
     &     HNN(2,3,4,4),HCC(2,3,2,2),HNC(2,4,2),HFF(2,4,12),HWW(2),
     &     HZZ(2),ZAB(12,2,2),HHB(2,3),GS
      COMMON /HWSPNC/ SW,CW,TW,E,G,RT,ORT,MW,MZ,GS,AFN,AFG,AFC,OIJ,OIJP,
     &               OIJPP,HNN,HCC,HNC,HFF,HWW,HZZ,ZAB,HHB
C--common block for the parameters for the new modes
      DOUBLE PRECISION LLEMAX,LLEAVR,LQDMAX,LQDAVR,UDDMAX,UDDAVR
      COMMON /HWDRP4/ LLEMAX(5,3,3,3),LLEAVR(5,3,3,3),
     &                LQDMAX(8,3,3,3),LQDAVR(8,3,3,3),
     &                UDDMAX(4,3,3,3),UDDAVR(4,3,3,3)
      IF(IDPDG(ID1).GT.0) THEN
         METYPE=1
C--chi^0_1 -> nu_i d_j dbar_k
         IF(MOD(ID1,2).EQ.0) THEN
C--indices for the couplings
            III = (ID1-120)/2
            JJJ = (ID2+1)/2
            KKK = (ID3-5)/2
            DO J=1,2
               DO K=1,2
C--RPC neutralino decay couplings
C--first diagram
                  B(K,J  ) = AFN(  K ,6+2*III,J,1)
C--second diagram
                  B(K,J+2) = AFN(  K ,2*JJJ-1,J,1)
C--third diagram
                  B(K,J+4) = AFN(O(K),2*KKK-1,J,1)
               ENDDO
C--the RPV couplings
               C(1,J  ) =-LAMDA2(III,JJJ,KKK)
               C(2,J  ) = 0.0D0
               C(1,J+2) =(-QMIXSS(2*JJJ-1,1,J))*LAMDA2(III,JJJ,KKK)
               C(2,J+2) = 0.0D0
               C(1,J+4) =(-QMIXSS(2*KKK-1,2,J))*LAMDA2(III,JJJ,KKK) 
               C(2,J+4) = 0.0D0
C--the intermediate particles
               IDINT(J  ) = 424+2*III+(J-1)*12
               IDINT(J+2) = 399+2*JJJ+(J-1)*12
               IDINT(J+4) = 399+2*KKK+(J-1)*12
            ENDDO
            C(1,2) = 0.0D0
C--set the maximum weight
            IF(ID4.EQ.125) THEN
               MAXWGT = LQDMAX(1,III,JJJ,KKK)
            ELSE
               MAXWGT = LQDMAX(3,III,JJJ,KKK)
            ENDIF
C--chi^0_1 -> l_i- u_j dbar_k
         ELSE
C--indices for the couplings
            III = (ID1-119)/2
            JJJ =  ID2/2
            KKK = (ID3-5)/2
            DO J=1,2
               DO K=1,2
C--   RPC neutralino decay couplings
C--   first diagram
                  B(K,J  ) = AFN(  K ,2*III+5,J,1)
C--   second diagram
                  B(K,J+2) = AFN(  K ,2*JJJ  ,J,1)
C--   third diagram
                  B(K,J+4) = AFN(O(K),2*KKK-1,J,1)
               ENDDO
C--   the RPV couplings
               C(1,J  ) =LMIXSS(2*III-1,1,J)*LAMDA2(III,JJJ,KKK)
               C(2,J  ) = 0.0D0
               C(1,J+2) =QMIXSS(2*JJJ,1,J)*LAMDA2(III,JJJ,KKK)
               C(2,J+2) = 0.0D0
               C(1,J+4) =QMIXSS(2*KKK-1,2,J)*LAMDA2(III,JJJ,KKK)
               C(2,J+4) = 0.0D0
C--the intermediate particles
               IDINT(J  ) = 423+2*III+(J-1)*12
               IDINT(J+2) = 400+2*JJJ+(J-1)*12
               IDINT(J+4) = 399+2*KKK+(J-1)*12
            ENDDO
C--maximum weight
            IF(ID4.EQ.125) THEN
               MAXWGT = LQDMAX(2,III,JJJ,KKK)
            ELSE
               MAXWGT = LQDMAX(4,III,JJJ,KKK)
            ENDIF
         ENDIF
      ELSE
         METYPE=3
C--chi^0_1 -> nu_i d_j dbar_k
         IF(MOD(ID1,2).EQ.0) THEN
C--indices for the couplings
            III = (ID1-126)/2
            JJJ = (ID2-5)/2
            KKK = (ID3+1)/2
C--   RPC neutralino decay couplings
            DO J=1,2
               DO K=1,2
C--first diagram
                  B(K,J  ) = AFN(O(K),6+2*III,J,1)
C--second diagram
                  B(K,J+2) = AFN(O(K),2*JJJ-1,J,1)
C--third diagram
                  B(K,J+4) = AFN(  K ,2*KKK-1,J,1)
               ENDDO
C--the RPV couplings
               C(1,J  ) = 0.0D0
               C(2,J  ) =-LAMDA2(III,JJJ,KKK)
               C(1,J+2) = 0.0D0
               C(2,J+2) =(-QMIXSS(2*JJJ-1,1,J))*LAMDA2(III,JJJ,KKK)
               C(1,J+4) = 0.0D0
               C(2,J+4) =(-QMIXSS(2*KKK-1,2,J))*LAMDA2(III,JJJ,KKK)
C--the intermediate particles
               IDINT(J  ) = 424+2*III+(J-1)*12
               IDINT(J+2) = 399+2*JJJ+(J-1)*12
               IDINT(J+4) = 399+2*KKK+(J-1)*12
            ENDDO
            C(2,2) = 0.0D0
C--maximum weight
            IF(ID4.EQ.125) THEN
               MAXWGT = LQDMAX(3,III,JJJ,KKK)
            ELSE
               MAXWGT = LQDMAX(1,III,JJJ,KKK)
            ENDIF
C--chi^0_1 -> l_i- u_j dbar_k
         ELSE
C--indices for the couplings
            III = (ID1-125)/2
            JJJ = (ID2-6)/2
            KKK = (ID3+1)/2
C--RPC neutralino decay couplings
            DO J=1,2
               DO K=1,2
C--first diagram
                  B(K,J  ) = AFN(O(K),2*III+5,J,1)
C--second diagram
                  B(K,J+2) = AFN(O(K),2*JJJ  ,J,1)
C--   third diagram
                  B(K,J+4) = AFN(  K ,2*KKK-1,J,1)
               ENDDO
C--the RPV couplings
               C(1,J  ) = 0.0D0
               C(2,J  ) = LMIXSS(2*III-1,1,J)*LAMDA2(III,JJJ,KKK)
               C(1,J+2) = 0.0D0
               C(2,J+2) = QMIXSS(2*JJJ,1,J)*LAMDA2(III,JJJ,KKK)
               C(1,J+4) = 0.0D0
               C(2,J+4) = QMIXSS(2*KKK-1,2,J)*LAMDA2(III,JJJ,KKK)
C--the intermediate particles
               IDINT(J  ) = 423+2*III+(J-1)*12
               IDINT(J+2) = 400+2*JJJ+(J-1)*12
               IDINT(J+4) = 399+2*KKK+(J-1)*12
            ENDDO
C--maximum weight
            IF(ID4.EQ.125) THEN
               MAXWGT = LQDMAX(4,III,JJJ,KKK)
            ELSE
               MAXWGT = LQDMAX(2,III,JJJ,KKK)
            ENDIF
         ENDIF
      ENDIF
      END
C-----------------------------------------------------------------------
      SUBROUTINE HWDRN3(ID1,ID2,ID3,ID4,B,C,METYPE,IDINT,MAXWGT)
C-----------------------------------------------------------------------
C     CALCULATE THE COUPLINGS FOR THE NEUTRALINO PART OF A FOUR BODY
C     STAU DECAY FOR UDD COUPLINGS
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      INTEGER ID1,ID2,ID3,ID4,J,K,METYPE,III,JJJ,KKK,O(2),IDINT(6)
      DOUBLE PRECISION B(2,6),C(2,6),MAXWGT
      DATA O/2,1/
C--common block for the couplings
      DOUBLE PRECISION SW,CW,TW,E,G,RT,ORT,MW,MZ,AFN(2,12,2,4),
     &     AFG(2,6,2),AFC(2,12,2,2),OIJ(2,4,2),OIJP(2,2,2),OIJPP(2,4,4),
     &     HNN(2,3,4,4),HCC(2,3,2,2),HNC(2,4,2),HFF(2,4,12),HWW(2),
     &     HZZ(2),ZAB(12,2,2),HHB(2,3),GS
      COMMON /HWSPNC/ SW,CW,TW,E,G,RT,ORT,MW,MZ,GS,AFN,AFG,AFC,OIJ,OIJP,
     &               OIJPP,HNN,HCC,HNC,HFF,HWW,HZZ,ZAB,HHB
C--common block for the parameters for the new modes
      DOUBLE PRECISION LLEMAX,LLEAVR,LQDMAX,LQDAVR,UDDMAX,UDDAVR
      COMMON /HWDRP4/ LLEMAX(5,3,3,3),LLEAVR(5,3,3,3),
     &                LQDMAX(8,3,3,3),LQDAVR(8,3,3,3),
     &                UDDMAX(4,3,3,3),UDDAVR(4,3,3,3)
C--chi^0_1 -> u_i d_j d_k
      IF(IDPDG(ID1).GT.0) THEN
         METYPE=2
C--indices for the rpv couplings
         III =  ID1   /2
         JJJ = (ID2+1)/2
         KKK = (ID3+1)/2
c-- RPC neutralino decay couplings
         DO J=1,2
            DO K=1,2
               B(K,J  ) = AFN(  K ,2*III  ,J,1)
               B(K,J+2) = AFN(  K ,2*JJJ-1,J,1)
               B(K,J+4) = AFN(  K ,2*KKK-1,J,1)
            ENDDO
C--   the RPV couplings
            C(1,J  ) = 0.0D0
            C(2,J  ) = QMIXSS(2*III  ,2,J)*LAMDA3(III,JJJ,KKK)
            C(1,J+2) = 0.0D0
            C(2,J+2) = QMIXSS(2*JJJ-1,2,J)*LAMDA3(III,JJJ,KKK)
            C(1,J+4) = 0.0D0
            C(2,J+4) = QMIXSS(2*KKK-1,2,J)*LAMDA3(III,JJJ,KKK)
C--the intermediate particles
            IDINT(J  ) = 400+2*III+(J-1)*12
            IDINT(J+2) = 399+2*JJJ+(J-1)*12
            IDINT(J+4) = 399+2*KKK+(J-1)*12
         ENDDO
C--maximum weight
         IF(ID4.EQ.125) THEN
            MAXWGT=UDDMAX(1,III,JJJ,KKK)
         ELSE
            MAXWGT=UDDMAX(2,III,JJJ,KKK)
         ENDIF
C--chi^0_1 -> ubar_i dbar_j dbar_k
      ELSE
         METYPE=4
C--indices for the rpv couplings
         III = (ID1-6)/2
         JJJ = (ID2-5)/2
         KKK = (ID3-5)/2
c-- RPC neutralino decay couplings
         DO J=1,2
            DO K=1,2
               B(K,J  ) = AFN(O(K),2*III  ,J,1)
               B(K,J+2) = AFN(O(K),2*JJJ-1,J,1)
               B(K,J+4) = AFN(O(K),2*KKK-1,J,1)
            ENDDO
C--   the RPV couplings
            C(1,J  ) = QMIXSS(2*III  ,2,J)*LAMDA3(III,JJJ,KKK)
            C(2,J  ) = 0.0D0
            C(1,J+2) = QMIXSS(2*JJJ-1,2,J)*LAMDA3(III,JJJ,KKK)
            C(2,J+2) = 0.0D0
            C(1,J+4) = QMIXSS(2*KKK-1,2,J)*LAMDA3(III,JJJ,KKK)
            C(2,J+4) = 0.0D0
C--the intermediate particles
            IDINT(J  ) = 400+2*III+(J-1)*12
            IDINT(J+2) = 399+2*JJJ+(J-1)*12
            IDINT(J+4) = 399+2*KKK+(J-1)*12
         ENDDO
C--maximum weight
         IF(ID4.EQ.125) THEN
            MAXWGT=UDDMAX(2,III,JJJ,KKK)
         ELSE
            MAXWGT=UDDMAX(1,III,JJJ,KKK)
         ENDIF
      ENDIF
      END
C-----------------------------------------------------------------------
      SUBROUTINE HWDRC1(ID1,ID2,ID3,ID4,B,C,METYPE,IDINT,MAXWGT)
C-----------------------------------------------------------------------
C     CALCULATE THE COUPLINGS FOR THE CHARGINO PART OF A FOUR BODY
C     STAU DECAY FOR LLE COUPLINGS
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      INTEGER ID1,ID2,ID3,ID4,J,K,METYPE,III,JJJ,KKK,O(2),IDINT(6)
      DOUBLE PRECISION B(2,6),C(2,6),MAXWGT
      DATA O/2,1/
C--common block for the couplings
      DOUBLE PRECISION SW,CW,TW,E,G,RT,ORT,MW,MZ,AFN(2,12,2,4),
     &     AFG(2,6,2),AFC(2,12,2,2),OIJ(2,4,2),OIJP(2,2,2),OIJPP(2,4,4),
     &     HNN(2,3,4,4),HCC(2,3,2,2),HNC(2,4,2),HFF(2,4,12),HWW(2),
     &     HZZ(2),ZAB(12,2,2),HHB(2,3),GS
      COMMON /HWSPNC/ SW,CW,TW,E,G,RT,ORT,MW,MZ,GS,AFN,AFG,AFC,OIJ,OIJP,
     &               OIJPP,HNN,HCC,HNC,HFF,HWW,HZZ,ZAB,HHB
C--common block for the parameters for the new modes
      DOUBLE PRECISION LLEMAX,LLEAVR,LQDMAX,LQDAVR,UDDMAX,UDDAVR
      COMMON /HWDRP4/ LLEMAX(5,3,3,3),LLEAVR(5,3,3,3),
     &                LQDMAX(8,3,3,3),LQDAVR(8,3,3,3),
     &                UDDMAX(4,3,3,3),UDDAVR(4,3,3,3)
C--first mode
      IF(MOD(ID1,2).EQ.0.AND.MOD(ID3,2).EQ.0) THEN
C--nu_i l_j- nubar_k
         IF(IDPDG(ID1).GT.0) THEN
            METYPE=5
C--indices for the couplings
            III = (ID1-120)/2
            JJJ = (ID2-119)/2
            KKK = (ID3-126)/2
            DO J=1,2
C--RPC chargino decay couplings
               DO K=1,2
                  B(K,J) = AFC(O(K),5+2*KKK,J,1)
               ENDDO
C--RPV coupling
               C(1,J)=LAMDA1(III,JJJ,KKK)*LMIXSS(2*KKK-1,2,J)
               C(2,J)=0.0D0
C--the intermediate particles
               IDINT(J) = 423+2*KKK+(J-1)*12
            ENDDO
C--maximum weight
            WGTMAX=LLEMAX(3,III,JJJ,KKK)
C--nubar_i l_j+ nu_k
         ELSE
            METYPE=10
C--indices for the couplings
            III = (ID1-126)/2
            JJJ = (ID2-125)/2
            KKK = (ID3-120)/2
C--   RPC chargino decay couplings
            DO J=1,2
               DO K=1,2
                  B(K,J) = AFC(  K ,5+2*KKK,J,1)
               ENDDO
C--   RPV coupling
               C(2,J)=LAMDA1(III,JJJ,KKK)*LMIXSS(2*KKK-1,2,J)
               C(1,J)=0.0D0
C--the intermediate particles
               IDINT(J) = 423+2*KKK+(J-1)*12
            ENDDO
C--maximum weight
            WGTMAX=LLEMAX(3,III,JJJ,KKK)
         ENDIF
C--second mode
      ELSEIF(MOD(ID1,2).EQ.0.AND.MOD(ID2,2).EQ.0) THEN
C--nu_i nu_j e_k+
         IF(IDPDG(ID1).GT.0) THEN
            METYPE=6
C--indices for the couplings
            III = (ID1-120)/2
            JJJ = (ID2-120)/2
            KKK = (ID3-125)/2
            DO J=1,2
C--RPC chargino decay couplings
               DO K=1,2
                  B(K,J  ) = AFC(  K ,5+2*III,J,1)
                  B(K,J+2) = AFC(  K ,5+2*JJJ,J,1)
               ENDDO
C--RPV coupling
               C(1,J  )=LAMDA1(III,JJJ,KKK)*LMIXSS(2*III-1,1,J)
               C(2,J  )=0.0D0
               C(1,J+2)=LAMDA1(III,JJJ,KKK)*LMIXSS(2*JJJ-1,1,J)
               C(2,J+2)=0.0D0
C--the intermediate particles
               IDINT(J  ) = 423+2*III+(J-1)*12
               IDINT(J+2) = 423+2*JJJ+(J-1)*12
            ENDDO
C--maximum weight
            MAXWGT = LLEMAX(4,III,JJJ,KKK)
C--nubar_i nubar_j e_k-
         ELSE
            METYPE=11
C--indices for the couplings
            III = (ID1-126)/2
            JJJ = (ID2-126)/2
            KKK = (ID3-119)/2
C--RPC chargino decay couplings
            DO J=1,2
               DO K=1,2
                  B(K,J  ) = AFC(O(K),5+2*III,J,1)
                  B(K,J+2) = AFC(O(K),5+2*JJJ,J,1)
               ENDDO
C--RPV coupling
               C(1,J  )=0.0D0
               C(2,J  )=LAMDA1(III,JJJ,KKK)*LMIXSS(2*III-1,1,J)
               C(1,J+2)=0.0D0
               C(2,J+2)=LAMDA1(III,JJJ,KKK)*LMIXSS(2*JJJ-1,1,J)
C--the intermediate particles
               IDINT(J  ) = 423+2*III+(J-1)*12
               IDINT(J+2) = 423+2*JJJ+(J-1)*12
            ENDDO
C--maximum weight
            MAXWGT = LLEMAX(4,III,JJJ,KKK)
         ENDIF
C--third mode
      ELSE
C--l_i- l_j- l_k+
         IF(IDPDG(ID1).GT.0) THEN
            METYPE=7
C--indices for the couplings
            III = (ID1-119)/2
            JJJ = (ID2-119)/2
            KKK = (ID3-125)/2
            DO J=1,2
C--RPC chargino decay couplings
               DO K=1,2
                  B(K,J  ) = AFC(  K ,6+2*III,J,1)
                  B(K,J+2) = AFC(  K ,6+2*JJJ,J,1)
               ENDDO
C--RPV coupling
               C(1,J  )=LAMDA1(III,JJJ,KKK)
               C(2,J  )=0.0D0
               C(1,J+2)=LAMDA1(III,JJJ,KKK)
               C(2,J+2)=0.0D0
C--the intermediate particles
               IDINT(J  ) = 424+2*III+(J-1)*12
               IDINT(J+2) = 424+2*JJJ+(J-1)*12
            ENDDO
            C(1,2)=0.0d0
            C(1,4)=0.0d0
C--maximum weight
            MAXWGT = LLEMAX(5,III,JJJ,KKK)
C--l_i- l_j- l_k+
         ELSE
            METYPE=12
C--indices for the couplings
            III = (ID1-125)/2
            JJJ = (ID2-125)/2
            KKK = (ID3-119)/2
C--RPC chargino decay couplings
            DO J=1,2
               DO K=1,2
                  B(K,J  ) = AFC(O(K),6+2*III,J,1)
                  B(K,J+2) = AFC(O(K),6+2*JJJ,J,1)
               ENDDO
C--RPV coupling
               C(1,J  )=0.0D0
               C(2,J  )=LAMDA1(III,JJJ,KKK)
               C(1,J+2)=0.0D0
               C(2,J+2)=LAMDA1(III,JJJ,KKK)
C--the intermediate particles
               IDINT(J  ) = 424+2*III+(J-1)*12
               IDINT(J+2) = 424+2*JJJ+(J-1)*12
            ENDDO
            C(2,2)=0.0d0
            C(2,4)=0.0d0
C--maximum weight
            MAXWGT = LLEMAX(5,III,JJJ,KKK)
         ENDIF
      ENDIF
      END
C-----------------------------------------------------------------------
      SUBROUTINE HWDRC2(ID1,ID2,ID3,ID4,B,C,METYPE,IDINT,MAXWGT)
C-----------------------------------------------------------------------
C     CALCULATE THE COUPLINGS FOR THE CHARGINO PART OF A FOUR BODY
C     STAU DECAY FOR LLE COUPLINGS
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      INTEGER ID1,ID2,ID3,ID4,J,K,METYPE,III,JJJ,KKK,O(2),IDINT(6)
      DOUBLE PRECISION B(2,6),C(2,6),MAXWGT
      DATA O/2,1/
C--common block for the couplings
      DOUBLE PRECISION SW,CW,TW,E,G,RT,ORT,MW,MZ,AFN(2,12,2,4),
     &     AFG(2,6,2),AFC(2,12,2,2),OIJ(2,4,2),OIJP(2,2,2),OIJPP(2,4,4),
     &     HNN(2,3,4,4),HCC(2,3,2,2),HNC(2,4,2),HFF(2,4,12),HWW(2),
     &     HZZ(2),ZAB(12,2,2),HHB(2,3),GS
      COMMON /HWSPNC/ SW,CW,TW,E,G,RT,ORT,MW,MZ,GS,AFN,AFG,AFC,OIJ,OIJP,
     &               OIJPP,HNN,HCC,HNC,HFF,HWW,HZZ,ZAB,HHB
C--common block for the parameters for the new modes
      DOUBLE PRECISION LLEMAX,LLEAVR,LQDMAX,LQDAVR,UDDMAX,UDDAVR
      COMMON /HWDRP4/ LLEMAX(5,3,3,3),LLEAVR(5,3,3,3),
     &                LQDMAX(8,3,3,3),LQDAVR(8,3,3,3),
     &                UDDMAX(4,3,3,3),UDDAVR(4,3,3,3)
C--first mode
      IF(MOD(ID1,2).EQ.1.AND.MOD(ID3,2).EQ.1) THEN
C--l_i- d_j dbar_k
         IF(IDPDG(ID1).GT.0) THEN
            METYPE=7
C--indices for the couplings
            III = (ID1-119)/2
            JJJ = (ID2+1)/2
            KKK = (ID3-5)/2
            DO J=1,2
C--RPC chargino decay couplings
               DO K=1,2
                  B(K,J  ) = AFC(  K ,6+2*III,J,1)
                  B(K,J+2) = AFC(  K ,  2*JJJ,J,1)
               ENDDO
C--RPV coupling
               C(1,J  )=LAMDA2(III,JJJ,KKK)
               C(2,J  )=0.0D0
               C(1,J+2)=LAMDA2(III,JJJ,KKK)*QMIXSS(2*JJJ,1,J)
               C(2,J+2)=0.0D0
C--the intermediate particles
               IDINT(J  ) = 424+2*III+(J-1)*12
               IDINT(J+2) = 400+2*JJJ+(J-1)*12
            ENDDO
            C(1,2)=0.0d0
C--maximum weight
            MAXWGT=LQDMAX(5,III,JJJ,KKK)
C--l_i+ dbar_j d_k
         ELSE
            METYPE=12
C--indices for the couplings
            III = (ID1-125)/2
            JJJ = (ID2-5)/2
            KKK = (ID3+1)/2
C--RPC chargino decay couplings
            DO J=1,2
               DO K=1,2
                  B(K,J  ) = AFC(O(K),6+2*III,J,1)
                  B(K,J+2) = AFC(O(K),2*JJJ  ,J,1)
               ENDDO
C--RPV coupling
               C(1,J  )=0.0D0
               C(2,J  )=LAMDA2(III,JJJ,KKK)
               C(1,J+2)=0.0D0
               C(2,J+2)=LAMDA2(III,JJJ,KKK)*QMIXSS(2*JJJ,1,J)
C--the intermediate particles
               IDINT(J  ) = 424+2*III+(J-1)*12
               IDINT(J+2) = 400+2*JJJ+(J-1)*12
            ENDDO
            C(2,2)=0.0d0
C--maximum weight
            MAXWGT=LQDMAX(5,III,JJJ,KKK)
         ENDIF
C--second mode
      ELSEIF(MOD(ID1,2).EQ.0.AND.MOD(ID3,2).EQ.1) THEN
C--nu_i u_j dbar_k
         IF(IDPDG(ID1).GT.0) THEN
            METYPE=6
C--indices for the couplings
            III = (ID1-120)/2
            JJJ = (ID2  )/2
            KKK = (ID3-5)/2
            DO J=1,2
C--RPC chargino decay couplings
               DO K=1,2
                  B(K,J  ) = AFC(  K ,5+2*III,J,1)
                  B(K,J+2) = AFC(  K ,2*JJJ-1,J,1)
               ENDDO
C--RPV coupling
               C(1,J  )=LAMDA2(III,JJJ,KKK)*LMIXSS(2*III-1,1,J)
               C(2,J  )=0.0D0
               C(1,J+2)=LAMDA2(III,JJJ,KKK)*QMIXSS(2*JJJ-1,1,J)
               C(2,J+2)=0.0D0
C--the intermediate particles
               IDINT(J  ) = 423+2*III+(J-1)*12
               IDINT(J+2) = 399+2*JJJ+(J-1)*12
            ENDDO
C--maximum weight
            MAXWGT=LQDMAX(6,III,JJJ,KKK)
C--nubar_i ubar_j d_k   
         ELSE
            METYPE=11
C--indices for the couplings
            III = (ID1-126)/2
            JJJ = (ID2-6)/2
            KKK = (ID3+1)/2
C--RPC chargino decay couplings
            DO J=1,2
               DO K=1,2
                  B(K,J  ) = AFC(O(K),5+2*III,J,1)
                  B(K,J+2) = AFC(O(K),2*JJJ-1,J,1)
               ENDDO
C--RPV coupling
               C(1,J  )=0.0D0
               C(2,J  )=LAMDA2(III,JJJ,KKK)*LMIXSS(2*III-1,1,J)
               C(1,J+2)=0.0D0
               C(2,J+2)=LAMDA2(III,JJJ,KKK)*QMIXSS(2*JJJ-1,1,J)
C--the intermediate particles
               IDINT(J  ) = 423+2*III+(J-1)*12
               IDINT(J+2) = 399+2*JJJ+(J-1)*12
            ENDDO
C--maximum weight
            MAXWGT=LQDMAX(6,III,JJJ,KKK)
         ENDIF
C--third mode
      ELSEIF(MOD(ID1,2).EQ.0.AND.MOD(ID3,2).EQ.0) THEN
C--nu_i d_j ubar_k
         IF(IDPDG(ID1).GT.0) THEN
            METYPE=5
C--indices for the couplings
            III = (ID1-120)/2
            JJJ = (ID2+1)/2
            KKK = (ID3-6)/2
            DO J=1,2
C--RPC chargino decay couplings
               DO K=1,2
                  B(K,J) = AFC(O(K),2*KKK-1,J,1)
               ENDDO
C--RPV coupling
               C(1,J)=LAMDA2(III,JJJ,KKK)*QMIXSS(2*KKK-1,2,J)
               C(2,J)=0.0D0
C--the intermediate particles
               IDINT(J) = 399+2*KKK+(J-1)*12
            ENDDO
C--maximum weight
            MAXWGT=LQDMAX(7,III,JJJ,KKK)
C--nubar_i dbar_j u_k
         ELSE
            METYPE=10
C--indices for the couplings
            III = (ID1-126)/2
            JJJ = (ID2-5)/2
            KKK = (ID3  )/2
C--   RPC chargino decay couplings
            DO J=1,2
               DO K=1,2
                  B(K,J) = AFC(  K ,2*KKK-1,J,1)
               ENDDO
C--   RPV coupling
               C(1,J)=0.0D0
               C(2,J)=LAMDA2(III,JJJ,KKK)*QMIXSS(2*KKK-1,2,J)
C--the intermediate particles
               IDINT(J) = 399+2*KKK+(J-1)*12
            ENDDO
C--maximum weight
            MAXWGT=LQDMAX(7,III,JJJ,KKK)
         ENDIF
C--fourth mode
      ELSE
C--l_i- u_j ubar_k
         IF(IDPDG(ID1).GT.0) THEN
            METYPE=5
C--indices for the couplings
            III = (ID1-119)/2
            JJJ = (ID2  )/2
            KKK = (ID3-6)/2
            DO J=1,2
C--RPC chargino decay couplings
               DO K=1,2
                  B(K,J) = AFC(O(K),2*KKK-1,J,1)
               ENDDO
C--RPV coupling
               C(1,J)=(-LAMDA2(III,JJJ,KKK))*QMIXSS(2*KKK-1,2,J)
               C(2,J)=0.0D0
C--the intermediate particles
               IDINT(J) = 399+2*KKK+(J-1)*12
            ENDDO
C--maximum weight
            MAXWGT=LQDMAX(8,III,JJJ,KKK)
C--l_i+ ubar_j u_k
         ELSE
            METYPE=10
C--indices for the couplings
            III = (ID1-125)/2
            JJJ = (ID2-6)/2
            KKK = (ID3  )/2
C--   RPC chargino decay couplings
            DO J=1,2
               DO K=1,2
                  B(K,J) = AFC(  K ,2*KKK-1,J,1)
               ENDDO
C--   RPV coupling
               C(1,J)=0.0D0
               C(2,J)=LAMDA2(III,JJJ,KKK)*QMIXSS(2*KKK-1,2,J)
C--the intermediate particles
               IDINT(J) = 399+2*KKK+(J-1)*12
            ENDDO
C--maximum weight
            MAXWGT=LQDMAX(8,III,JJJ,KKK)
         ENDIF
      ENDIF
      END
C-----------------------------------------------------------------------
      SUBROUTINE HWDRC3(ID1,ID2,ID3,ID4,B,C,METYPE,IDINT,MAXWGT)
C-----------------------------------------------------------------------
C     CALCULATE THE COUPLINGS FOR THE CHARGINO PART OF A FOUR BODY
C     STAU DECAY FOR LLE COUPLINGS
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      INTEGER ID1,ID2,ID3,ID4,J,K,METYPE,III,JJJ,KKK,O(2),IDINT(6)
      DOUBLE PRECISION B(2,6),C(2,6),MAXWGT
      DATA O/2,1/
C--common block for the couplings
      DOUBLE PRECISION SW,CW,TW,E,G,RT,ORT,MW,MZ,AFN(2,12,2,4),
     &     AFG(2,6,2),AFC(2,12,2,2),OIJ(2,4,2),OIJP(2,2,2),OIJPP(2,4,4),
     &     HNN(2,3,4,4),HCC(2,3,2,2),HNC(2,4,2),HFF(2,4,12),HWW(2),
     &     HZZ(2),ZAB(12,2,2),HHB(2,3),GS
      COMMON /HWSPNC/ SW,CW,TW,E,G,RT,ORT,MW,MZ,GS,AFN,AFG,AFC,OIJ,OIJP,
     &               OIJPP,HNN,HCC,HNC,HFF,HWW,HZZ,ZAB,HHB
C--common block for the parameters for the new modes
      DOUBLE PRECISION LLEMAX,LLEAVR,LQDMAX,LQDAVR,UDDMAX,UDDAVR
      COMMON /HWDRP4/ LLEMAX(5,3,3,3),LLEAVR(5,3,3,3),
     &                LQDMAX(8,3,3,3),LQDAVR(8,3,3,3),
     &                UDDMAX(4,3,3,3),UDDAVR(4,3,3,3)
C--first mode
      IF(MOD(ID1,2).EQ.0) THEN
C--u_i u_j d_k
         IF(IDPDG(ID1).GT.0) THEN
            METYPE=8
C--indices for the couplings
            III = (ID1  )/2
            JJJ = (ID2  )/2
            KKK = (ID3+1)/2
            DO J=1,2
C--RPC chargino decay couplings
               DO K=1,2
                  B(K,J  ) = AFC(  K ,2*III-1,J,1)
                  B(K,J+2) = AFC(  K ,2*JJJ-1,J,1)
               ENDDO
C--RPV coupling
               C(1,J  )=0.0D0
               C(2,J  )=LAMDA3(JJJ,KKK,III)*QMIXSS(2*III-1,2,J)
               C(1,J+2)=0.0D0
               C(2,J+2)=LAMDA3(III,JJJ,KKK)*QMIXSS(2*JJJ-1,2,J)
C--the intermediate particles
               IDINT(J  ) = 399+2*III+(J-1)*12
               IDINT(J+2) = 399+2*JJJ+(J-1)*12
            ENDDO
C--maximum weight
            MAXWGT=UDDMAX(3,III,JJJ,KKK)
C--ubar_i ubar_j dbar_k
         ELSE
            METYPE=13
C--indices for the couplings
            III = (ID1-6)/2
            JJJ = (ID2-6)/2
            KKK = (ID3-5)/2
            DO J=1,2
C--RPC chargino decay couplings
               DO K=1,2
                  B(K,J  ) = AFC(O(K),2*III-1,J,1)
                  B(K,J+2) = AFC(O(K),2*JJJ-1,J,1)
               ENDDO
C--RPV coupling
               C(1,J  )=LAMDA3(JJJ,KKK,III)*QMIXSS(2*III-1,2,J)
               C(2,J  )=0.0D0
               C(1,J+2)=LAMDA3(III,JJJ,KKK)*QMIXSS(2*JJJ-1,2,J)
               C(2,J+2)=0.0D0
C--the intermediate particles
               IDINT(J  ) = 399+2*III+(J-1)*12
               IDINT(J+2) = 399+2*JJJ+(J-1)*12
            ENDDO
C--maximum weight
            MAXWGT=UDDMAX(3,III,JJJ,KKK)
         ENDIF
C--second mode
      ELSE
C--d_i d_j d_k
         IF(IDPDG(ID1).GT.0) THEN
            METYPE=9
C--indices for the couplings
            III = (ID1+1)/2
            JJJ = (ID2+1)/2
            KKK = (ID3+1)/2
            DO J=1,2
C--RPC chargino decay couplings
               DO K=1,2
                  B(K,J  ) = AFC(  K ,2*III,J,1)
                  B(K,J+2) = AFC(  K ,2*JJJ,J,1)
                  B(K,J+4) = AFC(  K ,2*KKK,J,1)
               ENDDO
C--RPV coupling
               C(1,J  )=0.0D0
               C(2,J  )=LAMDA3(III,JJJ,KKK)*QMIXSS(2*III,2,J)
               C(1,J+2)=0.0D0
               C(2,J+2)=LAMDA3(JJJ,III,KKK)*QMIXSS(2*JJJ,2,J)
               C(1,J+4)=0.0D0
               C(2,J+4)=LAMDA3(KKK,III,JJJ)*QMIXSS(2*KKK,2,J)
C--the intermediate particles
               IDINT(J  ) = 400+2*III+(J-1)*12
               IDINT(J+2) = 400+2*JJJ+(J-1)*12
               IDINT(J+4) = 400+2*KKK+(J-1)*12
            ENDDO
C--maximum weight
            MAXWGT=UDDMAX(4,III,JJJ,KKK)
         ELSE
            METYPE=14
C--indices for the couplings
            III = (ID1-5)/2
            JJJ = (ID2-5)/2
            KKK = (ID3-5)/2
            DO J=1,2
C--RPC chargino decay couplings
               DO K=1,2
                  B(K,J  ) = AFC(O(K),2*III,J,1)
                  B(K,J+2) = AFC(O(K),2*JJJ,J,1)
                  B(K,J+4) = AFC(O(K),2*KKK,J,1)
               ENDDO
C--RPV coupling
               C(1,J  )=LAMDA3(III,JJJ,KKK)*QMIXSS(2*III,2,J)
               C(2,J  )=0.0D0
               C(1,J+2)=LAMDA3(JJJ,III,KKK)*QMIXSS(2*JJJ,2,J)
               C(2,J+2)=0.0D0
               C(1,J+4)=LAMDA3(KKK,III,JJJ)*QMIXSS(2*KKK,2,J)
               C(2,J+4)=0.0D0
C--the intermediate particles
               IDINT(J  ) = 400+2*III+(J-1)*12
               IDINT(J+2) = 400+2*JJJ+(J-1)*12
               IDINT(J+4) = 400+2*KKK+(J-1)*12
            ENDDO
C--maximum weight
            MAXWGT=UDDMAX(4,III,JJJ,KKK)
         ENDIF
      ENDIF
      END
