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
      include 'diff.f'
      include 'diss.f'
      include 'enew.f'
      include 'gamma.f'

cccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccccccccccccccccccccccccccccccccccccccccccccccccc

ccccccccccccccccccccccc
c main/superchic.f
      call length(outtag,outl)

      open(45,file='evrecs/evrec'//outtag(1:outl)//'.dat')

ccccccccccccccccccccccc
c main/superchic.f

      unw=.true.
      calcmax=.true.

cccccccccccccc

 999  if(genunw)then
C        avgio=avgi
C        sdo=sd

C         print*,''
C         print*,'*****************************'
C         print*,'Generating unweighted events'
C         print*,'*****************************'
C         print*,''
         NPRN=0
         
         ncall=nev*500
         if(ncall.gt.ncall1)ncall=ncall1
 566     ren=dble(ncall)
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
         
 100     format('  generated events so far = ',i7,'    total = ',i7)

C
         if(evnum.lt.nev)then
            if(inccall.lt.nev*500)then
               ncall=ncall+inccall
            else
               ncall=ncall+nev*500
            endif
            goto 566
         endif
      
         xsecup(1)=avgi
         xerrup(1)=sd
         xmaxup(1)=avgi
         
         if(enew)then
            call unwprintq
         else
            call unwprint
         endif
         
      endif

      close(45)

ccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      call cpu_time(t2)
C      print*,'time elapsed = ', t2, ' s'
      iseed=(10000*t2)

      return
      end
