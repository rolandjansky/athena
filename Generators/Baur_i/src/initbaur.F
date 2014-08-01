      
      SUBROUTINE INITBAUR
 
      IMPLICIT NONE

#include "Baur_i/madsho.inc"
C--GUP common block
#include "GeneratorFortranCommon/heprup.inc"

C...Pythia parameters. 
      INTEGER MSTP,MSTI
      DOUBLE PRECISION PARP,PARI
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)

C...Extra commonblock to transfer run info.
      INTEGER MODE,LNHIN,LNHOUT,NREAD,MSCAL
      REAL*8 SCALEF
      COMMON/BAUPPRIV/SCALEF,LNHIN,NREAD,LNHOUT,MODE,MSCAL
      DATA LNHIN,LNHOUT,MODE,MSCAL/77,6,0,1/
      SAVE/BAUPPRIV/

C...BRSUPP common block
      INTEGER NRES,IDRES
      REAL PEREVENT
      common/PARTCOMP/NRES,IDRES(100),PEREVENT(100)

C...Parameter arrays
      integer maxpara
      parameter (maxpara=100)
      integer mpdf
c      character*20 param(maxpara),value(maxpara)      

C...Local variables
      REAL*8 sf1,sf2
      INTEGER I,IBEAM(2)
      CHARACTER*20 PDFIN
      LOGICAL fixed_fac_scale

C...Set incoming beams: LHC.
      IDBMUP(1)=2212
      IDBMUP(2)=2212
      EBMUP(1)=7000D0
      EBMUP(2)=7000D0

C...Set PDF's of incoming beams: CTEQ 5L.
C...Note that Pythia will not look at PDFGUP and PDFSUP.  
      
C...If you want Pythia to use PDFLIB, you have to set it by hand.
C...(You also have to ensure that the dummy routines
C...PDFSET, STRUCTM and STRUCTP in Pythia are not linked.)      
C      MSTP(52)=2
C      MSTP(51)=1000*PDFGUP(1)+PDFSUP(1)

C...Decide on weighting strategy: unweighted on input.
C      IDWTUP=MODE
      IDWTUP=3

C--   open the file
      OPEN(UNIT=LNHIN,STATUS='OLD',FILE='baur.unweighted_events')

C...Resonants for BRSUPP suppression of cross-section
      NRES=5
      IDRES(1)=23
      IDRES(2)=24
      IDRES(3)=6
      IDRES(4)=25
      IDRES(5)=15
      do i=1,NRES
        PEREVENT(I)=0
      enddo

C...Number of external processes. 
      NPRUP=1

C...Extract the model parameter card and read it.
c      CALL MSSMPAR(LNHIN)

C...Read through data file to get run parameter settings
C...The reading routines taken from MadEvent, file rw_para.f

c      CALL read_params(LNHIN,npara,param,value)
c************************************************************************
c   PDF CHOICE                                                        *  
c************************************************************************
C...Note that in order to use the most modern and common pdfs, LHAPDF
C...must be installed

      
c      call get_string (npara,param,value," pdlabel ",PDFIN,'cteq6l1')
c      WRITE(LNHOUT,*) 'Trying LHAPDF/PDFlib for PDF ',PDFIN
      PDFIN='cteq6l1'
      call get_lhapdf(PDFIN,MPDF)
      
      IF(MPDF.NE.-1)THEN
         PDFGUP(1)=MPDF/10000
         PDFSUP(1)=MOD(MPDF,10000)
         PDFGUP(2)=PDFGUP(1)
         PDFSUP(2)=PDFSUP(1)
      ELSE
         WRITE(*,*) 'Error: PDF ',PDFIN,' not found. Quitting.'
         STOP
      ENDIF
      
c************************************************************************
c   Renormalization and factorization scales                          *
c************************************************************************
c   If the following flags to .false. then event-by-event
c   scale choice is requested. In this case edit the 
c   user subroutines set_ren_scale and set_fac_scale in setpara.f

c      call get_logical(npara,param,value," fixed_fac_scale ",
c     $   fixed_fac_scale,.true.)
      fixed_fac_scale=.true.
      sf1=91.188d0
      sf2=91.188d0
c      call get_real   (npara,param,value," dsqrt_q2fact1 ",sf1,91.188d0)
c      call get_real   (npara,param,value," dsqrt_q2fact2 ",sf2,91.188d0)
      if (fixed_fac_scale .and. sf1 .ne. sf2) then
        write (*,*) 'Error: different factorization scales for beams'
        write (*,*) 'Quitting...'
        STOP
      endif
      SCALEF=0d0

      if (fixed_fac_scale) THEN
        SCALEF=sf1
      ENDIF
      

c************************************************************************
c   Collider energy and type                                           *
c************************************************************************
c   lpp  = -1 (antiproton), 0 (no pdf), 1 (proton)
c   ebeam= energy of each beam in GeV
      IBEAM(1)=1
      IBEAM(2)=1
      EBMUP(1)=7d3
      EBMUP(2)=7d3


c      call get_integer(npara,param,value," lpp1 "   ,IBEAM(1),1  )
c      call get_integer(npara,param,value," lpp2 "   ,IBEAM(2),1  )

c      call get_real   (npara,param,value," ebeam1 " ,EBMUP(1),7d3)
c      call get_real   (npara,param,value," ebeam2 " ,EBMUP(2),7d3)
      
      if(mode.eq.1.or.mode.eq.2) then
        write(LNHOUT,*) 'Reading weighted events'
        if(mode.eq.1)
     $       write(LNHOUT,*)'in this mode Pythia will get wrong cs!'
      endif
      if(mode.eq.3.or.mode.eq.4) then
        write(LNHOUT,*)'Reading unweighted events'
        if(mode.eq.4)
     $       write(LNHOUT,*)'in this mode Pythia will get wrong cs!'
      endif
      NREAD=12
      XSECUP(1)=120d0
      XMAXUP(1)=120d0
c      call get_real (npara,param,value," xsec " ,XSECUP(1),1d0)
c      call get_real (npara,param,value," wtmax " ,XMAXUP(1),1d0)

C...Decide on weighting strategy: unweighted on input.
      IDWTUP=MODE

      WRITE(LNHOUT,*) ' Number of Events Read:: ',NREAD
      WRITE(LNHOUT,*) ' Total Cross Section  :: ',XSECUP(1),' pb'
      WRITE(LNHOUT,*) ' Maximum Weight       :: ',XMAXUP(1)
        
C--HERWIG/PYTHIA SPECIFIC PART - MGSHIN SETS MASSES AND RETURNS
C-- 1 FOR HERWIG, 661 FOR PYTHIA
      CALL MGSHIN(I)
      LPRUP(1) = I

      WRITE(LNHOUT,*) 'Using Pythia (661) or Herwig (1): ',I

      IF(I.EQ.661) THEN

C...Initialize pdfs for pythia
         MSTP(52)=2
         MSTP(51)=MPDF

C...Initialize widths and partial widths for resonances.
         CALL PYINRE
        
C...Adjust cross-section for suppressed decay modes
         CALL BRSUPP

         WRITE(LNHOUT,*) ' Total Cross Section after reduction :: ',
     $        XSECUP(1),' pb'
      ELSE ! HERWIG
c         CALL get_hwpdf(PDFIN,NDNS)
C     CONVERT PDF TYPES
c         CALL PDFCONVH(NDNS,NTMP,PDFTYP)
         PDFGUP(1)=-1
         PDFSUP(1)=-1
         PDFGUP(2)=PDFGUP(1)
         PDFSUP(2)=PDFSUP(1)         
         IDWTUP=2
         XSECUP(1)=XSECUP(1)*1d3
         XMAXUP(1)=XMAXUP(1)*1d3
      ENDIF

      DO I=1,2
        IF(IBEAM(I).EQ.1.OR.IBEAM(1).EQ.2) THEN
          IDBMUP(I)=2212
        ELSEIF(IBEAM(I).EQ.-1.OR.IBEAM(1).EQ.-2) THEN
          IDBMUP(I)=-2212
        ELSEIF(IBEAM(I).EQ.3) THEN
          IDBMUP(I)=11
        ELSEIF(IBEAM(I).EQ.-3) THEN
          IDBMUP(I)=-11
        ELSEIF(IBEAM(I).EQ.0) THEN
c       Use the first two particle ids for IBMUP (have already been set)
        ELSE
          IDBMUP(I)=IBEAM(1)
        ENDIF
      ENDDO

      WRITE(LNHOUT,*) 'Set beams to ',IDBMUP(1),' and ',IDBMUP(2)

      RETURN
      END
      

C*********************************************************************

      subroutine BRSUPP

      IMPLICIT NONE

C...Three Pythia functions return integers, so need declaring.
      INTEGER PYCOMP,MWID
      DOUBLE PRECISION WIDS
C...Resonance width and secondary decay treatment.
      COMMON/PYINT4/MWID(500),WIDS(500,5)
C...User process initialization commonblock.
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &   IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &   LPRUP(MAXPUP)

C...Extra commonblock to transfer run info.
      INTEGER MODE,LNHIN,LNHOUT,NREAD,MSCAL
      REAL*8 SCALEF
      COMMON/BAUPPRIV/SCALEF,LNHIN,NREAD,LNHOUT,MODE,MSCAL

C...BRSUPP common block
      INTEGER NRES,IDRES
      REAL PEREVENT
      common/PARTCOMP/NRES,IDRES(100),PEREVENT(100)

      INTEGER I
      REAL SUPPCS


      SUPPCS=1.
      do I=1,NRES
        WRITE(LNHOUT,*) 'Resonance ',IDRES(I), ' has wids ',
     $     wids(PYCOMP(IDRES(I)),2), ' and PEREVENT ',
     $     PEREVENT(I)
        if(abs(PEREVENT(I)-1.).lt.0.1) then
          if(wids(PYCOMP(IDRES(I)),2).lt.0.95) then
            write(*,*) 'FS particle ',IDRES(I),' has BRtot= ',
     &         wids(PYCOMP(IDRES(I)),2)
            write(*,*) 'Decreasing cross section!'
            SUPPCS=SUPPCS*wids(PYCOMP(IDRES(I)),2)
          endif
        endif
        if(abs(PEREVENT(I)-2.).lt.0.1) then
          if(wids(PYCOMP(IDRES(I)),2).lt.0.95) then
            write(*,*)'Double FS particle ',IDRES(I),' has BRtot= ',
     &         wids(PYCOMP(IDRES(I)),2)
            write(*,*) 'Decreasing cross section by ',
     &         wids(PYCOMP(IDRES(I)),1), '! (m.b. != BRtot**2)'
            SUPPCS=SUPPCS*wids(PYCOMP(IDRES(I)),1)
          endif
        endif
      enddo
      write(*,*)'Multiplying cross section by ',SUPPCS
      XSECUP(1)=XSECUP(1)*SUPPCS
      RETURN
      END

c----------------------------------------------------------------------
c   READ_PARAMS
c   Read the parameters from the MadEvent event file
c   
c   1. PDF set
c   2. Input values for the EW sector 
c   4. Higgs mass and width
c   3. Fermion masses (pole and MSbar) and widths
c   6. Renormalization and factorization scales
c   5. Collider parameters
c----------------------------------------------------------------------

      subroutine read_params(iunit,npara,param,value)
      implicit none
c   
c   parameters
c   
      integer maxpara
      parameter (maxpara=100)
c   
c   arguments
c   
      integer iunit
      character*20 param(maxpara),value(maxpara)
      integer npara

C...User process initialization commonblock.
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &   IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &   LPRUP(MAXPUP)
C...User process event common block.
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &   ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &   VTIMUP(MAXNUP),SPINUP(MAXNUP)
C...BRSUPP common block
      INTEGER NRES,IDRES
      REAL PEREVENT
      common/PARTCOMP/NRES,IDRES(100),PEREVENT(100)

C...Extra commonblock to transfer run info.
      INTEGER MODE,LNHIN,LNHOUT,NREAD,MSCAL
      REAL*8 SCALEF
      COMMON/BAUPPRIV/SCALEF,LNHIN,NREAD,LNHOUT,MODE,MSCAL
c   
c   local
c   
      character*20 ctemp
      integer i,l1,l2,j,jj
      character*132 buff
      integer NPARTS,KNTEV
      double precision WTEVNT,XSTOT,DUMMY,WTMAX
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
      XSTOT=0D0

c   
c   read in values
c   
      buff='#'
      do 10 while(.true.)
        read(iunit,'(a132)',end=99,err=98) buff
        if(buff.eq.' ') then
          goto 10
        endif
        if(buff(1:1).eq.'#')then
           if(buff(2:2).ne.'#'.and.index(buff,"=").ne.0) then
              l1=index(buff,"=")
              l2=index(buff,"!")
              if(l2.eq.0) l2=l1+20 !maybe there is no comment...
c     
              value(npara)=buff(2:l1-1)
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
           else if(buff(2:2).eq.'#') then
              l1=0
              if(index(buff,'Number of Events').ne.0)then
                 l1=index(buff,":")
                 param(npara)=" numev "
              else if(index(buff,'Integrated weight (pb)').ne.0)then
                 l1=index(buff,":")
                 param(npara)=" xsec "
              endif
              if(l1.ne.0)then
                 l2=l1+20
                 value(npara)=buff(l1+1:l2-1)
                 ctemp=value(npara)
                 call case_trap2(ctemp,20)
                 value(npara)=ctemp
                 npara=npara+1
              endif
           endif
           
           goto 10
        endif
C--   read number of particles and weight.
        read(buff,*) NPARTS,KNTEV,WTEVNT,SCALUP,AQEDUP,AQCDUP
        read(iunit,*,end=98,err=98) (IDUP(J),J=1,NPARTS)
        DO J=1,NPARTS
          DO JJ=1,NRES
            if(iabs(IDUP(J)).eq.IDRES(JJ))
     &         PEREVENT(JJ)=PEREVENT(JJ)+1.
          ENDDO
        ENDDO
c--   skip the rest of the event record
        DO J=1,6                !mothup(2),icolup(2),istup,spinup
          read(iunit,'(a132)',end=98,err=98) buff
        ENDDO                
        DO J=1,NPARTS-1         !momenta
          read(iunit,'(a132)',end=98,err=98) buff
        ENDDO
        IF(WTEVNT.GT.WTMAX) WTMAX=WTEVNT
        XSTOT=XSTOT+WTEVNT
        NREAD=NREAD+1
        read(iunit,'(a132)',end=99,err=99) buff
 10   continue
 99   continue

      REWIND(iunit)

      do J=1,NRES
        PEREVENT(J) = PEREVENT(J)/float(NREAD)
      enddo

C   Tentatively use the first two particle IDs IDBMUP
c      IF(IABS(IDUP(1)).EQ.11.OR.IABS(IDUP(2)).EQ.11)THEN
      IDBMUP(1)=IDUP(1)
      IDBMUP(2)=IDUP(2)
c      ENDIF

      if(MODE.EQ.0) then
         MODE=3
         WRITE(LNHOUT,*)'MODE set to ', MODE
      endif

      call get_integer   (npara+1,param,value," numev " ,i,-1)
      if(i.ne.NREAD) GOTO 97
      call get_real   (npara+1,param,value," xsec " ,DUMMY,0d0)
      if(ABS((DUMMY-XSTOT)/XSTOT).gt.1e-2) GOTO 96

      npara=npara+1
      param(npara)=" wtmax "
      write(value(npara),*) WTMAX*NREAD

      return

 96   WRITE(*,*)'Wrong xsec of events in file:',DUMMY,XSTOT
      WRITE(*,*)'Quitting...'
      STOP
 97   WRITE(*,*)'Wrong number of events in file:',I,NREAD
      WRITE(*,*)'Quitting...'
      STOP
 98   WRITE(*,*)'Unexpected end of file in event number ',
     $   NREAD+1
      WRITE(*,*)'Quitting...'
      STOP


      end

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

      subroutine get_real(npara,param,value,name,var,def_value)
c----------------------------------------------------------------------------------
c   finds the parameter named "name" in param and associate to "value" in value 
c----------------------------------------------------------------------------------
      implicit none

c   
c   parameters
c   
      integer maxpara
      parameter (maxpara=100)
c   
c   arguments
c   
      integer npara
      character*20 param(maxpara),value(maxpara)
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
c   parameters
c   
      integer maxpara
      parameter (maxpara=100)
c   
c   arguments
c   
      integer npara
      character*20 param(maxpara),value(maxpara)
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
c   
      subroutine get_string(npara,param,value,name,var,def_value)
c----------------------------------------------------------------------------------
c   finds the parameter named "name" in param and associate to "value" in value 
c----------------------------------------------------------------------------------
      implicit none

c   
c   parameters
c   
      integer maxpara
      parameter (maxpara=100)
c   
c   arguments
c   
      integer npara
      character*20 param(maxpara),value(maxpara)
      character*(*)  name
      character*(*)  var,def_value
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
c   
      subroutine get_logical(npara,param,value,name,var,def_value)
c----------------------------------------------------------------------------------
c   finds the parameter named "name" in param and associate to "value" in value 
c----------------------------------------------------------------------------------
      implicit none

c   
c   parameters
c   
      integer maxpara
      parameter (maxpara=100)
c   
c   arguments
c   
      integer npara
      character*20 param(maxpara),value(maxpara)
      character*(*)  name
      logical  var,def_value
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

C
C   GET_PDF
C   Convert MadEvent pdf name to PDFlib number (works also for LHAPDF)

      subroutine get_lhapdf(PDFIN,MPDF)
      IMPLICIT NONE

C...Extra commonblock to transfer run info.
      INTEGER MODE,LNHIN,LNHOUT,NREAD,MSCAL
      REAL*8 SCALEF
      COMMON/BAUPPRIV/SCALEF,LNHIN,NREAD,LNHOUT,MODE,MSCAL

      CHARACTER*20 PDFIN
      INTEGER MPDF
      INTEGER NPDFS,I
      PARAMETER (NPDFS=12)
      CHARACTER*20 PDFLABS(NPDFS)
      INTEGER NUMSPDF(NPDFS)
      DATA PDFLABS/'mrs02nl',
     $   'mrs02nn',
     $   'cteq4_m',
     $   'cteq4_l',
     $   'cteq4_d',
     $   'cteq5_m',
     $   'cteq5_d',
     $   'cteq5_l',
     $   'cteq5m1',
     $   'cteq6_m',
     $   'cteq6_l',
     $   'cteq6l1'/
      DATA NUMSPDF/20250,
     $   20270,
     $   19150,
     $   19170,
     $   19160,
     $   19050,
     $   19060,
     $   19070,
     $   19051,
     $   10050,
     $   10041,
     $   10042/
      
      MPDF=-1
      DO I=1,NPDFS
        IF(PDFIN .EQ. PDFLABS(I))
     $     MPDF=NUMSPDF(I)
      ENDDO

      IF(MPDF.EQ.-1) THEN
        WRITE(LNHOUT,*)'PDF ',PDFIN,' not implemented in get_lhapdf.'
        WRITE(LNHOUT,*)'Known pdfs are'
        WRITE(LNHOUT,*) PDFLABS
      ENDIF
      return
      end

C
C   GET_PDF
C   Convert MadEvent pdf name to PDFlib number (works also for LHAPDF)

      subroutine get_hwpdf(PDFIN,MPDF)
      IMPLICIT NONE

C...Extra commonblock to transfer run info.
      INTEGER MODE,LNHIN,LNHOUT,NREAD,MSCAL
      REAL*8 SCALEF
      COMMON/BAUPPRIV/SCALEF,LNHIN,NREAD,LNHOUT,MODE,MSCAL

      CHARACTER*20 PDFIN
      INTEGER MPDF
      INTEGER NPDFS,I
      PARAMETER (NPDFS=1)
      CHARACTER*20 PDFLABS(NPDFS)
      INTEGER NUMSPDF(NPDFS)
      DATA PDFLABS/'cteq6l1'/
      DATA NUMSPDF/8/
      
      MPDF=-1
      DO I=1,NPDFS
        IF(PDFIN .EQ. PDFLABS(I))
     $     MPDF=NUMSPDF(I)
      ENDDO

      IF(MPDF.EQ.-1) THEN
        WRITE(LNHOUT,*)'PDF ',PDFIN,' not implemented in get_pdflib.'
        WRITE(LNHOUT,*)'Known pdfs are'
        WRITE(LNHOUT,*) PDFLABS
      ELSE
        WRITE(LNHOUT,*)'Using Herwig PDF number',MPDF      
      ENDIF
      return
      end

C*********************************************************************
      
C...MSSMPAR
C...Checks if model is mssm and extracts SLHA file
C...Reads all particle masses and SM parameters in any case

      SUBROUTINE MSSMPAR(iunit)

      IMPLICIT NONE

C...Supersymmetry parameters.
      INTEGER IMSS
      DOUBLE PRECISION RMSS
      COMMON/PYMSSM/IMSS(0:99),RMSS(0:99)

C...Local variables      
      CHARACTER*132 buff
      CHARACTER*8 model
      INTEGER iunit
      
      buff='#'
      do 100 while(.true.)
         read(iunit,'(a132)',end=105,err=98) buff
         if(buff.eq.' ') then
            goto 100
         endif
         if(buff(1:1).ne.'#') goto 105
         if(buff(1:14).eq.'## Begin MODEL')then
            read(iunit,'(a132)',end=99,err=98) buff
            model=buff(2:)
         endif
         if(buff(1:23).eq.'## Begin param_card.dat')then
            open(24,FILE='SLHA.dat',ERR=90)
            do 10 while(.true.)
               read(iunit,'(a132)',end=99,err=98) buff
               if(buff(1:21).eq.'## End param_card.dat') goto 15
               write(24,'(a80)') buff(2:)
 10         continue
 15         continue
            close(24)
         endif
 100  continue
 105  continue
      REWIND(iunit)
      
      PRINT *,'Model is ',model

      open(24,FILE='SLHA.dat',ERR=91)
c     Pick out SM parameters
      CALL READSMLHA(24)

      if(model.eq.'mssm')then
         IMSS(1) = 11
         IMSS(21)= 24 ! Logical unit number of SLHA spectrum file
c         IMSS(22)= 24 ! Logical unit number of SLHA decay file
      endif

      RETURN

 90   WRITE(*,*)'Could not open file SLHA.dat for writing'
      WRITE(*,*)'Quitting...'
      STOP
 91   WRITE(*,*)'Could not open file SLHA.dat for reading'
      WRITE(*,*)'Quitting...'
      STOP
 98   WRITE(*,*)'Unexpected error reading file'
      WRITE(*,*)'Quitting...'
      STOP
 99   WRITE(*,*)'Unexpected end of file'
      WRITE(*,*)'Quitting...'
      STOP

      END

C*********************************************************************
      
C...READSMLHA
C...Reads the SMLHA file to extract SM parameters
C...as well as all particle masses.

      SUBROUTINE READSMLHA(iunit)

      IMPLICIT NONE

#include "Baur_i/madsho.inc"

C...Three Pythia functions return integers, so need declaring.
      INTEGER PYCOMP

C...Commonblocks.
C...Parameters.
      INTEGER MSTU,MSTJ,KCHG
      DOUBLE PRECISION PARU,PARJ,PMAS,PARF,VCKM
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
C...Particle properties + some flavour parameters.
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)

C...Local variables      
      CHARACTER*132 buff,block_name
c      CHARACTER*8 model
      INTEGER iunit,ivalue
      DOUBLE PRECISION value
      LOGICAL block_found
      
      block_found=.false.

      do 100 while(.true.)
         read(iunit,'(a132)',end=105,err=98) buff
         call case_trap2(buff,132)
         if(buff(1:1).eq.'b')then
            block_name=buff(7:)
            block_found=.true.
c         elseif(buff(1:1).eq.'d') then
c            read(buff(6:),*) ivalue,value
c            print *,'Reading decay ',ivalue,value
c            PMAS(PYCOMP(ivalue),2)=value            
         endif
         if (block_found) then
            do 10 while(.true.)
               read(iunit,'(a132)',end=105,err=98) buff
               if(buff(1:1).eq.'#') goto 10
               if(buff(1:1).ne.' ') then
                  block_found=.false.
                  backspace(iunit)
                  goto 100
               endif
               if(block_name(1:8).eq.'sminputs')then
                  read(buff,*) ivalue,value
                  print *,'Reading parameter ',block_name(1:8),
     $                 ivalue,value
                  if(ivalue.eq.1) PARU(103)=1d0/value
                  if(ivalue.eq.2) PARU(105)=value
c                  if(ivalue.eq.4) PMAS(23,1)=value
c                  if(ivalue.eq.6) PMAS(6,1)=value
                  if(ivalue.eq.4) MZ=value
                  if(ivalue.eq.6) MT=value
                  if(ivalue.eq.7) PMAS(15,1)=value
               else if(block_name(1:9).eq.'mgsmparam')then
                  read(buff,*) ivalue,value
                  print *,'Reading parameter ',block_name(1:9),
     $                 ivalue,value
                  if(ivalue.eq.1) PARU(102)=value
c                  if(ivalue.eq.2) PMAS(24,1)=value
                  if(ivalue.eq.2) MW=value
               else if(block_name(1:4).eq.'mass')then
                  read(buff,*) ivalue,value
                  print *,'Reading parameter ',block_name(1:4),
     $                 ivalue,value
                  if(ivalue.eq.4) then
                     MC=value
                  else if(ivalue.eq.5) then
                     MB=value
                  else if(ivalue.eq.6) then
                     MT=value
                  else if(ivalue.eq.23) then
                     MZ=value
                  else if(ivalue.eq.24) then
                     MW=value
                  else if(ivalue.eq.25) then
                     MH=value
                  else
                     PMAS(PYCOMP(ivalue),1)=value
                  endif
               endif
 10         continue
         endif
 100  continue
 105  continue

      REWIND(iunit)

      RETURN
 98   WRITE(*,*)'Unexpected error reading file'
      WRITE(*,*)'Quitting...'
      STOP
      END
