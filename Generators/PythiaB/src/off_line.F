*CMZ :          02/08/2001  11.46.16  by  Maria Smizanska
*-- Author :    Maria Smizanska   07/10/96


      SUBROUTINE OFF_LINE(IRET)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : OFF_LINE                                                      *
*        (module)                                                      *
*                                                                      *
* Description :  Accepting/rejecting event:                            *
*                a) find the whole tree of B-decay                     *
*                b) check if tree is inside cuts defined by            *
*                   datacard OFFLCUTS                                  *
*                This subroutine is skipped if user demanded           *
*                 inclusive muon events  pp->bbX->muX                  *
*                                                                      *
*                User can skip this subroutine by                      *
*                by defining datacard OFFLCUTS 1=0                     *
*                                                                      *
*                                                                      *
*                                                                      *
* Arguments   :                                                        *
*                                                                      *
*         IRET (out)  flag for keep/reject event                       *
* Banks/Tables:                                                        *
*        [name]  ([filled | used | modified])                          *
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Author : Maria Smizanska            Date 07/10/96:                   *
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



*KEND.
*      SAVE /LUJETS/,/LUDAT1/,/LUDAT2/
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/
*KEEP,HEAVYFLAV.
* store info about  multiple heavy flavour  produced in event
       common/multif/ibq,icq
* remeber links of  B and antiB chosen to decay free in bbbb
       common/fixedB/is1,is2

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
*KEEP,CUTOFF.
*  cuts on stable particles to select event
      common/cutoff/tr4,pt_pikp,eta_pikp,
     +              pt_muon,eta_muon,
     +              pt_ele,eta_ele
      SAVE/cutoff/
      dimension cut3(7)
      equivalence (cut3(1),tr4)
*KEEP,BTREE.
      common/btree/ntree,tree(4000)
      integer tree
      SAVE /btree/
*KEEP,NT5000.
*   users choice to fill  NTUPLE 5000 or to call user_fill
       common/nt5000/ntup_5000
       SAVE/nt5000/


*KEEP,FLAVOUR.
*  variable flav is set automatically  to 5 (4)
*  if user demanded
*  bb  (cc) production by selecting corresponding
*  keywords in atgen.proto

       common/flavour/flav

*KEND.

c           SAVE IFIRST
c           DATA IFIRST /1/

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
       integer beauty
       integer charm

C ----------- end CDE --------------------------------------------------


c local debugging control (inactive for idebug=0)
      idebug=0

c ----------------------------------------------------------------------
c if no exclusive B-decay forced channel was demanded
c or if user explicitely defined OFFLCUTS 1=0
c skip this subroutine
c ----------------------------------------------------------------------
      if(tr4.eq.0) go to 1701

c ----------------------------------------------------------------------
c reject event unless accepted explicitly
c ----------------------------------------------------------------------
      iret=1

c ----------------------------------------------------------------------
c  find B-decay tree and check
c  if whole tree is  inside cuts defined by OFFLCUTS datacard
c ----------------------------------------------------------------------


      DO 1700 I=1,N

c Is this B-hadron with anti-b quark (B+, antiLambda_b ...) ?
c 15.8.2000: in case of bbbb only one of two B-hadrons (with anti-b quark)
c in event will be checked with  OFFLINE cuts. It will be the one
c that was forced to decay to signal channel
c (see subroutine bbbb_luexec).   All other 3 b-hadrons
c in event are checked for LVL1 and LVL2 cuts if user activates
c these cuts.

      if(flav.eq.5.and.beauty(I).eq.0) go to 1700
      if((flav.eq.4.or.flav.eq.44).and.charm(I).eq.0) go to 1700

c Is there bbbb? Then finish if this is not the B-hadron
c (with anti-b quark, that is B+ etc...)
c that was  selected in bbbb_luexec (with link is1)
c NOT to be forced to decay to signal channel

      if(ibq.gt.1.and.I.eq.is1) go to 1700


c is this secondary B-hadron? If yes, skip it - it was
c already included in a decay chain of its B-hadron parent
      if(beauty(link_father(I)).eq.1) go to 1700
      if((flav.eq.4.or.flav.eq.44).and.charm(link_father(I)).eq.1)
     +       go to 1700





c      call VZERO(tree,4000)
      do ii=1,4000
        tree(ii)=0
      enddo
      if(idebug.ne.0) print*,'OFF_LINE:beauty',I,name_self(I)

c find decay tree using recursive call of find_children
      call find_children(I)

c if any particle in tree out of cuts reject this tree
      ktree=0
c      do ii=1,4000
c Modification by B. Kersevan, J. Catmore 13/03/2006 
      do ii=1,N 
      if(tree(ii).eq.1) ktree=ktree+1
      if(tree(ii).eq.-1) then ! reject if sec.part. out of cuts
      go to 1700
      endif
      enddo
      if(idebug.ne.0) print*,'OFF_LINE: ktree',ktree
      if(ktree.le.2) go to 1700 ! reject if <3 particles in tree
      iret=0
      return
 1700  CONTINUE
 1701  CONTINUE


      RETURN

*
      END          ! OFF_LINE



