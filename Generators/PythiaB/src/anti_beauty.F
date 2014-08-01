*CMZ :          02/08/2001  11.40.22  by  Maria Smizanska
*-- Author :    Maria Smizanska   22/09/96
      FUNCTION anti_beauty(I)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : anti_beauty                                                   *
*        (module)                                                      *
*                                                                      *
* Description :   Is it anti_beauty hadron ? Selects only hadrons with *
*                 b quark (B-, Baryons ...)                            *
* Arguments   :  I particle line # in LUJETS                           *
*                                                                      *
*                                                                      *
* Banks/Tables:                                                        *
*          LUJETS    used                                              *
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
*KEEP,LUJETS.
       COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
       integer anti_beauty
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
      anti_beauty=0

      if(name_self(I).eq.-511.or.name_self(I).eq.-521.or.
     +   name_self(I).eq.-531.or.name_self(I).eq.-541.or.
     +   name_self(I).eq.5122.or.name_self(I).eq.5132.or.
     +   name_self(I).eq.5232.or.name_self(I).eq.5332)
     +anti_beauty=1


      return
*
      END          ! BEAUTY
