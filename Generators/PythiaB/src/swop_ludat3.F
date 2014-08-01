*CMZ :          02/08/2001  11.47.04  by  Maria Smizanska
*-- Author :    Maria Smizanska   22/07/2000

         subroutine swop_ludat3(in)

c
c   make a copy of default (in=1) decays  to ludat3
c   make a copy of users defined (in=2) decays to ludat3
c   MDCY(500,3),MDME(2000,2),BRAT(2000),KFDP(2000,5)
c  MDCY(500,3),MDME(4000,2),BRAT(4000),KFDP(4000,5)
c
*KEEP,DOUB.
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
*KEEP,COLUDAT3.

       common /CLUDAT3/MDCYbi(500,3),MDMEbi(8000,2),
     *                       BRATbi(8000),KFDPbi(8000,5),
     *                       MDCYun(500,3),MDMEun(8000,2),
     *                       BRATun(8000),KFDPun(8000,5)

*KEEP,LUDAT3.
       COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
*KEND.
      
      

      idebug=0
*      if(idebug.ne.0) print *,'swop_ludat3: is called'
      
      
      if(in.eq.1) then
*      call ucopy(xludat3_unbiased,xludat3,n_ludat3)
      call icopy(MDCYun,MDCY,1500)
      call icopy(MDMEun,MDME,16000)
      call dcopy(BRATun,BRAT,8000)      
      call icopy(KFDPun,KFDP,40000)      

      endif

      if(in.eq.2) then
*      call ucopy(xludat3_biased,xludat3,n_ludat3)
      call icopy(MDCYbi,MDCY,1500)
      call icopy(MDMEbi,MDME,16000)
      call dcopy(BRATbi,BRAT,8000)      
      call icopy(KFDPbi,KFDP,40000)      

      endif


      if(idebug.ne.0) then
      print *,'swop_ludat3: unbiased=1 biased=2 actual=',in
      print *,'swop_ludat3: mdme(982,1)',(mdme(ii,1),ii=982,982)
      endif
      return
      end


      subroutine icopy(ia,ib,N)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      DIMENSION ia(*),ib(*)
      IF (N.EQ.0) RETURN
         DO 21 I=1,N
   21 ib(I)=ia(I)
      return 
      end
      


