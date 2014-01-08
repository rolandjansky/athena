!
!> backtracking of the muon candidates from the spectro entrance to the calo entrance, then to perigee
!> \author M.Virchaux
!
 SUBROUTINE BAKTRAK
 USE M_MB_Control
 USE M_MB_Reco_Tracks
 USE M_MB_COMUSC
 USE M_MB_Fit
 USE M_MB_CTRABAZ
 IMPLICIT NONE
 INTEGER :: ican, l, nmamax
 REAL(8) :: enmasco, r2fin, azfin
!
!
   enmasco = enmasc
!
!-- Loop on track candidates
   DO 20 ican=1,nbcan
     if( chican(ican) >= 999.d0 ) CYCLE
     if( flgcan(ican) == 1      ) CYCLE  ! do not baktrak tracks which do not leave spectro
!
!-- Get VDEB at the entrance of the muon spectro. (R_Z_cylin(3))
     vdeb(1:8,1) = vxxcan(1:8,ican)
!
!-- Set tracking limits to both sides of the calos
     rchami = rcylin(2)
     zchami = zcylin(2)
     rchama = rcylin(3)
     zchama = zcylin(3)
!
!-- Compute the radii of m.s. and e.loss through the calos
     nbmasc = 0
     if( abs(FixMoCur) < 0.1d0 )  call TRMUSC(12) ! needed only to take Eloss into account
     call TRMUSC(12)
     IF( Lstop ) THEN
       IF( MBPri >= 3 ) write(*,'(" Candidate track stops inside Calorimeter volume")')
       flgcan(ican) = 4
       vyycan(1:8,ican) = vxxcan(1:8,ican)
       paycan(1:5,ican) = paxcan(1:5,ican)
       vzzcan(1:8,ican) = vyycan(1:8,ican)
       pazcan(1:5,ican) = paycan(1:5,ican)
       CYCLE
     ENDIF
!
#if !defined(ATLAS_NOPRINTMU)
     if(MBPri >= 3) then
       print 7895,ican,nbmesu,nbmasc
7895   format(/' -------------------------------------------------',   &
       '----------------------------------------------------------',   &
       /' Back-Tracking of candidate track Nb.',I3                     &
       //' After 2nd inward extrapolation with TRMUSC : ',             &
       'NBmesu,NBmasc =',2I6)
       call PRIMASC
     endif
#endif
!
!-- Reduce the number of these radii of m.s. to 2
!-- (but not the number of radii of e.loss)
     nmamax = kmasc - nbmsxx(ican)
     call REDUMS(nmamax)
!--
!-- No eloss to be applied when momentum is fixed
     if( abs(FixMoCur) > 0.1d0 ) then
       do l=1,nbmasc
         llmasc(l) = ibclr(llmasc(l),0)
       enddo
     endif
#if !defined(ATLAS_NOPRINTMU)
     if(MBPri >= 3) then
       write(*,'(/" After REDUMS : NBmesu,NBmasc =",2I6)') nbmesu,nbmasc
       call PRIMASC
     endif
#endif
!-- ------------------------------------------------------------
!-- Back_track the candidate track to the exit of the I.D. (R_Z_cylin(2))
!-- and get  VFIN and VTHETAPHI  there
!-- (reverse starting muon direction and goes towards IP)
     vdeb(4:6,1) = - vdeb(4:6,1)
     vdeb( 8 ,1) = - vdeb( 8 ,1)
     nztpla = 0
     call TRABAZ(1,1,0)
!-- and retrive raddi of m.s. and e.loss in the muon spectro. region
     do l=1,nbmsxx(ican)
       rrmasc(l+nbmasc)         = rrmsxx(l,ican)
       llmasc(l+nbmasc)         = llmsxx(l,ican)
       jvmasc(l+nbmasc)         = 0
       x0masc(l+nbmasc)         = x0msxx(l,ican)
       s1masc(l+nbmasc)         = s1msxx(l,ican)
       s2masc(l+nbmasc)         = s2msxx(l,ican)
       thmasc(l+nbmasc)         = thmsxx(l,ican)
       almasc(l+nbmasc)         = eamsxx(l,ican)
       blmasc(l+nbmasc)         = ebmsxx(l,ican)
       VinVouMASC(1:8,l+nbmasc) = VinVouXX(1:8,l,ican)
       VinVou2MASC(1:8,l+nbmasc)= VinVouXX2(1:8,l,ican)
     enddo
     nbmasc = nbmasc + nbmsxx(ican)
     enmasc = enmsxx(ican)
     nbmsxx(ican) = min( nbmasc , NXXMAX )
     do l=1,nbmsxx(ican)
       rrmsxx(l,ican)        = rrmasc(l)
       llmsxx(l,ican)        = llmasc(l)
       x0msxx(l,ican)        = x0masc(l)
       s1msxx(l,ican)        = s1masc(l)
       s2msxx(l,ican)        = s2masc(l)
       thmsxx(l,ican)        = thmasc(l)
       if( mod(llmasc(l),2) == 1 ) then
         eamsxx(l,ican)      = almasc(l)
         ebmsxx(l,ican)      = blmasc(l)
       else
         eamsxx(l,ican)      = 1.d0
         ebmsxx(l,ican)      = 0.d0
       endif
       VinVouXX(1:8,l,ican)  = VinVouMASC(1:8,l)
       VinVouXX(4,l,ican)    = VinVouXX(4,l,ican)*VinVouXX(8,l,ican)*VXXCAN(8,ican)
       VinVouXX(5,l,ican)    = VinVouXX(5,l,ican)*VinVouXX(8,l,ican)*VXXCAN(8,ican)
       VinVouXX(6,l,ican)    = VinVouXX(6,l,ican)*VinVouXX(8,l,ican)*VXXCAN(8,ican)
       VinVouXX(8,l,ican)    = VXXCAN(8,ican)
       VinVouXX2(1:8,l,ican) = VinVou2MASC(1:8,l)
       VinVouXX2(4,l,ican)   = VinVouXX2(4,l,ican)*VinVouXX2(8,l,ican)*VXXCAN(8,ican)
       VinVouXX2(5,l,ican)   = VinVouXX2(5,l,ican)*VinVouXX2(8,l,ican)*VXXCAN(8,ican)
       VinVouXX2(6,l,ican)   = VinVouXX2(6,l,ican)*VinVouXX2(8,l,ican)*VXXCAN(8,ican)
       VinVouXX2(8,l,ican)   = VXXCAN(8,ican)
     enddo
#if !defined(ATLAS_NOPRINTMU)
     if( MBPri >= 3 ) then
       print 7901,nbmasc
7901   format(/' Stored in M_MB_Reco_Tracks : NBmasc =',I6)
       call PRIMASC
     endif
#endif
!
! flag muons which stops inside calorimeter
!
     IF( vfin(7) < 0.d0 ) THEN
       IF( MBPri >= 3 ) write(*,'(" Candidate track stops inside Calorimeter volume")')
       flgcan(ican) = 4
       vyycan(1:8,ican) = vxxcan(1:8,ican)
       paycan(1:5,ican) = paxcan(1:5,ican)
       vzzcan(1:8,ican) = vyycan(1:8,ican)
       pazcan(1:5,ican) = paycan(1:5,ican)
       CYCLE
     ENDIF
!
     r2fin = sqrt( vfin(1)**2 + vfin(2)**2 )
     azfin = abs( vfin(3) )
     IF( ( abs(r2fin-rchami) > 0.1d0 .AND. abs(azfin-zchami) > 0.1d0 ) .OR.  &
           dot_product(vfin(1:2),vfin(4:5)) < 0.d0 ) THEN
       IF( AtCosm == 0 ) THEN
#if !defined(ATLAS_NOPRINTMU)
         IF( MBPri >= 3 ) write(*,'(" Candidate track with Pmu =",F9.2," GeV has no perigee",    &
                                  & " inside InDet volume ====> Reject it !")')  vfin(8)*vfin(7)
#endif
         chican(ican) = chican(ican) + 1000.d0
       ENDIF
!-- Go back to spectro entrance (R_Z_cylin(3)) and compute perigee
       vdeb(1:8,1) = vxxcan(1:8,ican)
!-- flag this track
       flgcan(ican) = 2
     ELSE                         ! "standard" track crossing the calo entrance 
!-- store these parameters
       call TKVFIN2
       vyycan(1:8,ican) = vfin(1:8)
       paycan(1:5,ican) = vthetaphi(1:5)
!-- Get new starting parameters  VDEB  from  VFIN
       vdeb(1:8,1) = vfin(1:8) 
!-- flag this track
       flgcan(ican) = 3
     ENDIF
!-- Back-track further the candidate track to the perigee
!-- and get  VFIN and VPERIGEE  there
     call TOPERI
     call TKVFIN3
!-- and store these parameters
     vzzcan(1:8,ican) = vfin(1:8)
     pazcan(1:5,ican) = vperigee(1:5)
     if( flgcan(ican) == 2 ) then
       vyycan(1:8,ican) = vzzcan(1:8,ican)
       paycan(1:5,ican) = pazcan(1:5,ican)
     endif
!
20 enddo
      if(MBPri >= 3) print 7896
7896  format(/' -------------------------------------------------',  &
      '----------------------------------------------------------'/)
!
!
!>> Reset the number of m.s. centers and tracking limits to their std. values
   CALL Reset_MB_COMUSC
   enmasc = enmasco
   rchami = rcylin(3)
   zchami = zcylin(3)
   rchama = rcylin(4)
   zchama = zcylin(4)
!
 END SUBROUTINE baktrak
!
