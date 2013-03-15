c------------------------------------------------------------------------
      subroutine bsolinit(fname,flen)
c------------------------------------------------------------------------ 
c Initialise solenoid field map.
c Steve Snow 31/5/07
c------------------------------------------------------------------------
#include "BFieldCore/bsolcomm.inc"
      character*1 fname(*)
      integer flen
      character*500 filename

      do i=1,flen
        filename(i:i) = fname(i)
      enddo

      call bsolread(filename(1:flen))  ! read in the map 
      call bsolsetup(1)     ! set up pointers to cells for speed
      call bsolnewgrid      ! set up a new grid for the rotated/translated map
      call bsolsetup(2)     ! set up the pointers for the new grid 
      call bsolmove(0.,0.,0.,0.,0.) ! fill new grid with moved map

      end

c------------------------------------------------------------------------
      subroutine bsoltilt(dx,dy,dz,ax,ay)
c------------------------------------------------------------------------ 
c Initialise solenoid field map.
c Offsets dx,dy,dz in metres, rotations ax,ay in radians.
c Steve Snow 31/5/07
c------------------------------------------------------------------------
#include "BFieldCore/bsolcomm.inc"
      real dx,dy,dz,ax,ay,adz,adr
      integer flen

      adz = abs(dz)
      adr = 2.85*sqrt( ax*ax + ay*ay ) + sqrt( dx*dx + dy*dy )

      if( adz.gt.0.01 .or. adr.gt.0.01 )then
        write(6,*)' bsolinit: requested solenoid movement too large'
        write(6,*)' dx,dy,dz,ax,ay ',dx,dy,dz,ax,ay
        write(6,*)' bsolinit: set the movement to zero '
        dx = 0.0
        dy = 0.0
        dz = 0.0
        ax = 0.0
        ay = 0.0
      endif

      call bsolmove(dx,dy,dz,ax,ay) ! fill new grid with moved map

      end

c------------------------------------------------------------------------
      subroutine bsolnewgrid
c------------------------------------------------------------------------ 
c Make a new grid that is virtually the same as the one that was read in
c but slightly smaller. The new cylinder ends at z=2830, r=1085.
c Steve Snow 3/1/08
c------------------------------------------------------------------------
#include "BFieldCore/bsolcomm.inc"
      real zlim,rlim

      zlim = 2.835
      rlim = 1.090
c                          cut off the Z grid at both ends
      j = 1
      zgp(2,j) = -zlim
      do i=1,nz(1)
        if( abs(zgp(1,i)).lt.zlim )then
          j = j+1
          zgp(2,j) = zgp(1,i)
        endif
      enddo
      j = j+1
      zgp(2,j) = zlim
      nz(2) = j

c                          cut off the R grid at high R
      j = 0
      do i=1,nr(1)
        if( rgp(1,i).lt.rlim )then
          j = j+1
          rgp(2,j) = rgp(1,i)
        endif
      enddo
      j = j+1
      rgp(2,j) = rlim
      nr(2) = j
c                           keep the phi grid unchanged

      do i=1,nf(1)
        fgp(2,i) = fgp(1,i)
      enddo
      nf(2) = nf(1)

      end

c------------------------------------------------------------------------
      subroutine bsolread(fname)
c------------------------------------------------------------------------ 
c Read in a solenoid field map from file 'fname' 
c Steve Snow 31/5/07
c------------------------------------------------------------------------
#include "BFieldCore/bsolcomm.inc"

      character*(*) fname
      integer vno
      real oxin,oyin,ozin,axin,ayin
      
      write(6,*)' '
      write(6,*)' BSOLREAD. reading file ',fname

      open(unit=8,file=fname,status='OLD',err=999) 

      read(8,*,err=998,end=997)vno
      write(6,*)' Version number: ',vno   ! not used

      read(8,*,err=998,end=997)nz(1),nr(1),nf(1)
      if ( nz(1).gt.nzmax.or.nr(1).gt.nrmax.or.nf(1).gt.nfmax )then
        write(6,*)' input grid too big: ',nz(1),nr(1),nf(1)
        stop
      endif

      read(8,*,err=998,end=997)(rgp(1,j),j=1,nr(1))

      read(8,*,err=998,end=997)(zgp(1,j),j=1,nz(1))

      read(8,*,err=998,end=997)(fgp(1,j),j=1,nf(1))

      read(8,*,err=998,end=997)oxin,oyin,ozin,axin,ayin  ! not used

      write(6,*)' grid size: ',nz(1),nr(1),nf(1)
      write(6,*)' zgp: '
      write(6,1000)(zgp(1,i),i=1,nz(1))
      write(6,*)' rgp: '
      write(6,1000)(rgp(1,i),i=1,nr(1))
      write(6,*)' fgp: '
      write(6,1000)(fgp(1,i),i=1,nf(1))

      do k = 1,nf(1)
        do j = 1,nr(1)
          read(8,*,err=998,end=997)(bz(1,i,j,k),i=1,nz(1))
        enddo
        do j = 1,nr(1)
          read(8,*,err=998,end=997)(br(1,i,j,k),i=1,nz(1))
        enddo
        do j = 1,nr(1)
          read(8,*,err=998,end=997)(bf(1,i,j,k),i=1,nz(1))
        enddo
      enddo

      write(6,*)' Reading completed'
      write(6,*)' '
      close(8)

      return

 1000 format(10f10.5)

 997  write(6,*)' Error end of file in bsolread at i,j,k',i,j,k
      stop
 998  write(6,*)' Error reading file in bsolread at i,j,k',i,j,k
      stop
 999  write(6,*)' Error opening file in bsolread'
      stop
      end

c------------------------------------------------------------------------
      subroutine bsolsetup(ig)
c------------------------------------------------------------------------ 
c Set up pointers to the cells at 1 mm intervals in R and Z, 1 mrad in phi.
c Steve Snow 31/5/07
c------------------------------------------------------------------------
#include "BFieldCore/bsolcomm.inc"
      real zm,rm,fm
      integer jj,ig

      do n=1,mmz
        zm = ( float(n) - 0.5 )*0.001 + zgp(ig,1)
        do i=2,nz(ig)
          if( zgp(ig,i).gt.zm )then
            jj = i-1
            go to 1
          endif
        enddo
        jj = nz(ig)-1
 1      continue
        izp(ig,n) = jj
      enddo

      do n=1,mmr
        rm = ( float(n) - 0.5 )*0.001 + rgp(ig,1)
        do i=2,nr(ig)
          if( rgp(ig,i).gt.rm )then
            jj = i-1
            go to 2
          endif
        enddo
        jj = nr(ig)-1
 2      continue
        irp(ig,n) = jj
      enddo

      do n=1,mrf
        fm = ( float(n) - 0.5 )*0.001 + fgp(ig,1)
        do i=2,nf(ig)
          if( fgp(ig,i).gt.fm )then
            jj = i-1
            go to 3
          endif
        enddo
        jj = nf(ig)            ! phi is different
 3      continue
        ifp(ig,n) = jj
      enddo

c               set up inverse cell size arrays

      do i=1,nz(ig)-1
        zsinv(ig,i) = 1.0/ ( zgp(ig,i+1) - zgp(ig,i) )
      enddo

      do i=1,nr(ig)-1
        rsinv(ig,i) = 1.0/ ( rgp(ig,i+1) - rgp(ig,i) )
      enddo

      twopi = 8.0*atan2(1.0,1.0)

      do i=1,nf(ig)-1
        fsinv(ig,i) = 1.0/ ( fgp(ig,i+1) - fgp(ig,i) )
      enddo
      fsinv(ig,nf(ig))  = 1.0/ ( twopi - fgp(ig,nf(ig)) )    ! phi is different

      write(6,*)' Solenoid grid ',ig,' is set up for: '
      write(6,*)' Z range ',zgp(ig,1), ' to ',zgp(ig,nz(ig)),' meters.'
      write(6,*)' R range 0.0 to ',rgp(ig,nr(ig)),' meters.'
      write(6,*)' '

      end

c---------------------------------------------------------------------
      subroutine bsolinterp(ig,zi,ri,fi,bzo,bro,bfo)
c---------------------------------------------------------------------
c Return the field at point zi,ri,fi by linear interpolation.
c ig=1 is the field read from file, ig=2 is the moved version of the field.
c zi,ri in metres, fi in radians, output fields in Gauss.
c Steve Snow 31/5/07
c------------------------------------------------------------------------
#include "BFieldCore/bsolcomm.inc"
      real zi,ri,fi,z,r,f,bzo,bro,bfo,fz,fr,ff,gz,gr,gf
      integer izmm,irmm,ifmr,il,ih,jl,jh,kl,kh,ig

      if( ig.lt.1 .or. ig.gt. 2 )then
        write(6,*)' bsolinterp: disallowed ig value ',ig
        ig=1
      endif

      r = max( ri,rgp(ig,1) )           ! move r away from the axis
      if( fi.lt.0.0 )then               ! get phi into range 0 - 2pi
        f = fi + twopi
      elseif( fi.gt.twopi )then
        f = fi - twopi
      else
        f = fi
      endif
      z = zi

      izmm = 1 + ( z - zgp(ig,1) )*1000.0
      if( izmm.lt.1 )then   ! comment this out, when we're sure it works 
        write(6,*)' bsolinterp: izmm out of range ',izmm
        write(6,*)' ig,zi,ri,fi ',ig,zi,ri,fi
        izmm = 1
      elseif( izmm.ge.mmz )then
        write(6,*)' bsolinterp: izmm out of range ',izmm
        write(6,*)' ig,zi,ri,fi ',ig,zi,ri,fi
        izmm = mmz-1
      endif
      il  = izp(ig,izmm)
      ih  = il + 1

      irmm = 1 + ( r - rgp(ig,1) )*1000.0
      if( irmm.lt.1 )then   ! comment this out, when we're sure it works 
        write(6,*)' bsolinterp: irmm out of range ',irmm
        write(6,*)' ig,zi,ri,fi ',ig,zi,ri,fi
        irmm = 1
      elseif( irmm.ge.mmr )then
        write(6,*)' bsolinterp: irmm out of range ',irmm
        write(6,*)' ig,zi,ri,fi ',ig,zi,ri,fi
        irmm = mmr-1
      endif
      jl  = irp(ig,irmm)
      jh  = jl + 1
      
      ifmr = 1 + ( f - fgp(ig,1) )*1000.0
      if( ifmr.lt.1 )then   ! comment this out, when we're sure it works 
        write(6,*)' bsolinterp: ifmr out of range ',ifmr
        write(6,*)' ig,zi,ri,fi ',ig,zi,ri,fi
        ifmr = 1
      elseif( ifmr.ge.mrf )then
        write(6,*)' bsolinterp: ifmr out of range ',ifmr
        write(6,*)' ig,zi,ri,fi ',ig,zi,ri,fi
        ifmr = mrf-1
      endif
      kl  = ifp(ig,ifmr)
      kh  = kl + 1
      if ( kh.gt.nf(ig) ) kh = 1        ! phi is different

      fz = ( z - zgp(ig,il) ) * zsinv(ig,il)
      fr = ( r - rgp(ig,jl) ) * rsinv(ig,jl)
      ff = ( f - fgp(ig,kl) ) * fsinv(ig,kl)
      gz = 1.0 - fz
      gr = 1.0 - fr
      gf = 1.0 - ff
 
      bzo = bz(ig,il,jl,kl)*gz*gr*gf +
     +      bz(ig,ih,jl,kl)*fz*gr*gf +
     +      bz(ig,il,jh,kl)*gz*fr*gf +
     +      bz(ig,ih,jh,kl)*fz*fr*gf +
     +      bz(ig,il,jl,kh)*gz*gr*ff +
     +      bz(ig,ih,jl,kh)*fz*gr*ff +
     +      bz(ig,il,jh,kh)*gz*fr*ff +
     +      bz(ig,ih,jh,kh)*fz*fr*ff 

      bro = br(ig,il,jl,kl)*gz*gr*gf +
     +      br(ig,ih,jl,kl)*fz*gr*gf +
     +      br(ig,il,jh,kl)*gz*fr*gf +
     +      br(ig,ih,jh,kl)*fz*fr*gf +
     +      br(ig,il,jl,kh)*gz*gr*ff +
     +      br(ig,ih,jl,kh)*fz*gr*ff +
     +      br(ig,il,jh,kh)*gz*fr*ff +
     +      br(ig,ih,jh,kh)*fz*fr*ff 

      bfo = bf(ig,il,jl,kl)*gz*gr*gf +
     +      bf(ig,ih,jl,kl)*fz*gr*gf +
     +      bf(ig,il,jh,kl)*gz*fr*gf +
     +      bf(ig,ih,jh,kl)*fz*fr*gf +
     +      bf(ig,il,jl,kh)*gz*gr*ff +
     +      bf(ig,ih,jl,kh)*fz*gr*ff +
     +      bf(ig,il,jh,kh)*gz*fr*ff +
     +      bf(ig,ih,jh,kh)*fz*fr*ff 

      end

c--------------------------------------------------------------------
      subroutine bsolmove(ox,oy,oz,ax,ay)
c--------------------------------------------------------------------
c Carry out the translation and rotation of the field in grid 1
c and put the result in grid 2.
c Offsets ox,oy,oz in metres, angles ax,ay in radians.
c Steve Snow 16/1/08 
c--------------------------------------------------------------------
#include "BFieldCore/bsolcomm.inc"   
      real cax,cay,sax,say,x0,y0,z0,r0,f0,x3,y3,z3,r3,f3,x1,y1,z1,
     +     bx0,by0,bz0,br0,bf0,bx3,by3,bz3,br3,bf3,ox,oy,oz,ax,ay,
     +     fb0,tb0

      cax = cos(ax)
      cay = cos(ay)
      sax = sin(ax)
      say = sin(ay)

      do i=1,nz(2)
        z0 = zgp(2,i)
        do j=1,nr(2)
          r0 = rgp(2,j)
          do k=1,nf(2)
            f0 = fgp(2,k)
            x0 = r0*cos(f0)
            y0 = r0*sin(f0)
            x1 = x0 - ox
            y1 = y0 - oy
            z1 = z0 - oz 
            x3 = x1*cay - (z1*cax-y1*sax)*say
            y3 = y1*cax + z1*sax
            z3 = ( z1*cax - y1*sax )*cay + x1*say
            f3 = atan2(y3,x3)
            if( f3.lt.0.0 ) f3 = f3 + twopi
            if( f3.gt.twopi ) f3 = f3 - twopi
            r3 = sqrt( x3*x3 + y3*y3 )

            call bsolinterp(1,z3,r3,f3,bz3,br3,bf3)

            bx3 = br3*cos(f3) - bf3*sin(f3)
            by3 = br3*sin(f3) + bf3*cos(f3)
            bx0 = bx3*cay + bz3*say
            by0 = by3*cax - ( bz3*cay - bx3*say )*sax
            bz0 = ( bz3*cay - bx3*say )*cax + by3*sax
            fb0  = atan2(by0,bx0)
            tb0 = sqrt( bx0*bx0 + by0*by0 )
            bz(2,i,j,k) = bz0
            br(2,i,j,k) = tb0*cos(fb0-f0)
            bf(2,i,j,k) = tb0*sin(fb0-f0)
          enddo
        enddo
      enddo

      end


c------------------------------------------------------------------------
      subroutine bsolgtocart(ig,z,r,f,bzi,bri,bfi,bgcyl,bgcart)
c------------------------------------------------------------------------
c Convert the field gradient from cylindrical to cartesian coordinates 
c The contents of the gradient arrays are
c bgcyl:   1      2      3      4      5      6      7      8      9
c       dBz/dz dBz/dr dBz/df dBr/dz dBr/dr dBr/df dBf/dz dBf/dr dBf/df
c bgcart:
c       dBx/dx dBx/dy dBx/dz dBy/dx dBy/dy dBy/dz dBz/dx dBz/dy dBz/dz
c
c Steve Snow 24/9/08
c------------------------------------------------------------------------
#include "BFieldCore/bsolcomm.inc"
      real z,r,f,bzi,bri,bfi,bgcyl(9),bgcart(9),s,c,ss,sc,cc,a,b
      real ri
      integer ig

      ri = max( r, rgp(ig,1) )     ! move r away from the axis

      s = sin(f)
      c = cos(f)
      ss = s*s
      sc = s*c
      cc = c*c
      
      a = ( bfi - bgcyl(6) )/ri
      b = ( bri + bgcyl(9) )/ri

      bgcart(1) = cc*(  bgcyl(5)       ) + 
     +            sc*(  a - bgcyl(8)   ) +  
     +            ss*(  b              )

      bgcart(2) = cc*( -a              ) + 
     +            sc*(  bgcyl(5) - b   ) +  
     +            ss*( -bgcyl(8)       )

      bgcart(3) = c*bgcyl(4) - s*bgcyl(7)

      bgcart(4) = cc*(  bgcyl(8)       ) +
     +            sc*(  bgcyl(5) - b   ) +
     +            ss*(  a              )

      bgcart(5) = cc*(  b              ) +
     +            sc*(  bgcyl(8) - a   ) +
     +            ss*(  bgcyl(5)       )

      bgcart(6) = s*bgcyl(4) + c*bgcyl(7)       

      bgcart(7) = c*bgcyl(2) - s*bgcyl(3)

      bgcart(8) = s*bgcyl(2) + c*bgcyl(3)

      bgcart(9) = bgcyl(1)

      end

c---------------------------------------------------------------------
      subroutine bsolinterpg(ig,zi,ri,fi,bzo,bro,bfo,bgrad)
c---------------------------------------------------------------------
c An extended version of bsolinterp which returns the field and its
c gradient at point zi,ri,fi by linear interpolation.
c ig=1 is the field read from file, ig=2 is the moved version of the field.
c zi,ri in metres, fi in radians, output fields bz0,bro,bfo in Gauss.
c The nine gradients are returned in bgrad in this order:
c    1      2      3      4      5      6      7      8      9
c  dBz/dz dBz/dr dBz/df dBr/dz dBr/dr dBr/df dBf/dz dBf/dr dBf/df
c
c Gradient units are in combinations of Gauss, metres and radians.
c
c Steve Snow 23/9/08
c------------------------------------------------------------------------
#include "BFieldCore/bsolcomm.inc"
      real zi,ri,fi,z,r,f,bzo,bro,bfo,fz,fr,ff,gz,gr,gf,bgrad(9)
      integer izmm,irmm,ifmr,il,ih,jl,jh,kl,kh,ig

      if( ig.lt.1 .or. ig.gt. 2 )then
        write(6,*)' bsolinterpg: disallowed ig value ',ig
        ig=1
      endif

      r = max( ri,rgp(ig,1) )           ! move r away from the axis
      if( fi.lt.0.0 )then               ! get phi into range 0 - 2pi
        f = fi + twopi
      elseif( fi.gt.twopi )then
        f = fi - twopi
      else
        f = fi
      endif
      z = zi

      izmm = 1 + ( z - zgp(ig,1) )*1000.0
      if( izmm.lt.1 )then   ! comment this out, when we're sure it works 
        write(6,*)' bsolinterpg: izmm out of range ',izmm
        write(6,*)' ig,zi,ri,fi ',ig,zi,ri,fi
        izmm = 1
      elseif( izmm.ge.mmz )then
        write(6,*)' bsolinterpg: izmm out of range ',izmm
        write(6,*)' ig,zi,ri,fi ',ig,zi,ri,fi
        izmm = mmz-1
      endif
      il  = izp(ig,izmm)
      ih  = il + 1

      irmm = 1 + ( r - rgp(ig,1) )*1000.0
      if( irmm.lt.1 )then   ! comment this out, when we're sure it works 
        write(6,*)' bsolinterpg: irmm out of range ',irmm
        write(6,*)' ig,zi,ri,fi ',ig,zi,ri,fi
        irmm = 1
      elseif( irmm.ge.mmr )then
        write(6,*)' bsolinterpg: irmm out of range ',irmm
        write(6,*)' ig,zi,ri,fi ',ig,zi,ri,fi
        irmm = mmr-1
      endif
      jl  = irp(ig,irmm)
      jh  = jl + 1
      
      ifmr = 1 + ( f - fgp(ig,1) )*1000.0
      if( ifmr.lt.1 )then   ! comment this out, when we're sure it works 
        write(6,*)' bsolinterpg: ifmr out of range ',ifmr
        write(6,*)' ig,zi,ri,fi ',ig,zi,ri,fi
        ifmr = 1
      elseif( ifmr.ge.mrf )then
        write(6,*)' bsolinterpg: ifmr out of range ',ifmr
        write(6,*)' ig,zi,ri,fi ',ig,zi,ri,fi
        ifmr = mrf-1
      endif
      kl  = ifp(ig,ifmr)
      kh  = kl + 1
      if ( kh.gt.nf(ig) ) kh = 1        ! phi is different

      fz = ( z - zgp(ig,il) ) * zsinv(ig,il)
      fr = ( r - rgp(ig,jl) ) * rsinv(ig,jl)
      ff = ( f - fgp(ig,kl) ) * fsinv(ig,kl)
      gz = 1.0 - fz
      gr = 1.0 - fr
      gf = 1.0 - ff
 
      bzo = bz(ig,il,jl,kl)*gz*gr*gf +
     +      bz(ig,ih,jl,kl)*fz*gr*gf +
     +      bz(ig,il,jh,kl)*gz*fr*gf +
     +      bz(ig,ih,jh,kl)*fz*fr*gf +
     +      bz(ig,il,jl,kh)*gz*gr*ff +
     +      bz(ig,ih,jl,kh)*fz*gr*ff +
     +      bz(ig,il,jh,kh)*gz*fr*ff +
     +      bz(ig,ih,jh,kh)*fz*fr*ff 

      bro = br(ig,il,jl,kl)*gz*gr*gf +
     +      br(ig,ih,jl,kl)*fz*gr*gf +
     +      br(ig,il,jh,kl)*gz*fr*gf +
     +      br(ig,ih,jh,kl)*fz*fr*gf +
     +      br(ig,il,jl,kh)*gz*gr*ff +
     +      br(ig,ih,jl,kh)*fz*gr*ff +
     +      br(ig,il,jh,kh)*gz*fr*ff +
     +      br(ig,ih,jh,kh)*fz*fr*ff 

      bfo = bf(ig,il,jl,kl)*gz*gr*gf +
     +      bf(ig,ih,jl,kl)*fz*gr*gf +
     +      bf(ig,il,jh,kl)*gz*fr*gf +
     +      bf(ig,ih,jh,kl)*fz*fr*gf +
     +      bf(ig,il,jl,kh)*gz*gr*ff +
     +      bf(ig,ih,jl,kh)*fz*gr*ff +
     +      bf(ig,il,jh,kh)*gz*fr*ff +
     +      bf(ig,ih,jh,kh)*fz*fr*ff 

c dbz/dz
      bgrad(1) = zsinv(ig,il) * (       
     +   ( bz(ig,ih,jh,kh)*fr*ff + bz(ig,ih,jh,kl)*fr*gf +
     +     bz(ig,ih,jl,kh)*gr*ff + bz(ig,ih,jl,kl)*gr*gf    )        
     +  -( bz(ig,il,jh,kh)*fr*ff + bz(ig,il,jh,kl)*fr*gf +
     +     bz(ig,il,jl,kh)*gr*ff + bz(ig,il,jl,kl)*gr*gf    )  )

c dbz/dr
      bgrad(2) = rsinv(ig,jl) * (
     +   ( bz(ig,ih,jh,kh)*fz*ff + bz(ig,ih,jh,kl)*fz*gf +
     +     bz(ig,il,jh,kh)*gz*ff + bz(ig,il,jh,kl)*gz*gf   )
     +  -( bz(ig,ih,jl,kh)*fz*ff + bz(ig,ih,jl,kl)*fz*gf +
     +     bz(ig,il,jl,kh)*gz*ff + bz(ig,il,jl,kl)*gz*gf   )  )

c dbz/df
      bgrad(3) = fsinv(ig,kl) * (
     +   ( bz(ig,ih,jh,kh)*fz*fr + bz(ig,ih,jl,kh)*fz*gr +
     +     bz(ig,il,jh,kh)*gz*fr + bz(ig,il,jl,kh)*gz*gr   )
     +  -( bz(ig,ih,jh,kl)*fz*fr + bz(ig,ih,jl,kl)*fz*gr +
     +     bz(ig,il,jh,kl)*gz*fr + bz(ig,il,jl,kl)*gz*gr   )  )

c dbr/dz
      bgrad(4) = zsinv(ig,il) * (       
     +   ( br(ig,ih,jh,kh)*fr*ff + br(ig,ih,jh,kl)*fr*gf +
     +     br(ig,ih,jl,kh)*gr*ff + br(ig,ih,jl,kl)*gr*gf    )        
     +  -( br(ig,il,jh,kh)*fr*ff + br(ig,il,jh,kl)*fr*gf +
     +     br(ig,il,jl,kh)*gr*ff + br(ig,il,jl,kl)*gr*gf    )  )

c dbr/dr
      bgrad(5) = rsinv(ig,jl) * (
     +   ( br(ig,ih,jh,kh)*fz*ff + br(ig,ih,jh,kl)*fz*gf +
     +     br(ig,il,jh,kh)*gz*ff + br(ig,il,jh,kl)*gz*gf   )
     +  -( br(ig,ih,jl,kh)*fz*ff + br(ig,ih,jl,kl)*fz*gf +
     +     br(ig,il,jl,kh)*gz*ff + br(ig,il,jl,kl)*gz*gf   )  )

c dbr/df
      bgrad(6) = fsinv(ig,kl) * (
     +   ( br(ig,ih,jh,kh)*fz*fr + br(ig,ih,jl,kh)*fz*gr +
     +     br(ig,il,jh,kh)*gz*fr + br(ig,il,jl,kh)*gz*gr   )
     +  -( br(ig,ih,jh,kl)*fz*fr + br(ig,ih,jl,kl)*fz*gr +
     +     br(ig,il,jh,kl)*gz*fr + br(ig,il,jl,kl)*gz*gr   )  )

c dbf/dz
      bgrad(7) = zsinv(ig,il) * (       
     +   ( bf(ig,ih,jh,kh)*fr*ff + bf(ig,ih,jh,kl)*fr*gf +
     +     bf(ig,ih,jl,kh)*gr*ff + bf(ig,ih,jl,kl)*gr*gf    )        
     +  -( bf(ig,il,jh,kh)*fr*ff + bf(ig,il,jh,kl)*fr*gf +
     +     bf(ig,il,jl,kh)*gr*ff + bf(ig,il,jl,kl)*gr*gf    )  )

c dbf/dr
      bgrad(8) = rsinv(ig,jl) * (
     +   ( bf(ig,ih,jh,kh)*fz*ff + bf(ig,ih,jh,kl)*fz*gf +
     +     bf(ig,il,jh,kh)*gz*ff + bf(ig,il,jh,kl)*gz*gf   )
     +  -( bf(ig,ih,jl,kh)*fz*ff + bf(ig,ih,jl,kl)*fz*gf +
     +     bf(ig,il,jl,kh)*gz*ff + bf(ig,il,jl,kl)*gz*gf   )  )

c dbf/df
      bgrad(9) = fsinv(ig,kl) * (
     +   ( bf(ig,ih,jh,kh)*fz*fr + bf(ig,ih,jl,kh)*fz*gr +
     +     bf(ig,il,jh,kh)*gz*fr + bf(ig,il,jl,kh)*gz*gr   )
     +  -( bf(ig,ih,jh,kl)*fz*fr + bf(ig,ih,jl,kl)*fz*gr +
     +     bf(ig,il,jh,kl)*gz*fr + bf(ig,il,jl,kl)*gz*gr   )  )

      end
