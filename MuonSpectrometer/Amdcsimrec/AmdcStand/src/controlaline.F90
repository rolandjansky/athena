!
!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE ControlILine(IControlILine)
!
      IMPLICIT NONE
      INTEGER, INTENT(IN) :: IControlILine
!
      INTEGER :: IControlILineD
      INTEGER :: IControlILine1, IControlILine2, IControlILine3
      INTEGER :: IControlILine4, IControlILine5, IControlILine6
      INTEGER :: Item
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
      IControlILineD = IControlILine
      IControlILine1 = IControlILineD /100000
      IControlILineD = IControlILineD - IControlILine1 * 100000
      IControlILine2 = IControlILineD / 10000
      IControlILineD = IControlILineD - IControlILine2 *  10000
      IControlILine3 = IControlILineD /  1000
      IControlILineD = IControlILineD - IControlILine3 *   1000
      IControlILine4 = IControlILineD /   100
      IControlILineD = IControlILineD - IControlILine4 *    100
      IControlILine5 = IControlILineD /    10
      IControlILineD = IControlILineD - IControlILine5 *     10
      IControlILine6 = IControlILineD 
      
!      print *,'IControlILine1 ',IControlILine1
!      print *,'IControlILine2 ',IControlILine2
!      print *,'IControlILine3 ',IControlILine3
!      print *,'IControlILine4 ',IControlILine4
!      print *,'IControlILine5 ',IControlILine5
!      print *,'IControlILine6 ',IControlILine6
!     
      do Item=1,NBdwnlay
        if( IControlILine1 == 0 ) SZTtradwnlay(1,Item) = 0.
        if( IControlILine2 == 0 ) SZTtradwnlay(2,Item) = 0.
        if( IControlILine3 == 0 ) SZTtradwnlay(3,Item) = 0.
        if( IControlILine4 == 0 ) SZTrotdwnlay(1,Item) = 0.
        if( IControlILine5 == 0 ) SZTrotdwnlay(2,Item) = 0.
        if( IControlILine6 == 0 ) SZTrotdwnlay(3,Item) = 0.
      enddo
!
      END SUBROUTINE ControlILine
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE ControlALine(IControlALine)
!
      IMPLICIT NONE
      INTEGER, INTENT(IN) :: IControlALine
!
      INTEGER :: IControlALineD
      INTEGER :: IControlALine1, IControlALine2, IControlALine3
      INTEGER :: IControlALine4, IControlALine5, IControlALine6
      INTEGER :: Item
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
      IControlALineD = IControlALine
      IControlALine1 = IControlALineD /100000
      IControlALineD = IControlALineD - IControlALine1 * 100000
      IControlALine2 = IControlALineD / 10000
      IControlALineD = IControlALineD - IControlALine2 *  10000
      IControlALine3 = IControlALineD /  1000
      IControlALineD = IControlALineD - IControlALine3 *   1000
      IControlALine4 = IControlALineD /   100
      IControlALineD = IControlALineD - IControlALine4 *    100
      IControlALine5 = IControlALineD /    10
      IControlALineD = IControlALineD - IControlALine5 *     10
      IControlALine6 = IControlALineD 
      
!      print *,'IControlALine1 ',IControlALine1
!      print *,'IControlALine2 ',IControlALine2
!      print *,'IControlALine3 ',IControlALine3
!      print *,'IControlALine4 ',IControlALine4
!      print *,'IControlALine5 ',IControlALine5
!      print *,'IControlALine6 ',IControlALine6
!     
      do Item=1,NBadjust
        if( IControlALine1 == 0 ) SZTtraadjust(1,Item) = 0.
        if( IControlALine2 == 0 ) SZTtraadjust(2,Item) = 0.
        if( IControlALine3 == 0 ) SZTtraadjust(3,Item) = 0.
        if( IControlALine4 == 0 ) SZTrotadjust(1,Item) = 0.
        if( IControlALine5 == 0 ) SZTrotadjust(2,Item) = 0.
        if( IControlALine6 == 0 ) SZTrotadjust(3,Item) = 0.
      enddo
!
      END SUBROUTINE ControlALine
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE TruncatePline
!
      IMPLICIT NONE
!
      INTEGER :: Item
      REAL(8) :: SignK
      REAL(8) :: ValueNew
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
      do Item=1,MTYP_F_Z
        ValueNew = Pos_Alfa_V(Item)
!
        SignK = 1.
        if (ValueNew.lt.0.) SignK = -1.
        ValueNew = SignK*ValueNew
        ValueNew = anint(ValueNew*10000.)
        ValueNew = ValueNew/10000.
!
        Pos_Alfa_V(Item) = SignK * ValueNew
!
        ValueNew = Pos_Beta_V(Item)

        SignK = 1.
        if (ValueNew.lt.0.) SignK = -1.
        ValueNew = SignK*ValueNew
        ValueNew = anint(ValueNew*10000.)
        ValueNew = ValueNew/10000.
!
        Pos_Beta_V(Item) = SignK * ValueNew
!
        ValueNew = Pos_Gama_V(Item)
!
        SignK = 1.
        if (ValueNew.lt.0.) SignK = -1.
        ValueNew = SignK*ValueNew
        ValueNew = anint(ValueNew*10000.)
        ValueNew = ValueNew/10000.
!
        Pos_Gama_V(Item) = SignK * ValueNew
      enddo
!
      END SUBROUTINE TruncatePline
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE TruncateAline
!
      IMPLICIT NONE
!
      REAL(8) :: SignK
      REAL(8) :: ValueNew
      INTEGER :: Irot
      INTEGER :: Item
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
      do Item=1,NBadjust
        do Irot=1,3
          SignK = 1.
          if (SZTrotadjust(Irot,Item).lt.0.) SignK = -1.
          ValueNew = SignK*SZTrotadjust(Irot,Item)
          ValueNew = anint(ValueNew*10000.)
          ValueNew = ValueNew/10000.
          SZTrotadjust(Irot,Item) = SignK * ValueNew
        enddo
      enddo
!
      END SUBROUTINE TruncateAline
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
       SUBROUTINE ReFormatAline(TrS,TrZ,TrT,RoS,RoZ,RoT)
!
       IMPLICIT NONE
       REAL(8), INTENT(INOUT) :: TrS,TrZ,TrT,RoS,RoZ,RoT
!
       CHARACTER(20) :: CHA20
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
       write(CHA20,1010) TrS
       read (CHA20,1010) TrS
! 
       write(CHA20,1010) TrZ
       read (CHA20,1010) TrZ
! 
       write(CHA20,1010) TrT
       read (CHA20,1010) TrT
! 
       write(CHA20,1020) RoS
       read (CHA20,1020) RoS
! 
       write(CHA20,1020) RoZ
       read (CHA20,1020) RoZ
! 
       write(CHA20,1020) RoT
       read (CHA20,1020) RoT
! 
1010   format(F20.3)
1020   format(F20.6)

       END SUBROUTINE ReFormatAline
!
