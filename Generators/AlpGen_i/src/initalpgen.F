C----------------------------------------------------------------------
      SUBROUTINE INITALPGEN
C----------------------------------------------------------------------
C     HERWIG/PYTHIA UNIVERSAL EVENT INITIALITION ROUTINE
C----------------------------------------------------------------------
      IMPLICIT NONE
#include "AlpGen_i/alpsho.inc"
      CHARACTER *3 CSHO
C--   GUP Event common block
#include "GeneratorFortranCommon/hepeup.inc"
C--   GUP common block
#include "GeneratorFortranCommon/heprup.inc"
C     CALSIM AND JET VARIABLES
C      INTEGER NCY,NCPHI,NJMAX,JETNO,NCJET
      INTEGER NCY,NCPHI
      DOUBLE PRECISION YCMIN,YCMAX,PI,ET,DELPHI,CPHCAL,SPHCAL,DELY,
     &         CTHCAL,STHCAL
C     &         CTHCAL,STHCAL,PCJET,ETJET
      PARAMETER (NCY=100)
      PARAMETER (NCPHI=60,PI=3.141593D0)
      COMMON/CALOR_M/DELY,DELPHI,ET(NCY,NCPHI),
     $     CTHCAL(NCY),STHCAL(NCY),CPHCAL(NCPHI),SPHCAL(NCPHI),YCMIN
     $     ,YCMAX
C
      INTEGER IEND,INORAD
      COMMON/SHVETO/IEND,INORAD(MAXNUP)
C     LOCAL VARIABLES
      CHARACTER*70 STDUMMY
      INTEGER I,NTMP,IHEPMIN
      DOUBLE PRECISION TMP
      DOUBLE PRECISION PBEAM1,PBEAM2
C     USER ACCESS TO MATCHING PARAMETERS
      INTEGER IUSRMAT
      PARAMETER (IUSRMAT=1)
c -- oj (ver206 backword compatibility)
      INTEGER IVER206
      DOUBLE PRECISION VETCLUS,VRCLUS

C
c Open the file with the input parameters
      FILENAME = 'inparmAlpGen.dat'
      NUNITINI=71
      OPEN(UNIT=NUNITINI,FILE=FILENAME,STATUS='OLD',ERR=900)
c Open the file with the unweighted alpgen events
      FILENAME = 'alpgen.unw_events'
      NUNIT=72
      OPEN(UNIT=NUNIT,FILE=FILENAME,STATUS='OLD',ERR=901)
C      WRITE(*,*) 'INPUT NAME OF FILE CONTAINING EVENTS'
C      WRITE(*,*) '(FOR "file.unw" ENTER "file")'
C      READ(*,*) FILENAME
C      CALL STRCATH(FILENAME,'.unw',TMPSTR)
C      call GETUNIT(NUNIT)
C      OPEN(UNIT=NUNIT,FILE=TMPSTR,STATUS='OLD')
C      CALL STRCATH(FILENAME,'_unw.par',TMPSTR)
C      CALL GETUNIT(NUNITINI)
C      OPEN(UNIT=NUNITINI,FILE=TMPSTR,STATUS='OLD')
C     OPEN A LOG FILE
C      CALL ALSHCD(CSHO)
C      IF(CSHO.EQ.'HER') THEN
C        CALL STRCATH(FILENAME,'.her-log',TMPSTR)
C      ELSE
C        CALL STRCATH(FILENAME,'.pyt-log',TMPSTR)
C      ENDIF
C      CALL GETUNIT(NUNITOUT)
C      OPEN(UNIT=NUNITOUT,FILE=TMPSTR,STATUS='UNKNOWN')
C     START READING FILE
      DO I=1,10000
        READ(NUNITINI,'(A)') STDUMMY
        IF(STDUMMY(1:4).EQ.'****') GOTO 10
        WRITE(*,*) STDUMMY
C         WRITE(NUNITOUT,*) STDUMMY
      ENDDO
C
C     READ IN INPUT PARAMETERS
 10   READ(NUNITINI,*) IHRD
C
c     IF(IHRD.EQ.12) THEN
c     WRITE(*,*) 'HJET PROCESSES NOT AVAILABLE AS YET'
c     STOP
c     ENDIF
      READ(NUNITINI,*) MC,MB,MT,MW,MZ,MH
      IVER206 = 1
      DO I=1,1000
        READ(NUNITINI,*,ERR=20) NTMP,TMP
        IF(NTMP.LE.200.AND.NTMP.GE.1) THEN
           PARVAL(NTMP)=TMP
        ENDIF
c-- BC
        IF (NTMP.EQ.501) ETCLUS = TMP
        IF (NTMP.EQ.502) RCLUS = TMP
        IF (NTMP.EQ.503) ETACLMAX = TMP
        IF (NTMP.EQ.504) THEN
c -- oj
           IEXC = TMP
           IVER206 = 0
        ENDIF

c-- BC END
      ENDDO
 20   CONTINUE

      IF (IVER206.EQ.1) THEN
         WRITE(*,*) 'PARAMETERS USED AS V2.06 (=BACKWARD COMPATIBILITY)'
         IEXC = ETACLMAX
         ETACLMAX = PARVAL(40)+PARVAL(50) ! ETAJMAX+DRJMIN
      ENDIF

      READ(NUNITINI,*) AVGWGT,ERRWGT
      READ(NUNITINI,*) UNWEV,TOTLUM
      CLOSE(NUNITINI)
C      WRITE(NUNITOUT,*) " "
C      WRITE(NUNITOUT,*) "INPUT CROSS SECTION (PB):",AVGWGT," +/-",ERRWGT
C      WRITE(NUNITOUT,*) "NUMBER OF INPUT EVENTS:",UNWEV
C      WRITE(NUNITOUT,*) "INTEGRATED LUMINOSITY:",TOTLUM
C     WRITE PARAMETER VALUES
      CALL AHSPAR
      PBEAM1=DBLE(EBEAM)
      PBEAM2=DBLE(EBEAM)
      IH1=1
C     CONVERT PDF TYPES
      CALL PDFCONVH(NDNS,NTMP,PDFTYP)
C     DEFINE RANGE FOR PARTONS TO BE USED IN MATCHING
      NLJETS=PARVAL(10)     !  NJETS
      DO I=1,MAXNUP
        INORAD(I)=0
      ENDDO
      CALL ALSHCD(CSHO)
      IF(CSHO.EQ.'HER') THEN
        NPFST=149
        NPLST=149
C     HERWIG: ALL SHOWERS ORIGINATE FROM IHEP=6
        IEND=6
C     HERWIG: HEPEVT EVENT RECORD FOR FINAL STATE STARTS AT 7=6+1
        IHEPMIN=6
      ELSE
        NPFST=1
        NPLST=1
C     PYTHIA: ALL SHOWERS ORIGINATE FROM IHEP=O
        IEND=0
C     PYTHIA: HEPEVT EVENT RECORD FOR FINAL STATE STARTS AT 1=0+1
        IHEPMIN=0
        IDPRUP=661
      ENDIF
      IF(IHRD.LE.2) THEN
C        NLJETS=NPART-6
        NJSTART=4
        NJLAST=155
C     DO NOT INCLUDE IN MATCHING RADIATION ORIGINATING FROM THE W
        INORAD(IHEPMIN+2+NLJETS+1)=1
C     DO NOT INCLUDE IN MATCHING RADIATION ORIGINATING FROM HEAVY QUARK
C     PAIR
        INORAD(IHEPMIN+1)=1
        INORAD(IHEPMIN+2)=1
      ELSEIF(IHRD.LE.4) THEN
C        NLJETS=NPART-4
        NJSTART=2
        NJLAST=155
C     DO NOT INCLUDE IN MATCHING RADIATION ORIGINATING FROM THE W
        IF(CSHO.EQ.'HER'.AND.NLJETS.EQ.0) THEN
C     ALLOW RADIATION FROM IHEPMIN+1 IN HERWIG WHEN NJET=0
          INORAD(IHEPMIN+NLJETS+1)=0
        ELSE
          INORAD(IHEPMIN+NLJETS+1)=1
        ENDIF
      ELSEIF(IHRD.EQ.5) THEN
C        NLJETS=NPART-3*(NW+NZ)-NH-2
        NJSTART=2
        NJLAST=155
C     DO NOT INCLUDE IN MATCHING RADIATION ORIGINATING FROM THE W
        IF(CSHO.EQ.'HER'.AND.NLJETS.EQ.0.AND.(NW+NZ+NH+NPH).EQ.1) THEN
C     ALLOW RADIATION FROM IHEPMIN+1 IN HERWIG WHEN NJET=0 AND NW+NZ+NH
C     +NPH=1
          INORAD(IHEPMIN+NLJETS+1)=0
        ELSE
          DO I=1,NW+NZ+NH+NPH
            INORAD(IHEPMIN+NLJETS+I)=1
          ENDDO
        ENDIF
      ELSEIF(IHRD.EQ.6) THEN
C     DO NOT INCLUDE IN MATCHING RADIATION ORIGINATING FROM HEAVY QUARK
C     PAIR
C     NLJETS=NPART-8  (IHVY.EQ.6)     NLJETS=NPART-4  (IHVY.LT.6)
        INORAD(IHEPMIN+1)=1
        INORAD(IHEPMIN+2)=1
        NJSTART=4
        NJLAST=155
      ELSEIF(IHRD.EQ.9) THEN
C     NLJETS=NPART-2
        NJSTART=2
        NJLAST=155
      ELSEIF(IHRD.EQ.10) THEN
C        NLJETS=NPART-4
        NJSTART=3
        NJLAST=155
C     DO NOT INCLUDE IN MATCHING RADIATION ORIGINATING FROM THE W
        INORAD(IHEPMIN+NLJETS+1+1)=1
C     DO NOT INCLUDE IN MATCHING RADIATION ORIGINATING FROM THE CHARM
        INORAD(IHEPMIN+1)=1
      ELSEIF(IHRD.EQ.11) THEN
C        NLJETS=NPART-2-NPH
        NJSTART=2
        NJLAST=155
C     DO NOT INCLUDE IN MATCHING THE HARD PHOTONS
        DO I=1,NPH
          INORAD(IHEPMIN+NLJETS+I)=1
        ENDDO
      ELSEIF(IHRD.EQ.12) THEN
C        NLJETS=NPART-2-NH
        NJSTART=2
        NJLAST=155
C     DO NOT INCLUDE IN MATCHING THE HIGGS DECAY PRODUCTS
        IF(NLJETS+NH.GT.1) THEN
          DO I=1,NH
            INORAD(IHEPMIN+NLJETS+I)=1
          ENDDO
        ELSE
          IF(CSHO.EQ.'HER'.AND.NLJETS.EQ.0) THEN
C     ALLOW RADIATION FROM IHEPMIN+1 IN HERWIG WHEN NJET=0
            INORAD(IHEPMIN+1)=0
          ELSE
            INORAD(IHEPMIN+NLJETS+1)=1
          ENDIF
        ENDIF
      ELSEIF(IHRD.EQ.14) THEN
C        NLJETS=NPART-4-NPH
        NJSTART=2
        NJLAST=155
C     DO NOT INCLUDE IN MATCHING RADIATION ORIGINATING FROM THE W
        IF(CSHO.EQ.'HER'.AND.NLJETS.EQ.0.AND.NPH.EQ.0) THEN
C     ALLOW RADIATION FROM IHEPMIN+1 IN HERWIG WHEN NJET=0
          INORAD(IHEPMIN+NLJETS+1)=0
        ELSE
          INORAD(IHEPMIN+NLJETS+1)=1
        ENDIF
      ELSEIF(IHRD.EQ.15) THEN
C        NLJETS=NPART-6-NPH
        NJSTART=2
        NJLAST=155
C     DO NOT INCLUDE IN MATCHING RADIATION ORIGINATING FROM THE W
        INORAD(IHEPMIN+2+NLJETS+NPH+1)=1
C     DO NOT INCLUDE IN MATCHING RADIATION ORIGINATING FROM HEAVY QUARK
C     PAIR
C     NOTICE THAT HERE THE LIGHT JETS PRECEDE THE QQ PAIR
        INORAD(IHEPMIN+NLJETS+1)=1
        INORAD(IHEPMIN+NLJETS+2)=1
      ELSEIF(IHRD.EQ.16) THEN
C     DO NOT INCLUDE IN MATCHING RADIATION ORIGINATING FROM HEAVY QUARK
C     PAIR
C     NLJETS=NPART-8  (IHVY.EQ.6)     NLJETS=NPART-4  (IHVY.LT.6)
        INORAD(IHEPMIN+1)=1
        INORAD(IHEPMIN+2)=1
        NJSTART=4
        NJLAST=155
C     DO NOT INCLUDE IN MATCHING THE HARD PHOTONS
        DO I=1,NPH
          INORAD(IHEPMIN+2+NLJETS+I)=1
        ENDDO
      ENDIF
C
C     INPUT JET MATCHING CRITERIA
C
      IF(ICKKW.EQ.1) THEN
        WRITE(*,*) ' '
        WRITE(*,*) 'based on interface of v2.11 '
        WRITE(*,*) 'INPUT 0 FOR INCLUSIVE JET SAMPLE, 1 FOR EXCLUSIVE'
        WRITE(*,*) '(SELECT 0 FOR HIGHEST PARTON MULTIPLICITY SAMPLE)'
        WRITE(*,*) '(SELECT 1 OTHERWISE)'
        WRITE(*,*) 'IEXC = ',IEXC
Cbc        READ(*,*) IEXC
        IF(NLJETS.GT.0) THEN
          WRITE(*,*) 'INPUT ET(CLUS), R(CLUS), ETACLMAX'
C --oj          ETCLUS=MAX(PTJMIN+5,1.2*PTJMIN)
C --oj          RCLUS=DRJMIN
          VETCLUS=MAX(PTJMIN+5,1.2D0*PTJMIN)
          VRCLUS=DRJMIN
C -- oj          WRITE(*,*) '(SUGGESTED VALUES:',ETCLUS,RCLUS,ETAJMAX,')'
          WRITE(*,*) '(SUGGESTED VALUES:',VETCLUS,VRCLUS,ETAJMAX,')'
Cbc          READ(*,*) ETCLUS,RCLUS,ETACLMAX
        ELSEIF(NLJETS.EQ.0) THEN
          WRITE(*,*) 'INPUT ET(CLUS), R(CLUS), ETACLMAX'
          WRITE(*,*) '(MUST MATCH VALUES USED IN PROCESSING',
     +         ' OF NJET>0 EVENTS; THESE DEFAULT TO:'
          WRITE(*,*)
     +         'ETCLUS=MAX(PTJMIN+5,1.2*PTJMIN) RCLUS=DRJMIN, ',
     +         ' ETACLMAX=ETAJMAX)'
Cbc          READ(*,*) ETCLUS,RCLUS,ETACLMAX
        ENDIF
        WRITE(*,*) ' '
        WRITE(*,*) 'JET PARAMETERS FOR MATCHING:'
        WRITE(*,*) 'ET>',ETCLUS,' ETACLUS<',ETACLMAX,' R=',RCLUS
        WRITE(*,*) 'DR(PARTON-JET)<',1.5*RCLUS
      ENDIF
C
C     CALORIMETER ETA RANGE
C     YCMAX=ETAJMAX+DRJMIN
      YCMAX=ETACLMAX+RCLUS      ! MLM August 24
      YCMIN=-YCMAX
C     FROM NOW ON, PROCESS THE INFORMATION READ IN, TO COMPLETE SETTING
C     UP THE GUP COMMON
C
C--   SET UP THE BEAMS
C--   ID'S OF BEAM PARTICLES
      IF(IH1.EQ.1) THEN
        IDBMUP(1) = 2212
      ELSEIF(IH1.EQ.-1) THEN
        IDBMUP(1) =-2212
      ELSE
        WRITE(*,*) 'BEAM 1 NOT PROPERLY INITIALISED, STOP'
        STOP
      ENDIF
      IF(IH2.EQ.1) THEN
        IDBMUP(2) = 2212
      ELSEIF(IH2.EQ.-1) THEN
        IDBMUP(2) =-2212
      ELSE
        WRITE(*,*) 'BEAM 2 NOT PROPERLY INITIALISED, STOP'
        STOP
      ENDIF
      EBMUP(1) = ABS(PBEAM1)
      EBMUP(2) = ABS(PBEAM2)
C--   PDF'S FOR THE BEAMS; WILL BE EVALUATED USING THE NDNS VARIABLE
C     READ IN EARLIER
      PDFGUP(1) = -1
      PDFGUP(2) = -1
      PDFSUP(1) = -1
      PDFSUP(2) = -1
C--   WHAT DO DO WITH THE WEIGHTS(WE ARE GENERATING UNWEIGHTED EVENTS)
      IDWTUP = 3
C--   ONLY ONE PROCESS
      NPRUP  = 1
C--   CROSS SECTION
      XSECUP(1) = avgwgt
C--   ERROR ON THE CROSS SECTION
      XERRUP(1) = errwgt
C--   MAXIMUM WEIGHT
      XMAXUP(1) = avgwgt
C--   HERWIG/PYTHIA SPECIFIC PART
      CALL ALSHIN(I)
      LPRUP(1) = I
      RETURN

 900  WRITE (*,*)
     &        'INITALPGEN: File inparmAlpGen.dat could not be opened'
 901  WRITE (*,*)
     &        'INITALPGEN: File alpgen.unw_events could not be opened'
      STOP

      END

c-------------------------------------------------------------------
      subroutine AHspar
c     set list of parameters types and assign default values
c-------------------------------------------------------------------
      implicit none
#include "AlpGen_i/alpsho.inc"
c
      ih2=parval(2)
      ebeam=parval(3)
      ndns=parval(4)
      ickkw=parval(7)
      ihvy=parval(11)
      ihvy2=parval(12)
      nw=parval(13)
      nz=parval(14)
      nh=parval(15)
      nph=parval(16)
      ptjmin=parval(30)
      ptbmin=parval(31)
      ptcmin=parval(32)
      ptlmin=parval(33)
      metmin=parval(34)
      ptphmin=parval(35)
      etajmax=parval(40)
      etabmax=parval(41)
      etacmax=parval(42)
      etalmax=parval(43)
      etaphmax=parval(44)
      drjmin=parval(50)
      drbmin=parval(51)
      drcmin=parval(52)
      drlmin=parval(55)
      drphjmin=parval(56)
      drphlmin=parval(57)
      drphmin=parval(58)
      mllmin=parval(61)
      mllmax=parval(62)
      itopprc=parval(102)
c
      end

c-------------------------------------------------------------------
      subroutine pdfconvH(nin,nout,type)
c-------------------------------------------------------------------
c converts ALPHA convention for PDF namings to hvqpdf conventions
      implicit none
      integer nin,nout
      character*25 type
      character*25 pdftyp(20,2)
      data pdftyp/
c cteq sets
     $     'CTEQ4M ','CTEQ4L ','CTEQ4HJ',
     $     'CTEQ5M ','CTEQ5L ','CTEQ5HJ',
     $     'CTEQ6M ','CTEQ6L ',12*' ',
C MRST SETS
     $     'MRST99 ',
     $     'MRST01; as=0.119','MRST01; as=0.117','MRST01; as=0.121'
     $     ,'MRST01J; as=0.121','MRST02LO',14*' '/
      integer pdfmap(20,2)
      data pdfmap/
     $   81,83,88,   101,103, 104,   131,133, 12*0,
     $  111,  185,186,187,188,189,   14*0/
c
      nout=pdfmap(mod(nin ,100),1+nin /100)
      type=pdftyp(mod(nin ,100),1+nin /100)

      end


