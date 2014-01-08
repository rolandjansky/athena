!
!> \author SaMuSoG
!
 subroutine groupchb(nbgrp,nbchb,chchb,iprint)
 IMPLICIT REAL(8) (A-H,O-Z)
!
!> Grouping of stations for Serguei Baranov !
!
!  -meanning of output quantities is written down when iprint=1
!  -output:
!      integer     nbgrp
!      integer     nbchb(100)
!      character*3 chchb(100,100)
!
#include "AmdcStand/amdcsim_com.inc"
!
 character(3) :: cartyploc(mtyp)
 character(3) :: chprint(30)
 integer      :: ivalichb(mtyp)
 integer      :: idgrp(mtyp)
 integer      :: nbgrp
 integer      :: nbchb(100)
 character(3) ::  chchb(100,100)
!
! select the existing stations
      kount=0
      do jtyp=1,mtyp
       if (cartyp(jtyp).ne.'XXX') then
        ifound=0
        do jff=1,8
        do jzz=-mzz,mzz
         if (igeo_v(jtyp_f_z(jtyp,jff,jzz)) /= 0) ifound=1
        enddo
        enddo
        if (ifound.eq.1) then
         kount=kount+1
         cartyploc(kount)=cartyp(jtyp)
         ivalichb(kount) =1
         idgrp(kount)    =0
        endif
       endif
      enddo
      mtyploc=kount
!
! group them
      kount=0
      do ibe=1,5
      do jimo=1,4
      do ksl=1,2
       kount=kount+1
       ifound=0
       do jtyp=1,mtyploc
        if (ivalichb(jtyp).eq.1) then
!
        iok=0
        if( ibe.eq.1.and. cartyploc(jtyp)(1:1).eq.'B' ) iok=1
        if( ibe.eq.2.and. ( cartyploc(jtyp)(1:1).eq.'E'.or.(cartyploc(jtyp)(1:1).eq.'F'.and.jimo.ne.4)) ) iok=1
        if( ibe.eq.3.and. ( cartyploc(jtyp)(1:1).eq.'F'.or.(cartyploc(jtyp)(1:1).eq.'E'.and.jimo.ne.4)) ) iok=1
        ioki=iok
!
        iok=0
        if( jimo.eq.1.and. cartyploc(jtyp)(2:2).eq.'I' ) iok=1
        if( jimo.eq.2.and. cartyploc(jtyp)(2:2).eq.'E' ) iok=1
        if( jimo.eq.3.and. cartyploc(jtyp)(2:2).eq.'M' ) iok=1
        if( jimo.eq.4.and. cartyploc(jtyp)(2:2).eq.'O' ) iok=1
        iokj=iok
!
        iok=0
        if( ksl.eq.1 .and. ( cartyploc(jtyp)(3:3).eq.'S'.or. cartyploc(jtyp)(3:3).eq.'F' .or.     &
                             cartyploc(jtyp)(3:3).eq.'G'.or. cartyploc(jtyp)(3:3).eq.'H') ) iok=1
        if( ksl.eq.2.and. ( cartyploc(jtyp)(3:3).eq.'L'.or.(cartyploc(jtyp)(3:3).eq.'C'.and.jimo.ne.4)) ) iok=1
        iokk=iok
!
        if (ibe.eq.4.and. cartyploc(jtyp)(1:1).eq.'T') then
         ioki=1
         iokj=1
         iokk=1
        endif
!
        if (ibe.eq.5.and. cartyploc(jtyp)(1:1).eq.'C') then
         ioki=1
         iokj=1
         iokk=1
        endif
!
        if (ioki*iokj*iokk.eq.1) then
         ifound=1
         ivalichb(jtyp)=0
         idgrp(jtyp)=kount
        endif
!
        endif
       enddo
       if (ifound.eq.0) kount=kount-1
      enddo
      enddo
      enddo
      nbgrp=kount
!
!patch for the EOC
      idoit=0
      do jtyp=1,mtyploc
       if (cartyploc(jtyp).eq.'EOC') then
        idoit=1
        jtypk=jtyp
       endif
      enddo
      if (idoit.eq.1) then
       do jtyp=1,mtyploc
        if (cartyploc(jtyp).eq.'FOL') then
         idgrp(jtypk)=idgrp(jtyp)
         ivalichb(jtypk)=0
        endif
       enddo
      endif
!
!fill outputs arrays
      do jnbgrp=1,nbgrp
       nbchb(jnbgrp)=0
       do jtyp=1,mtyploc
        if (idgrp(jtyp).eq.jnbgrp) then
         nbchb(jnbgrp)=nbchb(jnbgrp)+1
         chchb(jnbgrp,nbchb(jnbgrp))=cartyploc(jtyp)
        endif
       enddo
      enddo
!
! include lonely stations
      do jtypl=1,mtyploc
       jtyp=mtyploc+1-jtypl
       if (ivalichb(jtyp).eq.1) then
        iposk=nbgrp+1
        do jnbgrp=1,nbgrp
         do ich=1,nbchb(jnbgrp)
          if (cartyploc(jtyp)(1:1).eq.'B') then
           if (chchb(jnbgrp,ich)(1:1).eq.'B') iposk=jnbgrp+1
          endif
          if (cartyploc(jtyp)(1:1).eq.'E' .or.cartyploc(jtyp)(1:1).eq.'F') then
           if (chchb(jnbgrp,ich)(1:1).eq.'E' .or.chchb(jnbgrp,ich)(1:1).eq.'F') iposk=jnbgrp+1
          endif
         enddo
        enddo
        do jnbgrpl=iposk,nbgrp
         jnbgrp=nbgrp+iposk-jnbgrpl
         nbchb(jnbgrp+1)=nbchb(jnbgrp)
         do ich=1,nbchb(jnbgrp)
          chchb(jnbgrp+1,ich)=chchb(jnbgrp,ich)
         enddo
        enddo
        nbchb(iposk)  =1
        chchb(iposk,1)=cartyploc(jtyp)
        ivalichb(jtyp)=0
        nbgrp=nbgrp+1
       endif
      enddo
!
      do jtyp=1,mtyploc
       if (ivalichb(jtyp).eq.1) then
        print *,'alone: ',cartyploc(jtyp)
        print *,' something is wrong => stop in groupchb '
        stop
       endif
      enddo
!
      if (iprint.eq.1) then
       print 2200
       print 2300
       print 2400
       print 2500
       nbchbmax=-10
       do jnbgrp=1,nbgrp
        if(nbchb(jnbgrp).ge.nbchbmax) nbchbmax=nbchb(jnbgrp)
       enddo
       print 2000,(jnbgrp,jnbgrp=1,nbgrp)
       do ich=1,nbchbmax
        do jnbgrp=1,nbgrp
         chprint(jnbgrp)='   '
         if (ich.le.nbchb(jnbgrp)) chprint(jnbgrp)=chchb(jnbgrp,ich)
        enddo
        print 2100,(chprint(jnbgrp),jnbgrp=1,nbgrp)
       enddo
      endif
2000  format('jnbgrp',30(1x,i3))
2100  format('ich   ',30(1x,a3))
2200  format('Below are shown chchb(jnbgrp,ich)    ')
2300  format(' jnbgrp horizontaly, ich vertically  ')
2400  format('  jnbgrp goes from 1 to nbgrp        ')
2500  format('  ich    goes from 1 to nbchb(jnbgrp)')
!
 END SUBROUTINE GROUPCHB
!
