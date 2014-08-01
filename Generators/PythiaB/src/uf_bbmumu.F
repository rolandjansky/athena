*CMZ :          
*-- Author :   SERGEY SIVOKLOKOV 2006-04-14
      SUBROUTINE uf_bbmumu(IRET)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : uf_bbmumu                                                   *
*        (module)                                                      *
*                                                                      *
* Description : User subroutine to define  requirements for event      *
*                 accepting/rejecting additional or                    *
*                 other that in subroutine finsel                      *
*                 accept events with M(mumu)<Mass_cut GeV (8GeV)       *
*                              *
*                 additional settings from cufdcp(10) array  
*                           cufdcp(1) = lower Mass_cut                 *
*                           cufdcp(2) = upper Mass_cut                 *
* Arguments   :  IRET (in/out) accepting(0)/rejecting(1) event         *
*                                                                      *
*                                                                      *
* Author : Sergey Sivoklokov            Date : 2006-04-14              *
*                                                                      *
*                                                                      *
* Last modifications :                                                 *
*        [name]                               Date : [date]            *
*                                                                      *
* Keywords :                                                           *
*        [keywords]                                                    *
*----------------------------------------------------------------------*
*
*+CDE, TYPING.
C ----------- end CDE --------------------------------------------------
*
*KEEP,DOUB.
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
c      INTEGER PYK,PYCHGE,PYCOMP
*KEEP,LUJETS.
       COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
*KEEP,LUDAT1.
       COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
*KEEP,LUDAT2.
       COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
*KEEP,PYPARS.
       COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
*KEEP,PYINT1.
       COMMON/PYINT1/MINT(400),VINT(400)
*KEEP,PYINT2.
       COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
*KEEP,PYINT5.
       COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
*KEEP,PYUPPR.
       COMMON/PYUPPR/NUP,KUP(20,7),NFUP,IFUP(10,2),PUP(20,5),Q2UP(0:10)

*     user finsel decay selection and decay parameters
       common /ufdecay/ cufdcp(10),iufdc
       save   /ufdecay/

*KEND.
*      SAVE /LUJETS/,/LUDAT1/,/LUDAT2/
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/
*KEEP,BTREE.
      common/btree/ntree,tree(4000)
      integer tree
      SAVE /btree/
      
      Integer Muplus(10),Muminus(10),Nmum,Nmup
            
      
*KEND.
c      SAVE IFIRST,IIPRNT
      SAVE IFIRST
      DATA IFIRST /1/
c      DATA IIPRNT /0/

*KEEP,FAMILY.
       name_self(I)=K(I,2)
       name_selv(I)=abs(K(I,2))
       name_father(I)=K(K(I,3),2)
       name_grandpa(I)=K(K(K(I,3),3),2)
       name_daughter1(I)=K(K(I,4),2)
       name_daughter2(I)=K(K(I,5),2)

       link_self(I)=I
       link_father(I)=K(I,3)
       link_grandpa(I)=K(K(I,3),3)
       link_daughter1(I)=K(I,4)
       link_daughter2(I)=K(I,5)
*KEND.
C ----------- end CDE --------------------------------------------------

       idebug=0

       IF(IFIRST.EQ.1) THEN
          IFIRST =0
          Print *,'PythiaB/uf_bmumu: lower Mass_cut=cufdcp(1)= ',cufdcp(1)
          Print *,'PythiaB/uf_bmumu: upper Mass_cut=cufdcp(2)= ',cufdcp(2)
          Print *,'iret = ',iret
       ENDIF

c     return immediately if already rejected
      if ( iret.eq.1 ) return

c
c Add your decay selection here.
c Set iret to 0 (acception) or 1 (rejection) per event.
c 
c Parameters: 0 (off) / 1 (on)
c    cufdcp( 1) :               
c    cufdcp( 2) :                
c    cufdcp( 3) :                
c    cufdcp( 4) :                   
c    cufdcp( 5) :                   
c    cufdcp( 6) :                   
c    cufdcp( 7) :                   
c    cufdcp( 8) :                   
c    cufdcp( 9) :                   
c    cufdcp(10) :                   


c     some counting
      nintree = 0

c     preset
      iret = 1

      if(idebug.eq.1)
     +   print *,'USER_FINSEL: BEGINNING'
      Nmum=0
      Nmup=0
c     particle loop  -- modify content as needed
      do  I=1,N
         
c         if (idebug.eq.1) then
c            print *,'USER_FINSEL: part',I,
c     +           link_father(I), tree(I),
c     +           name_self(I), name_father(I)
c         endif
         
c         if(tree(I).eq.1) then
c            if(idebug.eq.1) then
c               nintree = nintree+1
c               print *,'USER_FINSEL: tree',name_self(I)
c            endif
c     
c     Add your code here -- for particles in decay tree.
c     Set iret accordingly.
c         endif
         
         if( K(I,2).eq.13 ) then
            Nmum = Nmum + 1
            Muminus(Nmum) = i
         endif
         if( K(I,2).eq.-13 ) then
            Nmup = Nmup + 1
            Muplus(Nmup) = i
         endif

      enddo
      
      if(idebug.eq.1) print *,'uf_Bmumu Nmu+, Nmu- = ',Nmup,Nmum
            
      Do M = 1,Nmup
         Do L = 1,Nmum
            I = Muplus(M)
            J = Muminus(L)
            
            Py = P(i,2)+P(j,2)
            Pz = P(i,3)+P(j,3)
            E  = P(i,4)+P(j,4)
            Emm = 0.
            Emm2 = E*E - Px*Px - Py*Py - Pz*Pz
            pti = dsqrt(P(i,1)*P(i,1)+P(i,2)*P(i,2))
            ptj = dsqrt(P(j,1)*P(j,1)+P(j,2)*P(j,2))
            if(Emm2.gt.0) Emm = dsqrt(Emm2)
C-------------------------------------------------------------
      If(idebug.eq.1)  print *,' User Finsel: Emm,PTi,PTj= ',Emm,pTi,pTj
            if((Emm.gt.cufdcp(1)).and.(Emm.lt.cufdcp(2))) iret=0
c     x    ((PTi.gt.4.and.ptj.gt.6).or.(ptj.gt.4.and.pti.gt.6))) iret = 0
C-------------------------------------------------------------
         enddo
      enddo
c     evaluation
      
 1000 continue
      if(iret.ne.0) then
c     debug output
         if(idebug.eq.1)
     +        print *, 'USER_FINSEL: event rejected'
      endif

      
      return
*     
      END                       ! USER_FINSEL
