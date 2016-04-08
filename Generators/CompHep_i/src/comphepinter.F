*************************************************************************
*                      interface routines                               *
*          for CompHEP 4.4.3  -  PYTHIA 6.2 interface                   *
*************************************************************************
*  15.12.99         Ilyin V.A.                                          *  
*  24.12.99         Modified by A. Belyaev    for   PYTHIA 6.1          *
*  18.05.2000  Modified by A.Skachkova for CompHEP version N4           *
*  22.10.2000    SETWID is removed, V.A.Ilyin                           *
*  22.10.2000  Zero masses protected, V.A.Ilyin                         *
*  25.11.2001  updated to PYTHIA 6.2 by V.A.Ilyin                       *
*  27.03.2006  Modified for Version 4.4.3 by Vikas Bansal               *
*************************************************************************

*****************************************
*...write event (in PYJETS format)
*****************************************
      SUBROUTINE EVOUT
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)      
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      SAVE
      
      WRITE(6,*) N,((K(I,J),J=1,5),I=1,N),((P(I,J),J=1,5),I=1,N),
     .             ((V(I,J),J=1,5),I=1,N)
     
      RETURN
      END

**************************************************
*...read number of subprocesses
*************************************************
      SUBROUTINE SUBNUM
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
*...CompHEP-PYTHIA interface   
      CHARACTER XXXX*80,PROCESS*80,EVFILE*500, TotalProc*30
      COMMON /INPARM/ IS,NEVPY,PROCESS,EVFILE
      COMMON/SUBPROC/NUP(100),KFP(100,20),SIGMAERR(100),IDPR(100),
     .         NCUT(100),NEV(100),XM(100,20,3),SIGMA(100),NPROC
  
      IS = 44
1     READ(IS,FMT='(A80)') XXXX
      IF (XXXX(1:15).EQ.'##total: Nproc=') THEN
         READ(XXXX,*) TotalProc,TotalProc
         READ(TotalProc,FMT='(6x,i4)') nProc
      ELSE
         GOTO 1
      ENDIF  
      REWIND IS                  

      RETURN
      END           

*****************************
*...read header of the event file
************************************
      SUBROUTINE RHEADER
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
*...PYTHIA common blocks
      INTEGER PYCOMP
      COMMON /PYPARS/ MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &LPRUP(MAXPUP)
*...CompHEP-PYTHIA interface
      CHARACTER PROCESS*80,EVFILE*500
      CHARACTER BEAM*16
      LOGICAL  EOL
      CHARACTER*100 EMPTYS
      CHARACTER*100 SPROCESS,BEAM1,BEAM2,STRF1,STRF2,XXXX,EVENTS
      COMMON /INPARM/ IS,NEVPY,PROCESS,EVFILE
      COMMON/SUBPROC/NUP(100),KFP(100,20),SIGMAERR(100),IDPR(100),
     .         NCUT(100),NEV(100),XM(100,20,3),SIGMA(100),NPROC
      COMMON/BEAMS/BENERGY(2),KFB(2),BMASS(2),BEAM(2)
      SAVE

      READ(IS,FMT='(A80)') BEAM1
      READ(IS,FMT='(A80)') BEAM2
      READ(IS,FMT='(A80)') STRF1
      IF ( .NOT. EOL(STRF1)) READ(IS,FMT='(30x,A1)') XXXX
      READ(IS,FMT='(A80)') STRF2
      IF ( .NOT. EOL(STRF2)) READ(IS,FMT='(30x,A1)') XXXX

*...LOOP OVER NUMBER OF SUBPROCESSES
      DO NPRC=1,NPROC

*VB   Read Sub Process
      SPROCESS = EMPTYS(100)
      READ(IS,FMT='(20x,A99)') SPROCESS

*VB   READ Cross section for a sub-process
      j = 0
*VB   increasing j to near 'Xsection'
      j = j + 1
503   CONTINUE
      j = j + 1
      IF (SPROCESS(j:j).eq.';' .OR. j.GT. 120) goto 603
      IF(SPROCESS(j:(j+7)).EQ.'CrosSec=') THEN
*         READ(SPROCESS((j+8):),FMT='(G11.5)') SIGMA(NPRC)
         READ(SPROCESS((j+8):),*) SIGMA(NPRC)
         GOTO 603
      ENDIF   
      GOTO 503
603   CONTINUE

*VB   READ Cross section Error for a sub-process
      j = 0
*VB   increasing j to near 'XsectionErr'
      j = j + 1
513   CONTINUE
      j = j + 1
      IF (SPROCESS(j:j).eq.';' .OR. j.GT. 120) goto 613
      IF(SPROCESS(j:(j+10)).EQ.'CrosSecErr=') THEN
*         READ(SPROCESS((j+11):),FMT='(G11.5)') SIGMAERR(NPRC)
         READ(SPROCESS((j+11):),*) SIGMAERR(NPRC)
         GOTO 613
      ENDIF   
      GOTO 513
613   CONTINUE

*VB Reading second line of sub process only if it exists.
*VB It says master=3. It is fixed in comphep to this value.
*VB Its value being 3 corresponds to unweighted event as per Les Houches
*VB document hep-ph/0109068
      IF ( .NOT. EOL(SPROCESS)) READ(IS,FMT='(30x,A1)') XXXX

*...  which version of comphep. This is 4.4 compatible
      READ(IS,FMT='(10x,A80)') XXXX

*VB Read number of (unweighted) events in the input for a sub-process
      READ(IS,FMT='(9x,A100)') EVENTS
      j = 0
702   CONTINUE
      j = j + 1
      IF (EVENTS(j:j).eq.';' .OR. j.GT. 120) goto 802
      IF(EVENTS(j:(j+9)).EQ.'IDprocess=') THEN
         READ(EVENTS((j+10):),*) IDPR(NPRC)
         GOTO 802
      ENDIF   
      GOTO 702
802   CONTINUE
      
502   CONTINUE
      j = j + 1
      IF (EVENTS(j:j).eq.';' .OR. j.GT. 120) goto 602
      IF(EVENTS(j:(j+1)).EQ.'N=') THEN
         READ(EVENTS((j+2):),*) NEV(NPRC)
         GOTO 602
      ENDIF   
      GOTO 502
602   CONTINUE

703   CONTINUE
      j = j + 1
      IF (EVENTS(j:j).eq.';' .OR. j.GT. 120) goto 803
      IF(EVENTS(j:(j+4)).EQ.'CutN=') THEN
         READ(EVENTS((j+5):),*) NCUT(NPRC)
         GOTO 803
      ENDIF   
      GOTO 703
803   CONTINUE

*VB Reading second line of n_events only if it exists.
*VB It says origN. i.e. Events before mixing.
      IF ( .NOT. EOL(EVENTS)) READ(IS,FMT='(30x,A1)') XXXX
*VB   Read parton information of a sub-process
      CALL R_PROC(NPRC,SPROCESS)

*...read names of beams used in CompHEP and get their KF codes and masses
      KFB1=KFB(1)
      KFB2=KFB(2)

      BEAM(1) = EMPTYS(20)
      CALL R_BEAM(BEAM1,STRF1,KFB(1),BEAM(1),BENERGY(1), BMASS(1)
     &,PDFGUP(1), PDFSUP(1))
      BEAM(2) = EMPTYS(20)
      CALL R_BEAM(BEAM2,STRF2,KFB(2),BEAM(2),BENERGY(2), BMASS(2)
     &,PDFGUP(2), PDFSUP(2))
      CALL PYNAME(KFB(1),BEAM(1))
      CALL PYNAME(KFB(2),BEAM(2))

*...define beam momenta for PYTHIA 'USER" frame
      CALL BEAMFR
 
*...set out-particle masses in PYTHIA according to CompHEP events
      DO 30 i=3,NUP(nprc)
       IF(XM(nprc,i,1).gt.0.d0) PMAS(PYCOMP(KFP(nprc,i)),1)=XM(nprc,i,1)
30    CONTINUE

*...end of loop over subprocess headers
      ENDDO

      READ(IS,FMT='(1x,A1)') XXXX
      READ(IS,FMT='(1x,A1)') XXXX

      RETURN
      END


*******************************
* evaluate momenta of beams  
*******************************
      SUBROUTINE BEAMFR
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
*...PYTHIA common blocks
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
*...CompHEP-PYTHIA interface
      CHARACTER BEAM*16
      DIMENSION PBCMS(2,5)
      COMMON/SUBPROC/NUP(100),KFP(100,20),SIGMAERR(100),IDPR(100),
     .         NCUT(100),NEV(100),XM(100,20,3),SIGMA(100),NPROC
      COMMON/BEAMS/BENERGY(2),KFB(2),BMASS(2),BEAM(2)
      SAVE

*...evaluate beams momenta in CMS
        PBCMS(1,1) = 0
        PBCMS(1,2) = 0
        PBCMS(1,3) = sqrt(BENERGY(1)**2-BMASS(1)**2)
        PBCMS(1,4) = BENERGY(1)
        PBCMS(1,5) = BMASS(1)
        PBCMS(2,1) = 0
        PBCMS(2,2) = 0
        PBCMS(2,3) = -sqrt(BENERGY(2)**2-BMASS(2)**2)
        PBCMS(2,4) = BENERGY(2)
        PBCMS(2,5) = BMASS(2)
*...evaluate beams momenta in the collison frame using Lorentz boost 
*...given from the CompHEP session

        RPDTY = 0.0

        P(1,1) = PBCMS(1,1)
        P(1,2) = PBCMS(1,2)
        P(1,3) = SINH(RPDTY)*PBCMS(1,4) + COSH(RPDTY)*PBCMS(1,3)
        P(1,4) = COSH(RPDTY)*PBCMS(1,4) + SINH(RPDTY)*PBCMS(1,3)
        P(1,5) = PBCMS(1,5)
        P(2,1) = PBCMS(2,1)
        P(2,2) = PBCMS(2,2)
        P(2,3) = SINH(RPDTY)*PBCMS(2,4) + COSH(RPDTY)*PBCMS(2,3)
        P(2,4) = COSH(RPDTY)*PBCMS(2,4) + SINH(RPDTY)*PBCMS(2,3)
        P(2,5) = PBCMS(2,5)
 
      RETURN
      END

******************************
* parsing the beam strings
******************************
      SUBROUTINE R_BEAM(COLL_BEAM,STRF,KFB,BEAM,BENERGY,BMASS,IPDFGUP
     & ,IPDFSUP)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &LPRUP(MAXPUP)
      CHARACTER BEAM*16,STRF*80,COLL_BEAM*80,SFUNC*5
      COMMON /QCDPAR/ QCDLF(6),NL
      SAVE
      
      IS = 44
*** error case
      IF (COLL_BEAM.eq.'') THEN
         write(*,*) 'Error in COLL_BEAM'
         stop
      ENDIF

*** error case
      IF (STRF.eq.'') THEN
         write(*,*) 'Error in STRF'
         stop
      ENDIF

*** default beam mass
      BMASS = 0

*** default beam energy is that at LHC
      BENERGY = 7000

      i = 0
*VB   increasing i to 'energy'
      i = i + 14
500   CONTINUE
      i = i + 1
      IF (COLL_BEAM(i:i).eq.';' .OR. i.GT. 47) goto 600
      IF(COLL_BEAM(i:(i+5)).EQ.'energy') THEN
         READ(COLL_BEAM((i+7):),*) BENERGY
         i = 34
         GOTO 500
      ENDIF   

      IF(COLL_BEAM(i:(i+1)).EQ.'KF') THEN
         READ(COLL_BEAM((i+3):),*) KFB
         i = 37
         GOTO 500
      ENDIF   

      IF(COLL_BEAM(i:(i+3)).EQ.'name') THEN
         READ(COLL_BEAM((i+5):),*) BEAM
         i = 47
*VB  Check if beam name is of single alphabet        
         IF (BEAM(2:2).ne."'") THEN
            IF (BEAM.eq.'pr') THEN
               BEAM ='proton'
               BMASS = 0.938
            ENDIF
            IF (BEAM.eq.'an') THEN
               BEAM ='anti-proton'
               BMASS= 0.938
            ENDIF
            IF (BEAM.eq.'el') THEN
               BEAM ='electron'
               BMASS= 0.000511
            ENDIF
           IF (BEAM.eq.'La') THEN
               BEAM ='gamma'
               BMASS= 0.
            ENDIF
            GOTO 499
         ELSE 
            beam = beam(1:1)
            GOTO 499
         ENDIF
       ENDIF   
       GOTO 500

600   CONTINUE

*VB   If you are reaching here then wrong beam
      GOTO 998

499   CONTINUE

*VB------Now parsing Structure function

      i = 0
*VB   increasing i to 'strfun name'
      i = i + 20
510   CONTINUE
      i = i + 1
      IF (STRF(i:i).eq.';' .OR. i.GT. 100) goto 610
      IF(STRF(i:(i+3)).EQ.'name') THEN
         READ(STRF((i+5):),*) SFUNC
         IF (SFUNC .eq. 'OFF') THEN
            write(*,*) 'STRFUN is OFF. It is not required by Pythia'
            GOTO 599
         ENDIF
         i = i + 5
         GOTO 510
      ENDIF   

      IF(STRF(i:(i+4)).EQ.'PDFid') THEN
         READ(STRF((i+6):),*) IPDFSUP
         i = i + 4
         GOTO 510
      ENDIF   

      IF(STRF(i:(i+4)).EQ.'PDFgr') THEN
         READ(STRF((i+6):),*) IPDFGUP
         GOTO 599
      ENDIF   
      GOTO 510

610   CONTINUE

*VB----problem with strfun
      goto 999

599   CONTINUE

      RETURN

998   WRITE(*,*) 'Unknown BEAM'
      RETURN

999   WRITE(*,*) 'Unknown STRF'
      RETURN
      END


*********************************
* parsing the subprocess string
*********************************
      SUBROUTINE R_PROC(NPRC,PROCESS)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      COMMON/SUBPROC/NUP(100),KFP(100,20),SIGMAERR(100),IDPR(100),
     .         NCUT(100),NEV(100),XM(100,20,3),SIGMA(100),NPROC
      COMMON /QCDPAR/ QCDLF(6),NL
      CHARACTER*80 PROCESS, PARTON, PARTON_MASS, XXXX,QCDINFO
      CHARACTER*3 PNAME(1:8)
      CHARACTER*6 PNAME_TMP(1:8)
      LOGICAL EOL
      SAVE

      IS = 44

*VB   Number of particles in a subprocess
      NUP(NPRC) = 0

      j = 0
*VB   increasing j to near 'NPartons'
      j = j + 40
501   CONTINUE
      j = j + 1
      IF (PROCESS(j:j).eq.';') goto 601
      IF(PROCESS(j:(j+6)).EQ.'Nparton') THEN
         READ(PROCESS((j+8):),*) NUP(NPRC)
         GOTO 601
      ENDIF   
      GOTO 501
601   CONTINUE

      IF (NUP(NPRC).GT.10) THEN
        WRITE(*,*) 'Error: more than 10 partons in final state'
        STOP
      ENDIF

      IF(NUP(NPRC).EQ.0) THEN
        WRITE(*,*) 'No process found in event file. Stopping'
        STOP
      ENDIF

      DO K = 1, NCUT(NPRC)
        READ(IS,FMT='(22x,A80)') XXXX
      ENDDO

*VB Number of input particles
      NIN = 0
      DO K = 1, NUP(NPRC)

      READ(IS,FMT='(22x,A80)') PARTON
      READ(IS,FMT='(9x,A80)') PARTON_MASS

*VB  get parton IN status
      j = 0
514   CONTINUE
      j = j + 1
      IF (PARTON(j:j).eq.';' .OR. j.GT. 70) goto 614
      IF(PARTON(j:(j+2)).EQ.'in=') THEN
         READ(PARTON((j+3):),*) NIN_READ
         IF (NIN_READ .ge. NIN ) NIN = NIN_READ
         GOTO 614
      ENDIF   
      GOTO 514
614   CONTINUE


*VB  get parton KF code
      j = 0
504   CONTINUE
      j = j + 1
      IF (PARTON(j:j).eq.';' .OR. j.GT. 70) goto 604
      IF(PARTON(j:(j+2)).EQ.'KF=') THEN
         READ(PARTON((j+3):),*) KFP(NPRC, K)
         GOTO 604
      ENDIF   
      GOTO 504
604   CONTINUE

*VB  get parton name
      j = 0
505   CONTINUE
      j = j + 1
      IF (PARTON(j:j).eq.';' .OR. j.GT. 30) goto 605
      IF(PARTON(j:(j+3)).EQ.'name') THEN
         READ(PARTON((j+5):),*) PNAME_TMP(K)
         GOTO 605
      ENDIF   
      GOTO 505
605   CONTINUE

      DO 551 I=2,6
         IF(PNAME_TMP(K)(I:I) .eq. "'") L = I
551   CONTINUE
         PNAME(K) = PNAME_TMP(K)(2:(L-1))
*VB The following line is now(gfortran) sufficient
         PNAME(K) = PNAME_TMP(K)


*VB The following is only for exceptional events from COMPHEP.
*VB In newer version it is taken care by COMPHEP
*VB   If KF from comphep is 0, then find it in this file
      IF (KFP(NPRC,K).eq.0) THEN
        KFP(NPRC,K) = KFPART(PNAME(K))
        IF(KFP(NPRC,K).eq.0) THEN
           write(*,*) 'Unknown for PYTHIA particle name: ',
     &          PNAME(K)
          stop
        ENDIF
      ENDIF

*VB  get parton_mass
      j = 0
506   CONTINUE
      j = j + 1
      IF (PARTON_MASS(j:j).eq.';' .OR. j.GT. 10) goto 606
      IF(PARTON_MASS(j:(j+3)).EQ.'mass') THEN
         READ(PARTON_MASS((j+5):),*) XM(NPRC,K,1) 
         GOTO 606
      ENDIF   
      GOTO 506
606   CONTINUE

      ENDDO

      IF((NUP(NPRC)-NIN).le.0) THEN
        WRITE(*,*) 'No OUT particles in the process. Stopping'
        STOP
      ENDIF

      IF(NIN.eq.1) THEN
        WRITE(*,*) 'No decay process allowed. Stopping'
        STOP
      ENDIF

      IF(NIN.eq.0) THEN
        WRITE(*,*) 'No input particle in the process. Stopping'
        STOP
      ENDIF

      READ(IS,FMT='(20x,A80)') QCDINFO      
      i=0
511   CONTINUE
      i = i + 1
      IF (QCDINFO(i:i).eq.';' .OR. i.GT. 75) goto 611
      IF(QCDINFO(i:(i+2)).EQ.'NL=') THEN
        READ(QCDINFO((i+3):),*) NL
         i = 10
         GOTO 511
      ENDIF   

      IF(QCDINFO(i:(i+9)).EQ.'QCDLambda=') THEN
        READ(QCDINFO((i+10):),*) QCDLF(5)
        GOTO 611
      ENDIF
      GOTO 511


*VB READ FORMAT LINES
611   READ(IS,FMT='(20x,A80)') XXXX      
556   IF ( .NOT. EOL(XXXX)) THEN 
         READ(IS,FMT='(20x,A80)') XXXX
         GOTO 556
      ENDIF

      RETURN
      END


*######################################################
*  Some service routines
*######################################################

********************
* make empty string
********************
      FUNCTION EMPTYS(N)
      CHARACTER*(*) EMPTYS
      SAVE

      DO 15 k=1,N
           EMPTYS(k:k)=' '
15    CONTINUE
      RETURN
      END


********************
* Check End of Line
********************
      LOGICAL FUNCTION EOL(INPUT_LINE)
      CHARACTER*100 INPUT_LINE
      INTEGER K
      SAVE

      EOL = .FALSE.
      DO 16 K=1,100
        IF (INPUT_LINE(K:K).eq.';') EOL = .TRUE.
16    CONTINUE
      RETURN
      END

*******************************
* delete blanks in string
*******************************
      FUNCTION CMPRS(STR)
      CHARACTER*500 STR,XSTR,CMPRS
      CHARACTER*500 EMPTYS
      SAVE

      j=1
      XSTR = EMPTYS(500)
      DO 10 i=1,80
        IF(STR(i:i).ne.' ') THEN
          XSTR(j:j)=STR(i:i)
          j=j+1
        ENDIF
10    CONTINUE
      CMPRS = XSTR
      RETURN
      END
*****************************************
*...initialization procedure for input data
*   25.11.2001  V.Ilyin
*****************************************
      SUBROUTINE RINPAR
      COMMON /INPARM/ IS,NEVPY,PROCESS,EVFILE
      CHARACTER XXX*500,PROCESS*80,EVFILE*500,CMPRS*500
      SAVE

      OPEN(22,FILE='inparmCompHep.dat',STATUS='UNKNOWN')
      READ(22,FMT='(A500)') XXX
      READ(22,FMT='(A500)') XXX

      EVFILE = CMPRS(XXX)

      READ(22,FMT='(A500)') XXX
      READ(22,FMT='(A500)') XXX

      PROCESS = CMPRS(XXX)

c      READ(22,FMT='(i2)') IS
      IS = 44                              ! channel to read event file
*... input channel for unweighted events 
      OPEN(IS,FILE=EVFILE,STATUS='UNKNOWN')

      READ(22,FMT='(A500)') XXX
      READ(22,FMT='(i10)') NEVPY
      CLOSE(22)

      RETURN
      END

*************************************************************************
*                         KFPART routine                                *
*          for CompHEP 41.10  -  PYTHIA 6.2 interface                   *
*************************************************************************
*  25.11.2001  Ilyin V.A.                                               *
*************************************************************************
*VB In version 4.4.3 kfpart is part of COMPHEP
* This can be still kept here to add quick functionality in case of new particles which will have to be added here.
 
      INTEGER FUNCTION kfpart(pname)
      CHARACTER*3 pname

      IF     (pname.eq.'e1 ') THEN 
         kfpart=11
      ELSEIF (pname.eq.'e4 ') THEN 
         kfpart=17
      ELSEIF (pname.eq.'ex ') THEN ! e*-
         kfpart=4000011
      ELSEIF (pname.eq.'EX ') THEN !
         kfpart=-4000011
      ELSE 
         kfpart=0
      ENDIF

      RETURN
      END


