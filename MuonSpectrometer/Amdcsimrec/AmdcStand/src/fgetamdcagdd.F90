!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE FGetAMDCcharNumber( Nb )
!
      IMPLICIT NONE
      INTEGER, INTENT(OUT) :: Nb
!--------------------------------------------------------------------
! get Amdc char arrays nber
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
      Nb = LenAMDC
!
      END SUBROUTINE FGetAMDCcharNumber
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE FGetAMDCcharString( Nb, Char1 )
!
      IMPLICIT NONE
      INTEGER,           INTENT(IN)  :: Nb
      CHARACTER (len=1), INTENT(OUT) :: Char1(*)
!
      INTEGER :: I
!--------------------------------------------------------------------
! get Amdc char array
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
      IF( Nb < LenAMDC ) THEN
        PRINT 7771,LenAMDC,Nb
7771    FORMAT(/' In the AMDC package, Atempt to retrieve the AMDC ',   &
                 'Character String (',I7,' characters)',                 &
                /' in a too short String Allocation (',                  &
              I7,' characters) =====> STOP !!!')
        STOP
      ELSE
        DO I=1,LenAMDC
          Char1(I) = CarAMDC(I)
        ENDDO
        DO I=LenAMDC+1,Nb
          Char1(I) = ''
        ENDDO
      ENDIF
!
      END SUBROUTINE FGetAMDCcharString
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE FGetAGDDcharNumber( Nb )
!
      IMPLICIT NONE
      INTEGER, INTENT(OUT) :: Nb
!--------------------------------------------------------------------
! get Agdd strings nber
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
      Nb = LenAGDD
!
      END SUBROUTINE FGetAGDDcharNumber
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE FGetAGDDcharString( Nb, Char1 )
!
      IMPLICIT NONE
      INTEGER,           INTENT(IN)  :: Nb
      CHARACTER (len=1), INTENT(OUT) :: Char1(*)
!
      INTEGER :: I
!--------------------------------------------------------------------
! get Agdd string
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
      IF( Nb < LenAGDD ) THEN
        PRINT 7771,LenAGDD,Nb
7771    FORMAT(/' In the AMDC package, Atempt to retrieve the AGDD ',   &
                'Character String (',I7,' characters)',                 &
               /' in a too short String Allocation (',                  &
             I7,' characters) =====> STOP !!!')
        STOP
      ELSE
        DO I=1,LenAGDD
          Char1(I) = CarAGDD(I)
        ENDDO
        DO I=LenAGDD+1,Nb
          Char1(I) = ''
        ENDDO
      ENDIF
!
      END SUBROUTINE FGetAGDDcharString
!
