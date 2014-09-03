CDECK  ID>, HWHSSQ.
*CMZ :-        -18/05/99  20.33.45  by  Kosuke Odagiri
*-- Author :    Kosuke Odagiri
C-----------------------------------------------------------------------
      SUBROUTINE HWHSSQ
C-----------------------------------------------------------------------
C     SUSY HARD 2 PARTON -> 2 SPARTON PROCESSES
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      DOUBLE PRECISION HWRGEN, HWUALF, EPS, HCS, RCS, DIST, NC, NC2,
     & NC2C, ML2(6), ML4(6), MR2(6), MR4(6), MG2, SM, DM, QPE,
     & SQPE, FACTR, AFAC, AF, BONE, CFAC, CFC2, CFC3, CONE,
     & CONN, CONT, CONU, CONL, CONR, DFAC, DONE, PF, S,
     & S2, TT, TT2, TMG, TMG2, UU, UU2, UMG, UMG2,
     & L, L2, TTML, UUML, R, R2, TTMR, UUMR, SN2TH
      DOUBLE PRECISION
     & AUSTLL(6),   AUSTRR(6),
     & ASTULL(6,6), ASTURR(6,6), ASTULR(6,6), ASTURL(6,6),
     & AUTSLL(6,6), AUTSRR(6,6), AUTSLR(6,6), AUTSRL(6,6),
     & BSTULL(6),   BSTURR(6),   BSTULR(6),   BSTURL(6),
     & BSUTLL(6),   BSUTRR(6),   BSUTLR(6),   BSUTRL(6),
     & BUTSLL(6),   BUTSRR(6),   BUTSLR(6),   BUTSRL(6),
     & BUSTLL(6),   BUSTRR(6),   BUSTLR(6),   BUSTRL(6),
     & CSTU(6), CSUT(6), CSTUL(6), CSTUR(6), CSUTL(6), CSUTR(6),
     & CTSUL(6), CTSUR(6), CTUSL(6), CTUSR(6), DUTS, DTSU, DSTU
      INTEGER IQ, IQ1, IQ2, ID1, ID2, IGL, SSL, SSR, GLU
      EXTERNAL HWRGEN, HWUALF
      SAVE HCS, AUSTLL, AUSTRR, ASTULL, ASTURR, ASTULR, ASTURL,
     & AUTSLL, AUTSRR, AUTSLR, AUTSRL, BSTULL, BSTURR, BSTULR,
     & BSTURL, BSUTLL, BSUTRR, BSUTLR, BSUTRL, BUTSLL, BUTSRR, BUTSLR,
     & BUTSRL, BUSTLL, BUSTRR, BUSTLR, BUSTRL, CSTU, CSUT, CSTUL, CSTUR,
     & CSUTL, CSUTR, CTSUL, CTSUR, CTUSL, CTUSR, DUTS, DTSU, DSTU
      PARAMETER (EPS = 1.D-9, IGL = 49, SSL = 400, SSR = 412, GLU = 449)
      CALL    HWSGEN(.FALSE.)
      IF (GENEV) THEN
        RCS = HCS*HWRGEN(0)
      ELSE
        SN2TH = 0.25D0 - 0.25D0*COSTH**2
        S     = XX(1)*XX(2)*PHEP(5,3)**2
        FACTR = FACTSS*HWUALF(1,EMSCA)**2
        NC    = CAFAC
        NC2   = NC**2
        NC2C  = ONE - ONE/NC2
        AFAC  = FACTR*NC2C/FOUR
        CFAC  = FACTR*CFFAC/FOUR
        CFC2  = FACTR/CFFAC/FOUR
        CFC3  = FACTR/FOUR
        DFAC  = FACTR/NC2C
        S2    = S**2
        MG2   = RMASS(GLU)**2
        DO 10 IQ = 1, 6
          IQ1     = SSL + IQ
          IQ2     = SSR + IQ
          ML2(IQ) = RMASS(IQ1)**2
          ML4(IQ) = ML2(IQ)**2
          MR2(IQ) = RMASS(IQ2)**2
          MR4(IQ) = MR2(IQ)**2
   10   CONTINUE
c     gluino pair production
        QPE  = S - FOUR*MG2
        IF (QPE.GE.ZERO) THEN
          SQPE = SQRT(S*QPE)
          PF   = SQPE/S
          TT   = (SQPE*COSTH - S) / TWO
          TT2  = TT**2
          UU   = - S - TT
          UU2  = UU**2
c            ~ ~
c     g g -> g g
c
          DONE =
     &     DFAC*PF/TWO*(UU2+TT2+FOUR*MG2*S*SQPE**2*SN2TH/TT/UU)/S2/TT/UU
          DUTS = DONE*UU2
          DTSU = DONE*TT2
          DSTU = DONE*S2
c       _    ~ ~
c     q q -> g g
c
          DO 21 IQ = 1, 6
            L    = ML2(IQ)-MG2
            L2   = L**2
            TTML = TT-L
            UUML = UU-L
            R    = MR2(IQ)-MG2
            R2   = R**2
            TTMR = TT-R
            UUMR = UU-R
            CONE = TWO*PF**2*SN2TH
            CONL = CONE/UUML/TTML
            CONR = CONE/UUMR/TTMR
            CONT = (UU2-L2)*CONL+(UU2-R2)*CONR+L2/TTML**2+R2/TTMR**2
            CONU = (TT2-L2)*CONL+(TT2-R2)*CONR+L2/UUML**2+R2/UUMR**2
            CONN = CFAC*(PF-PF/NC2/(CONT+CONU)*( S2*(CONL+CONR)+
     &            L2*((TT-UU)*CONL/CONE)**2+R2*((TT-UU)*CONR/CONE)**2 ))
            CSTU(IQ) = CONT*CONN
            CSUT(IQ) = CONU*CONN
   21     CONTINUE
        ELSE
          DUTS = ZERO
          DTSU = ZERO
          DSTU = ZERO
          DO 23 IQ = 1, 6
            CSTU(IQ) = ZERO
            CSUT(IQ) = ZERO
   23     CONTINUE
        END IF
c     left handed squark (identical flavour) pair production
        DO 22 IQ = 1, 6
          QPE = S - FOUR*ML2(IQ)
          IF (QPE.GE.ZERO) THEN
            SQPE = SQRT(S*QPE)
            PF   = SQPE/S
            TT   = (SQPE*COSTH - S) / TWO
            TT2  = TT**2
            UU   = - S - TT
            UU2  = UU**2
c            ~ ~*
c     g g -> q q
c             L L
            CONE = CFC2*PF*((SQPE*PF*SN2TH)**2+ML4(IQ))/TT2/UU2
            CONN = CONE-CONE*S2/(TT2+UU2)/NC2
            CSTUL(IQ)  = CONN*UU2
            CSUTL(IQ)  = CONN*TT2
c            ~ ~
c     q q -> q q
c             L L
            TMG  = TT+ML2(IQ)-MG2
            TMG2 = TMG**2
            UMG  = UU+ML2(IQ)-MG2
            UMG2 = UMG**2
            BONE = AFAC*PF*MG2*S*(HALF-TMG*UMG/(TMG2+UMG2)/NC)
            BSTULL(IQ) = BONE/TMG2
            BSUTLL(IQ) = BONE/UMG2
c       _    ~ ~*
c     q q -> q q
c             L L
            AF   = AFAC*PF*PF**2*SN2TH
            BONE = AF/TMG2-AF*S/(HALF*S2+TMG2)/TMG/NC
            BUTSLL(IQ) = BONE*S2
            BUSTLL(IQ) = BONE*TWO*TMG2
c       _     ~ ~*
c     q q  -> q'q'       q =/= q'
c              L L
            AUSTLL(IQ) = TWO*AF
          ELSE
            CSTUL(IQ)  = ZERO
            CSUTL(IQ)  = ZERO
            BSTULL(IQ) = ZERO
            BSUTLL(IQ) = ZERO
            BUTSLL(IQ) = ZERO
            BUSTLL(IQ) = ZERO
            AUSTLL(IQ) = ZERO
          END IF
c     right handed squark (identical flavour) pair production
          QPE = S - FOUR*MR2(IQ)
          IF (QPE.GE.ZERO) THEN
            SQPE = SQRT(S*QPE)
            PF   = SQPE/S
            TT   = (SQPE*COSTH - S) / TWO
            TT2  = TT**2
            UU   = - S - TT
            UU2  = UU**2
c            ~ ~*
c     g g -> q q
c             R R
            CONE = CFC2*PF*((SQPE*PF*SN2TH)**2+MR4(IQ))/TT2/UU2
            CONN = CONE-CONE*S2/(TT2+UU2)/NC2
            CSTUR(IQ) = CONN*UU2
            CSUTR(IQ) = CONN*TT2
c            ~ ~
c     q q -> q q
c             R R
            TMG  = TT+MR2(IQ)-MG2
            TMG2 = TMG**2
            UMG  = UU+MR2(IQ)-MG2
            UMG2 = UMG**2
            BONE = AFAC*PF*MG2*S*(HALF-TMG*UMG/(TMG2+UMG2)/NC)
            BSTURR(IQ) = BONE/TMG2
            BSUTRR(IQ) = BONE/UMG2
c       _    ~ ~*
c     q q -> q q
c             R R
            AF = AFAC*PF*PF**2*SN2TH
            BONE = AF/TMG2-AF*S/(HALF*S2+TMG2)/TMG/NC
            BUTSRR(IQ) = BONE*S2
            BUSTRR(IQ) = BONE*TWO*TMG2
c       _     ~ ~*
c     q q  -> q'q'       q =/= q'
c              R R
            AUSTRR(IQ) = TWO*AF
          ELSE
            CSTUR(IQ)  = ZERO
            CSUTR(IQ)  = ZERO
            BSTURR(IQ) = ZERO
            BSUTRR(IQ) = ZERO
            BUTSRR(IQ) = ZERO
            BUSTRR(IQ) = ZERO
            AUSTRR(IQ) = ZERO
          END IF
c     left and right handed squark (identical flavour) pair production
          IQ1  = SSL + IQ
          IQ2  = SSR + IQ
          SM   = RMASS(IQ1)+RMASS(IQ2)
          QPE  = S - SM**2
          IF (QPE.GE.ZERO) THEN
            DM   = RMASS(IQ1)-RMASS(IQ2)
            SQPE = SQRT( QPE*(S-DM**2) )
            PF   = SQPE/S
            AF   = AFAC*PF
            TT   = (SQPE*COSTH - S - SM*DM) / TWO
            UU   = - S - TT
            TMG  = TT + ML2(IQ) - MG2
            TMG2 = TMG**2
            UMG  = UU + MR2(IQ) - MG2
            UMG2 = UMG**2
c            ~ ~
c     q q -> q q
c             L R
            BONE = AFAC*PF*SQPE**2*SN2TH
            BSTULR(IQ) = BONE/TMG2
            BSUTLR(IQ) = BONE/UMG2
c       _    ~ ~*
c     q q -> q q
c             L R
            BUTSLR(IQ) = AFAC*PF*MG2*S/TMG2
            BUSTLR(IQ) = ZERO
            TT   = (SQPE*COSTH - S + SM*DM) / TWO
            UU   = - S - TT
            TMG  = TT + MR2(IQ) - MG2
            TMG2 = TMG**2
            UMG  = UU + ML2(IQ) - MG2
            UMG2 = UMG**2
c            ~ ~
c     q q -> q q
c             R L
c            BONE = AFAC*PF*SQPE**2*SN2TH
c            BSTURL(IQ) = BONE/TMG2
c            BSUTRL(IQ) = BONE/UMG2
            BSTURL(IQ) = ZERO
            BSUTRL(IQ) = ZERO
c       _    ~ ~*
c     q q -> q q
c             R L
            BUTSRL(IQ) = AFAC*PF*MG2*S/TMG2
            BUSTRL(IQ) = ZERO
          ELSE
            BSTULR(IQ) = ZERO
            BSUTLR(IQ) = ZERO
            BUTSLR(IQ) = ZERO
            BUSTLR(IQ) = ZERO
            BSTURL(IQ) = ZERO
            BSUTRL(IQ) = ZERO
            BUTSRL(IQ) = ZERO
            BUSTRL(IQ) = ZERO
          END IF
   22   CONTINUE
c     distinct flavours - gq, qq'
        DO 11 ID1 = 1, 6
          IQ1  = SSL + ID1
          SM   = RMASS(GLU)+RMASS(IQ1)
          QPE  = S - SM**2
          IF (QPE.GE.ZERO) THEN
            DM   = RMASS(GLU)-RMASS(IQ1)
            SQPE = SQRT( QPE*(S-DM**2) )
            PF   = SQPE/S
            TT   = (SQPE*COSTH - S - SM*DM) / TWO
            TT2  = TT**2
            UU   = - S - TT
            UU2  = UU**2
c            ~ ~
c     g q -> g q
c               L
            CONE = (-UU+TWO*SM*DM*(ONE+MG2/TT+ML2(ID1)/UU))/S/TT/UU
            CONN = CFC3*PF*CONE*(ONE-TT2/(UU2+S2)/NC2)
            CTSUL(ID1) = CONN*UU2
            CTUSL(ID1) = CONN*S2
          ELSE
            CTSUL(ID1) = ZERO
            CTUSL(ID1) = ZERO
          END IF
          IQ2  = SSR + ID1
          SM   = RMASS(GLU)+RMASS(IQ2)
          QPE  = S - SM**2
          IF (QPE.GE.ZERO) THEN
            DM   = RMASS(GLU)-RMASS(IQ2)
            SQPE = SQRT( QPE*(S-DM**2) )
            PF   = SQPE/S
            TT   = (SQPE*COSTH - S - SM*DM) / TWO
            TT2  = TT**2
            UU   = - S - TT
            UU2  = UU**2
c            ~ ~
c     g q -> g q
c               R
            CONE = (-UU+TWO*SM*DM*(ONE+MG2/TT+MR2(ID1)/UU))/S/TT/UU
            CONN = CFC3*PF*CONE*(ONE-TT2/(UU2+S2)/NC2)
            CTSUR(ID1) = CONN*UU2
            CTUSR(ID1) = CONN*S2
          ELSE
            CTSUR(ID1) = ZERO
            CTUSR(ID1) = ZERO
          END IF
          IF(ID1.EQ.6) GOTO 11
          DO 12 ID2 = 1, 6
            IF(ID2.EQ.ID1) GOTO 12
            IQ1  = SSL + ID1
            IQ2  = SSL + ID2
            SM   = RMASS(IQ1)+RMASS(IQ2)
            QPE  = S - SM**2
            IF (QPE.GE.ZERO) THEN
              DM   = RMASS(IQ1)-RMASS(IQ2)
              SQPE = SQRT( QPE*(S-DM**2) )
              PF   = SQPE/S
              TT   = (SQPE*COSTH - S - SM*DM) / TWO
              UU   = - S - TT
              TMG  = TT+ML2(ID1)-MG2
              AF   = AFAC*PF/TMG/TMG
c             ~ ~
c     q q' -> q q'
c              L L
              ASTULL(ID1,ID2) = AF*MG2*S
c       _     ~ ~*
c     q q' -> q q'
c              L L
              AUTSLL(ID1,ID2) = AF*SQPE**2*SN2TH
            ELSE
              ASTULL(ID1,ID2) = ZERO
              AUTSLL(ID1,ID2) = ZERO
            END IF
            IQ1  = SSR + ID1
            IQ2  = SSR + ID2
            SM   = RMASS(IQ1)+RMASS(IQ2)
            QPE  = S - SM**2
            IF (QPE.GE.ZERO) THEN
              DM   = RMASS(IQ1)-RMASS(IQ2)
              SQPE = SQRT( QPE*(S-DM**2) )
              PF   = SQPE/S
              TT   = (SQPE*COSTH - S - SM*DM) / TWO
              UU   = - S - TT
              TMG  = TT+MR2(ID1)-MG2
              AF   = AFAC*PF/TMG/TMG
c             ~ ~
c     q q' -> q q'
c              R R
              ASTURR(ID1,ID2) = AF*MG2*S
c       _     ~ ~*
c     q q' -> q q'
c              R R
              AUTSRR(ID1,ID2) = AF*SQPE**2*SN2TH
            ELSE
              ASTURR(ID1,ID2) = ZERO
              AUTSRR(ID1,ID2) = ZERO
            END IF
            IQ1  = SSL + ID1
            IQ2  = SSR + ID2
            SM   = RMASS(IQ1)+RMASS(IQ2)
            QPE  = S - SM**2
            IF (QPE.GE.ZERO) THEN
              DM   = RMASS(IQ1)-RMASS(IQ2)
              SQPE = SQRT( QPE*(S-DM**2) )
              PF   = SQPE/S
              TT   = (SQPE*COSTH - S - SM*DM) / TWO
              UU   = - S - TT
              TMG  = TT+ML2(ID1)-MG2
              AF   = AFAC*PF/TMG/TMG
c             ~ ~
c     q q' -> q q'
c              L R
              ASTULR(ID1,ID2) = AF*SQPE**2*SN2TH
c       _     ~ ~*
c     q q' -> q q'
c              L R
              AUTSLR(ID1,ID2) = AF*MG2*S
              TT   = (SQPE*COSTH - S + SM*DM) / TWO
              UU   = - S - TT
              TMG    = TT+MR2(ID1)-MG2
              AF   = AFAC*PF/TMG/TMG
c             ~ ~
c     q q' -> q q'
c              R L
              ASTURL(ID2,ID1) = AF*SQPE**2*SN2TH
c       _     ~ ~*
c     q q' -> q q'
c              R L
              AUTSRL(ID2,ID1) = AF*MG2*S
            ELSE
              ASTULR(ID1,ID2) = ZERO
              AUTSLR(ID1,ID2) = ZERO
              ASTURL(ID2,ID1) = ZERO
              AUTSRL(ID2,ID1) = ZERO
            END IF
   12     CONTINUE
   11   CONTINUE
      END IF
      HCS = ZERO
      DO 6 ID1 = 1, 13
      IF (DISF(ID1,1).LT.EPS) GOTO 6
      DO 5 ID2 = 1, 13
      IF (DISF(ID2,2).LT.EPS) GOTO 5
      DIST = DISF(ID1,1)*DISF(ID2,2)
      IF (ID1.LT.7) THEN
       IQ1 = ID1
       IF (ID2.LT.7) THEN
        IQ2 = ID2
        IF (IQ1.NE.IQ2) THEN
c        ~ ~
c qq' -> q q'
         HCS = HCS + ASTULL(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,0,3421,10)
           GOTO 9
         ENDIF
         HCS = HCS + ASTURR(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,2,IQ2,2,3421,10)
           GOTO 9
         ENDIF
         HCS = HCS + ASTULR(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,2,3421,10)
           GOTO 9
         ENDIF
         HCS = HCS + ASTURL(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,2,IQ2,0,3421,10)
           GOTO 9
         ENDIF
        ELSE
c        ~ ~
c qq  -> q q
         HCS = HCS +     BSTULL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,0,3421,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BSTURR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,2,IQ2,2,3421,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BSTULR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,2,3421,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BSTURL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,2,IQ2,0,3421,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BSUTLL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,0,4312,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BSUTRR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,2,IQ2,2,4312,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BSUTLR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,2,4312,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BSUTRL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,2,IQ2,0,4312,10)
           GOTO 9
         ENDIF
        END IF
       ELSEIF (ID2.NE.13) THEN
        IQ2 = ID2-6
        IF (IQ1.NE.IQ2) THEN
c  _     ~ ~*
c qq' -> q q'
         HCS = HCS + AUTSLL(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,1,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS + AUTSRR(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,2,IQ2,3,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS + AUTSLR(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,3,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS + AUTSRL(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,2,IQ2,1,3142,10)
           GOTO 9
         ENDIF
        ELSE
c  _     ~ ~*
c qq  -> q'q'   (q =/= q')
         DO 30 IQ = 1, 6
         IF (IQ .EQ.IQ1) GOTO 30
         HCS = HCS +     AUSTLL(IQ )*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ ,0,IQ ,1,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS +     AUSTRR(IQ )*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ ,2,IQ ,3,2413,10)
           GOTO 9
         ENDIF
  30     CONTINUE
c  _     ~ ~*
c qq  -> q q
         HCS = HCS +     BUTSLL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,1,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BUTSRR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,2,IQ2,3,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BUTSLR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,3,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BUTSRL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,2,IQ2,1,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BUSTLL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,1,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BUSTRR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,2,IQ2,3,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BUSTLR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,3,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BUSTRL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,2,IQ2,1,2413,10)
           GOTO 9
         ENDIF
         IQ  = IGL
c  _     ~ ~
c qq  -> g g
         HCS = HCS +       CSTU(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ ,0,IQ ,0,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS +       CSUT(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ ,0,IQ ,0,2341,10)
           GOTO 9
         ENDIF
        END IF
       ELSE
         IQ2 = IGL
c        ~ ~
c qg  -> q g
         HCS = HCS +      CTSUL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,0,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS +      CTSUR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,2,IQ2,0,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS +      CTUSL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,0,3421,10)
           GOTO 9
         ENDIF
         HCS = HCS +      CTUSR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,2,IQ2,0,3421,10)
           GOTO 9
         ENDIF
       END IF
      ELSEIF (ID1.NE.13) THEN
       IQ1 = ID1 - 6
       IF (ID2.LT.7) THEN
        IQ2 = ID2
        IF (IQ1.NE.IQ2) THEN
c _      ~*~
c qq' -> q q'
         HCS = HCS + AUTSLL(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,1,IQ2,0,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS + AUTSRR(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,3,IQ2,2,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS + AUTSLR(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,1,IQ2,2,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS + AUTSRL(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,3,IQ2,0,2413,10)
           GOTO 9
         ENDIF
        ELSE
c _      ~*~
c qq  -> q'q'   (q =/= q')
         DO 31 IQ = 1, 6
         IF (IQ .EQ.IQ1) GOTO 31
         HCS = HCS +      AUSTLL(IQ)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ ,1,IQ ,0,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS +      AUSTRR(IQ)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ ,3,IQ ,2,3142,10)
           GOTO 9
         ENDIF
   31    CONTINUE
c _      ~*~
c qq  -> q q
         HCS = HCS +     BUTSLL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,1,IQ2,0,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BUTSRR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,3,IQ2,2,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BUTSLR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,1,IQ2,2,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BUTSRL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,3,IQ2,0,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BUSTLL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,1,IQ2,0,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BUSTRR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,3,IQ2,2,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BUSTLR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,1,IQ2,2,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BUSTRL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,3,IQ2,0,3142,10)
           GOTO 9
         ENDIF
c _      ~ ~
c qq  -> g g
         HCS = HCS +       CSTU(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IGL,0,IGL,0,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS +       CSUT(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IGL,0,IGL,0,4123,10)
           GOTO 9
         ENDIF
        END IF
       ELSEIF (ID2.NE.13) THEN
        IQ2 = ID2 - 6
        IF (IQ1.NE.IQ2) THEN
c __     ~*~*
c qq' -> q q'
         HCS = HCS + ASTULL(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,1,IQ2,1,4312,10)
           GOTO 9
         ENDIF
         HCS = HCS + ASTURR(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,3,IQ2,3,4312,10)
           GOTO 9
         ENDIF
         HCS = HCS + ASTULR(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,1,IQ2,3,4312,10)
           GOTO 9
         ENDIF
         HCS = HCS + ASTURL(IQ1,IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,3,IQ2,1,4312,10)
           GOTO 9
         ENDIF
        ELSE
c __     ~*~*
c qq  -> q q
         HCS = HCS +     BSTULL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,1,IQ2,1,4312,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BSTURR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,3,IQ2,3,4312,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BSTULR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,1,IQ2,3,4312,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BSTURL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,3,IQ2,1,4312,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BSUTLL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,1,IQ2,1,3421,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BSUTRR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,3,IQ2,3,3421,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BSUTLR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,1,IQ2,3,3421,10)
           GOTO 9
         ENDIF
         HCS = HCS +     BSUTRL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,3,IQ2,1,3421,10)
           GOTO 9
         ENDIF
        END IF
       ELSE
         IQ2 = IGL
c _      ~*~
c qg  -> q g
         HCS = HCS +      CTSUL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,1,IQ2,0,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS +      CTSUR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,3,IQ2,0,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS +      CTUSL(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,1,IQ2,0,4312,10)
           GOTO 9
         ENDIF
         HCS = HCS +      CTUSR(IQ1)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,3,IQ2,0,4312,10)
           GOTO 9
         ENDIF
       END IF
      ELSE
       IQ1 = IGL
       IF (ID2.LT.7) THEN
         IQ2 = ID2
c        ~ ~
c gq  -> g q
         HCS = HCS +      CTSUL(IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,0,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS +      CTSUR(IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,2,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS +      CTUSL(IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,0,3421,10)
           GOTO 9
         ENDIF
         HCS = HCS +      CTUSR(IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,2,3421,10)
           GOTO 9
         ENDIF
       ELSEIF (ID2.LT.13) THEN
         IQ2 = ID2 - 6
c  _     ~ ~*
c gq  -> g q
         HCS = HCS +      CTSUL(IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,1,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS +      CTSUR(IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,3,3142,10)
           GOTO 9
         ENDIF
         HCS = HCS +      CTUSL(IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,1,4312,10)
           GOTO 9
         ENDIF
         HCS = HCS +      CTUSR(IQ2)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,3,4312,10)
           GOTO 9
         ENDIF
       ELSE
         IQ2 = IGL
c        ~ ~*
c gg  -> q q
         DO 32 IQ = 1, 6
         HCS = HCS +       CSTUL(IQ)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ ,0,IQ ,1,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS +       CSTUR(IQ)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ ,2,IQ ,3,2413,10)
           GOTO 9
         ENDIF
         HCS = HCS +       CSUTL(IQ)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ ,0,IQ ,1,4123,10)
           GOTO 9
         ENDIF
         HCS = HCS +       CSUTR(IQ)*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ ,2,IQ ,3,4123,10)
           GOTO 9
         ENDIF
   32    CONTINUE
c        ~ ~
c gg  -> g g
         HCS = HCS +            DTSU*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,0,2341,10)
           GOTO 9
         ENDIF
         HCS = HCS +            DSTU*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,0,3421,10)
           GOTO 9
         ENDIF
         HCS = HCS +            DUTS*DIST
         IF (GENEV.AND.HCS.GT.RCS) THEN
           CALL HWHSSS(IQ1,0,IQ2,0,2413,10)
           GOTO 9
         ENDIF
       END IF
      END IF
    5 CONTINUE
    6 CONTINUE
      EVWGT = HCS
      RETURN
C---GENERATE EVENT
    9 IDN(1)=ID1
      IDN(2)=ID2
      IDCMF=15
      CALL HWETWO(.TRUE.,.TRUE.)
      IF (AZSPIN) THEN
C Calculate coefficients for constructing spin density matrices
C Set to zero for now
        CALL HWVZRO(7,GCOEF)
      END IF
      END
