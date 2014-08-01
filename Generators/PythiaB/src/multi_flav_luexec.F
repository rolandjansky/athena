*CMZ :          02/08/2001  11.45.54  by  Maria Smizanska
*-- Author :    Maria Smizanska   22/07/2000



      subroutine multi_flav_luexec
c -----------------------------------------------------------------------
c  multi_flav  special for cases bbbb or bbcc (or bbbbcc, bbbbcccc)
c -----------------------------------------------------------------------


*KEEP,DOUB.
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
c      INTEGER PYK,PYCHGE,PYCOMP
*KEEP,HEAVYFLAV.
* store info about  multiple heavy flavour  produced in event
       common/multif/ibq,icq
* remeber links of  B and antiB chosen to decay free in bbbb
       common/fixedB/is1,is2

*KEEP,BBMOD.
* internal number to recognise bb decay regime
       common/bbmod/IBBMOD
*KEND.

       idebug=0


c  (0,0) (0,1) (0,2) (1,0) NO
c  (1,1) (1,2) (2,0) (2,1,) (2,2)  YES
c take only bbcc or bbbb (or bbbbcc, bbbbcccc)
       if(ibq.le.1.and.(ibq*icq.eq.0))  return


c bbcc (1,1) (1,2)
       if(ibq.eq.1.and.icq.ne.0) call bbcc_luexec
c bbbb (2,0) (2,1) (2,2)
       if(ibq.ge.2) call bbbb_luexec(1)


       return
       end



