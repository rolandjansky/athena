*CMZ :          02/08/2001  13.11.38  by  Maria Smizanska
*-- Author :    Maria Smizanska   23/09/96

      SUBROUTINE TRIG2(iret)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : TRIG2                                                         *
*        (module)                                                      *
*                                                                      *
* Description : Check LVL2 trigger: second muon or a high pT electron  *
*               if demanded by user. We see  not reasonable to test    *
*               more  than this of LVL2 trigger in ATGEN package.      *
*                                                                      *
*                                                                      *
* Arguments   :                                                        *
*        iret | input/output   |   event rejection flag (=1)           *
*                                                                      *
* Banks/Tables:                                                        *
*        [name]  ([filled | used | modified])                          *
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Author : Maria Smizanska                    Date : 23/09/96          *
*                                                                      *
*                                                                      *
* Last modifications :                                                 *
*        [name]                               Date : [date]            *
*                                                                      *
* Keywords :                                                           *
*        [keywords]                                                    *
*----------------------------------------------------------------------*
*
C+CDE, TYPING.
*KEEP,DOUB.
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      INTEGER PYK,PYCHGE,PYCOMP
*KEEP,LUJETS.
       COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
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
*
c local debugging control
          idebug=0
          if(idebug.ne.0) print *,'LVL2: pt_trig2 etha_trig2',
     +    pt_trig2,etha_trig2

          i_muon_trig2=0
          i_ele_trig2=0

c is LVL2 required by user? - if no - return
          if(tr2.eq.0) return

c reject event unless accepted
          iret=1

c find LVL2  muon or electron according to user choice by 'type'
c type is defined by users datacard
          i_search=type
          do I=1,N
           PT = SQRT(P(I,1)**2+P(I,2)**2)
           TH = ACOS(P(I,3)/SQRT(PT**2+P(I,3)**2))
           ETA = -LOG(MAX(.0001,ABS(TAN(.5*TH))))

           if(name_selv(I).eq.i_search.and.
     +     PT.gt.pt_trig2.and.
     +     abs(ETA).lt.etha_trig2) then
           if(i_search.eq.11) iret=0
c if muon then look if not LVL1 muon
           if(i_search.eq.13.and.I.ne.i_muon_trig1)  iret=0
           if(i_search.eq.13) i_muon_trig2 = I
           if(i_search.eq.11) i_ele_trig2  = I
           if(iret.eq.0) return
           endif
          enddo

          return
*
      END          ! TRIG2
