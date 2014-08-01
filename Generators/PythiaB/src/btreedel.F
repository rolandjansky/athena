c       *******************************************************************
c       *                                                                 *
c       *               subroutines TREEDEL_v103                          *
c       *                 (for PYTHIA 6.207)                              *
c       *                                                                 *
c       *       For delete the tree of the partical dotted                *
c       *                                                                 *
c       *           The last modification:   18.02.2004                   *
c       *                                                                 *
c       *      Author: Nickolay Nikitin, nik679@monet.npi.msu.su          *
c       *                                                                 *
c       *******************************************************************

      subroutine TREEDEL (ijkl)
      
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
c      INTEGER PYK,PYCHGE,PYCOMP
        
c......Standard COMMON blocs                                                                          
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
      COMMON/PYUPPR/NUP,KUP(20,7),NFUP,IFUP(10,2),PUP(20,5),Q2UP(0:10)
                                                    
                                                                                                                     
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/

           
c      print *, '     '     
c      print *,'Level 0'
c      print *, '   KF code =', k(ijkl,2)
c      print *,ijkl,'  dotted strings:',k(ijkl,4),
c     #' - ',k(ijkl,5)
c      print *,'    '
      
      jkm=0
      do jkm=k(ijkl,4),k(ijkl,5)

        if(k(jkm,4).NE.0) then
          call treedel1(jkm)
        endif

c        print *,jkm,'  treedel -OK!'
        do i=1,5
         k(jkm,i)=0
c         print *,'  k(',jkm,',',i,') =', k(jkm,i)
        enddo
        
        do i=1,5
         p(jkm,i)=0.D0
c         print *,'  p(',jkm,',',i,') =', p(jkm,i)
        enddo

        do i=1,5
         v(jkm,i)=0.D0
c         print *,'  v(',jkm,',',i,') =', v(jkm,i)
        enddo
c        print *,'   '

      enddo

      return

      end



      subroutine TREEDEL1 (ijkl)
      
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
c      INTEGER PYK,PYCHGE,PYCOMP
        
c......Standard COMMON blocs                                                                   
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
      COMMON/PYUPPR/NUP,KUP(20,7),NFUP,IFUP(10,2),PUP(20,5),Q2UP(0:10)
                                                                                                                   
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/
            
c      print *,'Level 1'
c      print *, '   KF code =', k(ijkl,2) 
c      print *,ijkl,'  dotted strings:',k(ijkl,4),
c     #' - ',k(ijkl,5)
c      print *,'  '
     
      do jkm=k(ijkl,4),k(ijkl,5)
        if(k(jkm,4).NE.0) then
          call treedel2(jkm)
        endif

c        print *,jkm,'  treedel -OK!'
        do i=1,5
         k(jkm,i)=0
c         print *,'  k(',jkm,',',i,') =', k(jkm,i)
        enddo
        
        do i=1,5
         p(jkm,i)=0.D0
c         print *,'  p(',jkm,',',i,') =', p(jkm,i)
        enddo

        do i=1,5
         v(jkm,i)=0.D0
c         print *,'  v(',jkm,',',i,') =', v(jkm,i)
        enddo
c        print *,'   '

      enddo

      return 

      end



      subroutine TREEDEL2 (ijkl)
      
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
c      INTEGER PYK,PYCHGE,PYCOMP
        
c......Standard COMMON blocs                                                                  
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)      
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
      COMMON/PYUPPR/NUP,KUP(20,7),NFUP,IFUP(10,2),PUP(20,5),Q2UP(0:10)
                                                                                                                 
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/
      
c      print *,'Level 2'      
c      print *, '   KF code =', k(ijkl,2)
c      print *,ijkl,'  dotted strings:',k(ijkl,4),
c     #' - ',k(ijkl,5)
c      print *,'    '
     
      do jkm=k(ijkl,4),k(ijkl,5)
        if(k(jkm,4).NE.0) then
          call treedel3(jkm)
        endif
       
c        print *,jkm,'  treedel -OK!'
        do i=1,5
         k(jkm,i)=0
c         print *,'  k(',jkm,',',i,') =', k(jkm,i)
        enddo
        
        do i=1,5
         p(jkm,i)=0.D0
c         print *,'  p(',jkm,',',i,') =', p(jkm,i)
        enddo

        do i=1,5
         v(jkm,i)=0.D0
c         print *,'  v(',jkm,',',i,') =', v(jkm,i)
        enddo
c        print *,'   '
     
      enddo

      return 

      end



      subroutine TREEDEL3 (ijkl)

C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
c      INTEGER PYK,PYCHGE,PYCOMP
        
c......Standard COMMON blocs                                                             
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
      COMMON/PYUPPR/NUP,KUP(20,7),NFUP,IFUP(10,2),PUP(20,5),Q2UP(0:10)
                                                                                                                  
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/              
      
c      print *,'Level 3'
c      print *,ijkl,'  dotted strings:',k(ijkl,4),
c     #' - ',k(ijkl,5)
c      print *,'   '

      do jkm=k(ijkl,4),k(ijkl,5)
        if(k(jkm,4).NE.0) then
          call treedel4(jkm)
        endif
        
c	print *,jkm,'  treedel -OK!'
        do i=1,5
         k(jkm,i)=0
c         print *,'  k(',jkm,',',i,') =', k(jkm,i)
        enddo
        
        do i=1,5
         p(jkm,i)=0.D0
c         print *,'  p(',jkm,',',i,') =', p(jkm,i)
        enddo

        do i=1,5
         v(jkm,i)=0.D0
c         print *,'  v(',jkm,',',i,') =', v(jkm,i)
        enddo
c        print *,'   '        
      
      enddo
      return 

      end


      subroutine TREEDEL4 (ijkl)

C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
c      INTEGER PYK,PYCHGE,PYCOMP
        
c......Standard COMMON blocs                                                            
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)       
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
      COMMON/PYUPPR/NUP,KUP(20,7),NFUP,IFUP(10,2),PUP(20,5),Q2UP(0:10)
                                                                                                                                                                         
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/     
      
c      print *,'Level 4'      
c      print *,ijkl,'  dotted strings:',k(ijkl,4),
c     #' - ',k(ijkl,5)
c      print *,'   '

      do jkm=k(ijkl,4),k(ijkl,5)
        if(k(jkm,4).NE.0) then
          call treedel5(jkm)
        endif
	
c	print *,jkm,'  treedel -OK!'
        do i=1,5
         k(jkm,i)=0
c         print *,'  k(',jkm,',',i,') =', k(jkm,i)
        enddo
        
        do i=1,5
         p(jkm,i)=0.D0
c         print *,'  p(',jkm,',',i,') =', p(jkm,i)
        enddo

        do i=1,5
         v(jkm,i)=0.D0
c         print *,'  v(',jkm,',',i,') =', v(jkm,i)
        enddo
c        print *,'   '    

      enddo
      return 

      end


      subroutine TREEDEL5 (ijkl)
      
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
c      INTEGER PYK,PYCHGE,PYCOMP
        
c......Standard COMMON blocs                                                                    
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
      COMMON/PYUPPR/NUP,KUP(20,7),NFUP,IFUP(10,2),PUP(20,5),Q2UP(0:10)
                                                                                                                                                                                                                                                
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/   
      
c      print *,'Level 5'
c      print *,ijkl,'  dotted strings:',k(ijkl,4),
c     #' - ',k(ijkl,5)
c      print *,'   '

      do jkm=k(ijkl,4),k(ijkl,5)
        if(k(jkm,4).NE.0) then
          call treedel6(jkm)
        endif
                                         
c	print *,jkm,'  treedel -OK!'
        do i=1,5
         k(jkm,i)=0
c         print *,'  k(',jkm,',',i,') =', k(jkm,i)
        enddo
        
        do i=1,5
         p(jkm,i)=0.D0
c         print *,'  p(',jkm,',',i,') =', p(jkm,i)
        enddo

        do i=1,5
         v(jkm,i)=0.D0
c         print *,'  v(',jkm,',',i,') =', v(jkm,i)
        enddo
c        print *,'   '   
      
      enddo
      return 

      end


      subroutine TREEDEL6 (ijkl)
      
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
c      INTEGER PYK,PYCHGE,PYCOMP
        
c......Standard COMMON blocs                                                                   
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
      COMMON/PYUPPR/NUP,KUP(20,7),NFUP,IFUP(10,2),PUP(20,5),Q2UP(0:10)
                                                                                                                                                                                                                                                  
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/
      
c      print *,'Level 6'      
c      print *,ijkl,'  dotted strings:',k(ijkl,4),
c     #' - ',k(ijkl,5)
c      print *,'       '
                                                         
      do jkm=k(ijkl,4),k(ijkl,5)
      
c	print *,jkm,'  treedel -OK!'
        do i=1,5
         k(jkm,i)=0
c         print *,'  k(',jkm,',',i,') =', k(jkm,i)
        enddo
        
        do i=1,5
         p(jkm,i)=0.D0
c         print *,'  p(',jkm,',',i,') =', p(jkm,i)
        enddo

        do i=1,5
         v(jkm,i)=0.D0
c         print *,'  v(',jkm,',',i,') =', v(jkm,i)
        enddo
c        print *,'   '   
      
      enddo                                                    
      return                                                  
      end

