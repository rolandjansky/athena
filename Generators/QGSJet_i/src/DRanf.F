      double precision function dranf(dummy)
      double precision atl_qgsjet_rndm
      integer dummy
 1    dranf=atl_qgsjet_rndm(dummy)
      if(dranf.le.0.d0.or.dranf.ge.1d0)goto 1
      return
      end

      subroutine ranfgt(seed)
      double precision seed,dumseed
      dumseed=seed
      end

      subroutine ranfst(seed)
      double precision seed,dumseed
      dumseed=seed
      end
