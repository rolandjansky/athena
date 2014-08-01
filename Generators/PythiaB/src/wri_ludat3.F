*CMZ :          02/08/2001  11.43.38  by  Maria Smizanska
*-- Author :    Maria Smizanska   22/07/2000

      SUBROUTINE wri_ludat3(in)
c
c   make a copy ludat3 of default (in=1) decays
c   make a copy ludat3 of users defined (in=2) decays
c   MDCY(500,3),MDME(2000,2),BRAT(2000),KFDP(2000,5)
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
      

      idebug=1
      if(idebug.ne.0) print *,'wri_ludat3: is called'
      
      if(in.eq.1) then
      write (60) MDCY,MDME,BRAT,KFDP 
      write (61,100) BRAT
      endif
100   format(10F10.7)      
      
      
      if(in.eq.2) then
      read (60) MDCYun,MDMEun,BRATun,KFDPun 
      endif
      


      if(idebug.ne.0) then
      print *,'wri_ludat3: write 1 read 2',in
      print *,'wri_ludat3: mdme(982,1)',(mdme(ii,1),ii=982,982)
      print *,'wri_ludat3: mdmeun(982,1)',(mdmeun(ii,1),ii=982,982)
      endif
      return
      end







