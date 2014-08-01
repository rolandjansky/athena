C...Inline function definitions
C      write(*,*) 'in inlines'
      Gfn(s,t)=(s**4+10.0d0*s**3*t+42.0d0*s**2*t**2+
     $     64.0d0*s*t**3+32.0d0*t**4)/4.0d0
      hqapb(z)= (9.0D0/256)*(1 - 3*z**2 + 4*z**4)
      hqpqp(z)=(9.0D0/2048)*(149 + 232*z+ 114*z**2+ 16*z**3 + z**4)
      hqqqq(z)=(3.0D0/1024)*(547 + 306*z**2+ 3*z**4)
      hqaqa(z)=(3.0D0/2048)*(443 + 692*z+ 354*z**2+116*z**3 + 107*z**4)
      hggqa(z)=(3.0D0/8)*(1 - z**4)
      hqgqg(z)=(3.0D0/8)*(1 + z)*(5 + 2*z + z**2)
      hgggg(z)=(9.0D0/4)*(3 + z**2)**2
      gqapb(z)=0.0D0
      gqpqp(z)=0.0D0
C...  Changed sign of the gqqqq and gqaqa-terms to agree with Giudice
      gqqqq(z)=-(4.0D0*(5-3*z**2)/(1-z**2))
      gqaqa(z)=-((1.0D0/4)*(11- 14*z-z**2)*(1+z**2)**2/(1-z))
      gggqa(z)=4.0D0*(1+z**2)
      gqgqg(z)=-(2.0D0*(5+2*z+z**2))
C...  Multiply with factor 2 to agree with Giudice
      ggggg(z)=-(2.0D0*120.0D0*(3+z**2))
      fqapb(z)=8.0D0*(1+z**2)
      fqpqp(z)=16.0D0*(5+2*z+z**2)/(1-z)**2
      fqqqq(z)=(32.0D0/3)*(z**2+11)*(3*z**2+1)/(1-z**2)**2
      fqaqa(z)=(8.0D0/3)*(7-4*z+z**2)*(5+4*z+3*z**2)/(1-z**2)**2
      fggqa(z)=(16.0D0/3)*(9*z**2+7)*(1+z**2)/(1-z**2)
      fqgqg(z)=(32.0D0/3)*(5+2*z+z**2)*(11+5*z+2*z**2)/(1+z)/(1-z**2)
      fgggg(z)=288.0D0*(3+z**2)**3/(1-z**2)**2
