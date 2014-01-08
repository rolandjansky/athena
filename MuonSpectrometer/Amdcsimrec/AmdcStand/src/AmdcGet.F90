!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  SUBROUTINE AmdcGet3AngleFromRotMat( AS, AZ, AT , RotMat )
!
  implicit none 
  REAL(8), INTENT(OUT) :: AS, AZ, AT
  REAL(8), INTENT(IN)  :: RotMat(3,3)
!      
  REAL(8)  :: a13, a23, a33, a11, a12
!--------------------------------------------------------------------
!  
!--------------------------------------------------------------------
!
      a13 = RotMat(1,3)
      a23 = RotMat(2,3)
      a33 = RotMat(3,3)
      a11 = RotMat(1,1)
      a12 = RotMat(1,2)
!
      AZ = asin(a13)
!
      a12 =-a12 
      a23 =-a23
!
      AT = atan2(a12,a11)
      AS = atan2(a23,a33)
!
  END SUBROUTINE AmdcGet3AngleFromRotMat
!     
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  SUBROUTINE AmdcGetRotMat( AS, AZ, AT , RotMat )
!
  implicit none 
  REAL(8),  INTENT(IN)  :: AS, AZ, AT
  REAL(8),  INTENT(OUT) :: RotMat(3,3)
!
  REAL(8) :: Alfa(3) 
  REAL(8) :: RotS(3,3) ,RotZ(3,3) ,RotT(3,3) 
  REAL(8) :: RotB1(3,3)
!--------------------------------------------------------------------
!  
!--------------------------------------------------------------------
!
      Alfa(1) = AS
      Alfa(2) = AZ
      Alfa(3) = AT
!
      RotS(1,1:3) = ((/  1.d0,           0.d0,           0.d0 /))
      RotS(2,1:3) = ((/  0.d0,   cos(Alfa(1)), - sin(Alfa(1)) /))
      RotS(3,1:3) = ((/  0.d0,   sin(Alfa(1)),   cos(Alfa(1)) /))
!
      RotZ(1,1:3) = ((/   cos(Alfa(2)),  0.d0,   sin(Alfa(2)) /))
      RotZ(2,1:3) = ((/           0.d0,  1.d0,           0.d0 /))
      RotZ(3,1:3) = ((/ - sin(Alfa(2)),  0.d0,   cos(Alfa(2)) /))
!
      RotT(1,1:3) = ((/   cos(Alfa(3)), - sin(Alfa(3)),  0.d0 /))
      RotT(2,1:3) = ((/   sin(Alfa(3)),   cos(Alfa(3)),  0.d0 /))
      RotT(3,1:3) = ((/           0.d0,           0.d0,  1.d0 /))
!
      call AmDeProdMatAMatB(RotZ ,RotT ,RotB1)
      call AmDeProdMatAMatB(RotS ,RotB1,RotMat)
!
  END SUBROUTINE AmdcGetRotMat
!      
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  SUBROUTINE AmdcGetSumVec( W1, V1 , W2, V2 , R )
!
  implicit none
  REAL(8), INTENT(IN)  :: W1, V1(3)
  REAL(8), INTENT(IN)  :: W2, V2(3)
  REAL(8), INTENT(OUT) :: R(3)
!--------------------------------------------------------------------
!  
!--------------------------------------------------------------------
!
      R(1:3) = W1 * V1(1:3) + W2 * V2(1:3)
!      
  END SUBROUTINE AmdcGetSumVec
!     
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  SUBROUTINE AmdcGetVecByRot( RotMat, V_in, V_out )
!
  implicit none
  REAL(8), INTENT(IN)  :: RotMat(3,3)
  REAL(8), INTENT(IN)  :: V_in(3)
  REAL(8), INTENT(OUT) :: V_out(3)
!--------------------------------------------------------------------
!  
!--------------------------------------------------------------------
!      
      V_out(1:3)=RotMat(1:3,1)*V_in(1)  &
                +RotMat(1:3,2)*V_in(2)  &
                +RotMat(1:3,3)*V_in(3)
!
  END SUBROUTINE AmdcGetVecByRot     
!
