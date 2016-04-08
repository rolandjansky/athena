*CMZ :          02/08/2001  11.45.28  by  Maria Smizanska
*-- Author :    Maria Smizanska   24/09/96

      real FUNCTION FORCED(I)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : FORCED                                                        *
*
*        Confirm  particles that have
*        been forced not to decay - by user                            *
*        This function is to be used by function QUALIFY               *
* Description :                                                        *
*        [description]                                                 *
*                                                                      *
* Arguments   :                                                        *
*        [name]  ([in | out | in/out])      [description]              *
*                                                                      *
* Banks/Tables:                                                        *
*        [name]  ([filled | used | modified])                          *
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Author : Maria Smizanska                    Date : 24/09/96          *
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
*KEEP,LUJETS.
       COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
*KEND.
        INTEGER ISTAB(15)
         data ISTAB/11,12,13,14,16,18,22,211,321,2212,5*0/
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
*
          idebug=0
          forced=0.
           if(idebug.ne.0) print *,'FORCED: particle tested',
     +          K(I,2),K(I,1)
c not take decaying particles (stable or forced not to decay)
          if(K(I,1).ne.1) return

c look if it is forced or really stable and choose forced
          do ii=1,15
          if(name_selv(I).eq.ISTAB(ii))  return
          enddo
          if(idebug.ne.0) print *,'FORCED: particle recognized',
     +          K(I,2),K(I,1)
          forced=1.


      END          ! FORCED
