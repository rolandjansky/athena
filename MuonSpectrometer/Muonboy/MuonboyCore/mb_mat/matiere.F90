!
!> Load the matter used by muonboy
!
 SUBROUTINE MATIERE(IFLAMA, IaddMat)
 USE M_MB_Control
 USE M_MB_COMATI
 USE M_MB_Mat_AGDD2GeoSwitches
 USE M_MB_Mat_TagInfo
 USE M_MB_General_Volumes
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IFLAMA, IaddMat
 INTEGER :: I, Item, LenAGDD
 INTEGER :: IFLAM1, IFLAM2, IFLAM3, IFLAM4, IFLAM5, IFLAM6, IFLAM7, IFLAM8, IFLAM9
 INTEGER :: J, L, NbVol, NDeadM
 INTEGER, PARAMETER :: NDeadMatter=10
 CHARACTER(len=22) :: DeadMatter(NDeadMatter) = (/ 'MBWH_BigWheels        '    &
                                                  ,'SADL_CalorimeterSaddle'    &
                                                  ,'TBWH_BigWheels        '    &
                                                  ,'TGC3_BigWheels        '    &
                                                  ,'TGC1_BigWheels        '    &
                                                  ,'HFTruckRail           '    &
                                                  ,'RailAssembly          '    &
                                                  ,'MBAP_AccessPlatform   '    &
                                                  ,'servicesAtZ0          '    &
                                                  ,'NewSmallWheel         ' /)
 CHARACTER(len=16) :: AmdcName
!
#include "AmdcStand/comama.inc"
!
#include "AmdcStand/comamu.inc"
!
      IF( MBPri >= 1 ) THEN
!TagInfo data
        if ( NewTagInfoMat == -1 ) then
          print *,' TagInfo data NOT furnished'
        else
          print *,' TagInfo data furnished'
          if ( NewTagInfoMat == 0 ) then
            print *,'->But it has been already used'
          else
            print *,'->And it has NOT been already used'
            print *,'->The nber of items is ',NberTagInfo
            DO Item=1,NberTagInfo
              print *,'-> Item ',Item 
              print *,'->    A ',ChaTagInfoA(Item)
              print *,'->    B ',ChaTagInfoB(Item)
            ENDDO
!         Do what ever is needed and tell that the data were used 
            NewTagInfoMat = 0
          endif
        endif
!
!AGDD2GeoSwitches data
        if ( NewAGDD2GeoSwitches.eq.-1) then
          print *,' AGDD2GeoSwitches data NOT furnished'
        else
          print *,' AGDD2GeoSwitches data furnished'
          if ( NewAGDD2GeoSwitches.eq.0) then
            print *,'->But it has been already used'
          else
            print *,'->And it has NOT been already used'
            print *,'->The nber of items is ',NberAGDD2GeoSwitches
            DO Item=1,NberAGDD2GeoSwitches
              print *,'-> Item ',Item,'  ->  ',Trim(ChaAGDD2GeoSwitches(Item))
            ENDDO
!         Do what ever is needed and tell that the data were used 
            NewAGDD2GeoSwitches = 0
          endif
        endif
      ENDIF
!- Initialize all Comati arrays
      CALL Init_MB_Comati
!
      IF( IFLAMA.LE.0 ) RETURN
      I      = IFLAMA
      J      = I / 10
      IFLAM1 = I - J*10
      I      = J / 10
      IFLAM2 = J - I*10
      J      = I / 10
      IFLAM3 = I - J*10
      I      = J / 10
      IFLAM4 = J - I*10
      J      = I / 10
      IFLAM5 = I - J*10
      I      = J / 10
      IFLAM6 = J - I*10
      J      = I / 10
      IFLAM7 = I - J*10
      I      = J / 10
      IFLAM8 = J - I*10
      J      = I / 10
      IFLAM9 = I - J*10
!
      AGDD2GeoAlreadyUsed(1:NberAGDD2GeoSwitches) = .false.
!
!=====================  ATLAS  MATTER  ==================================
!
!@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
      IF( X0MAMA(77) > 999.D0 ) THEN
        call MboyEloss
        X0MAMA(77) = 100.D0
      ENDIF
!@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
!
      CALL FGetAGDDcharNumber(LenAGDD)
!
!-- Barrel toroid !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IF( IFLAM1 > 0 ) THEN
        NbVol = 0
        IF( LenAGDD > 0 ) THEN
          CALL Get_AGDD_Matter('BAR_Toroid',NbVol)
          CALL RemoveMatAGDD2GeoSwitches( 'BAR_Toroid' )
        ENDIF
        IF( NbVol <= 0 ) CALL AmdcBarrelToroid
      ENDIF
!
!
!-- End-cap toroids !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IF( IFLAM2 > 0 ) THEN
        NbVol = 0
        IF( LenAGDD > 0 ) THEN
          CALL Get_AGDD_Matter('ECT_Toroids',NbVol)
          CALL RemoveMatAGDD2GeoSwitches( 'ECT_Toroids' )
        ENDIF
        IF( NbVol <= 0 ) CALL AmdcEndcapToroid
      ENDIF
!
!
!-- Feet !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IF( IFLAM3 > 0 ) THEN
        NbVol = 0
        IF( LenAGDD > 0 ) THEN
          CALL Get_AGDD_Matter('Feet', NbVol)
          CALL RemoveMatAGDD2GeoSwitches( 'Feet' )
        ENDIF
        IF( NbVol <= 0 ) CALL AmdcFeet
      ENDIF
!
!
!-- Schielding !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IF( IFLAM4 > 0 ) THEN
        NbVol = 0
        IF( LenAGDD > 0 ) THEN
          CALL Get_AGDD_Matter('JDSH_Shield',NbVol)
          CALL RemoveMatAGDD2GeoSwitches( 'JDSH_Shield' )
        ENDIF
        IF( NbVol <= 0 ) CALL AmdcShield
      ENDIF
!
!
!-- Had. "tile" calorimeters !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IF( IFLAM5 > 0 ) CALL AmdcTile
!
!
!-- Liq. argon calorimeters (+ solenoid) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IF( IFLAM6 > 0 ) CALL AmdcLarg
!
!
!-- Inner Detector !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IF( IFLAM7 > 0 ) CALL AmdcInDet
!
!
!-- Alignement tubes and bars and boxes and ... in all Phi sectors but only for Z+
      IF( IFLAM8 > 0 ) THEN
        IF( Nb_Align_Tub+Nb_Align_Bar > 0 ) CALL MATALIGNEMENT
      ENDIF
!
!
!-- RPCs dead matter !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IF( IFLAM9 > 0 ) CALL MATRPC
!
!
!----------------------------------------------------------------------
!
      IF( LenAGDD > 0 ) THEN
! additionnal matter  
        IF( IaddMat > 0 ) THEN
          IF( IsSimu == 0 ) THEN   ! use all available dead matter
            NDeadM = NDeadMatter
            CALL GetAmdcName(AmdcName)
            IF( AmdcName(1:7) == 'R.03 01' .OR. AmdcName(1:7) == 'R.03 02' ) THEN
              NDeadM = NDeadMatter - 1  ! problem with service at z=0
            ENDIF
            DO L=1,NDeadM
              CALL Get_AGDD_Matter(TRIM(DeadMatter(L)),NbVol)
              CALL RemoveMatAGDD2GeoSwitches(DeadMatter(L))
            ENDDO
          ENDIF
! now add what is in AGDD2GEO (avoiding double counting)
          DO L=1,NberAGDD2GeoSwitches
            IF( AGDD2GeoAlreadyUsed(L) ) CYCLE
            CALL Get_AGDD_Matter(TRIM(ChaAGDD2GeoSwitches(L)),NbVol)
          ENDDO
        ENDIF
      ENDIF
!
      IF( NVMATI > KVOLU ) THEN
        PRINT 1000
1000    FORMAT(//' In MATIERE : Parameter KVOLU is not big enough  =====>  STOP !')
        STOP
      ENDIF
!========================================================================
!
!-- Compute all useful parameters of matter volumes
      CALL INIMAT
!
      WRITE(*,2000) NVMATI,NPMATI,NEMATI,NAMATI,NTMATI,NMANAM
      WRITE(*,2001) KVOLU,KPLAN,KEDGE,KADGE,KtransMati,KMANAM
      WRITE(*,2002) NbNamVol,SIZE(CharacNamVol)
      DO I=1,4
        WRITE(*,2003) NVPOSMATI(I),NVNEGMATI(I),NVCENMATI(I),KVOLUmat
      ENDDO
2000  FORMAT(/,' ===> At the end of subroutine MATIERE :   ',       &
               'NVMATI,NPMATI,NEMATI,NAMATI,NTMATI,NMANAM =',6I7)
2001  FORMAT(  '                                           ',       &
               '   while reserved sizes are                ',6I7)
2002  FORMAT(  '                                           ',       &
               '  and NbNamVol used/reserved               ',2I7,/)
2003  FORMAT(/,' NVPOSMATI,NVNEGMATI,NVCENMATI reserved    ',4I7,/)
!
 END SUBROUTINE MATIERE
!
