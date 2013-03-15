c---------------------------------------------------------------------
      subroutine bsiinterp(zi,ri,fi,bzo,bro,bfo)
c---------------------------------------------------------------------
c Return the field at point zi,ri,fi by linear interpolation
c Steve Snow 31/5/07
c Change it to use the 'BSOL' instead of the 'BSI' subroutines 
c------------------------------------------------------------------------
      real zi,ri,fi,z,r,bzo,bro,bfo

c     Convert from cm to m.
      z = zi / 100.
      r = ri / 100.

c interpolate on grid 1; the original map not the moved copy

      call bsolinterp(1,z,r,fi,bzo,bro,bfo)

c     Convert to Tesla.
      bzo = bzo / 10000.
      bro = bro / 10000.
      bfo = bfo / 10000.

      end
