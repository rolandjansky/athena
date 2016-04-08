*CMZ :          02/08/2001  11.48.54  by  Maria Smizanska
*-- Author :    Maria Smizanska   20/11/96

      SUBROUTINE UINIT
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : UINIT                                                         *
*        (module)                                                      *
*                                                                      *
* Description :                                                        *
*        Booking of NTUPLES 500 5001 5002                              *
*        Another histogram/ntuple booking can be added by user here    *
* Arguments   :                                                        *
*        i1 , i2   not used       [description]                        *
*                                                                      *
* Banks/Tables:                                                        *
*        [name]  ([filled | used | modified])                          *
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Author : Maria Smizanska                    Date : 20/11/96          *
*                                                                      *
*                                                                      *
* Last modifications :                                                 *
*        [name]                               Date : [date]            *
*                                                                      *
* Keywords :                                                           *
*        [keywords]                                                    *
*----------------------------------------------------------------------*
*
C +CDE, TYPING.
*KEEP,DOUB.
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      INTEGER PYK,PYCHGE,PYCOMP
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
       COMMON/PAWC/H(500000)
*KEND.
C ----------- end CDE --------------------------------------------------



      CHARACTER*8 TAGS(NDIM)
      SAVE TAGS

      CHARACTER*8 TAGS1(NDIM1)
      SAVE TAGS1

      CHARACTER*8 TAGS2(NDIM2)
      SAVE TAGS2

      DATA TAGS /
     +'event','NTRY','ntree','itree','lineLUJE',
     +'K1','K2','K3','K4','K5',
     +'P1','P2','P3','P4','P5',
     +'V1','V2','V3','V4','V5'   /

      DATA TAGS1 /
     +'ISUB','XSECTION','NHADRONI','PTHARD','YHARD',
     +'PT_B','ETA_B','PHI_B','FATHER_B','PT_AB','ETA_AB',
     +'PHI_AB','FATHERAB'  /

      DATA TAGS2 /
     +'event','NTRY','nmuons','imuon','name',
     +'fathname','PT','ETA','phi','XV','YV',
     +'ZV'  /

c       idebug=1
c       if(idebug.ne.0) print *,'UINIT: moj uinit0'
c       CALL HLIMIT(500000)
c       if(idebug.ne.0) print *,'UINIT: moj uinit1'
c       CALL HROPEN(16,'AT','at1.hbook','N',1024,0)
c       if(idebug.ne.0) print *,'UINIT: moj uinit2'
* Ntuple 5000 contains 'LUJET' type of information for
* B-signal chain and triggering muon

c        CALL HBOOKN(5000,'BBANAL',NDIM,'AT',1000,TAGS)
c        if(idebug.ne.0) print *,'UINIT: moj uinit3'
* Ntuple 5001 contains  information about hard process
* for those events that are accepted and written out

c        CALL HBOOKN(5001,'BBHARD',NDIM1,'AT',1000,TAGS1)

* Ntuple 5002 is filled in the case of muon inclusive
* process bb->muX was simulated. It contains information
* about all muons in such events passing the LVL1 selection cuts

c        CALL HBOOKN(5002,'BBMUON',NDIM1,'AT',1000,TAGS2)

c      call hbook1(1,'Particles per event',100,0.,2000.,1.)


*
      END          ! UINIT
