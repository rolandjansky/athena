!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE insTGCa(jsta,iret,iprint,                                         &
                    nbevol,widchb,fwirch,fwixch, namvol,icovol,zpovol,widvol)
 IMPLICIT REAL(8) (A-H,O-Z)
 integer       :: jsta,iret,iprint
 integer       :: nbevol
 character(11) :: namvol(30)
 integer       :: icovol(30)
 real(8)       :: zpovol(30),widvol(30)
 character(11) ::  namsta(10)
 integer       :: i,iv
 integer, save :: jtec=0
 logical, save :: first=.true.
!--------------------------------------------------------------------
!
! extraction of TGC inner structure parameters (in cm)
!  for stacking index type JSTA
!
!  -meanning of output quantities is written down when iprint=1
!  -if extraction is sucessfull iret=1 (=0 if not)
!  -output:
!      integer nbevol
!      character*11 namvol(30)
!      integer icovol(30)
!      dimension  zpovol(30),widvol(30)
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
 data namsta/ ' tgc gas   ', ' g10       ',  ' honeycomb ',  &
              ' copper    ', ' carbon    ',5*' undefined '/
!
      if (first) then
       first=.false.
       do i=1,mtec
        if (cartec(i).eq.'TGC') jtec=i
       enddo
       if (jtec.eq.0) then
        print *,'**Pb in INSTGC: jtec index not found'
        print *,'====> STOP'
        stop
       endif
      endif
!
      iret=0
!
      if (jsta.gt.msta) then
       print *,'*Wrong call to INSTGC with jsta=',jsta
       print *,'  the maximal value of jsta is  ',msta
       return
      endif
!
      if (jsta.lt.1) then
       print *,'*Wrong call to INSTGC with jsta=',jsta
       print *,'  the minimal value of jsta is 1'
       return
      endif
!
      if (iforma(jtec,jsta).eq.0) then
       print *,'*Wrong call to INSTGC with jsta ',jsta
       print *,'  this is a undefined index of stacking type'
       return
      endif
!
! TGC formats for which no extraction is prepared
      if (iforma(jtec,jsta).ne.2) then
        if (iprint.eq.1) then
          print *,' '
          print 1000,jsta
          print *,'==> extraction of TGC parameters not prepared'
          print *,'==>  for this TGC format'
          print *,' '
        endif
      endif
!
! Second TGC format
      if (iforma(jtec,jsta).eq.2) then
      iret=1
      nbevol=nlaz(jtec,jsta)
      widchb=staee(jtec,jsta)/10.
      fwirch=starr(jtec,jsta)/10.
      fwixch=stapp(jtec,jsta)/10.
      do iv=1,nlaz(jtec,jsta)
        namvol(iv)=namsta(istama(jtec,jsta,iv))
        icovol(iv)=istama(jtec,jsta,iv)
        zpovol(iv)=statt(jtec,jsta,iv)/10.
        widvol(iv)=staoo(jtec,jsta,iv)/10.
      enddo
      if (iprint.eq.1) then
       print *,' '
       print 1000,jsta
       print 1002,'widchb= ',widchb ,' Width of the chber along z     '
       print 1002,'fwirch= ',fwirch ,' Frame width in r               '
       print 1002,'fwixch= ',fwixch ,' Frame width in x               '
       print 1003,'nbevol= ',nbevol ,' nber of subvolumes             '
       print 2000
       print 2002
       do iv=1,nbevol
         print 1004,iv,icovol(iv),namvol(iv),zpovol(iv),widvol(iv)
       enddo
       print *,' '
      endif
      endif
!
1000  format('*TGC inner structure parameters (in cm):'/' jsta=',I4)
1002  format(1x,A8,F10.3,A32)
1003  format(1x,A8,  I10,A32)
1004  format(8x,i4,8x,i4,8x,a11,3x,f10.3,4x,f10.3)
2000  format('  volume nber | code Material | Material name',   &
             ' | relat. z pos. | z width   ')
2002  format('        iv    | icovol(iv)    | namvol(iv)   ',   &
             ' | zpovol(iv)    | widvol(iv)')
!
 END SUBROUTINE insTGCa
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 subroutine insSUPa(jsta,iret,  nxxsup,nzzsup,xxsup,zzsup)
 IMPLICIT REAL(8) (A-H,O-Z)
 integer :: jsta,iret,nxxsup,nzzsup
 real(8) ::  xxsup(*),zzsup(*)
 integer :: i
 integer, save :: jtec=0
 logical, save :: first=.true.
!--------------------------------------------------------------------
!
!  XXi = xxsup(i)
!  ZZi = zzsup(i)
!
!  Origin is at O
!
!--------------------------------------------------------------------
!
!  nxxsup = 2
!  nzzsup = 1
!
!       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@. . . . . .
!       @  Aluminium                      @    ^
!       @                                 @    | XX1
!       @     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@. . v . . .
!       @     @                        ^
!       @     @                        |
!       @     @                        |
!       @     @                        |
!       @     @                        |
!       @     @                        |
!       @     @                        |
!       @     @                        | XX2
!       @     @                        |
!       @     @                        |
!       @     @                        |
!       @     @                        |
!       @     @                        |
!       @     @                        |
!       @     @                        v
!       @     O@@@@@@@@@@@@@@@@@@@@@@@@@@@@. . . . . .
!       @     .                           @    ^
!       @     .                           @    | XX1
!       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@. . v . . .
!       .     .                           .
!       .     .                           .
!       <-----><-------------------------->
!       . XX1 .            ZZ1            .
!       .     .                           .
!
!--------------------------------------------------------------------
!
!  nxxsup = 4
!  nzzsup = 4                                   .     .
!                                               <----->
!                                               . ZZ1 .
!                                               .     .
!. . . . . . . . . . . . . . . . .@@@@@@@@@@@@@@@@@@@@@
!  ^                              @             .     @
!  |                              @             .     @
!  | XX2                          @             .     @
!  |     . . . . . .@@@@@@@@@@@@@@@             .     @@@@@@@@@@@@@@@
!  |       ^        @                Aluminium  .                   @
!  |       | XX1    @                           .                   @
!. v . . . v . . . .@@@@@@@@@@@@@@@@@@@@@       @@@@@@@@@@@@@@@@@@@@@. . . .
!              ^    .                   @       @                   .  ^
!              |    <------------------>@       @<------------------>  |
!              |    .       ZZ2         @       @        ZZ2        .  |
!              |    .                   @       @                   .  |
!              |                        @<----->@                      |
!              |                        @  ZZ4  @                      |
!              | XX3                    @       @                      |
!              |       .                @       @                      | XX4
!              |       <--------------->@       @                      |
!              |       .       ZZ3      @       @                      |
!              |       .                @       @                      |
!  . . . . . . v . . . @@@@@@@@@@@@@@@@@@       @                      |
!      ^               @                        @                      |
!      |               @                        @                      |
!      |               @                        @                      |
!      |               @       @@@@@@@@@@@@@@@@@@. . . . . . . . . . . v . .
!      |               @       @                .         ^
!      |               @       @<--------------->         |
!      |               @       @       ZZ3      .         |
!      | XX4           @       @                .         |
!      |               @<----->@                          |
!      |               @  ZZ4  @                          | XX3
!      |               @       @                   .      |
!      |               @       @                   .      |
!      |               @       @<------------------>      |
!      |               @       @         ZZ2       .      |
!      v               @       @                   .      |
!  @@@@@@@@@@@@@@@@@@@@O       @@@@@@@@@@@@@@@@@@@@@. . . v . . . . .
!  @                   .                           @  ^        ^
!  @                   .                           @  | XX1    |
!  @@@@@@@@@@@@@@@     .             @@@@@@@@@@@@@@@. v . .    |
!                @     .             @                         | XX2
!                @     .             @                         |
!                @     .             @                         |
!                @@@@@@@@@@@@@@@@@@@@@ . . . . . . . . . . . . v . .
!                .     .
!                .     .
!                <----->
!                . ZZ1 .
!
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
      if (first) then
        first=.false.
        do i=1,mtec
         if (cartec(i).eq.'SUP') jtec=i
        enddo
        if (jtec.eq.0) then
          print *,'==> Pb in INSSUP: jtec index not found'
          print *,'======> STOP !'
          stop
        endif
      endif
!
      iret=0
!
      if (jsta.gt.msta) then
        print *,'==> Wrong call to INSSUP with jsta =',jsta
        print *,'==> The maximal value of jsta is ',msta
        return
      elseif (jsta.lt.1) then
        print *,'==> Wrong call to INSSUP with jsta =',jsta
        print *,'==> the minimal value of jsta is 1'
        return
      endif
!
!-- SUPport of rpc format !
      if (iforma(jtec,jsta).eq.1 ) then
        iret=1
        nxxsup = nlas(jtec,jsta)
        nzzsup = nlaz(jtec,jsta)
        do i=1,nxxsup
         xxsup(i) = statt(jtec,jsta,i)/10.D0
        enddo
        do i=1,nzzsup
         zzsup(i) = staoo(jtec,jsta,i)/10.D0
        enddo
      else
        print *,'==> jsta, iforma =',jsta,iforma(jtec,jsta)
        print *,'==> extraction of SUP parameters not prepared'
        print *,'==> for this SUP format !'
      endif
!
 END SUBROUTINE insSUPa
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 subroutine insRPCb(jsta,iret,iprint,                           &
                    ttotal,layrpc,trpcsu,tuphcb,auphcb,tckrla,  &
                    tckfsp,ackfsp,tlohcb,alohcb,spitch,         &
                    zpitch,dedstr,tckbak,tckgas,tckssu,         &
                    tckstr,sdedmi,zdedmi,spdiam,sppitc,         &
                    stroff,nsrest,nzrest,wrpcsu,                &
                    sfirst,zfirst,dedsep,nsrost,nzrost)
 IMPLICIT REAL(8) (A-H,O-Z)
 integer :: jsta,iret,iprint
 integer :: layrpc,nsrest,nzrest
 real(8) :: stroff(3)
 integer :: i
 integer, save :: jtec=0
 logical, save :: first=.true.
!--------------------------------------------------------------------
!
! extraction of RPC inner structure parameters (in cm)
!  for stacking index type JSTA
!
!  -meanning of output quantities is written down when iprint=1
!  -if extraction is sucessfull iret=1 (=0 if not)
!  -array:
!     integer layrpc,nsrest,nzrest
!     dimension stroff(3)
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
      if (first) then
       first=.false.
       do i=1,mtec
        if (cartec(i).eq.'RPC') jtec=i
       enddo
       if (jtec.eq.0) then
        print *,'**Pb in INSRPC: jtec index not found'
        print *,'====> STOP'
        stop
       endif
      endif
!
      iret=0
!
      if (jsta.gt.msta) then
       print *,'*Wrong call to INSRPC with jsta=',jsta
       print *,'  the maximal value of jsta is  ',msta
       return
      endif
!
      if (jsta.lt.1) then
       print *,'*Wrong call to INSRPC with jsta=',jsta
       print *,'  the minimal value of jsta is 1'
       return
      endif
!
      if (iforma(jtec,jsta).eq.0) then
       print *,'*Wrong call to INSRPC with jsta ',jsta
       print *,'  this is a undefined index of stacking type'
       return
      endif
!
! RPC formats for which no extraction is prepared
      if (iforma(jtec,jsta).ne.2 .and. iforma(jtec,jsta).ne.4) then
      if (iprint.eq.1) then
       print *,' '
       print 10,jsta
       print *,'==> extraction of RPC parameters not prepared'
       print *,'==>  for this RPC format'
       print *,' '
      endif
      endif
!
! second RPC format
      if (iforma(jtec,jsta).eq.2 .or. iforma(jtec,jsta).eq.4) then
      iret=1
      ttotal =staee(jtec,jsta)/10.    ! total thickness
      layrpc =max( nlaz(jtec,jsta) , nlas(jtec,jsta) )  ! nber of RPCs layers
      trpcsu =statt(jtec,jsta,1)/10.  ! thick. of RPC support
      wrpcsu =staoo(jtec,jsta,6)/10.  ! width  of RPC support
      tuphcb =statt(jtec,jsta,2)/10.  ! thick. of             Upper Honeycomb
      auphcb =statt(jtec,jsta,3)/10.  ! thick. of Al plate of Upper Honeycomb
      tckrla =statt(jtec,jsta,4)/10.  ! thick. of an RPC layer
      tckfsp =statt(jtec,jsta,5)/10.  ! thick. of             foam spacer
      ackfsp =statt(jtec,jsta,6)/10.  ! thick. of Al plate of foam spacer
      tlohcb =statt(jtec,jsta,7)/10.  ! thick. of             Lower Honeycomb
      alohcb =statt(jtec,jsta,8)/10.  ! thick. of Al plate of Lower Honeycomb
      spitch =statt(jtec,jsta,10)/10. ! s-strips pitch
      zpitch =statt(jtec,jsta,11)/10. ! z-strips pitch
      dedstr =statt(jtec,jsta,12)/10. ! dead reagion between strip
      tckbak =statt(jtec,jsta,13)/10. ! thick. of bakelite
      tckgas =statt(jtec,jsta,14)/10. ! thick. of gas gap
      tckssu =statt(jtec,jsta,15)/10. ! thick. of strips support
      tckstr =statt(jtec,jsta,16)/10. ! thick. of strips
      sdedmi =statt(jtec,jsta,17)/10. ! s internal mid-chber dead region
      zdedmi =statt(jtec,jsta,18)/10. ! z internal mid-chber dead region
      spdiam =statt(jtec,jsta,19)/10. ! spacer diameter
      sppitc =statt(jtec,jsta,20)/10. ! spacer pitch
      stroff(1)=staoo(jtec,jsta,1)/10.! strip offset s
      stroff(2)=staoo(jtec,jsta,4)/10.! strip offset first z
      stroff(3)=staoo(jtec,jsta,5)/10.! strip offset second z
      nsrest =NINT(staoo(jtec,jsta,2)) ! nber of s strips readouts
      nzrest =NINT(staoo(jtec,jsta,3)) ! nber of z strips readouts
      sfirst =     staoo(jtec,jsta,11)/10. ! s-phi strip offset
      zfirst =     staoo(jtec,jsta,12)/10. ! z-eta strip offset
      dedsep =     staoo(jtec,jsta,13)/10. ! dead separation
      nsrost =NINT(staoo(jtec,jsta,14))    ! number of s-phi readout strips
      nzrost =NINT(staoo(jtec,jsta,15))    ! number of z-eta readout strips
      if (iprint.eq.1) then
      print *,' '
      print 10,jsta
      print 12,'ttotal',ttotal,'total thickness                      '
      print 13,'layrpc',layrpc,'nber of RPCs layers                  '
      print 12,'trpcsu',trpcsu,'thick. of RPC support                '
      print 12,'wrpcsu',wrpcsu,'width  of RPC support                '
      print 12,'tuphcb',tuphcb,'thick. of             Upper Honeycomb'
      print 12,'auphcb',auphcb,'thick. of Al plate of Upper Honeycomb'
      print 12,'tckrla',tckrla,'thick. of an RPC layer               '
      print 12,'tckfsp',tckfsp,'thick. of             foam spacer    '
      print 12,'ackfsp',ackfsp,'thick. of Al plate of foam spacer    '
      print 12,'tlohcb',tlohcb,'thick. of             Lower Honeycomb'
      print 12,'alohcb',alohcb,'thick. of Al plate of Lower Honeycomb'
      print 12,'spitch',spitch,'s-strips pitch                       '
      print 12,'zpitch',zpitch,'z-strips pitch                       '
      print 12,'dedstr',dedstr,'dead reagion between strip           '
      print 12,'tckbak',tckbak,'thick. of bakelite                   '
      print 12,'tckgas',tckgas,'thick. of gas gap                    '
      print 12,'tckssu',tckssu,'thick. of strips support             '
      print 12,'tckstr',tckstr,'thick. of strips                     '
      print 12,'sdedmi',sdedmi,'s internal mid-chber dead region     '
      print 12,'zdedmi',zdedmi,'z internal mid-chber dead region     '
      print 12,'spdiam',spdiam,'spacer diameter                      '
      print 12,'sppitc',sppitc,'spacer pitch                         '
      print 14,'stroff(1)',stroff(1),'strip offset s                       '
      print 14,'stroff(2)',stroff(3),'strip offset first z                 '
      print 14,'stroff(3)',stroff(3),'strip offset second z                '
      print 12,'sfirst',sfirst,'s-phi strip offset                   '
      print 12,'zfirst',zfirst,'z-eta strip offset                   '
      print 12,'dedsep',dedsep,'dead separation                      '
      print 13,'nsrest',nsrest,'nber of s strips readouts            '
      print 13,'nzrest',nzrest,'nber of z strips readouts            '
      print 13,'nsrost',nsrost,'nber of s-phi readout strips         '
      print 13,'nzrost',nzrost,'nber of z-eta readout strips         '
      print *,' '
      endif
      endif
!
10    format('*RPC inner structure parameters (in cm):'/' jsta=',I4)
12    format(1x,A6,' = ',F10.3,1x,A37)
13    format(1x,A6,' = ',I10  ,1x,A37)
14    format(1x,A9,' = ',F10.3,1x,A37)
!
 END SUBROUTINE insRPCb
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 subroutine insRPCa(jsta,iret,iprint,                           &
                    ttotal,layrpc,trpcsu,tuphcb,auphcb,tckrla,  &
                    tckfsp,ackfsp,tlohcb,alohcb,spitch,         &
                    zpitch,dedstr,tckbak,tckgas,tckssu,         &
                    tckstr,sdedmi,zdedmi,spdiam,sppitc,         &
                    stroff,nsrest,nzrest,wrpcsu)
 IMPLICIT REAL(8) (A-H,O-Z)
 integer :: jsta,iret,iprint
 integer :: layrpc,nsrest,nzrest
 real(8) :: stroff(3)
 integer :: i
 integer, save :: jtec=0
 logical, save :: first=.true.
!--------------------------------------------------------------------
!
! extraction of RPC inner structure parameters (in cm)
!  for stacking index type JSTA
!
!  -meanning of output quantities is written down when iprint=1
!  -if extraction is sucessfull iret=1 (=0 if not)
!  -array:
!     integer layrpc,nsrest,nzrest
!     dimension stroff(3)
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
      if (first) then
       first=.false.
       do i=1,mtec
        if (cartec(i).eq.'RPC') jtec=i
       enddo
       if (jtec.eq.0) then
        print *,'**Pb in INSRPC: jtec index not found'
        print *,'====> STOP'
        stop
       endif
      endif
!
      iret=0
!
      if (jsta.gt.msta) then
       print *,'*Wrong call to INSRPC with jsta=',jsta
       print *,'  the maximal value of jsta is  ',msta
       return
      endif
!
      if (jsta.lt.1) then
       print *,'*Wrong call to INSRPC with jsta=',jsta
       print *,'  the minimal value of jsta is 1'
       return
      endif
!
      if (iforma(jtec,jsta).eq.0) then
       print *,'*Wrong call to INSRPC with jsta ',jsta
       print *,'  this is a undefined index of stacking type'
       return
      endif
!
! RPC formats for which no extraction is prepared
      if (iforma(jtec,jsta).ne.2 .and. iforma(jtec,jsta).ne.4) then
      if (iprint.eq.1) then
       print *,' '
       print 10,jsta
       print *,'==> extraction of RPC parameters not prepared'
       print *,'==>  for this RPC format'
       print *,' '
      endif
      endif
!
! second RPC format
      if (iforma(jtec,jsta).eq.2 .or. iforma(jtec,jsta).eq.4) then
      iret=1
      ttotal =staee(jtec,jsta)/10.    ! total thickness
      layrpc =max( nlaz(jtec,jsta) , nlas(jtec,jsta) )  ! nber of RPCs layers
      trpcsu =statt(jtec,jsta,1)/10.  ! thick. of RPC support
      wrpcsu =staoo(jtec,jsta,6)/10.  ! width  of RPC support
      tuphcb =statt(jtec,jsta,2)/10.  ! thick. of             Upper Honeycomb
      auphcb =statt(jtec,jsta,3)/10.  ! thick. of Al plate of Upper Honeycomb
      tckrla =statt(jtec,jsta,4)/10.  ! thick. of an RPC layer
      tckfsp =statt(jtec,jsta,5)/10.  ! thick. of             foam spacer
      ackfsp =statt(jtec,jsta,6)/10.  ! thick. of Al plate of foam spacer
      tlohcb =statt(jtec,jsta,7)/10.  ! thick. of             Lower Honeycomb
      alohcb =statt(jtec,jsta,8)/10.  ! thick. of Al plate of Lower Honeycomb
      spitch =statt(jtec,jsta,10)/10. ! s-strips pitch
      zpitch =statt(jtec,jsta,11)/10. ! z-strips pitch
      dedstr =statt(jtec,jsta,12)/10. ! dead reagion between strip
      tckbak =statt(jtec,jsta,13)/10. ! thick. of bakelite
      tckgas =statt(jtec,jsta,14)/10. ! thick. of gas gap
      tckssu =statt(jtec,jsta,15)/10. ! thick. of strips support
      tckstr =statt(jtec,jsta,16)/10. ! thick. of strips
      sdedmi =statt(jtec,jsta,17)/10. ! s internal mid-chber dead region
      zdedmi =statt(jtec,jsta,18)/10. ! z internal mid-chber dead region
      spdiam =statt(jtec,jsta,19)/10. ! spacer diameter
      sppitc =statt(jtec,jsta,20)/10. ! spacer pitch
      stroff(1)=staoo(jtec,jsta,1)/10.! strip offset s
      stroff(2)=staoo(jtec,jsta,4)/10.! strip offset first z
      stroff(3)=staoo(jtec,jsta,5)/10.! strip offset second z
      nsrest =NINT(staoo(jtec,jsta,2)) ! nber of s strips readouts
      nzrest =NINT(staoo(jtec,jsta,3)) ! nber of z strips readouts
      if (iprint.eq.1) then
      print *,' '
      print 10,jsta
      print 12,'ttotal',ttotal,'total thickness                      '
      print 13,'layrpc',layrpc,'nber of RPCs layers                  '
      print 12,'trpcsu',trpcsu,'thick. of RPC support                '
      print 12,'wrpcsu',wrpcsu,'width  of RPC support                '
      print 12,'tuphcb',tuphcb,'thick. of             Upper Honeycomb'
      print 12,'auphcb',auphcb,'thick. of Al plate of Upper Honeycomb'
      print 12,'tckrla',tckrla,'thick. of an RPC layer               '
      print 12,'tckfsp',tckfsp,'thick. of             foam spacer    '
      print 12,'ackfsp',ackfsp,'thick. of Al plate of foam spacer    '
      print 12,'tlohcb',tlohcb,'thick. of             Lower Honeycomb'
      print 12,'alohcb',alohcb,'thick. of Al plate of Lower Honeycomb'
      print 12,'spitch',spitch,'s-strips pitch                       '
      print 12,'zpitch',zpitch,'z-strips pitch                       '
      print 12,'dedstr',dedstr,'dead reagion between strip           '
      print 12,'tckbak',tckbak,'thick. of bakelite                   '
      print 12,'tckgas',tckgas,'thick. of gas gap                    '
      print 12,'tckssu',tckssu,'thick. of strips support             '
      print 12,'tckstr',tckstr,'thick. of strips                     '
      print 12,'sdedmi',sdedmi,'s internal mid-chber dead region     '
      print 12,'zdedmi',zdedmi,'z internal mid-chber dead region     '
      print 12,'spdiam',spdiam,'spacer diameter                      '
      print 12,'sppitc',sppitc,'spacer pitch                         '
      print 14,'stroff(1)',stroff(1),'strip offset s                       '
      print 14,'stroff(2)',stroff(3),'strip offset first z                 '
      print 14,'stroff(3)',stroff(3),'strip offset second z                '
      print 13,'nsrest',nsrest,'nber of s strips readouts            '
      print 13,'nzrest',nzrest,'nber of z strips readouts            '
      print *,' '
      endif
      endif
!
10    format('*RPC inner structure parameters (in cm):'/' jsta=',I4)
12    format(1x,A6,' = ',F10.3,1x,A37)
13    format(1x,A6,' = ',I10  ,1x,A37)
14    format(1x,A9,' = ',F10.3,1x,A37)
!
 END SUBROUTINE insRPCa
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 subroutine insMDTa(jsta,iret,iprint,                          &
                    laymdt,tubpit,tubrad,tubsta,tubdea,tubwal, &
                    tubxco,tubyco)
 IMPLICIT REAL(8) (A-H,O-Z)
 integer :: jsta,iret,iprint
 integer :: laymdt
 real(8) :: tubxco(10),tubyco(10)
 integer :: i,l
 integer, save :: jtec=0
 logical, save :: first=.true.
!--------------------------------------------------------------------
!
! extraction of MDT inner structure parameters (in cm)
!  for stacking index type JSTA
!
!  -meanning of output quantities is written down when iprint=1
!  -if extraction is sucessfull iret=1 (=0 if not)
!  -output:
!      integer laymdt
!      dimension tubxco(10),tubyco(10)
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
      if (first) then
       first=.false.
       do i=1,mtec
        if (cartec(i).eq.'MDT') jtec=i
       enddo
       if (jtec.eq.0) then
        print *,'**Pb in INSMDT: jtec index not found'
        print *,'====> STOP'
        stop
       endif
      endif
!
      iret=0
!
      if (jsta.gt.msta) then
       print *,'*Wrong call to INSMDT with jsta=',jsta
       print *,'  the maximal value of jsta is  ',msta
       return
      endif
!
      if (jsta.lt.1) then
       print *,'*Wrong call to INSMDT with jsta=',jsta
       print *,'  the minimal value of jsta is 1'
       return
      endif
!
      if (iforma(jtec,jsta).eq.0) then
       print *,'*Wrong call to INSMDT with jsta ',jsta
       print *,'  this is a undefined index of stacking type'
       return
      endif
!
! MDT formats for which no extraction is prepared
      if (iforma(jtec,jsta).ne.1) then
      if (iprint.eq.1) then
       print *,' '
       print 1000,jsta
       print *,'==> extraction of MDT parameters not prepared'
       print *,'==>  for this MDT format'
       print *,' '
      endif
      endif
!
! First MDT format
      if (iforma(jtec,jsta).eq.1) then
      iret=1
      laymdt =nlaz(jtec,jsta)      ! nber of MDTs layers
      tubpit =STAPP(jtec,jsta)/10. ! tubes pitch
      tubrad =STARR(jtec,jsta)/10. ! inner tube radius
      tubsta =STAEE(jtec,jsta)/10. ! total station thickness
      tubdea =STAPG(jtec,jsta)/10. ! dead length in tubes
      do l=1,laymdt
       tubxco(l)=staoo(jtec,jsta,l)/10. ! tube horizontal coordinates
       tubyco(l)=statt(jtec,jsta,l)/10. ! tube vertical   coordinates
      enddo
      tubwal=staoo(jtec,jsta,laymdt+1)/10. ! tube wall thickness
      if (iprint.eq.1) then
       print *,' '
       print 1000,jsta
       print 1003,'laymdt =',laymdt ,' nber of MDTs layers            '
       print 1002,'tubpit =',tubpit ,' tubes pitch                    '
       print 1002,'tubrad =',tubrad ,' inner tube radius              '
       print 1002,'tubsta =',tubsta ,' total station thickness        '
       print 1002,'tubdea =',tubdea ,' dead length in tubes           '
       print 1002,'tubwal =',tubwal ,' tube wall thickness            '
       print 2000
       do l=1,laymdt
        print 2002,l,tubxco(l),tubyco(l)
       enddo
       print *,' '
      endif
      endif
!
1000  format('*MDT inner structure parameters (in cm):'/' jsta=',I4)
1002  format(1x,A8,F10.3,A32)
1003  format(1x,A8,  I10,A32)
2000  format(' Tub nber  horiz. coord.     vert. coord.')
2002  format(i2,1x,5x,f8.2,10x,f8.2)
!
 END SUBROUTINE insMDTa
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 subroutine insLBa(jsta,iret,  Bheight,Bthickn)
 IMPLICIT REAL(8) (A-H,O-Z)
 integer :: jsta,iret
 real(8) :: Bheight,Bthickn
 integer :: i
 integer, save :: jtec=0
 logical, save :: first=.true.
!--------------------------------------------------------------------
!
!  LB  i.e. MDT Long Beam  Structure :
!
!
!     @@@@@@@@@@@@@@@@@@@@@@@@  ^
!     @   Aluminium          @  |
!     @  @@@@@@@@@@@@@@@@@@  @  |
!     @  @                @  @  |
!     @  @                @  @  |
!     @  @                @  @  |
!     @  @  Bthickn       @  @  |
!  -->@  @<---------      @  @  |
!     @  @                @  @  |
!     @  @                @  @  | Bheight
!     @  @                @  @  |
!     @  @                @  @  |
!     @  @                @  @  |
!     @  @                @  @  |
!     @  @                @  @  |
!     @  @                @  @  |
!     @  @@@@@@@@@@@@@@@@@@  @  |
!     @                      @  |
!     @@@@@@@@@@@@@@@@@@@@@@@@  v
!
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
      if (first) then
        first=.false.
        do i=1,mtec
         if (cartec(i).eq.'LB ') jtec=i
        enddo
        if (jtec.eq.0) then
          print *,'==> Pb in InsLB: jtec index not found'
          print *,'======> STOP !'
          stop
        endif
      endif
!
      iret=0
!
      if (jsta.gt.msta) then
        print *,'==> Wrong call to InsLB with jsta =',jsta
        print *,'==> The maximal value of jsta is ',msta
        return
      elseif (jsta.lt.1) then
        print *,'==> Wrong call to InsLB with jsta =',jsta
        print *,'==> the minimal value of jsta is 1'
        return
      endif
!
      if (iforma(jtec,jsta).eq.1 ) then
        iret=1
        Bheight = stapp(jtec,jsta)/10.D0
        Bthickn = starr(jtec,jsta)/10.D0
      else
        print *,'==> jsta, iforma =',jsta,iforma(jtec,jsta)
        print *,'==> extraction of LB parameters not prepared'
        print *,'==> for this LB format !'
      endif
!
 END SUBROUTINE insLBa
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 subroutine insDEDa(jsta,iret,  tckded,auphcb)
 IMPLICIT REAL(8) (A-H,O-Z)
 integer :: jsta,iret
 real(8) :: tckded,auphcb
 integer :: i
 integer, save :: jtec=0
 logical, save :: first=.true.
!--------------------------------------------------------------------
!
!  DED structure
!
!  Origin is at O
!
!       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@. . . . . . .
!       @                         ^               ^
!       @   Aluminium             | auphcb        |
!       @                         v               |
!       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@. . .   |
!       @                                         |
!       @                                         |
!       @                                         |
!       @                                         |
!       @                                         |
!       @   Honeycomb                             | tckded
!       @                                         |
!       @                                         |
!       @                                         |
!       @                                         |
!       @                                         |
!       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@. . .   |
!       @                         ^               |
!       @   Aluminium             | auphcb        |
!       @                         v               |
!       O@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@. . . . v . .
!
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
      if (first) then
        first=.false.
        do i=1,mtec
         if (cartec(i).eq.'DED') jtec=i
        enddo
        if (jtec.eq.0) then
          print *,'==> Pb in INSDED: jtec index not found'
          print *,'======> STOP !'
          stop
        endif
      endif
!
      iret=0
!
      if (jsta.gt.msta) then
        print *,'==> Wrong call to INSDED with jsta =',jsta
        print *,'==> The maximal value of jsta is ',msta
        return
      elseif (jsta.lt.1) then
        print *,'==> Wrong call to INSDED with jsta =',jsta
        print *,'==> the minimal value of jsta is 1'
        return
      endif
!
!-- DEaD matter format !
      if (iforma(jtec,jsta).eq.1 ) then
        iret=1
        tckded = staee(jtec,jsta)  /10.D0
        auphcb = statt(jtec,jsta,1)/10.D0
!-- Default value of Alu. sheet thickness is 300 microns !
        if( auphcb.lt.0.00001D0 ) auphcb = 0.0300D0
      else
        print *,'==> jsta, iforma =',jsta,iforma(jtec,jsta)
        print *,'==> extraction of DED parameters not prepared'
        print *,'==> for this DED format !'
      endif
!
 END SUBROUTINE insDEDa
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 subroutine insCSCa(jsta,iret,iprint,                                 &
                    ttotal,laycsc,tnomex,tlag10,wispac,dancat,        &
                    pcatre,gstrip,wrestr,wflstr,                      &
                    trrwas,wroxac,groxwi,wgasba,tgasba,wgascu,tgascu, &
                    wfixwi,tfixwi,pwiba1,pwiba2,pwiba3,psndco)
 IMPLICIT REAL(8) (A-H,O-Z)
 integer :: jsta,iret,iprint
 integer :: laycsc
 integer :: i
 integer, save :: jtec=0
 logical, save :: first=.true.
!--------------------------------------------------------------------
!
! extraction of CSC inner structure parameters (in cm)
!  for stacking index type JSTA
!
!  -meanning of output quantities is written down when iprint=1
!  -if extraction is sucessfull iret=1 (=0 if not)
!  -output:
!     integer laycsc
!
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
      if (first) then
       first=.false.
       do i=1,mtec
        if (cartec(i).eq.'CSC') jtec=i
       enddo
       if (jtec.eq.0) then
        print *,'**Pb in INSCSC: jtec index not found'
        print *,'====> STOP'
        stop
       endif
      endif
!
      iret=0
!
      if (jsta.gt.msta) then
       print *,'*Wrong call to INSCSC with jsta=',jsta
       print *,'  the maximal value of jsta is  ',msta
       return
      endif
!
      if (jsta.lt.1) then
       print *,'*Wrong call to INSCSC with jsta=',jsta
       print *,'  the minimal value of jsta is 1'
       return
      endif
!
      if (iforma(jtec,jsta).eq.0) then
       print *,'*Wrong call to INSCSC with jsta ',jsta
       print *,'  this is a undefined index of stacking type'
       return
      endif
!
! CSC formats for which no extraction is prepared
      if (iforma(jtec,jsta).ne.2.and.iforma(jtec,jsta).ne.3) then
      if (iprint.eq.1) then
       print *,' '
       print 10,jsta
       print *,'==> extraction of CSC parameters not prepared'
       print *,'==>  for this CSC format'
       print *,' '
      endif
      endif
!
! second CSC format
      if (iforma(jtec,jsta).eq.2.or.iforma(jtec,jsta).eq.3) then
      iret=1
      ttotal =staee(jtec,jsta)/10.     ! total thickness
      laycsc =nlaz(jtec,jsta)          ! nber of CSCs layers
      tnomex =statt(jtec,jsta, 2)/10.  ! nomex honeycomb thickness
      tlag10 =statt(jtec,jsta, 3)/10.  ! G10 laminates thickness
      wispac =statt(jtec,jsta, 4)/10.  ! Wire spacing
      dancat =statt(jtec,jsta, 5)/10.  ! Anode-Cathode distance
      pcatre =statt(jtec,jsta, 6)/10.  ! Cathode readout pitch
      gstrip =statt(jtec,jsta, 7)/10.  ! Gap between cathode strips
      wrestr =statt(jtec,jsta, 8)/10.  ! Width of readout strips
      wflstr =statt(jtec,jsta, 9)/10.  ! Width of floating strips
      trrwas =statt(jtec,jsta,10)/10.  ! rigit rectangular washer thicknes
      wroxac =statt(jtec,jsta,11)/10.  ! roxacell width
      groxwi =statt(jtec,jsta,12)/10.  ! roxacell and wire bar gap
      wgasba =statt(jtec,jsta,13)/10.  ! full gas gap bar width
      tgasba =statt(jtec,jsta,14)/10.  ! full gas gap bar thick.
      wgascu =statt(jtec,jsta,15)/10.  ! cuts gas gap bar width
      tgascu =statt(jtec,jsta,16)/10.  ! cuts gas gap bar thick.
      wfixwi =statt(jtec,jsta,17)/10.  ! full wire fix. bar wid.
      tfixwi =statt(jtec,jsta,18)/10.  ! full wire fix. bar thick.
      pwiba1 =staoo(jtec,jsta, 1)/10.  ! wire bar position
      pwiba2 =staoo(jtec,jsta, 2)/10.  ! wire bar position
      pwiba3 =staoo(jtec,jsta, 3)/10.  ! wire bar position
      psndco =staoo(jtec,jsta, 4)/10.  ! 2nd coordinate pitch
      if (iprint.eq.1) then
      print *,' '
      print 10,jsta
      print 12,'ttotal',ttotal,'total thickness                      '
      print 13,'laycsc',laycsc,'nber of CSCs layers                  '
      print 12,'tnomex',tnomex,'nomex honeycomb thickness            '
      print 12,'tlag10',tlag10,'G10 laminates thickness              '
      print 12,'wispac',wispac,'Wire spacing                         '
      print 12,'dancat',dancat,'Anode-Cathode distance               '
      print 12,'pcatre',pcatre,'Cathode readout pitch                '
      print 12,'gstrip',gstrip,'Gap between cathode strips           '
      print 12,'wrestr',wrestr,'Width of readout strips              '
      print 12,'wflstr',wflstr,'Width of floating strips             '
      if (iforma(jtec,jsta).eq.3) then
      print 12,'trrwas',trrwas,'rigit rectangular washer thicknes    '
      print 12,'wroxac',wroxac,'roxacell width                       '
      print 12,'groxwi',groxwi,'roxacell and wire bar gap            '
      print 12,'wgasba',wgasba,'full gas gap bar width               '
      print 12,'tgasba',tgasba,'full gas gap bar thick.              '
      print 12,'wgascu',wgascu,'cuts gas gap bar width               '
      print 12,'tgascu',tgascu,'cuts gas gap bar thick.              '
      print 12,'wfixwi',wfixwi,'full wire fix. bar wid.              '
      print 12,'tfixwi',tfixwi,'full wire fix. bar thick.            '
      print 12,'pwiba1',pwiba1,'wire bar position                    '
      print 12,'pwiba2',pwiba2,'wire bar position                    '
      print 12,'pwiba3',pwiba3,'wire bar position                    '
      print 12,'psndco',psndco,'2nd coordinate pitch                 '
      endif
      print *,' '
      endif
      endif
!
10    format('*CSC inner structure parameters (in cm):'/' jsta=',I4)
12    format(1x,A6,' = ',F10.3,1x,A37)
13    format(1x,A6,' = ',I10  ,1x,A37)
!
 END SUBROUTINE insCSCa
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 subroutine insCROa(jsta,iret,  CBheight,CBTwidth,CBTthick)
 IMPLICIT REAL(8) (A-H,O-Z)
 integer :: jsta,iret
 real(8) :: CBheight,CBTwidth,CBTthick
 integer :: i
 integer, save :: jtec=0
 logical, save :: first=.true.
!--------------------------------------------------------------------
!
!  CRO  i.e. MDT Cross-Barr on the Read Out side  Structure :
!
!             CBTwidth
!     <---------------------->
!
!     @@@@@@@@@@@@@@@@@@@@@@@@  ^             ^
!     @                      @  |             |
!     @   Aluminium          @  | CBTthick    |
!     @                      @  |             |
!     @@@@@@@@@@    @@@@@@@@@@  v             |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         | CBheight
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!     @@@@@@@@@@    @@@@@@@@@@                |
!     @                      @                |
!     @                      @                |
!     @                      @                |
!     @@@@@@@@@@@@@@@@@@@@@@@@                v
!
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
      if (first) then
        first=.false.
        do i=1,mtec
         if (cartec(i).eq.'CRO') jtec=i
        enddo
        if (jtec.eq.0) then
          print *,'==> Pb in InsCRO: jtec index not found'
          print *,'======> STOP !'
          stop
        endif
      endif
!
      iret=0
!
      if (jsta.gt.msta) then
        print *,'==> Wrong call to InsCRO with jsta =',jsta
        print *,'==> The maximal value of jsta is ',msta
        return
      elseif (jsta.lt.1) then
        print *,'==> Wrong call to InsCRO with jsta =',jsta
        print *,'==> the minimal value of jsta is 1'
        return
      endif
!
      if (iforma(jtec,jsta).eq.1 ) then
        iret=1
        CBheight = stapp(jtec,jsta)/10.D0
        CBTwidth = starr(jtec,jsta)/10.D0
        CBTthick = staee(jtec,jsta)/10.D0
      else
        print *,'==> jsta, iforma =',jsta,iforma(jtec,jsta)
        print *,'==> extraction of CRO parameters not prepared'
        print *,'==> for this CRO format !'
      endif
!
 END SUBROUTINE insCROa
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 subroutine insCMIa(jsta,iret,  CBheight,CBTwidth,CBTthick)
 IMPLICIT REAL(8) (A-H,O-Z)
 integer :: jsta,iret
 real(8) :: CBheight,CBTwidth,CBTthick
 integer :: i
 integer, save :: jtec=0
 logical, save :: first=.true.
!--------------------------------------------------------------------
!
!  CMI  i.e. MDT Cross-Barr in the MIddle  Structure :
!
!             CBTwidth
!     <---------------------->
!
!     @@@@@@@@@@@@@@@@@@@@@@@@  ^             ^
!     @                      @  |             |
!     @   Aluminium          @  | CBTthick    |
!     @                      @  |             |
!     @@@@@@@@@@    @@@@@@@@@@  v             |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         | CBheight
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!     @@@@@@@@@@    @@@@@@@@@@                |
!     @                      @                |
!     @                      @                |
!     @                      @                |
!     @@@@@@@@@@@@@@@@@@@@@@@@                v
!
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
      if (first) then
        first=.false.
        do i=1,mtec
         if (cartec(i).eq.'CMI') jtec=i
        enddo
        if (jtec.eq.0) then
          print *,'==> Pb in InsCMI: jtec index not found'
          print *,'======> STOP !'
          stop
        endif
      endif
!
      iret=0
!
      if (jsta.gt.msta) then
        print *,'==> Wrong call to InsCMI with jsta =',jsta
        print *,'==> The maximal value of jsta is ',msta
        return
      elseif (jsta.lt.1) then
        print *,'==> Wrong call to InsCMI with jsta =',jsta
        print *,'==> the minimal value of jsta is 1'
        return
      endif
!
      if (iforma(jtec,jsta).eq.1 ) then
        iret=1
        CBheight = stapp(jtec,jsta)/10.D0
        CBTwidth = starr(jtec,jsta)/10.D0
        CBTthick = staee(jtec,jsta)/10.D0
      else
        print *,'==> jsta, iforma =',jsta,iforma(jtec,jsta)
        print *,'==> extraction of CMI parameters not prepared'
        print *,'==> for this CMI format !'
      endif
!
 END SUBROUTINE insCMIa
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 subroutine insCHVa(jsta,iret,  CBheight,CBTwidth,CBTthick)
 IMPLICIT REAL(8) (A-H,O-Z)
 integer :: jsta,iret
 real(8) :: CBheight,CBTwidth,CBTthick
 integer :: i
 integer, save :: jtec=0
 logical, save :: first=.true.
!--------------------------------------------------------------------
!
!  CHV  i.e. MDT Cross-Barr on the High Voltage side  Structure :
!
!             CBTwidth
!     <---------------------->
!
!     @@@@@@@@@@@@@@@@@@@@@@@@  ^             ^
!     @                      @  |             |
!     @   Aluminium          @  | CBTthick    |
!     @                      @  |             |
!     @@@@@@@@@@    @@@@@@@@@@  v             |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         | CBheight
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!              @    @                         |
!     @@@@@@@@@@    @@@@@@@@@@                |
!     @                      @                |
!     @                      @                |
!     @                      @                |
!     @@@@@@@@@@@@@@@@@@@@@@@@                v
!
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
      if (first) then
        first=.false.
        do i=1,mtec
         if (cartec(i).eq.'CHV') jtec=i
        enddo
        if (jtec.eq.0) then
          print *,'==> Pb in InsCHV: jtec index not found'
          print *,'======> STOP !'
          stop
        endif
      endif
!
      iret=0
!
      if (jsta.gt.msta) then
        print *,'==> Wrong call to InsCHV with jsta =',jsta
        print *,'==> The maximal value of jsta is ',msta
        return
      elseif (jsta.lt.1) then
        print *,'==> Wrong call to InsCHV with jsta =',jsta
        print *,'==> the minimal value of jsta is 1'
        return
      endif
!
      if (iforma(jtec,jsta).eq.1 ) then
        iret=1
        CBheight = stapp(jtec,jsta)/10.D0
        CBTwidth = starr(jtec,jsta)/10.D0
        CBTthick = staee(jtec,jsta)/10.D0
      else
        print *,'==> jsta, iforma =',jsta,iforma(jtec,jsta)
        print *,'==> extraction of CHV parameters not prepared'
        print *,'==> for this CHV format !'
      endif
!
 END SUBROUTINE insCHVa
!
