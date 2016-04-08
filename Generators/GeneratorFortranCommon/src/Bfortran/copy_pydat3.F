*CMZ :          02/08/2001  11.43.38  by  Maria Smizanska
*-- Author :    Maria Smizanska   22/07/2000

      SUBROUTINE copy_pydat3
c
c   read  ludat3 of default  decays
c   make a copy ludat3 of users defined  decays
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
       DATA IFIRST /1/
*KEND.
      
      IF(IFIRST.EQ.1) THEN
      OPEN(60,FILE='Bdecays0.dat',STATUS='OLD',FORM='UNFORMATTED')
        IFIRST =0
      ENDIF
      
      idebug=0
      read (60) MDCYun,MDMEun,BRATun,KFDPun
      
      call icopy(MDCY,MDCYbi,1500)
      call icopy(MDME,MDMEbi,16000)
      call dcopy(BRAT,BRATbi,8000)      
      call icopy(KFDP,KFDPbi,40000)      

      
      

      if(idebug.ne.0) then
      print *,'copy_pydat3: mdmeun(981-9,1)',(mdmeun(ii,1),ii=981,989)
      print *,'copy_pydat3: mdmebi(981-9,1)',(mdmebi(ii,1),ii=981,989)
      endif
      return
      end







