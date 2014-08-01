*CMZ :          02/08/2001  11.45.06  by  Maria Smizanska
*-- Author :    Nick Ellis   13/07/95

      SUBROUTINE FINSEL(IRET)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : FINSEL                                                        *
*        (module)                                                      *
*                                                                      *
* Description :  Accepting/rejecting event after hadronization is made *
*                in  5 steps:                                          *
*                0.  reject event if undecayed quarks found
*                1.  LVL1CUTS                                          *
*                2.  LVL2CUTS                                          *
*                3.  OFFLCUTS                                          *
*                4.  'user_finsel'                                     *
*                                                                      *
*                User wishing completely control accepting/rejecting   *
*                by himself can switch off                             *
*                steps LVL1CUTS, LVL2CUTS and OFFLCUTS                 *
*                by defining  datacards                                *
*       LVL1CUTS 1=0    (default is 1=1)
*       LVL2CUTS 1=0    (default is 1=0)
*       OFFLCUTS 1=0    (default is 1=0 for inclusive muon events
*                                   1=1 for inclusive forced decays)
*
*                Subroutine user_finsel is called as empty routine     *
*                can be filled by user.                                *
*                                                                      *
*                                                                      *
*      fill_ntup fills    NTUPLE with trigger muon and signal tree     *
*      number of events written into NTUPLE is controled by            *
*          user datacard NT5000                                        *
*                                                                      *
* Arguments   :                                                        *
*         ITB (in) # of hadronisations before this event               *
*         IRET (out)  flag for keep/reject event                       *
* Banks/Tables:                                                        *
*        [name]  ([filled | used | modified])                          *
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Author : Nick Ellis                         Date : 13/07/95          *
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
*KEEP,PRIVEVN.
* numbering of accepted b-events
       common/bevent/ieve,keve
*       SAVE/ieve/
       SAVE/bevent/

*KEND.
c      SAVE IFIRST,IIPRNT
      SAVE IFIRST
      DATA IFIRST /1/
c      DATA IIPRNT /0/
C      DATA IEVE   /0/
*KEEP,BTREE.
      common/btree/ntree,tree(4000)
      integer tree
      SAVE /btree/
*KEEP,HEAVYFLAV.
* store info about  multiple heavy flavour  produced in event
       common/multif/ibq,icq     
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
c  debug control for this subroutine (non active for idebug=0)
      idebug=0

*...DEFINE NUMBER OF SELECTED EVENTS TO PRINT OUT
      NNPRNT = 1

*...INITIALIZE
      IF(IFIRST.EQ.1) THEN
c        call pylist(12)


        IFIRST =0
      ENDIF



c
c accept  event unless explicitely vetoed
c
      iret=0

c
c decide if it is worth keeping event
c

c -------------------------------------------------
c  Check for undecayed quarks and reject event if any
c ------------------------------------------------------
      DO 1700 I=1,N
      if(name_selv(I).ge.1.and.name_selv(I).le.6
     +      .and.K(I,1).eq.1) iret=1
        
      if(iret.eq.1) print*,'FINSEL: undecayed quark event rejected'
         if(iret.eq.1) return
 1700  continue     
c ------------------------------------------------------------------
c 1. check LVL1 with any muon in event
c ------------------------------------------------------------------
        call trig1(iret)
        if(iret.eq.1) return
      if(idebug.ne.0) print*,'FINSEL: trig1 passed'

c ----------------------------------------------------------------------
c 2. quasi LVL2 finds second muon or tagging electron
c    normally  subroutine trig2 is inactive
c    only if demanded explicitly by user - LVL2CUTS datacard
c ----------------------------------------------------------------------
        call trig2(iret)
        if(iret.eq.1) return
      if(idebug.ne.0) print*,'FINSEL: trig2 passed'

c ----------------------------------------------------------------------
c 3. for forced channels finds B-decay tree and check
c    if whole tree is inside cuts defined by OFFLCUTS
c ----------------------------------------------------------------------
      call off_line(iret)
        if(iret.eq.1) return
      if(idebug.ne.0) print*,'FINSEL: off_line passed'

c ----------------------------------------------------------------------
c 4. users routine of additional demands on this event
c ----------------------------------------------------------------------

       call user_finsel(iret)
        if(iret.eq.1) return
      if(idebug.ne.0) print*,'FINSEL: user_finsel passed'

       if(iret.ne.0) return

       ieve=ieve+1
       if(idebug.ne.0)
     +     print *,'FINSEL: eve',ieve,' accepted with ',N,' particles'

c ----------------------------------------------------------------------
c 5. if bbbb call again pyexec and decay undecaied events
c ----------------------------------------------------------------------
       if(iret.eq.0.and.ibq.gt.1) call bbbb_luexec(2)


       
c ----------------------------------------------------------------------
c  6. Store LVL1 muon and signal chain in NTUPLE
c     number of events to be stored is controled by user datacard NT5000
c ----------------------------------------------------------------------
        call fill_ntup(0)

c         call hfill(1,float(N),1.,1.)
c...Print  NNPRNT events that  passed cuts
c        IF(IIPRNT.LT.NNPRNT) THEN
c        print *,'FINSEL: eve',ieve,' accepted with ',N,' particles'
c          CALL PYLIST(1)
c          IIPRNT = IIPRNT+1
c        ENDIF

      RETURN

*
      END          ! FINSEL
