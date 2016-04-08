      subroutine usemadcup
      implicit none
     
#include "GeneratorFortranCommon/heprup.inc"      
#include "GeneratorFortranCommon/hepeup.inc"
#include "MadCUP_i/hepup.inc"
#include "MadCUP_i/iofiles.inc"

      integer istg
      common /evtcon/ istg
      
c     local variables 
      logical interfacedebug
      parameter (interfacedebug=.false.)
  
C      integer i,ii,j,k,ios,ievt,ProcNum,np,numMother
      integer i,j,k,ievt,ProcNum,np
      data ievt /0/
      save ievt
      logical ldebug
      parameter (ldebug=.false.)

c      double precision tpup(20,5), tspinup(20)
      integer id, tidup(20)


c  read in an event
      read(unitnumber,'(5i7,4g15.8)',END=100) ievt,idprup,nup,
     &                         colorprocess,
     &                         decayprocess,
     &                         xwgtup,SCALUP, AQEDUP, AQCDUP
      if (nup.eq.0) then
         write(*,*) 'no more events in data file'
         return
      endif


      if (ldebug) then
         if(idprup.ne.lidprup(decayprocess) .or.
     &        idprup.ne.lidprup(colorprocess+numdecay) ) then
            write(*,*) ' process number mismatch in UPEVNT '
            write(*,*) lidprup(decayprocess),' in decay Table '
            write(*,*) lidprup(colorprocess+numdecay),' in color Table '
            write(*,*) idprup,' in event data file '
         endif

         if(decayprocess.gt.numdecay) then
            write(*,*) 'error: decayprocess out of range in event',ievt
            write(*,*) 'decayprocess = ',decayprocess
         endif
         if(colorprocess.gt.numcolor) then
            write(*,*) 'error: colorprocess out of range in event',ievt
            write(*,*) 'colorprocess = ',colorprocess
         endif
      endif

      ProcNum=colorprocess+numdecay
      np=numentry(procnum)
      do i=1,2
        read(unitnumber,'(i3,g18.8,g18.4,f4.0,i6)')
     &      id,(pup(j,i),j=3,3),pup(5,i),spinup(i),tidup(i)
        pup(1,i)=0
        pup(2,i)=0
        pup(4,i)=sqrt(pup(5,i)**2+pup(3,i)**2)
        istup(i)=listup(i,ProcNum)
        mothup(1,i)=lmothup(1,i,ProcNum)
        mothup(2,i)=lmothup(2,i,ProcNum)
        icolup(1,i)=licolup(1,i,ProcNum)
        icolup(2,i)=licolup(2,i,ProcNum)
        vtimup(i)=0
        idup(id)=lidup(id,ProcNum)
        if( idup(id).ne.tidup(i)) then
           write(*,*) ' mismatch of idup for event ',ievt,' id = ',id
        endif
      enddo
      
      do i=3,nup   
         if (i.le.np) then
            ProcNum = colorprocess+numdecay
         else
            ProcNum = decayprocess
         endif
         istup(i)=listup(i,ProcNum)
         mothup(1,i)=lmothup(1,i,ProcNum)
         mothup(2,i)=lmothup(2,i,ProcNum)
         icolup(1,i)=licolup(1,i,ProcNum)
         icolup(2,i)=licolup(2,i,ProcNum)
         vtimup(i)=0
         idup(i)=lidup(i,ProcNum) 
         if(abs(istup(i)).eq.1) then
            read(unitnumber,'(i3,3g18.8,g18.4,f4.0,i6)')
     &          id,(pup(j,i),j=1,3),pup(5,i),spinup(i),tidup(i)
            pup(4,i)=sqrt(pup(5,i)**2+pup(1,i)**2+
     &                    pup(2,i)**2+pup(3,i)**2)
            if( idup(i).ne.tidup(i)) then
               write(*,*) 'mismatch of idup for event ',
     &                    ievt,' id = ',id,i
               print*,decayprocess,colorprocess
               write(*,*) ' idup = ',idup(i)
               write(*,*) ' tidup = ',tidup(i)
            endif
         else  !data for this particle was not stored; 
               !reset momentum for treatment below
            do j=1,5
               pup(j,i)=0
            enddo
         endif
      enddo

c fill momenta of mothers corresponding to decaying particles
      do i=3,nup
         k = mothup(1,i)
         if ( (k.eq.mothup(2,i) .or. mothup(2,i).eq.0)
     &        .and. k.ge.3 .and. abs(istup(k)).ne.1) then
            do j = 1,4
               pup(j,k) = pup(j,k) + pup(j,i)
            enddo
         endif
      enddo
      do i=3,nup
         if (abs(istup(i)).ne.1) then
            pup(5,i) = sqrt(pup(4,i)**2-pup(1,i)**2-
     &                       pup(2,i)**2-pup(3,i)**2)
         endif
      enddo


      if(interfacedebug) then
        call printHEPUPcontents
      endif
      
      return

 100  nup = 0
      istg = 1
      return
      
      end
