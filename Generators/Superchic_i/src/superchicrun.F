ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c This is a subroutine to generate events (same as  superchic.f code)
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine superchicrun
      implicit double precision (a-z)
      integer i,j,k
      integer nhistmax
      integer outl
      integer iinc,ncallu
      logical histol
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
c      include 'mphi.f'

cccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccccccccccccccccccccccccccccccccccccccccccccccccc

      forward=.false.

      call length(outtag,outl)

      open(45,file='evrecs/evrec'//outtag(1:outl)//'.dat')
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
c      mphi=1.019461d0
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

ccccccccccccccccccccccccccccccccccccccccccccccc
cccc     HEPEVT
ccccccccccccccccccccccccccccccccccccccccccccccc

      do k=1,2
         do j=1,4
            phep(j,k)=q(j,k)
         enddo
         phep(5,k)=mass(k)
         if(beam.eq.'el')then
            phep(5,k)=me
         elseif(beam.eq.'prot')then
            phep(5,k)=mp
         elseif(beam.eq.'ion'.or.beam.eq.'ionp')then
            phep(5,k)=mion
         endif
      enddo
      do k=1,20
         do j=1,4
            vhep(j,k)=0d0
         enddo
      enddo
      isthep(1)=2
      isthep(2)=2
      isthep(3)=1
      isthep(4)=1
      jmohep(2,5)=2
      jdahep(1,1)=0
      jdahep(2,1)=0
      jdahep(1,2)=0
      jdahep(2,2)=0
      jdahep(1,3)=0
      jdahep(2,3)=0
      jdahep(1,4)=0
      jdahep(2,4)=0


ccccccccccccccccccccccccccccccccccccccccccccccc
ccccc Les Houches
ccccccccccccccccccccccccccccccccccccccccccccccc

      nprup=1
      idwtup=3
      pdfsup(1)=0
      pdfsup(2)=0
      pdfgup(1)=0
      pdfgup(2)=0
      idprup=0
      xwgtup=1d0
      aqedup=alpha

ccc   NEW LHE init

      pdfsup(1)=247000
      pdfsup(2)=247000
      pdfgup(1)=0
      pdfgup(2)=0

      do k=1,2
         do j=1,4
            pup(j,k)=q(j,k)
         enddo
         pup(5,k)=mass(k)
         if(beam.eq.'el')then
            pup(5,k)=me
         elseif(beam.eq.'prot')then
            pup(5,k)=mp
         elseif(beam.eq.'ion'.or.beam.eq.'ionp')then
            pup(5,k)=mion
         endif
      enddo
      istup(1)=-1
      istup(2)=-1
      istup(3)=1
      istup(4)=1
      mothup(1,1)=0
      mothup(2,1)=0
      mothup(1,2)=0
      mothup(2,2)=0
      mothup(1,3)=0
      mothup(2,3)=0
      mothup(1,4)=0
      mothup(2,4)=0
      mothup(1,5)=0
      mothup(2,5)=0
      icolup(1,1)=0
      icolup(2,1)=0
      icolup(1,2)=0
      icolup(2,2)=0
      icolup(1,3)=0
      icolup(2,3)=0
      icolup(1,4)=0
      icolup(2,4)=0
      icolup(1,5)=0
      icolup(2,5)=0
      do i=1,20
         vtimup(i)=0
         spinup(i)=9
      enddo

      do i=1,2
         do j=1,5
            jmohep(i,j)=mothup(i,j)
         enddo
      enddo

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      if(proc.eq.18.or.proc.eq.19.or.proc.eq.20)then
         nup=11
      elseif(proc.eq.54.or.proc.eq.55)then
         nup=11
      elseif(proc.eq.73.or.proc.eq.74.or.proc.eq.75)then
         nup=11
      elseif(proc.eq.76)then
         nup=9
      elseif(decay4)then
         if(proc.eq.53)then
            nup=10
         else
            nup=9
         endif
      elseif(decay6)then
         nup=11
      elseif(dps.eq.2.or.decay2)then
         nup=7
      elseif(dps.eq.3)then
         nup=8
      elseif(decay3)then
         nup=9
      elseif(dps.eq.1)then
         nup=5
      endif

      do j=3,nup
         do i=1,4
            evrec(evnum,j,i)=q(i,j)
         enddo
      enddo

      nhep=nup
      nup=nup-2
      if(proc.eq.68)then
         nhep=7
         nup=7
      endif

cccccccccccccccccccccccccccccccccccccccccccccc

      nhist=0
      nhistmax=20



      neff=0
      neff0=0

      do i=1,10
         xu(i)=1d0
         xl(i)=0d0
      enddo

      ACC=-1D0
      NPRN=1

      idum=-abs(iseed)
      randum=ran2()


      ITMX1=1

      bin=.false.
      sfac=.false.
      unw=.false.
      calcmax=.false.
      iinc=1

            print*,''
      print*,'*****************************************************'
      print*,'Vegas:initialisation run (outputs *bare* crosssection)'
      print*,'******************************************************'
      ncall=ncall1+10000
      CALL VEGAS(cs,AVGI,SD,CHI2A)


      if(readwt)goto 779

            print*,''
      print*,'*********************************'
      print*,'  Vegas : main run              '
      print*,'*********************************'

      if(gencuts)then

      print*,''
      print*,'**********************************'
      print*,''
      write(6,19)dble(neff)/dble(neff0)*100d0
      print*,''

      iinc=nint(dble(neff0)/dble(neff))

      write(6,20)iinc
      print*,''
      print*,'***********************************'
      print*,''

 19   FORMAT(' Cut Efficiency = ',G10.4,'%')
 20   FORMAT(' => multiply NCALL by ',i4)

      endif

      ITMX=ITMX1
      NCALL=NCALL1+1000
      avgi1=avgi
      sd1=sd

      ncall=ncall*iinc
      inccall=inccall*iinc

 779  bin=.true.

      ncallu=ncall

      sfac=sfaci
      unw=.false.
      calcmax=.true.
      ren=1d0

      it=1
      itmx=1

      if(readwt)goto 778

      ren=dble(ncall)

      CALL VEGAS1(cs,AVGI,SD,CHI2A)

c      prec=prec*1d-2
      prec=prec

 777  if(dabs(sd/avgi).gt.prec)then

         it=it+1
         ncall=ncall+inccall
         ren=dble(ncall)

         CALL VEGAS2(cs,AVGI,SD,CHI2A)


         if(it.gt.itend)goto 778

         goto 777

      endif

 778  unw=.true.
      calcmax=.true.

  10  FORMAT(' cross section = ',G16.7,' +/-',G16.7,' ( ',F9.4,' )')

cccccccccccccc

 999  if(genunw)then

         avgio=avgi
         sdo=sd

      print*,''
      print*,'*****************************'
      print*,'Generating unweighted events'
      print*,'*****************************'
      print*,''

c      ncall=nev
      ncall=ncallu
      if(ncall.lt.1000)ncall=1000
 566  ren=dble(ncall)
      itmx=1

      CALL VEGAS2(cs,AVGI,SD,CHI2A)


      if(evnum.lt.nev)then

      print*,''
      print*,'**************************************************************
     &**************'
      write(6,100)evnum,nev
      print*,'**************************************************************
     &**************'

      endif

 100  format('  generated events so far = ',i7,'    total = ',i7)

c      ncall=ncall+nev
      ncall=ncall+inccall

      if(evnum.lt.nev)goto 566

      xsecup(1)=avgi
      xerrup(1)=sd
      xmaxup(1)=avgi

      call unwprint

      if(readwt)then
      else
         avgi=avgio
         sd=sdo
      endif

      endif

      close(55)
      close(45)

ccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      print*,''
      write(6,301)avgi,sd
      print*,''

 301  format(' Cross section  = ',G16.7,' +/-',G16.7,' pb')

      call cpu_time(t2)
      print*,'time elapsed = ', t2, ' s'
      iseed=(10000*t2)

      return
      end
