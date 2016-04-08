c
C  This version adapted from pythia-pgs/src/ME2pythia.f
c  .. version pythia-pgs_V2.1.7.tar.gz
c  in MadGraph download pages in:
c  http://madgraph.hep.uiuc.edu/
c
C******************************************************
C*          MadEvent - Pythia interface.              *
C*           Version 4.2, 4 March 2007                *
C*                                                    *
C*  - Improvement of matching routines                *
C*                                                    *
C*  Version 4.1                                       *
C*                                                    *
C*  - Possibility to use several files                *
C*                                                    *
C*  Version 4.0                                       *
C*                                                    *
C*  - Routines for matching of ME and PS              *
C*                                                    *
C*  Version 3.8                                       *
C*                                                    *
C*  - Give the event number in the event file in the  *
C*    new variable IEVNT in MGUPPRIV                  *
C*                                                    *
C*  Version 3.7                                       *
C*                                                    *
C*  - Set mass of massless outgoing particles to      *
C*    Pythia mass (PMAS(I,1))                         *
C*                                                    *
C*  Version 3.6                                       *
C*                                                    *
C*  - Removed the 1st # from the event file header    *
C*                                                    *
C*  Version 3.5                                       *
C*                                                    *
C*  - Reads according to the new LH event file format *
C*  - Now only LNHIN, LNHOUT and MSCAL in MGUPPRIV    *
C*                                                    *
C*  Version 3.4                                       *
C*                                                    *
C*  - Reads particle masses from event file           *
C*                                                    *
C*  Version 3.3                                       *
C*                                                    *
C*  - Added option MSCAL in common block MGUPPRIV to  *
C*    choose between fix (0, default) or event-based  *
C*    (1) scale for Pythia parton showering (SCALUP). *
C*  - Fixed bug in reading the SLHA file              *
C*                                                    *
C*  Version 3.2                                       *
C*                                                    *
C*  - Reading the SLHA format param_card from the     *
C*    banner                                          *
C*  - Added support for lpp1/lpp2 = 2 or 3            *
C*  - Removed again support for different MadEvent    *
C*    processes in different files (no longer         *
C*    necessary with new multiple processes support   *
C*    in MadGraph/MadEvent                            *
C*                                                    *
C*  Version 3.1                                       *
C*  - Added support for different MadEvent processes  *
C*    in different files                              *
C*  - Fixed bug in e+e- collisions                    *
C*                                                    *
C*     Written by J.Alwall, alwall@fyma.ucl.ac.be     *
C*      Earlier versions by S.Mrenna, M.Kirsanov      *
C*                                                    *
C******************************************************
C*                                                    *
C* Instructions:                                      *
C* Please use the common block MGUPPRIV:              *
C* - The logical unit LNHIN must be an opened         *
C*   MadEvent event file                              *
C* - The output unit LNHOUT is by default 6 (std out) *
C* - Set MSCAL to 1 if a dynamical scale is desired   *
C*   for parton showers rather than the one given as  *
C*   factorization scale by MadEvent (otherwise 0)    *
C* - IEVNT gives the number of the event in the event *
C*   file                                             *
C* - ICKKW is set automatically depending on whether  *
C*   the events generated are matched or not          *
C*                                                    *
C******************************************************

C...  UPINIT
C...  Routine called by PYINIT to set up user-defined processes.

      SUBROUTINE INITMADGRAPH

      IMPLICIT NONE
c$$$      CHARACTER*132 CHAR_READ

C...  Pythia parameters.
      INTEGER MSTP,MSTI,MRPY
      DOUBLE PRECISION PARP,PARI,RRPY
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYDATR/MRPY(6),RRPY(100)
      save /pypars/,/pydatr/

C...  User process initialization commonblock.
C #include "GeneratorModules/heprup.inc"
c..   following is already included in include above
c     (../../../InstallArea/include/AtlasProduction/../InstallArea/include/GeneratorModules/GeneratorModules/heprup.inc)
C...User process initialization commonblock.
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &     IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &     LPRUP(MAXPUP)



C...  Extra commonblock to transfer run info.
      INTEGER LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE
      COMMON/MGUPPRIV/LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE

C...  Inputs for the matching algorithm
      double precision etcjet,rclmax,etaclmax,qcut,clfact
      integer maxjets,minjets,iexcfile,ktsche,mektsc,nexcres,excres(30)
      integer nremres, remres(30)
      integer nqmatch,nexcproc,iexcproc(MAXPUP),iexcval(MAXPUP)
      logical nosingrad,showerkt,jetprocs
      common/MEMAIN/etcjet,rclmax,etaclmax,qcut,clfact,maxjets,minjets,
     $   iexcfile,ktsche,mektsc,nexcres,excres,nremres,remres,
     $   nqmatch,nexcproc,iexcproc,iexcval,nosingrad,showerkt,jetprocs

C...  Parameter arrays (local)
      integer maxpara
      parameter (maxpara=1000)
      integer npara,iseed
      character*20 param(maxpara),value(maxpara)

C...  Lines to read in assumed never longer than 200 characters.
      INTEGER MAXLEN,IBEG,IPR,I
      PARAMETER (MAXLEN=200)
      CHARACTER*(MAXLEN) STRING

C...  Functions
      INTEGER iexclusive
      EXTERNAL iexclusive

C...  Set pythia input card and STDHEP output file
      CHARACTER*80 pythia_card

C...  Format for reading lines.
      CHARACTER*6 STRFMT
      STRFMT='(A000)'
      WRITE(STRFMT(3:5),'(I3)') MAXLEN


c************************************************************************
c     Collider energy and type                                          *
c************************************************************************
c     lpp  = -1 (antiproton), 0 (no pdf), 1 (proton)
c     ebeam= energy of each beam in GeV


c$$$C...  Set incoming beams: default LHC.
c       These values should be read directly from the input file
c$$$c...
c$$$      IDBMUP(1)=2212
c$$$      IDBMUP(2)=2212
c$$$      EBMUP(1)=3500D0
c$$$      EBMUP(2)=3500D0
c$$$
c$$$C...  Decide on weighting strategy: unweighted on input.
c$$$      IDWTUP=3
c$$$
c
C--   open the file
      OPEN(UNIT=LNHIN,STATUS='OLD',FILE='events.lhe')

C     Check for new LH interface
c...  use now only the LH interface!!!
      READ(LNHIN,STRFMT) STRING
      IF (STRING(1:17).EQ.'<LesHouchesEvents') THEN
         Print *,'  ==> Version pythia-pgs_V2.1.7 <== '
c         Print *,'LesHouchesEvents format detected: OK  !!!'
      ELSE
         Print *,'Error: not Les Houches format !!!'
         stop
      ENDIF

c     ------------------------------------------------------
C...  Extract the model parameter card and read it.
      CALL MODELPAR(LNHIN)

c...  Read the <init> block information

C...  Loop until finds line beginning with "<init>" or "<init ".
 100  READ(LNHIN,STRFMT,END=130,ERR=130) STRING
C...  Pick out random number seed and use for PYR initialization
      IF(INDEX(STRING,'iseed').NE.0)THEN
         READ(STRING,*) iseed
         IF(iseed.gt.0) THEN
            WRITE(LNHOUT,*) 'Initializing PYR with random seed ',iseed
            MRPY(1) = iseed
            MRPY(2) = 0
         ENDIF
      ENDIF
      IBEG=0
 110  IBEG=IBEG+1
C...  Allow indentation.
      IF(STRING(IBEG:IBEG).EQ.' '.AND.IBEG.LT.MAXLEN-5) GOTO 110
      IF(STRING(IBEG:IBEG+5).NE.'<init>'.AND.
     &     STRING(IBEG:IBEG+5).NE.'<init ') GOTO 100

C...  Read first line of initialization info.
      READ(LNHIN,*,END=130,ERR=130) IDBMUP(1),IDBMUP(2),EBMUP(1),
     &     EBMUP(2),PDFGUP(1),PDFGUP(2),PDFSUP(1),PDFSUP(2),IDWTUP,NPRUP

C...  Read NPRUP subsequent lines with information on each process.
      DO 120 IPR=1,NPRUP
         READ(LNHIN,*,END=130,ERR=130) XSECUP(IPR),XERRUP(IPR),
     &        XMAXUP(IPR),LPRUP(IPR)
 120  CONTINUE

C...  Set PDFLIB or LHAPDF pdf number for Pythia

      IF(PDFSUP(1).NE.19070.AND.(PDFSUP(1).NE.0.OR.PDFSUP(2).NE.0))THEN
c     Not CTEQ5L, which is standard in Pythia
         CALL PYGIVE('MSTP(52)=2') ! use of PDFLIB/LHAPDF
c     The following works for both PDFLIB and LHAPDF (where PDFGUP(1)=0)
c     But note that the MadEvent output uses the LHAPDF numbering scheme
         IF(PDFSUP(1).NE.0)THEN
            MSTP(51)=1000*PDFGUP(1)+PDFSUP(1)
         ELSE
            MSTP(51)=1000*PDFGUP(2)+PDFSUP(2)
         ENDIF
      ENDIF

C...Initialize widths and partial widths for resonances.
      CALL PYINRE

C...  Calculate xsec reduction due to non-decayed resonances
C...  based on first event only!

      CALL BRSUPP

      REWIND(LNHIN)

C...  Extract cuts and matching parameters
      CALL read_params(LNHIN,npara,param,value,maxpara)

      call get_integer(npara,param,value," ickkw ",ickkw,0)
      if(ickkw.eq.1)then
         call get_integer(npara,param,value," ktscheme ",mektsc,1)
         write(*,*)'Running matching with ME ktscheme ',mektsc
      endif


c$$$c..   read beam energies  -->already  read from input file
c$$$      call get_real   (npara,param,value," ebeam1 " ,EBMUP(1),7d3)
c$$$      call get_real   (npara,param,value," ebeam2 " ,EBMUP(2),7d3)

C...  Read pythia input card
      pythia_card=' '
cc      IF(ickkw.gt.0) then
         pythia_card='pythia_card.dat'
         PRINT *
         PRINT *,'Reading pythia input card'
         CALL RPYCARD(pythia_card)
cc      END IF
C
C...  Set kt clustering scheme (if not already set)
C
      IF(ABS(IDBMUP(1)).EQ.11.AND.ABS(IDBMUP(2)).EQ.11.AND.
     $     IDBMUP(1).EQ.-IDBMUP(2).AND.ktsche.EQ.0)THEN
         ktsche=1
      ELSE IF(ktsche.EQ.0) THEN
c     see ktclusdble.f for explanation of ktsche (IMODE)
         ktsche=4313
      ENDIF

C...  Enhance primordial kt
c     CALL PYGIVE('PARP(91)=2.5')
c     CALL PYGIVE('PARP(93)=15')

      IF(ickkw.gt.0.and.(NPRUP.gt.1.or.iexclusive(LPRUP(1)).ne.-1))
     $     CALL set_matching(LNHIN,npara,param,value)

C...  For photon initial states from protons: Set proton not to break up
      CALL PYGIVE('MSTP(98)=1')

C...  Reset event numbering
      IEVNT=0

      RETURN

C...  Error exit: give up if initalization does not work.
 130  WRITE(*,*) ' Failed to read LHEF initialization information.'
      WRITE(*,*) ' Event generation will be stopped.'
      STOP


      RETURN
      END


C************************************************

      SUBROUTINE RPYCARD(pythia_card)

      IMPLICIT NONE

c$$$C...Commonblock to set PDF library path
c$$$      CHARACTER*232 LHAPATH
c$$$      COMMON/LHAPDFC/LHAPATH
c$$$      SAVE /LHAPDFC/

C...Extra commonblock to transfer run info.
      INTEGER LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE
      COMMON/MGUPPRIV/LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE

C...Inputs for jet clustering and Pythia run
      INTEGER NITER,NFILES,PIDCUT(2,10),PIDCUTN(2,10),NPIDCUT
      COMMON/JETPAR/NITER,NFILES,PIDCUT,PIDCUTN,NPIDCUT

c$$$C...GETJET commonblocks
c$$$      INTEGER MNCY,MNCPHI,NCY,NCPHI
c$$$c$$$      INTEGER NJMAX,JETNO,NJJET
c$$$      DOUBLE PRECISION YCMIN,YCMAX,DELY,DELPHI,ET,STHCAL,CTHCAL,CPHCAL,
c$$$     &  SPHCAL
c$$$c$$$      DOUBLE PRECISION PJJET,ETJETJ
c$$$      PARAMETER (MNCY=200)
c$$$      PARAMETER (MNCPHI=200)
c$$$      COMMON/CALOR/DELY,DELPHI,ET(MNCY,MNCPHI),
c$$$     $CTHCAL(MNCY),STHCAL(MNCY),CPHCAL(MNCPHI),SPHCAL(MNCPHI),
c$$$     $YCMIN,YCMAX,NCY,NCPHI

C...Commonblock to transfer event-by-event matching info
      INTEGER NLJETS,IEXC,Ifile
      DOUBLE PRECISION PTCLUS
      COMMON/MEMAEV/PTCLUS(20),NLJETS,IEXC,Ifile

C...Inputs for the matching algorithm
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      double precision etcjet,rclmax,etaclmax,qcut,clfact
      integer maxjets,minjets,iexcfile,ktsche,mektsc,nexcres,excres(30)
      integer nremres, remres(30)
      integer nqmatch,nexcproc,iexcproc(MAXPUP),iexcval(MAXPUP)
      logical nosingrad,showerkt,jetprocs
      common/MEMAIN/etcjet,rclmax,etaclmax,qcut,clfact,maxjets,minjets,
     $   iexcfile,ktsche,mektsc,nexcres,excres,nremres,remres,
     $   nqmatch,nexcproc,iexcproc,iexcval,nosingrad,showerkt,jetprocs

      INTEGER ios, i
      CHARACTER*80 pythia_card
      CHARACTER*132 line

      if (pythia_card.ne.' ') then
        open (85, file=pythia_card, status='old',
     .     form='formatted', ERR=999)    ! removed for Linux: readonly)
        ios = 0
        do while(ios.eq.0)
          read(85,fmt='(a)',iostat=ios) line
          if (ios.eq.0) then
             call remove_comments(line,132)
c$$$             if(index(line,'lhapath').ne.0.or.
c$$$     $           index(line,'LHAPATH').ne.0) then
c$$$                LHAPATH=line(index(line,'=')+1:)
c$$$                write(*,*) 'Set LHAPATH to '//LHAPATH
c$$$                cycle
c$$$             endif
             call downcase_line(line,132)
             if(index(line,'psscale').ne.0) then
                READ(line(index(line,'=')+1:),*,err=100) MSCAL
                write(*,*) 'Set scale choice to ',MSCAL
                if(MSCAL.eq.0) write(*,*) ' (fixed scale)'
                if(MSCAL.gt.0) write(*,*) ' (Herwig scale)'
             else if(index(line,'iexcfile').ne.0) then
                READ(line(index(line,'=')+1:),*,err=100) IEXCFILE
                if(IEXCFILE.eq.0)
     $             write(*,*) ' Assuming inclusive sample ',
     $             '(IEXCFILE=0)'
                if(IEXCFILE.gt.0)
     $             write(*,*) ' Assuming exclusive sample',
     $             '(IEXCFILE=',IEXCFILE,')'
             else if(index(line,'qcut').ne.0) then
                READ(line(index(line,'=')+1:),*,err=100) qcut
                write(*,*) 'Read Qcut   = ',qcut
             else if(index(line,'etcjet').ne.0) then
                READ(line(index(line,'=')+1:),*,err=100) etcjet
                write(*,*) 'Read Etcjet   = ',etcjet
             else if(index(line,'clfact').ne.0) then
                READ(line(index(line,'=')+1:),*,err=100) clfact
                write(*,*) 'Read clfact = ',clfact
c$$$             else if(index(line,'ncy').ne.0) then
c$$$                READ(line(index(line,'=')+1:),*,err=100) ncy
c$$$                write(*,*) 'Read NCY = ',ncy
c$$$             else if(index(line,'ncphi').ne.0) then
c$$$                READ(line(index(line,'=')+1:),*,err=100) ncphi
c$$$                write(*,*) 'Read NCPHI = ',ncphi
c$$$             else if(index(line,'niter').ne.0) then
c$$$                READ(line(index(line,'=')+1:),*,err=100) niter
c$$$                write(*,*) 'Read Niter = ',niter
c$$$             else if(index(line,'nfiles').ne.0) then
c$$$                READ(line(index(line,'=')+1:),*,err=100) nfiles
c$$$                write(*,*) 'Read Nfiles = ',nfiles
c$$$             else if(index(line,'ifile').ne.0) then
c$$$                READ(line(index(line,'=')+1:),*,err=100) ifile
c$$$                write(*,*) 'Read Ifile = ',ifile
             else if(index(line,'ktsche').ne.0) then
                READ(line(index(line,'=')+1:),*,err=100) ktsche
                write(*,*) 'Read Ktsche = ',ktsche
             else if(index(line,'iscale').ne.0) then
                READ(line(index(line,'=')+1:),*,err=100) ISCALE
                write(*,*) 'Read ISCALE = ',ISCALE
             else if(index(line,'maxjets').ne.0) then
                READ(line(index(line,'=')+1:),*,err=100) MAXJETS
                write(*,*) 'Read MAXJETS = ',MAXJETS
             else if(index(line,'minjets').ne.0) then
                READ(line(index(line,'=')+1:),*,err=100) MINJETS
                write(*,*) 'Read MINJETS = ',MINJETS
             else if(index(line,'excres').ne.0) then
                if(nexcres.ge.30)then
                   write(*,*) 'Too many excluded resonances.'
                   cycle
                endif
                nexcres=nexcres+1
                READ(line(index(line,'=')+1:),*,err=100) EXCRES(nexcres)
                write(*,*) 'Read EXCRES = ',EXCRES(nexcres)
             else if(index(line,'remres').ne.0) then
                if(nremres.ge.30)then
                   write(*,*) 'Too many excluded resonances.'
                   cycle
                endif
                nremres=nremres+1
                READ(line(index(line,'=')+1:),*,err=100) REMRES(nremres)
                write(*,*) 'Read REMRES = ',REMRES(nremres)
             else if(index(line,'nosingrad').ne.0) then
                READ(line(index(line,'=')+1:),*,err=100) nosingrad
                write(*,*) 'Read nosingrad = ',nosingrad
             else if(index(line,'showerkt').ne.0) then
                READ(line(index(line,'=')+1:),*,err=100) showerkt
                write(*,*) 'Read showerkt = ',showerkt
             else if(index(line,'nqmatch').ne.0) then
                READ(line(index(line,'=')+1:),*,err=100) nqmatch
                write(*,*) 'Read nqmatch = ',nqmatch
             else if(index(line,'iexcproc').ne.0) then
                nexcproc=nexcproc+1
                if(nexcproc.le.MAXPUP) then
                   READ(line(index(line,'(')+1:),*,err=100)
     $                  iexcproc(nexcproc)
                   READ(line(index(line,'=')+1:),*,err=100)
     $                  iexcval(nexcproc)
                   write(*,*) 'Read iexcproc(',nexcproc,')=',
     $                  iexcproc(nexcproc),' and iexcval=',
     $                  iexcval(nexcproc)
                else
                   write(*,*) 'Too many iexcproc definitions.'
                endif
             else if(index(line,'pidcut').ne.0) then
                npidcut=npidcut+1
                if(npidcut.le.10) then
                   if(index(line,'-').gt.0)then
                      READ(line(index(line,'(')+1:index(line,'-')-1),
     $                     *,err=100) pidcut(1,npidcut)
                      READ(line(index(line,'-')+1:),*,err=100)
     $                     pidcut(2,npidcut)
                   else
                      READ(line(index(line,'(')+1:),*,err=100)
     $                     pidcut(1,npidcut)
                      pidcut(2,npidcut)=pidcut(1,npidcut)
                   endif
                   READ(line(index(line,'=')+1:),*,err=100)
     $                  pidcutn(1,npidcut),pidcutn(2,npidcut)
                   write(*,*) 'Read pidcut(',npidcut,')=',
     $                  pidcut(1,npidcut),'-',pidcut(2,npidcut),
     $                  ' with min=',pidcutn(1,npidcut),
     $                  ' and max=',pidcutn(2,npidcut)
                else
                   write(*,*) 'Too many iexcproc definitions.'
                endif
             else if(line.ne.' ') then
                call pygive(line)
             endif
          endif
          cycle
 100      write(*,*) 'Failed to read line: ',line(1:len_trim(line))
       enddo
       close(85)
      endif
      RETURN
 999  if(ickkw .gt.0) then
         write(6,*)
     &        " ===> No file pythia_card.dat, but ickkw > 0 ==> STOP"
         stop
      else
         write(6,*) " ===> No file pythia_card.dat "
      end if
      RETURN
      END

C************************************************

      subroutine remove_comments(line,len)

      implicit none
      character*(*) line
      integer i,len

      logical comment

      comment = .false.
      if(line(1:1).eq.'#') comment = .true.
      do i=1,len
        if(line(i:i).eq.'!') comment = .true.
        if(comment) line(i:i) = ' '
      enddo

      return
      end
C*********************************************************************


      subroutine case_trap2(name,n)
c**********************************************************
c   change the string to lowercase if the input is not
c**********************************************************
      implicit none
c
c   ARGUMENT
c
      character(*) name
      integer n
c
c   LOCAL
c
      integer i,k

      do i=1,n
        k=ichar(name(i:i))
        if(k.ge.65.and.k.le.90) then !upper case A-Z
          k=ichar(name(i:i))+32
          name(i:i)=char(k)
        endif
      enddo

      return
      end


C*********************************************************************


C*********************************************************************

      subroutine BRSUPP

      IMPLICIT NONE

C...Three Pythia functions return integers, so need declaring.
      INTEGER PYCOMP,MWID
      DOUBLE PRECISION WIDS

C...Resonance width and secondary decay treatment.
      COMMON/PYINT4/MWID(500),WIDS(500,5)

C...User process initialization commonblock.
C #include "GeneratorModules/heprup.inc"
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &   IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &   LPRUP(MAXPUP)
C...User process event common block.
C #include "GeneratorModules/hepeup.inc"
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &   ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &   VTIMUP(MAXNUP),SPINUP(MAXNUP)

C...Extra commonblock to transfer run info.
      INTEGER LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE
      COMMON/MGUPPRIV/LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE

      INTEGER I,J,IBEG
      INTEGER IPR,isup
      DOUBLE PRECISION SUPPCS
      LOGICAL SUPDONE(MAXPUP)
      DATA SUPDONE/MAXPUP*.false./

C...Lines to read in assumed never longer than 200 characters.
      INTEGER MAXLEN
      PARAMETER (MAXLEN=200)
      CHARACTER*(MAXLEN) STRING

C...Format for reading lines.
      CHARACTER*6 STRFMT
      STRFMT='(A000)'
      WRITE(STRFMT(3:5),'(I3)') MAXLEN

C...Loop until finds line beginning with "<event>" or "<event ".
  100 READ(LNHIN,STRFMT,END=130,ERR=130) STRING
      IBEG=0
  110 IBEG=IBEG+1
C...Allow indentation.
      IF(STRING(IBEG:IBEG).EQ.' '.AND.IBEG.LT.MAXLEN-6) GOTO 110
      IF(STRING(IBEG:IBEG+6).NE.'<event>'.AND.
     &STRING(IBEG:IBEG+6).NE.'<event ') GOTO 100

C...Read first line of event info.
      READ(LNHIN,*,END=130,ERR=130) NUP,IDPRUP,XWGTUP,SCALUP,
     &AQEDUP,AQCDUP

      do IPR=1,NPRUP
         if(IDPRUP .eq. LPRUP(IPR))then
            isup=IPR
            exit
         endif
      enddo

      if(SUPDONE(isup)) GOTO 100

C...Read NUP subsequent lines with information on each particle.
      DO 120 I=1,NUP
        READ(LNHIN,*,END=130,ERR=130) IDUP(I),ISTUP(I),
     &  MOTHUP(1,I),MOTHUP(2,I),ICOLUP(1,I),ICOLUP(2,I),
     &  (PUP(J,I),J=1,5),VTIMUP(I),SPINUP(I)
  120 CONTINUE

      SUPPCS=1.
      do I=3,NUP
        if (ISTUP(I).EQ.1.AND.(IABS(IDUP(I)).GE.23.OR.
     $     (IABS(IDUP(I)).GE.6.AND.IABS(IDUP(I)).LE.8)))
     $     THEN
          WRITE(LNHOUT,*) 'Resonance ',IDUP(I), ' has BRTOT ',
     $       wids(PYCOMP(IDUP(I)),2)
          if(wids(PYCOMP(IDUP(I)),2).lt.0.95) then
            SUPPCS=SUPPCS*wids(PYCOMP(IDUP(I)),2)
          endif
        endif
      enddo
      if(SUPPCS.gt.0)then
         write(*,*)'Multiplying cross section for process ',
     $        IDPRUP,' by ',SUPPCS
         XSECUP(isup)=XSECUP(isup)*SUPPCS
      else
         write(*,*) 'Warning! Got cross section suppression 0 ',
     $        'for process ',IDPRUP
         write(*,*) 'No cross section reduction done'
      endif

      SUPDONE(isup)=.true.
      do i=1,NPRUP
         if(.not. SUPDONE(i)) GOTO 100
      enddo

 130  RETURN
      END

C*********************************************************************

C...modelpar
C...Checks if model is mssm and extracts SLHA file
C...Reads all particle masses and SM parameters in any case

      SUBROUTINE MODELPAR(iunit)

      IMPLICIT NONE

C...Three Pythia functions return integers, so need declaring.
      INTEGER IMSS
      DOUBLE PRECISION RMSS
C...Supersymmetry parameters.
      COMMON/PYMSSM/IMSS(0:99),RMSS(0:99)
C...Pythia common blocks
      INTEGER MSTU,MSTJ,KCHG
      DOUBLE PRECISION PARU,PARJ,PMAS,PARF,VCKM
C...Parameters.
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
C...Particle properties + some flavour parameters.
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
C...Inputs for the matching algorithm
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      double precision etcjet,rclmax,etaclmax,qcut,clfact
      integer maxjets,minjets,iexcfile,ktsche,mektsc,nexcres,excres(30)
      integer nremres, remres(30)
      integer nqmatch,nexcproc,iexcproc(MAXPUP),iexcval(MAXPUP)
      logical nosingrad,showerkt,jetprocs
      common/MEMAIN/etcjet,rclmax,etaclmax,qcut,clfact,maxjets,minjets,
     $   iexcfile,ktsche,mektsc,nexcres,excres,nremres,remres,
     $   nqmatch,nexcproc,iexcproc,iexcval,nosingrad,showerkt,jetprocs

C...Local variables
      CHARACTER*132 buff,block_name
      CHARACTER*8 model
      INTEGER iunit,ivalue
      DOUBLE PRECISION value
      LOGICAL block_found
      INTEGER i,ifail

      buff=' '
      do 100 while(buff.ne.'</slha>' .and.
     $     buff(1:21).ne.'# End param_card.dat')
        read(iunit,'(a132)',end=105,err=98) buff

        if(buff.eq.'<slha>' .or.
     $       buff(1:23).eq.'# Begin param_card.dat')then
c       Write out the SLHA file to unit 24
          open(24,status='scratch')
          do while(.true.)
            read(iunit,'(a132)',end=99,err=98) buff
            if(buff.eq.'</slha>' .or.
     $           buff(1:21).eq.'# End param_card.dat') goto 105
            write(24,'(a80)') buff
          end do
        endif

        call case_trap2(buff,len_trim(buff))
c     Find and store model used
        if(buff(1:14).eq.'# begin model')then
c  change to avoid compiler warnings
c          read(iunit,'(a132)',end=99,err=98) buff
c          model=buff
          read(iunit,'(a8)',end=99,err=98) model
        endif
c     Find and store QED or HIG order used
c        if(index(buff,'qed').gt.0.or.index(buff,'hig').gt.0)then
c          read(buff(index(buff,'=')+1:),*,err=100) ivalue
c          nosingrad=nosingrad.or.(ivalue.ge.2)
c          if(nosingrad) print *,'Set nosingrad to .true.'
c        endif
 100  continue
 105  continue
      REWIND(iunit)
      REWIND(24)


C...Read the SLHA file
      block_found=.false.
      do 200 while(.true.)
        read(24,'(a132)',end=205,err=98) buff
        call case_trap2(buff,len_trim(buff))
c      Look for "block" to find SM and mass parameters
         if(buff(1:1).eq.'b')then
            block_name=buff(7:)
            block_found=.true.
         endif
         if (block_found) then
            do 10 while(.true.)
               read(24,'(a132)',end=205,err=98) buff
               if(buff(1:1).eq.'#') goto 10
               if(buff(1:1).ne.' ') then
                  block_found=.false.
                  backspace(24)
                  goto 200
               endif
               if(block_name(1:8).eq.'sminputs')then
                  read(buff,*) ivalue,value
                  print *,'Reading parameter ',block_name(1:8),
     $                 ivalue,value
                  if(ivalue.eq.1) PARU(103)=1d0/value
                  if(ivalue.eq.2) PARU(105)=value
                  if(ivalue.eq.4) PMAS(23,1)=value
                  if(ivalue.eq.6) PMAS(6,1)=value
                  if(ivalue.eq.7) PMAS(15,1)=value
               endif
 10         continue
         endif
 200  continue
 205  continue
      PARU(102)  = 0.5d0-sqrt(0.25d0-
     $     PARU(1)/sqrt(2d0)*PARU(103)/PARU(105)/PMAS(23,1)**2)
      REWIND(24)

      write(*,*) 'Reading model: ',model

c      open(24,FILE='SLHA.dat',ERR=91)
c     Pick out SM parameters
c      CALL READSMLHA(iunit)

cccc      write(*,*) ' index(model) = ', index(model,'mssm')

      if(index(model,'mssm').ne.0) then
         call PYGIVE('IMSS(1) = 11')
         CALL PYSLHA(1,0,IFAIL)
      endif
      call PYGIVE('IMSS(21)= 24') ! Logical unit number of SLHA spectrum file
      if(model(1:2).ne.'sm'.and.model(1:4).ne.'mssm') then
         call PYGIVE('IMSS(22)= 24') ! Logical unit number of SLHA decay file
c     Let Pythia read all new particles ("qnumbers")
         CALL PYSLHA(0,0,IFAIL)
      endif
c     Let Pythia read all masses and, if possible, decays
      CALL PYSLHA(5,0,IFAIL)
      CALL PYSLHA(2,0,IFAIL)
c     Stop Pythia from reading particles and decays a second time
      if(model(1:2).ne.'sm'.and.model(1:4).ne.'mssm') then
         call PYGIVE('IMSS(21)= 0') ! Logical unit number of SLHA spectrum file
         call PYGIVE('IMSS(22)= 0') ! Logical unit number of SLHA decay file
      endif
      RETURN

 90   WRITE(*,*)'Could not open file SLHA.dat for writing'
      WRITE(*,*)'Quitting...'
      STOP
 98   WRITE(*,*)'Unexpected error reading file'
      WRITE(*,*)'Quitting...'
      STOP
 99   WRITE(*,*)'Unexpected end of file'
      WRITE(*,*)'Quitting...'
      STOP

      END

c----------------------------------------------------------------------
c   READ_PARAMS
c   Read the parameters from the run_card in the MadEvent event file
c----------------------------------------------------------------------

      subroutine read_params(iunit,npara,param,value,maxpara)
      implicit none

c
c   arguments
c
      integer iunit
      character*20 param(*),value(*)
      integer npara,maxpara

C...User process initialization commonblock.
C #include "GeneratorModules/heprup.inc"
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &   IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &   LPRUP(MAXPUP)
C...User process event common block.
C #include "GeneratorModules/hepeup.inc"
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &   ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &   VTIMUP(MAXNUP),SPINUP(MAXNUP)

c
c   local
c
      logical fopened
      character*20 ctemp
      integer k,i,l1,l2,j,jj
      character*132 buff
      integer NPARTS,KNTEV,NJETS,MAXNJ,NREAD
      parameter(MAXNJ=6)
      double precision WTEVNT,XSTOT(MAXNJ),DUMMY,WTMAX
c
c----------
c   start
c----------

      NREAD=0

c
c   read the input-card.dat
c
      npara=1
      param(1)=' '
      value(1)=' '
      WTMAX=0D0
c
c   read in values
c
      buff=' '
      do while(index(buff,'<MGRunCard>').eq.0 .and.
     $     index(buff,'Begin run_card.dat').eq.0)
        read(iunit,'(a132)',end=99,err=99) buff
      enddo
      do 10 while(index(buff,'</MGRunCard>').eq.0 .and.
     $     index(buff,'End run_card.dat').eq.0 .and.
     $     npara.le.maxpara)
        read(iunit,'(a132)',end=99,err=99) buff
        if(buff.eq.' ') then
          goto 10
        endif
        if(index(buff,'=').ne.0) then
          l1=index(buff,'=')
          l2=index(buff,'!')
          if(l2.eq.0) l2=l1+20  !maybe there is no comment...
c
          value(npara)=buff(1:l1-1)
          ctemp=value(npara)
          call case_trap2(ctemp,20)
          value(npara)=ctemp
c
          param(npara)=" "//buff(l1+1:l2-1)
          ctemp=param(npara)
          call case_trap2(ctemp,20)
          param(npara)=ctemp
c
          npara=npara+1
        endif
 10   continue

      REWIND(iunit)

      return

 99   WRITE(*,*)'Unexpected error reading file'
      WRITE(*,*)'Quitting...'
      STOP
      end

C*********************************************************************
C...set_matching
C...Sets parameters for the matching, i.e. cuts and jet multiplicities
C*********************************************************************

      SUBROUTINE set_matching(iunit,npara,param,value)
      implicit none
c
c   arguments
c
      integer iunit,npara
      character*20 param(*),value(*)

C...Pythia parameters.
      INTEGER MSTP,MSTI
      DOUBLE PRECISION PARP,PARI
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)

C...User process initialization commonblock.
C #include "GeneratorModules/heprup.inc"
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &   IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &   LPRUP(MAXPUP)

C...User process event common block.
C #include "GeneratorModules/hepeup.inc"
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &   ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &   VTIMUP(MAXNUP),SPINUP(MAXNUP)

C...Extra commonblock to transfer run info.
      INTEGER LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE
      COMMON/MGUPPRIV/LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE

C...Inputs for the matching algorithm
      double precision etcjet,rclmax,etaclmax,qcut,clfact
      integer maxjets,minjets,iexcfile,ktsche,mektsc,nexcres,excres(30)
      integer nremres, remres(30)
      integer nqmatch,nexcproc,iexcproc(MAXPUP),iexcval(MAXPUP)
      logical nosingrad,showerkt,jetprocs
      common/MEMAIN/etcjet,rclmax,etaclmax,qcut,clfact,maxjets,minjets,
     $   iexcfile,ktsche,mektsc,nexcres,excres,nremres,remres,
     $   nqmatch,nexcproc,iexcproc,iexcval,nosingrad,showerkt,jetprocs

C...Commonblock to transfer event-by-event matching info
      INTEGER NLJETS,IEXC,Ifile
      DOUBLE PRECISION PTCLUS
      COMMON/MEMAEV/PTCLUS(20),NLJETS,IEXC,Ifile

C...Local variables
      INTEGER I,MAXNJ,NREAD,MINJ,MAXJ
      parameter(MAXNJ=6)
      DOUBLE PRECISION XSTOT(MAXNJ),XSECTOT
      DOUBLE PRECISION ptjmin,etajmax,drjmin,ptbmin,etabmax,xqcut

C...Functions
      INTEGER iexclusive
      EXTERNAL iexclusive

C...Need lower scale for final state radiation in e+e-
      IF(IABS(IDBMUP(1)).EQ.11.AND.IABS(IDBMUP(2)).EQ.11) then
        CALL PYGIVE('PARP(71)=1')
      ENDIF

C...CRUCIAL FOR JET-PARTON MATCHING: CALL UPVETO, ALLOW JET-PARTON MATCHING
      call pygive('MSTP(143)=1')

C
C...Check jet multiplicities and set processes
C
      DO I=1,MAXNJ
        XSTOT(I)=0D0
      ENDDO
      MINJ=MAXNJ
      MAXJ=0
      NREAD=0
      DO WHILE(.true.)
        CALL UPEVNT()
        IF(NUP.eq.0) goto 20
        IF(IEXC.EQ.-1) cycle
        if(NLJETS.GT.MAXJ) MAXJ=NLJETS
        if(NLJETS.LT.MINJ) MINJ=NLJETS
c        XSTOT(NLJETS+1)=XSTOT(NLJETS+1)+XWGTUP
        XSTOT(NLJETS+1)=XSTOT(NLJETS+1)+1
        NREAD=NREAD+1
      ENDDO

 20   continue

      REWIND(iunit)

      write(LNHOUT,*) 'Minimum number of jets in file: ',MINJ
      write(LNHOUT,*) 'Maximum number of jets in file: ',MAXJ

      XSECTOT=0d0
      DO I=1,NPRUP
         XSECTOT=XSECTOT+XSECUP(I)
      ENDDO

      IF(NPRUP.eq.1.AND.MINJ.lt.MAXJ)THEN
C...If different process ids not set by user, set by jet number
         jetprocs=.true.
         IF(IEXCFILE.eq.0.AND.iexclusive(LPRUP(1)).ne.1) THEN
            nexcproc=1
            IEXCPROC(1)=MAXJ-MINJ
            IEXCVAL(1)=0
         ENDIF
         NPRUP=1+MAXJ-MINJ
         DO I=MINJ,MAXJ
            XSECUP(1+I-MINJ) = XSECTOT*XSTOT(I+1)/NREAD
            XMAXUP(1+I-MINJ) = XMAXUP(1)
            LPRUP(1+I-MINJ)  = I-MINJ
         ENDDO
      ELSE IF(IEXCFILE.EQ.0) THEN
C...Check if any IEXCPROC set, then set IEXCFILE=1
         DO I=1,NPRUP
            IF(iexclusive(LPRUP(I)).EQ.0) IEXCFILE=1
         ENDDO
      ENDIF

      WRITE(LNHOUT,*) ' Number of Events Read:: ',NREAD
      WRITE(LNHOUT,*) ' Total cross section (pb):: ',XSECTOT
      WRITE(LNHOUT,*) ' Process   Cross Section (pb):: '
      DO I=1,NPRUP
        WRITE(LNHOUT,'(I5,E23.5)') I,XSECUP(I)
      ENDDO

      IF(MINJETS.EQ.-1) MINJETS=MINJ
      IF(MAXJETS.EQ.-1) MAXJETS=MAXJ
      write(LNHOUT,*) 'Minimum number of jets allowed: ',MINJETS
      write(LNHOUT,*) 'Maximum number of jets allowed: ',MAXJETS
      write(LNHOUT,*) 'IEXCFILE = ',IEXCFILE
      write(LNHOUT,*) 'jetprocs = ',jetprocs
      DO I=1,NPRUP
         write(LNHOUT,*) 'IEXCPROC(',LPRUP(I),') = ',
     $        iexclusive(LPRUP(I))
      ENDDO

      CALL FLUSH()

C...Run PYPTFS instead of PYSHOW
c        CALL PYGIVE("MSTJ(41)=12")

c***********************************************************************
c   Read in jet cuts
c***********************************************************************

      call get_real   (npara,param,value," ptj " ,ptjmin,7d3)
      call get_real   (npara,param,value," etaj " ,etajmax,7d3)
      call get_real   (npara,param,value," ptb " ,ptbmin,7d3)
      call get_real   (npara,param,value," etab " ,etabmax,7d3)
      call get_real   (npara,param,value," drjj " ,drjmin,7d3)
      call get_real   (npara,param,value," xqcut " ,xqcut,0d0)

      if(qcut.lt.xqcut) then
         if(showerkt) then
            qcut=xqcut
         else
            qcut=max(xqcut*1.2,xqcut+5)
         endif
      endif
      if(xqcut.le.0)then
         write(*,*) 'Warning! ME generation QCUT = 0. QCUT set to 0!'
         qcut=0
      endif

c     etajmax=min(etajmax,etabmax)
c     ptjmin=max(ptjmin,ptbmin)

c     IF(ICKKW.EQ.1) THEN
c     WRITE(*,*) ' '
c     WRITE(*,*) 'INPUT 0 FOR INCLUSIVE JET SAMPLE, 1 FOR EXCLUSIVE'
c     WRITE(*,*) '(SELECT 0 FOR HIGHEST PARTON MULTIPLICITY SAMPLE)'
c     WRITE(*,*) '(SELECT 1 OTHERWISE)'
c     READ(*,*) IEXCFILE
c     ENDIF

C     INPUT PARAMETERS FOR CONE ALGORITHM

      IF(ETCJET.LE.PTJMIN)THEN
         ETCJET=MAX(PTJMIN+5,1.2*PTJMIN)
      ENDIF

      RCLMAX=DRJMIN
      ETACLMAX=ETAJMAX
      IF(qcut.le.0)THEN
         WRITE(*,*) 'JET CONE PARAMETERS FOR MATCHING:'
         WRITE(*,*) 'ET>',ETCJET,' R=',RCLMAX
         WRITE(*,*) 'DR(PARTON-JET)<',1.5*RCLMAX
         WRITE(*,*) 'ETA(JET)<',ETACLMAX
      ELSE IF(ickkw.eq.1) THEN
         WRITE(*,*) 'KT JET PARAMETERS FOR MATCHING:'
         WRITE(*,*) 'QCUT=',qcut
         WRITE(*,*) 'ETA(JET)<',ETACLMAX
         WRITE(*,*) 'Note that in ME generation, qcut = ',xqcut
         if(showerkt.and.MSTP(81).LT.20)THEN
            WRITE(*,*)'WARNING: "shower kt" needs pT-ordered showers'
            WRITE(*,*)'         Setting MSTP(81)=',20+MOD(MSTP(81),10)
            MSTP(81)=20+MOD(MSTP(81),10)
         endif
      else if(ickkw.eq.2)then
c     Turn off color coherence suppressions (leave this to ME)
         CALL PYGIVE('MSTP(62)=2')
         CALL PYGIVE('MSTP(67)=0')
         if(MSTP(81).LT.20)THEN
            WRITE(*,*)'WARNING: Must run CKKW with pt-ordered showers'
            WRITE(*,*)'         Setting MSTP(81)=',20+MOD(MSTP(81),10)
            MSTP(81)=20+MOD(MSTP(81),10)
         endif
      endif
      return
      end

      subroutine get_real(npara,param,value,name,var,def_value)
c----------------------------------------------------------------------------------
c   finds the parameter named "name" in param and associate to "value" in value
c----------------------------------------------------------------------------------
      implicit none

c
c   arguments
c
      integer npara
      character*20 param(*),value(*)
      character*(*)  name
      real*8 var,def_value
c
c   local
c
      logical found
      integer i
c
c   start
c
      i=1
      found=.false.
      do while(.not.found.and.i.le.npara)
        found = (index(param(i),name).ne.0)
        if (found) read(value(i),*) var
c     if (found) write (*,*) name,var
        i=i+1
      enddo
      if (.not.found) then
        write (*,*) "Warning: parameter ",name," not found"
        write (*,*) "         setting it to default value ",def_value
        var=def_value
      else
        write(*,*),'Found parameter ',name,var
      endif
      return

      end
c

      subroutine get_integer(npara,param,value,name,var,def_value)
c----------------------------------------------------------------------------------
c   finds the parameter named "name" in param and associate to "value" in value
c----------------------------------------------------------------------------------
      implicit none
c
c   arguments
c
      integer npara
      character*20 param(*),value(*)
      character*(*)  name
      integer var,def_value
c
c   local
c
      logical found
      integer i
c
c   start
c
      i=1
      found=.false.
      do while(.not.found.and.i.le.npara)
        found = (index(param(i),name).ne.0)
        if (found) read(value(i),*) var
c     if (found) write (*,*) name,var
        i=i+1
      enddo
      if (.not.found) then
        write (*,*) "Warning: parameter ",name," not found"
        write (*,*) "         setting it to default value ",def_value
        var=def_value
      else
        write(*,*)'Found parameter ',name,var
      endif
      return

      end

C***********************************
C Common block initialization block
C***********************************

      BLOCK DATA MEPYDAT

      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
C...Inputs for the matching algorithm
      double precision etcjet,rclmax,etaclmax,qcut,clfact
      integer maxjets,minjets,iexcfile,ktsche,mektsc,nexcres,excres(30)
      integer nremres, remres(30)
      integer nqmatch,nexcproc,iexcproc(MAXPUP),iexcval(MAXPUP)
      logical nosingrad,showerkt,jetprocs
      common/MEMAIN/etcjet,rclmax,etaclmax,qcut,clfact,maxjets,minjets,
     $   iexcfile,ktsche,mektsc,nexcres,excres,nremres,remres,
     $   nqmatch,nexcproc,iexcproc,iexcval,nosingrad,showerkt,jetprocs

c$$$C...GETJET commonblocks
c$$$      INTEGER MNCY,MNCPHI,NCY,NCPHI,NJMAX,JETNO,NCJET
c$$$      DOUBLE PRECISION YCMIN,YCMAX,DELY,DELPHI,ET,STHCAL,CTHCAL,CPHCAL,
c$$$     &  SPHCAL,PCJET,ETJET
c$$$      PARAMETER (MNCY=200)
c$$$      PARAMETER (MNCPHI=200)
c$$$      COMMON/CALORM/DELY,DELPHI,ET(MNCY,MNCPHI),
c$$$     $CTHCAL(MNCY),STHCAL(MNCY),CPHCAL(MNCPHI),SPHCAL(MNCPHI),
c$$$     $YCMIN,YCMAX,NCY,NCPHI

C...Extra commonblock to transfer run info.
      INTEGER LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE
      COMMON/MGUPPRIV/LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE

C...Initialization statements
      DATA qcut,clfact,etcjet/0d0,0d0,0d0/
      DATA ktsche,mektsc,maxjets,minjets,nexcres/0,1,-1,-1,0/
      DATA nqmatch/5/
      DATA nexcproc/0/
      DATA iexcproc/MAXPUP*-1/
      DATA iexcval/MAXPUP*-2/
      DATA nosingrad,showerkt,jetprocs/.false.,.false.,.false./

c$$$      DATA NCY,NCPHI/50,60/

      DATA LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE/77,6,0,0,0,1/

      END
C************************************************

      subroutine downcase_line(line,len)

      implicit none
      character*(*) line
      integer i,len,k

      do i=1,len
        k=ichar(line(i:i))
        if(k.ge.65.and.k.le.90) then !upper case A-Z
          k=ichar(line(i:i))+32
          line(i:i)=char(k)
        endif
      enddo

      return
      end
