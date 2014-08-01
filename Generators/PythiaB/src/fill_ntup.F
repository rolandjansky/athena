*CMZ :          02/08/2001  13.11.14  by  Maria Smizanska
*-- Author :    Maria Smizanska   22/09/96

      SUBROUTINE FILL_NTUP(ITB)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : FILL_NTUP                                                     *
*        (module)                                                      *
*                                                                      *
* Description :  store B-decay tree in XNTUP                           *
*                                                                      *
*                                                                      *
* Arguments   :                                                        *
*                    ITB  # of events hadronized to get this one       *
*                                                                      *
* Banks/Tables:                                                        *
*        [name]  ([filled | used | modified])                          *
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Author : Maria Smizanska                    Date : 21/09/96          *
*                                                                      *
*                                                                      *
* Last modifications :                                                 *
*        [name]                               Date : [date]            *
*                                                                      *
* Keywords :                                                           *
*        [keywords]                                                    *
*----------------------------------------------------------------------*
*
C +CDE, TYPING.
*KEEP,DOUB.
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
c      INTEGER PYK,PYCHGE,PYCOMP
*KEEP,NNTUPLE.
*      DECLARATION FOR NTUPLE 5000
       PARAMETER(NDIM = 20)
       COMMON/NNICK/XNTUP(NDIM)
       SAVE/NNICK/

*      DECLARATION FOR NTUPLE 5001
       PARAMETER(NDIM1 = 13)
       COMMON/HARD/XNTUP1(NDIM1)
       SAVE/HARD/

*      DECLARATION FOR NTUPLE 5002
       PARAMETER(NDIM2 = 12)
       COMMON/MUON/XNTUP2(NDIM2)
       SAVE/MUON/
*KEEP,LUJETS.
       COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
*KEEP,PYPARS.
       COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
*KEEP,BTREE.
      common/btree/ntree,tree(4000)
      integer tree
      SAVE /btree/
*KEEP,LVL1.
*  LVL1 muon cuts
      common/lvl1/tr1,pt_trig1, etha_trig1
      common/trg1/i_muon_trig1
      SAVE/lvl1/
      SAVE/trg1/
      dimension cut1(3)
      equivalence (cut1(1),tr1)
*KEEP,CUTOFF.
*  cuts on stable particles to select event
      common/cutoff/tr4,pt_pikp,eta_pikp,
     +              pt_muon,eta_muon,
     +              pt_ele,eta_ele
      SAVE/cutoff/
      dimension cut3(7)
      equivalence (cut3(1),tr4)
*KEEP,NT5000.
*   users choice to fill  NTUPLE 5000 or to call user_fill
       common/nt5000/ntup_5000
       SAVE/nt5000/


*KEEP,PRIVEVN.
* numbering of accepted b-events
       common/bevent/ieve,keve
       SAVE/bevent/

*KEND.
           dimension xntup3(7)
           DATA IFIRST /1/
           SAVE IFIRST
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


          if(IFIRST.eq.1) IFIRST=0


       if(idebug.ne.0) print *,'FILL_NTUP:ieve',ieve

        DO NN=1,NDIM
        XNTUP(NN) = 0
        ENDDO
        DO NN=1,NDIM2
        XNTUP2(NN) = 0
        ENDDO




c -------------------------------------------------------------------
c First finish filling XNTUP1 after PYDOCU was called
c fill ntuple for first ntup_5000 events
c write out XNTUP1 formated output for all events
c -------------------------------------------------------------------

       XNTUP1(1)=float(MSTI(1))
       XNTUP1(2)=PARI(1)
       XNTUP1(3)=ITB
       XNTUP1(4)=PARI(17)
       XNTUP1(5)=PARI(37)

       if (ntup_5000.ge.ieve) then
       if(idebug.eq.1)
     +     print *,'FILL_NTUP: hard process filled'
c       call HFN(5001,XNTUP1)
       WRITE (53,1070) float(ieve),xntup1
       endif
 1070  FORMAT(14f12.5)


c -------------------------------------------------------------------
c   if LVL1-required and satisfied and off-line not required
c   store only muons from this event
c -------------------------------------------------------------------

          if(tr1.ne.0.and.i_muon_trig1.ne.0.and.tr4.eq.0) then

          nmuon=0
          do  I=1,N
          if(name_selv(I).eq.13) then
          nmuon=nmuon+1
          endif
          enddo

          imuon=0
          do  I=1,N

          if(name_selv(I).eq.13) then
          imuon=imuon+1

           xntup2(1)=float(ieve)    ! this accepted ev number
           xntup2(2)=float(ITB)    ! # of events hadronised to get this one
           xntup2(3)=float(nmuon)
           xntup2(4)=float(imuon)

           xntup2(5)=float(name_self(I))
           father=float(name_father(I))
           xntup2(6)=father

           pt=sqrt(P(I,1)**2+P(I,2)**2)
           TH = ACOS(P(I,3)/SQRT(PT**2+P(I,3)**2))
           ETA = -LOG(MAX(.0001,ABS(TAN(.5*TH))))
           phi=atan2(P(I,1),P(I,2))

           xntup2(7)=pt
           xntup2(8)=eta
           xntup2(9)=phi

           xntup2(10)=V(I,1)
           xntup2(11)=V(I,2)
           xntup2(12)=V(I,3)


           xntup3(1)=xntup2(1)
           xntup3(2)=xntup2(3)
           xntup3(3)=xntup2(4)
           xntup3(4)=xntup2(6)
           xntup3(5)=xntup2(7)
           xntup3(6)=xntup2(8)
           xntup3(7)=xntup2(9)

           if (ntup_5000.ge.ieve) then
           if(idebug.eq.1)
     +     print *,'FILL_NTUP: all muons are filled',
     +     K(I,2),K(I,1)
c            call HFN(5002,XNTUP2)
	   WRITE (52,1071) xntup3,float(link_father(I)) 
           endif

           

1071       FORMAT(F10.1,2F5.1,F8.1,3F8.3,F8.1)
           endif
           enddo
           endif

c -------------------------------------------------------------------
c   if LVL1-required and satisfied and off-line is required and satisfied
c    store trigger muon and signal tree
c -------------------------------------------------------------------

c is signal tree inside off-line cuts? if not return
          if(tr4.ne.0) then
          ntree=0
          do  ii=1,N
          if(tree(ii).eq.-1) return
          if(tree(ii).gt.0) ntree=ntree+1
          enddo
          endif

          if(tr1.ne.0.and.tr4.ne.0.and.i_muon_trig1.ne.0.and.
     +       ntree.gt.0) then


           xntup(1)=float(ieve)    ! this accepted ev number
           xntup(2)=float(ITB)    ! # of events hadronised to get this one
           xntup(3)=float(0)
           xntup(4)=float(0)
           xntup(5)=float(i_muon_trig1)


           xntup(6)=float(K(i_muon_trig1,1))
           xntup(7)=float(K(i_muon_trig1,2))
           xntup(8)=float(K(i_muon_trig1,3))
           xntup(9)=float(K(i_muon_trig1,4))
           xntup(10)=float(K(i_muon_trig1,5))

           xntup(11)=P(i_muon_trig1,1)
           xntup(12)=P(i_muon_trig1,2)
           xntup(13)=P(i_muon_trig1,3)
           xntup(14)=P(i_muon_trig1,4)
           xntup(15)=P(i_muon_trig1,5)


           xntup(16)=V(i_muon_trig1,1)
           xntup(17)=V(i_muon_trig1,2)
           xntup(18)=V(i_muon_trig1,3)
           xntup(19)=V(i_muon_trig1,4)
           xntup(20)=V(i_muon_trig1,5)
c          if (ntup_5000.ge.ieve) call HFN(5000,XNTUP)
          if (ntup_5000.ge.ieve) 
     +	  WRITE (50,1072) xntup 
1072       FORMAT(20F14.6)



          itree=0
          do  I=1,N
          if(tree(I).ne.0) then
          itree=itree+1
           xntup(1)=float(ieve)    ! this accepted ev number
           xntup(2)=float(ITB)    ! # of events hadronised to get this one
           xntup(3)=float(ntree)
           xntup(4)=float(itree)
           xntup(5)=float(I)


           xntup(6)=float(K(I,1))
           xntup(7)=float(K(I,2))
           xntup(8)=float(K(I,3))
           xntup(9)=float(K(I,4))
           xntup(10)=float(K(I,5))

           xntup(11)=P(I,1)
           xntup(12)=P(I,2)
           xntup(13)=P(I,3)
           xntup(14)=P(I,4)
           xntup(15)=P(I,5)


           xntup(16)=V(I,1)
           xntup(17)=V(I,2)
           xntup(18)=V(I,3)
           xntup(19)=V(I,4)
           xntup(20)=V(I,5)



           if (ntup_5000.ge.ieve) then
           if(idebug.eq.1)
     +     print *,'FILL_NTUP: fill ntuple for tree ',
     +     K(I,2),K(I,1)
c           call HFN(5000,XNTUP)
	   WRITE (50,1072) xntup
           endif
              endif
              enddo

              endif

              return
*
*
      END          ! FILL_NTUP
