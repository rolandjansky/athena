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
      include 'mphi.f'

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

ccccccccc


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

ccccccccc
c
c      surv=1d0
c      if(beam.eq.'prot'.or.ionqcd.eq.'coh'.or.ionqcd.eq.'incoh')then
c         call initparsr(isurv)
c         call readscreen
c         if(beam.eq.'prot'.or.ionqcd.eq.'incoh')surv=1d0/norm**2
c      endif
c
c      if(qcd)then
c         call calcsud
c         call calchg
c      endif
c
c
c      if(beam.eq.'ion'.or.beam.eq.'ionp')then
c         call ioninit
c      endif
c
cccccccccccc
c      if(beam.eq.'ion'.or.beam.eq.'ionp')call ioninit

c      if(beam.eq.'ionp')then
c         rts=rtspa
c         s=spa
c      elseif(beam.eq.'ion')then
c         rts=rtsaa
c         beta=dsqrt(1d0-4d0*mp**2/s)
c         saa=an**2*s
c         rts=dsqrt(saa)
c         rts=rts
c         s=saa        
c      endif


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
      NCALL=NCALL1
      avgi1=avgi
      sd1=sd

      ncall=ncall*iinc
c      inccall=inccall*iinc
      inccall=inccall

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
