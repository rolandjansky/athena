*CMZ :          02/08/2001  11.41.41  by  Maria Smizanska
*-- Author :    Maria Smizanska   22/07/2000



      subroutine bbcc_luexec
c -----------------------------------------------------------------------
c
c bbcc  special: prevent that directly produced charm hadron
c is not forced to decay as the one from B-hadron (if so)
c example: Bs->Ds->phipi.
c if NOFORCED decays:  normal regime, call luexec.
c if B-FORCED    decay:  first close all charm decays then
c decay  B+antiB, then find charm hadrons coming from forced B-decay
c and decay them, then  decay table and decay rest of charm.
c
c -----------------------------------------------------------------------


c INPUT PARAMETERS: input  selects two functions of this subroutine

*KEEP,DOUB.
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      INTEGER PYCOMP
*KEEP,HEAVYFLAV.
* store info about  multiple heavy flavour  produced in event
       common/multif/ibq,icq
* remeber links of  B and antiB chosen to decay free in bbbb
       common/fixedB/is1,is2

*KEEP,LUDAT3.
       COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
*KEEP,LUJETS.
       COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
*KEEP,BBMOD.
* internal number to recognise bb decay regime
       common/bbmod/IBBMOD
*KEND.
        dimension ic2(4)
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

       idebug=0

       if(idebug.ne.0)
     + print *,'bbcc_luexec: begin ibq, icq',ibq,icq


c -----------------------------------------------------------------


       if(ibq.le.0.or.icq.le.0)  return





C ALL OTHER bbcc CASES when B-was forced


 


c
c  First call provides forced decays of B and anti B
c

c close all C-hadrons - weak decays

      mdcy(pycomp(411),1)=0
      mdcy(pycomp(421),1)=0
      mdcy(pycomp(431),1)=0
      mdcy(pycomp(4122),1)=0
      mdcy(pycomp(4132),1)=0
      mdcy(pycomp(4232),1)=0
      mdcy(pycomp(4332),1)=0

c performe event

       call pyexec
       if(idebug.ne.0) print *,'bbcc_luexec: called pylist1'
       if(idebug.ne.0) call pylist(1)

c
c find 3 charm hadrons: 2 not coming from B-decays
c and one coming from B- (etc.) hadrons where we do not require forcing
c

      mdcy(pycomp(411),1)=1
      mdcy(pycomp(421),1)=1
      mdcy(pycomp(431),1)=1
      mdcy(pycomp(4122),1)=1
      mdcy(pycomp(4132),1)=1
      mdcy(pycomp(4232),1)=1
      mdcy(pycomp(4332),1)=1

	ic1=0
	call vzero(ic2,4)
	
	
	do 100 ii=1,N
	if(charm(ii).eq.0.and.anti_charm(ii).eq.0) go to 100
        if(idebug.ne.0)	print *,'bbcc_luexec:  C-hadron',
     + 	ii,name_self(ii),name_father(ii),name_grandpa(ii)
	iii=ii
	kk=0
11      continue	
	jj=link_father(iii)
	if(beauty(jj).eq.1) go to 100
	iii=jj
	kk=kk+1
	if(kk.gt.100) then
	print *,'bbcc_luexec: trapped in loop, forced to finish'
	go to 100
	endif
	if(name_selv(jj).ge.400) go to 11
	K(ii,1)=16
        ic1=ic1 + 1
	if(ic1.le.4) ic2(ic1)=ii
100      continue



       if(idebug.ne.0) then
       print *,'bbcc_luexec: direct C-hadrons'
       print *,'bbcc_luexec: links',ic2
       print *,'bbcc_luexec: names',
     + name_self(ic2(1)),name_father(ic2(1)),name_grandpa(ic2(1)),
     + name_self(ic2(2)),name_father(ic2(2)),name_grandpa(ic2(2)),
     + name_self(ic2(3)),name_father(ic2(2)),name_grandpa(ic2(3))
       endif

              call pyexec
       if(idebug.ne.0) print *,'bbcc_luexec: called pylist1-2'
       if(idebug.ne.0) call pylist(1)




c -----------------------------------------------------------------
c
c   decays remaining C and anti C with free decay table
c  but before close the decay of B-hadron(B+, etc.) that was not required
c  by user to decay


c
c swap to free decay table
c

12      call SWOP_LUDAT3(1)


c
c     now  decay undecayed C-hadrons
c
	do ii=1,N
	if(charm(ii).eq.1.and.K(ii,1).eq.16) K(ii,1)=1
	if(anti_charm(ii).eq.1.and.K(ii,1).eq.16) K(ii,1)=1
	if(beauty(ii).eq.1.and.K(ii,1).eq.1) K(ii,1)=16	
	enddo

              call pyexec
       if(idebug.ne.0) print *,'bbcc_luexec: called pylist1-3'
       if(idebug.ne.0) call pylist(1)

c
c swap back to user decay table
c

      call SWOP_LUDAT3(2)





c ------------------------------------------------------------
       return
       end



