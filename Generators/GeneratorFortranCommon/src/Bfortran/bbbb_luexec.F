*CMZ :          02/08/2001  11.41.18  by  Maria Smizanska
*-- Author :    Maria Smizanska   22/07/2000



      subroutine bbbb_luexec(INPAR)
c -----------------------------------------------------------------------
c
c  bbbb  special
c
c if NOFORCED decays:  normal regime, call luexec.
c if FORCED decay and INPAR=1 decay one B+antiB, and stop decay other B-aB  
c if FORCED decay and INPAR=2 then decay the other pair with noforce table
c                Hadronization is reapeated twice.
c
c -----------------------------------------------------------------------


c INPUT PARAMETERS: input  selects two functions of this subroutine

*KEEP,DOUB.
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      INTEGER PYK,PYCHGE,PYCOMP
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




c -----------------------------------------------------------------

       if(ibq.le.1)  return




       if(INPAR.eq.1) then  

C ALL OTHER CASES
11     continue




c
c  First call provides forced decays of one B and anti B
c

c close all B-hadron weak decays (this is what we force)

      mdcy(pycomp(511),1)=0
      mdcy(pycomp(521),1)=0
      mdcy(pycomp(531),1)=0
      mdcy(pycomp(541),1)=0
      mdcy(pycomp(5122),1)=0
      mdcy(pycomp(5132),1)=0
      mdcy(pycomp(5232),1)=0
      mdcy(pycomp(5332),1)=0

c performe event

       call pyexec
       if(idebug.ne.0) print *,'bbbb_luexec: called pylist1'
       if(idebug.ne.0) call pylist(1)

c
c find 4 beauty hadrons and select B and antiB  that will remain stable
c in next pyexec call  while allowing to decay the rest two according to
c forced  decay table defined by user
c

      mdcy(pycomp(511),1)=1
      mdcy(pycomp(521),1)=1
      mdcy(pycomp(531),1)=1
      mdcy(pycomp(541),1)=1
      mdcy(pycomp(5122),1)=1
      mdcy(pycomp(5132),1)=1
      mdcy(pycomp(5232),1)=1
      mdcy(pycomp(5332),1)=1


        i1=rndm(0.)+1.5
	i2=rndm(0.)+1.5
	i1b=0
	i2b=0
	
	
	do ii=1,N
	if(beauty(ii).eq.1) then
	i1b=i1b+1
	        if(i1.eq.i1b) then
	        K(ii,1)=16
	        go to 1
	        endif
	endif
	enddo
1       continue

        do jj=1,N
	if(anti_beauty(jj).eq.1) then
	i2b=i2b+1
	        if(i2.eq.i2b) then
	        K(jj,1)=16
	        go to 2
	        endif
	endif
	enddo
2       continue

        is1=ii
	is2=jj

       if(idebug.ne.0) print *,'bbbb_luexec: called pylist12-0'
       if(idebug.ne.0) call pylist(12)
       if(idebug.ne.0) then
       do ii=1,N
	if(beauty(ii).eq.1.or.anti_beauty(ii).eq.1)
     +  print *,'bbbb_luexec:  B hadron',ii, name_self(ii),K(ii,1)
	enddo
       print *,'bbbb_luexec: particles remain stable',is1,
     +        name_self(is1),K(is1,1),is2,name_self(is2),K(is2,1)
       endif

              call pyexec
       if(idebug.ne.0) print *,'bbbb_luexec: called pylist1-2'
       if(idebug.ne.0) call pylist(1)

       endif ! INPAR=1
       
       if(INPAR.eq.2) then


c -----------------------------------------------------------------
c
c   decays remaining B and anti B with free decay table
c   this is called after finsel selects the event



c
c swap to free decay table
c

12      call SWOP_LUDAT3(1)
       if(idebug.ne.0) print *,'bbbb_luexec: called pylist12-1'
       if(idebug.ne.0) call pylist(12)

c
c     now  decay undecayed B-hadrons,
c     

	do ii=1,N
	if(beauty(ii).eq.1.and.K(ii,1).eq.16) K(ii,1)=1
	if(anti_beauty(ii).eq.1.and.K(ii,1).eq.16) K(ii,1)=1
	enddo

              call pyexec
       if(idebug.ne.0) print *,'bbbb_luexec: called pylist1-3'
       if(idebug.ne.0) call pylist(1)

c
c swap back to user decay table
c

      call SWOP_LUDAT3(2)

       if(idebug.ne.0) print *,'bbbb_luexec: called pylist12-2'
       if(idebug.ne.0) call pylist(12)

        endif  ! INPAR=2

c ------------------------------------------------------------
       return
       end



