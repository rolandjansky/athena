!
 SUBROUTINE MUDEMO04
 USE M_MB_Reco_Tracks
 IMPLICIT NONE
 INTEGER :: ican, i, i1, i2
 REAL(8) :: eloss
!
!- Loop on tracks
   DO ican=1,nbcan
     IF( chican(ican) >= 999.d0 )  CYCLE
     print 5000,ican
!
     write(*,'(" global chi2",F9.2," with",i3," dof")') cxxcan(1,ican),ndfcan(ican)
!
     if( mod(flgcan(ican),10) /= 4 ) then
       eloss = vyycan(7,ican) - vxxcan(7,ican)
       print 5002,eloss
     endif
!
     print 6000,FLGCAN(ican)
     
     print 5004
     print 5006,paxcan(1:5,ican),1.d0/paxcan(5,ican)
     print 5012,(sqrt(coxcan(i,i,ican)),i=1,5),sqrt(coxcan(5,5,ican))/paxcan(5,ican)**2
     if( flgcan(ican) /= 4 ) then
       print 5008,paycan(1:5,ican),1.d0/paycan(5,ican)
       print 5012,(sqrt(coycan(i,i,ican)),i=1,5),sqrt(coycan(5,5,ican))/paycan(5,ican)**2
       print 5010,pazcan(1:5,ican)
       print 5012,(sqrt(cozcan(i,i,ican)),i=1,5)
     endif
!
     print 5020
     print 5022,(coxcan( 1,i2,ican)*100./sqrt(coxcan( 1, 1,ican)*coxcan(i2,i2,ican)),i2=1,5)
     do i1=2,5
       print 5028,(coxcan(i1,i2,ican)*100./sqrt(coxcan(i1,i1,ican)*coxcan(i2,i2,ican)),i2=1,5)
     enddo
     if( mod(flgcan(ican),10) /= 4 ) then
       print 5024,(coycan( 1,i2,ican)*100./sqrt(coycan( 1, 1,ican)*coycan(i2,i2,ican)),i2=1,5)
       do i1=2,5
         print 5028,(coycan(i1,i2,ican)*100./sqrt(coycan(i1,i1,ican)*coycan(i2,i2,ican)),i2=1,5)
       enddo
       print 5026,(cozcan( 1,i2,ican)*100./sqrt(cozcan( 1, 1,ican)*cozcan(i2,i2,ican)),i2=1,5)
       do i1=2,5
         print 5028,(cozcan(i1,i2,ican)*100./sqrt(cozcan(i1,i1,ican)*cozcan(i2,i2,ican)),i2=1,5)
       enddo
     endif
   ENDDO
!
5000 FORMAT('    For track nber ',I4)
5002 FORMAT('     Eloss = ',F12.5)
5004 FORMAT('     Track parameters at ')
5006 FORMAT('      Muon System entrance:  ',6F12.5)
5008 FORMAT('      Calorimeter entrance:  ',6F12.5)
5010 FORMAT('      at vertex           :  ',6F12.5)
5012 FORMAT('                       +/-   ',8F12.5)
5020 FORMAT('     Covariance matrice at ')
5022 FORMAT('      Muon System entrance:  ',5F12.2)
5024 FORMAT('      Calorimeter entrance:  ',5F12.2)
5026 FORMAT('      at vertex           :  ',5F12.2)
5028 FORMAT('                             ',5F12.2)
6000 FORMAT('     Track validity      ',I5)
!
 END SUBROUTINE MUDEMO04
!
