!
!> Create the generic volume coresponding to the XML "solid" object Iobj
!> \author SaMuSoG
!
 SUBROUTINE CREAT_XML_SOLI(Iobj)
 USE M_MB_Xml
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Iobj
 CHARACTER(80) :: CAR80, NameElem, NameVolu
 CHARACTER(80) :: ATAB(5)
 INTEGER :: ITAB(10), NXYDIM(200)
 REAL(8) :: ADIM(10), RTAB(80)
 REAL(8) :: XDIM(1600), YDIM(1600), ZDIM(200)
 REAL(8) :: CM, DEG, RR, AA, DZ, Radius, Phi0, dPhi
 INTEGER :: Icolor, ISHAPE, NXDIM,NYDIM,NZDIM, NADIM, NRDIM1,NRDIM2, IELE, NCAR
 INTEGER :: IATT, IA,II,IR, Io2_Beg,Io2_End,Io2, NXYTOT, Io3_Beg,Io3_End,Io3 
 INTEGER :: IELE2,IELE3, I12, NR, I4, NXDIM3, I, NbPhi, IZ
!
    Icolor = Icol_info(Info_xob(Iobj))
    IF( Icolor <= 0 ) Icolor = 1
    ISHAPE = 0
    NXDIM  = 0
    NYDIM  = 0
    NZDIM  = 0
    NADIM  = 0
    NRDIM1 = 0
    NRDIM2 = 0
    NameVolu = 'Undefined'
    CM     = 0.1D0
    DEG    = 1.0D0
!
    IELE = Iele_xob(Iobj)
    IF(IELE < Iel0_xml_solid .OR. IELE > Iel1_xml_solid) GO TO 100
!
!
    CALL GIV_NAM_XML_ELE(IELE, NameElem,NCAR)
    IF(     NameElem == 'box'       ) THEN
      ISHAPE = 11
    ELSEIF( NameElem == 'trd'       ) THEN
      ISHAPE = 12
    ELSEIF( NameElem == 'tubs'      ) THEN
      ISHAPE = 13
    ELSEIF( NameElem == 'cons'      ) THEN
      ISHAPE = 14
    ELSEIF( NameElem == 'pcon'      ) THEN
      ISHAPE = 15
    ELSEIF( NameElem == 'trtmodule' ) THEN
      ISHAPE = 16
    ELSEIF( NameElem == 'gvxy'      ) THEN
      ISHAPE = 17
    ELSEIF( NameElem == 'gvxysx'    ) THEN
      ISHAPE = 18
    ELSEIF( NameElem == 'gvxysxy'   ) THEN
      ISHAPE = 19
    ELSEIF( NameElem == 'snake'     ) THEN
      ISHAPE = 20
    ELSEIF( NameElem == 'elipso'    ) THEN
      ISHAPE = 21
    ELSEIF( NameElem == 'gcylrz'    ) THEN
      ISHAPE = 22
    ELSEIF( NameElem == 'gslice'    ) THEN
      ISHAPE = 23
    ELSEIF( NameElem == 'elcyl'     ) THEN
      ISHAPE = 24
    ENDIF
!
!
    DO IATT=1,Nb_xml_att(IELE)
      CALL GIV_NAM_XML_ATT(IELE,IATT, CAR80,NCAR)
      IF( CAR80 == 'name' ) THEN
        CALL GIV_VAL_XML_ATT(Iobj,IATT, IA,II,IR,ATAB,ITAB,RTAB)
        NameVolu = ATAB(1)
      ELSEIF( CAR80 == 'unit_length' ) THEN
        CALL GIV_VAL_XML_ATT(Iobj,IATT, IA,II,IR,ATAB,ITAB,RTAB)
        IF( ATAB(1) == 'default' ) ATAB(1) = Default_unit_length
        CM = 0.1D0
!****** IF( ATAB(1) == 'mm' ) CM =   0.1D0
        IF( ATAB(1) == 'cm' ) CM =   1.0D0
        IF( ATAB(1) == 'm'  ) CM = 100.0D0
      ELSEIF( CAR80 == 'unit_angle' ) THEN
        CALL GIV_VAL_XML_ATT(Iobj,IATT, IA,II,IR,ATAB,ITAB,RTAB)
        IF( ATAB(1) == 'default' ) ATAB(1) = Default_unit_angle
        DEG = 1.0D0
!****** IF( ATAB(1) == 'deg'    ) DEG = 1.0D0
!****** IF( ATAB(1) == 'degree' ) DEG = 1.0D0
        IF( ATAB(1) == 'mrad'   ) DEG = 0.05729577951D0
        IF( ATAB(1) == 'radian' ) DEG = 57.29577951D0
      ELSEIF( ISHAPE == 0 .AND. CAR80 == 'shape' ) THEN
        CALL GIV_VAL_XML_ATT(Iobj,IATT, IA,II,IR,ATAB,ITAB,RTAB)
        IF( ATAB(1) == 'box'       ) ISHAPE = 1
        IF( ATAB(1) == 'trd'       ) ISHAPE = 2
        IF( ATAB(1) == 'tubs'      ) ISHAPE = 3
        IF( ATAB(1) == 'cons'      ) ISHAPE = 4
        IF( ATAB(1) == 'pcon'      ) ISHAPE = 5
        IF( ATAB(1) == 'trtmodule' ) ISHAPE = 6
      ELSEIF( ISHAPE <= 10 .AND. CAR80 == 'dim'          .OR.    &
              ISHAPE == 11 .AND. CAR80 == 'X_Y_Z'        .OR.    &
              ISHAPE == 12 .AND. CAR80 == 'Xmp_Ymp_Z'    .OR.    &
              ISHAPE == 13 .AND. CAR80 == 'Rio_Z'        .OR.    &
              ISHAPE == 14 .AND. CAR80 == 'Rio1_Rio2_Z'  .OR.    &
              ISHAPE == 16 .AND. CAR80 == 'Bh1_B2_Z'     .OR.    &
              ISHAPE == 17 .AND. CAR80 == 'X'            .OR.    &
              ISHAPE == 18 .AND. CAR80 == 'X'            .OR.    &
              ISHAPE == 19 .AND. CAR80 == 'X'            .OR.    &
              ISHAPE == 20 .AND. CAR80 == 'radius'       .OR.    &
              ISHAPE == 21 .AND. CAR80 == 'radiusXYZ'    .OR.    &
              ISHAPE == 24 .AND. CAR80 == 'RXio_RYio_Z'  ) THEN
        CALL GIV_VAL_XML_ATT(Iobj,IATT, IA,II,IR,ATAB,ITAB,RTAB)
        NXDIM      = IR
        XDIM(1:IR) = RTAB(1:IR)
      ELSEIF( ISHAPE <= 19 .AND. CAR80 == 'Y' ) THEN
        CALL GIV_VAL_XML_ATT(Iobj,IATT, IA,II,IR,ATAB,ITAB,RTAB)
        NYDIM      = IR
        YDIM(1:IR) = RTAB(1:IR)
      ELSEIF( ISHAPE <= 19 .AND. CAR80 == 'dZ' ) THEN
        CALL GIV_VAL_XML_ATT(Iobj,IATT, IA,II,IR,ATAB,ITAB,RTAB)
        NZDIM      = IR
        ZDIM(1:IR) = RTAB(1:IR)
      ELSEIF( ISHAPE == 13 .OR. ISHAPE == 14 .OR. ISHAPE == 15 .OR.    &
              ISHAPE == 20 .OR. ISHAPE == 21 .OR. ISHAPE == 24 ) THEN
        IF( CAR80 == 'profile' .OR. CAR80 == 'Phi0' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT, IA,II,IR,ATAB,ITAB,RTAB)
          NADIM      = IR
          ADIM(1:IR) = RTAB(1:IR)
        ELSEIF( CAR80 == 'nbPhi' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT, IA,II,IR,ATAB,ITAB,RTAB)
          NRDIM1     = ITAB(1)
          IF( NRDIM2 < 1 ) NRDIM2 = NRDIM1
        ENDIF
      ELSEIF( ISHAPE == 12 ) THEN
        IF( CAR80 == 'inclination') THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT, IA,II,IR,ATAB,ITAB,RTAB)
          NADIM      = IR
          ADIM(1:IR) = RTAB(1:IR)
        ELSEIF( CAR80 == 'side_tilts') THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT, IA,II,IR,ATAB,ITAB,RTAB)
          NYDIM      = IR
          YDIM(1:IR) = RTAB(1:IR)
        ENDIF
      ELSEIF( ISHAPE == 22 ) THEN
        IF(     CAR80 == 'Phi0' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT, IA,II,IR,ATAB,ITAB,RTAB)
          IF( IR /= 1 ) CYCLE
          NZDIM   = NZDIM + 1
          ZDIM(1) = RTAB(1)
        ELSEIF( CAR80 == 'dPhi' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT, IA,II,IR,ATAB,ITAB,RTAB)
          IF( IR /= 1 ) CYCLE
          NZDIM   = NZDIM + 1
          ZDIM(2) = RTAB(1)
        ELSEIF( CAR80 == 'nbPhi' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT, IA,II,IR,ATAB,ITAB,RTAB)
          NRDIM1  = ITAB(1)
        ENDIF
      ENDIF
    ENDDO
!
!
    IF( ISHAPE == 12 ) THEN
!>>  <trd>   elements : 'side_tilts' angles are added to the 'inclination' angles
      ADIM(NADIM+1:NADIM+NYDIM) = YDIM(1:NYDIM)
      NADIM = NADIM + NYDIM
    ELSEIF( ISHAPE == 15 .OR. ISHAPE == 20 ) THEN
!>>  <polyplane>   elements embedded in  <pcon>               or
!>> <snake_point>  elements embedded in  <snake>
      Io2_Beg = MAX( Iob_xob_inf0(Iobj) , 1 )  ! fix for bug in the Pacific_Sierra F90 Linux compiler (?)
      Io2_End =      Iob_xob_inf1(Iobj)        ! fix for bug in the Pacific_Sierra F90 Linux compiler (?)
      DO Io2=Io2_Beg,Io2_End
        IELE2 = Iele_xob(Io2)
        CALL GIV_NAM_XML_ELE(IELE2,CAR80,NCAR)
        IF( (ISHAPE == 15 .AND. CAR80 /= 'polyplane'  ) .OR.    &
            (ISHAPE == 20 .AND. CAR80 /= 'snake_point') ) THEN
          PRINT 7770,TRIM(CAR80),TRIM(NameElem)
7770      FORMAT(/' An XML element of type         : ',A,    &
                 /' is embedded in another of type : ',A,    &
                 /' ===============================> One STOPs !!!')
          STOP
        ENDIF
        IF( ISHAPE == 15 ) THEN
          CALL GIV_VAL_XML_ATT(Io2,'Rio_Z',IA,II,IR,ATAB,ITAB,RTAB)
          XDIM(NXDIM+1:NXDIM+IR) = RTAB(1:IR)
          NXDIM = MIN( NXDIM+IR , 1594 )
        ELSE
          CALL GIV_VAL_XML_ATT(Io2,'X_Y_Z',IA,II,IR,ATAB,ITAB,RTAB)
          XDIM(NXDIM+1:NXDIM+IR) = RTAB(1:IR)
          NXDIM = MIN( NXDIM+IR , 1594 )
        ENDIF
      ENDDO
    ELSEIF( ISHAPE == 17 .OR. ISHAPE == 18 .OR. ISHAPE == 19 ) THEN
!>>  <gvxy_point>  elements embedded in  <gvxy>, <gvxysx> or <gvxysxy>
      IF( NXDIM < 2 ) NXDIM = 0
      IF( NYDIM < 2 ) NYDIM = 0
      Io2_Beg = MAX( Iob_xob_inf0(Iobj) , 1 )
      Io2_End =      Iob_xob_inf1(Iobj)
      DO Io2=Io2_Beg,Io2_End
        IELE2 = Iele_xob(Io2)
        CALL GIV_NAM_XML_ELE(IELE2,CAR80,NCAR)
        IF( CAR80 /= 'gvxy_point' ) THEN
          PRINT 7770,TRIM(CAR80),TRIM(NameElem)
          STOP
        ENDIF
        CALL GIV_VAL_XML_ATT(Io2,'X_Y',IA,II,IR,ATAB,ITAB,RTAB)
        IF( IR /= 2 ) CYCLE   
        NXDIM = MIN( NXDIM+1 , 396 ) 
        XDIM(NXDIM) = RTAB(1)
        NYDIM = MIN( NYDIM+1 , 396 )
        YDIM(NYDIM) = RTAB(2)
      ENDDO
    ELSEIF( ISHAPE == 22 ) THEN
!>>  <gcylrz_point>  elements embedded in  <gcylrz>
      NXDIM = 0
      NYDIM = 0
      Io2_Beg = MAX( Iob_xob_inf0(Iobj) , 1 )
      Io2_End =      Iob_xob_inf1(Iobj)
      DO Io2=Io2_Beg,Io2_End
        IELE2 = Iele_xob(Io2)
        CALL GIV_NAM_XML_ELE(IELE2,CAR80,NCAR)
        IF( CAR80 /= 'gcylrz_point' ) THEN
          PRINT 7770,TRIM(CAR80),TRIM(NameElem)
          STOP
        ENDIF
        CALL GIV_VAL_XML_ATT(Io2,'R_Z',IA,II,IR,ATAB,ITAB,RTAB)
        IF( IR /= 2 ) CYCLE
        NXDIM = MIN( NXDIM+1 , 1594 )
        XDIM(NXDIM) = RTAB(1)
        NYDIM = MIN( NYDIM+1 , 1594 )
        YDIM(NYDIM) = RTAB(2)
      ENDDO
    ELSEIF( ISHAPE == 23 ) THEN
!>>  <gslice_point>  elements embedded in  <gslice_new>  elements embedded in  <gslice>
      NXDIM  = 0
      NYDIM  = 0
      NZDIM  = 0
      NXYTOT = 0
      Io2_Beg = MAX( Iob_xob_inf0(Iobj) , 1 )
      Io2_End =      Iob_xob_inf1(Iobj)
      DO Io2=Io2_Beg,Io2_End
        IELE2 = Iele_xob(Io2)
        CALL GIV_NAM_XML_ELE(IELE2,CAR80,NCAR)
        IF( CAR80 == 'gslice_point' ) CYCLE
        IF( CAR80 /= 'gslice_new' ) THEN
          PRINT 7770,TRIM(CAR80),TRIM(NameElem)
          STOP
        ENDIF
        CALL GIV_VAL_XML_ATT(Io2,'Z',IA,II,IR,ATAB,ITAB,RTAB)
        IF( IR /= 1 ) CYCLE
        NZDIM = MIN( NZDIM+1 , 200 )
        ZDIM(NZDIM) = RTAB(1)
        Io3_Beg = MAX( Iob_xob_inf0(Io2) , 1 )
        Io3_End =      Iob_xob_inf1(Io2)
        DO Io3=Io3_Beg,Io3_End
          IELE3 = Iele_xob(Io3)
          CALL GIV_NAM_XML_ELE(IELE3,CAR80,NCAR)
          IF( CAR80 /= 'gslice_point' ) THEN
            NameElem = 'gslice_new'
            PRINT 7770,TRIM(CAR80),TRIM(NameElem)
            STOP
          ENDIF
          CALL GIV_VAL_XML_ATT(Io3,'X_Y',IA,II,IR,ATAB,ITAB,RTAB)
          IF( IR /= 2 ) CYCLE
          NXYTOT = MIN( NXYTOT+1, 1594 )
          XDIM(NXYTOT) = RTAB(1)
          YDIM(NXYTOT) = RTAB(2)
        ENDDO
        NXYDIM(NZDIM) = NXYTOT
      ENDDO
    ENDIF
!
!
    IF( ISHAPE >= 11 ) THEN
      ISHAPE = ISHAPE - 10
      XDIM(NXDIM+1:NXDIM+NADIM) = ADIM(1:NADIM)
      NXDIM  = NXDIM + NADIM
    ENDIF
!
!
100 IF( ISHAPE == 1 ) THEN
!>> Box !!! ------------------------------------------------------------
      IF( NXDIM /= 3 ) THEN
        PRINT 7771,NXDIM
7771    FORMAT(/' There exists one volume of shape BOX without',    &
                ' properly defined 3 dimentions (',I4,')  =====>  STOP !')
        STOP
      ENDIF
      XDIM(1:3) = XDIM(1:3) * CM
      CALL DEF_BOX_DXDYDZ( NameVolu, Icolor, XDIM )
    ELSEIF( ISHAPE == 2 ) THEN
!>> Trd !!! ------------------------------------------------------------
      IF( NXDIM /= 9 ) THEN
        PRINT 7772,NXDIM
7772    FORMAT(/' There exists one volume of shape TRD without',     &
                ' properly defined 5 to 9 dimentions (',I4,')  =====>  STOP !')
        STOP
      ENDIF
      XDIM(1:5) = XDIM(1:5) * CM
      XDIM(6:9) = XDIM(6:9) * DEG
      CALL DEF_BOX_DX12DY12DZA1234( NameVolu, Icolor, XDIM )
    ELSEIF( ISHAPE == 3 ) THEN
!>> Tubs !!! -----------------------------------------------------------
      IF( NXDIM /= 5 ) THEN
        PRINT 7773,NXDIM
7773    FORMAT(/' There exists one volume of shape TUBS without',    &
                ' properly defined 3 (or 5) dimentions (',I4,')  =====>  STOP !')
        STOP
      ENDIF
      XDIM(1:3) = XDIM(1:3) * CM
      XDIM(4:5) = XDIM(4:5) * DEG
      XDIM( 5 ) = XDIM( 4 ) + XDIM( 5 )
      IF( NRDIM1 <= 2 ) THEN
        RR = MAX( ABS(XDIM(1)) , ABS(XDIM(2)) )
        I12= 0.8D0 * SQRT(RR) + 1.D0
        NR = 12 * MIN( 4 , I12 )
        AA = (XDIM(5)-XDIM(4)) / 360.D0
        IF( AA < 0.95D0 ) THEN
          NR = AA*NR + 1
          NR = MAX( 3 , NR )
        ENDIF
      ELSE
        NR = NRDIM1 
      ENDIF
      CALL DEF_TUBS_R1R2DZA1A2( NameVolu, Icolor, XDIM, NR )
    ELSEIF( ISHAPE == 4 ) THEN
!>> Cons !!! -----------------------------------------------------------
      IF( NXDIM /= 7 ) THEN
        PRINT 7774,NXDIM
7774    FORMAT(/' There exists one volume of shape CONS without',    &
                ' properly defined 5 (or 7) dimentions (',I4,')  =====>  STOP !')
        STOP
      ENDIF
      XDIM(1:5) = XDIM(1:5) * CM
      XDIM(6:7) = XDIM(6:7) * DEG
      XDIM( 7 ) = XDIM( 6 ) + XDIM( 7 )
      IF( NRDIM1 <= 2 ) THEN
        RR = MAX( ABS(XDIM(2)) , ABS(XDIM(4)) )
        I4 = 2.4D0 * SQRT(RR) + 1.D0
        NR = 4 * MIN( 12 , MAX( 2 , I4 ) )
        AA = (XDIM(7)-XDIM(6)) / 360.D0
        IF( AA < 0.95D0 ) THEN
          NR = AA*NR + 1
          NR = MAX( 3 , NR )
        ENDIF
      ELSE
        NR = NRDIM1
      ENDIF
      CALL DEF_CONS_RRRRDZA1A2( NameVolu, Icolor, XDIM, NR )
    ELSEIF( ISHAPE == 5 ) THEN
!>> Pcon !!! -----------------------------------------------------------
      NXDIM3 = (NXDIM-2) / 3
      IF( NXDIM < 8 .OR. NXDIM /= 3*NXDIM3+2 ) THEN
        PRINT 7775,NXDIM
7775    FORMAT(/' There exists one volume of shape PCON without',    &
                ' properly defined  3xN+2  dimentions (',I4,')  =====>  STOP !')
        STOP
      ENDIF
      XDIM(   1   :NXDIM-2) = XDIM(   1   :NXDIM-2) * CM
      XDIM(NXDIM-1:NXDIM  ) = XDIM(NXDIM-1:NXDIM  ) * DEG
      XDIM(NXDIM) = XDIM(NXDIM-1) + XDIM(NXDIM)
      IF( NRDIM1 <= 2 ) THEN
        RR = MAX( ABS(XDIM(2)) , ABS(XDIM(5)) )
        I4 = 2.4D0 * SQRT(RR) + 1.D0
        NR = 4 * MIN( 12 , MAX( 2 , I4 ) )
        AA = (XDIM(NXDIM)-XDIM(NXDIM-1)) / 360.D0
        IF( AA < 0.95D0 ) THEN
          NR = AA*NR + 1
          NR = MAX( 3 , NR )
        ENDIF
      ELSE
        NR = NRDIM1
      ENDIF
      CALL DEF_PCON_RIOZA1A2( NameVolu, Icolor, NXDIM, XDIM, NR )
    ELSEIF( ISHAPE == 6 ) THEN
!>> TRTmodule !!! ------------------------------------------------------
      IF( NXDIM /= 4 ) THEN
        PRINT 7776,NXDIM
7776    FORMAT(/' There exists one volume of shape TRTMODULE',    &
                ' without properly defined 4 dimentions (',I4,')  =====>  STOP !')
        STOP
      ENDIF
      XDIM(1:4) = XDIM(1:4) * CM
      CALL DEF_TRTBM_BH1B2Z( NameVolu, Icolor, XDIM )
    ELSEIF( ISHAPE == 7 ) THEN
!>> Gvxy !!! -----------------------------------------------------------
      IF( NXDIM /= NYDIM .OR. NXDIM <= 2 .OR. NZDIM /= 1 ) THEN
        PRINT 7777,NXDIM,NYDIM,NZDIM
7777    FORMAT(/' There exists one volume of shape GVXY without',    &
                ' properly defined dimentions',                      &
                ' in X, Y and Z (',3I4,')  =====>  STOP !')
        STOP
      ENDIF
      XDIM(1:NXDIM) = XDIM(1:NXDIM) * CM
      YDIM(1:NYDIM) = YDIM(1:NYDIM) * CM
      DZ            = ZDIM(1)       * CM
      CALL DEF_GVXY_NXYDZ( NameVolu, Icolor, NXDIM, XDIM,YDIM,DZ )
    ELSEIF( ISHAPE == 8 ) THEN
!>> Gvxysx !!! ---------------------------------------------------------
      IF( NXDIM /= NYDIM .OR. NXDIM <= 1 .OR. NZDIM /= 1 ) THEN
        PRINT 7778,NXDIM,NYDIM,NZDIM
7778    FORMAT(/' There exists one volume of shape GVXYSX without',    &
                ' properly defined dimentions',                        &
                ' in X, Y and Z (',3I4,')  =====>  STOP !')
        STOP
      ENDIF
      XDIM(1:NXDIM) = XDIM(1:NXDIM) * CM
      YDIM(1:NYDIM) = YDIM(1:NYDIM) * CM
      DZ            = ZDIM(1)       * CM
      CALL DEF_GVXYSX_NXYDZ( NameVolu, Icolor, NXDIM, XDIM,YDIM,DZ )
    ELSEIF( ISHAPE == 9 ) THEN
!>> Gvxysxy !!! --------------------------------------------------------
      IF( NXDIM /= NYDIM .OR. NXDIM <= 0 .OR. NZDIM /= 1 ) THEN
        PRINT 7779,NXDIM,NYDIM,NZDIM
7779    FORMAT(/' There exists one volume of shape GVXYSXY without',    &
                ' properly defined dimentions',                         &
                ' in X, Y and Z (',3I4,')  =====>  STOP !')
        STOP
      ENDIF
      XDIM(1:NXDIM) = XDIM(1:NXDIM) * CM
      YDIM(1:NYDIM) = YDIM(1:NYDIM) * CM
      DZ            = ZDIM(1)       * CM
      CALL DEF_GVXYSXY_NXYDZ( NameVolu, Icolor, NXDIM, XDIM,YDIM,DZ )
    ELSEIF( ISHAPE == 10 ) THEN
!>> Snake !!! ----------------------------------------------------------
      NXDIM3 = (NXDIM-2) / 3
      IF( NXDIM < 7 .OR. NXDIM /= 3*NXDIM3+2 ) THEN
        PRINT 7780,NXDIM
7780    FORMAT(/' There exists one volume of shape SNAKE without',    &
                ' properly defined  3xN+2  dimentions (',I4,')  =====>  STOP !')
        STOP
      ENDIF
      XDIM(1:NXDIM-1) = XDIM(1:NXDIM-1) * CM
      XDIM(  NXDIM  ) = XDIM(  NXDIM  ) * DEG
      Radius = XDIM(  1  )
      Phi0   = XDIM(NXDIM)
      DO I=1,NXDIM3
        XDIM(I) = XDIM(3*I-1)
        YDIM(I) = XDIM(3*I)
        ZDIM(I) = XDIM(3*I+1)
      ENDDO
      IF( NRDIM1 <= 2 ) THEN
        RR = ABS(Radius)
        I4 = 2.4D0 * SQRT(RR) + 1.D0
        NR = 4 * MIN( 12 , MAX( 2 , I4 ) )
      ELSE
        NR = NRDIM1
      ENDIF
      CALL DEF_SNAKE_NXYZNRF( NameVolu, Icolor, NXDIM3,XDIM,YDIM,ZDIM, NR,Radius,Phi0 )
    ELSEIF( ISHAPE == 11 ) THEN
!>> Elipso !!! ---------------------------------------------------------
      IF( NXDIM /= 3 ) THEN
        PRINT 7781,NXDIM
7781    FORMAT(/' There exists one volume of shape ELIPSO',    &
                ' without properly defined 3 dimentions (',I4,')  =====>  STOP !')
        STOP
      ENDIF
      XDIM(1:3) = XDIM(1:3) * CM
      CALL DEF_ELIPSO_RXYZN1N2( NameVolu, Icolor, XDIM,NRDIM1,NRDIM2)
    ELSEIF( ISHAPE == 12 ) THEN
!>> Gcylrz !!! ---------------------------------------------------------
      IF( NXDIM /= NYDIM .OR. NXDIM <= 2 .OR. NZDIM /= 2 ) THEN
        PRINT 7782,NXDIM,NYDIM,NZDIM
7782    FORMAT(/' There exists one volume of shape GcylRZ without',    &
                ' properly defined dimentions',                        &
                ' in R, Z and Phi (',3I4,')  =====>  STOP !')
        STOP
      ENDIF
      XDIM(1:NXDIM) = XDIM(1:NXDIM) * CM
      YDIM(1:NYDIM) = YDIM(1:NYDIM) * CM
      Phi0          = ZDIM(1)       * DEG
      dPhi          = ZDIM(2)       * DEG
      NbPhi         = MAX( NRDIM1 , 1 )
      CALL DEF_GcylRZ_NRZDF( NameVolu, Icolor, NXDIM, XDIM,YDIM, Phi0,dPhi, NbPhi )
    ELSEIF( ISHAPE == 13 ) THEN
!>> Gslice !!! ---------------------------------------------------------
      IF( NZDIM < 2 ) THEN
        PRINT 7783,NZDIM
7783    FORMAT(/' There exists one volume of shape Gslice without',    &
                ' properly defined dimentions : Nb_Z_slices =',I4,'  =====>  STOP !')
        STOP
      ENDIF
      DO IZ=1,NZDIM
        IF( IZ < 2 ) THEN
          NXYTOT = NXYDIM(IZ)
        ELSE
          NXYTOT = NXYDIM(IZ) - NXYDIM(IZ-1)
        ENDIF
        IF( NXYTOT > 2 ) CYCLE
        PRINT 7784,IZ,NXYTOT
7784    FORMAT(/' There exists one volume of shape Gslice ',    &
                'without properly defined dimentions : ',       &
                'for Z_slice',I4,', Nb_XY_points =',I4,'  =====>  STOP !')
        STOP
      ENDDO
      ZDIM(1:NZDIM)         = ZDIM(1:NZDIM)         * CM
      XDIM(1:NXYDIM(NZDIM)) = XDIM(1:NXYDIM(NZDIM)) * CM
      YDIM(1:NXYDIM(NZDIM)) = YDIM(1:NXYDIM(NZDIM)) * CM
      CALL DEF_GSLICEXY_NXYDZ( NameVolu, Icolor, NZDIM, ZDIM, NXYDIM, XDIM, YDIM )
    ELSEIF( ISHAPE == 14 ) THEN
!>> Elcyl !!! ----------------------------------------------------------
      IF( NXDIM /= 7 ) THEN
        PRINT 7785,NXDIM
7785    FORMAT(/' There exists one volume of shape ELCYL without',    &
                ' properly defined 5 (or 7) dimentions (',I4,')  =====>  STOP !')
        STOP
      ENDIF
      XDIM(1:5) = XDIM(1:5) * CM
      XDIM(6:7) = XDIM(6:7) * DEG
      XDIM( 7 ) = XDIM( 6 ) + XDIM( 7 )
      IF( NRDIM1 <= 2 ) THEN
        RR = MAX( ABS(XDIM(1)) , ABS(XDIM(2)) )
        I12= 0.8D0 * SQRT(RR) + 1.D0
        NR = 12 * MIN( 4 , I12 )
        AA = (XDIM(5)-XDIM(4)) / 360.D0
        IF( AA < 0.95D0 ) THEN
          NR = AA*NR + 1
          NR = MAX( 3 , NR )
        ENDIF
      ELSE
        NR = NRDIM1 
      ENDIF
      CALL DEF_ELCYL_R1R2DZA1A2( NameVolu, Icolor, XDIM, NR )
    ELSE
!>> Else ??? -----------------------------------------------------------
      PRINT 7786,TRIM(NameVolu)
7786  FORMAT(' Cannot create generic volume : ',A,' =====>  STOP !')
      STOP
    ENDIF
!
 END SUBROUTINE CREAT_XML_SOLI
