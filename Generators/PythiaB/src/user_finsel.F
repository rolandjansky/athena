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
* modifications:                                                       *
* 2006-04-02, W.Walkowiak                                              *
*    added common block ufdecay to tunnel iufdc and ciudcp(10)         *
*    to user_finsel.F;                                                 *
*    decays are now contained in subfiles following                    *
*    the naming convention uf_<decayname>.F                            *
* 2006-04-09, W. Walkowiak                                             *
*    added uf_BsJpsimumuphiKK() -- code by James Catmore               *
* 2006-04-17, W. Walkowiak                                             *
*    added uf_bbmumu() -- code by Sergey Sivoklokov                    *
* 2008-05-09, L. Gladilin                                              *
*    added uf_ChHadr() -- code by Leonid Gladilin                      *
* 2010-08-26, P. Reznicek                                              *
*    added uf_BdJpsimumuK0starKpi() -- code by Pavel Reznicek          *
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

*     user finsel decay selection and decay parameters
       common /ufdecay/ cufdcp(10),iufdc
       save   /ufdecay/

*KEND.
*      SAVE /LUJETS/,/LUDAT1/,/LUDAT2/
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/
*KEEP,BTREE.
      common/btree/ntree,tree(4000)
      integer tree
      SAVE /btree/
*KEND.
c      SAVE IFIRST,IIPRNT
      SAVE IFIRST
      DATA IFIRST /1/
c      DATA IIPRNT /0/

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
c select user defined decay channel
c
c  0: decay channel skeleton
      if ( iufdc .eq. 0 ) call uf_Skeleton(iret)
c
c  1: DsPhiX
      if ( iufdc .eq. 1 ) call uf_DsPhiX(iret) 
c
c  2: LambdabmumuLambda (Pavel R.)
      if ( iufdc .eq. 2 ) call uf_LambdabmumuLambda(iret)
c
c  3: LambdabJpsimumuLambda (Michigan)
      if ( iufdc .eq. 3 ) call uf_LambdabJpsimumuLambda(iret)
c
c  4: BsJpsimumuphiKK (James Catmore)
      if ( iufdc .eq. 4) call uf_BsJpsimumuphiKK(iret) 

c  5: bbmumu (Sergey Sivoklokov)
      if ( iufdc .eq. 5) call uf_bbmumu(iret) 

c  6: Kstarmumu (Sergey Sivoklokov)
      if ( iufdc .eq. 6) call uf_KstarMuMu(iret) 

c  7: Phi0mumu (Sergey Sivoklokov)
      if ( iufdc .eq. 7) call uf_PhiMuMu(iret) 

c  8: Gammamumu (Sergey Sivoklokov)
      if ( iufdc .eq. 8) call uf_GammaMuMu(iret) 

c  9: ChHadr (Leonid Gladilin)
      if ( iufdc .eq. 9) call uf_ChHadr(iret) 

c 10: BsAngles (J Catmore)
      if ( iufdc .eq. 10) call uf_BsAngles(iret)

c 11: BdJpsimumuK0starKpi (Pavel Reznicek)
      if ( iufdc .eq. 11 ) call uf_BdJpsimumuK0starKpi(iret)

c 12: BsAnglesCDF (Alastair Dewhurst, Pavel Reznicek)
      if ( iufdc .eq. 12) call uf_BsAnglesCDF(iret)

c 13: BdAnglesCDF (Alastair Dewhurst, Pavel Reznicek)
      if ( iufdc .eq. 13) call uf_BdAnglesCDF(iret)


      return
*
      END          ! USER_FINSEL
