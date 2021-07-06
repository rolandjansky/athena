      function rangen()
      real rangen
      double precision dranf
      integer dummy
 2    rangen=sngl(dranf(dummy))
      if(rangen.le.0.0d0.or.rangen.ge.1.0d0)goto 2
      end

      double precision function drangen(dummy)
      double precision dummy,dranf
      integer idummy = dummy
      drangen=dranf(idummy)
      end
