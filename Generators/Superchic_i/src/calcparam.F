ccc   reads input parameters and calculated needed parameters 
      subroutine calcparam
      implicit double precision (a-z)
      integer i,j,k
      character*100 dum
      integer idum
      COMMON /ranno/ idum

      include 'pdfinf.f'
      include 'genunw.f'
      include 'survin.f'
      include 'mesflag.f'
      include 'procn.f'
      include 'gencuts.f'
      include 'pi.f'
      include 'pdg.f'
      include 'unweighted.f'
      include 'surv.f'
      include 'bin.f'
      include 'ewpars.f'
      include 'zi.f'
      include 'vars.f'
      include 'survpars.f'
      include 'mom.f'
      include 'range.f'
      include 'polarization.f'
      include 'proc.f'
      include 'mq.f'
      include 'norm.f'
      include 'mixing.f'
      include 'meta.f'
      include 'mres.f'
      include 'forward.f'
      include 'mfact.f'
      include 'jetalg.f'
      include 'decay.f'
      include 'record.f'
      include 'mp.f'
      include 'quarkonia.f'
      include 'scorr.f'
      include 'output.f'
      include 'nhist.f'
      include 'intag.f'
      include 'vegas.f'
      include 'vegaspars.f'
      include 'hepevt.f'
      include 'leshouches.f'
      include 'photo.f'
      include 'nsurv.f'
      include 'eff.f'
      include 'prec.f'
      include 'wmax.f'
      include 'wtmax.f'
      include 'mkp.f'
      include 'mpip.f'
      include 'widths.f'
      include 'lep.f'
      include 'monopar.f'
      include 'gax.f'
      include 'beam.f'
      include 'ion.f'
      include 'mion.f'
      include 'ionqcd.f'
      include 'qcd.f'
      include 'mn.f'
      include 'rech.f'
      include 'ptXcuts.f'
      include 'mcharg.f'
      include 'varsi.f'
      include 'spA.f'
      include 'sAA.f'
      include 'diff.f'
      include 'diss.f'
      include 'enew.f'
      include 'gamma.f'

ccccccc

      open(unit=1,file='input.DAT',status='old', action='read')

      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)rts
      read(1,*)isurv
      read(1,*)intag
      read(1,*)dum
      read(1,*)dum
      read(1,*)PDFname
      read(1,*)PDFmember
      read(1,*)dum
      read(1,*)proc
      read(1,*)beam
      read(1,*)outtag
      read(1,*)sfaci
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)diff
c      read(1,*)elcoll
      read(1,*)dum
      read(1,*)an
      read(1,*)az
      read(1,*)rz
      read(1,*)dz
      read(1,*)rn
      read(1,*)dn
      read(1,*)ionqcd
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)ncall
      read(1,*)itmx
      read(1,*)prec
      read(1,*)ncall1
      read(1,*)inccall
      read(1,*)itend
      read(1,*)iseed
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)genunw
      read(1,*)nev
      read(1,*)erec
      read(1,*)readwt
      read(1,*)wtmax
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)ymin
      read(1,*)ymax
      read(1,*)mmin
      read(1,*)mmax
      read(1,*)gencuts
      read(1,*)scorr
      read(1,*)fwidth
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)ptxmax
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)ptamin
      read(1,*)ptbmin
      read(1,*)etaamin
      read(1,*)etaamax
      read(1,*)etabmin
      read(1,*)etabmax
      read(1,*)acoabmax
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)ptamin3
      read(1,*)ptbmin3
      read(1,*)ptcmin3
      read(1,*)etaamin3
      read(1,*)etaamax3
      read(1,*)etabmin3
      read(1,*)etabmax3
      read(1,*)etacmin3
      read(1,*)etacmax3
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)ptamin4
      read(1,*)ptbmin4
      read(1,*)ptcmin4
      read(1,*)ptdmin4
      read(1,*)etaamin4
      read(1,*)etaamax4
      read(1,*)etabmin4
      read(1,*)etabmax4
      read(1,*)etacmin4
      read(1,*)etacmax4
      read(1,*)etadmin4
      read(1,*)etadmax4
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)ptamin6
      read(1,*)ptbmin6
      read(1,*)ptcmin6
      read(1,*)ptdmin6
      read(1,*)ptemin6
      read(1,*)ptfmin6
      read(1,*)etaamin6
      read(1,*)etaamax6
      read(1,*)etabmin6
      read(1,*)etabmax6
      read(1,*)etacmin6
      read(1,*)etacmax6
      read(1,*)etadmin6
      read(1,*)etadmax6
      read(1,*)etaemin6
      read(1,*)etaemax6
      read(1,*)etafmin6
      read(1,*)etafmax6
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)rjet
      read(1,*)jalg
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)m2b
      read(1,*)pdgid(6)
      read(1,*)pdgid(7)
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)malp
      read(1,*)gax
      read(1,*)alpt
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)mpol
      read(1,*)mmon
      read(1,*)gamm
      read(1,*)dum
      read(1,*)dum
      read(1,*)dum
      read(1,*)mcharg
      read(1,*)mneut

ccccccccccccc
c main/superchic.f
      
      diffsd=diff
      if(diff.eq.'sda'.or.diff.eq.'sdb')then
         diff='sd'
      else
         diffsd='n'
      endif

      offshell=.false.

      forward=.false.

C      call length(outtag,outl)

C      open(45,file='evrecs/evrec'//outtag(1:outl)//'.dat')
      wmax=0d0
      evnum=0

      if(genunw)then
      else
         readwt=.false.
      endif
      if(readwt)wmax=wtmax

      if(erec.eq.'hepmc')then
         erech=.true.
         erec='lhe'
      endif

      iw=0

      gf=1.16639d-5
      v=dsqrt(1d0/dsqrt(2d0)/gf)
      mt=173d0
      mb=4.75d0
      mc=1.4d0
      mmu=0.10566d0
      mpsi=3.096916d0
      mpsip=3.686109d0
      mups=9.46030d0
      mchic0=3.41475d0
      mchib0=9.85944d0
      mp=0.938272046d0
      mn=0.939565413d0
      mw=80.318d0
      me=0.511d-3
      mtau=1.77682d0
      mpip=0.13957018d0
      mkp=0.493677d0
      alpha=7.2974d-3

      pi=dacos(-1d0)
      conv=389379d3
      zi=(0d0,1d0)
      mup=0.062d0
      md=0.083d0
      ms=0.215d0

      rmf1( 1) = 1d-10
      rmf1( 2) = me
      rmf1( 3) = 1d-10
      rmf1( 4) = mmu
      rmf1( 5) = 1d-10
      rmf1( 6) = mtau
      rmf1( 7) = 0.062d0
      rmf1( 8) = 0.083d0
      rmf1( 9) = mc
      rmf1(10) = 0.215d0
      rmf1(11) = mt
      rmf1(12) = mb

      rmf1( 1) = me
      rmf1( 2) = mmu
      rmf1( 3) = mtau
      rmf1( 4) = md
      rmf1( 5) = mup
      rmf1( 6) = ms
      rmf1( 7) = mc
      rmf1( 8) = mb
      rmf1( 9) = mt


      mq=0d0
      hel=1
      mes=.false.
      mfact='mx'
      forward=.false.
      decay2=.false.
      decay3=.false.
      decay4=.false.
      decay6=.false.

cccccccccccc

      do i=1,20
         jdahep(1,i)=0
         jdahep(2,i)=0
      enddo

cccccccccccccccccccccccccc
c main/superchic.f

      call inpdf
      print*,'Read PDF successfully...'
      call supinit

      s2int=8
      if(beam.eq.'ionp')s2int=16
      if(diff.eq.'el'.and.gamma.eqv..true.)s2int=16

      call header
      call gaminit
      call gaminit_comb
      call readcoh
      call gaussinit

cccccccccccccccccccccccccc
c main/superchic.f

      if(diff.eq.'sd'.or.diff.eq.'dd')then
         if(offshell.eqv..false.)then
            print*,'Dissociation not currently supported'//
     &           ' for this process/beam - STOP'
            stop
         endif
         if(erec.eq.'hepevt'.or.erec.eq.'hepmc')then
            print*,'Dissociation currently only supported with LHE'
         endif
      endif

      elcoll=.false.
      difftot=.false.
      if(diff.eq.'el')then
         diss1=.false.
         diss2=.false.
      elseif(diff.eq.'sd')then
         ndim=ndim+1
         if(genunw)elcoll=.true.
      elseif(diff.eq.'dd')then
         diss1=.true.
         diss2=.true.
         ndim=ndim+2
      elseif(diff.eq.'tot')then
         ndim=ndim+3
         difftot=.true.
      endif

      if(diff.eq.'tot'.or.diff.eq.'sd'.or.diff.eq.'dd')then
         call apfelinit
         call calcs2diss
      endif

cccccccccccccccccccccccccc
c main/superchic.f

      if(mes)then
         call calcmes
         call wfinit
      endif

ccccccccccccccccccccccccc
c main/superchic.f

      if(beam.eq.'el')then
         if(sfaci)then
            print*,'Error : must have sfaci = .false. for initial-state
     &electrons'
            stop
         endif
      endif

cccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccccccccccccccccccccccccccccccccccccccccccccccccc
ccc   Initialises grids for skewed PDFs and survival factors
c init/init.f

      call initpars(isurv)   ! Initialise soft survival parameters
      call calcop            ! proton opacity
      call calcscreen        ! screening amplitude

      call readcoh
      call dd
      call readscreen
      call sdcoh
      call apfelinit
      call sdincoh

c      stop

      call calcsud          ! sudakov factor
      call calchg            ! skewed PDF
cccccccccccccccccccccccccccccccccccccccccccccccccccc
      print*,'Calculated opacity,screening ampl.,sudakov, & skewed PDF'

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c main/superchic.f
C rts ... CMS collision energy (GeV)

      s=rts**2

      if(beam.eq.'prot')then
         beta=dsqrt(1d0-4d0*mp**2/s)
      elseif(beam.eq.'el')then
         beta=dsqrt(1d0-4d0*me**2/s)
      elseif(beam.eq.'ion'.or.beam.eq.'ionp')then
         mion=mp*an
         rtsi=rts
         si=s
      endif

      q(1,1)=0d0
      q(2,1)=0d0
      q(3,1)=rts/2d0*beta
      q(4,1)=rts/2d0

      q(1,2)=0d0
      q(2,2)=0d0
      q(3,2)=-rts/2d0*beta
      q(4,2)=rts/2d0

      if(beam.eq.'ionp')call pAinit
      if(beam.eq.'ion')call AAinit

      if(beam.eq.'prot')then
         pdgid(1)=2212
         pdgid(2)=2212
         pdgid(3)=2212
         pdgid(4)=2212
      elseif(beam.eq.'el')then
         pdgid(1)=11
         pdgid(2)=-11
         pdgid(3)=11
         pdgid(4)=-11
      elseif(beam.eq.'ion')then
         pdgid(1)=1000000000
         pdgid(1)=pdgid(1)+nint(az)*10000
         pdgid(1)=pdgid(1)+nint(an)*10
         pdgid(2)=pdgid(1)
         pdgid(3)=pdgid(1)
         pdgid(4)=pdgid(1)
      elseif(beam.eq.'ionp')then
         pdgid(2)=1000000000
         pdgid(2)=pdgid(2)+nint(az)*10000
         pdgid(2)=pdgid(2)+nint(an)*10
         pdgid(1)=2212
         pdgid(3)=pdgid(1)
         pdgid(4)=pdgid(2)
      endif

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      return
      end
