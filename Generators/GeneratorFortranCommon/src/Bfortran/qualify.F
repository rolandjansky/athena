*CMZ :          02/08/2001  13.10.36  by  Maria Smizanska
*-- Author :    Maria Smizanska   20/09/96

      real function QUALIFY(i_line)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : QUALIFY                                                       *
*        (module)                                                      *
*                                                                      *
* Description : Check if particle satisfies selection criteria         *                                                       *
*               defined by user                                        *
*                                                                      *
* Arguments   :                                                        *
*                   input: i_line     particle line # in LUJET         *
*                                                                      *
* Banks/Tables:                                                        *
*                   LUJETS     used                                    *
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Author : Maria Smizanska                    Date : 20/09/96          *
*                                                                      *
*                                                                      *
* Last modifications :                                                 *
*        [name]                               Date : [date]            *
*                                                                      *
* Keywords :                                                           *
*        [keywords]                                                    *
*----------------------------------------------------------------------*
*
c +CDE, TYPING.
*KEEP,DOUB.
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      INTEGER PYK,PYCHGE,PYCOMP
*KEEP,LUJETS.
       COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
*KEEP,CUTOFF.
*  cuts on stable particles to select event
      common/cutoff/tr4,pt_pikp,eta_pikp,
     +              pt_muon,eta_muon,
     +              pt_ele,eta_ele
      SAVE/cutoff/
      dimension cut3(7)
      equivalence (cut3(1),tr4)
*KEEP,LVL1.
*  LVL1 muon cuts
      common/lvl1/tr1,pt_trig1, etha_trig1
      common/trg1/i_muon_trig1
      SAVE/lvl1/
      SAVE/trg1/
      dimension cut1(3)
      equivalence (cut1(1),tr1)
*KEEP,LVL2.
*  LVL2 muon cut and electron tag cut
      common/lvl2/tr2,type,pt_trig2, etha_trig2
      common/trg2/i_muon_trig2,i_ele_trig2
      SAVE/lvl2/
      SAVE/trg2/
      dimension cut2(4)
      equivalence (cut2(1),tr2)
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

           idebug=0
           eta_stable=2.5
           eta_max=2.5
           pt_stable=0.5

c           pt_pikp=0.5
c           pt_muon=3.        ! this is third muon in event
c           pt_ele=0.5        ! these are no tag electrons
c           eta_pikp=2.5
c           eta_muon=2.5       ! this is third muon in event
c           eta_ele= 2.5       ! these are no tag electrons


c  accept inless veto
         qualify=1.
c         if(idebug.eq.1) print *,'QUALIFY: was called tr4=',tr4
c accept if no selection cuts required by user

         if(tr4.eq.0)    return

c         if(idebug.eq.1) print *,'QUALIFY: pt_pikp pt_ele pt_muon',
c      +   pt_pikp,pt_ele,pt_muon




c if neutrino or gamma, then accept particle and return

           if(name_selv(i_line).eq.12.or.
     +        name_selv(i_line).eq.14.or.
     +        name_selv(i_line).eq.16.or.
     +        name_selv(i_line).eq.18.or.
     +        name_selv(i_line).eq.22    )     return

c if K_long reject

           if(name_selv(i_line).eq.130) then
           qualify=0.
           return
           endif

c if particle is non stable but the decay was closed by user - rejet

           if(forced(i_line).eq.1)  then

           qualify=0.
           return
           endif

c is this pi0 from K0s? - reject ( we cannot close this channel in PYTHIA,
c but we reject it in signal chain, because it cannot be recovered
c by detector

        if(name_selv(i_line).eq.111.and.
     +        name_father(i_line).eq.310) then
           qualify=0.
           return
           endif



           PT = SQRT(P(i_line,1)**2+P(i_line,2)**2)
           TH = ACOS(P(i_line,3)/SQRT(PT**2+P(i_line,3)**2))
           ETA = -LOG(MAX(.0001,ABS(TAN(.5*TH))))

c if not stable just check etha

        if(children(i_line).ne.0) then
c        if(abs(ETA).gt.eta_max)
c     +   qualify=0.

        else

c if stable then check etha and pt according to particle type

c stable particle other that muon or ele ?
         pt_stable=pt_pikp
         eta_stable=eta_pikp

c is this muon a trigger1 or trigger2 muon? - if yes take it,
c it has been already checked before
c if not apply  cuts
         if(idebug.eq.1.and.name_selv(i_line).eq.13)
     +    print *,'QUALIFY:muon_line i_muon_trig1 i_muon_trig2',
     +   name_selv(i_line),i_line,i_muon_trig1,i_muon_trig2
        if(name_selv(i_line).eq.13.and.
     +     (i_line.eq.i_muon_trig1.or.i_line.eq.i_muon_trig2)) return

        if(name_selv(i_line).eq.13.and.i_line.ne.i_muon_trig1.
     +     and.i_line.ne.i_muon_trig2) then
         pt_stable=pt_muon
         eta_stable=eta_muon

         endif



c is this electron trigger2(tag) electron? -  if yes take it
c it has been already checked before
c if not apply  cuts




        if(name_selv(i_line).eq.11.and.
     +     i_line.eq.i_ele_trig2) return

        if(name_selv(i_line).eq.11.and.i_line.ne.i_ele_trig2) then
          pt_stable=pt_ele
          eta_stable=eta_ele
          endif


c check pt and etha


        if(PT.lt.pt_stable.or.abs(ETA).gt.eta_stable)
     +   qualify=0.
        if(idebug.ne.0.and.
     +   name_selv(i_line).eq.13)
     +   print *,'QUALIFY:pT muon  and threshold',
     +        PT,pt_stable

        endif

        return
*
*
      END          ! QUALIFY
