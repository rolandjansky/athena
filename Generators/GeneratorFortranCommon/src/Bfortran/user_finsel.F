*CMZ :          02/08/2001  11.49.18  by  Maria Smizanska
*-- Author :    Maria Smizanska   06/10/96
      SUBROUTINE USER_FINSEL(IRET)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : USER_FINSEL                                                   *
*        (module)                                                      *
*                                                                      *
* Description : User subroutine to define  requirements for event      *
*                 accepting/rejecting additional or                    *
*                 other that in subroutine finsel                      *
* Arguments   :  IRET (in/out) accepting(0)/rejecting(1) event         *
*                                                                      *
*                                                                      *
* Author : user                         Date : xxx                     *
*                                                                      *
*                                                                      *
* Last modifications :                                                 *
*        [name]                               Date : [date]            *
*                                                                      *
* Keywords :                                                           *
*        [keywords]                                                    *
*----------------------------------------------------------------------*
*
*+CDE, TYPING.
C ----------- end CDE --------------------------------------------------
*
*KEEP,DOUB.
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      INTEGER PYK,PYCHGE,PYCOMP
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



*KEND.
*      SAVE /LUJETS/,/LUDAT1/,/LUDAT2/
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/
*KEEP,BTREE.
      common/btree/ntree,tree(4000)
      SAVE /btree/
*KEND.
           SAVE IFIRST,IIPRNT
      DATA IFIRST /1/
      DATA IIPRNT /0/
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
      IF(IFIRST.EQ.1) THEN
        IFIRST =0
      ENDIF


c
c Following lines is an example for user how to
c select a channel of secondary decay which
c could not be forced by datacards
c          
	  
	  
c	  iret=1
c          if(idebug.eq.1)
c     +          print *,'USER_FINSEL: BEGINNING'
c          do  I=1,N
c          if(tree(I).eq.1) then
cc
cc  accept only phi->K+K-
cc
c          if(idebug.eq.1)
c     +    print *,'USER_FINSEL: tree',name_self(I)

c          if(name_selv(I).eq.321.
c     +          and.name_father(I).eq.333) then
c          if(idebug.eq.1)
c     +          print *,'USER_FINSEL: K+- from eta',
c     +    name_self(I),name_father(I)
c          iret=0
c          endif



c          endif
c          enddo





      return
*
      END          ! USER_FINSEL
