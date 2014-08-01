*CMZ :          
*-- Author :    Wolfgang Walkowiak 2006-04-02
      SUBROUTINE uf_DsPhiX(IRET)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : uf_DsPhiX                                                     *
*        (module)                                                      *
*                                                                      *
* Description : User subroutine to define  requirements for event      *
*                 accepting/rejecting additional or                    *
*                 other that in subroutine finsel                      *
*                 for Ds(PhiPi)X decays using                          *
*                 additional settings from cufdcp(10) array            *
* Arguments   :  IRET (in/out) accepting(0)/rejecting(1) event         *
*                                                                      *
*                                                                      *
* Author : Wolfgang Walkowiak           Date : 2006-04-02              *
*                                                                      *
*                                                                      *
* Last modifications :                                                 *
*        Wolfgang Walkowiak             Date : 2006-04-13              *
*          Fortran comparisons changed from                            *
*            cufdcp(i).eq.1.0  to  cufdcp(i).gt.0.5                    *
*          for additional safety.                                      *
*        Wolfgang Walkowiak             Date : 2006-09-05              *
*          Small bug in the debug output (cufdcp(1)) fixed.            *
*        Wolfgang Walkowiak             Date : 2006-12-11              *
*          Added support for B0->D-(PhiPi)Pi and conjugate.            *
*                                                                      *
* Keywords :                                                           *
*        [keywords]                                                    *
*
* $Id: uf_DsPhiX.F,v 1.4 2007-03-01 17:13:19 msmizans Exp $
*
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
       ENDIF

c     return immediately if already rejected
      if ( iret.eq.1 ) return

c
c Add your decay selection here.
c Set iret to 0 (acception) or 1 (rejection) per event.
c 
c Parameters: 0 (off) / 1 (on)
c    cufdcp(1) : PhiKK   decay -- Phi -> K+K-
c    cufdcp(2) : DsPhi   decay -- Ds*- -> Ds-X, Ds- -> Phi pi-, Phi -> K+K-
c    cufdcp(3) : A1RhoPi decay -- a1 -> RhoPi
c    cufdcp(4) : RhoPiPi decay -- Rho -> Pi+Pi
c   
c
c 

c     some counting
      nintree = 0
c     
      iret_phi   = 0
      iret_dsphi = 0
      iret_a1    = 0
      iret_ro    = 0
      if ( cufdcp(1).gt.0.5 ) iret_phi   = 1
      if ( cufdcp(2).gt.0.5 ) iret_dsphi = 1
      if ( cufdcp(3).gt.0.5 ) iret_a1    = 1
      if ( cufdcp(4).gt.0.5 ) iret_ro    = 1

      if(idebug.eq.1)
     +   print *,'USER_FINSEL: BEGINNING'


      do  I=1,N
         
         if (idebug.eq.1) then
            print *,'USER_FINSEL: part',I,
     +           link_father(I), tree(I),
     +           name_self(I), name_father(I)
         endif
         
         if(tree(I).eq.1) then
            if(idebug.eq.1) then
               nintree = nintree+1
               print *,'USER_FINSEL: tree',name_self(I)
            endif
            
c     a1 -> RhoPi (from B0s->X a1+, B0sbar->X a1- 
c                  or B0(bar)->X a1- or B0(bar)->X a1+)
            if ( iret_a1.eq.1 ) then
               if ( ( name_self(I).eq.20213 .and.
     +              (name_father(I).eq.531 .or.
     +              abs(name_father(I)).eq.511) ) .or.
     +              (name_self(I).eq.-20213 .and.
     +              (name_father(I).eq.-531 .or. 
     +              abs(name_father(I)).eq.511) ) ) then
                  num_daugh=abs(k(I,5)-k(I,4))+1
                  if(k(I,4).eq.0) num_daugh=0
                  if(idebug.eq.1)
     +                 print *, 'USER_FINSEL: a1+/- daughters',
     +                 name_self(I), num_daugh, ':',
     +                 k(k(I,4),2), k(k(I,5),2)
                  if(num_daugh.ne.2) then
                     iret=1
                     if(idebug.eq.1)
     +                    print *,
     +                    'USER_FINSEL: a1+/- not into two daughters',
     +                    name_self(I)
                  else
                     if((name_daughter1(I).eq.113 .and.
     +                    abs(name_daughter2(I)).eq.211      ) .or.
     +                    (abs(name_daughter1(I)).eq.211 .and.
     +                    name_daughter2(I).eq.113      ) )  then
                        iret_a1=0
                        if(idebug.eq.1)
     +                       print *,
     +                       'USER_FINSEL: a1+/- ->ro0 pi+/- in Bs',
     +                       name_self(I)
                     else
                        iret=1
                        if(idebug.eq.1)
     +                       print *,
     +                       'USER_FINSEL: no ro0 pi+/- decay for a1+/-'
     +                       ,name_self(I)
                     endif
                  endif
               endif
            endif

            if (iret.eq.1) go to 2000

c     Rho -> Pi+Pi- (from B0s->X a1+, B0sbar->X a1-
c                    or B0(bar)->X a1- or B0(bar)->X a1+)

            if ( iret_ro.eq.1 ) then
               if ( name_self(I).eq.113 .and.
     +              ( ( name_father(I).eq.20213 .and.
     +              ( name_grandpa(I).eq.531 .or.
     +              abs(name_grandpa(I)).eq.511 ) ) .or.
     +              ( ( name_father(I).eq.-20213 .and.
     +              ( name_grandpa(I).eq.-531 .or.
     +              abs(name_grandpa(I)).eq.511 ) ) ) ) ) then
                  
                  num_daugh=abs(k(I,5)-k(I,4))+1
                  if(k(I,4).eq.0) num_daugh=0
                  if(num_daugh.ne.2) then
                     iret=1
                     if(idebug.eq.1)
     +                    print *,
     +                    'USER_FINSEL: ro0 not into two daughters',
     +                    name_self(I)
                  else
                     if(abs(name_daughter1(I)).ne.211 .or.
     +                    abs(name_daughter2(I)).ne.211     ) then
                        iret=1
                        if(idebug.eq.1)
     +                       print *,
     +                       'USER_FINSEL: no pi+ pi- decay for ro',
     +                       name_self(I)
                     else
                        iret_ro=0
                        if(idebug.eq.1)
     +                       print *, 'USER_FINSEL: ro0 -> pi+ pi-',
     +                       name_self(I)
                     endif
                  endif
               endif
            endif
            
            if(iret.eq.1) go to 2000
            
c     Phi -> K+K- ((from B0s, B0bar or B-) or (from B0sbar, B0 or B+)
c                  or (from B0->D-X) or (B0bar->D+X))
            if ( iret_phi.eq.1 ) then
               if (name_self(I).eq.333 .and. (
     +              ( name_father(I).eq.-431 .and.
     +              ( name_grandpa(I).eq.531 .or.
     +              name_grandpa(I).eq.-511 .or.
     +              name_grandpa(I).eq.-521 ) ) .or.
     +              ( name_father(I).eq.+431 .and.
     +              ( name_grandpa(I).eq.-531 .or.
     +              name_grandpa(I).eq.+511 .or.
     +              name_grandpa(I).eq.+521 ) ) .or.
     +              ( name_father(I).eq.-411 .and.
     +              name_grandpa(I).eq.+511 ) .or. 
     +              ( name_father(I).eq.+411 .and. 
     +              name_grandpa(I).eq.-511 ) ) ) then
                  
                  num_daugh=abs(k(I,5)-k(I,4))+1
                  if(k(I,4).eq.0) num_daugh=0
                  if(num_daugh.ne.2) then
                     iret=1
                     if(idebug.eq.1)
     +                    print *,
     +                    'USER_FINSEL: phi not into two daughters',
     +                    name_self(I)
                  else
                     if(abs(name_daughter1(I)).ne.321 .or.
     +                    abs(name_daughter2(I)).ne.321     ) then
                        iret=1
                        if(idebug.eq.1)
     +                       print *,
     +                       'USER_FINSEL: no K+ K- decay for phi',
     +                       name_self(I)
                     else
                        iret_phi=0
                        if(idebug.eq.1)
     +                       print *, 'USER_FINSEL: phi--> K+ K- ',
     +                       name_self(I)
                     endif
                  endif
               endif
            endif
            

c     Ds*- -> Ds-X, Ds- -> Phi pi-, Phi -> K+K-
c             ((from B0s, B0bar or B-) or (from B0sbar, B0 or B+))
            if ( iret_dsphi.eq.1 ) then
               if( ( name_self(I).eq.-431 .and.
     +              name_father(I).eq.-433 .and.
     +              ( name_grandpa(I).eq.+531 .or.
     +              name_grandpa(I).eq.-511 .or.
     +              name_grandpa(I).eq.-521 ) ) .or.
     +              ( name_self(I).eq.+431  .and.
     +              name_father(I).eq.+433 .and.
     +              ( name_grandpa(I).eq.-531 .or.
     +              name_grandpa(I).eq.+511 .or.
     +              name_grandpa(I).eq.+521 ) )
     +              ) then
                  
                  num_daugh=abs(k(I,5)-k(I,4))+1
                  if(k(I,4).eq.0) num_daugh=0
                  if(num_daugh.ne.2) then
                     iret=1
                     if(idebug.eq.1)
     +                    print *,
     +                    'USER_FINSEL: ds not into two daughters',
     +                    name_self(I)
                  else
                     iddau = -1
                     iret=1
                     if ( name_daughter1(I).eq. 333 .and.
     +                    ( ( name_daughter2(I).eq.-211 .and.
     +                    name_self(I).eq.-431) .or.
     +                    ( name_daughter2(I).eq.+211 .and.
     +                    name_self(I).eq.+431) )
     +                    ) then
                        iddau = link_daughter1(I)
                        iret=0
                     endif
                     if ( ( ( name_daughter1(I).eq.-211 .and.
     +                    name_self(I).eq.-431) .or.
     +                    ( name_daughter1(I).eq.+211 .and.
     +                    name_self(I).eq.+431) ) .and.
     +                    name_daughter2(I).eq. 333
     +                    ) then
                        iddau = link_daughter2(I)
                        iret=0
                     endif
                     if(idebug.eq.1 .and. iret.eq.1)
     +                    print *,
     +                    'USER_FINSEL: no Phi pi- decay for ds',
     +                    name_self(I),
     +                    name_daughter1(I), name_daughter2(I)
                     
                     if ( iret.eq.0 ) then
                        num_daugh=abs(k(iddau,5)-k(iddau,4))+1
                        if(k(iddau,4).eq.0) num_daugh=0
                        if(num_daugh.ne.2) then
                           iret=1
                           if(idebug.eq.1)
     +                          print *,
     +                          'USER_FINSEL: phi not into',
     +                          ' two daughters',
     +                          name_self(iddau),
     +                          name_daughter1(iddau),
     +                          name_daughter2(iddau)
                        else
                           if(abs(name_daughter1(iddau)).ne.321 .or.
     +                          abs(name_daughter2(iddau)).ne.321)
     +                          then
                              iret=1
                              if(idebug.eq.1)
     +                             print *,
     +                             'USER_FINSEL: no K+ K- decay ',
     +                             'for phi',
     +                             name_self(I)
                           else
                              iret_dsphi=0
                              if(idebug.eq.1)
     +                             print *, 'USER_FINSEL: ',
     +                             'Ds-->Phi pi, Phi--> K+ K- ',
     +                             name_self(I)
                           endif
                        endif
                     endif
                  endif
               endif
            endif
            
            if(iret.eq.1) go to 2000
            
c     quick exit if all are found
            if ( iret_a1.eq.0 .and. iret_ro.eq.0
     +           .and. iret_phi.eq.0
     +           .and. iret_dsphi.eq.0 )
     +           goto 1000
            
         endif
      enddo

c     evaluation
      
 1000 continue
      iret=1
      if(iret_a1.ne.0) then
         iret=1
         if(idebug.eq.1)
     +        print *, 'USER_FINSEL: event rejected, a1+ flag'
      elseif(iret_ro.ne.0) then
         iret=1
         if(idebug.eq.1)
     +        print *, 'USER_FINSEL: event rejected, ro0 flag'
      elseif(iret_phi.ne.0) then
         iret=1
         if(idebug.eq.1)
     +        print *, 'USER_FINSEL: event rejected, phi flag'
      elseif(iret_dsphi.ne.0) then
         iret=1
         if(idebug.eq.1)
     +        print *, 'USER_FINSEL: event rejected, dsphi flag'
      else
         iret=0
      endif
      
 2000 continue
      
c     debug output
      if (idebug.eq.1)
     +     print *,
     +     'USER_FINSEL: iPhiKK, iDsPhi, iA1RhoPi, iRhoPiPi, ',
     +     'N, nintree: ',
     +     cufdcp(1),cufdcp(2),cufdcp(3),cufdcp(4),N,nintree
      
      return
*     
      END                       ! USER_FINSEL
