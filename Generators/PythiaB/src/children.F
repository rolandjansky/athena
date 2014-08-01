*CMZ :          02/08/2001  11.43.16  by  Maria Smizanska
*-- Author :    Maria Smizanska   22/09/96


      FUNCTION CHILDREN(I)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : CHILDREN                                                      *
*        (module)                                                      *
*                                                                      *
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
* Author : Maria Smizanska                    Date : 22/09/96          *
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
*KEEP,LUJETS.
       COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
*KEND.
      integer children

C ----------- end CDE --------------------------------------------------
*
      idebug=0
      children=abs(K(I,5)-K(I,4))+1
      if(K(I,4).eq.0) children=0

         return
*
      END          ! CHILDREN
