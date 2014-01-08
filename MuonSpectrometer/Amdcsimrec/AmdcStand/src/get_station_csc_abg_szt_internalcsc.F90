!
!> \author SaMuSoG
!
!--------------------------------------------------------------------
!
!   The general transformation formula between local vector l_in and global vector g_out is:
!        g_out = Rp [ Ra [ Ri * l_in + Ti ] + Ta ] + Tp
!       
!   where Rp, Ra and Ri are rotation matrices defined by the P, A and Ilines
!   and Tp, Ta and Ti are translation vectors defined by the same lines
!  
!   Mboy does not use local vector but instead considers global vector before  transformation.
!   A choice of hat is a global vector before  transformation depends is to be made.
!   Mboy does the same choice for all stations but for the CSC
!
!   For CSC one has:
!        g_in = Rg * l_in + Tp
!
!   and
!        G = Rg^-1 * g
!
!   where Rg is the rotation by the gamma angle
!
!   One gets:
!     G_out = Rg^-1 * Ro * Ri * G_in   
!           + Rg^-1 * [ To + Ro * Ti + Ro * [ 1 - Ri * Rg^-1 ] * Tp ]
!           
!   The routine  GET_STATION_CSC_ABG_SZT_TRANSFORM gives back        
!       Rcsc = Rg^-1 * Ro * Rg^-1 * Rg * G_in   
!   and Tcsc = Rg^-1 * [ To + Ro * [ 1 -Rg^-1 ] * Tp ]   
!          
!   This gives :
!     G_out = Rcsc * Ri * G_in +  Tcsc  + Rcsc * [ Ti + [ 1 - Ri ] * Rg^-1 * Tp ]
!           
!--------------------------------------------------------------------
      SUBROUTINE GET_STATION_CSC_ABG_SZT_INTERNALCSC &
      ( Jtyp,Jff,Jzz,Job, JLay, Iok, TransformDir,TransformBak  )
      IMPLICIT NONE
      INTEGER, INTENT(IN)  :: Jtyp,Jff,Jzz,Job,JLay
      INTEGER, INTENT(OUT) :: Iok
      REAL(8), INTENT(OUT) :: TransformDir(3,4),TransformBak(3,4)
      REAL(8) :: InterAliDir (3,4)
      REAL(8) :: InterAliBak (3,4)
!
      REAL(8) :: RCSC (3,3),TCSC (3)
      REAL(8) :: TP   (3)
      REAL(8) :: RI   (3,3),TI   (3)
      REAL(8) :: Gama0 
      REAL(8) :: Caa,Saa 
      REAL(8) :: RgaM1(3,3) 
      REAL(8) :: Rotat(3,3) 
      REAL(8) :: Trans(3) 
      REAL(8) :: SZT_a(3), SZT_B(3)
      INTEGER :: IokICsc
!
#include "AmdcStand/copipi.inc"
!
#include "AmdcStand/amdcsim_com.inc"
!
!-- Statement functions
#include "AmdcStand/amdcsim_stfunc.inc"
!
      CALL GET_STATION_CSC_ABG_SZT_TRANSFORM( Jtyp,Jff,Jzz,Job, Iok, TransformDir,TransformBak  )
      IF( Iok < 1 ) RETURN
      CALL GET_STATION_ABG_SZT_INTERNALCSC( Jtyp,Jff,Jzz,Job,JLay, IokICsc, InterAliDir,InterAliBak )
      IF( IokICsc < 1 ) RETURN
!
      IF( ICSCCH(Jtyp,Jff,Jzz) /= 1 ) RETURN
!  
!This is  Rcsc and Tcsc
      RCSC(1:3,1:3) = TransformDir(1:3,1:3)
      TCSC(1:3)     = TransformDir(1:3,  4)
!  
!This is  Ri and Ti
      RI  (1:3,1:3) = InterAliDir (1:3,1:3)
      TI  (1:3    ) = InterAliDir (1:3,  4)
!
!This is  Rcsc * Ri
      CALL MULT3M3M( RI, RCSC, Rotat )
!
!This is Tp
      TP(1) = Pos_S(Jtyp,Jff,Jzz) 
      TP(2) = Pos_R(JTYP,JFF,JZZ) 
      TP(3) = Pos_Z(JTYP,JFF,JZZ)
!
!This is Rg^-1 
      Gama0 = Pos_Gama(Jtyp,Jff,Jzz) * PIS180
      IF( Jzz < 0 ) Gama0 = -Gama0
      Caa = COS(Gama0)
      Saa = SIN(Gama0)
      RgaM1( 1 ,1:3) = (/ 1.D0, 0.D0, 0.D0 /)
      RgaM1( 2 ,1:3) = (/ 0.D0,  Caa,  Saa /)
      RgaM1( 3 ,1:3) = (/ 0.D0, -Saa,  Caa /)
!
!This is Rg^-1 * Tp
     SZT_A(1:3) =                    &  
                 + RgaM1(1:3,1) * TP(1) &
                 + RgaM1(1:3,2) * TP(2) &
                 + RgaM1(1:3,3) * TP(3)
!This is [ 1 - Ri ] * Rg^-1 * Tp
      SZT_B(1:3) = SZT_A(1:3)         &  
                 - RI(1:3,1) * SZT_A(1) &
                 - RI(1:3,2) * SZT_A(2) &
                 - RI(1:3,3) * SZT_A(3)
!This is Tcsc  + Rcsc * [ Ti + [ 1 - Ri ] * Rg^-1 * Tp ]
      Trans(1:3) = TCSC(1:3)                          &
                 + RCSC(1:3,1) * ( TI(1) + SZT_B(1) ) &
                 + RCSC(1:3,2) * ( TI(2) + SZT_B(2) ) &
                 + RCSC(1:3,3) * ( TI(3) + SZT_B(3) )
!
      TransformDir(1:3,1:3) = Rotat(1:3,1:3)
      TransformDir(1:3, 4 ) = Trans(1:3) 
!
      Rotat = TRANSPOSE(Rotat)                 ! inversion
      TransformBak(1:3,1:3) = Rotat(1:3,1:3)               
      TransformBak(1:3, 4 ) =                         &
                            - Rotat(1:3,1) * Trans(1) &
                            - Rotat(1:3,2) * Trans(2) &
                            - Rotat(1:3,3) * Trans(3)
!
      END SUBROUTINE GET_STATION_CSC_ABG_SZT_INTERNALCSC
!
!--------------------------------------------------------------------
!         
!--------------------------------------------------------------------
      SUBROUTINE GET_STATION_ABG_SZT_INTERNALCSC &
      (  Jtyp,Jff,Jzz,Job, JLay, Iok, TransformDir,TransformBak  )
      IMPLICIT NONE
      INTEGER, INTENT(IN)  :: Jtyp,Jff,Jzz,Job,JLay
      INTEGER, INTENT(OUT) :: Iok
      REAL(8), INTENT(OUT) :: TransformDir(3,4),TransformBak(3,4)
      REAL(8) :: RotaSZT(3,3)
      REAL(8) :: Translation(3),Rotation(3,3)
      REAL(8) :: RotS(3,3),RotZ(3,3),RotT(3,3),RotX(3,3)
      REAL(8) :: Alfa(3)
      INTEGER :: IokSZT
      REAL(8) :: COSA1,SINA1
      REAL(8) :: COSA2,SINA2
      REAL(8) :: COSA3,SINA3
      INTEGER :: FindObjectICSC
!
#include "AmdcStand/copipi.inc"
!
#include "AmdcStand/amdcsim_com.inc"
!
!-- Statement functions
#include "AmdcStand/amdcsim_stfunc.inc"
!
      Iok = 0
      TransformDir( 1 ,1:4) = (/ 1.D0, 0.D0, 0.D0, 0.D0 /)
      TransformDir( 2 ,1:4) = (/ 0.D0, 1.D0, 0.D0, 0.D0 /)
      TransformDir( 3 ,1:4) = (/ 0.D0, 0.D0, 1.D0, 0.D0 /)
      TransformBak(1:3,1:4) = TransformDir(1:3,1:4)
!
      IokSZT = FindObjectICSC(Jtyp,Jff,Jzz,Job,Jlay, NBdwnlay, JTYPFZOLdwnlay) 
      IF( IokSZT < 1 ) RETURN
      Iok = MAX( 1 , IokSZT )
!
      IF( IokSZT > 0 ) THEN
!
            IF( Jzz > 0 ) THEN
               Translation( 1 ) = -SZTtradwnlayShifted( 1 ,IokSZT)
               Translation(2:3) =  SZTtradwnlayShifted(2:3,IokSZT)
               Alfa( 1 )        =  SZTrotdwnlay( 1 ,IokSZT)
               Alfa(2:3)        = -SZTrotdwnlay(2:3,IokSZT)
            ELSE
               Translation(1:2) =  SZTtradwnlayShifted(1:2,IokSZT)
               Translation( 3 ) = -SZTtradwnlayShifted( 3 ,IokSZT)
               Alfa(1:2)        = -SZTrotdwnlay(1:2,IokSZT)
               Alfa( 3 )        =  SZTrotdwnlay( 3 ,IokSZT)
            ENDIF
!            
         COSA1 = COS(Alfa(1))
         SINA1 = SIN(Alfa(1))
         COSA2 = COS(Alfa(2))
         SINA2 = SIN(Alfa(2))
         COSA3 = COS(Alfa(3))
         SINA3 = SIN(Alfa(3))
!         
         RotS(1:3,1:3) =   0.D0
         RotZ(1:3,1:3) =   0.D0
         RotT(1:3,1:3) =   0.D0
!         
         RotS( 1 , 1 ) =   1.D0
         RotS( 2 , 2 ) =   COSA1
         RotS( 3 , 2 ) =   SINA1
         RotS( 2 , 3 ) = - SINA1
         RotS( 3 , 3 ) =   COSA1
 !        
         RotZ( 2 , 2 ) =   1.D0
         RotZ( 1 , 1 ) =   COSA2
         RotZ( 3 , 1 ) = - SINA2
         RotZ( 1 , 3 ) =   SINA2
         RotZ( 3 , 3 ) =   COSA2
!         
         RotT( 3 , 3 ) =   1.D0
         RotT( 1 , 1 ) =   COSA3
         RotT( 2 , 1 ) =   SINA3
         RotT( 1 , 2 ) = - SINA3
         RotT( 2 , 2 ) =   COSA3
!         
         CALL MULT3M3M( RotT,RotZ, RotX )
         CALL MULT3M3M( RotX,RotS, RotaSZT )
!         
      ELSE
         Translation(1:3)     = 0.D0
         RotaSZT( 1 ,1:3) = (/ 1.D0, 0.D0, 0.D0 /)
         RotaSZT( 2 ,1:3) = (/ 0.D0, 1.D0, 0.D0 /)
         RotaSZT( 3 ,1:3) = (/ 0.D0, 0.D0, 1.D0 /)
      ENDIF
!
      Rotation(1:3,1:3) = RotaSZT(1:3,1:3)
!
      TransformDir(1:3,1:3) = Rotation(1:3,1:3)
      TransformDir(1:3, 4 ) = Translation(1:3) 
!
      Rotation = TRANSPOSE(Rotation)                 ! inversion
      TransformBak(1:3,1:3) = Rotation(1:3,1:3)               
      TransformBak(1:3, 4 ) =                                 &
                            - Rotation(1:3,1)* Translation(1) &
                            - Rotation(1:3,2)* Translation(2) &
                            - Rotation(1:3,3)* Translation(3)
!
      END SUBROUTINE GET_STATION_ABG_SZT_INTERNALCSC
!
