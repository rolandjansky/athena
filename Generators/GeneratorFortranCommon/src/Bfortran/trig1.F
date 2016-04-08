*CMZ :          02/08/2001  13.09.46  by  Maria Smizanska
*-- Author :    Maria Smizanska   22/09/96
        SUBROUTINE TRIG1(iret)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : TRIG1                                                         *
*        (module)                                                      *
*                                                                      *
* Description :  Check if event pass LVL1 muon trigger                 *
*                                                                      *
*                                                                      *
* Arguments   :                                                        *
*                                                                      *
*                                                                      *
* Banks/Tables:                                                        *
*        [name]  ([filled | used | modified])                          *
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
C +CDE, TYPING.
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

c local debugging control (swiched off for idebug=0)
           idebug=0

           i_muon_trig1=0

c is LVL1 required?
           if(tr1.eq.0) return
           if(idebug.eq.1) print *,'LVL1:pt_trig1 etha_trig1',
     +      pt_trig1,etha_trig1

c reject event unless accepted
           iret=1

           do I=1,N
           if(name_selv(I).eq.13) then
           PT = SQRT(P(I,1)**2+P(I,2)**2)
           TH = ACOS(P(I,3)/SQRT(PT**2+P(I,3)**2))
           ETA = -LOG(MAX(.0001,ABS(TAN(.5*TH))))

        if(PT.gt.pt_trig1.and.abs(ETA).lt.etha_trig1) then
             iret=0.
             i_muon_trig1=I
             return
           endif
           endif
           enddo


          return
*
*
      END          ! TRIG1
