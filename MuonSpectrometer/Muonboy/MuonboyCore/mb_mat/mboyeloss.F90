!
 SUBROUTINE MboyEloss
 USE M_MB_Control
 IMPLICIT NONE
!
#include "AmdcStand/comama.inc"
!
      INTEGER :: i,im
      REAL(8) :: RatioCor
      REAL(8) :: EADATA_Old
      REAL(8) :: EBDATA_Old
      REAL(8) :: Ec_New
      REAL(8) :: ZMboyMaterial
      REAL(8), PARAMETER :: SCALEIONI=1.15d0
!
      WRITE(*,'(" MboyEloss: MEloss = ",I3)') MEloss
!     
      if( MEloss == 0 ) then
!- Correct energy dependence of E.loss which looks too strong
        do i=1,NBMAMA
          im = IIMAMA(i)
          EADATA_Old = EAMAMA(im)
          EBDATA_Old = EBMAMA(im)
          EAMAMA(im) = EADATA_Old / 1.15D0
          EBMAMA(im) = EBDATA_Old
          WRITE(*,1500) im,EADATA_Old,EAMAMA(im),EAMAMA(im)/EADATA_Old,  &
                           EBDATA_Old,EBMAMA(im),EBMAMA(im)/EBDATA_Old
        enddo      
!
      else if( MEloss == 1 ) then
        do i=1,NBMAMA
          im = IIMAMA(i)
          ZMboyMaterial = abs(ZZMAMA(im)) 
          if( ZMboyMaterial >= 1.d0 ) then
            EADATA_Old = EAMAMA(im)
            EBDATA_Old = EBMAMA(im)
            RatioCor   = SCALEIONI
            EBMAMA(im) = RatioCor * EBDATA_Old
            Ec_New     = 5700.D0/(ZMboyMaterial+1.47D0)**0.838D0
            EAMAMA(im) = 1000.D0 * EBMAMA(im)/Ec_New
            WRITE(*,1500) im,EADATA_Old,EAMAMA(im),EAMAMA(im)/EADATA_Old,  &
                             EBDATA_Old,EBMAMA(im),EBMAMA(im)/EBDATA_Old
          endif
        enddo      
!
      else if( MEloss == 2 ) then
        do i=1,NBMAMA
          im = IIMAMA(i)
          if ( im == 14 .or. im == 28 .or. im == 23 ) then
            EADATA_Old = EAMAMA(im)
            EBDATA_Old = EBMAMA(im)
            RatioCor = 1.d0
!Correct Tile matter
            if( im == 14 ) RatioCor = 0.82d0/0.70d0
!Correct Tile ECH1
            if( im == 28 ) RatioCor = 0.94d0/0.80d0
!Correct Tile ECH2
            if( im == 23 ) RatioCor = 0.97d0/0.88d0
            EAMAMA(im) = EADATA_Old * RatioCor
            EBMAMA(im) = EBDATA_Old * RatioCor
            WRITE(*,1500) im,EADATA_Old,EAMAMA(im),EAMAMA(im)/EADATA_Old,  &
                             EBDATA_Old,EBMAMA(im),EBMAMA(im)/EBDATA_Old
          end if
        enddo
!
      else if( MEloss == 3 ) then
        do i=1,NBMAMA
          im = IIMAMA(i)
          ZMboyMaterial = abs(ZZMAMA(im)) 
          if( ZMboyMaterial >= 1.d0 ) then
            EADATA_Old = EAMAMA(im)
            EBDATA_Old = EBMAMA(im)
            RatioCor = 1.d0
!Correct Tile matter        
            if( im == 14 ) RatioCor = 0.82d0/0.70d0
!Correct Tile ECH1       
            if( im == 28 ) RatioCor = 0.94d0/0.80d0
!Correct Tile ECH2        
            if( im == 23 ) RatioCor = 0.97d0/0.88d0
            EBMAMA(im) = RatioCor * EBDATA_Old
            Ec_New     = 5700.D0/(ZMboyMaterial+1.47D0)**0.838D0
            EAMAMA(im) = 1000.D0 * EBMAMA(im)/Ec_New
            WRITE(*,1500) im,EADATA_Old,EAMAMA(im),EAMAMA(im)/EADATA_Old,  &
                             EBDATA_Old,EBMAMA(im),EBMAMA(im)/EBDATA_Old
          endif
        enddo      
!
      else if( MEloss == 4 ) then
        do i=1,NBMAMA
          im = IIMAMA(i)
          ZMboyMaterial = abs(ZZMAMA(im)) 
          if( ZMboyMaterial >= 1.d0 ) then
            EADATA_Old = EAMAMA(im)
            EBDATA_Old = EBMAMA(im)
            RatioCor = 1.d0
!Correct Tile matter
            if( im == 14 ) RatioCor = 0.82d0/0.70d0 * 0.90d0
!Correct Tile ECH1       
            if( im == 28 ) RatioCor = 0.94d0/0.80d0 * 0.90d0
!Correct Tile ECH2        
            if( im == 23 ) RatioCor = 0.97d0/0.88d0 * 0.88d0
            EBMAMA(im) = RatioCor * EBDATA_Old
            Ec_New     = 5700.D0/(ZMboyMaterial+1.47D0)**0.838D0
            EAMAMA(im) = 1000.D0 * EBMAMA(im)/Ec_New
            WRITE(*,1500) im,EADATA_Old,EAMAMA(im),EAMAMA(im)/EADATA_Old,  &
                             EBDATA_Old,EBMAMA(im),EBMAMA(im)/EBDATA_Old
          endif
        enddo
!
      else
        WRITE(*,'(" ERROR : value not expected in MuonboyCore/MboyEloss")')        
      endif
!
1500  FORMAT(' Material nber ',I3,                      &
             '  EAMAMA ',F7.2,'->',F7.2,'{',F7.2,')',   &
             '  EBMAMA ',F7.2,'->',F7.2,'{',F7.2,')')
!
 END SUBROUTINE MboyEloss
!
