       subroutine bcopy(ic)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
c      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
c      COMMON/COPYJET/NN,NNPAD,KK(4000,5),PP(4000,5),VV(4000,5)

      COMMON/PYJETS/INT(20002),REA(40000)
      COMMON/COPYJET/INTC(20002),REAC(40000)
      
      if(ic.eq.1) then
c store PYJETS to COPYJET after hard process
      do ii=1,20002
      INTC(ii)=INT(ii)
      enddo
      do ii=1,40000
      REAC(ii)=REA(ii)
      enddo 
      endif
      
      if(ic.eq.2) then
c store COPYJET to PYJETS before repeated fragmentation
      do ii=1,20002
      INT(ii)=INTC(ii)
      enddo
      do ii=1,40000
      REA(ii)=REAC(ii)
      enddo       
      
      
      endif
      
      if(ic.ne.1.and.ic.ne.2) 
     +      print *,'COPY: error, input should be 1 or 2 '
      
      return
      end
      

      return
      end
