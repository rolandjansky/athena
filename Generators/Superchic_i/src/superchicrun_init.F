ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c This is an init subroutine to generate events (same as  superchic.f code)
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine superchicrun_init
      implicit double precision (a-z)
      integer i,j,k
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
      include 'diff.f'
      include 'diss.f'
      include 'enew.f'
      include 'gamma.f'

ccccccccccccccccccccccccccccccccccccccccccccccc
cccc     HEPEVT
ccccccccccccccccccccccccccccccccccccccccccccccc
c main/superchic.f

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
c main/superchic.f

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

      nhep=nup

      if(diff.eq.'sd')nup=nup-1
      if(diff.eq.'el')nup=nup-2

ccccccccc
c main/superchic.f

      surv=1d0
      if(beam.eq.'prot'.or.ionqcd.eq.'coh'.or.ionqcd.eq.'incoh')then
         call initparsr(isurv)
         call readscreen
         if(beam.eq.'prot'.or.ionqcd.eq.'incoh')surv=1d0/norm**2
      endif

      if(qcd)then
C LCG_88b/MCGenerators/superchic/4.01p1
C         call calcsud
C         call calchg
         call readsud
         call readhg
      endif

      if(beam.eq.'ion'.or.beam.eq.'ionp')call ioninit

      if(beam.eq.'ionp')then
         rts=rtspa
         s=spa
      elseif(beam.eq.'ion')then
         rts=rtsaa
         s=saa
      endif

cccccccccccc
c main/superchic.f

      nhist=0
      nhistmax=20

cccccccccccccccc
c main/superchic.f

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
      print*,'  Vegas : main run               '
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
C      NCALL=NCALL1+1000
C      NCALL=NCALL1
      avgi1=avgi
      sd1=sd

      NCALL=NCALL1
      if(NCALL1.lt.10000)then
         NCALL=10000
      endif

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

      prec=prec*1d-2

 777  if(dabs(sd/avgi).gt.prec)then

         it=it+1
         if(inccall.lt.10000)then
            ncall=ncall+10000
         else
            ncall=ncall+inccall
         endif
         ren=dble(ncall)

         CALL VEGAS2(cs,AVGI,SD,CHI2A)

         if(it.gt.itend)goto 778

         goto 777

      endif

C
 778  print*,''
      write(6,301)avgi,sd
      print*,''

C     for superchicrun.F
      ncall=ncall1

 301  format(' Cross section  = ',G16.7,' +/-',G16.7,' pb')

      call cpu_time(t2)
C      print*,'time elapsed = ', t2, ' s'
      iseed=(10000*t2)

      return
      end
