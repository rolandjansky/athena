C**********************************************************************C
C                                                                      C
C                          J E T N E T - 3.5                           C
C                                                                      C
C           A Neural Network program for jet discrimination            C
C         and other High Energy Physics triggering situations          C
C                                                                      C
C                    Latest date of change 97.04.16                    C
C                                                                      C
C                              Authors :                               C
C                                                                      C
C                   Leif Lonnblad, Carsten Peterson,                   C
C                 Hong Pi and Thorsteinn Rognvaldsson                  C
C                                                                      C
C                   Department of Theoretical Physics                  C
C                  University of Lund, Solvegatan 14A,                 C
C                           S-223 62 Lund                              C
C                               Sweden                                 C
C                                                                      C
C                        tel  int+46-46109073                          C
C                        fax  int+46-46104438                          C
C                                                                      C
C                     internet    leif@thep.lu.se                      C
C                                 carsten@thep.lu.se                   C
C                                 pihong@thep.lu.se                    C
C                                 denni@thep.lu.se                     C
C                                                                      C
C           Copyright 1991-1997 L. Lonnblad & Th. Rognvaldsson         C
C                                                                      C
C          Please report any errors to: <denni@thep.lu.se>             C
C                                                                      C
C**********************************************************************C

C**********************************************************************C
C                                                                      C
C An updated version of the program is obtainable through anonymous    C
C ftp from thep.lu.se in directory /pub/LundPrograms/Jetnet/           C
C                                                                      C
C**********************************************************************C

C**********************************************************************C
C  A description of the models and the program can be found in:        C
C                                                                      C
C  (i) Lonnblad et. al., "Self-organizing Networks for Extracting      C
C   Jet Features", Computer Physics Communications, vol. 67,           C
C   pp. 193-209, 1991.                                                 C
C                                                                      C
C  (ii) Lonnblad et. al., "Pattern recognition in High Energy Physics  C
C  with Artificial Neural Networks - JETNET 2.0", Computer Physics     C
C  Communications, nr. 70, pp. 167-182, 1992.                          C
C                                                                      C
C  (iii) Lonnblad et. al. "JETNET 3.0 - A Versatile Artificial         C
C  Neural Network Package", Computer Physics Communications, vol. 81,  C
C  pp. 185-220, 1994.                                                  C
C                                                                      C
C**********************************************************************C

C**********************************************************************C
C         Order of appearance of subroutines and functions:            C
C                                                                      C
C 1) Feed-forward network (JN):                                        C
C   ERRJN, GAUSJN, GJN, GPJN, JNCHOP, JNCOGR, JNCGBE, JNDELT, JNDUMP,  C
C   JNERR, JNESRT, JNFEED, JNHEAD, JNHEIG, JNHESS, JNINDX, JNINIT,     C
C   JNLINS, JNREAD, JNROLD, JNSATM, JNSCGR, JNSEFI, JNSEPA, JNSTAT,    C
C   JNTEST, JNTRAL, JNTRTQ                                             C
C                                                                      C
C 2) Self-organizing network (JM):                                     C
C   GJM, JMDUMP, JMERR, JMFEED, JMINDX, JMINIT, JMINWE, JMNBHD,        C
C   JMNORM, JMREAD, JMSEPA, JMSTAT, JMTEST, JMTRAL, JMWARN             C
C                                                                      C
C The block-data subroutine JNDATA is placed at the end, together      C
C with a test-deck in the subroutine JNTDEC, and a random number       C
C generator called RJN.                                                C
C**********************************************************************C
C**********************************************************************C

C**********************************************************************C
C PART ONE: FEED-FORWARD NETWORK                                       C
C**********************************************************************C


      REAL FUNCTION ERRJN(IDUM)

C...JetNet function calculate ERRor.

C...Returns the error function.
C...The error measure is selected by MSTJN(4).

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      SAVE /JNDAT1/,/JNINT1/,/JNINT2/


      ERR=0.0
      IF (MSTJN(4).EQ.0) THEN
C...Summed square error:

        DO 100 I=1,M(NL)
          OUTVAL=O(JNINDX(NL,I,0))
          ERR=ERR+0.5*(OUT(I)-OUTVAL)**2
          OUT(I)=OUTVAL
100     CONTINUE

      ELSEIF (MSTJN(4).EQ.1) THEN
C...Cross Entropy error:

        DO 110 I=1,M(NL)
C...It is assumed that OUTVAL=0.0 or OUTVAL=1.0 never happens.
          OUTVAL=O(JNINDX(NL,I,0))
          ERR=ERR-(OUT(I)*LOG(OUTVAL)+(1.-OUT(I))*LOG(1.-OUTVAL))
          OUT(I)=OUTVAL
110     CONTINUE

      ELSEIF (MSTJN(4).GE.2) THEN
C...Kullback error:

        DO 120 I=1,M(NL)
C...It is assumed that OUTVAL=0.0 never happens.
          OUTVAL=O(JNINDX(NL,I,0))
          IF(OUT(I).GT.0.0) THEN
            ERR=ERR+OUT(I)*LOG(OUT(I)/OUTVAL)
          ENDIF
          OUT(I)=OUTVAL
120     CONTINUE

      ELSEIF (MSTJN(4).EQ.-1) THEN
C...Log-squared error:

        DO 130 I=1,M(NL)
C...It is assumed that |OUT(I)-OUTVAL|=1.0 never happens.
          OUTVAL=O(JNINDX(NL,I,0))
          ERR=ERR-0.5*LOG(1.-(OUT(I)-OUTVAL)**2)
          OUT(I)=OUTVAL
130     CONTINUE

      ENDIF

      ERRJN=ERR

      RETURN

C**** END OF ERRJN *****************************************************
      END
C***********************************************************************


      REAL FUNCTION GAUSJN(IDUM)
C...JetNet function GAUSsian random number.

C...Generates Gaussian distributed random numbers with
C...standard deviation 1.0 and mean 0.0. Polar method.

      PARAMETER (TINY=1.E-20)

      COMMON /JNGAUS/ ISET,GASDEV
      SAVE /JNGAUS/

      IF (ISET.EQ.0) THEN
100     V1=2.*RJN(IDUM)-1.
        V2=2.*RJN(IDUM)-1.
        R=V1**2+V2**2
        IF ((R.GE.1.).OR.(R.LE.TINY)) GOTO 100
C...Box-Muller transformation:
        FAC=SQRT(-2.*LOG(R)/R)
        GAUSJN=V1*FAC
        GASDEV=V2*FAC
        ISET=1
      ELSE
        GAUSJN=GASDEV
        ISET=0
      ENDIF

      RETURN

C**** END OF GAUSJN ****************************************************
      END
C***********************************************************************


      REAL FUNCTION GJN(IND,X,N)

C...JetNet function G

C...Gives sigmoid function N with argument X
C...The derivative GPrime is also calculated and stored in GPJN.

      PARAMETER(MAXV=2000,MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNSIGM/ GPJN(MAXV),GPPJN(MAXV)
      SAVE /JNDAT1/,/JNSIGM/


      IF(N.EQ.1) THEN
C...        1 -> g(x)=1/(1+exp(-2x))
        ARG=TANH(X)
        GJN=0.5*(1.0+ARG)
        GPJN(IND)=0.5*(1.0-ARG**2)+PARJN(23)
        GPPJN(IND)=ARG*(ARG**2-1.)
      ELSEIF(N.EQ.2) THEN
C...        2 -> g(x)=tanh(x)
        ARG=TANH(X)
        GJN=ARG
        GPJN(IND)=1.-ARG**2+PARJN(23)
        GPPJN(IND)=2.*ARG*(ARG**2-1.)
      ELSEIF(N.EQ.3) THEN
C...        3 -> g(x)=exp(x) (only used internally for Potts-nodes)
        GJN=EXP(MAX(-50.0,MIN(X,50.0)))
        GPJN(IND)=1.0
        GPPJN(IND)=0.0
      ELSEIF(N.EQ.4) THEN
C...        4 -> g(x)=x
        GJN=X
        GPJN(IND)=1.0
        GPPJN(IND)=0.0
      ELSEIF(N.EQ.5) THEN
C...        5 -> g(x)=1/(1+exp(-2x)) (only used internally for
C...             entropy error)
        GJN=0.5*(1.0+TANH(X))
        GPJN(IND)=2.0
        GPPJN(IND)=0.0
      ELSEIF(N.EQ.-1) THEN
C...        same as above, but with fixed precision
        ARG=TANH(X)
        NS=2**ABS(MSTJN(28))
        SS=1.0/(NS-1)
        G=0.5*(1.0+ARG)
        NG=INT(G/SS+0.5)
        GJN=FLOAT(NG)*SS
        GPJN(IND)=0.5*(1.0-ARG**2)+PARJN(23)
        GPPJN(IND)=ARG*(ARG**2-1.)
      ELSEIF(N.EQ.-2) THEN
        ARG=TANH(X)
        NS=2**(ABS(MSTJN(28))-1)
        IF(NS.EQ.1) THEN
          GJN=SIGN(1.0,ARG)
        ELSE
          SS=1.0/(NS-1)
          G=ARG
          NG=INT(ABS(G)/SS+0.5)
          GJN=SIGN(FLOAT(NG)*SS,G)
        ENDIF
        GPJN(IND)=1.-ARG**2+PARJN(23)
        GPPJN(IND)=2.*ARG*(ARG**2-1.)
      ELSE
        MSTJN(3)=N
        CALL JNERR(15)
      ENDIF

      RETURN

C**** END OF GJN *******************************************************
      END 
C***********************************************************************


      SUBROUTINE JNCGBE(BETAK,IOP)
C...JetNet subroutine Conjugate Gradient BEta_k.

C...If IOP=0, it only saves current value of (DW,DT)*(DW,DT) to be 
C...used later and returns BETAK=0.0. If IOP=1, it also calculates 
C...the beta_k value used to generate next search direction. Which 
C...formula to use is determined by MSTJN(5).

C...Note: The vector (DW,DT) equals the negative gradient of E.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)
      PARAMETER(TINY=1.E-8,BIG=1.E+8)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT4/ ILINON,NC,G2,NIT,ERRLN(0:3),DERRLN,STEPLN(0:3),
     &                STEPMN,ERRMN,IEVAL,ISUCC,ICURVE,NSC,GVEC2
      SAVE /JNDAT2/,/JNINT1/,/JNINT2/,/JNINT4/


      BETA=1.0
      BETAK=0.0
      OLDG2=G2
      G2=0.0

      IF ((MSTJN(5).EQ.4).OR.(MSTJN(5).EQ.10)) THEN
C...Polak-Ribiere's formula:

        DO 100 IL=NL,1,-1
          IF(TINV(IL).EQ.0.0) THEN
            BETA=BETA*PARJN(3)
          ELSE
            BETA=BETA*ABS(TINV(IL))
          ENDIF
          DO 110 I=MM0(IL)+1,MM0(IL+1)
            DW(I)=DW(I)/FLOAT(MSTJN(2))
            G2=G2+FLOAT(NSELF(I))*(DW(I)*BETA)**2
            BETAK=BETAK+FLOAT(NSELF(I))*
     &                  DW(I)*ODW(I)*BETA**2
            ODW(I)=DW(I)
110       CONTINUE
          DO 120 I=MV0(IL)+1,MV0(IL+1)
            DT(I)=DT(I)/FLOAT(MSTJN(2))
            G2=G2+FLOAT(NTSELF(I))*(DT(I)*BETA)**2
            BETAK=BETAK+FLOAT(NTSELF(I))*
     &                  DT(I)*ODT(I)*BETA**2
            ODT(I)=DT(I)
120       CONTINUE
100     CONTINUE

        IF (IOP.EQ.0) THEN
          BETAK=0.0
        ELSE
          IF (ABS(OLDG2).GT.TINY) THEN
            BETAK=(G2-BETAK)/OLDG2
          ELSE
            BETAK=0.0
          ENDIF
        ENDIF

      ELSEIF ((MSTJN(5).EQ.5).OR.(MSTJN(5).EQ.11)) THEN
C...Hestenes-Stiefel's formula:

        DGDW=0.0
        DO 200 IL=NL,1,-1
          IF(TINV(IL).EQ.0.0) THEN
            BETA=BETA*PARJN(3)
          ELSE
            BETA=BETA*ABS(TINV(IL))
          ENDIF
          DO 210 I=MM0(IL)+1,MM0(IL+1)
            DW(I)=DW(I)/FLOAT(MSTJN(2))
            G2=G2+FLOAT(NSELF(I))*(DW(I)*BETA)**2
            BETAK=BETAK+FLOAT(NSELF(I))*
     &                  DW(I)*ODW(I)*BETA**2
            DGDW=DGDW+G(I)*(ODW(I)-DW(I))*BETA**2
            ODW(I)=DW(I)
210       CONTINUE
          DO 220 I=MV0(IL)+1,MV0(IL+1)
            DT(I)=DT(I)/FLOAT(MSTJN(2))
            G2=G2+FLOAT(NTSELF(I))*(DT(I)*BETA)**2
            BETAK=BETAK+FLOAT(NTSELF(I))*
     &                  DT(I)*ODT(I)*BETA**2
            DGDW=DGDW+G(MM0(NL+1)+I)*(ODT(I)-DT(I))*BETA**2
            ODT(I)=DT(I)
220       CONTINUE
200     CONTINUE

        IF (IOP.EQ.0) THEN
          BETAK=0.0
        ELSE
          IF (ABS(DGDW).GT.TINY) THEN
            BETAK=(G2-BETAK)/DGDW
          ELSE
            BETAK=0.0
          ENDIF
        ENDIF

      ELSEIF ((MSTJN(5).EQ.6).OR.(MSTJN(5).EQ.12)) THEN
C...Fletcher-Reeves' formula: 

        DO 300 IL=NL,1,-1
          IF(TINV(IL).EQ.0.0) THEN
            BETA=BETA*PARJN(3)
          ELSE
            BETA=BETA*ABS(TINV(IL))
          ENDIF
          DO 310 I=MM0(IL)+1,MM0(IL+1)
            DW(I)=DW(I)/FLOAT(MSTJN(2))
            G2=G2+FLOAT(NSELF(I))*(DW(I)*BETA)**2
            ODW(I)=DW(I)
310       CONTINUE
          DO 320 I=MV0(IL)+1,MV0(IL+1)
            DT(I)=DT(I)/FLOAT(MSTJN(2))
            G2=G2+FLOAT(NTSELF(I))*(DT(I)*BETA)**2
            ODT(I)=DT(I)
320       CONTINUE
300     CONTINUE

       IF (IOP.EQ.0) THEN
          BETAK=0.0
        ELSE
          IF (ABS(OLDG2).GT.TINY) THEN
            BETAK=G2/OLDG2
          ELSE
            BETAK=0.0
          ENDIF
        ENDIF

      ELSEIF ((MSTJN(5).EQ.7).OR.(MSTJN(5).EQ.13)) THEN
C...Shanno's formula: 

        F1=0.0
        F2=0.0
        F3=0.0
        F4=0.0
        FACT1=0.0
        FACT2=0.0
        DO 400 IL=NL,1,-1
          IF(TINV(IL).EQ.0.0) THEN
            BETA=BETA*PARJN(3)
          ELSE
            BETA=BETA*ABS(TINV(IL))
          ENDIF
          DO 410 I=MM0(IL)+1,MM0(IL+1)
            DW(I)=DW(I)/FLOAT(MSTJN(2))
            G2=G2+FLOAT(NSELF(I))*(DW(I)*BETA)**2
            F1=F1-G(I)*ODW(I)*BETA
            F2=F2+G(I)*(ODW(I)-DW(I))*BETA
            F3=F3+ODW(I)*(DW(I)-ODW(I))*BETA**2
            F4=F4+((DW(I)-ODW(I))*BETA)**2
            Y=(ODW(I)-DW(I))
            ODW(I)=DW(I)
            DW(I)=Y
410       CONTINUE
          DO 420 I=MV0(IL)+1,MV0(IL+1)
            DT(I)=DT(I)/FLOAT(MSTJN(2))
            G2=G2+FLOAT(NTSELF(I))*(DT(I)*BETA)**2
            F1=F1-G(MM0(NL+1)+I)*ODT(I)*BETA
            F2=F2+G(MM0(NL+1)+I)*(ODT(I)-DT(I))*BETA
            F3=F3+ODT(I)*(DT(I)-ODT(I))*BETA**2
            F4=F4+((DT(I)-ODT(I))*BETA)**2
            Y=(ODT(I)-DT(I))
            ODT(I)=DT(I)
            DT(I)=Y
420       CONTINUE
          F1=F1*STEPLN(0)
          F2=F2*STEPLN(0)
          IF ((ABS(F2).GT.TINY).AND.(ABS(F2).LT.BIG)) THEN
            FACT1=F1/F2
            FACT2=(1.0+F4/F2)*FACT1-F3/F2
          ELSE
            FACT1=0.0
            FACT2=0.0
          ENDIF
400     CONTINUE

        IF (IOP.EQ.0) THEN
          BETAK=0.0
          DO 430 I=1,MM0(NL+1)
            DW(I)=ODW(I)
430       CONTINUE
          DO 440 I=1,MV0(NL+1)
            DT(I)=ODT(I)
440       CONTINUE
        ELSE
          BETAK=-FACT2*STEPLN(0)
          DO 450 I=1,MM0(NL+1)
            DW(I)=ODW(I)+FACT1*DW(I)
450       CONTINUE
          DO 460 I=1,MV0(NL+1)
            DT(I)=ODT(I)+FACT1*DT(I)
460       CONTINUE
        ENDIF

      ENDIF

      RETURN        

C**** END OF JNCGBE ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNCHOP(ICHP)
C...JetNet subroutine CHOP weights

C...Switches on (ICHP>0) or off (ICHP<0) fixed precision weights 
C...thresholds and sigmoid functions. For IHCP >= 0 the weights and
C...thresholds are chopped to the fixed precision. 

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      SAVE /JNDAT1/,/JNINT1/,/JNINT2/


      IF(ICHP.GT.0) THEN
        ICPON=1
        DO 100 I=1,NL-1
          IF(ABS(NG(I)).NE.1.AND.ABS(NG(I)).NE.2) CALL JNERR(14)
          IF(MSTJN(28).NE.0) THEN 
            NG(I)=-ABS(NG(I))
          ELSE
            NG(I)=ABS(NG(I))
          ENDIF
100     CONTINUE
        IF(MSTJN(28).GT.0) THEN
          IF(ABS(NG(NL)).NE.1.AND.ABS(NG(NL)).NE.2) CALL JNERR(14)
          NG(NL)=-ABS(NG(NL))
        ELSE
          NG(NL)=ABS(NG(NL))
        ENDIF

      ENDIF

      IF(ICHP.LT.0) THEN
        ICPON=0
        DO 110 I=1,NL
          NG(I)=ABS(NG(I))
110     CONTINUE
      ENDIF

      IF(ICHP.GE.0) THEN

        NSW=2**(MSTJN(30)-1)
        NST=2**(MSTJN(29)-1)

        DO 200 IL=1,NL

          IF(MSTJN(30).GE.1) THEN
            WMAX=0.0
            DO 210 I=MM0(IL)+1,MM0(IL+1)
              WMAX=MAX(WMAX,ABS(W(I)))
210         CONTINUE
            SS=1.0
            IF(NSW.GT.1) THEN
              SS=WMAX/(NSW-1)
            ENDIF
            DO 220 I=MM0(IL)+1,MM0(IL+1)
             IF(NSW.GT.1) THEN
                W(I)=SIGN(FLOAT(INT(ABS(W(I))/SS+0.5))*SS,W(I))
              ELSE
                W(I)=SIGN(WMAX,W(I))
              ENDIF
220         CONTINUE
          ENDIF

          IF(MSTJN(29).GE.1) THEN
            TMAX=0.0
            DO 230 I=MV0(IL)+1,MV0(IL+1)
              TMAX=MAX(TMAX,ABS(T(I)))
230         CONTINUE
            SS=1.0
            IF(NST.GT.1) THEN
              SS=TMAX/(NST-1)
            ENDIF
            DO 240 I=MV0(IL)+1,MV0(IL+1)
              IF(NST.GT.1) THEN
                T(I)=SIGN(FLOAT(INT(ABS(T(I))/SS+0.5))*SS,T(I))
              ELSE
                T(I)=SIGN(TMAX,T(I))
              ENDIF
240         CONTINUE
          ENDIF

200     CONTINUE

      ENDIF

      RETURN

C**** END OF JNCHOP ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNCOGR

C...JetNet subroutine COnjugate GRadient

C...Performs Conjugate Gradient updating. 

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT4/ ILINON,NC,G2,NIT,ERRLN(0:3),DERRLN,STEPLN(0:3),
     &                STEPMN,ERRMN,IEVAL,ISUCC,ICURVE,NSC,GVEC2
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/,/JNINT4/


      IF(ILINON.EQ.0) THEN
C...Calc. new conjugate search direction + calculate gradient:

        CALL JNCGBE(BETAK,MOD(NC,(MM0(NL+1)+MV0(NL+1))))
        NC=NC+1
        NSC=0
        DERRLN=0.0
        BETA=1.0
        STEPMN=0.0
        DO 100 IL=NL,1,-1

C...set effective beta in layer IL:
          IF(TINV(IL).EQ.0.0) THEN
            BETA=BETA*PARJN(3)
          ELSE
            BETA=BETA*ABS(TINV(IL))
          ENDIF

          DO 110 I=MM0(IL)+1,MM0(IL+1)
            G(I)=BETAK*G(I)+DW(I)*FLOAT(NSELF(I))*BETA
            DERRLN=DERRLN-ODW(I)*FLOAT(NSELF(I))*BETA*G(I)
110       CONTINUE

          DO 120 I=MV0(IL)+1,MV0(IL+1)
            G(I+MM0(NL+1))=BETAK*G(I+MM0(NL+1))
     &                     +DT(I)*FLOAT(NTSELF(I))*BETA
            DERRLN=DERRLN-ODT(I)*FLOAT(NTSELF(I))*BETA*G(I+MM0(NL+1))
120       CONTINUE

100     CONTINUE

        ILINON=1
        NIT=0
        CALL JNLINS

      ELSE
C...Do line search

        CALL JNLINS

        IF (ILINON.EQ.0) THEN
C...Zero (DW,DT)
          DO 130 I=1,MM0(NL+1)
            DW(I)=0.0
130       CONTINUE
          DO 140 I=1,MV0(NL+1)
            DT(I)=0.0
140       CONTINUE
        ENDIF

      ENDIF

      RETURN

C**** END OF JNCOGR ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNDELT

C...JetNet subroutine DELTa weights

C...Calculates the change in weights and thresholds to minimize the
C...cost function according to gradient descent

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT3/ NXIN,NYIN,NXRF,NYRF,NXHRF,NYHRF,NHRF,NRFW,NHPRF
      COMMON /JNSIGM/ GPJN(MAXV),GPPJN(MAXV)
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/,/JNINT3/,/JNSIGM/


C...(Learning rate and inverse temperature are multiplied in JNTRAL).

C...calculate the deltas at nodes in output layer

      IF (MSTJN(4).EQ.-1) THEN
        DO 101 I=1,M(NL)
          MI=MV0(NL)+I
          DIFF=OUT(I)-O(MI)
          D(MI)=DIFF*GPJN(MI)/(1.-DIFF**2)
101     CONTINUE
      ELSE
        DO 100 I=1,M(NL)
          MI=MV0(NL)+I
          D(MI)=(OUT(I)-O(MI))*GPJN(MI)
100     CONTINUE
      ENDIF

C...calculate deltas in following layers

      DO 200 IL=NL-1,1,-1

C...calculate the deltas at nodes in layer IL

       DO 210 J=1,M(IL)
         MJ=MV0(IL)+J
         MIJ=MM0(IL+1)+(J-1)*M(IL+1)
         SUM=0.0
         DO 220 I=MV0(IL+1)+1,MV0(IL+1)+M(IL+1)
           MIJ=MIJ+1
           SUM=SUM+D(I)*W(MIJ)
220      CONTINUE
        D(MJ)=SUM*GPJN(MJ)
210     CONTINUE
200   CONTINUE

C...calculate deltas at all weights between first and second layer

      NEXTL=2

      IF(NXIN.EQ.0) THEN

C...normal first layer

        DO 300 I=1,M(1)
          MIJ=I-M(1)
          DO 310 J=1,M(0)
            MIJ=MIJ+M(1)
            DW(MIJ)=DW(MIJ)+D(I)*OIN(J)
310       CONTINUE
          DT(I)=DT(I)+D(I)
300     CONTINUE

      ELSE

C...receptive fields in first layer

        DO 320 IHPRF=1,NHPRF

          SUMRFT=0.0

          DO 330 IY=1,NYHRF
            IH=IY-NYHRF+(IHPRF-1)*NHRF
            DO 340 IX=1,NXHRF
              IH=IH+NYHRF
              DO 350 JY=1,NYRF
                IW=JY-NYRF+(IHPRF-1)*NRFW
                DO 360 JX=1,NXRF
                  IW=IW+NYRF
                  INX=IX+JX-1
                  IF(INX.GT.ABS(NXIN)) INX=INX-ABS(NXIN)
                  INY=IY+JY-1
                  IF(INY.GT.ABS(NYIN)) INY=INY-ABS(NYIN)
                  IN=(INX-1)*ABS(NYIN)+INY
                  DW(IW)=DW(IW)+D(IH)*OIN(IN)/FLOAT(NHRF)
360             CONTINUE
350           CONTINUE
              DO 370 IN=ABS(NXIN*NYIN)+1,M(0)
                IW=NXRF*NYRF+IN-ABS(NXIN*NYIN)+(IHPRF-1)*NRFW
                DW(IW)=DW(IW)+D(IH)*OIN(IN)/FLOAT(NHRF)
370           CONTINUE
              SUMRFT=SUMRFT+D(IH)
340         CONTINUE
330       CONTINUE

          SUMRFT=SUMRFT/FLOAT(NXHRF*NYHRF)

          DO 380 IH=1,NXHRF*NYHRF
            DT(IH+(IHPRF-1)*NHRF)=DT(IH+(IHPRF-1)*NHRF)+SUMRFT
380       CONTINUE

320     CONTINUE

        DO 390 IH=NHRF*NHPRF+1,M(1)
          IW=NRFW*NHPRF+IH-M(1)
          DO 400 IN=1,M(0)
            IW=IW+M(1)
            DW(IW)=DW(IW)+D(IH)*OIN(IN)
400       CONTINUE
          DT(IH)=DT(IH)+D(IH)
390     CONTINUE

        IF(MSTJN(27).LT.0) THEN

          DO 500 I=1,M(2)
            MI=MV0(2)+I
            DO 510 IHPRF=1,NHPRF
              SUMRFW=0.0
              DO 520 J=(IHPRF-1)*NHRF+1,IHPRF*NHRF
                SUMRFW=SUMRFW+D(MI)*O(J)
520           CONTINUE
              MIJ=MM0(2)+((IHPRF-1)*NHRF-1)*M(2)+I
              SUMRFW=SUMRFW/FLOAT(NHRF)
              DO 530 J=1,NHRF
                MIJ=MIJ+M(2)
                DW(MIJ)=DW(MIJ)+SUMRFW
530           CONTINUE
510         CONTINUE
            MIJ=MM0(2)+I+(NHRF*NHPRF-1)*M(2)
            DO 540 J=NHRF*NHPRF+1,M(1)
              MIJ=MIJ+M(2)
              DW(MIJ)=DW(MIJ)+D(MI)*O(J)
540         CONTINUE
            DT(MI)=DT(MI)+D(MI)
500       CONTINUE

          NEXTL=3

        ENDIF

      ENDIF

C...calculate deltas at all weights between following layers

      DO 410 IL=NEXTL,NL

       DO 420 I=1,M(IL)
         MIJ=MM0(IL)+I-M(IL)
         MI=MV0(IL)+I
         DO 430 J=MV0(IL-1)+1,MV0(IL-1)+M(IL-1)
           MIJ=MIJ+M(IL)
           DW(MIJ)=DW(MIJ)+D(MI)*O(J)
430      CONTINUE
         DT(MI)=DT(MI)+D(MI)
420     CONTINUE
410   CONTINUE

      RETURN

C**** END OF JNDELT ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNDUMP(NF)

C...JetNet subroutine DUMP weights

C...Dumps weights, threshold and other characteristics of the
C...net to a file for use in other programs

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT3/ NXIN,NYIN,NXRF,NYRF,NXHRF,NYHRF,NHRF,NRFW,NHPRF
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/,/JNINT3/

      CHARACTER C(0:1)


      IF(NF.LT.0) THEN

C...unformatted dump

        JF=-NF
        WRITE(JF) 30
        WRITE(JF) MSTJN,PARJN,TINV,IGFN,ETAL,WIDL,SATM

        DO 100 I=1,MM0(NL+1)
          WRITE(JF) W(I)
100     CONTINUE

        DO 110 I=1,MV0(NL+1)
          WRITE(JF) T(I)
110     CONTINUE

        DO 120 I=1,MM0(NL+1)
          WRITE(JF) NSELF(I)
120     CONTINUE

        DO 130 I=1,MV0(NL+1)
          WRITE(JF) NTSELF(I)
130     CONTINUE

      ELSE

C...Formatted dump

        C(1)=' '
        C(0)='*'

        NFSAVE=MSTJN(6)
        MSTJN(6)=NF

        WRITE(NF,600)
        CALL JNHEAD
        CALL JNSTAT(1)
        CALL JNSTAT(2)

        MSTJN(6)=NFSAVE

        WRITE(NF,*)
        WRITE(NF,*)
        WRITE(NF,*)

        IF(NXIN.EQ.0) THEN

          WRITE(NF,610)0,1
          DO 200 J=1,M(0)
            WRITE(NF,*)
            WRITE(NF,640)(W(JNINDX(1,I,J)),
     &                    C(NSELF(JNINDX(1,I,J))),I=1,M(1))
200       CONTINUE

        ELSE

          WRITE(NF,650)
          DO 210 IHPRF=1,NHPRF
            WRITE(NF,*)
            WRITE(NF,640)(W(IW),C(NSELF(IW)),
     &                      IW=NRFW*(IHPRF-1)+1,NRFW*IHPRF)
210       CONTINUE
          IF(NHRF*NHPRF.LT.M(1)) THEN
            WRITE(NF,*)
            WRITE(NF,660)
            DO 220 J=1,M(0)
              WRITE(NF,*)
              WRITE(NF,640)(W(JNINDX(1,I,J)),
     &                    C(NSELF(JNINDX(1,I,J))),I=NHRF*NHPRF+1,M(1))
220         CONTINUE
          ENDIF

        ENDIF

        WRITE(NF,*)
        WRITE(NF,630) 1
        WRITE(NF,*)
        WRITE(NF,640)(T(JNINDX(1,I,0)),
     &                C(NTSELF(JNINDX(1,I,0))),I=1,M(1))

        DO 300 IL=2,NL

          WRITE(NF,*)
          WRITE(NF,610)IL-1,IL
          DO 310 J=1,M(IL-1)
            WRITE(NF,*)
            WRITE(NF,640)(W(JNINDX(IL,I,J)),
     &                    C(NSELF(JNINDX(IL,I,J))),I=1,M(IL))
310       CONTINUE

          WRITE(NF,*)
          WRITE(NF,630)IL
          WRITE(NF,*)
          WRITE(NF,640)(T(JNINDX(IL,I,0)),
     &                  C(NTSELF(JNINDX(IL,I,0))),I=1,M(IL))
300     CONTINUE

      ENDIF

600   FORMAT(26X,'Dump of weights generated by')
610   FORMAT(21X,'Values of weights between layer',I2,' (rows) and',I2,
     &           ' (columns)')
630   FORMAT(30X,'Thresholds in layer',I2)
640   FORMAT(6(E12.4,A1))
650   FORMAT(21X,'Values of weights in receptive fields')
660   FORMAT(21X,'Values of other weights between input layer (rows)',
     &           ' and layer  1 (columns)')

      RETURN

C**** END OF JNDUMP ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNERR(IERR)

C...JetNet subroutine ERROR

C...Writes out an error message and stops the execution

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)
      PARAMETER(MAXD2E=300)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/


      IF (MSTJM(8).EQ.1) MSTJN(6)=MSTJM(6)
      WRITE(MSTJN(6),600) IERR

      IF(IERR.EQ.1) THEN
        WRITE(MSTJN(6),610) MSTJN(1)
      ELSEIF(IERR.EQ.2) THEN
        WRITE(MSTJN(6),620) MV0(NL+1),MAXV
      ELSEIF(IERR.EQ.3) THEN
        WRITE(MSTJN(6),630) MM0(NL+1),MAXM
      ELSEIF(IERR.EQ.4) THEN
        WRITE(MSTJN(6),640)
      ELSEIF(IERR.EQ.5) THEN
        WRITE(MSTJN(6),650) 'JNINDX'
      ELSEIF(IERR.EQ.6) THEN
        WRITE(MSTJN(6),650) 'JNSEFI'
      ELSEIF(IERR.EQ.7) THEN
        WRITE(MSTJN(6),660) MSTJN(10),MAXI
      ELSEIF(IERR.EQ.8) THEN
        WRITE(MSTJN(6),670) MSTJN(NL),MAXO
      ELSEIF(IERR.EQ.9) THEN
        WRITE(MSTJN(6),680) MSTJN(5)
      ELSEIF(IERR.EQ.10) THEN
        WRITE(MSTJN(6),690) (MSTJN(I),I=23,26)
      ELSEIF(IERR.EQ.11) THEN
        WRITE(MSTJN(6),700) MSTJN(10),MSTJN(23),MSTJN(24)
      ELSEIF(IERR.EQ.12) THEN
        WRITE(MSTJN(6),710)
      ELSEIF(IERR.EQ.13) THEN
        WRITE(MSTJN(6),720)
      ELSEIF(IERR.EQ.14) THEN
        WRITE(MSTJN(6),730)
      ELSEIF(IERR.EQ.15) THEN
        WRITE(MSTJN(6),740)MSTJN(3)
      ELSEIF(IERR.EQ.16) THEN
        WRITE(MSTJN(6),750)
      ELSEIF(IERR.EQ.17) THEN
        WRITE(MSTJN(6),760)
      ELSEIF(IERR.EQ.18) THEN
        WRITE(MSTJN(6),770)
      ELSEIF(IERR.EQ.19) THEN
        WRITE(MSTJN(6),780)
      ELSEIF(IERR.EQ.20) THEN
        WRITE(MSTJN(6),790)
      ELSEIF(IERR.EQ.21) THEN
        WRITE(MSTJN(6),800)MSTJN(38)
        WRITE(MSTJN(6),805)MSTJN(36)
      ELSEIF(IERR.EQ.22) THEN
        WRITE(MSTJN(6),650) 'JNTRAL'
      ELSEIF(IERR.EQ.23) THEN
        WRITE(MSTJN(6),650) 'JNTEST'
      ELSEIF(IERR.EQ.24) THEN
        WRITE(MSTJN(6),810)MSTJN(9)
      ELSEIF(IERR.EQ.25) THEN
        WRITE(MSTJN(6),820)MSTJN(7)
      ELSEIF(IERR.EQ.26) THEN
        WRITE(MSTJN(6),650)'JNHESS'
      ELSEIF(IERR.EQ.27) THEN
        WRITE(MSTJN(6),830)MM0(NL+1)+MV0(NL+1)
      ELSEIF(IERR.EQ.28) THEN
        WRITE(MSTJN(6),650)'JNHDIA'
      ELSEIF(IERR.EQ.29) THEN
        WRITE(MSTJN(6),840)MSTJN(39)
        WRITE(MSTJN(6),850)MSTJN(9)*MSTJN(2)
      ELSEIF(IERR.EQ.30) THEN
        WRITE(MSTJN(6),860)
      ELSEIF(IERR.EQ.31) THEN
        WRITE(MSTJN(6),870)MSTJN(4)
        WRITE(MSTJN(6),880)IGFN(NL)
      ELSEIF(IERR.EQ.32) THEN
        WRITE(MSTJN(6),890)
      ENDIF

      IF(IERR.GT.0) STOP 0

600   FORMAT(' *** JETNET ERROR:',I2,' ***')
610   FORMAT(' Illegal number of layers (',I3,')')
620   FORMAT(' Total number of nodes (',I6,') exceeds limit (',I6,').')
630   FORMAT(' Total number of weights (',I6,') exceeds limit (',
     &I6,').')
640   FORMAT(' Number of nodes in output layer is incompatible ',/,
     &       ' with the dimension of the Potts-nodes.')
650   FORMAT(' JETNET must be initialized (with JNINIT or JNREAD) ',
     &       'before ',A6,' can be called.')
660   FORMAT(' Total number of input nodes (',I6,
     &                   ') exceeds limit (',I6,').')
670   FORMAT(' Total number of output nodes (',I6,
     &                   ') exceeds limit (',I6,').')
680   FORMAT(' Undefined updating algorithm (',I2,') chosen.')
690   FORMAT(' Inconsistent geometry for receptive fields:',/,
     &       ' (MSTJN(23) = ',I4,', MSTJN(24) = ',I4,
     &       ', MSTJN(25) = ',I4,', MSTJN(26) = ',I4,')')
700   FORMAT(' Too few input nodes (=',I4,') for receptive fields',/,
     &       ' (MSTJN(23) = ',I4,' and MSTJN(24) = ',I4,').')
710   FORMAT(' In JNSEFI: attempt to connect/disconnect unconnectable',
     &       ' nodes.')
720   FORMAT(' Cannot read file - wrong format. Try JNROLD instead.')
730   FORMAT(' Chopping not allowed on non-sigmoid functions.')
740   FORMAT(' Undefined transfer function (',I2,') in GJN.')
750   FORMAT(' Call to JNINIT after calling JMINIT')
760   FORMAT(' JNREAD cannot read data-file produced by JMDUMP')
770   FORMAT(' JNROLD cannot read data-file produced by JMDUMP')
780   FORMAT(' You cannot start learning by terminating Conj. Grad.')
790   FORMAT(' Too many warnings issued by JETNET.')
800   FORMAT(' Nr. of restarts (',I4,') in Quickprop, line search, or ')
805   FORMAT(' Scaled Conj. Grad. exceeds maximum MSTJN(36) = ',I4)
810   FORMAT(' MSTJN(9) (',I3,') must be > 0')
820   FORMAT(' Layer ',I2,' has no nodes')
830   FORMAT(' Nr. of weights (',I6,') exceeds limit in JNHESS.')
840   FORMAT(' Nr. of calls to JNHESS (',I5,') must be an integer ')
850   FORMAT(' multiple of MSTJN(9)*MSTJN(2) (',I5,') if JNHEIG',
     &       ' is invoked')
860   FORMAT(' Too many iterations in subroutine JNTRTQ.')
870   FORMAT(' Error function, MSTJN(4) = ',I2,', incompatible with')
880   FORMAT(' using output transfer function = ',I3)
890   FORMAT(' Updating turned off, MSTJN(5) = 9, when calling JNINIT')

      RETURN

C**** END OF JNERR *****************************************************
      END
C***********************************************************************


      SUBROUTINE JNESRT(NWGTS,IGRAD)

C...JetNet subroutine Eigenvector/value SoRT

C...Sorts the eigenvectors and eigenvalues into descending order.
C...The routine is taken from Press et al. "Numerical Recipes", where
C...it is called eigsrt.
C...If IGRAD =|= 0, then eigenvectors are also sorted, otherwise not.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)
      PARAMETER(MAXD2E=300)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT5/ D2E(MAXD2E,MAXD2E)
      SAVE /JNDAT1/,/JNINT5/

      DO 13 I=1,NWGTS-1
        K=I
        TEMP=OUT(I)
        DO 11 J=I+1,NWGTS
          IF(OUT(J).GE.TEMP) THEN
            K=J
            TEMP=OUT(J)
          ENDIF
11      CONTINUE
        IF(K.NE.I) THEN
          OUT(K)=OUT(I)
          OUT(I)=TEMP
          IF(IGRAD.NE.0) THEN
            DO 12 J=1,NWGTS
              TEMP=D2E(J,I)
              D2E(J,I)=D2E(J,K)
              D2E(J,K)=TEMP
12          CONTINUE
          ENDIF
        ENDIF
13    CONTINUE

      RETURN

C**** END OF JNESRT ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNFEED

C...JetNet subroutine FEED signal through net

C...Takes the the values of OIN and calculates the values of
C...the output nodes without writing to OUT

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT3/ NXIN,NYIN,NXRF,NYRF,NXHRF,NYHRF,NHRF,NRFW,NHPRF
      SAVE /JNDAT1/,/JNDAT2/, /JNINT1/,/JNINT2/,/JNINT3/


C...set beta in first layer

      IF(TINV(1).EQ.0.0) THEN
        BETA=PARJN(3)
      ELSE
        BETA=ABS(TINV(1))
      ENDIF

C...calculate nodes in first layer

      IF(NXIN.EQ.0) THEN

C...normal first layer

        DO 100 I=1,M(1)
          A(I)=T(I)
          MIJ=I-M(1)
          DO 110 J=1,M(0)
            MIJ=MIJ+M(1)
            A(I)=A(I)+W(MIJ)*OIN(J)
110       CONTINUE
          O(I)=GJN(I,BETA*A(I),NG(1))
100     CONTINUE

      ELSE

C...receptive fields in first layer

        DO 120 IHPRF=1,NHPRF

          DO 130 IY=1,NYHRF
            IH=IY-NYHRF+(IHPRF-1)*NHRF
            DO 140 IX=1,NXHRF
              IH=IH+NYHRF
              A(IH)=T(IH)
              DO 150 JY=1,NYRF
                IW=JY-NYRF+(IHPRF-1)*NRFW
                DO 160 JX=1,NXRF
                  IW=IW+NYRF
                  INX=IX+JX-1
                  IF(INX.GT.ABS(NXIN)) INX=INX-ABS(NXIN)
                  INY=IY+JY-1
                  IF(INY.GT.ABS(NYIN)) INY=INY-ABS(NYIN)
                  IN=(INX-1)*ABS(NYIN)+INY
                  A(IH)=A(IH)+W(IW)*OIN(IN)
160             CONTINUE
150           CONTINUE
              DO 170 IN=ABS(NXIN*NYIN)+1,M(0)
                IW=NXRF*NYRF+IN-ABS(NXIN*NYIN)+(IHPRF-1)*NRFW
                A(IH)=A(IH)+W(IW)*OIN(IN)
170           CONTINUE
              O(IH)=GJN(IH,BETA*A(IH),NG(1))
140         CONTINUE
130       CONTINUE
120     CONTINUE

        DO 180 IH=NHRF*NHPRF+1,M(1)
          A(IH)=T(IH)
          IW=NHRF*NHPRF+IH-M(1)
          DO 190 IN=1,M(0)
            IW=IW+M(1)
            A(IH)=A(IH)+W(IW)*OIN(IN)
190       CONTINUE
          O(IH)=GJN(IH,BETA*A(IH),NG(1))
180     CONTINUE

      ENDIF

C...calculate nodes in following layers

      DO 200 IL=2,NL

C...set beta in layer IL

       IF(TINV(IL).EQ.0.0) THEN
         BETA=PARJN(3)
       ELSE
         BETA=ABS(TINV(IL))
       ENDIF

C...calculate nodes in layer IL

       DO 210 I=1,M(IL)
         MI=MV0(IL)+I
         A(MI)=T(MI)
         MIJ=MM0(IL)-M(IL)+I
         DO 220 J=MV0(IL-1)+1,MV0(IL-1)+M(IL-1)
           MIJ=MIJ+M(IL)
           A(MI)=A(MI)+W(MIJ)*O(J)
220      CONTINUE
         O(MI)=GJN(MI,BETA*A(MI),NG(IL))
210     CONTINUE
200   CONTINUE

      IF(IPOTT.LT.2) RETURN

C...Special treatment of output layer if Potts-nodes

      DO 300 I=1,M(NL)/IPOTT
        DD=0.0
        JO=MV0(NL)+(I-1)*IPOTT
        DO 310 J=1,IPOTT
          DD=DD+O(JO+J)
310     CONTINUE
        DO 320 J=1,IPOTT
          O(JO+J)=O(JO+J)/DD
320     CONTINUE
300   CONTINUE

      RETURN

C**** END OF JNFEED ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNHEAD

C...JetNet subroutine write HEADer

C...Writes a header on file number NF


      PARAMETER(MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      SAVE /JNDAT1/


      IF(MSTJM(8).EQ.1) MSTJN(6)=MSTJM(6)

      WRITE(MSTJN(6),*)
      WRITE(MSTJN(6),*)
      WRITE(MSTJN(6),600)
      WRITE(MSTJN(6),610)
      WRITE(MSTJN(6),*)

600   FORMAT(14X,'The Lund Neural Network Program - JETNET version 3.5')
610   FORMAT(14X,'******  Latest date of change: April 16, 1997 ******')

      RETURN

C**** END OF JNHEAD ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNHEIG(IGRAD)

C...JetNet subroutine Hessian EIGenvalues.

C...Diagonalizes the Hessian matrix stored in D2E. The eigenvalues
C...are placed in the vector OUT. If IGRAD isn't equal to 0 then the
C...eigenvectors of the Hessian are calculated.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,TINY=1.E-20)
      PARAMETER(MAXD2E=300)
      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT5/ D2E(MAXD2E,MAXD2E)
      SAVE /JNDAT1/,/JNINT2/,/JNINT5/


      IF (MSTJN(8).EQ.0) CALL JNERR(28)
      IF ((MOD(MSTJN(39),(MSTJN(2)*MSTJN(9))).NE.0).OR.
     &    (MSTJN(39).LE.0)) CALL JNERR(29)

      NWGTS=MM0(NL+1)+MV0(NL+1)
C...Compute eigenvectors/values
      CALL JNTRTQ(NWGTS,IGRAD)
C...Sort them in descending order
      CALL JNESRT(NWGTS,IGRAD)

      RETURN

C**** END OF JNHEIG ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNHESS

C...JetNet subroutine calculate HESSian

C...Calculates the Hessian for the network. It assumes a summed square
C...error (MSTJN(4)=0).

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,TINY=1.E-20)
      PARAMETER(MAXD2E=300)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT5/ D2E(MAXD2E,MAXD2E)
      COMMON /JNSIGM/ GPJN(MAXV),GPPJN(MAXV)
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/,/JNINT5/,/JNSIGM/

      DIMENSION DD(MAXD2E),Q(MAXD2E,MAXD2E)

      IF (MSTJN(8).EQ.0) CALL JNERR(26)
      IF (MSTJN(9).LE.0) CALL JNERR(24)

      NWGTS=MM0(NL+1)+MV0(NL+1)
      IF (NWGTS.GT.MAXD2E) CALL JNERR(27)
      IF (MOD(MSTJN(39),(MSTJN(2)*MSTJN(9))).EQ.0) THEN
C...zero Hessian:
        DO 100 I=1,NWGTS
          DO 110 J=1,NWGTS
            D2E(I,J)=0.0
110       CONTINUE
100     CONTINUE
      ENDIF

      MSTJN(39)=MSTJN(39)+1

      CALL JNFEED

C...rescale GPJN and GPPJN:
      DO 200 IL=1,NL
        IF(TINV(IL).EQ.0.0) THEN
          BETA=PARJN(3)
        ELSE
          BETA=ABS(TINV(IL))
        ENDIF
        DO 210 I=1,M(IL)
          MI=JNINDX(IL,I,0)
          GPJN(MI)=GPJN(MI)*BETA
          GPPJN(MI)=GPPJN(MI)*BETA**2
210     CONTINUE
200   CONTINUE

C...compute Q:
      DO 220 IL1=NL,2,-1
        DO 230 IL2=IL1-1,1,-1
          DO 240 I=1,M(IL1)
            MI=JNINDX(IL1,I,0)
            DO 250 J=1,M(IL2)
              MJ=JNINDX(IL2,J,0)
              IF (IL2.EQ.IL1-1) THEN
                MIJ=JNINDX(IL1,I,J)
                Q(MI,MJ)=W(MIJ)
              ELSE
                SUM=0.0
                DO 260 J2=1,M(IL2+1)
                  MJ2=JNINDX(IL2+1,J2,0)
                  MJK=JNINDX(IL2+1,J2,J)
                  SUM=SUM+Q(MI,MJ2)*GPJN(MJ2)*W(MJK)
260             CONTINUE
                Q(MI,MJ)=SUM
              ENDIF
250         CONTINUE
240       CONTINUE
230     CONTINUE
220   CONTINUE

C...Loop over output units:
      DO 300 I=1,M(NL)
        MI=JNINDX(NL,I,0)
        GAM=(O(MI)-OUT(I))*GPJN(MI)
        GAMCAP=(O(MI)-OUT(I))*GPPJN(MI)

C...Diagonal weights - output layer:
        D2E(I,I)=D2E(I,I)+GAMCAP
        IF (NL.NE.1) THEN
          DO 310 J1=1,M(NL-1)
            MJ1=JNINDX(NL-1,J1,0)
            MIJ1=M(NL)+(I-1)*M(NL-1)+J1
            TERM=GAMCAP*O(MJ1)
            D2E(I,MIJ1)=D2E(I,MIJ1)+TERM
            DO 320 J2=1,J1
              MJ2=JNINDX(NL-1,J2,0)
              MIJ2=M(NL)+(I-1)*M(NL-1)+J2
              D2E(MIJ2,MIJ1)=D2E(MIJ2,MIJ1)+TERM*O(MJ2)
320         CONTINUE
310       CONTINUE
        ELSE
          DO 330 J1=1,M(0)
            MIJ1=M(NL)+(I-1)*M(0)+J1
            TERM=GAMCAP*OIN(J1)
            D2E(I,MIJ1)=D2E(I,MIJ1)+TERM
            DO 340 J2=1,J1
              MIJ2=M(NL)+(I-1)*M(0)+J2
              D2E(MIJ2,MIJ1)=D2E(MIJ2,MIJ1)+TERM*OIN(J2)
340         CONTINUE
330       CONTINUE
        ENDIF

C...Diagonal weights - other layers:
        IOFST=0
        DO 350 IL=NL-1,1,-1
          IOFST=IOFST+M(IL+1)*(1+M(IL))
          DO 360 J1=1,M(IL)
            MJ1=JNINDX(IL,J1,0)
            FACTOR=GAM*Q(MI,MJ1)*GPPJN(MJ1)

            D2E(IOFST+J1,IOFST+J1)=D2E(IOFST+J1,IOFST+J1)+FACTOR
            IF (IL.GT.1) THEN
              DO 370 K1=1,M(IL-1)
                MK1=JNINDX(IL-1,K1,0)
                MJK1=M(IL)+(J1-1)*M(IL-1)+K1
                TERM=FACTOR*O(MK1)
                D2E(IOFST+J1,IOFST+MJK1)=
     &          D2E(IOFST+J1,IOFST+MJK1)+TERM
                DO 380 K2=1,K1
                  MK2=JNINDX(IL-1,K2,0)
                  MJK2=M(IL)+(J1-1)*M(IL-1)+K2
                  D2E(IOFST+MJK2,IOFST+MJK1)=
     &            D2E(IOFST+MJK2,IOFST+MJK1)+TERM*O(MK2)
380             CONTINUE
370           CONTINUE
            ELSE
              DO 390 K1=1,M(0)
                MJK1=M(1)+(J1-1)*M(0)+K1
                TERM=FACTOR*OIN(K1)
                D2E(IOFST+J1,IOFST+MJK1)=
     &          D2E(IOFST+J1,IOFST+MJK1)+TERM
                DO 400 K2=1,K1
                  MJK2=M(1)+(J1-1)*M(0)+K2
                  D2E(IOFST+MJK2,IOFST+MJK1)=
     &            D2E(IOFST+MJK2,IOFST+MJK1)+TERM*OIN(K2)
400             CONTINUE
390           CONTINUE
            ENDIF

            DO 410 J2=1,M(IL)
              MJ2=JNINDX(IL,J2,0)
              FACTOR=GAMCAP*Q(MI,MJ1)*GPJN(MJ1)*Q(MI,MJ2)*GPJN(MJ2)
              IF (IL.LE.NL-2) THEN
                SUM=0.0
                DO 420 IL2=NL-1,IL+1,-1
                  DO 430 J=1,M(IL2)
                    MJ=JNINDX(IL2,J,0)
                    SUM=SUM+Q(MI,MJ)*GPPJN(MJ)*Q(MJ,MJ1)*Q(MJ,MJ2)
430               CONTINUE
420             CONTINUE
                FACTOR=FACTOR+SUM*GAM*GPJN(MJ1)*GPJN(MJ2)
              ENDIF

              IF (J2.GE.J1) THEN
                D2E(IOFST+J1,IOFST+J2)=D2E(IOFST+J1,IOFST+J2)+FACTOR
              ENDIF
              IF (IL.GT.1) THEN
                DO 431 K1=1,M(IL-1)
                  MK1=JNINDX(IL-1,K1,0)
                  MJK1=M(IL)+(J2-1)*M(IL-1)+K1
                  TERM=FACTOR*O(MK1)
                  D2E(IOFST+J1,IOFST+MJK1)=
     &            D2E(IOFST+J1,IOFST+MJK1)+TERM
                  IF (J2.EQ.J1) THEN
                    DO 440 K2=1,K1
                      MK2=JNINDX(IL-1,K2,0)
                      MJK2=M(IL)+(J1-1)*M(IL-1)+K2
                      D2E(IOFST+MJK2,IOFST+MJK1)=
     &                D2E(IOFST+MJK2,IOFST+MJK1)+TERM*O(MK2)
440                 CONTINUE
                  ELSEIF (J2.GT.J1) THEN
                    DO 450 K2=1,M(IL-1)
                      MK2=JNINDX(IL-1,K2,0)
                      MJK2=M(IL)+(J1-1)*M(IL-1)+K2
                      D2E(IOFST+MJK2,IOFST+MJK1)=
     &                D2E(IOFST+MJK2,IOFST+MJK1)+TERM*O(MK2)
450                 CONTINUE
                  ENDIF
431             CONTINUE
              ELSE
                DO 460 K1=1,M(0)
                  MJK1=M(1)+(J2-1)*M(0)+K1
                  TERM=FACTOR*OIN(K1)
                  D2E(IOFST+J1,IOFST+MJK1)=
     &            D2E(IOFST+J1,IOFST+MJK1)+TERM
                  IF (J2.EQ.J1) THEN
                    DO 470 K2=1,K1
                      MJK2=M(1)+(J1-1)*M(0)+K2
                      D2E(IOFST+MJK2,IOFST+MJK1)=
     &                D2E(IOFST+MJK2,IOFST+MJK1)+TERM*OIN(K2)
470                 CONTINUE
                  ELSEIF (J2.GT.J1) THEN
                    DO 480 K2=1,M(0)
                      MJK2=M(1)+(J1-1)*M(0)+K2
                      D2E(IOFST+MJK2,IOFST+MJK1)=
     &                D2E(IOFST+MJK2,IOFST+MJK1)+TERM*OIN(K2)
480                 CONTINUE
                  ENDIF
460             CONTINUE
              ENDIF

410         CONTINUE
360       CONTINUE

350     CONTINUE
C...End of diagonal weights.

C...1st off-diagonal - output layer:
        IOFST2=M(NL)*(1+M(NL-1))
        DO 500 J=1,M(NL-1)
          MJ=JNINDX(NL-1,J,0)
          FACTOR=GAMCAP*Q(MI,MJ)*GPJN(MJ)

          D2E(I,IOFST2+J)=D2E(I,IOFST2+J)+FACTOR

          MIJ=M(NL)+(I-1)*M(NL-1)+J
          FACT2=GAM*GPJN(MJ)

          D2E(MIJ,IOFST2+J)=D2E(MIJ,IOFST2+J)+FACT2
          IF (NL.GT.2) THEN
            DO 510 K=1,M(NL-2)
              MK=JNINDX(NL-2,K,0)
              MJK=M(NL-1)+(J-1)*M(NL-2)+K
              D2E(I,IOFST2+MJK)=
     &        D2E(I,IOFST2+MJK)+FACTOR*O(MK)
              D2E(MIJ,IOFST2+MJK)=
     &        D2E(MIJ,IOFST2+MJK)+FACT2*O(MK)
510         CONTINUE
          ELSE
            DO 520 K=1,M(0)
              MJK=M(1)+(J-1)*M(0)+K
              D2E(I,IOFST2+MJK)=
     &        D2E(I,IOFST2+MJK)+FACTOR*OIN(K)
              D2E(MIJ,IOFST2+MJK)=
     &        D2E(MIJ,IOFST2+MJK)+FACT2*OIN(K)
520         CONTINUE
          ENDIF
          DO 530 J2=1,M(NL-1)
            MJ2=JNINDX(NL-1,J2,0)
            MIJ2=M(NL)+(I-1)*M(NL-1)+J2
            TERM=FACTOR*O(MJ2)
            D2E(MIJ2,IOFST2+J)=D2E(MIJ2,IOFST2+J)+TERM
            IF (NL.GT.2) THEN
              DO 540 K=1,M(NL-2)
                MK=JNINDX(NL-2,K,0)
                MJK=M(NL-1)+(J-1)*M(NL-2)+K
                D2E(MIJ2,IOFST2+MJK)=
     &          D2E(MIJ2,IOFST2+MJK)+TERM*O(MK)
540           CONTINUE
            ELSE
              DO 550 K=1,M(0)
                MJK=M(1)+(J-1)*M(0)+K
                D2E(MIJ2,IOFST2+MJK)=
     &          D2E(MIJ2,IOFST2+MJK)+TERM*OIN(K)
550           CONTINUE
            ENDIF
530       CONTINUE

500     CONTINUE

C...1st off-diagonal - other layers:
        IOFST1=0
        DO 560 IL=NL-1,2,-1
          IOFST1=IOFST1+M(IL+1)*(1+M(IL))
          IOFST2=IOFST2+M(IL)*(1+M(IL-1))
          DO 570 J=1,M(IL)
            MJ=JNINDX(IL,J,0)
            DO 580 K=1,M(IL-1)
              MK=JNINDX(IL-1,K,0)
              FACTOR=GAMCAP*Q(MI,MJ)*GPJN(MJ)*Q(MI,MK)*GPJN(MK)+
     &             GAM*Q(MI,MJ)*GPPJN(MJ)*Q(MJ,MK)*GPJN(MK)
              IF (IL.LE.NL-2) THEN
                SUM=0.0
                DO 590 IL2=NL-1,IL+1,-1
                  DO 600 J2=1,M(IL2)
                    MJ2=JNINDX(IL2,J2,0)
                    SUM=SUM+Q(MI,MJ2)*GPPJN(MJ2)*Q(MJ2,ML)*Q(MJ2,MJ)
600               CONTINUE
590             CONTINUE
                FACTOR=FACTOR+SUM*GAM*GPJN(ML)*GPJN(MJ)
              ENDIF

              D2E(IOFST1+I,IOFST2+J)=D2E(IOFST1+I,IOFST2+J)+FACTOR

              MJK=M(IL)+(J-1)*M(IL-1)+K
              FACT2=GAM*Q(MI,MJ)*GPJN(MJ)*GPJN(MK)

              D2E(IOFST1+MJK,IOFST2+J)=D2E(IOFST1+MJK,IOFST2+J)+FACT2

              IF (IL-1.GT.1) THEN
                DO 610 L=1,M(IL-2)
                  ML=JNINDX(IL-2,L,0)
                  MKL=M(IL-1)+(K-1)*M(IL-2)+L
                  D2E(IOFST1+J,IOFST2+MKL)=
     &            D2E(IOFST1+J,IOFST2+MKL)+FACTOR*O(ML)
                  D2E(IOFST1+MJK,IOFST2+MKL)=
     &            D2E(IOFST1+MJK,IOFST2+MKL)+FACT2*O(ML)
610             CONTINUE
              ELSE
                DO 620 L=1,M(0)
                  MKL=M(1)+(K-1)*M(0)+L
                  D2E(IOFST1+J,IOFST2+MKL)=
     &            D2E(IOFST1+J,IOFST2+MKL)+FACTOR*OIN(L)
                  D2E(IOFST1+MJK,IOFST2+MKL)=
     &            D2E(IOFST1+MJK,IOFST2+MKL)+FACT2*OIN(L)
620             CONTINUE
              ENDIF
              DO 630 K2=1,M(IL-1)
                MK2=JNINDX(IL-1,K2,0)
                MJK2=M(IL)+(K-1)*M(IL-1)+K2
                TERM=FACTOR*O(MK2)
                D2E(IOFST1+MJK2,IOFST2+K)=D2E(IOFST1+MJK2,IOFST2+K)+
     &                                    TERM
                IF (IL-1.GT.1) THEN
                  DO 640 L=1,M(IL-2)
                    ML=JNINDX(IL-2,L,0)
                    MKL=M(IL-1)+(K-1)*M(IL-2)+L
                    D2E(IOFST1+MJK2,IOFST2+MKL)=
     &              D2E(IOFST1+MJK2,IOFST2+MKL)+TERM*O(ML)
640               CONTINUE
                ELSE
                  DO 650 L=1,M(0)
                    MKL=M(1)+(K-1)*M(0)+L
                    D2E(IOFST1+MJK2,IOFST2+MKL)=
     &              D2E(IOFST1+MJK2,IOFST2+MKL)+TERM*OIN(L)
650               CONTINUE
                ENDIF
630           CONTINUE

580         CONTINUE
570       CONTINUE
560     CONTINUE
C...End of 1st off-diagonal.

C...Higher off-diagonals - output layer:
        IOFST2=M(NL)*(1+M(NL-1))
        DO 660 IL=NL-2,1,-1
          IOFST2=IOFST2+M(IL+1)*(1+M(IL))
          DO 670 K=1,M(IL)
            MK=JNINDX(IL,K,0)
            FACTOR=GAMCAP*Q(MI,MK)*GPJN(MK)

            D2E(I,IOFST2+K)=D2E(I,IOFST2+K)+FACTOR
            DO 690 J=1,M(NL-1)
              MJ=JNINDX(NL-1,J,0)
              MJK=M(NL)+(I-1)*M(NL-1)+J
              FACT2=GAM*GPJN(MJ)*Q(MJ,MK)*GPJN(MK)
              D2E(MJK,IOFST2+K)=D2E(MJK,IOFST2+K)+FACT2
              IF (IL.GT.1) THEN
                DO 700 L=1,M(IL-1)
                  ML=JNINDX(IL-1,L,0)
                  MKL=M(IL)+(K-1)*M(IL-1)+L
                  D2E(I,IOFST2+MKL)=
     &            D2E(I,IOFST2+MKL)+FACTOR*O(ML)
                  D2E(MJK,IOFST2+MKL)=
     &            D2E(MJK,IOFST2+MKL)+FACT2*O(ML)
700             CONTINUE
              ELSE
                DO 710 L=1,M(0)
                  MKL=M(1)+(K-1)*M(0)+L
                  D2E(I,IOFST2+MKL)=
     &            D2E(I,IOFST2+MKL)+FACTOR*OIN(L)
                  D2E(MJK,IOFST2+MKL)=
     &            D2E(MJK,IOFST2+MKL)+FACT2*OIN(L)
710             CONTINUE
              ENDIF
690         CONTINUE
670       CONTINUE
660     CONTINUE

C...Higher off-diagonals - other layers:
        IOFST1=0
        DO 720 IL1=NL-1,2,-1
          IOFST1=IOFST1+M(IL1+1)*(1+M(IL1))
          IOFST2=M(NL)*(1+M(NL-1))
          DO 730 IL2=IL1-2,1,-1
            IOFST2=IOFST2+M(IL2+1)*(1+M(IL2))
            DO 740 J=1,M(IL1)
              MJ=JNINDX(IL1,J,0)
              DO 750 L=1,M(IL2)
                ML=JNINDX(IL2,L,0)
                FACTOR=GAMCAP*Q(MI,ML)*GPJN(ML)*Q(MI,MJ)*GPJN(MJ)+
     &                 GAM*Q(MI,MJ)*GPPJN(MJ)*Q(MJ,ML)*GPJN(ML)
                IF (IL1.LE.NL-2) THEN
                  SUM=0.0
                  DO 760 IL3=NL-1,IL1+1,-1
                    DO 770 J2=1,M(IL3)
                      MJ2=JNINDX(IL3,J2,0)
                      SUM=SUM+Q(MI,MJ2)*GPPJN(MJ2)*Q(MJ2,MJ)*Q(MJ2,ML)
770                 CONTINUE
760               CONTINUE
                  FACTOR=FACTOR+SUM*GAM*GPJN(ML)*GPJN(MJ)
                ENDIF

                D2E(IOFST1+J,IOFST2+L)=D2E(IOFST1+J,IOFST2+L)+
     &                                 FACTOR
                DO 780 K=1,M(IL1-1)
                  MK=JNINDX(IL1-1,K,0)
                  MKL=M(IL1)+(J-1)*M(IL1-1)+K
                  FACT2=GAM*Q(MI,MJ)*GPJN(MJ)*GPJN(MK)*Q(MK,ML)*GPJN(ML)
                  D2E(IOFST1+MKL,IOFST2+L)=D2E(IOFST1+MKL,IOFST2+L)+
     &                                     FACT2
                  IF (IL2.GT.1) THEN
                    DO 790 M1=1,M(IL2-1)
                      MM=JNINDX(IL2-1,M1,0)
                      MLM=M(IL2)+(L-1)*M(IL2-1)+M1
                      D2E(IOFST1+J,IOFST2+MLM)=
     &                D2E(IOFST1+J,IOFST2+MLM)+FACTOR*O(MM)
                      D2E(IOFST1+MKL,IOFST2+MLM)=
     &                D2E(IOFST1+MKL,IOFST2+MLM)+FACT2*O(MM)
790                 CONTINUE
                  ELSE
                    DO 800 M1=1,M(0)
                      MLM=M(1)+(L-1)*M(0)+M1
                      D2E(IOFST1+J,IOFST2+MLM)=
     &                D2E(IOFST1+J,IOFST2+MLM)+FACTOR*OIN(M1)
                      D2E(IOFST1+MKL,IOFST2+MLM)=
     &                D2E(IOFST1+MKL,IOFST2+MLM)+FACT2*OIN(M1)
800                 CONTINUE
                  ENDIF
780             CONTINUE

750           CONTINUE
740         CONTINUE
730       CONTINUE
720     CONTINUE

300   CONTINUE
C...End of loop over outputs.


C...Add Jacobian part:
      DO 900 I=1,M(NL)
        DO 901 J=1,M(NL)
          DD(J)=0.0
          DO 902 K=1,M(NL-1)
            DD(M(NL)+(J-1)*M(NL-1)+K)=0.0
902       CONTINUE
901     CONTINUE

        MI=JNINDX(NL,I,0)
        D(MI)=GPJN(MI)

        DO 910 IL=NL-1,1,-1

          DO 920 J=1,M(IL)
            MJ=MV0(IL)+J
            SUM=0.0
            IF (IL.LT.NL-1) THEN
              MIJ=MM0(IL+1)+(J-1)*M(IL+1)
              DO 930 II=MV0(IL+1)+1,MV0(IL+1)+M(IL+1)
                MIJ=MIJ+1
                SUM=SUM+D(II)*W(MIJ)
930           CONTINUE
              D(MJ)=SUM*GPJN(MJ)
            ELSE
              MIJ=JNINDX(NL,I,J)
              D(MJ)=D(MI)*W(MIJ)*GPJN(MJ)
            ENDIF
920       CONTINUE

910     CONTINUE

        DD(I)=D(MI)

        IF (NL.EQ.1) THEN
          DO 940 J=1,M(0)
            DD(M(1)+(I-1)*M(0)+J)=D(MI)*OIN(J)
940       CONTINUE
        ELSE
          DO 950 J=1,M(NL-1)
            MJ=MV0(NL-1)+J
            DD(M(NL)+(I-1)*M(NL-1)+J)=D(MI)*O(MJ)
            DD(M(NL)+M(NL)*M(NL-1)+J)=D(MJ)
950       CONTINUE
          IOFST=M(NL)+M(NL)*M(NL-1)+M(NL-1)
          DO 960 IL=NL-2,1,-1
            DO 970 K=1,M(IL)
              MK=MV0(IL)+K
              INDX=IOFST+M(IL)*M(IL+1)+K
              DD(INDX)=D(MK)
              DO 980 J=1,M(IL+1)
                MJ=MV0(IL+1)+J
                INDX=IOFST+(J-1)*M(IL)+K
                DD(INDX)=D(MJ)*O(MK)
980           CONTINUE
970         CONTINUE
            IOFST=IOFST+M(IL)*M(IL+1)+M(IL)
960       CONTINUE
          DO 990 K=1,M(0)
            DO 1000 J=1,M(1)
              MJ=MV0(1)+J
              INDX=IOFST+(J-1)*M(0)+K
              DD(INDX)=D(MJ)*OIN(K)
1000        CONTINUE
990       CONTINUE
        ENDIF

        DO 1010 IW=1,NWGTS
          DO 1020 IV=IW,NWGTS
            D2E(IW,IV)=D2E(IW,IV)+DD(IW)*DD(IV)
1020      CONTINUE
1010    CONTINUE

900   CONTINUE

10    IF (MOD(MSTJN(39),(MSTJN(2)*MSTJN(9))).EQ.0) THEN
C...Symmetrize and normalize the Hessian.
        NWGTS=MM0(NL+1)+MV0(NL+1)
        DO 20 I=1,NWGTS
          D2E(I,I)=D2E(I,I)/FLOAT(MSTJN(2)*MSTJN(9))
          DO 30 J=I+1,NWGTS
            D2E(I,J)=D2E(I,J)/FLOAT(MSTJN(2)*MSTJN(9))
            D2E(J,I)=D2E(I,J)
30        CONTINUE
20      CONTINUE
      ENDIF

      RETURN

C**** END OF JNHESS ****************************************************
      END
C***********************************************************************


      INTEGER FUNCTION JNINDX(IL,I,J)

C...JetNet function INDeX

C...Gives the node vector index of node I in layer IL for J=0
C...else gives the weight vector index of weight between node  
C...I of layer IL and node J of layer IL-1

      PARAMETER(MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT3/ NXIN,NYIN,NXRF,NYRF,NXHRF,NYHRF,NHRF,NRFW,NHPRF
      SAVE /JNDAT1/,/JNINT2/,/JNINT3/


      IF(MSTJN(8).EQ.0) CALL JNERR(5)

      IF(J.EQ.0) THEN
        JNINDX=MV0(IL)+I
      ELSE
        IF(NXIN.EQ.0.OR.IL.GT.1) THEN
          JNINDX=MM0(IL)+(J-1)*M(IL)+I
        ELSE
          IF(I.LE.NHRF*NHPRF) THEN
            IF(J.LE.ABS(NXIN*NYIN)) THEN
              IX=(I-1)/NYHRF+1
              IY=MOD(I-1,NYHRF)+1
              INX=(J-1)/ABS(NYIN)+1
              INY=MOD(J-1,ABS(NYIN))+1
              JX=INX-IX+1
              IF(JX.LE.0) JX=JX+NXRF
              IF(JX.LE.0) CALL JNERR(12)
              IF(JX.GT.NXRF) CALL JNERR(12)
              JY=INY-IY+1
              IF(JY.LE.0) JY=JY+NYRF
              IF(JY.LE.0) CALL JNERR(12)
              IF(JY.GT.NYRF) CALL JNERR(12)
              JNINDX=(JX-1)*NYRF+JY+((I-1)/NHRF)*NRFW
            ELSE
              JNINDX=NXRF*NYRF+J-ABS(NXIN*NYIN)+((I-1)/NHRF)*NRFW
            ENDIF
          ELSE
            JNINDX=NHPRF*NRFW+(J-1)*M(1)+I-NXHRF*NYHRF
          ENDIF
        ENDIF
      ENDIF

      RETURN

C**** END OF JNINDX ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNINIT

C...JetNet subroutine INITialize net

C...Initializes a net according to switches and parameters in 
C.../JNDAT1/ and /JNDAT2/

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT4/ ILINON,NC,G2,NIT,ERRLN(0:3),DERRLN,STEPLN(0:3),
     &                STEPMN,ERRMN,IEVAL,ISUCC,ICURVE,NSC,GVEC2
      COMMON /JNINT3/ NXIN,NYIN,NXRF,NYRF,NXHRF,NYHRF,NHRF,NRFW,NHPRF
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/,/JNINT3/,/JNINT4/


C...Check if JMINIT has been called

      IF(MSTJM(8).EQ.1) CALL JNERR(16)

C...Set parameters in /JNINT2/

      CALL JNSEPA

C...Set initial values of weights and thresholds

      DO 100 IL=1,NL

C...Set width in this layer

        IF(WIDL(IL).LE.0) THEN
          WIDTH=PARJN(4)
        ELSE
          WIDTH=WIDL(IL)
        ENDIF

C...Initialize weights

        DO 110 I=MM0(IL)+1,MM0(IL+1)
          IDUM=I
          IF (WIDTH.GE.0.) THEN
            W(I)=(2.0*RJN(IDUM)-1.0)*WIDTH
          ELSE
            W(I)=-RJN(IDUM)*WIDTH
          ENDIF
110     CONTINUE

C...Initialize thresholds

        DO 120 I=MV0(IL)+1,MV0(IL+1)
          IDUM=I
          IF (WIDTH.GE.0.) THEN
            T(I)=(2.0*RJN(IDUM)-1.0)*WIDTH
          ELSE
            T(I)=-RJN(IDUM)*WIDTH
          ENDIF
120     CONTINUE

100   CONTINUE

      IF(NXIN.NE.0) THEN
        DO 130 IHPRF=1,NHPRF
          DO 140 I=2,NHRF
            T((IHPRF-1)*NHRF+I)=T((IHPRF-1)*NHRF+1)
140       CONTINUE
130     CONTINUE

        IF(MSTJN(27).LT.0) THEN
          DO 150 I=1,M(2)
            DO 160 IHPRF=1,NHPRF
              MIJ=MM0(2)+(IHPRF-1)*NHRF*M(2)+I
              SUMRFW=W(MIJ)
              DO 170 J=2,NHRF
                MIJ=MIJ+M(2)
                W(MIJ)=SUMRFW
170           CONTINUE
160         CONTINUE
150       CONTINUE
        ENDIF

      ENDIF

C...Write statistics on output file

      IF(MSTJN(6).LT.0) RETURN

      CALL JNHEAD

      CALL JNSTAT(1)

      WRITE(MSTJN(6),600)

600   FORMAT(22X,'Weights and thresholds set randomly')

      RETURN

C**** END OF JNINIT ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNLINS

C...JetNet subroutine do LINe Search.

C...Performs a line search in the direction of G.
C...The algorithm is a mixture between 'golden section search' and
C...quadratic interpolation. Termination of the search is controlled
C...by either of two criteria: (1) If the error has decreased 
C...sufficiently much - set by PARJN(24); or (2) if the predicted
C...location of the error is within the preset - PARJN(25) - tolerance
C...distance from the current best point.
C...The first step is always equal to PARJN(1), but PARJN(1) is set to
C...about half the size of the last successful step every time
C...the algorithm finds a minimum (provided that this step size is
C...smaller than the maximum allowed step size).

C...ERRLN(1) = error value in current point.
C...ERRLN(2-3) = error values in previous points.
C...ERRLN(0) = error value in the starting point.
C...STEPLN(1) = step to be taken (the current point is always at x=0).
C...STEPLN(2-3) = distance to previous points.
C...STEPLN(0) = distance to starting point.
C...STEPMN = distance to best minimum so far.
C...PARJN(26)=minimum allowed relative change in error.
C...PARJN(27)=maximum allowed step size.


      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)
      PARAMETER(GOLD=1.618034,CGOLD=0.3819660,GLIMIT=10.0,
     &          TINY=1.E-20,ZEPS=1.E-8)

C...ZEPS=machine precision.
C...TINY=small number to prevent division by zero.

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT4/ ILINON,NC,G2,NIT,ERRLN(0:3),DERRLN,STEPLN(0:3),
     &                STEPMN,ERRMN,IEVAL,ISUCC,ICURVE,NSC,GVEC2
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/,/JNINT4/


      IF (MSTJN(5).EQ.8) THEN
C...Back up to best point so far and terminate updating.
        IF (PARJN(8).LE.ERRMN) THEN
          ERRMN=PARJN(8)
          STEPMN=0.0
        ENDIF
        STEPLN(1)=-STEPMN
C...Freeze updating:
        MSTJN(5)=9
        ILINON=0
        NC=0
        NIT=0
        NSC=0
        GOTO 20
      ENDIF

      NIT=NIT+1
      IF (NIT.GT.MSTJN(35)) THEN
C...Too many iterations -> restart from best minimum so far
C...and rescale PARJN(1) according to previous success.
        IF (MSTJN(38).GE.MSTJN(36)) CALL JNERR(21)
        MSTJN(38)=MSTJN(38)+1
        IF (ILINON.GT.0) THEN
          IF (ERRLN(1).LE.ERRLN(0)) THEN
            IF (ABS(STEPLN(0)).GT.TINY) THEN
              PARJN(1)=MIN(MAX(ABS(STEPLN(0)),
     &                     PARJN(1)*GOLD),PARJN(27))
            ELSE
              PARJN(1)=MIN(PARJN(1)*GOLD,PARJN(27))
            ENDIF
          ELSE
            PARJN(1)=PARJN(1)*CGOLD
          ENDIF
        ELSE
          PARJN(1)=PARJN(1)*CGOLD
        ENDIF
        PARJN(1)=MAX(PARJN(1),ZEPS)
        STEPLN(1)=-STEPMN
        ILINON=0
        NC=0
        NIT=0
        GOTO 20
      ENDIF

      ETA=0.0
10    IF (NIT.LT.3) THEN
C...At least 3 points are needed

        ERRLN(2)=ERRLN(1)
C...Store last updated error 
        ERRLN(1)=PARJN(8)

        IF (NIT.EQ.1) THEN
          ERRLN(0)=ERRLN(1)
          ERRMN=ERRLN(1)
          STEPLN(0)=0.0
          STEPMN=0.0
          DO 100 I=2,3
            STEPLN(I)=0.0
            ERRLN(I)=0.0
100       CONTINUE
          IF (ABS(DERRLN).GT.TINY) THEN
            STEPLN(1)=-SIGN(PARJN(1),DERRLN)
          ELSE
            STEPLN(1)=PARJN(1)
          ENDIF
        ELSE
          STEPLN(2)=-STEPLN(1)
          DE2=2.*(ERRLN(1)-ERRLN(2)+STEPLN(2)*DERRLN)/STEPLN(2)**2
          IF (ABS(DERRLN/STEPLN(2)).LT.(DE2*GLIMIT)) THEN
            STEPLN(1)=-DERRLN/DE2
          ELSE
            STEPLN(1)=STEPLN(1)*GOLD
          ENDIF
        ENDIF

        IF (ERRLN(1).LT.ERRMN) THEN
          STEPMN=0.0
          ERRMN=ERRLN(1)
        ENDIF

      ELSEIF (ILINON.GT.0) THEN
C...Bracket the minimum

C...Update error and step values:
        ERRLN(3)=ERRLN(2)
        ERRLN(2)=ERRLN(1)
        STEPLN(3)=-STEPLN(1)+STEPLN(2)
        STEPLN(2)=-STEPLN(1)
        STEPLN(1)=0.0
        ERRLN(1)=PARJN(8)
        IF (ERRLN(1).LT.ERRMN) THEN
          STEPMN=0.0
          ERRMN=ERRLN(1)
        ENDIF

C...Check if the search is improving - else take default step.
        IF (ABS(1.-ERRLN(0)/ERRLN(1)).LT.PARJN(26)) THEN
          STEPLN(1)=-STEPLN(2)*GOLD
          GOTO 20
        ENDIF

C...Quadratic fit
        IF (((ERRLN(1)-ERRLN(3))*TINY).GT.STEPLN(3)) THEN
          FACTOR=-1.0
        ELSE
          BC=((ERRLN(1)-ERRLN(3))/(STEPLN(3)+TINY) -
     &       (ERRLN(1)-ERRLN(2))/(STEPLN(2)+TINY))/
     &       (STEPLN(2)-STEPLN(3)+TINY)
          AC=-BC*STEPLN(2)-(ERRLN(1)-ERRLN(2))/(STEPLN(2)+TINY)
          ETA=-AC/(2.*BC+TINY)
          IF (ABS(ETA).GT.TINY) THEN
            FACTOR=(ERRLN(1)-ERRLN(2))/
     &             (STEPLN(2)*(2.*ETA-STEPLN(2))+TINY)
          ELSE
            FACTOR=-1.0
          ENDIF
        ENDIF

        IF (ERRLN(1).LT.ERRLN(2)) THEN
          IF (ERRLN(1).LT.ERRLN(3)) THEN
            IF (STEPLN(2)*STEPLN(3).LT.0.) THEN
C...Minimum is bracketed -> find it
              ILINON=-1
              GOTO 10
            ELSE
C...Keep searching:
              IF (FACTOR.GT.0.) THEN
C...Quadratic fit OK.
                IF ((ABS(ETA/STEPLN(2)).LT.GLIMIT).OR.
     &              (ABS(ETA/STEPLN(3)).LT.GLIMIT)) THEN
                  STEPLN(1)=ETA
                ELSE
                  STEPLN(1)=-STEPLN(2)*GOLD
                ENDIF
              ELSE
                STEPLN(1)=-STEPLN(2)*GOLD
              ENDIF
            ENDIF
          ELSE
            IF (STEPLN(2)/STEPLN(3).GT.1.) THEN
C...Back up to point (3)
              STEPLN(1)=STEPLN(3)
            ELSE
C...Back up beyond point (3)
              STEPLN(1)=GOLD*STEPLN(3)
              STEPLN(2)=STEPLN(3)
              ERRLN(2)=ERRLN(3)
            ENDIF
          ENDIF
        ELSEIF (ERRLN(1).GT.ERRLN(2)) THEN
          IF (ERRLN(2).LT.ERRLN(3)) THEN
            IF (STEPLN(3)/STEPLN(2).GT.1.) THEN
C...Minimum is bracketed -> back up towards point (2)
              IF ((ETA/STEPLN(2).GT.0.).AND.
     &           (ETA/STEPLN(2).LT.1.)) THEN
                STEPLN(1)=ETA
              ELSE
                STEPLN(1)=STEPLN(2)
                STEPLN(2)=STEPLN(3)
                ERRLN(2)=ERRLN(3)
              ENDIF
              ILINON=-2
            ELSE
              STEPLN(1)=STEPLN(2)*GOLD
            ENDIF
          ELSE
C...Rearrange and move beyond point (3)
            STEPLN(1)=STEPLN(3)*GOLD
            STEPLN(2)=STEPLN(3)
            ERRLN(2)=ERRLN(3)
          ENDIF
        ELSE
C...Take default step
          STEPLN(1)=-STEPLN(2)*GOLD
        ENDIF

      ELSEIF (ILINON.LT.0) THEN
C...Find minimum (knowing that minimum is bracketed)
        ERRNOW=PARJN(8)
        IF (ERRNOW.LT.ERRMN) THEN
          STEPMN=0.0
          ERRMN=ERRNOW
        ENDIF
C...Check bracket condition:
        IF ((ERRNOW.GE.ERRLN(2)).OR.(ERRNOW.GE.ERRLN(3))) THEN
          ILINON=1
          GOTO 10
        ENDIF

        IF ((ERRLN(0)-ERRNOW).LE.
     &       PARJN(24)*STEPLN(0)*DERRLN) THEN
C...Satisfactory -> terminate search
          NIT=0
          ILINON=0
          IF (ABS(STEPLN(0)).GT.ZEPS) THEN
            PARJN(1)=MAX(MIN(ABS(STEPLN(0))*CGOLD,PARJN(27)),ZEPS)
          ENDIF
          IF (ERRNOW.GT.ERRMN) THEN
C...Back up to best minimum so far
            STEPLN(1)=-STEPMN
            GOTO 20
          ELSE
            MSTJN(37)=0
            RETURN
          ENDIF
        ELSE
          IF (ILINON.NE.-1) THEN
C...Rearrange points:
            IF (ERRNOW.LE.ERRLN(1)) THEN
              IF (STEPLN(1)/(STEPLN(2)+TINY).GT.0.) THEN
                STEPLN(3)=-STEPLN(1)
                ERRLN(3)=ERRLN(1)
                STEPLN(2)=STEPLN(2)-STEPLN(1)
                STEPLN(1)=0.0
                ERRLN(1)=ERRNOW
              ELSE
                STEPLN(3)=STEPLN(3)-STEPLN(1)
                STEPLN(2)=-STEPLN(1)
                ERRLN(2)=ERRLN(1)
                STEPLN(1)=0.0
                ERRLN(1)=ERRNOW
              ENDIF
            ELSE
              IF (STEPLN(1)/(STEPLN(2)+TINY).GT.0.) THEN
                STEPLN(2)=STEPLN(1)
                ERRLN(2)=ERRNOW
                STEPLN(1)=0.0
              ELSE
                STEPLN(3)=STEPLN(1)
                ERRLN(3)=ERRNOW
                STEPLN(1)=0.0
              ENDIF
            ENDIF
          ELSE
            STEPLN(1)=0.0
            ILINON=-2
          ENDIF
C...Quadratic fit
          IF (((ERRLN(1)-ERRLN(3))*TINY).GT.STEPLN(3)) THEN
            FACTOR=-1.0
          ELSE
            BC=((ERRLN(1)-ERRLN(3))/(STEPLN(3)+TINY) -
     &         (ERRLN(1)-ERRLN(2))/(STEPLN(2)+TINY))/
     &         (STEPLN(2)-STEPLN(3)+TINY)
            AC=-BC*STEPLN(2)-(ERRLN(1)-ERRLN(2))/(STEPLN(2)+TINY)
            ETA=-AC/(2.*BC+TINY)
            IF (ABS(ETA).GT.TINY) THEN
              FACTOR=(ERRLN(1)-ERRLN(2))/
     &               (STEPLN(2)*(2.*ETA-STEPLN(2))+TINY)
            ELSE
              FACTOR=-1.0
            ENDIF
          ENDIF
C..Tolerance:
          TOL=MAX(PARJN(25),ZEPS)
          IF (FACTOR.GT.0.) THEN
C...Quadratic fit OK
            IF (ETA/(STEPLN(2)+TINY).GT.0.) THEN
              IF ((ETA/(STEPLN(2)+TINY).LT.1.).AND.
     &            (ABS(ETA-STEPLN(2)).GT.TOL)) THEN
                STEPLN(1)=ETA
              ELSE
                STEPLN(1)=CGOLD*STEPLN(2)
              ENDIF
            ELSEIF (ETA/(STEPLN(3)+TINY).GT.0.) THEN
              IF ((ETA/(STEPLN(3)+TINY).LT.1.).AND.
     &            (ABS(ETA-STEPLN(3)).GT.TOL)) THEN
                STEPLN(1)=ETA
              ELSE
                STEPLN(1)=CGOLD*STEPLN(3)
              ENDIF
            ELSE
C...Step too large -> decrease
              STEPLN(1)=CGOLD*SIGN(MIN(STEPLN(2),STEPLN(3)),ETA)
            ENDIF
          ELSE
C...Take step towards the most distant of points (2) and (3)
            IF (STEPLN(2)/(STEPLN(3)+TINY).LT.-1.) THEN
              STEPLN(1)=CGOLD*STEPLN(2)
            ELSE
              STEPLN(1)=CGOLD*STEPLN(3)
            ENDIF
          ENDIF
        ENDIF
        IF (ABS(STEPLN(1)).LE.TOL) THEN
C...Predicted step less than tolerance from current point
C...                                  -> terminate search
          NIT=0
          ILINON=0
          IF (ABS(STEPLN(0)).GT.ZEPS) THEN
            PARJN(1)=MAX(MIN(ABS(STEPLN(0))*CGOLD,PARJN(27)),ZEPS)
          ENDIF
          IF (ERRNOW.GT.ERRMN) THEN
C...Back up to best minimum so far
            STEPLN(1)=-STEPMN
          ELSE
            MSTJN(37)=ABS(ILINON)
            RETURN
          ENDIF
        ENDIF
      ENDIF

20    CONTINUE
C...Update weight vector:
      DO 110 I=1,MM0(NL+1)
        W(I)=W(I)+STEPLN(1)*G(I)*FLOAT(NSELF(I))
110   CONTINUE
      DO 120 I=1,MV0(NL+1)
        T(I)=T(I)+STEPLN(1)*G(I+MM0(NL+1))*FLOAT(NTSELF(I))
120   CONTINUE
C...Keep track of starting point and best point up to now:
      STEPLN(0)=STEPLN(0)+STEPLN(1)
      STEPMN=STEPMN+STEPLN(1)

      MSTJN(37)=ABS(ILINON)
      RETURN

C**** END OF JNLINS ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNREAD(NF)

C...JetNet subroutine READ weights and parameters.

C...Reads weights, thresholds and other statistics from a file NF and
C...initializes the net

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT3/ NXIN,NYIN,NXRF,NYRF,NXHRF,NYHRF,NHRF,NRFW,NHPRF
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/,/JNINT3/

      CHARACTER LINE*100
      DIMENSION MST(6),PAR(21)


      IF(NF.LT.0) THEN

C...unformatted read

        NFSAVE=MSTJN(6)

        JF=-NF
        READ(JF) IVERS
        IF(IVERS.LT.0) CALL JNERR(17)
        IF(IVERS.LT.20) CALL JNERR(13)
        IF(IVERS/10.EQ.2) THEN
C...New meanings for MSTJN(35-) and PARJN(20-)
          DO 400 I=1,6
            MST(I)=MSTJN(34+I)
400       CONTINUE
          DO 410 I=1,21
            PAR(I)=PARJN(19+I)
410       CONTINUE
        ENDIF
        READ(JF) MSTJN,PARJN,TINV,IGFN,ETAL,WIDL,SATM

        IF(IVERS/10.EQ.2) THEN
          DO 420 I=1,6
            MSTJN(34+I)=MST(I)
420       CONTINUE
          DO 430 I=1,21
            PARJN(19+I)=PAR(I)
430       CONTINUE
        ENDIF

        CALL JNSEPA

        DO 100 I=1,MM0(NL+1)
          READ(JF) W(I)
100     CONTINUE

        DO 110 I=1,MV0(NL+1)
          READ(JF) T(I)
110     CONTINUE

        DO 120 I=1,MM0(NL+1)
          READ(JF) NSELF(I)
120     CONTINUE

        DO 130 I=1,MV0(NL+1)
          READ(JF) NTSELF(I)
130     CONTINUE

        MSTJN(6)=NFSAVE

      ELSE

C...Formatted dump

        READ(NF,690)LINE
        IF (LINE(27:28).EQ.' D') CALL JNERR(17)
        READ(NF,*)
        READ(NF,*)
        READ(NF,650)FVERS
        IVERS=INT(FVERS*10.0+0.001)
        IF(IVERS.LT.20) CALL JNERR(13)
        IF(IVERS/10.EQ.2) THEN
C...New meanings for MSTJN(35-) and PARJN(20-)
          DO 440 I=1,6
            MST(I)=MSTJN(34+I)
440       CONTINUE
          DO 450 I=1,21
            PAR(I)=PARJN(19+I)
450       CONTINUE
        ENDIF

900     READ(NF,690) LINE
        IF(LINE(1:24).NE.'        I       1      2') GOTO 900

        NFSAVE=MSTJN(6)

        READ(NF,601)(MSTJN(I),I=1,6),TRN,(MSTJN(I),I=8,10)
        MSTJN(7)=INT(10**TRN+0.5)
        READ(NF,600)(MSTJN(10+I),I=1,10)
        READ(NF,600)(MSTJN(20+I),I=1,10)
        READ(NF,600)(MSTJN(30+I),I=1,10)
        READ(NF,610)(PARJN(I),I=1,10)
        READ(NF,610)(PARJN(10+I),I=1,10)
        READ(NF,610)(PARJN(20+I),I=1,10)
        READ(NF,610)(PARJN(30+I),I=1,10)
        PARJN(22)=10.**PARJN(22)
        READ(NF,600)(IGFN(I),I=1,10)
        READ(NF,610)(TINV(I),I=1,10)
        READ(NF,610)(ETAL(I),I=1,10)
        READ(NF,610)(WIDL(I),I=1,10)
        READ(NF,610)(SATM(I),I=1,10)
        READ(NF,*)

        MSTJN(6)=NFSAVE

        IF(IVERS/10.EQ.2) THEN
          DO 460 I=1,6
            MSTJN(34+I)=MST(I)
460       CONTINUE
          DO 470 I=1,21
            PARJN(19+I)=PAR(I)
470       CONTINUE
        ENDIF

        CALL JNSEPA
     
        READ(NF,*)
        READ(NF,*)
        READ(NF,*)

        IF(NXIN.EQ.0) THEN

          READ(NF,*)
          DO 200 J=1,M(0)
            READ(NF,*)
            READ(NF,620)(W(JNINDX(1,I,J)),
     &                    LINE(I:I),I=1,M(1))
            DO 210 I=1,M(1)
              IF(LINE(I:I).EQ.'*') THEN
                NSELF(JNINDX(1,I,J))=0
              ELSE
                NSELF(JNINDX(1,I,J))=1
              ENDIF
210         CONTINUE

200       CONTINUE

        ELSE


          READ(NF,*)
          DO 220 IHPRF=1,NHPRF
            READ(NF,*)
            READ(NF,620)(W(IW),LINE(IW:IW),
     &                     IW=NRFW*(IHPRF-1)+1,NRFW*IHPRF)
            DO 230 IW=NRFW*(IHPRF-1)+1,NRFW*IHPRF
              IF(LINE(IW:IW).EQ.'*') THEN
                NSELF(IW)=0
              ELSE
                NSELF(IW)=1
              ENDIF
230         CONTINUE
220       CONTINUE

          IF(NHRF*NHPRF.LT.M(1)) THEN
            READ(NF,*)
            READ(NF,*)
            DO 240 J=1,M(0)
              READ(NF,*)
              READ(NF,620)(W(JNINDX(1,I,J)),LINE(I:I),
     &                                I=NHRF*NHPRF+1,M(1))
              DO 250 I=NHRF*NHPRF+1,M(1)
                IF(LINE(I:I).EQ.'*') THEN
                  NSELF(JNINDX(1,I,J))=0
                ELSE
                  NSELF(JNINDX(1,I,J))=1
                ENDIF
250           CONTINUE
240         CONTINUE
          ENDIF

        ENDIF

        READ(NF,*)
        READ(NF,*)
        READ(NF,*)
        READ(NF,620)(T(JNINDX(1,I,0)),LINE(I:I),I=1,M(1))
        DO 260 I=1,M(1)
          IF(LINE(I:I).EQ.'*') THEN
            NTSELF(JNINDX(1,I,0))=0
          ELSE
            NTSELF(JNINDX(1,I,0))=1
          ENDIF
260     CONTINUE

        DO 300 IL=2,NL

          READ(NF,*)
          READ(NF,*)
          DO 310 J=1,M(IL-1)
            READ(NF,*)
            READ(NF,620)(W(JNINDX(IL,I,J)),LINE(I:I),I=1,M(IL))
            DO 320 I=1,M(IL)
              IF(LINE(I:I).EQ.'*') THEN
                NSELF(JNINDX(IL,I,J))=0
              ELSE
                NSELF(JNINDX(IL,I,J))=1
              ENDIF
320         CONTINUE
310       CONTINUE

          READ(NF,*)
          READ(NF,*)
          READ(NF,*)
          READ(NF,620)(T(JNINDX(IL,I,0)),LINE(I:I),I=1,M(IL))
          DO 330 I=1,M(IL)
            IF(LINE(I:I).EQ.'*') THEN
              NTSELF(JNINDX(IL,I,0))=0
            ELSE
              NTSELF(JNINDX(IL,I,0))=1
            ENDIF
330       CONTINUE

300    CONTINUE

      ENDIF

C...Write statistics on output file

      IF(MSTJN(6).LT.0) RETURN

      CALL JNHEAD

      CALL JNSTAT(1)

      WRITE(MSTJN(6),640)

600   FORMAT(TR11,10I7)
601   FORMAT(TR11,6I7,F7.3,3I7)
610   FORMAT(TR11,10F7.4)
620   FORMAT(6(F12.4,A1))
640   FORMAT(29X,'Weights read from file')
650   FORMAT(TR63,F3.1)
690   FORMAT(A)

      RETURN

C**** END OF JNREAD ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNROLD(NF)

C...JetNet subroutine Read weights from OLD versions
C...(JETNET 1.0 and 1.1)

C...Reads weights, thresholds and other statistics from a file NF and
C...initializes the net

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/

      CHARACTER*80 LINE
      DIMENSION MSTJNO(20),PARJNO(20)


C...MSTJN(9) has a new meaning:
      MSTJ9=MSTJN(9)

C...PARJN(5) has new meaning:
      PARJ5=PARJN(5)

C...PARJN(11-20) have new meanings:
      PARJ11=PARJN(11)
      PARJ12=PARJN(12)
      PARJ13=PARJN(13)
      PARJ14=PARJN(14)
      PARJ15=PARJN(15)
      PARJ16=PARJN(16)
      PARJ17=PARJN(17)
      PARJ18=PARJN(18)
      PARJ19=PARJN(19)
      PARJ20=PARJN(20)

      IF(NF.LT.0) THEN

C...unformatted read

        JF=-NF
        READ(JF) MSTJNO,PARJNO,TINV,IGFN

        DO 100 I=1,20
          MSTJN(I)=MSTJNO(I)
          PARJN(I)=PARJNO(I)
100     CONTINUE

        MSTJN(9)=MSTJ9
        PARJN(5)=PARJ5
        PARJN(11)=PARJ11
        PARJN(12)=PARJ12
        PARJN(13)=PARJ13
        PARJN(14)=PARJ14
        PARJN(15)=PARJ15
        PARJN(16)=PARJ16
        PARJN(17)=PARJ17
        PARJN(18)=PARJ18
        PARJN(19)=PARJ19
        PARJN(20)=PARJ20

        CALL JNSEPA

        DO 110 I=1,MM0(NL+1)
          READ(JF) W(I)
110     CONTINUE

        DO 120 I=1,MV0(NL+1)
          READ(JF) T(I)
120     CONTINUE

        DO 130 I=1,MM0(NL+1)
          READ(JF) NSELF(I)
130     CONTINUE

      ELSE

C...Formatted dump

        READ(NF,690)LINE
        IF (LINE(27:28).EQ.' D') CALL JNERR(18)
900     READ(NF,690) LINE
        IF(LINE(1:24).NE.'         I      1      2'.AND.
     &     LINE(1:25).NE.'          I      1      2') GOTO 900

        NFSAVE=MSTJN(6)

        READ(NF,600)(MSTJNO(I),I=1,10)
        READ(NF,600)(MSTJNO(10+I),I=1,10)
        READ(NF,610)(PARJNO(I),I=1,10)
        READ(NF,600)(IGFN(I),I=1,10)
        READ(NF,610)(TINV(I),I=1,10)
        READ(NF,*)

        DO 200 I=1,20
          MSTJN(I)=MSTJNO(I)
          PARJN(I)=PARJNO(I)
200     CONTINUE

        MSTJN(6)=NFSAVE
        MSTJN(9)=MSTJ9
        PARJN(5)=PARJ5
        PARJN(11)=PARJ11
        PARJN(12)=PARJ12
        PARJN(13)=PARJ13
        PARJN(14)=PARJ14
        PARJN(15)=PARJ15
        PARJN(16)=PARJ16
        PARJN(17)=PARJ17
        PARJN(18)=PARJ18
        PARJN(19)=PARJ19
        PARJN(20)=PARJ20

        CALL JNSEPA
     
        READ(NF,*)
        READ(NF,*)
        DO 210 IL=1,NL

          READ(NF,*)
          READ(NF,*)
          DO 220 J=1,M(IL-1)
            READ(NF,*)
            READ(NF,620)(W(JNINDX(IL,I,J)),LINE(I:I),I=1,M(IL))
            DO 230 I=1,M(IL)
              IF(LINE(I:I).EQ.'*') THEN
                NSELF(JNINDX(IL,I,J))=0
              ELSE
                NSELF(JNINDX(IL,I,J))=1
              ENDIF
230         CONTINUE
220       CONTINUE

         READ(NF,*)
         READ(NF,*)
         READ(NF,*)
         READ(NF,630)(T(JNINDX(IL,I,0)),I=1,M(IL))
210    CONTINUE

      ENDIF

C...Write statistics on output file

      IF(MSTJN(6).LT.0) RETURN

      CALL JNHEAD

      CALL JNSTAT(1)

      WRITE(MSTJN(6),640)

600   FORMAT(TR11,10I7)
610   FORMAT(TR11,10F7.4)
620   FORMAT(10(F7.4,A1))
630   FORMAT(10F8.4)
640   FORMAT(17X,'Weights read from file produced with version 1')
690   FORMAT(A)

      RETURN

C**** END OF JNROLD ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNSATM
C...JetNet subroutine SATuration Measure

C...Calculates the saturation measure "S" for each layer.
C...Note: The response function for the layer must be a sigmoid.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/


      DO 100 IL=1,NL

        IF (ABS(NG(IL)).EQ.1.OR.ABS(NG(IL)).EQ.5) THEN

          SUM=0.0
          DO 110 I=1,M(IL)
            MI=MV0(IL)+I
            SUM=SUM+(1.-2.*O(MI))**2
110       CONTINUE
          SM(IL)=SM(IL)+SUM/FLOAT(M(IL))

        ELSEIF (ABS(NG(IL)).EQ.2) THEN

          SUM=0.0
          DO 120 I=1,M(IL)
            MI=MV0(IL)+I
            SUM=SUM+O(MI)**2
120       CONTINUE
          SM(IL)=SM(IL)+SUM/FLOAT(M(IL))

        ELSE
          SM(IL)=0.0
        ENDIF

100   CONTINUE

      RETURN

C**** END OF JNSATM ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNSCGR

C...JetNet subroutine Scaled Conjugate GRadient

C...Performs the Scaled Conjugate Gradient updating.

C...The algorithm is described in:
C...M. F. Moller, "A Scaled Conjugate Gradient Algorithm for Fast
C...Supervised Learning", Neural Networks, Vol. 6, pp 525-533 (1993)

C...The following notation is used (cf. Moller's article):
C...S-vector = -(DW,DT)
C...R-vector = (ODW,ODT)
C...P-vector = G
C...K = NSC
C...MU=-DERRLN

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,MAXD=2)
      PARAMETER(ZEPS=1.E-8,TINY=1.E-20,XLAMX=1.0)

C...ZEPS=Machine precision

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT4/ ILINON,NC,G2,NIT,ERRLN(0:3),DERRLN,STEPLN(0:3),
     &                STEPMN,ERRMN,IEVAL,ISUCC,ICURVE,NSC,GVEC2
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/,/JNINT4/

      REAL LAMBDA

      EQUIVALENCE (ALPHA,STEPLN(1)),(LAMBDA,STEPLN(2)),
     &            (SIGMA,ERRLN(1)),(DELTA,ERRLN(2)),(CDELTA,ERRLN(3))


      IF (MSTJN(5).EQ.14) THEN
C...Move to last minimum and terminate the search.
        IF (PARJN(8).LE.ERRLN(0)) STEPLN(0)=0.0
        DO 400 I=1,MM0(NL+1)
          W(I)=W(I)-STEPLN(0)*G(I)
400     CONTINUE
        DO 410 I=1,MV0(NL+1)
          T(I)=T(I)-STEPLN(0)*G(I+MM0(NL+1))
410     CONTINUE
        NSC=0
        NIT=0
        NC=0
        ILINON=0
        MSTJN(5)=9
        MSTJN(37)=ABS(ILINON)
        RETURN
      ENDIF

      IF (IEVAL.EQ.1) GOTO 10

      IF (ISUCC.GT.0) THEN
C...Calculate 2nd order information:
        NSC=NSC+1
        NC=0

        IF (ICURVE.EQ.0) THEN
C...1st sweep -> Create new search direction and 
C...             Get curvature information

          ERRLN(0)=PARJN(8)

          CALL JNCGBE(BETAK,MOD((NSC-1),(MM0(NL+1)+MV0(NL+1))))
          DERRLN=0.0
          BETA=1.0
          GVEC2=0.0
          DO 100 IL=NL,1,-1

C...set effective beta in layer IL:
            IF(TINV(IL).EQ.0.0) THEN
              BETA=BETA*PARJN(3)
            ELSE
              BETA=BETA*ABS(TINV(IL))
            ENDIF

            DO 110 I=MM0(IL)+1,MM0(IL+1)
              G(I)=BETAK*G(I)+ODW(I)*FLOAT(NSELF(I))*BETA
              DERRLN=DERRLN-ODW(I)*FLOAT(NSELF(I))*BETA*G(I)
              GVEC2=GVEC2+G(I)**2
110         CONTINUE

            DO 120 I=MV0(IL)+1,MV0(IL+1)
              G(I+MM0(NL+1))=BETAK*G(I+MM0(NL+1))
     &                       +ODT(I)*FLOAT(NTSELF(I))*BETA
              DERRLN=DERRLN-ODT(I)*FLOAT(NTSELF(I))*BETA*G(I+MM0(NL+1))
              GVEC2=GVEC2+G(I+MM0(NL+1))**2
120         CONTINUE

100       CONTINUE

C...Initial value for lambda
          IF (NSC.EQ.1) LAMBDA=PARJN(29)

          NIT=1
          SIGMA=PARJN(28)/(SQRT(GVEC2)+TINY)
          FACTOR=FLOAT(MSTJN(2))
          DO 200 I=1,MM0(NL+1)
            DW(I)=-DW(I)*FACTOR
            W(I)=W(I)+SIGMA*G(I)
200       CONTINUE
          DO 210 I=1,MV0(NL+1)
            DT(I)=-DT(I)*FACTOR
            T(I)=T(I)+SIGMA*G(I+MM0(NL+1))
210       CONTINUE
          ICURVE=1
          MSTJN(37)=ABS(ILINON)
          STEPLN(0)=SIGMA
          RETURN
        ELSE
C...2nd sweep -> Curvature information exists
          DELTA=0.0
          FACTOR=SIGMA*FLOAT(MSTJN(2))+TINY
          DO 220 I=1,MM0(NL+1)
            DW(I)=-DW(I)/FACTOR
            DELTA=DELTA+G(I)*DW(I)
            W(I)=W(I)-SIGMA*G(I)
220       CONTINUE
          DO 230 I=1,MV0(NL+1)
            DT(I)=-DT(I)/FACTOR
            DELTA=DELTA+G(I+MM0(NL+1))*DT(I)
            T(I)=T(I)-SIGMA*G(I+MM0(NL+1))
230       CONTINUE
          ILINON=1
          ICURVE=0
          STEPLN(0)=0.0
        ENDIF
      ENDIF

      IF ((DELTA+LAMBDA*GVEC2).LE.0.0) THEN
C...Make Hessian positive definite:
        LAMBDA=2.*(LAMBDA-DELTA/(GVEC2+TINY))
      ENDIF
      DELTA=DELTA+LAMBDA*GVEC2

C...Update weights to calculate comparison parameter:
      ALPHA=-DERRLN/(DELTA+TINY)
      IF ((ABS(ALPHA).LE.ZEPS).OR.(NIT.GE.MSTJN(35)).OR.
     &(LAMBDA.GT.XLAMX)) THEN
C...Search is stuck! -> Restart.
        DO 280 I=1,MM0(NL+1)
          DW(I)=0.0
280     CONTINUE
        DO 290 I=1,MV0(NL+1)
          DT(I)=0.0
290     CONTINUE
        STEPLN(0)=0.0
        ISUCC=1
        IEVAL=0
        ILINON=0
        NSC=0
        MSTJN(38)=MSTJN(38)+1
        IF (MSTJN(38).GT.MSTJN(36)) CALL JNERR(21)
        MSTJN(37)=0
        RETURN
      ENDIF
      DO 300 I=1,MM0(NL+1)
        W(I)=W(I)+ALPHA*G(I)
300   CONTINUE
      DO 310 I=1,MV0(NL+1)
        T(I)=T(I)+ALPHA*G(I+MM0(NL+1))
310   CONTINUE
      STEPLN(0)=ALPHA
      IEVAL=1
      NIT=NIT+1
      MSTJN(37)=ABS(ILINON)
      RETURN

C...Come here if the comparison parameter is to be calculated:
10    CDELTA=2.*DELTA*(ERRLN(0)-PARJN(8))/(DERRLN**2+TINY)
      IEVAL=0

      IF (CDELTA.GE.0.0) THEN
C...Successful reduction in error.
        ISUCC=1
        DO 320 I=1,MM0(NL+1)
          DW(I)=0.0
320     CONTINUE
        DO 330 I=1,MV0(NL+1)
          DT(I)=0.0
330     CONTINUE
        STEPLN(0)=0.0
        ILINON=0
        IF (CDELTA.GE.0.75) LAMBDA=LAMBDA/4.0
      ELSE
C...Not a successful error reduction -> move back and make new attempt
        ISUCC=0
        DO 340 I=1,MM0(NL+1)
          W(I)=W(I)-ALPHA*G(I)
340     CONTINUE
        DO 350 I=1,MV0(NL+1)
          T(I)=T(I)-ALPHA*G(I+MM0(NL+1))
350     CONTINUE
        STEPLN(0)=0.0
      ENDIF
      IF (CDELTA.LT.0.25) LAMBDA=LAMBDA+DELTA*(1.-CDELTA)/GVEC2

      MSTJN(37)=ABS(ILINON)
      RETURN

C**** END OF JNSCGR ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNSEFI(ILA,I1,I2,J1,J2,NO)

C...JetNet subroutine SElect FIelds

C...Switches the updating of the weights between nodes I1 to I2 in layer 
C...ILA and nodes J1 to J2 in layer ILA-1 on or off according to NO. If 
C...NO<=0 updating is turned off else it is turned on. In addition if
C...NO=0 the weight is set to zero and if NO=1 the weight is 
C...reinitialized.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/


      IF(MSTJN(8).EQ.0) CALL JNERR(6)

      IF=MAX(I1,1)
      IL=MIN(I2,M(ILA))

      IF(WIDL(ILA).LE.0.0) THEN
        WIDTH=PARJN(4)
      ELSE
        WIDTH=WIDL(ILA)
      ENDIF

      IF(J1.NE.0.OR.J2.NE.0) THEN

        JF=MAX(J1,1)
        JL=MIN(J2,M(ILA-1))

        DO 100 II=IF,IL
          DO 110 JJ=JF,JL
            IF(NO.GT.0) THEN
              NSELF(JNINDX(ILA,II,JJ))=1
            ELSE
              NSELF(JNINDX(ILA,II,JJ))=0
            ENDIF
            IF(NO.EQ.1) THEN
              IDUM=JJ
              W(JNINDX(ILA,II,JJ))=(2.0*RJN(IDUM)-1.0)*WIDTH
            ELSEIF(NO.EQ.0) THEN
              W(JNINDX(ILA,II,JJ))=0.0
            ENDIF
110       CONTINUE
100     CONTINUE

      ELSE

        DO 200 II=IF,IL
          IF(NO.GT.0) THEN
            NTSELF(JNINDX(ILA,II,0))=1
          ELSE
            NTSELF(JNINDX(ILA,II,0))=0
          ENDIF
          IF(NO.EQ.1) THEN
            IDUM=II
            T(JNINDX(ILA,II,0))=(2.0*RJN(IDUM)-1.0)*WIDTH
          ELSEIF(NO.EQ.0) THEN
            T(JNINDX(ILA,II,0))=0.0
          ENDIF
200     CONTINUE

      ENDIF

      RETURN

C**** END OF JNSEFI ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNSEPA

C...JetNet subroutine SEt PArameters

C...Sets parameters in /JNINT2/

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)
      PARAMETER(MAXD2E=300)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT3/ NXIN,NYIN,NXRF,NYRF,NXHRF,NYHRF,NHRF,NRFW,NHPRF
      COMMON /JNINT4/ ILINON,NC,G2,NIT,ERRLN(0:3),DERRLN,STEPLN(0:3),
     &                STEPMN,ERRMN,IEVAL,ISUCC,ICURVE,NSC,GVEC2
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/,/JNINT3/,/JNINT4/


C...last layer:

      NL=MSTJN(1)-1
      IF(NL.GT.10.OR.NL.LT.1) CALL JNERR(1)

C...number of nodes in each layer

      IF(MSTJN(10).GT.MAXI) CALL JNERR(7)
      IF(MSTJN(10+NL).GT.MAXO) CALL JNERR(8)

      DO 400 IL=1,MSTJN(1)
        IF(MSTJN(9+IL).EQ.0) THEN
          MSTJN(7)=IL
          CALL JNERR(25)
        ENDIF
400   CONTINUE

      IF(MSTJN(23).NE.0) THEN

C...receptive fields will be used, check consistency and set indexes

        IF(MSTJN(24).EQ.0.OR.MSTJN(25).LE.0.OR.MSTJN(26).LE.0)
     &      CALL JNERR(10)

        NXIN=MSTJN(23)
        NYIN=MSTJN(24)
        NXRF=MSTJN(25)
        NYRF=MSTJN(26)
        NHPRF=ABS(MSTJN(27))
        IF(MSTJN(10).LT.ABS(NXIN*NYIN)) CALL JNERR(11)
        if(NXRF.GT.ABS(NXIN).OR.NYRF.GT.ABS(NYIN)) CALL JNERR(11)

        IF(NXIN.GT.0) THEN
          NXHRF=NXIN-NXRF+1
        ELSE
          NXHRF=-NXIN
        ENDIF

        IF(NYIN.GT.0) THEN
          NYHRF=NYIN-NYRF+1
        ELSE
          NYHRF=-NYIN
        ENDIF

        NHRF=NXHRF*NYHRF
        MSTJN(11)=MAX(MSTJN(11),NHRF*NHPRF)

        NRFW=NXRF*NYRF+MSTJN(10)-ABS(NXIN*NYIN)
        NRFLW=NRFW*NHPRF+(MSTJN(11)-NHRF*NHPRF)*MSTJN(10)

      ELSE

        NXIN=0
        NYIN=0
        NXRF=0
        NYRF=0
        NXHRF=0
        NYHRF=0
        NHRF=0
        NRFW=0
        NRFLW=0

      ENDIF

      DO 100 IL=0,NL
        M(IL)=MSTJN(10+IL)
100   CONTINUE

C...offset index in node vectors and weight vectors

      MV0(1)=0
      MM0(1)=0
      MV0(2)=M(1)
      MM0(2)=M(0)*M(1)
      IF(NXIN.NE.0) MM0(2)=NRFLW

      DO 110 IL=3,NL+1
        MV0(IL)=MV0(IL-1)+M(IL-1)
        MM0(IL)=MM0(IL-1)+M(IL-1)*M(IL-2)
110   CONTINUE

      IF(MV0(NL+1).GT.MAXV) CALL JNERR(2)
      IF(MM0(NL+1).GT.MAXM) CALL JNERR(3)

C...check Potts-nodes

      IPOTT=MSTJN(4)
      IF(IPOTT.GE.2) THEN
        IF(MOD(M(NL),IPOTT).NE.0) CALL JNERR(4)
        IGFN(NL)=3
      ENDIF
      IF(IPOTT.EQ.1) IGFN(NL)=5

C...set transfer functions to use

      DO 120 IL=1,NL
        IF(IGFN(IL).EQ.0) THEN
          NG(IL)=MSTJN(3)
        ELSE
          NG(IL)=IGFN(IL)
        ENDIF
120   CONTINUE

C...Check consistency between error measure and transfer function.
      IF(MSTJN(4).EQ.1) THEN
        IF((IGFN(NL).EQ.2).OR.(IGFN(NL).EQ.4)) CALL JNERR(31)
      ENDIF

C...Zero weight and threshold vectors
      DO 200 I=1,MV0(NL+1)
        DT(I)=0.0
        NTSELF(I)=1
200   CONTINUE

      DO 210 I=1,MM0(NL+1)
        DW(I)=0.0
        NSELF(I)=1
210   CONTINUE

C...set precision chopping

      IF((MSTJN(28).GT.0).OR.(MSTJN(29).GT.0).OR.
     &(MSTJN(30).GT.0)) ICPON=1

C...If updating is turned off, stop here.
      IF(MSTJN(5).EQ.9) CALL JNERR(32)

C...Initialize Quickprop, Rprop and Conjugate Gradient searches

      IF ((MSTJN(5).GE.3).AND.(MSTJN(5).LE.14)) THEN
        DO 300 I=1,MM0(NL+1)
          ODW(I)=0.0
          G(I)=0.0
300     CONTINUE
        DO 310 I=1,MV0(NL+1)
          ODT(I)=0.0
          G(MM0(NL+1)+I)=0.0
310     CONTINUE
      ENDIF
      IF (MSTJN(5).EQ.8) CALL JNERR(19)

      MSTJN(8)=1

C...Initialize Rprop learning rate:
      DO 500 IL=NL,1,-1
        IF (ETAL(IL).EQ.0.0) THEN
          ETA=PARJN(1)/FLOAT(MSTJN(2))
        ELSE
          ETA=ETAL(IL)/FLOAT(MSTJN(2))
        ENDIF
        DO 510 I=1,M(IL)
          IT=JNINDX(IL,I,0)
          ETAV(MM0(NL+1)+IT)=ETA
          DO 520 J=1,M(IL-1)
            IW=JNINDX(IL,I,J)
            ETAV(IW)=ETA
520       CONTINUE
510     CONTINUE
500   CONTINUE

C...Reset restart counter
      MSTJN(38)=0

      RETURN

C**** END OF JNSEPA ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNSTAT(IS)

C...JetNet subroutine output STATistics 

C...Statistics chosen by IS is written on the default file

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)
      PARAMETER(MAXD2E=300)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT3/ NXIN,NYIN,NXRF,NYRF,NXHRF,NYHRF,NHRF,NRFW,NHPRF
      COMMON /JNINT5/ D2E(MAXD2E,MAXD2E)
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/,/JNINT3/,/JNINT5/


      IF(IS.EQ.1) THEN
C...Write out number of layers, units and receptive field status
        WRITE(MSTJN(6),*)
        WRITE(MSTJN(6),600) NL+1
        WRITE(MSTJN(6),615) M(NL),NL
        DO 100 IL=NL-1,1,-1
          WRITE(MSTJN(6),620) M(IL),IL
100     CONTINUE
        WRITE(MSTJN(6),610) M(0)
        WRITE(MSTJN(6),*)
        IF(IPOTT.GT.1) WRITE(MSTJN(6),630)IPOTT
        IF(MSTJN(23).NE.0) THEN
          WRITE(MSTJN(6),631) ABS(MSTJN(23)*MSTJN(24)),ABS(MSTJN(23)),
     &    ABS(MSTJN(24)),MSTJN(25),MSTJN(26),ABS(MSTJN(27))
        ENDIF
        IF(MSTJN(23).LT.0) WRITE(MSTJN(6),632)
        IF(MSTJN(24).LT.0) WRITE(MSTJN(6),633)
        IF(MSTJN(27).LT.0) WRITE(MSTJN(6),634) 
        IF(IPOTT.EQ.1) WRITE(MSTJN(6),635)
        IF(MSTJN(5).EQ.0) THEN
          WRITE(MSTJN(6),639)
        ELSEIF(MSTJN(5).EQ.1) THEN
          WRITE(MSTJN(6),640)
        ELSEIF(MSTJN(5).EQ.2) THEN
          WRITE(MSTJN(6),645)
        ELSEIF(MSTJN(5).EQ.3) THEN
          WRITE(MSTJN(6),646)
        ELSEIF(MSTJN(5).EQ.4) THEN
          WRITE(MSTJN(6),647)
        ELSEIF(MSTJN(5).EQ.5) THEN
          WRITE(MSTJN(6),648)
        ELSEIF(MSTJN(5).EQ.6) THEN
          WRITE(MSTJN(6),649)
        ELSEIF(MSTJN(5).EQ.7) THEN
          WRITE(MSTJN(6),651)
        ELSEIF(MSTJN(5).EQ.10) THEN
          WRITE(MSTJN(6),652)
        ELSEIF(MSTJN(5).EQ.11) THEN
          WRITE(MSTJN(6),653)
        ELSEIF(MSTJN(5).EQ.12) THEN
          WRITE(MSTJN(6),654)
        ELSEIF(MSTJN(5).EQ.13) THEN
          WRITE(MSTJN(6),655)
        ELSEIF(MSTJN(5).EQ.15) THEN
          WRITE(MSTJN(6),656)
        ENDIF
        WRITE(MSTJN(6),*)

      ELSEIF(IS.EQ.2) THEN
C...Write out values of parameters and switches
        PAR22=PARJN(22)
        PARJN(22)=LOG10(PAR22)
        WRITE(MSTJN(6),*)
        WRITE(MSTJN(6),650)
        WRITE(MSTJN(6),*)
        WRITE(MSTJN(6),660)'I ',(I,I=1,10)
        WRITE(MSTJN(6),661)'MSTJN (I)',(MSTJN(I),I=1,6),
     &   LOG10(MAX(FLOAT(MSTJN(7)),1.)),(MSTJN(I),I=8,10)
        WRITE(MSTJN(6),660)'(10+I)',(MSTJN(10+I),I=1,10)
        WRITE(MSTJN(6),660)'(20+I)',(MSTJN(20+I),I=1,10)
        WRITE(MSTJN(6),660)'(30+I)',(MSTJN(30+I),I=1,10)
        WRITE(MSTJN(6),670)'PARJN (I)',(PARJN(I),I=1,10)
        WRITE(MSTJN(6),670)'(10+I)',(PARJN(10+I),I=1,10)
        WRITE(MSTJN(6),670)'(20+I)',(PARJN(20+I),I=1,10)
        WRITE(MSTJN(6),670)'(30+I)',(PARJN(30+I),I=1,10)
        WRITE(MSTJN(6),660)'IGFN (I)',(IGFN(I),I=1,10)
        WRITE(MSTJN(6),670)'TINV (I)',(TINV(I),I=1,10)
        WRITE(MSTJN(6),670)'ETAL (I)',(ETAL(I),I=1,10)
        WRITE(MSTJN(6),670)'WIDL (I)',(WIDL(I),I=1,10)
        WRITE(MSTJN(6),670)'SATM (I)',(SATM(I),I=1,10)
        WRITE(MSTJN(6),*)
        PARJN(22)=PAR22

      ELSEIF(IS.EQ.3) THEN
C...Write out time factor for net
        NWFAC=0
        IF(NXIN.EQ.0) THEN
          NWFAC=MM0(NL+1)+MV0(NL+1)
          NWSUM=NWFAC
        ELSE
          NWFAC=MM0(NL+1)+MV0(NL+1)-MM0(2)+NHRF*NRFW*NHPRF
     &          +(MSTJN(11)-NHRF*NHPRF)*MSTJN(10)
          NWSUM=MM0(NL+1)+MV0(NL+1)-MM0(2)+NRFW*NHPRF-(NHRF-1)*NHPRF
     &          +(MSTJN(11)-NHRF*NHPRF)*MSTJN(10)
          IF(MSTJN(27).LT.0) NWSUM=NWSUM-(NHRF-1)*NHPRF*MSTJN(12)
        ENDIF
        WRITE(MSTJN(6),680) NWFAC
        WRITE(MSTJN(6),690) NWSUM

      ELSEIF(IS.EQ.4) THEN
C...Write out Hessian Matrix
        NWGTS=MM0(NL+1)+MV0(NL+1)
        WRITE(MSTJN(6),700)NWGTS,NWGTS
        WRITE(MSTJN(6),*)
        NHOP=NWGTS/7
        NEXTRA=NWGTS-NHOP*7
        DO 200 I=1,NWGTS
          WRITE(MSTJN(6),720)I
          DO 210 JHOP=1,NHOP
            WRITE(MSTJN(6),710)(D2E((JHOP-1)*7+J,I),J=1,7)
210       CONTINUE
          IF (NEXTRA.GT.0) THEN
            WRITE(MSTJN(6),710)(D2E(NHOP*7+J,I),J=1,NEXTRA)
          ENDIF
          WRITE(MSTJN(6),*)
200     CONTINUE

      ELSEIF(IS.EQ.5) THEN
C...Write out the diagonal elements of the Hessian Matrix and its Trace
        TRACE=0.0
        NWGTS=MM0(NL+1)+MV0(NL+1)
        DO 220 IW=1,NWGTS
          TRACE=TRACE+D2E(IW,IW)
220     CONTINUE
        WRITE(MSTJN(6),700)NWGTS,NWGTS
        WRITE(MSTJN(6),730)
        WRITE(MSTJN(6),*)
        NHOP=NWGTS/7
        NEXTRA=NWGTS-NHOP*7
        DO 230 JHOP=1,NHOP
          WRITE(MSTJN(6),710)(D2E((JHOP-1)*7+J,(JHOP-1)*7+J),J=1,7)
230     CONTINUE
        IF (NEXTRA.GT.0) THEN
          WRITE(MSTJN(6),710)(D2E(NHOP*7+J,NHOP*7+J),J=1,NEXTRA)
        ENDIF
        WRITE(MSTJN(6),*)
        WRITE(MSTJN(6),740)TRACE
        WRITE(MSTJN(6),*)

      ENDIF

600   FORMAT(22X,'Initialized for a',I2,' layered net with')
610   FORMAT(22X,I3,' nodes in layer number 0 (input layer)')
615   FORMAT(22X,I3,' nodes in layer number',I2,' (output layer)')
620   FORMAT(22X,I3,' nodes in layer number',I2)
630   FORMAT(5X,'with',I3,'-dimensional Potts nodes in output layer.')
631   FORMAT(5X,'receptive fields in first layer assuming the ',I4,
     &   ' first nodes in the',/,
     &   5X,'input layer are organised in a plane of ',
     &   I3,'*',I3,' nodes, where the',/,
     &   5X,'receptive field nodes scan ',
     &   I3,'*',I3,' input nodes each with ',I3,' hidden'/,
     &   5X,'nodes per field.')
632   FORMAT(5X,'The input layer is assumed to be cyclic ',
     &       'in the x-direction.')
633   FORMAT(5X,'The input layer is assumed to be cyclic ',
     &       'in the y-direction.')
634   FORMAT(5X,'The weights from equivalent nodes with receptive ',
     &           'fields are clamped.')
635   FORMAT(22X,'Using Cross-Entropy error.')
639   FORMAT(22X,'Standard Back-Propagation updating.')
640   FORMAT(22X,'Manhattan updating.')
645   FORMAT(22X,'Langevin updating.')
646   FORMAT(22X,'Quickprop updating.')
647   FORMAT(22X,'Conjugate Gradient updating (Polak-Ribiere).')
648   FORMAT(22X,'Conjugate Gradient updating (Hestenes-Stiefel).')
649   FORMAT(22X,'Conjugate Gradient updating (Fletcher-Reeves).')
650   FORMAT(18X,'Values of parameters and switches in JETNET')
651   FORMAT(22X,'Conjugate Gradient updating (Shanno).')
652   FORMAT(22X,'Scaled Conj. Grad. updating (Polak-Ribiere).')
653   FORMAT(22X,'Scaled Conj. Grad. updating (Hestenes-Stiefel).')
654   FORMAT(22X,'Scaled Conj. Grad. updating (Fletcher-Reeves).')
655   FORMAT(22X,'Scaled Conj. Grad. updating (Shanno).')
656   FORMAT(22X,'Rprop updating.')
660   FORMAT(A10,10I7)
661   FORMAT(A10,6I7,F7.3,3I7)
670   FORMAT(A10,10F7.4)
680   FORMAT(5X,'Time factor for this net:',I10)
690   FORMAT(5X,'Effective number of weights:',I7)
700   FORMAT(5X,'The Hessian Matrix: (',I3,' x ',I3,')')
710   FORMAT(5X,7(E9.2,1X))
720   FORMAT(5X,'Column: ',I3)
730   FORMAT(5X,'Diagonal elements only')
740   FORMAT(5X,'Trace(H) = ',F10.5)
      RETURN

C**** END OF JNSTAT ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNTEST

C...JetNet subroutine TEST 

C...Sets the values of OUT according to given pattern in OIN and
C...current weights

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/


      IF (MSTJN(8).EQ.0) CALL JNERR(23)

      CALL JNFEED

      DO 100 I=1,M(NL)
        OUT(I)=O(JNINDX(NL,I,0))
100   CONTINUE

      RETURN

C**** END OF JNTEST ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNTRAL

C...JetNet subroutine TRaining ALgorithm

C...Trains the net.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,TINY=1.E-20)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT4/ ILINON,NC,G2,NIT,ERRLN(0:3),DERRLN,STEPLN(0:3),
     &                STEPMN,ERRMN,IEVAL,ISUCC,ICURVE,NSC,GVEC2
      SAVE /JNDAT1/,/JNDAT2/,/JNINT1/,/JNINT2/,/JNINT4/


      IF (MSTJN(8).EQ.0) CALL JNERR(22)
      IF (MSTJN(9).LE.0) CALL JNERR(24)

      MSTJN(7)=MSTJN(7)+1

      CALL JNFEED
      IF (ILINON.EQ.0) CALL JNDELT

      ERR=ERRJN(0)

      ERR=ERR/FLOAT(M(NL))
      PARJN(7)=ERR
      ER1=ER1+ERR
      ER2=ER2+ERR

      IF (MSTJN(22).NE.0) CALL JNSATM

      IF(MOD(MSTJN(7),MSTJN(2)).NE.0) RETURN

C...update only every MSTJN(2) calls

      PARJN(8)=ER1/FLOAT(MSTJN(2))
      ER1=0.0


      IF(MSTJN(21).GT.0) THEN

C...Include pruning factors

        BETA=1.0
        DO 110 IL=NL,1,-1

C...set beta in layer IL:
          IF(TINV(IL).EQ.0.0) THEN
            BETA=BETA*PARJN(3)
          ELSE
            BETA=BETA*ABS(TINV(IL))
          ENDIF

          FACTOR=2.0*FLOAT(MSTJN(2))*PARJN(14)*PARJN(18)**2/BETA

          DO 120 I=MM0(IL)+1,MM0(IL+1)
            DW(I)=DW(I)-FACTOR*W(I)/
     &            (PARJN(18)**2+W(I)**2)**2
120       CONTINUE

          DO 130 I=MV0(IL)+1,MV0(IL+1)
            DT(I)=DT(I)-FACTOR*T(I)/
     &            (PARJN(18)**2+T(I)**2)**2
130       CONTINUE

110     CONTINUE

      ENDIF

      IF(MSTJN(5).EQ.0) THEN

C...Normal updating:

        BETA=1.0
        DO 200 IL=NL,1,-1

C...set beta in layer IL:
          IF(TINV(IL).EQ.0.0) THEN
            BETA=BETA*PARJN(3)
          ELSE
            BETA=BETA*ABS(TINV(IL))
          ENDIF

C...set eta in layer IL:

          IF(ETAL(IL).EQ.0.0) THEN
            ETA=PARJN(1)/FLOAT(MSTJN(2))*BETA
          ELSE
            ETA=ETAL(IL)/FLOAT(MSTJN(2))*BETA
          ENDIF

          DO 210 I=MM0(IL)+1,MM0(IL+1)
            W(I)=(1.0-PARJN(5)*FLOAT(NSELF(I)))*W(I)+
     &           DW(I)*FLOAT(NSELF(I))*ETA
            DW(I)=DW(I)*PARJN(2)
210       CONTINUE

          DO 220 I=MV0(IL)+1,MV0(IL+1)
            T(I)=(1.0-PARJN(5)*FLOAT(NTSELF(I)))*T(I)+
     &           DT(I)*FLOAT(NTSELF(I))*ETA
            DT(I)=DT(I)*PARJN(2)
220       CONTINUE

200     CONTINUE

        ILINON=0
        NC=0
        NSC=0

      ELSEIF(MSTJN(5).EQ.1) THEN

C...Manhattan updating:

C...set eta in layer IL:

        DO 300 IL=1,NL
          IF(ETAL(IL).EQ.0.0) THEN
            ETA=PARJN(1)/FLOAT(MSTJN(2))
          ELSE
            ETA=ETAL(IL)/FLOAT(MSTJN(2))
          ENDIF

          DO 310 I=MM0(IL)+1,MM0(IL+1)
            W(I)=(1.0-PARJN(5)*FLOAT(NSELF(I)))*W(I)+
     &             SIGN(ETA,DW(I))*FLOAT(NSELF(I))
            DW(I)=DW(I)*PARJN(2)
310       CONTINUE

          DO 320 I=MV0(IL)+1,MV0(IL+1)
            T(I)=(1.0-PARJN(5)*FLOAT(NTSELF(I)))*T(I)+
     &            SIGN(ETA,DT(I))*FLOAT(NTSELF(I))
            DT(I)=DT(I)*PARJN(2)
320       CONTINUE

300     CONTINUE

        ILINON=0
        NC=0
        NSC=0

      ELSEIF(MSTJN(5).EQ.2) THEN

C...Langevin updating:

        BETA=1.0
        DO 400 IL=NL,1,-1

C...set effective beta in layer IL:
          IF(TINV(IL).EQ.0.0) THEN
            BETA=BETA*PARJN(3)
          ELSE
            BETA=BETA*ABS(TINV(IL))
          ENDIF

C...set eta in layer IL:

          IF(ETAL(IL).EQ.0.0) THEN
            ETA=PARJN(1)/FLOAT(MSTJN(2))*BETA
          ELSE
            ETA=ETAL(IL)/FLOAT(MSTJN(2))*BETA
          ENDIF

          DO 410 I=MM0(IL)+1,MM0(IL+1)
            IDUM=I
            W(I)=(1.0-PARJN(5)*FLOAT(NSELF(I)))*W(I)+
     &           DW(I)*FLOAT(NSELF(I))*ETA+
     &           GAUSJN(IDUM)*PARJN(6)
            DW(I)=DW(I)*PARJN(2)
410       CONTINUE

          DO 420 I=MV0(IL)+1,MV0(IL+1)
            IDUM=I
            T(I)=(1.0-PARJN(5)*FLOAT(NTSELF(I)))*T(I)+
     &           DT(I)*FLOAT(NTSELF(I))*ETA+
     &           GAUSJN(IDUM)*PARJN(6)
            DT(I)=DT(I)*PARJN(2)
420       CONTINUE

400     CONTINUE

        ILINON=0
        NC=0
        NSC=0

      ELSEIF(MSTJN(5).EQ.3) THEN

C...Fahlman's Quickprop:

        WMAX=0.0
        BETA=1.0
        DO 500 IL=NL,1,-1

C...set beta in layer IL:
          IF(TINV(IL).EQ.0.0) THEN
            BETA=BETA*PARJN(3)
          ELSE
            BETA=BETA*ABS(TINV(IL))
          ENDIF

C...set eta in layer IL:

          IF(ETAL(IL).EQ.0.0) THEN
            ETA=PARJN(1)/FLOAT(MSTJN(2))*BETA
          ELSE
            ETA=ETAL(IL)/FLOAT(MSTJN(2))*BETA
          ENDIF

          DO 510 I=MM0(IL)+1,MM0(IL+1)
            SCALE=MAX(-PARJN(21),
     &            MIN(PARJN(21),DW(I)/(ODW(I)-DW(I)+TINY)))
            SWITCH=FLOAT(NSELF(I))*(SIGN(0.5,ODW(I)*DW(I))+0.5)
            G(I)=DW(I)*SWITCH*ETA+SCALE*G(I)
            W(I)=(1.0-PARJN(5))*W(I)+G(I)
            ODW(I)=DW(I)
            DW(I)=0.0
            IF (ABS(W(I)).GT.WMAX) WMAX=ABS(W(I))
510       CONTINUE

          DO 520 I=MV0(IL)+1,MV0(IL+1)
            SCALE=MAX(-PARJN(21),
     &            MIN(PARJN(21),DT(I)/(ODT(I)-DT(I)+TINY)))
            SWITCH=FLOAT(NTSELF(I))*(SIGN(0.5,ODT(I)*DT(I))+0.5)
            G(MM0(NL+1)+I)=DT(I)*SWITCH*ETA+SCALE*G(MM0(NL+1)+I)
            T(I)=(1.0-PARJN(5))*T(I)+G(MM0(NL+1)+I)
            ODT(I)=DT(I)
            DT(I)=0.0
            IF (ABS(T(I)).GT.WMAX) WMAX=ABS(T(I))
520       CONTINUE

500     CONTINUE

        IF (WMAX.GT.PARJN(22)) THEN
C...Quickprop is stuck -> reset weights and restart
          DO 530 IL=1,NL
            IF(WIDL(IL).LE.0) THEN
              WIDTH=PARJN(4)
            ELSE
              WIDTH=WIDL(IL)
            ENDIF
            DO 540 I=MM0(IL)+1,MM0(IL+1)
              IDUM=I
              IF (WIDTH.GE.0.) THEN
                W(I)=(2.0*RJN(IDUM)-1.0)*WIDTH
              ELSE
                W(I)=-RJN(IDUM)*WIDTH
              ENDIF
540         CONTINUE
            DO 550 I=MV0(IL)+1,MV0(IL+1)
              IDUM=I
              IF (WIDTH.GE.0.) THEN
                T(I)=(2.0*RJN(IDUM)-1.0)*WIDTH
              ELSE
                T(I)=-RJN(IDUM)*WIDTH
              ENDIF
550         CONTINUE
530       CONTINUE
          MSTJN(38)=MSTJN(38)+1
          IF (MSTJN(38).GT.MSTJN(36)) CALL JNERR(21)
        ENDIF

        ILINON=0
        NC=0
        NSC=0

      ELSEIF((MSTJN(5).GE.4).AND.(MSTJN(5).LE.8)) THEN

C...Conjugate Gradient updating:

        CALL JNCOGR

      ELSEIF(MSTJN(5).EQ.9) THEN

C...Minimization terminated - don't update:

        RETURN

      ELSEIF((MSTJN(5).GE.10).AND.(MSTJN(5).LE.14)) THEN

C...Scaled Conjugate Gradient:

        CALL JNSCGR

      ELSEIF(MSTJN(5).EQ.15) THEN

C...Riedmiller's & Braun's Rprop:

        DO 700 IW=1,MM0(NL+1)
          IF (DW(IW)*ODW(IW).GT.0.) THEN
            ETAV(IW)=MIN(PARJN(32),MAX(PARJN(33),ETAV(IW)*PARJN(30)))
          ELSEIF (DW(IW)*ODW(IW).LT.0.) THEN
            ETAV(IW)=MIN(PARJN(32),MAX(PARJN(33),ETAV(IW)*PARJN(31)))
          ENDIF
          W(IW)=W(IW)+SIGN(ETAV(IW),DW(IW)*FLOAT(NSELF(IW)))
          ODW(IW)=DW(IW)
          DW(IW)=0.0
700     CONTINUE
        DO 710 IT=1,MV0(NL+1)
          IF (DT(IT)*ODT(IT).GT.0.) THEN
            ETAV(MM0(NL+1)+IT)=MIN(PARJN(32),
     &           MAX(PARJN(33),ETAV(MM0(NL+1)+IT)*PARJN(30)))
          ELSEIF (DT(IT)*ODT(IT).LT.0.) THEN
            ETAV(MM0(NL+1)+IT)=MIN(PARJN(32),MAX(PARJN(33),
     &           ETAV(MM0(NL+1)+IT)*PARJN(31)))
          ENDIF
          T(IT)=T(IT)+SIGN(ETAV(MM0(NL+1)+IT),
     &                DT(IT)*FLOAT(NTSELF(IT)))
          ODT(IT)=DT(IT)
          DT(IT)=0.0
710     CONTINUE

      ELSE

        CALL JNERR(9)

      ENDIF

C...do fixed precision weights

      IF(ICPON.EQ.1) CALL JNCHOP(0)

C...Scale temperature

      IF(MSTJN(22).GE.0) THEN

        SCALE=PARJN(13)**(1.0/FLOAT(MSTJN(9)))
        PARJN(3)=PARJN(3)/SCALE
        DO 600 I=1,NL
          TINV(I)=TINV(I)/SCALE
600     CONTINUE

      ENDIF

      IF(MOD(MSTJN(7),MSTJN(2)*MSTJN(9)).NE.0) RETURN

C...Update some parameters every epoch

      OLDE=PARJN(9)
      PARJN(9)=ER2/FLOAT(MSTJN(2)*MSTJN(9))
      ER2=0.0

      IF (MSTJN(21).GT.0) THEN

C...Update pruning parameters

        PARJN(10)=PARJN(16)*PARJN(10)+(1.-PARJN(16))*PARJN(9)
        IF((PARJN(9).LT.OLDE).OR.(PARJN(9).LT.PARJN(19))) THEN
          PARJN(14)=PARJN(14)+PARJN(15)
        ELSEIF(PARJN(9).LT.PARJN(10)) THEN
          PARJN(14)=PARJN(14)-PARJN(15)
        ELSE
          PARJN(14)=PARJN(17)*PARJN(14)
        ENDIF
      ENDIF

      IF (MSTJN(22).NE.0) THEN

C...Calculate saturation measures

        DO 610 IL=1,NL
          SATM(IL)=SM(IL)/FLOAT(MSTJN(2)*MSTJN(9))
          SM(IL)=0.0
610     CONTINUE
      ENDIF

      IF(MSTJN(22).LT.0) THEN

        DO 620 I=1,NL

          IF(TINV(I).GE.0.0) GOTO 620

          IF(SATM(I).GT.0.75) THEN
            TINV(I)=TINV(I)/(1.0+16.0*(PARJN(13)-1.0)*(SATM(I)-0.5)**2)
            GOTO 630
          ELSEIF(SATM(I).LT.0.25) THEN
            TINV(I)=TINV(I)*(1.0+16.0*(PARJN(13)-1.0)*(0.5-SATM(I))**2)
            GOTO 630
          ENDIF
620     CONTINUE

630   ENDIF


C...Scale parameters:

      IF (MSTJN(5).LE.2) THEN
        IF (PARJN(11).GT.0.) THEN
C...Change eta using 'bold driver':
          IF (PARJN(9).GE.OLDE) THEN
            PARJN(1)=PARJN(1)*PARJN(11)
            DO 640 I=1,10
              ETAL(I)=ETAL(I)*PARJN(11)
640         CONTINUE
          ELSE
            PARJN(1)=PARJN(1)*(1.0+0.1*(1.0-PARJN(11)))
            DO 650 I=1,10
              ETAL(I)=ETAL(I)*(1.0+0.1*(1.0-PARJN(11)))
650         CONTINUE
          ENDIF
        ELSEIF (PARJN(11).LT.0.) THEN
C...Decrease eta geometrically:
          PARJN(1)=PARJN(1)*ABS(PARJN(11))
          DO 660 I=1,10
            ETAL(I)=ETAL(I)*ABS(PARJN(11))
660       CONTINUE
        ENDIF
      ENDIF
C...Scale alpha:
      PARJN(2)=PARJN(2)*PARJN(12)
C...Scale Langevin noise:
      PARJN(6)=PARJN(6)*PARJN(20)

      RETURN

C**** END OF JNTRAL ****************************************************
      END
C***********************************************************************


      SUBROUTINE JNTRTQ(N,IGRAD)

C...JetNet subroutine TR and TQ.
C...This routine changed ( = error correction) from version 3.4 to 3.5.

C...Computes the eigenvalues for the Hessian Matrix.
C...If IGRAD does not equal 0, then the eigenvectors are returned
C...as column vectors in D2E

C...First:
C...Householder reduction of the NxN Hessian stored in D2E. 
C...This routine is taken from "Numerical Recipes" by W.H.Press 
C...et. al., where it is called TRED2. It has been slightly changed to
C...fit into JETNET.

      PARAMETER(MAXI=1000,MAXO=1000)
      PARAMETER(MAXD2E=300,MAXIT=100)
      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT5/ D2E(MAXD2E,MAXD2E)
      SAVE /JNDAT1/,/JNINT5/
      DIMENSION A(MAXD2E,MAXD2E),D(MAXD2E),E(MAXD2E)

C...  First, commute matrix so that the smallest values are in the
C...  top left corner.
      DO 101 IW=1,N
        DO 102 JW=1,N
          A(N-IW+1,N-JW+1)=D2E(IW,JW)
102     CONTINUE
101   CONTINUE

      IF(N.GT.1)THEN
        DO 18 I=N,2,-1  
          L=I-1
          H=0.
          SCALE=0.
          IF(L.GT.1)THEN
            DO 11 K=1,L
              SCALE=SCALE+ABS(A(I,K))
11          CONTINUE
            IF(SCALE.EQ.0.)THEN
              E(I)=A(I,L)
            ELSE
              DO 12 K=1,L
                A(I,K)=A(I,K)/SCALE
                H=H+A(I,K)**2
12            CONTINUE
              F=A(I,L)
              G=-SIGN(SQRT(H),F)
              E(I)=SCALE*G
              H=H-F*G
              A(I,L)=F-G
              F=0.
              DO 15 J=1,L
C...If eigenvectors are needed
                IF (IGRAD.NE.0) A(J,I)=A(I,J)/H
                G=0.
                DO 13 K=1,J
                  G=G+A(J,K)*A(I,K)
13              CONTINUE
                IF(L.GT.J)THEN
                  DO 14 K=J+1,L
                    G=G+A(K,J)*A(I,K)
14                CONTINUE
                ENDIF
                E(J)=G/H
                F=F+E(J)*A(I,J)
15            CONTINUE
              HH=F/(H+H)
              DO 17 J=1,L
                F=A(I,J)
                G=E(J)-HH*F
                E(J)=G
                DO 16 K=1,J
                  A(J,K)=A(J,K)-F*E(K)-G*A(I,K)
16              CONTINUE
17            CONTINUE
            ENDIF
          ELSE
            E(I)=A(I,L)
          ENDIF
          D(I)=H
18      CONTINUE
      ENDIF
C...If eigenvectors are needed:
      IF (IGRAD.NE.0) D(1)=0.
      E(1)=0.
      DO 23 I=1,N
C...If eigenvectors are needed:
        IF (IGRAD.NE.0) THEN
          L=I-1
          IF(D(I).NE.0.)THEN
            DO 21 J=1,L
              G=0.
              DO 19 K=1,L
                G=G+A(I,K)*A(K,J)
19            CONTINUE
              DO 20 K=1,L
                A(K,J)=A(K,J)-G*A(K,I)
20            CONTINUE
21          CONTINUE
          ENDIF
        ENDIF
C...
        D(I)=A(I,I)
C...If eigenvectors are needed:
        IF (IGRAD.NE.0) THEN
          A(I,I)=1.
          IF(L.GE.1)THEN
            DO 22 J=1,L
              A(I,J)=0.
              A(J,I)=0.
22          CONTINUE
          ENDIF
        ENDIF
C...
23    CONTINUE

C...Second:
C...QL algorithm with implicit shifts to determine the eigenvalues and
C...eigenvectors of the NxN Hessian. 
C...Eigenvalues and eigenvectors are computed if IGRAD is non-zero.
C...At return the eigenvectors are stored as columns in D2E and the
C...eigenvalues are placed in the vector OUT.
C...This routine is taken from "Numerical Recipes" by W.H.Press 
C...et. al., where it is called TQLI. It has been slightly modified 
C...to fit into JETNET.

      IF (N.GT.1) THEN
        DO 51 I=2,N
          E(I-1)=E(I)
51      CONTINUE
        E(N)=0.
        DO 55 L=1,N
          ITER=0
41        DO 52 M=L,N-1
            DD=ABS(D(M))+ABS(D(M+1))
            IF (ABS(E(M))+DD.EQ.DD) GO TO 42
52        CONTINUE
          M=N
42        IF(M.NE.L)THEN
            IF(ITER.EQ.MAXIT) CALL JNERR(30)
            ITER=ITER+1
            G=(D(L+1)-D(L))/(2.*E(L))
C...        Pythagorean without under/overflow
            ABSG=ABS(G)
            IF(ABSG.GT.1.0) THEN
              R=ABSG*SQRT(1.+(1./ABSG)**2)
            ELSE
              R=SQRT(1.+ABSG**2)
            ENDIF
C...
            G=D(M)-D(L)+E(L)/(G+SIGN(R,G))
            SS=1.
            C=1.
            P=0.
            DO 54 I=M-1,L,-1
              F=SS*E(I)
              B=C*E(I)
C...          Pythagorean without over/underflow
              ABSF=ABS(F)
              ABSG=ABS(G)
              IF(ABSF.GT.ABSG) THEN
                R=ABSF*SQRT(1.+(ABSG/ABSF)**2)
              ELSE
                IF(ABSG.EQ.0.) THEN
                  R=0.
                ELSE
                  R=ABSG*SQRT(1.+(ABSF/ABSG)**2)
                ENDIF
              ENDIF
C...
              E(I+1)=R
C...Recover from underflow
              IF(R.EQ.0.) THEN
                D(I+1)=D(I+1)-P
                E(M)=0.
                GOTO 41
              ENDIF
              SS=F/R
              C=G/R
              G=D(I+1)-P
              R=(D(I)-G)*SS+2.*C*B
              P=SS*R
              D(I+1)=G+P
              G=C*R-B
C...If eigenvectors are needed:
              IF (IGRAD.NE.0) THEN
                DO 53 K=1,N
                  F=A(K,I+1)
                  A(K,I+1)=SS*A(K,I)+C*F
                  A(K,I)=C*A(K,I)-SS*F
                  D2E(N-K+1,N-I+1)=A(K,I)
53              CONTINUE
              ENDIF
C...
54          CONTINUE
            D(L)=D(L)-P
            E(L)=G
            E(M)=0.
            GO TO 41
          ENDIF
55      CONTINUE
      ENDIF

C...Put eigenvalues in OUT:
      DO 100 I=1,N
        OUT(I)=D(N-I+1)
100   CONTINUE

      RETURN

C**** END OF JNTRTQ ****************************************************
      END
C***********************************************************************

C**********************************************************************C
C PART TWO: SELF-ORGANIZING MAP NETWORK                                C
C**********************************************************************C


      REAL FUNCTION GJM(X,N)
C...JetMap function G.

C...Gives response function N with argument X.

      PARAMETER(MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      SAVE /JNDAT1/


      IF(N.EQ.1) THEN
        GJM=0.5*(1.0+TANH(X))
      ELSEIF(N.EQ.2) THEN
        GJM=EXP(MAX(-50.0,MIN(-X,50.0)))
      ELSE
        MSTJM(3)=N
        CALL JMERR(11)
      ENDIF

      RETURN

C**** END OF GJM *******************************************************
      END 
C***********************************************************************


      SUBROUTINE JMDUMP(NF)

C...JetMap subroutine DUMP weights

C...Dumps weights and other characteristics of the
C...net to file NF for use in other programs.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,MAXD=2)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JMINT1/ NDIM,ISW(10),NODES(0:MAXD+1),NBO
      SAVE /JNDAT1/,/JNINT1/,/JMINT1/

      DIMENSION INDW(MAXV)
      EQUIVALENCE (NTSELF(1),INDW(1))


      IF (MSTJM(8).NE.1) CALL JMERR(8)

      IF(NF.LT.0) THEN

C...Unformatted dump

        JF=-NF
        WRITE(JF) -30
        WRITE(JF) MSTJM,PARJM

        DO 100 IW=1,INDW(NODES(MAXD+1)+1)-1
          WRITE(JF) W(IW)
100     CONTINUE

      ELSE

C...Formatted dump

        NFSAVE=MSTJM(6)
        MSTJM(6)=NF

        WRITE(NF,600)
        CALL JNHEAD
        CALL JMSTAT(1)
        CALL JMSTAT(2)

        MSTJM(6)=NFSAVE

        DO 200 INOD=1,NODES(MAXD+1)
          WRITE(NF,*)
          CALL JMINDX(INOD,I,J)
          IF (NDIM.EQ.1) THEN
            WRITE(NF,610)I
          ELSE
            WRITE(NF,620)I,J
          ENDIF
         
          IW=INDW(INOD)-1
          WRITE(NF,630)(W(IW+K),K=1,NODES(0))

200    CONTINUE

      ENDIF

600   FORMAT(26X,' Dump of weights generated by')
610   FORMAT('Unit ',I2)
620   FORMAT('Unit (',I2,',',I2,')')
630   FORMAT(10(F8.4,1X))

      RETURN

C**** END OF JMDUMP ****************************************************
      END
C***********************************************************************


      SUBROUTINE JMERR(IERR)
C...JetMap subroutine ERRor.

C...Writes out an error message and stops the execution.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,MAXD=2)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JMINT1/ NDIM,ISW(10),NODES(0:MAXD+1),NBO
      SAVE /JNDAT1/,/JNINT1/,/JMINT1/

      DIMENSION INDW(MAXV)
      EQUIVALENCE (NTSELF(1),INDW(1))


      IF (MSTJN(8).EQ.1) MSTJM(6)=MSTJN(6)
      WRITE(MSTJM(6),600)IERR

      IF (IERR.EQ.1) THEN
        WRITE(MSTJM(6),610)MSTJM(1)
      ELSEIF (IERR.EQ.2) THEN
        WRITE(MSTJM(6),620)MSTJM(10),MAXI
      ELSEIF (IERR.EQ.3) THEN
        WRITE(MSTJM(6),630)ABS(MSTJM(11)*MSTJM(12)),MAXO
      ELSEIF (IERR.EQ.4) THEN
        WRITE(MSTJM(6),640)NODES(MAXD+1)*(NODES(0)+1),MAXM
      ELSEIF (IERR.EQ.5) THEN
        WRITE(MSTJM(6),650)'JMTEST'
      ELSEIF (IERR.EQ.6) THEN
        WRITE(MSTJM(6),650)'JMTRAL'
      ELSEIF (IERR.EQ.7) THEN
        WRITE(MSTJM(6),650)'JMINDX'
      ELSEIF (IERR.EQ.8) THEN
        WRITE(MSTJM(6),650)'JMDUMP'
      ELSEIF (IERR.EQ.9) THEN
        WRITE(MSTJM(6),650)'JMINWE'
      ELSEIF (IERR.EQ.10) THEN
        WRITE(MSTJM(6),660)
      ELSEIF (IERR.EQ.11) THEN
        WRITE(MSTJM(6),670)MSTJM(3)
      ELSEIF (IERR.EQ.12) THEN
        WRITE(MSTJM(6),680)
      ELSEIF (IERR.EQ.13) THEN
        WRITE(MSTJM(6),690)
      ELSEIF (IERR.EQ.14) THEN
        WRITE(MSTJM(6),650)'JMNBHD'
      ENDIF

      IF (IERR.GT.0) STOP 0

600   FORMAT(' *** JETMAP ERROR:',I2,' ***')
610   FORMAT(' Illegal number of dimensions (',I2,')')
620   FORMAT(' Total number of input nodes (',I6,') exceeds limit (',
     &I6,')')
630   FORMAT(' Total number of network nodes (',I6,
     &') exceeds limit (',I6,')')
640   FORMAT(' The number of weights (',I6,') exceeds limit (',I5,')')
650   FORMAT(' Network must be initialized (with JMINIT or JMREAD) ',
     &'before ',A6,' can be called')
660   FORMAT(' Call to JMINIT after calling JNINIT')
670   FORMAT(' Undefined response function (',I2,') in GJM.')
680   FORMAT(' JMREAD cannot read data-file produced by JNDUMP')
690   FORMAT(' Too many warnings issued by JETMAP')

      RETURN

C**** END OF JMERR *****************************************************
      END
C***********************************************************************


      SUBROUTINE JMFEED
C...JetMap subroutine FEED signal to net.

C...Feeds the input signal into the network and calculates
C...MXNDJM and DW.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,MAXD=2)
      PARAMETER(BIAS=0.5)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JMINT1/ NDIM,ISW(10),NODES(0:MAXD+1),NBO
      SAVE /JNDAT1/,/JNINT1/,/JMINT1/

      DIMENSION INDW(MAXV)
      EQUIVALENCE (NTSELF(1),INDW(1))


      MXNDJM=0

      IF (ISW(1).EQ.1) THEN
C *** Sigmoidal unit ***

        ODUM=0.0
        IW=0
        DO 100 J=1,NODES(MAXD+1)
          RSUM=0.0
          DO 110 K=1,NODES(0)
            IW=IW+1
            DW(IW)=OIN(K)-W(IW)
            RSUM=RSUM+OIN(K)*W(IW)
110       CONTINUE
          O(J)=GJM((RSUM-BIAS)*PARJM(3),ISW(1))
          IF (O(J).GT.ODUM) THEN
            ODUM=O(J)
            MXNDJM=J
          ENDIF
100     CONTINUE

      ELSEIF(ISW(1).EQ.2) THEN
C *** Gaussian unit ***

        ODUM=EXP(-49.0)
        IW=0
        DO 200 J=1,NODES(MAXD+1)
          RSUM=0.0
          DO 210 K=1,NODES(0)
            IW=IW+1
            DW(IW)=OIN(K)-W(IW)
            RSUM=RSUM+DW(IW)**2
210       CONTINUE
          O(J)=GJM(RSUM*PARJM(3),ISW(1))
          IF (O(J).GT.ODUM) THEN
            ODUM=O(J)
            MXNDJM=J
          ENDIF
200     CONTINUE

      ENDIF

      IF (MXNDJM.EQ.0) CALL JMWARN(1)

      RETURN

C**** END OF JMFEED ****************************************************
      END
C***********************************************************************


      SUBROUTINE JMINDX(INOD,I,J)
C...JetMap subroutine INDeX.

C...If INOD>0 it returns the (I,J)-coordinates for INOD, if
C...INOD=0 it returns the INOD-number (in O array) for unit (I,J)
C...in net.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,MAXD=2)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JMINT1/ NDIM,ISW(10),NODES(0:MAXD+1),NBO
      SAVE /JNDAT1/,/JNINT1/,/JMINT1/

      DIMENSION INDW(MAXV)
      EQUIVALENCE (NTSELF(1),INDW(1))


      IF (MSTJM(8).NE.1) CALL JMERR(7)

      IF (INOD.GT.0) THEN
C...INOD -> (I,J)

        IF (NDIM.EQ.1) THEN
          J=1
          I=INOD
        ELSEIF (NDIM.EQ.2) THEN
          J=MOD(INOD,NODES(2))
          I=INOD/NODES(2)+1
          IF (J.EQ.0) THEN
            J=NODES(2)
            I=I-1
          ENDIF
        ENDIF

      ELSEIF (INOD.EQ.0) THEN
C...(I,J) -> INOD

        IF (NDIM.EQ.1) THEN
          INOD=I
        ELSEIF (NDIM.EQ.2) THEN
          INOD=(I-1)*NODES(2)+J
        ENDIF

      ENDIF

      RETURN

C**** END OF JMINDX ****************************************************
      END
C***********************************************************************


      SUBROUTINE JMINIT
C...JetMap subroutine INITialize net.

C...Initializes the net according to switches and parameters in
C.../JMDAT1/ and /JMDAT2/.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,MAXD=2)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JMINT1/ NDIM,ISW(10),NODES(0:MAXD+1),NBO
      SAVE /JNDAT1/,/JNINT1/,/JMINT1/

      DIMENSION INDW(MAXV)
      EQUIVALENCE (NTSELF(1),INDW(1))


C...Check if JNINIT has been called:
      IF(MSTJN(8).EQ.1) CALL JMERR(10)

C...Set parameters:
      CALL JMSEPA

C...Set initial values of weights:
      DO 100 IW=1,INDW(NODES(MAXD+1)+1)-1
        IDUM=IW
        IF (MSTJM(2).EQ.0) THEN
          W(IW)=RJN(IDUM)*PARJM(4)
        ELSEIF (MSTJM(2).EQ.1) THEN
          W(IW)=(2.*RJN(IDUM)-1.)*PARJM(4)
        ENDIF
100   CONTINUE

C...Normalize weights: 
      IF (MSTJM(7).EQ.1) CALL JMNORM

C...Write statistics on output file:

      IF (MSTJM(6).LT.0) RETURN

      CALL JNHEAD

      CALL JMSTAT(1)

      IF (MSTJM(5).EQ.0) THEN
        WRITE(MSTJM(6),600)
      ELSEIF (MSTJM(5).EQ.1) THEN
        WRITE(MSTJM(6),610)
      ELSEIF (MSTJM(5).EQ.2) THEN
        WRITE(MSTJM(6),660)
      ENDIF

      WRITE(MSTJM(6),*)

      IF (MSTJM(2).EQ.0) THEN
        WRITE(MSTJM(6),620) PARJM(4)
      ELSEIF (MSTJM(2).EQ.1) THEN
        WRITE(MSTJM(6),630) PARJM(4)
      ENDIF

      WRITE(MSTJM(6),*)

      IF (MSTJM(7).EQ.0) THEN
        WRITE(MSTJM(6),640)
      ELSEIF (MSTJM(7).EQ.1) THEN
        WRITE(MSTJM(6),650)
      ENDIF

      WRITE(MSTJM(6),*)

600   FORMAT(26X,'Self-organized Clustering')
610   FORMAT(25X,'Learning Vector Quantization')
620   FORMAT(16X,'Weights set randomly between 0.0 and + ',F6.3)
630   FORMAT(20X,'Weights set randomly between +/- ',F6.3)
640   FORMAT(26X,'Weights are not normalized')
650   FORMAT(28X,'Weights are normalized')
660   FORMAT(24X,'LVQ with neighborhood function')

      RETURN

C**** END OF JMINIT ****************************************************
      END
C***********************************************************************


      SUBROUTINE JMINWE(INOD)
C...JetMap subroutine INitial WEight.

C...Sets the weight vector for unit INOD equal to the 
C...input pattern stored in OIN.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,MAXD=2)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JMINT1/ NDIM,ISW(10),NODES(0:MAXD+1),NBO
      SAVE /JNDAT1/,/JNINT1/,/JMINT1/

      DIMENSION INDW(MAXV)
      EQUIVALENCE (NTSELF(1),INDW(1))


      IF (MSTJM(8).NE.1) CALL JMERR(9)

      DO 100 K=1,NODES(0)
        IW=INDW(INOD)+K-1
        W(IW)=OIN(K)
100   CONTINUE

      RETURN

C**** END OF JMINWE ****************************************************
      END
C***********************************************************************


      SUBROUTINE JMNBHD
C...JetMap subroutine NeighBourHooD.

C...Specifies the neighbourhood to update according to MSTJM(9).

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,MAXD=2)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JMINT1/ NDIM,ISW(10),NODES(0:MAXD+1),NBO
      SAVE /JNDAT1/,/JNINT1/,/JMINT1/

      DIMENSION INDW(MAXV),NBHD(MAXO,0:121)
      EQUIVALENCE (NSELF(1),NBHD(1,0)),(NTSELF(1),INDW(1))


      IF(MSTJM(8).NE.1) CALL JMERR(14)

      IF (NDIM.EQ.1.AND.ABS(MSTJM(9)).GT.121) THEN
        CALL JMWARN(3)
        MSTJM(9)=SIGN(121,MSTJM(9))
      ELSEIF (NDIM.EQ.2.AND.ABS(MSTJM(9)).GT.5) THEN
        CALL JMWARN(2)
        MSTJM(9)=SIGN(5,MSTJM(9))
      ENDIF

      IF (MSTJM(9).EQ.0) THEN
        DO 200 INOD=1,NODES(MAXD+1)
          NBHD(INOD,1)=INOD
          NBHD(INOD,0)=1
200     CONTINUE
        RETURN
      ENDIF

      IF (NDIM.EQ.1) THEN

        DO 100 INOD=1,NODES(MAXD+1)
          IF (MSTJM(11).GE.0) THEN
C...Non-periodic boundary:
            ISTRT=MAX(1,INOD-ABS(MSTJM(9)))
            IEND=MIN(MSTJM(11),INOD+ABS(MSTJM(9)))
          ELSE
C...Periodic boundary:
            ISTRT=INOD-ABS(MSTJM(9))
            IEND=ISTRT+2*ABS(MSTJM(9))
          ENDIF
          NBNUM=0
          DO 110 I=ISTRT,IEND
            IND=MOD(I-1,MSTJM(11))+1
            NBNUM=NBNUM+1
            NBHD(INOD,NBNUM)=IND
110       CONTINUE
          NBHD(INOD,0)=NBNUM
100     CONTINUE

      ELSEIF (NDIM.EQ.2) THEN

        IF (MSTJM(9).GT.0) THEN
C...Square neighbourhood:
          DO 120 INOD=1,NODES(MAXD+1)
            CALL JMINDX(INOD,IC,JC)
            IF (MSTJM(11).GE.0) THEN
C...Non-periodic in dim. 1:
              ISTRT=MAX(1,IC-MSTJM(9))
              IEND=MIN(MSTJM(11),IC+MSTJM(9))
            ELSE
C...Periodic in dim. 1:
              ISTRT=ABS(MSTJM(11))+IC-MSTJM(9)
              ISTRT=MOD(ISTRT-1,ABS(MSTJM(11)))+1
              IEND=ISTRT+2*MSTJM(9)
            ENDIF
            IF (MSTJM(12).GE.0) THEN
C...Non-periodic in dim.2:
              JSTRT=MAX(1,JC-MSTJM(9))
              JEND=MIN(MSTJM(12),JC+MSTJM(9))
            ELSE
C...Periodic in dim. 2:
              JSTRT=ABS(MSTJM(12))+JC-MSTJM(9)
              JSTRT=MOD(JSTRT-1,ABS(MSTJM(12)))+1
              JEND=JSTRT+2*MSTJM(9)
            ENDIF
            NBNUM=0
            DO 130 I=ISTRT,IEND
              DO 140 J=JSTRT,JEND
                IND=MOD(I-1,MSTJM(11))+1
                JND=MOD(J-1,MSTJM(12))+1
                NBNUM=NBNUM+1
                NBNOD=0
                CALL JMINDX(NBNOD,IND,JND)
                NBHD(INOD,NBNUM)=NBNOD
140           CONTINUE
130         CONTINUE
            NBHD(INOD,0)=NBNUM
120       CONTINUE

        ELSEIF (MSTJM(9).LT.0) THEN
C...Circular neighbourhood:
          DO 150 INOD=1,NODES(MAXD+1)
            CALL JMINDX(INOD,IC,JC)
            IF (MSTJM(11).GE.0) THEN
C...Non-periodic in dim. 1:
              ISTRT=MAX(1,IC+MSTJM(9))
              IEND=MIN(MSTJM(11),IC-MSTJM(9))
            ELSE
C...Periodic in dim. 1:
              ISTRT=ABS(MSTJM(11))+IC+MSTJM(9)
              ISTRT=MOD(ISTRT-1,ABS(MSTJM(11)))+1
              IEND=ISTRT-2*MSTJM(9)
              IC=ISTRT-MSTJM(9)
            ENDIF
            IF (MSTJM(12).GE.0) THEN
C...Non-periodic in dim.2:
              JSTRT=MAX(1,JC+MSTJM(9))
              JEND=MIN(MSTJM(12),JC-MSTJM(9))
            ELSE
C...Periodic in dim. 2:
              JSTRT=ABS(MSTJM(12))+JC+MSTJM(9)
              JSTRT=MOD(JSTRT-1,ABS(MSTJM(12)))+1
              JEND=JSTRT-2*MSTJM(9)
              JC=JSTRT-MSTJM(9)
            ENDIF
            NBNUM=0
            DO 160 I=ISTRT,IEND
              DO 170 J=JSTRT,JEND
                RDIST=SQRT(FLOAT((IC-I)**2+(JC-J)**2))
                IF (RDIST.LE.FLOAT(-MSTJM(9))) THEN
                  IND=MOD(I-1,ABS(MSTJM(11)))+1
                  JND=MOD(J-1,ABS(MSTJM(12)))+1
                  NBNUM=NBNUM+1
                  NBNOD=0
                  CALL JMINDX(NBNOD,IND,JND)
                  NBHD(INOD,NBNUM)=NBNOD
                ENDIF
170           CONTINUE
160         CONTINUE
            NBHD(INOD,0)=NBNUM
150       CONTINUE
        ENDIF

      ENDIF

      NBO=MSTJM(9)

      RETURN

C**** END OF JMNBHD ****************************************************
      END
C***********************************************************************


      SUBROUTINE JMNORM
C...JetMap subroutine NORMalize weights.

C...Normalizes the weights.

      PARAMETER(MAXV=2000,MAXM=150000,MAXD=2)

      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JMINT1/ NDIM,ISW(10),NODES(0:MAXD+1),NBO
      SAVE /JNINT1/,/JMINT1/

      DIMENSION INDW(MAXV)
      EQUIVALENCE (NTSELF(1),INDW(1))


      DO 100 INOD=1,NODES(MAXD+1)
        RNORM=0.0
        IWSTRT=INDW(INOD)
        IWEND=INDW(INOD)+NODES(0)-1
        DO 110 IW=IWSTRT,IWEND
          RNORM=RNORM+W(IW)*W(IW)
110     CONTINUE
        RNORM=SQRT(RNORM)
        DO 120 IW=IWSTRT,IWEND
          W(IW)=W(IW)/RNORM
120     CONTINUE
100   CONTINUE

      RETURN

C**** END OF JMNORM ****************************************************
      END
C***********************************************************************


      SUBROUTINE JMREAD(NF)
C...JetMap subroutine READ weights and parameters.

C...Reads weights and parameters from file NF and initializes the net.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,MAXD=2)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JMINT1/ NDIM,ISW(10),NODES(0:MAXD+1),NBO
      SAVE /JNDAT1/,/JNINT1/,/JMINT1/

      DIMENSION INDW(MAXV)
      EQUIVALENCE (NTSELF(1),INDW(1))

      CHARACTER LINE*100


      NDUM=MSTJM(6)

      IF(NF.LT.0) THEN

C...Unformatted read
       JF=-NF
       READ(JF) IVERS
       IF(IVERS.GE.0) CALL JMERR(12)
       READ(JF) MSTJM,PARJM
       
       CALL JMSEPA

       DO 100 IW=1,INDW(NODES(MAXD+1)+1)-1
        READ(JF) W(IW)
100    CONTINUE

      ELSE
C...Formatted read

        NFSAVE=MSTJM(6)

        READ(NF,690)LINE
        IF (LINE(27:28).EQ.'Du') CALL JMERR(12)
        READ(NF,*)
        READ(NF,*)
        READ(NF,710)FVERS
        IVERS=INT(FVERS*10.0+0.001)

900     READ(NF,690)LINE
        IF (LINE(1:17).NE.'         I      1') GOTO 900

        READ(NF,610)LINE(1:10),(MSTJM(I),I=1,10)
        READ(NF,610)LINE(1:10),(MSTJM(10+I),I=1,10)
        READ(NF,620)LINE(1:10),(PARJM(I),I=1,10)
        READ(NF,620)LINE(1:10),(PARJM(10+I),I=1,10)
        READ(NF,*)

        MSTJM(6)=NFSAVE

        CALL JMSEPA

        IF (NDIM.EQ.1) THEN
          DO 110 INOD=1,NODES(MAXD+1)
            READ(NF,*)
            READ(NF,630)LINE,I
            IW=INDW(INOD)-1
            READ(NF,640)(W(IW+K),K=1,NODES(0))
110       CONTINUE
        ELSEIF(NDIM.EQ.2) THEN
          DO 120 INOD=1,NODES(MAXD+1)
            READ(NF,*)
            READ(NF,650)LINE,I,LINE,J,LINE
            IW=INDW(INOD)-1
            READ(NF,640)(W(IW+K),K=1,NODES(0))
120       CONTINUE
        ENDIF

      ENDIF

      MSTJM(6)=NDUM

C...Write statistics on output file:
      IF (MSTJM(6).LT.0) RETURN
      CALL JNHEAD
      CALL JMSTAT(1)
      WRITE(MSTJM(6),600)

600   FORMAT(21X,'Weights read from file')
610   FORMAT(A,10I7)
620   FORMAT(A,10F7.4)
630   FORMAT(A5,I2)
640   FORMAT(10F9.4)
650   FORMAT(A6,I2,A1,I2,A1)
690   FORMAT(A)
700   FORMAT(I2,A)
710   FORMAT(TR63,F2.1)

      RETURN

C**** END OF JMREAD ****************************************************
      END
C***********************************************************************


      SUBROUTINE JMSEPA
C...JetMap subroutine SEt PArameters.

C...Sets parameters in /JMINT1/.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,MAXD=2)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JMINT1/ NDIM,ISW(10),NODES(0:MAXD+1),NBO
      SAVE /JNDAT1/,/JNINT1/,/JMINT1/

      DIMENSION INDW(MAXV)
      EQUIVALENCE (NTSELF(1),INDW(1))


C...Number of dimensions:
      NDIM=MSTJM(1)
      IF (NDIM.GT.MAXD.OR.NDIM.LE.0) CALL JMERR(1)

C...Number of nodes:
      IF (MSTJM(10).GT.MAXI) CALL JMERR(2)
      IF (NDIM.EQ.1) THEN
        NODES(MAXD+1)=ABS(MSTJM(11))
      ELSEIF (NDIM.EQ.2) THEN
        NODES(MAXD+1)=ABS(MSTJM(11)*MSTJM(12))
      ENDIF
      IF (NODES(MAXD+1).GT.MAXO) CALL JMERR(3)
      DO 100 IDIM=1,MAXD+1
        NODES(IDIM-1)=ABS(MSTJM(9+IDIM))
100   CONTINUE

C *** Set internal switches: ***
C...Response function:
      ISW(1)=MSTJM(3)
C...Error measure:
      ISW(2)=MSTJM(4)

C *** Calculate weight pointers: ***
      IF (NODES(MAXD+1)*NODES(0).GT.MAXM) CALL JMERR(4)
      DO 110 INOD=1,NODES(MAXD+1)+1
        INDW(INOD)=(INOD-1)*NODES(0)+1
110   CONTINUE

      MSTJM(8)=1

C...Initialize neighbourhood:
      CALL JMNBHD

      RETURN

C**** END OF JMSEPA ****************************************************
      END
C***********************************************************************


      SUBROUTINE JMSTAT(IS)

C...JetMap subroutine output STATistics.

C...Statistics chosen by IS is written on file MSTJM(6).

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,MAXD=2)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JMINT1/ NDIM,ISW(10),NODES(0:MAXD+1),NBO
      SAVE /JNDAT1/,/JNINT1/,/JMINT1/

      DIMENSION INDW(MAXV)
      EQUIVALENCE (NTSELF(1),INDW(1))


      IF(IS.EQ.1) THEN
        WRITE(MSTJM(6),*)
        WRITE(MSTJM(6),600) NDIM
        DO 100 IDIM=1,NDIM
          WRITE(MSTJM(6),610) NODES(IDIM),IDIM
100     CONTINUE
        WRITE(MSTJM(6),620) NODES(0)
        WRITE(MSTJM(6),*)
        WRITE(MSTJM(6),*)

      ELSEIF(IS.EQ.2) THEN
        WRITE(MSTJM(6),*)
        WRITE(MSTJM(6),630)
        WRITE(MSTJM(6),*)
        WRITE(MSTJM(6),640)'I',(I,I=1,10)
        WRITE(MSTJM(6),640)'MSTJM I',(MSTJM(I),I=1,10)
        WRITE(MSTJM(6),640)'MSTJM 10+I',(MSTJM(10+I),I=1,10)
        WRITE(MSTJM(6),650)'PARJM I',(PARJM(I),I=1,10)
        WRITE(MSTJM(6),650)'PARJM 10+I',(PARJM(10+I),I=1,10)
        WRITE(MSTJM(6),*)

      ELSE
        NWFAC=INDW(NODES(MAXD+1)+1)-1
        WRITE(MSTJM(6),660)NWFAC
      ENDIF

600   FORMAT(22X,'Initialized for a ',I1,'-dimensional map with')
610   FORMAT(27X,I3,' nodes in dimension ',I1)
620   FORMAT(27X,I3,' input nodes')
630   FORMAT(18X,'Values of parameters and switches in JETNET')
640   FORMAT(A10,10I7)
650   FORMAT(A10,10F7.4)
660   FORMAT(5X,'Time factor for this map:',I10)

      RETURN

C**** END OF JMSTAT ****************************************************
      END
C***********************************************************************


      SUBROUTINE JMTEST
C...JetMap subroutine TEST

C...Sets the values of OUT according to given pattern in OIN
C...and current values of weights.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,MAXD=2)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JMINT1/ NDIM,ISW(10),NODES(0:MAXD+1),NBO
      SAVE /JNDAT1/,/JNINT1/,/JMINT1/

      DIMENSION INDW(MAXV)
      EQUIVALENCE (NTSELF(1),INDW(1))


      IF (MSTJM(8).NE.1) CALL JMERR(7)

      CALL JMFEED

      DO 100 INOD=1,NODES(MAXD+1)
        OUT(INOD)=O(INOD)
100   CONTINUE

      RETURN

C**** END OF JMTEST ****************************************************
      END
C***********************************************************************


      SUBROUTINE JMTRAL
C...JetMap subroutine TRaining ALgorithm.

C...Trains the net according to /JMDAT1/.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,MAXD=2)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JMINT1/ NDIM,ISW(10),NODES(0:MAXD+1),NBO
      SAVE /JNDAT1/,/JNINT1/,/JMINT1/

      DIMENSION INDW(MAXV),NBHD(MAXO,0:121)
      EQUIVALENCE (NSELF(1),NBHD(1,0)),(NTSELF(1),INDW(1))


      IF (MSTJM(8).NE.1) CALL JMERR(6)

C...Update neighbourhood:
      IF (MSTJM(9).NE.NBO) CALL JMNBHD

      CALL JMFEED
      IF (MXNDJM.EQ.0) RETURN

      IF (MSTJM(5).EQ.0) THEN
C...Normal updating
        DO 100 INBH=1,NBHD(MXNDJM,0)
          IWSTRT=INDW(NBHD(MXNDJM,INBH))
          IWEND=INDW(NBHD(MXNDJM,INBH)+1)-1
          DO 110 IW=IWSTRT,IWEND
            W(IW)=W(IW)+PARJM(1)*DW(IW)
110       CONTINUE
100     CONTINUE

      ELSEIF (MSTJM(5).EQ.1) THEN
C...Learning Vector Quantization
        IWSTRT=INDW(MXNDJM)
        IWEND=INDW(MXNDJM+1)-1
        IF (OUT(MXNDJM).LT.0.5) THEN
C...Wrong answer:
          DO 200 IW=IWSTRT,IWEND
            W(IW)=W(IW)-PARJM(1)*DW(IW)
200       CONTINUE
        ELSE
C...Correct answer:
          DO 210 IW=IWSTRT,IWEND
            W(IW)=W(IW)+PARJM(1)*DW(IW)
210       CONTINUE
        ENDIF

      ELSEIF (MSTJM(5).EQ.2) THEN
C...LVQ with neighborhood function:
        IWSTRT=INDW(MXNDJM)
        IWEND=INDW(MXNDJM+1)-1
        DO 220 IW=IWSTRT,IWEND
          W(IW)=W(IW)+PARJM(1)*DW(IW)*OUT(MXNDJM)
220     CONTINUE

      ENDIF

      DO 300 INOD=1,NODES(MAXD+1)
        OUT(INOD)=O(INOD)
300   CONTINUE

C...If normalize:
      IF (MSTJM(7).EQ.1) CALL JMNORM

      RETURN

C**** END OF JMTRAL ****************************************************
      END
C***********************************************************************


      SUBROUTINE JMWARN(IWARN)
C...JetMap subroutine WARNing.

C...Writes out a warning on file MSTJM(6).

      PARAMETER(MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      SAVE /JNDAT1/


      MSTJN(34)=MSTJN(34)+1
      MSTJN(33)=IWARN
      IF(MSTJN(34).LT.MSTJN(32)) THEN

       WRITE(MSTJM(6),600)IWARN

       IF (IWARN.EQ.1) THEN
         WRITE(MSTJM(6),610)
       ELSEIF (IWARN.EQ.2) THEN
         WRITE(MSTJM(6),620)MSTJM(9)
         WRITE(MSTJM(6),630)
         WRITE(MSTJM(6),640)
       ELSEIF (IWARN.EQ.3) THEN
         WRITE(MSTJM(6),620)MSTJM(9)
         WRITE(MSTJM(6),650)
         WRITE(MSTJM(6),660)
       ENDIF

      ELSEIF(MSTJN(31).GE.1) THEN

       CALL JMERR(13)

      ENDIF

600   FORMAT(' *** JETMAP WARNING:',I2,' ***')
610   FORMAT(' No response in net for presented input')
620   FORMAT(' Illegal value of neighbourhood size (',I2,')')
630   FORMAT(' absolute value of MSTJM(9) must be within [0,5]')
640   FORMAT(' MSTJM(9) set to limit value 5')
650   FORMAT(' absolute value of MSTJM(9) must be within [0,121]')
660   FORMAT(' MSTJM(9) set to limit value 121')

      RETURN

C**** END OF JMWARN ****************************************************
      END
C***********************************************************************


      BLOCK DATA JNDATA

C...JetNet block DATA 

C...Initial values for parameters and switches for JETNET.

      PARAMETER(MAXV=2000,MAXM=150000,MAXI=1000,MAXO=1000,MAXD=2)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)
      COMMON /JNINT1/ O(MAXV),A(MAXV),D(MAXV),T(MAXV),DT(MAXV),
     &                W(MAXM),DW(MAXM),NSELF(MAXM),NTSELF(MAXV),
     &                G(MAXM+MAXV),ODW(MAXM),ODT(MAXV),ETAV(MAXM+MAXV)
      COMMON /JNINT2/ M(0:10),MV0(11),MM0(11),NG(10),NL,IPOTT,
     &                ER1,ER2,SM(10),ICPON
      COMMON /JNINT4/ ILINON,NC,G2,NIT,ERRLN(0:3),DERRLN,STEPLN(0:3),
     &                STEPMN,ERRMN,IEVAL,ISUCC,ICURVE,NSC,GVEC2
      COMMON /JMINT1/ NDIM,ISW(10),NODES(0:MAXD+1),NBO
      COMMON /JNGAUS/ ISET,GASDEV
      COMMON /JNDATR/ MRJN(5),RRJN(100)


C...Brief explanation of parameters and switches :
C...
C.../JNDAT1/:
C...Feed-forward net:
C...MSTJN(1) (D=3)      number of layers in net
C...MSTJN(2) (D=10)     number of patterns per update in JNTRAL
C...MSTJN(3) (D=1)      overall transfer function used in net
C...        1 -> g(x)=1/(1+exp(-2x))
C...        2 -> g(x)=tanh(x)
C...        3 -> g(x)=exp(x) (only used internally for Potts-nodes)
C...        4 -> g(x)=x
C...        5 -> g(x)=1/(1+exp(-2x)) (only used internally for
C...             entropy error)
C...MSTJN(4) (D=0)      error measure
C...       -1 -> log-squared error:     E = -log(1-(o-t)**2)
C...        0 -> summed square error:   E = 0.5*(o-t)**2
C...        1 -> entropy error:         E = -t*log(o) + (1-t)*log(1-o)
C...      >=2 -> Kullback measure, using Potts nodes of dimension 
C...             MSTJN(4):              E = t*log(t/o)
C...MSTJN(5) (D=0)      updating procedure
C...        0 -> standard Back-Propagation updating
C...        1 -> Manhattan updating
C...        2 -> Langevin updating
C...        3 -> Quickprop
C...        4 -> Conjugate Gradient - Polak-Ribiere
C...        5 -> Conjugate Gradient - Hestenes-Stiefel
C...        6 -> Conjugate Gradient - Fletcher-Reeves
C...        7 -> Conjugate Gradient - Shanno
C...        8 -> Terminate Conjugate Gradient search
C...        9 -> No updating
C...       10 -> Scaled Conjugate Gradient - Polak-Ribiere
C...       11 -> Scaled Conjugate Gradient - Hestenes-Stiefel
C...       12 -> Scaled Conjugate Gradient - Fletcher-Reeves
C...       13 -> Scaled Conjugate Gradient - Shanno
C...       14 -> Terminate Scaled Conjugate Gradient Search
C...       15 -> Rprop
C...MSTJN(6) (D=6)      file number for output statistics
C...MSTJN(7) (I)        number of calls to JNTRAL
C...MSTJN(8) (I)        initialization done -> 0 = no
C...MSTJN(9) (D=100)    number of updates per epoch 
C...MSTJN(10+I)         number of nodes in layer I (I=0 => input layer)
C...MSTJN(10) (D=16)
C...MSTJN(11) (D=8)
C...MSTJN(12) (D=1)
C...MSTJN(13-20) (D=0)
C...MSTJN(21) (D=0)     pruning (>0 -> on)
C...MSTJN(22) (D=0)     saturation measure (<>0 -> on)
C...                    <0 -> update temperature to give measure ~0.5
C...MSTJN(23,24) (D=0)  geometry of input nodes for receptive fields
C...MSTJN(25,26) (D=0)  geometry of receptive fields
C...MSTJN(27)    (D=1)  number of hidden nodes per receptive field
C...MSTJN(28-30) (D=0)  precision in bits (0 -> machine precision) for
C...                    sigmoid functions (28), thresholds (29) and
C...                    weights (30)
C...MSTJN(31)    (D=1)  Warning procedure
C...        0 -> No action is taken after a warning
C...        1 -> The execution is stopped after the program 
C...             has experienced MSTJN(32) warnings
C...             in any case only MSTJN(32) warning messages are printed
C...             out.
C...MSTJN(32) (D=10)    Maximum number of warning messages to be  
C...                    printed. As described above.
C...MSTJN(33) (I)       code for latest warning issued by the program.
C...MSTJN(34) (I)       Number of warnings issued by the program so far.
C...MSTJN(35) (D=10)    Max. number of iterations allowed in line search.
C...MSTJN(36) (D=10)    Max. number of allowed restarts in line search.
C...MSTJN(37) (I)       Status of line search
C...        0 -> Minimum found
C...        1 -> Searching for minimum
C...MSTJN(38) (I)       Number of restarts in Quickprop/ConjGr/ScConjGr
C...MSTJN(39) (I)       Number of calls to JNHESS.
C...MSTJN(40)           not used
C...
C...
C...PARJN(1) (D=0.001)  learning parameter eta
C...PARJN(2) (D=0.5)    momentum term alfa
C...PARJN(3) (D=1.0)    overall inverse temperature beta
C...PARJN(4) (D=0.1)    width of initial weights
C...        > 0 -> [-width,+width]
C...        < 0 -> [0,+width]
C...PARJN(5) (D=0.0)    forgetting parameter epsilon
C...PARJN(6) (D=0.0)    noise width in Langevin equation
C...PARJN(7) (R)        last error per node
C...PARJN(8) (R)        mean error in last update
C...PARJN(9) (R)        mean error last epoch (equal to MSTJN(9) updates)
C...PARJN(10)(R)        weighted mean average used in pruning
C...PARJN(11) (D=1.0)   change in eta (scale factor per epoch)
C...        > 0 -> Geometric with "bold driver" dynamics
C...        < 0 -> Geometric decrease of eta
C...PARJN(12) (D=1.0)   change in momentum alpha (scale factor per epoch)
C...PARJN(13) (D=1.0)   change in temperature (scale factor per epoch)
C...PARJN(14) (D=0.0)   pruning parameter lambda
C...PARJN(15) (D=1.E-6) change in lambda
C...PARJN(16) (D=0.9)   parameter gamma used for calculation of PARJN(10)
C...PARJN(17) (D=0.9)   pruning "cut-off"
C...PARJN(18) (D=1.0)   scale parameter W(0), used in pruning
C...PARJN(19) (D=0.0)   target error when pruning
C...PARJN(20) (D=1.0)   decrease in Langevin noise (scale factor per epoch)
C...PARJN(21) (D=1.75)  maximum scale for Quickprop updating
C...PARJN(22) (D=1000.) maximum allowed size of weights in Quickprop
C...PARJN(23) (D=0.0)   constant added to g'(x) to avoid 'flat spot'
C...PARJN(24) (D=0.1)   line search convergence parameter (0 < ... < 1)
C...PARJN(25) (D=0.05)  tolerance of minimum in line search
C...PARJN(26) (D=0.001) minimum allowed change in error in line search
C...PARJN(27) (D=2.0)   maximum allowed step size in line search
C...PARJN(28) (D=1.E-4) constant sigma_0 used in SCG
C...PARJN(29) (D=1.E-6) initial value for lambda in SCG
C...PARJN(30) (D=1.2)   scale-up factor used in Rprop
C...PARJN(31) (D=0.5)   scale-down factor used in Rprop
C...PARJN(32) (D=50.)   maximum scale-up factor in Rprop
C...PARJN(33) (D=1.E-6) minimum scale-down factor in Rprop
C...PARJN(34-40)        not used
C...
C...
C...Self-organizing net:
C...MSTJM(1)   (D=1)    number of dimensions in net
C...MSTJM(2)   (D=0)    symmetry of initial weights
C...        0 -> [0,+width]
C...        1 -> [-width,+width]
C...MSTJM(3)   (D=2)    response function
C...        1 -> g(x)=0.5*(1.0+tanh(x) : for normalized data
C...        2 -> g(x)=exp(-x)          : for unnormalized data
C...MSTJM(4)   (D=1)    error measure
C...        1 -> summed square error
C...MSTJM(5)   (D=0)    updating procedure
C...        0 -> unsupervized clustering & topological ordering
C...        1 -> Learning Vector Quantization (LVQ 1)
C...        2 -> as 1, but with neighborhood function.
C...MSTJM(6)   (D=6)     output file number
C...MSTJM(7)   (D=0)     normalize weights or not
C...        0 -> unnormalized
C...        1 -> normalized
C...MSTJM(8) (I)         initialization done
C...MSTJM(9)   (D=0)     neighbourhood size
C...        0< -> square neighbourhood
C...        <0 -> circular neighbourhood
C...MSTJM(10)  (D=8)     number of input nodes
C...MSTJM(11)  (D=10)    number of nodes in dimension 1.
C...        <0 -> periodic boundary
C...MSTJM(12)  (D=1)     number of nodes in dimension 2.
C...        <0 -> periodic boundary
C...MSTJM(13-20)         not used
C...
C...
C...PARJM(1)   (D=0.001) learning parameter eta
C...PARJM(2)   (D=0.0)   not used
C...PARJM(3)   (D=0.01)  overall inverse temperature beta
C...PARJM(4)   (D=0.5)   initial width of weights
C...PARJM(5-20)          not used
C...
C...
C.../JNDAT2/:
C...TINV(I) (D=0.0)     inverse temperature of layer I (if 0 use PARJN(3))
C...
C...IGFN(I) (D=0)       sigmoid function for layer I (if 0 use MSTJN(3))
C...
C...ETAL(I) (D=0.0)     learning parameter in layer I (if 0 use PARJN(1))
C...
C...WIDL(I) (D=0.0)     initial width in layer I (if 0 use PARJN(4))
C...
C...SATM(I) (R)         saturation measure "S" for layer I.
C...      MSTJN(3)=1 -> S = sum[(1.-2.*O(J))**2]
C...      MSTJN(3)=2 -> S = sum[O(J)**2]
C...
C...End of description


      DATA MSTJN/3,10,1,0,0,6,0,0,100,16,8,1,14*0,1,3*0,
     &           1,10,2*0,10,10,4*0/
      DATA PARJN/0.001,0.5,1.0,0.1,6*0.0,
     &           3*1.0,0.0,1.E-6,0.9,0.9,1.0,0.0,1.0,
     &           1.75,1000.0,0.0,0.1,0.05,0.001,2.0,1.E-4,1.E-6,1.2,
     &           0.5,50.,1.E-6,7*0.0/
      DATA MSTJM/1,0,2,1,0,6,0,0,0,10,10,1,8*0/
      DATA PARJM/0.001,0.0,0.01,0.5,16*0.0/
      DATA TINV/10*0.0/
      DATA IGFN/10*0/
      DATA ETAL/10*0.0/
      DATA WIDL/10*0.0/
      DATA SATM/10*0.0/
      DATA ER1/0.0/
      DATA ER2/0.0/
      DATA SM/10*0.0/
      DATA NBO/10/
      DATA ICPON/0/
      DATA ILINON,NC,NIT,IEVAL,ISUCC,ICURVE,NSC/4*0,1,2*0/
      DATA G2,ERRLN,DERRLN,STEPLN,STEPMN,ERRMN,GVEC2/13*0.0/
      DATA ISET/0/
      DATA GASDEV/0.0/
      DATA MRJN/19780503,0,0,97,33/

C**** END OF JNDATA ****************************************************
      END
C***********************************************************************

      SUBROUTINE JNTDEC(METHOD)
C...JetNet subroutine Test-DECk

C...Runs a test-program using data from two overlapping Gaussian
C...distributions in the input space. The test-program uses the
C...method specified by METHOD.

      PARAMETER(MAXI=1000,MAXO=1000)

      COMMON /JNDAT1/ MSTJN(40),PARJN(40),MSTJM(20),PARJM(20),
     &                OIN(MAXI),OUT(MAXO),MXNDJM
      SAVE /JNDAT1/

      PARAMETER(INDIM=5,HIDDEN=10,NTRAIN=5000,NTEST=10000,NEPOCH=100)
      PARAMETER(WID1=1.,WID2=2.,XI=0.00,BAYES=85.2)
      DIMENSION TIN(NTRAIN+NTEST,INDIM),TOUT(NTRAIN+NTEST)


      WRITE(MSTJN(6),600)

      WRITE(MSTJN(6),610)INDIM
      WRITE(MSTJN(6),620)WID1,WID2
      WRITE(MSTJN(6),621)XI
      WRITE(MSTJN(6),*)

C...Generate data:
      WRITE(MSTJN(6),625)
      DO 100 IPAT=1,NTRAIN+NTEST
        IDUM=IPAT
        IF (RJN(IDUM).GT.0.5) THEN
          DO 110 I=1,INDIM
            TIN(IPAT,I)=WID1*GAUSJN(IDUM)
110       CONTINUE
          TOUT(IPAT)=1.0
        ELSE
          TIN(IPAT,1)=WID2*GAUSJN(IDUM)+XI
          DO 120 I=2,INDIM
            TIN(IPAT,I)=WID2*GAUSJN(IDUM)
120       CONTINUE
          TOUT(IPAT)=0.0
        ENDIF
100   CONTINUE
      WRITE(MSTJN(6),626)

C...Set network architecture: MSTJN(1)-layered network with 
C...MSTJN(11) hidden nodes, MSTJN(12) output nodes and 
C...MSTJN(10) inputs.
      MSTJN(1)=3
      MSTJN(10)=INDIM
      MSTJN(11)=HIDDEN
      MSTJN(12)=1

C...Set sigmoid function: 
      MSTJN(3)=1

C...Initial width of weights:
      PARJN(4)=0.5

C...Choose updating method
      MSTJN(5)=METHOD
      IF ((MSTJN(5).EQ.8).OR.(MSTJN(5).EQ.9).OR.(MSTJN(5).EQ.14).OR.
     &(MSTJN(5).LT.0).OR.(MSTJN(5).GT.15)) THEN
        WRITE(MSTJN(6),660)
        STOP 0
      ENDIF

C...Initialize network:
      CALL JNINIT

C...Set parameters suitable for the given method of updating
      IF (MSTJN(5).EQ.0) THEN
C...Normal Backprop
        PARJN(1)=2.0
        PARJN(2)=0.5
        PARJN(11)=0.999
      ELSEIF (MSTJN(5).EQ.1) THEN
C...Manhattan
        PARJN(1)=0.05
        PARJN(2)=0.5
        PARJN(11)=-0.99
      ELSEIF (MSTJN(5).EQ.2) THEN
C...Langevin
        PARJN(1)=1.0
        PARJN(2)=0.5
        PARJN(6)=0.01
        PARJN(11)=0.999
        PARJN(20)=0.99
      ELSEIF (MSTJN(5).EQ.3) THEN
C...Quickprop
        PARJN(1)=2.0
        PARJN(2)=0.0
        PARJN(6)=0.0
        PARJN(11)=1.0
        PARJN(20)=1.0
        MSTJN(2)=NTRAIN
      ELSEIF ((MSTJN(5).GE.4).AND.(MSTJN(5).LE.7)) THEN
C...Conjugate Gradient
        PARJN(1)=1.0
        MSTJN(2)=NTRAIN
      ELSEIF ((MSTJN(5).GE.10).AND.(MSTJN(5).LE.13)) THEN
C...Scaled Conjugate Gradient
        MSTJN(2)=NTRAIN
      ELSEIF (MSTJN(5).EQ.15) THEN
C...Rprop
        PARJN(1)=1.0
        MSTJN(2)=NTRAIN
      ENDIF

C...Define the size of one epoch. Note that for batch training, the
C...number of patterns per update, MSTJN(2), must be set to the
C...total number of training patterns, and hence MSTJN(9), the
C...number of updates per epoch must be set to one.
      MSTJN(9)=MAX(1,NTRAIN/MSTJN(2))

C...Other parameters keep their default values.

      WRITE(MSTJN(6),*)
      WRITE(MSTJN(6),630)

      TESTMX=0.0
      TRNMX=0.0
C...Main loop over epochs:
      DO 300 IEPOCH=1,NEPOCH

C...Training loop:
        NRIGHT=0
        DO 310 IP=1,NTRAIN
          IF (MSTJN(5).LE.2) THEN
C...Note that for non-batch training it is often a good idea to pick
C...training patterns at random
            IPAT=INT(RJN(IP)*FLOAT(NTRAIN))+1
          ELSE
            IPAT=IP
          ENDIF

C...Put pattern into OIN:
          DO 320 I=1,MSTJN(10)
            OIN(I)=TIN(IPAT,I)
320       CONTINUE
C...Put target output value into OUT:
          OUT(1)=TOUT(IPAT)

C...Invoke training algorithm:
          CALL JNTRAL

C...Calculate performance on training set:
          IF (ABS(OUT(1)-TOUT(IPAT)).LT.0.5) NRIGHT=NRIGHT+1

310     CONTINUE
        TRAIN=FLOAT(NRIGHT)/FLOAT(NTRAIN)

        IF (MOD(IEPOCH,10).EQ.0) THEN
C...Testing loop:
          NRIGHT=0
          DO 330 IPAT=NTRAIN+1,NTRAIN+NTEST

C...Put pattern into OIN:
            DO 340 I=1,MSTJN(10)
              OIN(I)=TIN(IPAT,I)
340         CONTINUE

C...Get network output:
            CALL JNTEST

C...Calculate performance on test set (=generalization):
            IF (ABS(OUT(1)-TOUT(IPAT)).LT.0.5) NRIGHT=NRIGHT+1
330       CONTINUE
          TEST=FLOAT(NRIGHT)/FLOAT(NTEST)
          IF ((MSTJN(5).GT.3).AND.(MSTJN(5).LT.15)) THEN
            IF (TRAIN.GT.TRNMX) THEN
              TRNMX=TRAIN
              TESTMX=TEST
            ENDIF
            TEST=TESTMX
            TRAIN=TRNMX
          ENDIF


C...Display performance:
          WRITE(MSTJN(6),640)IEPOCH,TRAIN,TEST
        ENDIF

C...Terminate CG and SCG training:
        IF (IEPOCH.EQ.NEPOCH-1) THEN
          IF ((MSTJN(5).GT.3).AND.(MSTJN(5).LT.15)) THEN
            IF (MSTJN(5).LT.9) THEN
              MSTJN(5)=8
            ELSE
              MSTJN(5)=14
            ENDIF
            TRNMX=0.0
            TESTMX=0.0
          ENDIF
        ENDIF

300   CONTINUE

      WRITE(MSTJN(6),*)
      WRITE(MSTJN(6),650)BAYES
      IF (METHOD.EQ.0) THEN
        WRITE(MSTJN(6),670)
      ELSEIF (METHOD.EQ.1) THEN
        WRITE(MSTJN(6),680)
      ELSEIF (METHOD.EQ.2) THEN
        WRITE(MSTJN(6),690)
      ELSEIF (METHOD.EQ.3) THEN
        WRITE(MSTJN(6),700)
      ELSEIF (METHOD.EQ.4) THEN
        WRITE(MSTJN(6),710)
      ELSEIF (METHOD.EQ.5) THEN
        WRITE(MSTJN(6),720)
      ELSEIF (METHOD.EQ.6) THEN
        WRITE(MSTJN(6),730)
      ELSEIF (METHOD.EQ.7) THEN
        WRITE(MSTJN(6),740)
      ELSEIF (METHOD.EQ.10) THEN
        WRITE(MSTJN(6),750)
      ELSEIF (METHOD.EQ.11) THEN
        WRITE(MSTJN(6),760)
      ELSEIF (METHOD.EQ.12) THEN
        WRITE(MSTJN(6),770)
      ELSEIF (METHOD.EQ.13) THEN
        WRITE(MSTJN(6),780)
      ELSEIF (METHOD.EQ.15) THEN
        WRITE(MSTJN(6),790)
      ENDIF

600   FORMAT(31X,'JETNET Test-Deck')
610   FORMAT(15X,'Two overlapping Gaussian distributions in ',
     &I2,' dimensions.')
620   FORMAT(15X,'Their standard deviations are ',F3.1,' and ',F3.1)
621   FORMAT(15X,'Their mean values are separated by ',F4.2)
625   FORMAT(15X,'Generating training and test patterns...')
626   FORMAT(15X,'...done generating data.')
630   FORMAT('   Epoch   /  Training  /  General. ')
640   FORMAT(I8,2X,2(' /',F9.3,2X))
650   FORMAT(' The optimal generalization performance is ',F4.1,'%')
660   FORMAT(' Undefined training algorithm in call to JNTDEC')
670   FORMAT(' Backprop should reach (81.0 +- 2.2)% in 100 epochs')
680   FORMAT(' Manhattan should reach (84.3 +- 0.6)% in 100 epochs')
690   FORMAT(' Langevin should reach (82.9 +- 1.8)% in 100 epochs')
700   FORMAT(' Quickprop should reach (82.8 +- 8.8)% in 100 epochs')
710   FORMAT(' Polak-Ribiere CG should reach (79.0 +- 7.0)% in 100',
     &' epochs')
720   FORMAT(' Hestenes-Stiefel CG should reach (79.8 +- 5.6)% in 100',
     &' epochs')
730   FORMAT(' Fletcher-Reeves CG should reach (79.6 +- 5.6)% in 100',
     &' epochs')
740   FORMAT(' Shanno CG should reach (71.7 +- 11.6)% in 100 epochs')
750   FORMAT(' Polak-Ribiere SCG should reach (84.0 +- 1.6)% in 100',
     &' epochs')
760   FORMAT(' Hestenes-Stiefel SCG should reach (84.1 +- 2.6)% in 100',
     &' epochs')
770   FORMAT(' Fletcher-Reeves SCG should reach (81.4 +- 5.2)% in 100',
     &' epochs')
780   FORMAT(' Shanno SCG should reach (70.7 +- 8.1)% in 100 epochs')
790   FORMAT(' Rprop should reach (83.5 +- 2.2)% in 100 epochs')

      RETURN

C**** END OF JNTDEC ****************************************************
      END
C***********************************************************************

      REAL FUNCTION RJN(IDUM)
C...JetNet function Random number generator.
C...Generates random numbers uniformly distributed in ]0,1[

C...This function is taken from the Lund program JETSET
C...written by T. Sjostrand.
C...The algorithm is due to Marsaglia, Zaman and Tsang:
C...Stat. Prob. Lett., vol. 9, (1990)
C...This function is very much based on a routine
C...written by F.James: F.James, Comp. Phys. Comm., vol 60 (1990).

C...Names have been changed w.r.t. the JETSET function RLU and
C...the switch that determines the current position has been
C...removed.


      COMMON /JNDATR /MRJN(5),RRJN(100)
      SAVE /JNDATR/


C...Initialize generation from given seed.
      IF(MRJN(2).EQ.0) THEN
        IJ=MOD(MRJN(1)/30082,31329)
        KL=MOD(MRJN(1),30082)
        I=MOD(IJ/177,177)+2
        J=MOD(IJ,177)+2
        K=MOD(KL/169,178)+1
        L=MOD(KL,169)
        DO 110 II=1,97
        S=0.
        T=0.5
        DO 100 JJ=1,24
        M=MOD(MOD(I*J,179)*K,179)
        I=J
        J=K
        K=M
        L=MOD(53*L+1,169)
        IF(MOD(L*M,64).GE.32) S=S+T
  100   T=0.5*T
  110   RRJN(II)=S
        TWOM24=1.
        DO 120 I24=1,24
  120   TWOM24=0.5*TWOM24
        RRJN(98)=362436.*TWOM24
        RRJN(99)=7654321.*TWOM24
        RRJN(100)=16777213.*TWOM24
        MRJN(2)=1
        MRJN(3)=0
        MRJN(4)=97
        MRJN(5)=33
      ENDIF

C...Generate next random number.
  130 RUNI=RRJN(MRJN(4))-RRJN(MRJN(5))
      IF(RUNI.LT.0.) RUNI=RUNI+1.
      RRJN(MRJN(4))=RUNI
      MRJN(4)=MRJN(4)-1
      IF(MRJN(4).EQ.0) MRJN(4)=97
      MRJN(5)=MRJN(5)-1
      IF(MRJN(5).EQ.0) MRJN(5)=97
      RRJN(98)=RRJN(98)-RRJN(99)
      IF(RRJN(98).LT.0.) RRJN(98)=RRJN(98)+RRJN(100)
      RUNI=RUNI-RRJN(98)
      IF(RUNI.LT.0.) RUNI=RUNI+1.
      IF(RUNI.LE.0.OR.RUNI.GE.1.) GOTO 130

C...Update counters. Random number to output.
      MRJN(3)=MRJN(3)+1
      IF(MRJN(3).EQ.1000000000) THEN
        MRJN(2)=MRJN(2)+1
        MRJN(3)=0
      ENDIF
      RJN=RUNI

      RETURN

C**** END OF RJN *******************************************************
      END
