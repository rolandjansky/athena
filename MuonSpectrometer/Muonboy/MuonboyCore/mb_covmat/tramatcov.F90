!
!> Transport the covariance matrix from the spectrometer entrance
!> to the calorimeter entrance, and then to the perigee
!> \author J.F.Laporte
!
 SUBROUTINE TRAMATCOV
 USE M_MB_Control
 USE M_MB_General_CONSTANTS, ONLY : Pi, TwoPi
 USE M_MB_Reco_Tracks
 USE M_MB_CTRABAZ
 USE M_MB_COMUSC
 USE M_MB_FIT
 IMPLICIT NONE
 INTEGER, PARAMETER :: nparmax=50
 real(8) :: vparin(nparmax),vparin0(nparmax)
 real(8) :: vparou(nparmax),vparou0(nparmax)
 real(8) :: grandt(nparmax,nparmax),grandtpm(nparmax,nparmax,2)
 integer :: ltopar(nparmax)
 real(8) :: var(nparmax),evar(nparmax)
 real(8) :: grandc(nparmax,nparmax), grandm(nparmax,nparmax), grandk(nparmax,nparmax)
 INTEGER :: npp, itypfo, ican, ikind, ivar, isens, nparam, iprloc
 INTEGER :: l, i, i1, i2
 REAL(8) :: enmasco
 REAL(8), PARAMETER :: chouiar = 1.d0, chouiaz = 1.d0
!
!- Set type fit
   itypfo  = ItypFit
   ItypFit = 3
   enmasco = enmasc
!
   npp = 5
   if( abs(FixMoCur) > 0.1 ) npp = 4
!
   iprloc = 0
#if !defined(ATLAS_NOPRINTMU)
   if( MBPri >= 1 ) iprloc = 1
   if( iprloc >= 1 ) print *,'**tramatcov '
#endif
!
!- Set loose external limits
   rchama = rcylin(3) + chouiar
   zchama = zcylin(3) + chouiaz
!
!- Loop on tracks
   do ican=1,nbcan
     if( chican(ican) >= 999.d0 ) CYCLE
     if( flgcan(ican) <= 1      ) CYCLE   ! stays inside spectrometer
     if( flgcan(ican) == 4      ) THEN    ! stops inside calorimeter
       coycan(1:5,1:5,ican) = coxcan(1:5,1:5,ican)
       cozcan(1:5,1:5,ican) = coycan(1:5,1:5,ican)
       CYCLE
     endif
#if !defined(ATLAS_NOPRINTMU)
     if (iprloc >= 1) print *,'  ican  ', ican
#endif
!
!- Loop on candidate kind
!- ikind=2=ID exit, ikind=3=at vertex
     do 1010 ikind=2,3
       if( ikind == 3 .AND. flgcan(ican) == 2 ) then
         cozcan(1:5,1:5,ican) = coycan(1:5,1:5,ican)
         EXIT
       endif
#if !defined(ATLAS_NOPRINTMU)
       if (iprloc >= 1) print *,'ikind  ', ikind
#endif
!
!- Set matter centers arrays
       nbmasc     = nbmsxx(ican)
       enmasc     = enmsxx(ican)
       rrmasc(1:nbmasc) = rrmsxx(1:nbmasc,ican)
       llmasc(1:nbmasc) = llmsxx(1:nbmasc,ican)
       jvmasc(1:nbmasc) = 0
       x0masc(1:nbmasc) = x0msxx(1:nbmasc,ican)
       s1masc(1:nbmasc) = s1msxx(1:nbmasc,ican)
       s2masc(1:nbmasc) = s2msxx(1:nbmasc,ican)
       thmasc(1:nbmasc) = thmsxx(1:nbmasc,ican)
       almasc(1:nbmasc) = eamsxx(1:nbmasc,ican)
       blmasc(1:nbmasc) = ebmsxx(1:nbmasc,ican)
!
!- Store central values for the input and output parameters
       ltopar(1:5)  = 0
       if(ikind == 2) then
         vparin0(1:5) = paxcan(1:5,ican)
         vparou0(1:5) = paycan(1:5,ican)
       else if(ikind == 3) then
         vparin0(1:5) = paycan(1:5,ican)
         vparou0(1:5) = pazcan(1:5,ican)
       endif
       nparam = 5
       if (ikind == 2) then
         do l=1,nbmasc
           if (llmasc(l) >= 6) then
             nparam = nparam + 1
             ltopar(nparam)  = l
             vparin0(nparam) = s1masc(l)
             vparou0(nparam) = s1masc(l)
             nparam = nparam + 1
             ltopar(nparam) = -l
             vparin0(nparam) = s2masc(l)
             vparou0(nparam) = s2masc(l)
           endif
         enddo
         nparam = nparam + 1  ! eloss fluctuation term
         ltopar(nparam)  = 0
         vparin0(nparam) = 0.d0
         vparou0(nparam) = 0.d0
       endif
!
!- Set variations amplitudes of the input parameters
       do i=1,5
         if (ikind == 2) evar(i) = sqrt(coxcan(i,i,ican))
         if (ikind == 3) evar(i) = sqrt(coycan(i,i,ican))
       enddo
       evar(6:nparam) = 1.d0
!
!- Vary parameters one by one
       do 1100 ivar=1,nparam
         if( npp == 4 .AND. ivar == 5 ) then
           grandtpm(1:nparam,ivar,1:2) = 0.d0
           grandtpm(ivar    ,ivar,1:2) = 1.d0           
           cycle
         endif
!
!- Vary direction of variation
         do 1200 isens=1,2
!
!-- Set actual variations of the input parameters
           var(1:nparam)  = 0.d0
           var(ivar)= (-1.d0)**isens
!-- Set input parameters
           vparin(1:nparam) = vparin0(1:nparam) + evar(1:nparam)*var(1:nparam)
!-- Set matter/Eloss parameters
           do i=1,nparam
             if (ltopar(i) > 0) s1masc( ltopar(i)) = vparin(i)
             if (ltopar(i) < 0) s2masc(-ltopar(i)) = vparin(i)
           enddo
           enmasc = enmsxx(ican)
           if (ikind == 2) enmasc = enmsxx(ican)*(1.d0 + vparin(nparam)*sigenmasc)
!-- Get starting muon vector
           if (ikind == 2) rchami = rcylin(3)
           if (ikind == 2) zchami = zcylin(3)
           if (ikind == 3) rchami = rcylin(2)
           if (ikind == 3) zchami = zcylin(2)
           call setvdeb(2,vparin)
!-- Reverse starting muon vector
           vdeb(4:6,1) = - vdeb(4:6,1)
           vdeb( 8 ,1) = - vdeb( 8 ,1)
!- Track and get exit parameters
           if( ikind == 2 .and. flgcan(ican) > 2 ) then
             rchami = rcylin(2)
             zchami = zcylin(2)
             nztpla = 0
             call TRABAZ(1,1,0)
             call TKVFIN2
             vparou(1:5) = vthetaphi(1:5)
             if( vparou(2) - vparou0(2) >  Pi ) vparou(2) = vparou(2) - TwoPi
             if( vparou(2) - vparou0(2) < -Pi ) vparou(2) = vparou(2) + TwoPi
             if( vparou(4) - vparou0(4) >  Pi ) vparou(4) = vparou(4) - TwoPi
             if( vparou(4) - vparou0(4) < -Pi ) vparou(4) = vparou(4) + TwoPi
           else
             call TOPERI
             call TKVFIN3
             vparou(1:5) = vperigee(1:5)
             if( vparou(3) - vparou0(3) >  Pi ) vparou(3) = vparou(3) - TwoPi
             if( vparou(3) - vparou0(3) < -Pi ) vparou(3) = vparou(3) + TwoPi
           endif
           vparou(6:nparam) = vparin(6:nparam)
!- Compute variations of exit parameters
           grandtpm(1:nparam,ivar,isens) = (vparou(1:nparam)- vparou0(1:nparam))*var(ivar)/evar(ivar)
!
1200     enddo
1100   enddo
!
!- Compute transfert matrice T
       grandt(1:nparam,1:nparam) = (grandtpm(1:nparam,1:nparam,1)+grandtpm(1:nparam,1:nparam,2))/2.d0
!- C = (c)0
!-      0 I
       grandc(1:nparam,1:nparam) = 0.d0
       if (ikind == 2) grandc(1:5,1:5) = coxcan(1:5,1:5,ican)
       if (ikind == 3) grandc(1:5,1:5) = coycan(1:5,1:5,ican)
       do l=6,nparam
         grandc(l,l) = 1.d0
       enddo
!- M = T x C
       do i1=1,nparam
         do i2 =1,nparam
           grandm(i1,i2) = SUM( grandt(i1,1:nparam)*grandc(1:nparam,i2) )
         enddo
       enddo
!            t
!- K = M x T
       do i1=1,nparam
         do i2 =1,nparam
           grandk(i1,i2) = SUM( grandm(i1,1:nparam)*grandt(i2,1:nparam) )
         enddo
       enddo
!-- Store convariance matrix
       if (ikind == 2) coycan(1:5,1:5,ican) = grandk(1:5,1:5)
       if (ikind == 3) cozcan(1:5,1:5,ican) = grandk(1:5,1:5)
!
#if !defined(ATLAS_NOPRINTMU)
!- Print out +/- transfert matrices
       if (iprloc >= 2) then
         write(*,*)
         do i1 =1,nparam
           write(*,'("grandt s1 ",i2,15f12.6)') i1,grandtpm(i1,1:nparam,1)
         enddo
         write(*,*)
         do i1 =1,nparam
           write(*,'("grandt s2 ",i2,15f12.6)') i1,grandtpm(i1,1:nparam,2)
         enddo
       endif
!- Print out intermediate matrices
       if (iprloc >= 2) then
         write(*,*)
         do i1 =1,nparam
           write(*,'("grandt    ",i2,15f12.6)') i1,grandt(i1,1:nparam)
         enddo
         write(*,*)
         do i1 =1,nparam
           write(*,'("grandc    ",i2,15f12.6)') i1,grandc(i1,1:nparam)
         enddo
         write(*,'(" Covmat  ",3x,15i12)') (i2,i2=1,nparam)
         do i1 =1,nparam
           write(*,'("         ",i3,15f12.6)') i1,grandk(i1,1:nparam)
         enddo
       endif
!- Print out  covariance matrix in compact format
       if( iprloc >= 1 ) then
           write(*,'(" Norm    ",3x,15i8)') (i2,i2=1,5)
         do i1=1,5
           write(*,'("         ",i3,15f8.2)') i1,(grandk(i1,i2)*100./sqrt(grandk(i1,i1)*grandk(i2,i2)),i2=1,5)
         enddo
         write(*,'(" Parameters ")')
         if( ikind == 2 ) write(*,'(" tetP,phiP,tetV,phiV,Pinv = ",5f13.7)') vparou0(1:5)
         if( ikind == 3 ) write(*,'(" a0,z0,phi,cotth,ptinv    = ",5f13.7)') vparou0(1:5)
                          write(*,'("                        +/- ",5f13.7)') (sqrt(grandk(i,i)),i=1,5)
       endif
#endif
!
1010 enddo
   enddo
!
!- Reset the number of m.s. centers to 0,
!- ItypFit to its original value and tracking limits to their std. values
   CALL Reset_MB_COMUSC
   enmasc  = enmasco
   ItypFit = itypfo
   rchami  = rcylin(3)
   zchami  = zcylin(3)
   rchama  = rcylin(4)
   zchama  = zcylin(4)
!
 END SUBROUTINE TRAMATCOV
!
