*CMZ :          02/08/2001  11.43.38  by  Maria Smizanska
*-- Author :    Maria Smizanska   22/07/2000

      SUBROUTINE copy_ludat3(in)
c
c   make a copy ludat3 of default (in=1) decays
c   make a copy ludat3 of users defined (in=2) decays
c   MDCY(500,3),MDME(2000,2),BRAT(2000),KFDP(2000,5)
c
*KEEP,DOUB.
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      INTEGER PYK,PYCHGE,PYCOMP
*KEEP,COLUDAT3.

       common /CLUDAT3/MDCYbi(500,3),MDMEbi(4000,2),
     *                       BRATbi(4000),KFDPbi(4000,5),
     *                       MDCYun(500,3),MDMEun(4000,2),
     *                       BRATun(4000),KFDPun(4000,5)

*KEEP,LUDAT3.
       COMMON/PYDAT3/MDCY(500,3),MDME(4000,2),BRAT(4000),KFDP(4000,5)
*KEND.
      

      idebug=0
      if(idebug.ne.0) print *,'copy_ludat3: is called'
      if(in.eq.1) then
*      call ucopy(xludat3,xludat3_unbiased,n_ludat3)
      call ucopy(MDCY,MDCYun,1500)
      call ucopy(MDME,MDMEun,8000)
      call ucopy(BRAT,BRATun,1500)      
      call ucopy(KFDP,KFDPun,20000)


      endif

      if(in.eq.2) then
*      call ucopy(xludat3,xludat3_biased,n_ludat3)
      call ucopy(MDCY,MDCYbi,1500)
      call ucopy(MDME,MDMEbi,8000)
      call ucopy(BRAT,BRATbi,1500)      
      call ucopy(KFDP,KFDPbi,20000)      

      endif


      if(idebug.ne.0) then
      print *,'copy_ludat3: unbiased 1 biased 2',in
      print *,'copy_ludat3: mdme(524-559,1)',(mdme(ii,1),ii=524,559)
      endif
      return
      end







