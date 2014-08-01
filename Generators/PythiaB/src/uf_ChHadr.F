      SUBROUTINE uf_ChHadr(IRET)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : uf_ChHadr                                                     *
*        (module)                                                      *
*                                                                      *
* Description : User subroutine to define  requirements for event      *
*                 accepting/rejecting additional or                    *
*                 other that in subroutine finsel                      *
*                 for selection of charm hadrons                       *
*                 additional settings from cufdcp(10) array            *
* Arguments   :  IRET (in/out) accepting(0)/rejecting(1) event         *
*                                                                      *
*                                                                      *
* Author : Leonid Gladilin                   Date : 2008-04-07         *
*                                                                      *
*                                                                      *
* Last modifications :                                                 *
*  Leonid Gladilin                           Date : 2008-05-09         *
*
*  Leonid Gladilin                           Date : 2010-11-26         *
*  ranges for selecting background candidates made wider
*                                                                      *
* Keywords :                                                           *
*        beauty, charm, D mesons                                       *
*----------------------------------------------------------------------*
c123456789012345678901234567890123456789012345678901234567890123456789012
*
*+CDE, TYPING.
C ----------- end CDE --------------------------------------------------
*
*KEEP,DOUB.
C...Double precision and integer declarations.
cc      Implicit NONE
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
c      INTEGER PYK,PYCHGE,PYCOMP
*KEEP,LUJETS.
       COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
*KEEP,LUDAT1.
       COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
*KEEP,LUDAT2.
       COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
*KEEP,PYPARS.
       COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
*KEEP,PYINT1.
       COMMON/PYINT1/MINT(400),VINT(400)
*KEEP,PYINT2.
       COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
*KEEP,PYINT5.
       COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
*KEEP,PYUPPR.
       COMMON/PYUPPR/NUP,KUP(20,7),NFUP,IFUP(10,2),PUP(20,5),Q2UP(0:10)

C...HEPEVT commonblock.
      INTEGER NMXHEP,NEVHEP,NHEP,ISTHEP,IDHEP,JMOHEP,JDAHEP
      PARAMETER (NMXHEP=10000)
      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      DOUBLE PRECISION PHEP,VHEP
      SAVE /HEPEVT/

*     user finsel decay selection and decay parameters
       double precision cufdcp
       integer                     iufdc
       common /ufdecay/ cufdcp(10),iufdc
       save   /ufdecay/
*
c 
c Parameters: >0 : minimal pT of charm hadron / <=0 : OFF
c
c    cufdcp( 1) : min pT(D+  -> K-pi+pi+ (+c.c.))
c    cufdcp( 2) : min pT(Ds+ -> K-K+pi+  (+c.c.))
c    cufdcp( 3) : min pT(Lc+ -> K-p+pi+  (+c.c.))
c
c    cufdcp( 4) : min pT(D*+ -> (K-pi+)pi+   (+c.c.))
c    cufdcp( 5) : min pT(D*+ -> (mu nu X)pi+ (+c.c.))
c    cufdcp( 6) : min pT(D*+ -> (e  nu X)pi+ (+c.c.))
c
c    cufdcp( 7) : min pT(D0 -> K-pi+   (+c.c.))
c    cufdcp( 8) : min pT(D0 -> mu nu X (+c.c.))
c    cufdcp( 9) : min pT(D0 -> e  nu X (+c.c.))
c
c    cufdcp(10) : max abs(eta(D)) for all channels
c
*
c
c min/max "K,pi,pi" mass for D+ selection :
c
cg 26.11.2010      double precision  rdcmin / 1.70  /, rdcmax / 2.10  /
      double precision  rdcmin / 1.50  /, rdcmax / 2.30  /
      save              rdcmin          , rdcmax
c
c min/max "K,K,pi" mass for Ds+ selection :
c
cg 26.11.2010      double precision  rdsmin / 1.70  /, rdsmax / 2.15  /
      double precision  rdsmin / 1.50  /, rdsmax / 2.30  /
      save              rdsmin          , rdsmax
c
c max "K,K" masses for Ds+->(Phi)pi selection :
c ( not applied if <=0 )
c
cg 26.11.2010      double precision  rphimx / 1.055 /
      double precision  rphimx / 1.06 /
      save              rphimx
c
c min/max "K,p,pi" mass for L_c+ selection :
c
cg 26.11.2010      double precision  rdlmin / 2.05  /, rdlmax / 2.50  /
      double precision  rdlmin / 2.00  /, rdlmax / 2.60  /
      save              rdlmin          , rdlmax
c
c min "K,pi" mass for D*->D0,pi selection :
c
cg 26.11.2010      double precision  rd0smn / 1.65  /
      double precision  rd0smn / 1.50  /
      save              rd0smn
c
c max delta_M for D*->D0,pi background combinations :
c
      double precision  rdelmx / 0.18  /
      save              rdelmx
c
c min "K,pi" mass for D0 selection :
c
cg 26.11.2010      double precision  rd0min / 1.60  /
      double precision  rd0min / 1.50  /
      save              rd0min
c
c min track momentum :
c
cg 26.11.2010      double precision  pttrmn / 0.45  /
cg 26.11.2010      save              pttrmn
      double precision  pttrmn / 0.45  /, ptpsmn / 0.20 /
      save              pttrmn, ptpsmn
c
c +/- eta for D meson preselection :
c
      double precision  etaabs / 5.d0  /
      save              etaabs
c
c (m_pi+)^2, (m_K+)^2, (m_p)^2 :
c
      double precision   sqmpi  / 0.019479835d0 /
     &               ,   sqmka  / 0.24371698d0  /
     &               ,   sqmpr  / 0.88035435d0  /
      save sqmpi, sqmka, sqmpr
c
c internal variables :
c
      integer j, jj1, jj2, jj3, jj4, jj5, jj6
c     & , idobs(10), numka, numpi, numcha, numneu, numpi0
     & , idobs(10), numka, numcha, numneu, numpi0
     & , in, in1, in2, in3, in4, j15, j16, j25, j26, j35, j36
     & , jka, jp1, jp2, nelec, jelec, nmuon, jmuon, jtr
c
      double precision  pdobs(5), ptdobs, etdobs
     & , pmobs4, pmobs5
c     & , cdobs(10), etamax, ptd, etd, pt, pt2, pm, pm2
     & , cdobs(10), etamax, ptd, etd, pt2, pm, pm2
     & , ct, th, arge, ptmuon, etmuon, ptelec, etelec
     & , pttr, ettr
     & , pm2phi, ephi, ejka, rmphi
     & , epis, eds4, eds5, rmds4, rmds5
c
*
*KEND.
*      SAVE /LUJETS/,/LUDAT1/,/LUDAT2/
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/
*KEEP,BTREE.
      common/btree/ntree,tree(4000)
      integer tree
      SAVE /btree/
*KEND.
c      SAVE IDEBUG, IFIRST, IIPRNT, NENTRY
      SAVE IDEBUG, IFIRST, NENTRY
      Integer IDEBUG /0/
      Integer IFIRST /1/
c      Integer IIPRNT /0/
      integer NENTRY /0/
*
*KEEP,LVL1.
*  LVL1 muon cuts
      common/lvl1/tr1,pt_trig1, etha_trig1
      common/trg1/i_muon_trig1
      SAVE/lvl1/
      SAVE/trg1/
      dimension cut1(3)
      equivalence (cut1(1),tr1)
*KEEP,FAMILY.
       name_self(I)=K(I,2)
       name_selv(I)=abs(K(I,2))
       name_father(I)=K(K(I,3),2)
       name_grandpa(I)=K(K(K(I,3),3),2)
       name_daughter1(I)=K(K(I,4),2)
       name_daughter2(I)=K(K(I,5),2)

       link_self(I)=I
       link_father(I)=K(I,3)
       link_grandpa(I)=K(K(I,3),3)
       link_daughter1(I)=K(I,4)
       link_daughter2(I)=K(I,5)
*KEND.
C ----------- end CDE --------------------------------------------------
*
      IF(IFIRST.EQ.1) THEN
         IFIRST =0
      ENDIF
*
c     return immediately if already rejected
      if ( iret.eq.1 ) return
*
      if ( cufdcp(10) .gt. 0.d0 ) then
        etamax = cufdcp(10)
      else
        etamax = etaabs
      endif
*
c     some counting
c     nintree = 0
*
c     preset
      iret = 1
*
      call pyhepc(1)
*
      if(idebug.eq.1) then
c
         nentry = nentry + 1
         print *
         print *,' *** uf_ChHadr *** , nhep, nentry =',nhep,nentry
         print *
         print *,' cufdcp =',cufdcp
      endif
*
      do  2222  j = 3 ,  nhep
c                                     D meson analysis
c
      if ( iabs(idhep(j)) .ne. 413 .and.  !  D*+/-
     &     iabs(idhep(j)) .ne. 411 .and.  !  D+/-
     &     iabs(idhep(j)) .ne. 421 .and.  !  D0
     &     iabs(idhep(j)) .ne. 431 .and.  !  Ds+/-
     &     iabs(idhep(j)) .ne.4122        !  Lambda_c+/-
     &                             )  goto  2221
*
      call pteta(j,ptd,etd)
*
      if ( abs(etd) .gt. etaabs ) goto 2221
*
      do i = 1, 9
        if ( cufdcp(i) .gt. 0.d0 .and.
     &       ptd .ge. cufdcp(i) ) goto 11
      enddo
      goto  2221
 11   continue
*
      jj1 = jdahep(1,j)
      jj2 = jdahep(2,j)
c
      if ( ( cufdcp(1) .gt. 0.d0 .or. cufdcp(2) .gt. 0.d0
     &                           .or. cufdcp(3) .gt. 0.d0 )
     & .and.
     &     ( iabs(idhep(j)) .eq. 411 .or.        !  D+/-
     &       iabs(idhep(j)) .eq. 431 .or.        !  Ds+/-
     &       iabs(idhep(j)) .eq.4122    ) ) then !  Lambda_c+/-
c
      numka  = 0
      numcha = 0
      numneu = 0
      numpi0 = 0
c
      pdobs(1) = 0.d0
      pdobs(2) = 0.d0
      pdobs(3) = 0.d0
c
      do in1 = jj1, jj2
c
        if ( isthep(in1) .ne. 1 .and.
     &                          iabs(idhep(in1)) .ne. 111 ) then
          j15 = jdahep(1,in1)
          j16 = jdahep(2,in1)
c
          do in2 = j15, j16
c 
            if ( isthep(in2) .ne. 1 .and.
     &                            iabs(idhep(in2)) .ne. 111 ) then
              j25 = jdahep(1,in2)
              j26 = jdahep(2,in2)
c
              do in3 = j25, j26
                if ( isthep(in3) .ne. 1 .and.
     &                              iabs(idhep(in3)) .ne. 111 ) then
                  j35 = jdahep(1,in3)
                  j36 = jdahep(2,in3)
c
                  do in4 = j35, j36
                    if ( isthep(in4) .ne. 1 .and.
     &                                iabs(idhep(in4)) .ne. 111 ) then
                      numcha = 99
                    else
                      if ( iabs(idhep(in4)) .eq.  310 .or.
     &                     iabs(idhep(in4)) .eq. 3122 )    then
                        numka = numka + 1
c
                        pdobs(1) = pdobs(1) + phep(1,in4)
                        pdobs(2) = pdobs(2) + phep(2,in4)
                        pdobs(3) = pdobs(3) + phep(3,in4)
c
                      elseif ( iabs(idhep(in4)) .eq. 211 .or.
     &                         iabs(idhep(in4)) .eq. 321 .or.
     &                         iabs(idhep(in4)) .eq.  11 .or.
     &                         iabs(idhep(in4)) .eq.  13 .or.
     &                         iabs(idhep(in4)) .eq.2212 ) then
                        numcha = numcha + 1
c
                        pdobs(1) = pdobs(1) + phep(1,in4)
                        pdobs(2) = pdobs(2) + phep(2,in4)
                        pdobs(3) = pdobs(3) + phep(3,in4)
c
                        idobs(numcha) = in4
c
                      elseif ( iabs(idhep(in4)) .eq. 111 ) then
                        numpi0 = numpi0 + 1
                      elseif ( iabs(idhep(in4)) .eq. 12 .or.
     &                         iabs(idhep(in4)) .eq. 14  ) then
                        numneu = numneu + 1
                      else
                        numcha = 99
                      endif
                    endif
                  enddo
                else
                  if ( iabs(idhep(in3)) .eq.  310 .or.
     &                 iabs(idhep(in3)) .eq. 3122 )    then
                    numka = numka + 1
c
                    pdobs(1) = pdobs(1) + phep(1,in3)
                    pdobs(2) = pdobs(2) + phep(2,in3)
                    pdobs(3) = pdobs(3) + phep(3,in3)
c
                  elseif ( iabs(idhep(in3)) .eq. 211 .or.
     &                     iabs(idhep(in3)) .eq. 321 .or.
     &                     iabs(idhep(in3)) .eq.  11 .or.
     &                     iabs(idhep(in3)) .eq.  13 .or.
     &                     iabs(idhep(in3)) .eq.2212 ) then
                    numcha = numcha + 1
c
                    pdobs(1) = pdobs(1) + phep(1,in3)
                    pdobs(2) = pdobs(2) + phep(2,in3)
                    pdobs(3) = pdobs(3) + phep(3,in3)
c
                    idobs(numcha) = in3
c
                  elseif ( iabs(idhep(in3)) .eq. 111 ) then
                    numpi0 = numpi0 + 1
                  elseif ( iabs(idhep(in3)) .eq. 12 .or.
     &                     iabs(idhep(in3)) .eq. 14  ) then
                    numneu = numneu + 1
                  else
                    numcha = 99
                  endif
                endif
              enddo
c
            else
              if ( iabs(idhep(in2)) .eq.  310 .or.
     &             iabs(idhep(in2)) .eq. 3122 )    then
                numka = numka + 1
c
                pdobs(1) = pdobs(1) + phep(1,in2)
                pdobs(2) = pdobs(2) + phep(2,in2)
                pdobs(3) = pdobs(3) + phep(3,in2)
c
              elseif ( iabs(idhep(in2)) .eq. 211 .or.
     &                 iabs(idhep(in2)) .eq. 321 .or.
     &                 iabs(idhep(in2)) .eq.  11 .or.
     &                 iabs(idhep(in2)) .eq.  13 .or.
     &                 iabs(idhep(in2)) .eq.2212 ) then
                numcha = numcha + 1
c
                pdobs(1) = pdobs(1) + phep(1,in2)
                pdobs(2) = pdobs(2) + phep(2,in2)
                pdobs(3) = pdobs(3) + phep(3,in2)
c
                idobs(numcha) = in2
c
              elseif ( iabs(idhep(in2)) .eq. 111 ) then
                numpi0 = numpi0 + 1
              elseif ( iabs(idhep(in2)) .eq. 12 .or.
     &                 iabs(idhep(in2)) .eq. 14  ) then
                numneu = numneu + 1
              else
                numcha = 99
              endif
            endif
c
          enddo
        else
          if ( iabs(idhep(in1)) .eq.  310 .or.
     &         iabs(idhep(in1)) .eq. 3122 )    then
            numka = numka + 1
c
            pdobs(1) = pdobs(1) + phep(1,in1)
            pdobs(2) = pdobs(2) + phep(2,in1)
            pdobs(3) = pdobs(3) + phep(3,in1)
c
          elseif ( iabs(idhep(in1)) .eq. 211 .or.
     &             iabs(idhep(in1)) .eq. 321 .or.
     &             iabs(idhep(in1)) .eq.  11 .or.
     &             iabs(idhep(in1)) .eq.  13 .or.
     &             iabs(idhep(in1)) .eq.2212 ) then
            numcha = numcha + 1
c
            pdobs(1) = pdobs(1) + phep(1,in1)
            pdobs(2) = pdobs(2) + phep(2,in1)
            pdobs(3) = pdobs(3) + phep(3,in1)
c
            idobs(numcha) = in1
c
          elseif ( iabs(idhep(in1)) .eq. 111 ) then
            numpi0 = numpi0 + 1
          elseif ( iabs(idhep(in1)) .eq. 12 .or.
     &             iabs(idhep(in1)) .eq. 14  ) then
            numneu = numneu + 1
          else
            numcha = 99
          endif
        endif
c
      enddo
c
c D+,Ds+,Lc+ ->  3 charged tracks and 0 K0s
c                and 0 or 1 neutral pion
c                and 0 or 1 neutrino
c
      if ( ( numcha .eq. 3 .and. numka  .eq. 0 .and.
     &       numpi0 .le. 1 .and. numneu .le. 1 )
c
cc     & .or.( numcha .eq. 1 .and. numka  .eq. 1 .and.
cc     &       numpi0 .le. 1 .and. numneu .le. 1 )
c
     &   )  then
c
        pt2    = pdobs(1)**2+pdobs(2)**2
        ptdobs = sqrt(pt2)
        pm2 = pt2+pdobs(3)**2
        pm  = sqrt(pm2)
        ct  = 0.d0
        if ( pm .ne. 0.d0 ) ct = pdobs(3) / pm
        th  = acos(ct)
        arge = tan(th*0.5d0)
        if ( arge .le. 0.d0 ) then
          etdobs = 10.d0
          if ( pdobs(3) .lt. 0.d0 ) etdobs = -10.d0
            goto  111
        endif
        etdobs = -log(arge)
 111    continue
*
        if ( abs(etdobs) .le. etamax ) then
c
          do in = 1, 3
c
c do not include triggered lepton:
c
            if ( iabs(idhep(idobs(in))) .eq. 13 .and.
     &    ( cufdcp(5) .gt. 0. .or. cufdcp(8) .gt. 0. ) ) then
c
              jmuon = idobs(in)
              call pteta(jmuon,ptmuon,etmuon)
c
              if  ( ptmuon .ge. pt_trig1 .and.
     &          abs(etmuon) .le. etha_trig1 ) goto 121
            endif
c
            if ( iabs(idhep(idobs(in))) .eq. 11 .and.
     &    ( cufdcp(6) .gt. 0. .or. cufdcp(9) .gt. 0. ) ) then
c
              jelec = idobs(in)
              call pteta(jelec,ptelec,etelec)
c
              if  ( ptelec .ge. pt_trig1 .and.
     &          abs(etelec) .le. etha_trig1 ) goto 121
            endif
c
c do not use background with soft tracks:
c
            if ( numpi0 .gt. 0 .or. numneu .gt. 0 ) then
c
              jtr = idobs(in)
              call pteta(jtr,pttr,ettr)
c
              if  ( pttr .lt. pttrmn .or.
     &          abs(ettr) .gt. etaabs ) goto 121
            endif
c
c assign charge:
c
            if ( idhep(idobs(in)) .eq. 211 .or.
     &           idhep(idobs(in)) .eq. 321 .or.
     &           idhep(idobs(in)) .eq. -11 .or.
     &           idhep(idobs(in)) .eq. -13 .or.
     &           idhep(idobs(in)) .eq.2212 ) then
c
              cdobs(in) =  1.d0
            else
              cdobs(in) = -1.d0
            endif
          enddo
c
          if     ( cdobs(1)*cdobs(2) .lt. 0.d0 .and.
     &             cdobs(1)*cdobs(3) .lt. 0.d0 ) then
            jka = 1
            jp1 = 2
            jp2 = 3
          elseif ( cdobs(2)*cdobs(1) .lt. 0.d0 .and.
     &             cdobs(2)*cdobs(3) .lt. 0.d0 ) then
            jka = 2
            jp1 = 1
            jp2 = 3
          elseif ( cdobs(3)*cdobs(1) .lt. 0.d0 .and.
     &             cdobs(3)*cdobs(2) .lt. 0.d0 ) then
            jka = 3
            jp1 = 1
            jp2 = 2
          endif
c
          if ( cufdcp(1) .gt. 0.d0 .and. ptdobs .ge. cufdcp(1) ) then
c
            pdobs(4) = sqrt( phep(1,idobs(jka))**2
     &                     + phep(2,idobs(jka))**2
     &                     + phep(3,idobs(jka))**2 + sqmka )
     &               + sqrt( phep(1,idobs(jp1))**2
     &                     + phep(2,idobs(jp1))**2
     &                     + phep(3,idobs(jp1))**2 + sqmpi )
     &               + sqrt( phep(1,idobs(jp2))**2
     &                     + phep(2,idobs(jp2))**2
     &                     + phep(3,idobs(jp2))**2 + sqmpi )
c
            pmobs4 = sqrt(pdobs(4)**2-pm2)
c
            if ( pmobs4 .ge. rdcmin .and. pmobs4 .le. rdcmax ) then
c
c D+ -> K-pi+pi+ and backgrounds
c
              iret = 0
c
              if ( idebug .eq. 1 ) then
                print *
                print *,
     &' *** uf_ChHAdr *** : D+ -> K-pi+pi+ and backgrounds'
                print *
                print *,' j, jj1, jj2 =',j,jj1,jj2
                print *,' numcha, numka, numpi0, numneu =',
     &                    numcha, numka, numpi0, numneu
                print *,' ptdobs, etdobs, pmobs4 =',ptdobs,etdobs,pmobs4
                print *
                call pylist(1)
              endif
c
              Return
c
            endif
c
          endif
c
          if ( cufdcp(2) .gt. 0.d0 .and. ptdobs .ge. cufdcp(2) ) then
c
            ejka = sqrt( phep(1,idobs(jka))**2 + phep(2,idobs(jka))**2
     &                 + phep(3,idobs(jka))**2 + sqmka )
c
            pdobs(4) = ejka
     &               + sqrt( phep(1,idobs(jp1))**2
     &                     + phep(2,idobs(jp1))**2
     &                     + phep(3,idobs(jp1))**2 + sqmka )
     &               + sqrt( phep(1,idobs(jp2))**2
     &                     + phep(2,idobs(jp2))**2
     &                     + phep(3,idobs(jp2))**2 + sqmpi )
c
            pmobs4 = sqrt(pdobs(4)**2-pm2)
c
            pdobs(5) = ejka
     &               + sqrt( phep(1,idobs(jp2))**2
     &                     + phep(2,idobs(jp2))**2
     &                     + phep(3,idobs(jp2))**2 + sqmka )
     &               + sqrt( phep(1,idobs(jp1))**2
     &                     + phep(2,idobs(jp1))**2
     &                     + phep(3,idobs(jp1))**2 + sqmpi )
c
            pmobs5 = sqrt(pdobs(5)**2-pm2)
c
            if ( ( pmobs4 .ge. rdsmin .and. pmobs4 .le. rdsmax ) .or.
     &           ( pmobs5 .ge. rdsmin .and. pmobs5 .le. rdsmax ) ) then
c
c Ds+ -> K-K+pi+ and backgrounds
c
              if ( rphimx .gt. 0.d0 ) then
c
                pm2phi = (phep(1,idobs(jka))+phep(1,idobs(jp1)))**2
     &                 + (phep(2,idobs(jka))+phep(2,idobs(jp1)))**2
     &                 + (phep(3,idobs(jka))+phep(3,idobs(jp1)))**2
c
                ephi   = ejka
     &                 + sqrt( phep(1,idobs(jp1))**2
     &                       + phep(2,idobs(jp1))**2
     &                       + phep(3,idobs(jp1))**2 + sqmka )
c
                rmphi  = sqrt( ephi**2 - pm2phi )
c
                if ( pmobs4 .ge. rdsmin .and. pmobs4 .le. rdsmax
     &          .and. rmphi .le. rphimx )  goto 31
c
                pm2phi = (phep(1,idobs(jka))+phep(1,idobs(jp2)))**2
     &                 + (phep(2,idobs(jka))+phep(2,idobs(jp2)))**2
     &                 + (phep(3,idobs(jka))+phep(3,idobs(jp2)))**2
c
                ephi   = ejka
     &                 + sqrt( phep(1,idobs(jp2))**2
     &                       + phep(2,idobs(jp2))**2
     &                       + phep(3,idobs(jp2))**2 + sqmka )
c
                rmphi  = sqrt( ephi**2 - pm2phi )
c
                if ( pmobs5 .ge. rdsmin .and. pmobs5 .le. rdsmax
     &          .and. rmphi .le. rphimx )  goto 31
c
                goto 33
c
              else
                rmphi = rphimx
              endif
c
 31           continue
              iret = 0
c
              if ( idebug .eq. 1 ) then
                print *
                print *,
     &' *** uf_ChHAdr *** : Ds+ -> K-K+pi+ and backgrounds'
                print *
                print *,' j, jj1, jj2 =',j,jj1,jj2
                print *,' numcha, numka, numpi0, numneu =',
     &                    numcha, numka, numpi0, numneu
                print *,' ptdobs, etdobs, pmobs4, pmobs5, rmphi =',
     &                    ptdobs, etdobs, pmobs4, pmobs5, rmphi
                print *
                call pylist(1)
              endif
c
              Return
c
 33         continue
            endif
c
          endif
c
          if ( cufdcp(3) .gt. 0.d0 .and. ptdobs .ge. cufdcp(3) ) then
c
            pdobs(4) = sqrt( phep(1,idobs(jka))**2
     &                     + phep(2,idobs(jka))**2
     &                     + phep(3,idobs(jka))**2 + sqmka )
     &               + sqrt( phep(1,idobs(jp1))**2
     &                     + phep(2,idobs(jp1))**2
     &                     + phep(3,idobs(jp1))**2 + sqmpr )
     &               + sqrt( phep(1,idobs(jp2))**2
     &                     + phep(2,idobs(jp2))**2
     &                     + phep(3,idobs(jp2))**2 + sqmpi )
c
            pmobs4 = sqrt(pdobs(4)**2-pm2)
c
            pdobs(5) = sqrt( phep(1,idobs(jka))**2
     &                     + phep(2,idobs(jka))**2
     &                     + phep(3,idobs(jka))**2 + sqmka )
     &               + sqrt( phep(1,idobs(jp2))**2
     &                     + phep(2,idobs(jp2))**2
     &                     + phep(3,idobs(jp2))**2 + sqmpr )
     &               + sqrt( phep(1,idobs(jp1))**2
     &                     + phep(2,idobs(jp1))**2
     &                     + phep(3,idobs(jp1))**2 + sqmpi )
c
            pmobs5 = sqrt(pdobs(5)**2-pm2)
c
            if ( ( pmobs4 .ge. rdlmin .and. pmobs4 .le. rdlmax ) .or.
     &           ( pmobs5 .ge. rdlmin .and. pmobs5 .le. rdlmax ) ) then
c
c Lc+ -> K-p+pi+ and backgrounds
c
              iret = 0
c
              if ( idebug .eq. 1 ) then
                print *
                print *,
     &' *** uf_ChHAdr *** : Lc+ -> K-p+pi+ and backgrounds'
                print *
                print *,' j, jj1, jj2 =',j,jj1,jj2
                print *,' numcha, numka, numpi0, numneu =',
     &                    numcha, numka, numpi0, numneu
                print *,' ptdobs, etdobs, pmobs4, pmobs5 =',
     &                    ptdobs, etdobs, pmobs4, pmobs5
                print *
                call pylist(1)
              endif
c
              Return
c
            endif
c
          endif
c
 121    continue
c
        endif
*
      endif
*
      elseif ( iabs(idhep(j)) .eq. 413 .and. (   !  D*+/-
     &              cufdcp(4) .gt. 0.d0
     &         .or. cufdcp(5) .gt. 0.d0
     &         .or. cufdcp(6) .gt. 0.d0
     &                                        )  ) then
c
c D*+/- -> D0,pi-s
c
      if ( (jj2-jj1) .eq. 1 .and.
     & (iabs(idhep(jj1)).eq.421 .or. iabs(idhep(jj2)).eq.421) .and.
     & (iabs(idhep(jj1)).eq.211 .or. iabs(idhep(jj2)).eq.211) ) then
c
      if ( iabs(idhep(jj1)).eq.211) then
        jj3 = jj1
        jj4 = jj2
      else
        jj3 = jj2
        jj4 = jj1
      endif
c
      jj5 = jdahep(1,jj4)
      jj6 = jdahep(2,jj4)
c
      numka  = 0
      numcha = 0
      numneu = 0
      numpi0 = 0
c
      pdobs(1) = 0.d0
      pdobs(2) = 0.d0
      pdobs(3) = 0.d0
c
      nelec = 0
      jelec = 0
c
      nmuon = 0
      jmuon = 0
c
      do in1 = jj5, jj6
c
        if ( isthep(in1) .ne. 1 .and.
     &                          iabs(idhep(in1)) .ne. 111 ) then
          j15 = jdahep(1,in1)
          j16 = jdahep(2,in1)
c
          do in2 = j15, j16
c 
            if ( isthep(in2) .ne. 1 .and.
     &                            iabs(idhep(in2)) .ne. 111 ) then
              j25 = jdahep(1,in2)
              j26 = jdahep(2,in2)
c
              do in3 = j25, j26
                if ( isthep(in3) .ne. 1 .and.
     &                              iabs(idhep(in3)) .ne. 111 ) then
                  j35 = jdahep(1,in3)
                  j36 = jdahep(2,in3)
c
                  do in4 = j35, j36
                    if ( isthep(in4) .ne. 1 .and.
     &                                iabs(idhep(in4)) .ne. 111 ) then
                      numcha = 99
                    else
                      if ( iabs(idhep(in4)) .eq.  310 .or.
     &                     iabs(idhep(in4)) .eq. 3122 )    then
                        numka = numka + 1
c
                        pdobs(1) = pdobs(1) + phep(1,in4)
                        pdobs(2) = pdobs(2) + phep(2,in4)
                        pdobs(3) = pdobs(3) + phep(3,in4)
c
                      elseif ( iabs(idhep(in4)) .eq. 211 .or.
     &                         iabs(idhep(in4)) .eq. 321 .or.
     &                         iabs(idhep(in4)) .eq.  11 .or.
     &                         iabs(idhep(in4)) .eq.  13 .or.
     &                         iabs(idhep(in4)) .eq.2212 ) then
                        numcha = numcha + 1
c
                        if ( iabs(idhep(in4)) .eq.  13 ) then
                          jmuon = in4
                          nmuon = nmuon + 1
                        elseif ( iabs(idhep(in4)) .eq.  11 ) then
                          jelec = in4
                          nelec = nelec + 1
                        endif
c
                        pdobs(1) = pdobs(1) + phep(1,in4)
                        pdobs(2) = pdobs(2) + phep(2,in4)
                        pdobs(3) = pdobs(3) + phep(3,in4)
c
                        idobs(numcha) = in4
c
                      elseif ( iabs(idhep(in4)) .eq. 111 ) then
                        numpi0 = numpi0 + 1
                      elseif ( iabs(idhep(in4)) .eq. 12 .or.
     &                         iabs(idhep(in4)) .eq. 14  ) then
                        numneu = numneu + 1
                      else
                        numcha = 99
                      endif
                    endif
                  enddo
c
                else
                  if ( iabs(idhep(in3)) .eq.  310 .or.
     &                 iabs(idhep(in3)) .eq. 3122 )    then
                    numka = numka + 1
c
                    pdobs(1) = pdobs(1) + phep(1,in3)
                    pdobs(2) = pdobs(2) + phep(2,in3)
                    pdobs(3) = pdobs(3) + phep(3,in3)
c
                  elseif ( iabs(idhep(in3)) .eq. 211 .or.
     &                     iabs(idhep(in3)) .eq. 321 .or.
     &                     iabs(idhep(in3)) .eq.  11 .or.
     &                     iabs(idhep(in3)) .eq.  13 .or.
     &                     iabs(idhep(in3)) .eq.2212 ) then
                    numcha = numcha + 1
c
                    if ( iabs(idhep(in3)) .eq.  13 ) then
                      jmuon = in3
                      nmuon = nmuon + 1
                    elseif ( iabs(idhep(in3)) .eq.  11 ) then
                      jelec = in3
                      nelec = nelec + 1
                    endif
c
                    pdobs(1) = pdobs(1) + phep(1,in3)
                    pdobs(2) = pdobs(2) + phep(2,in3)
                    pdobs(3) = pdobs(3) + phep(3,in3)
c
                    idobs(numcha) = in3
c
                  elseif ( iabs(idhep(in3)) .eq. 111 ) then
                    numpi0 = numpi0 + 1
                  elseif ( iabs(idhep(in3)) .eq. 12 .or.
     &                     iabs(idhep(in3)) .eq. 14  ) then
                    numneu = numneu + 1
                  else
                    numcha = 99
                  endif
                endif
              enddo
c
            else
              if ( iabs(idhep(in2)) .eq.  310 .or.
     &             iabs(idhep(in2)) .eq. 3122 )    then
                numka = numka + 1
c
                pdobs(1) = pdobs(1) + phep(1,in2)
                pdobs(2) = pdobs(2) + phep(2,in2)
                pdobs(3) = pdobs(3) + phep(3,in2)
c
              elseif ( iabs(idhep(in2)) .eq. 211 .or.
     &                 iabs(idhep(in2)) .eq. 321 .or.
     &                 iabs(idhep(in2)) .eq.  11 .or.
     &                 iabs(idhep(in2)) .eq.  13 .or.
     &                 iabs(idhep(in2)) .eq.2212 ) then
                numcha = numcha + 1
c
                if ( iabs(idhep(in2)) .eq.  13 ) then
                  jmuon = in2
                  nmuon = nmuon + 1
                elseif ( iabs(idhep(in2)) .eq.  11 ) then
                  jelec = in2
                  nelec = nelec + 1
                endif
c
                pdobs(1) = pdobs(1) + phep(1,in2)
                pdobs(2) = pdobs(2) + phep(2,in2)
                pdobs(3) = pdobs(3) + phep(3,in2)
c
                idobs(numcha) = in2
c
              elseif ( iabs(idhep(in2)) .eq. 111 ) then
                numpi0 = numpi0 + 1
              elseif ( iabs(idhep(in2)) .eq. 12 .or.
     &                 iabs(idhep(in2)) .eq. 14  ) then
                numneu = numneu + 1
              else
                numcha = 99
              endif
            endif
          enddo
c
        else
          if ( iabs(idhep(in1)) .eq.  310 .or.
     &         iabs(idhep(in1)) .eq. 3122 )    then
            numka = numka + 1
c
            pdobs(1) = pdobs(1) + phep(1,in1)
            pdobs(2) = pdobs(2) + phep(2,in1)
            pdobs(3) = pdobs(3) + phep(3,in1)
c
          elseif ( iabs(idhep(in1)) .eq. 211 .or.
     &             iabs(idhep(in1)) .eq. 321 .or.
     &             iabs(idhep(in1)) .eq.  11 .or.
     &             iabs(idhep(in1)) .eq.  13 .or.
     &             iabs(idhep(in1)) .eq.2212 ) then
            numcha = numcha + 1
c
            if ( iabs(idhep(in1)) .eq.  13 ) then
              jmuon = in1
              nmuon = nmuon + 1
            elseif ( iabs(idhep(in1)) .eq.  11 ) then
              jelec = in1
              nelec = nelec + 1
            endif
c
            pdobs(1) = pdobs(1) + phep(1,in1)
            pdobs(2) = pdobs(2) + phep(2,in1)
            pdobs(3) = pdobs(3) + phep(3,in1)
c
            idobs(numcha) = in1
c
          elseif ( iabs(idhep(in1)) .eq. 111 ) then
            numpi0 = numpi0 + 1
          elseif ( iabs(idhep(in1)) .eq. 12 .or.
     &             iabs(idhep(in1)) .eq. 14  ) then
            numneu = numneu + 1
          else
            numcha = 99
          endif
        endif
c
      enddo
*
      if ( cufdcp(4) .gt. 0.d0 .and.
     &   ( numcha .eq. 2 .and. numka  .eq. 0 .and.
     &     numpi0 .le. 1 .and. numneu .le. 1 ) )  then
c
c D*+/- -> D0 pi, D0   -> ( 2 charged tracks and 0 K0s )
c                           and 0 or 1 neutral pion
c                           and 0 or 1 neutrino
c
        pm2 = pdobs(1)**2 + pdobs(2)**2 + pdobs(3)**2
c
        pdobs(4) = sqrt( phep(1,idobs(1))**2 + phep(2,idobs(1))**2
     &                 + phep(3,idobs(1))**2 + sqmka )
     &           + sqrt( phep(1,idobs(2))**2 + phep(2,idobs(2))**2
     &                 + phep(3,idobs(2))**2 + sqmpi )
c
        pmobs4 = sqrt(pdobs(4)**2-pm2)
c
        pdobs(5) = sqrt( phep(1,idobs(2))**2 + phep(2,idobs(2))**2
     &                 + phep(3,idobs(2))**2 + sqmka )
     &           + sqrt( phep(1,idobs(1))**2 + phep(2,idobs(1))**2
     &                 + phep(3,idobs(1))**2 + sqmpi )
c
        pmobs5 = sqrt(pdobs(5)**2-pm2)
c
        if ( pmobs4 .ge. rd0smn .or. pmobs5 .ge. rd0smn ) then
c
          pdobs(1) = pdobs(1) +  phep(1,jj3)
          pdobs(2) = pdobs(2) +  phep(2,jj3)
          pdobs(3) = pdobs(3) +  phep(3,jj3)
c
          pt2    = pdobs(1)**2+pdobs(2)**2
          ptdobs = sqrt(pt2)
          pm2 = pt2+pdobs(3)**2
          pm  = sqrt(pm2)
          ct  = 0.d0
          if ( pm .ne. 0.d0 ) ct = pdobs(3) / pm
          th  = acos(ct)
          arge = tan(th*0.5d0)
          if ( arge .le. 0.d0 ) then
            etdobs = 10.d0
            if ( pdobs(3) .lt. 0.d0 ) etdobs = -10.d0
              goto  222
          endif
          etdobs = -log(arge)
 222      continue
c
          if ( ptdobs .ge. cufdcp(4) .and.
     &         abs(etdobs) .le. etamax ) then
c
            if ( numpi0 .gt. 0 .or. numneu .gt. 0 ) then
c
c do not use background with soft tracks from D0:
c
              do in = 1,2
c
                jtr = idobs(in)
                call pteta(jtr,pttr,ettr)
c
                if  ( pttr .lt. pttrmn .or.
     &            abs(ettr) .gt. etaabs ) goto 231
c
              enddo
c
c also for "slow" pion:
c
              call pteta(jj3,pttr,ettr)
c
              if  ( pttr .lt. ptpsmn .or.
     &          abs(ettr) .gt. etaabs ) goto 231
c
c and limit on delta_M for background:
c
              epis = sqrt(phep(1,jj3)**2 + phep(2,jj3)**2 +
     &                    phep(3,jj3)**2 + sqmpi)
c
              eds4 = pdobs(4) + epis
              rmds4 = sqrt(eds4**2 - pm2)
c
              eds5 = pdobs(5) + epis
              rmds5 = sqrt(eds5**2 - pm2)
c
              if ( ( pmobs4 .lt. rd0smn .or.
     &               (rmds4-pmobs4) .gt. rdelmx ) .and.
     &             ( pmobs5 .lt. rd0smn .or.
     &               (rmds5-pmobs5) .gt. rdelmx ) ) goto 231
c
            endif
c
c D*+/- -> D0 pi, D0 -> 2 charged tracks + 0/1 pi0 + 0/1 nuetrino
c
            iret = 0
c
            if ( idebug .eq. 1 ) then
              print *
              print *,
     &' *** uf_ChHAdr *** : D*+/- -> D0 pi, D0 -> 2 charged tracks'
     &,' + 0/1 pi0 + 0/1 nuetrino '
              print *
              print *,' j, jj1, jj2, jj5, jj6 =',j,jj1,jj2,jj5,jj6
              print *,' numcha, numka, numpi0, numneu =',
     &                  numcha, numka, numpi0, numneu
              print *,' ptdobs, etdobs, pmobs4, pmobs5 =',
     &                  ptdobs, etdobs, pmobs4, pmobs5
              print *
              call pylist(1)
            endif
c
            Return
c
 231        continue
c
          endif
        endif
      endif
*
      if ( nmuon .eq. 1 .and. cufdcp(5) .gt. 0.d0 ) then
c
        call pteta(jmuon,ptmuon,etmuon)
c
        if  ( ptmuon .ge. pt_trig1 .and.
     &    abs(etmuon) .le. etha_trig1 .and.
     &        ptd .ge. cufdcp(5) ) then
c
c D*+/- -> D0 pi, D0 -> muon + X
c
          iret = 0
c
          if ( idebug .eq. 1 ) then
            print *
            print *,
     &' *** uf_ChHAdr *** : D*+/- -> D0 pi, D0 -> muon + X'
            print *
            print *,' j, jj1, jj2, jj5, jj6 =',j,jj1,jj2,jj5,jj6
            print *,' numcha, numka, numpi0, numneu =',
     &                numcha, numka, numpi0, numneu
            print *,' nmuon, jmuon =',nmuon,jmuon
            print *,' ptmuon, etmuon =',ptmuon,etmuon
            print *
            call pylist(1)
          endif
c
          Return
c
        endif
      endif
*
      if ( nelec .eq. 1 .and. cufdcp(6) .gt. 0.d0 ) then
c
        call pteta(jelec,ptelec,etelec)
c
        if  ( ptelec .ge. pt_trig1 .and.
     &    abs(etelec) .le. etha_trig1 .and.
     &        ptd .ge. cufdcp(6) ) then
c
c D*+/- -> D0 pi, D0 -> electron + X
c
          iret = 0
c
          if ( idebug .eq. 1 ) then
            print *
            print *,
     &' *** uf_ChHAdr *** : D*+/- -> D0 pi, D0 -> electron + X'
            print *
            print *,' j, jj1, jj2, jj5, jj6 =',j,jj1,jj2,jj5,jj6
            print *,' numcha, numka, numpi0, numneu =',
     &                numcha, numka, numpi0, numneu
            print *,' nelec, jelec =',nelec,jelec
            print *,' ptelec, etelec =',ptelec,etelec
            print *
            call pylist(1)
          endif
c
          Return
c
        endif
      endif
      endif
*
      elseif ( iabs(idhep(j)) .eq. 421 .and. (   !  D0
     &              cufdcp(7) .gt. 0.d0
     &         .or. cufdcp(8) .gt. 0.d0
     &         .or. cufdcp(9) .gt. 0.d0
     &                                        )  ) then
*
      numka  = 0
      numcha = 0
      numneu = 0
      numpi0 = 0
c
      pdobs(1) = 0.d0
      pdobs(2) = 0.d0
      pdobs(3) = 0.d0
c
      nelec = 0
      jelec = 0
c
      nmuon = 0
      jmuon = 0
c
      do in1 = jj1, jj2
c
        if ( isthep(in1) .ne. 1 .and.
     &                          iabs(idhep(in1)) .ne. 111 ) then
          j15 = jdahep(1,in1)
          j16 = jdahep(2,in1)
c
          do in2 = j15, j16
c 
            if ( isthep(in2) .ne. 1 .and.
     &                            iabs(idhep(in2)) .ne. 111 ) then
              j25 = jdahep(1,in2)
              j26 = jdahep(2,in2)
c
              do in3 = j25, j26
                if ( isthep(in3) .ne. 1 .and.
     &                              iabs(idhep(in3)) .ne. 111 ) then
                  j35 = jdahep(1,in3)
                  j36 = jdahep(2,in3)
c
                  do in4 = j35, j36
                    if ( isthep(in4) .ne. 1 .and.
     &                                iabs(idhep(in4)) .ne. 111 ) then
                      numcha = 99
                    else
                      if ( iabs(idhep(in4)) .eq.  310 .or.
     &                     iabs(idhep(in4)) .eq. 3122 )    then
                        numka = numka + 1
c
                        pdobs(1) = pdobs(1) + phep(1,in4)
                        pdobs(2) = pdobs(2) + phep(2,in4)
                        pdobs(3) = pdobs(3) + phep(3,in4)
c
                      elseif ( iabs(idhep(in4)) .eq. 211 .or.
     &                         iabs(idhep(in4)) .eq. 321 .or.
     &                         iabs(idhep(in4)) .eq.  11 .or.
     &                         iabs(idhep(in4)) .eq.  13 .or.
     &                         iabs(idhep(in4)) .eq.2212 ) then
                        numcha = numcha + 1
c
                        if ( iabs(idhep(in4)) .eq.  13 ) then
                          jmuon = in4
                          nmuon = nmuon + 1
                        elseif ( iabs(idhep(in4)) .eq.  11 ) then
                          jelec = in4
                          nelec = nelec + 1
                        endif
c
                        pdobs(1) = pdobs(1) + phep(1,in4)
                        pdobs(2) = pdobs(2) + phep(2,in4)
                        pdobs(3) = pdobs(3) + phep(3,in4)
c
                        idobs(numcha) = in4
c
                      elseif ( iabs(idhep(in4)) .eq. 111 ) then
                        numpi0 = numpi0 + 1
                      elseif ( iabs(idhep(in4)) .eq. 12 .or.
     &                         iabs(idhep(in4)) .eq. 14  ) then
                        numneu = numneu + 1
                      else
                        numcha = 99
                      endif
                    endif
                  enddo
                else
                  if ( iabs(idhep(in3)) .eq.  310 .or.
     &                 iabs(idhep(in3)) .eq. 3122 )    then
                    numka = numka + 1
c
                    pdobs(1) = pdobs(1) + phep(1,in3)
                    pdobs(2) = pdobs(2) + phep(2,in3)
                    pdobs(3) = pdobs(3) + phep(3,in3)
c
                  elseif ( iabs(idhep(in3)) .eq. 211 .or.
     &                     iabs(idhep(in3)) .eq. 321 .or.
     &                     iabs(idhep(in3)) .eq.  11 .or.
     &                     iabs(idhep(in3)) .eq.  13 .or.
     &                     iabs(idhep(in3)) .eq.2212 ) then
                    numcha = numcha + 1
c
                    if ( iabs(idhep(in3)) .eq.  13 ) then
                      jmuon = in3
                      nmuon = nmuon + 1
                    elseif ( iabs(idhep(in3)) .eq.  11 ) then
                      jelec = in3
                      nelec = nelec + 1
                    endif
c
                    pdobs(1) = pdobs(1) + phep(1,in3)
                    pdobs(2) = pdobs(2) + phep(2,in3)
                    pdobs(3) = pdobs(3) + phep(3,in3)
c
                    idobs(numcha) = in3
c
                  elseif ( iabs(idhep(in3)) .eq. 111 ) then
                    numpi0 = numpi0 + 1
                  elseif ( iabs(idhep(in3)) .eq. 12 .or.
     &                     iabs(idhep(in3)) .eq. 14  ) then
                    numneu = numneu + 1
                  else
                    numcha = 99
                  endif
                endif
              enddo
c
            else
              if ( iabs(idhep(in2)) .eq.  310 .or.
     &             iabs(idhep(in2)) .eq. 3122 )    then
                numka = numka + 1
c
                pdobs(1) = pdobs(1) + phep(1,in2)
                pdobs(2) = pdobs(2) + phep(2,in2)
                pdobs(3) = pdobs(3) + phep(3,in2)
c
              elseif ( iabs(idhep(in2)) .eq. 211 .or.
     &                 iabs(idhep(in2)) .eq. 321 .or.
     &                 iabs(idhep(in2)) .eq.  11 .or.
     &                 iabs(idhep(in2)) .eq.  13 .or.
     &                 iabs(idhep(in2)) .eq.2212 ) then
                numcha = numcha + 1
c
                if ( iabs(idhep(in2)) .eq.  13 ) then
                  jmuon = in2
                  nmuon = nmuon + 1
                elseif ( iabs(idhep(in2)) .eq.  11 ) then
                  jelec = in2
                  nelec = nelec + 1
                endif
c
                pdobs(1) = pdobs(1) + phep(1,in2)
                pdobs(2) = pdobs(2) + phep(2,in2)
                pdobs(3) = pdobs(3) + phep(3,in2)
c
                idobs(numcha) = in2
c
              elseif ( iabs(idhep(in2)) .eq. 111 ) then
                numpi0 = numpi0 + 1
              elseif ( iabs(idhep(in2)) .eq. 12 .or.
     &                 iabs(idhep(in2)) .eq. 14  ) then
                numneu = numneu + 1
              else
                numcha = 99
              endif
            endif
c
          enddo
        else
          if ( iabs(idhep(in1)) .eq.  310 .or.
     &         iabs(idhep(in1)) .eq. 3122 )    then
            numka = numka + 1
c
            pdobs(1) = pdobs(1) + phep(1,in1)
            pdobs(2) = pdobs(2) + phep(2,in1)
            pdobs(3) = pdobs(3) + phep(3,in1)
c
          elseif ( iabs(idhep(in1)) .eq. 211 .or.
     &             iabs(idhep(in1)) .eq. 321 .or.
     &             iabs(idhep(in1)) .eq.  11 .or.
     &             iabs(idhep(in1)) .eq.  13 .or.
     &             iabs(idhep(in1)) .eq.2212 ) then
            numcha = numcha + 1 
c
            if ( iabs(idhep(in1)) .eq.  13 ) then
              jmuon = in1
              nmuon = nmuon + 1
            elseif ( iabs(idhep(in1)) .eq.  11 ) then
              jelec = in1
              nelec = nelec + 1
            endif
c
            pdobs(1) = pdobs(1) + phep(1,in1)
            pdobs(2) = pdobs(2) + phep(2,in1)
            pdobs(3) = pdobs(3) + phep(3,in1)
c
            idobs(numcha) = in1
c
          elseif ( iabs(idhep(in1)) .eq. 111 ) then
            numpi0 = numpi0 + 1
          elseif ( iabs(idhep(in1)) .eq. 12 .or.
     &             iabs(idhep(in1)) .eq. 14  ) then
            numneu = numneu + 1
          else
            numcha = 99
          endif
        endif
c
      enddo
*
      if ( cufdcp(7) .gt. 0.d0   .and.
     &   ( numcha .eq. 2 .and. numka  .eq. 0 .and.
     &     numpi0 .le. 1 .and. numneu .le. 1 )  )  then
c
c D0   -> ( 2 charged tracks and 0 K0s )
c           and 0 or 1 neutral pion
c           and 0 or 1 neutrino
c
        pt2    = pdobs(1)**2+pdobs(2)**2
        ptdobs = sqrt(pt2)
        pm2 = pt2+pdobs(3)**2
        pm  = sqrt(pm2)
        ct  = 0.d0
        if ( pm .ne. 0.d0 ) ct = pdobs(3) / pm
        th  = acos(ct)
        arge = tan(th*0.5d0)
        if ( arge .le. 0.d0 ) then
          etdobs = 10.d0
          if ( pdobs(3) .lt. 0.d0 ) etdobs = -10.d0
            goto  333
        endif
        etdobs = -log(arge)
 333    continue
c
        if ( ptdobs .ge. cufdcp(7) .and.
     &       abs(etdobs) .le. etamax ) then
c
          pdobs(4) = sqrt( phep(1,idobs(1))**2 + phep(2,idobs(1))**2
     &                   + phep(3,idobs(1))**2 + sqmka )
     &             + sqrt( phep(1,idobs(2))**2 + phep(2,idobs(2))**2
     &                   + phep(3,idobs(2))**2 + sqmpi )
c
          pmobs4 = sqrt(pdobs(4)**2-pm2)
c
          pdobs(5) = sqrt( phep(1,idobs(2))**2 + phep(2,idobs(2))**2
     &                   + phep(3,idobs(2))**2 + sqmka )
     &             + sqrt( phep(1,idobs(1))**2 + phep(2,idobs(1))**2
     &                   + phep(3,idobs(1))**2 + sqmpi )
c
          pmobs5 = sqrt(pdobs(5)**2-pm2)
c
          if ( pmobs4 .ge. rd0min .or. pmobs5 .ge. rd0min ) then
c
c D0 -> 2 charged tracks + 0/1 pi0 + 0/1 nuetrino
c
            iret = 0
c
            if ( idebug .eq. 1 ) then
              print *
              print *,
     &' *** uf_ChHAdr *** : D0 -> 2 charged tracks'
     &,' + 0/1 pi0 + 0/1 nuetrino '
              print *
              print *,' j, jj1, jj2 =',j,jj1,jj2
              print *,' numcha, numka, numpi0, numneu =',
     &                  numcha, numka, numpi0, numneu
              print *,' ptdobs, etdobs, pmobs4, pmobs5 =',
     &                  ptdobs, etdobs, pmobs4, pmobs5
              print *
              call pylist(1)
            endif
c
            Return
c
          endif
        endif
      endif
*
      if ( nmuon .eq. 1 .and. cufdcp(8) .gt. 0.d0 ) then
c
        call pteta(jmuon,ptmuon,etmuon)
*
        if  ( ptmuon .ge. pt_trig1 .and.
     &    abs(etmuon) .le. etha_trig1 .and.
     &        ptd .ge. cufdcp(8) ) then
c
c D0 -> muon + X
c
          iret = 0
c
          if ( idebug .eq. 1 ) then
            print *
            print *,
     &' *** uf_ChHAdr *** : D0 -> muon + X'
            print *
            print *,' j, jj1, jj2 =',j,jj1,jj2
            print *,' numcha, numka, numpi0, numneu =',
     &                numcha, numka, numpi0, numneu
            print *,' nmuon, jmuon =',nmuon,jmuon
            print *,' ptmuon, etmuon =',ptmuon,etmuon
            print *
            call pylist(1)
          endif
c
          Return
c
        endif
      endif
*
      if ( nelec .eq. 1 .and. cufdcp(9) .gt. 0.d0 ) then
c
        call pteta(jelec,ptelec,etelec)
*
        if  ( ptelec .ge. pt_trig1 .and.
     &    abs(etelec) .le. etha_trig1 .and.
     &        ptd .ge. cufdcp(9) ) then
c
c D0 -> electron + X
c
          iret = 0
c
          if ( idebug .eq. 1 ) then
            print *
            print *,
     &' *** uf_ChHAdr *** : D0 -> electron + X'
            print *
            print *,' j, jj1, jj2 =',j,jj1,jj2
            print *,' numcha, numka, numpi0, numneu =',
     &                numcha, numka, numpi0, numneu
            print *,' nelec, jelec =',nelec,jelec
            print *,' ptelec, etelec =',ptelec,etelec
            print *
            call pylist(1)
          endif
c
          Return
c
        endif
      endif
*
      endif
*
 2221 continue
 2222 enddo

c     evaluation
      
 1000 continue
c
cc      if(iret.ne.0) then
c     debug output
cc         if(idebug.eq.1)
cc     +        print *, 'uf_ChHadr: event rejected'
cc      endif

      
      return
*     
      END                       ! USER_FINSEL

      subroutine pteta(j,pt,et)
c
      Implicit None
*
C...HEPEVT commonblock.
c
      Integer    NMXHEP
      PARAMETER (NMXHEP=10000)
      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      DOUBLE PRECISION PHEP,VHEP
      Integer NEVHEP,NHEP,ISTHEP,IDHEP,JMOHEP,JDAHEP
      SAVE /HEPEVT/
c
      Double Precision pt, pt2, pm, et , ct, th, arge
c
      Integer j
c
      pt2 = phep(1,j)**2+phep(2,j)**2
      pt   = sqrt(pt2)
      pm   = sqrt(pt2+phep(3,j)**2)
      ct  = 0.d0
      if ( pm .ne. 0.d0 ) ct = phep(3,j) / pm
      th  = acos(ct)
      arge = tan(th*0.5d0)
      if ( arge .gt. 0.d0 ) then
        et = -log(arge)
      else
        if ( phep(3,j) .lt. 0.d0 ) then
          et = -100.d0
        else
          et =  100.d0
        endif
      endif
c
      end
