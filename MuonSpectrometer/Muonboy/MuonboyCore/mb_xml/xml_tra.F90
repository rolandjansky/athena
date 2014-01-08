!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE XML_TRA(Iobj,IobjPRE, Jrep, TRA,ROT,I_G_S,GAP_SHIFT)
 USE M_MB_Xml
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
 INTEGER :: Iobj,IobjPRE,Jrep,I_G_S
 REAL(8) :: TRA(3),ROT(3,3),GAP_SHIFT
 CHARACTER(20), PARAMETER :: ACOMPO='composition',  ACOMPX='stackX',      ACOMPY='stackY', ACOMPZ='stackZ'
 CHARACTER(20), PARAMETER :: AINTER='intersection', ASUBTR='subtraction', AUNION='union',  AFOREA='foreach'
 REAL(8), PARAMETER :: EpsLoc=0.000001D0
 CHARACTER(80) :: CAR80E,CAR80A
 CHARACTER(20) :: CAR20P
 CHARACTER(80) :: ATAB( 5)
 INTEGER       :: ITAB(10), IATsectOK(80)
 REAL(8)       :: RTAB(10)
 INTEGER :: IELE, IELEPRE, IA,II,IR, IATT, NCAR, IATImpRot, NATCOPY,NATsectOK
 INTEGER :: IXYZ,IXYZ2,IXYZ3
 REAL(8) :: CM, RAD, ATRR,ATFI,ATZZ,ATSS, SINA,COSA, ATZ0,ATDZ,ATR0,ATDR
 REAL(8) :: ATRRxy,ATRRrf, ATFIxy,ATFIrf, ATX0,ATDX,ATYY,ATY0,ATDY,ATXX
 REAL(8) :: ATF0,ATDF, ATGA,ATG0,ATSH,ATS0,ATRO, GAP, SHIFT
!
    TRA(1:3)     = 0.D0
    ROT(1:3,1:3) = 0.D0
    ROT(  1,  1) = 1.D0
    ROT(  2,  2) = 1.D0
    ROT(  3,  3) = 1.D0
    I_G_S        = 0
    GAP_SHIFT    = 0.D0
!
    IF( Iobj <= 0 .OR. IobjPRE <= 0 )                      GO TO 900
    IELE    = Iele_xob(Iobj)
    IELEPRE = Iele_xob(IobjPRE)
    IF( IELE < Iel0_xml_sipos .OR. IELE > Iel1_xml_mupos ) GO TO 900
!
    CALL GIV_NAM_XML_ELE(IELE,    CAR80E ,NCAR)
    CALL GIV_NAM_XML_ELE(IELEPRE, ATAB(1),NCAR)
    CAR20P = ATAB(1)(1:20)
!
!>> Units !!!
    CALL GIV_VAL_XML_ATT(Iobj,'unit_length',IA,II,IR,ATAB,ITAB,RTAB)
    IF( ATAB(1) == 'default' ) ATAB(1) = Default_unit_length
    CM = 0.1D0
!** IF( ATAB(1) == 'mm' ) CM =   0.1D0
    IF( ATAB(1) == 'cm' ) CM =   1.0D0
    IF( ATAB(1) == 'm'  ) CM = 100.0D0
    CALL GIV_VAL_XML_ATT(Iobj,'unit_angle' ,IA,II,IR,ATAB,ITAB,RTAB)
    IF( ATAB(1) == 'default' ) ATAB(1) = Default_unit_angle
    RAD = PIS180
!** IF( ATAB(1) == 'deg'    ) RAD = PIS180
!** IF( ATAB(1) == 'degree' ) RAD = PIS180
    IF( ATAB(1) == 'mrad'   ) RAD = 0.001D0
    IF( ATAB(1) == 'radian' ) RAD = 1.000D0
!
    IF( CAR80E == 'posXYZ' ) THEN
!>> ------------- ******** -------------------------------------------
      IF( CAR20P /= ACOMPO .AND. CAR20P /= AINTER .AND.    &
          CAR20P /= ASUBTR .AND. CAR20P /= AUNION .AND.    &
          CAR20P /= AFOREA ) THEN
        PRINT 7771,CAR80E,CAR20P
7771    FORMAT( /' An XML element of type         : ',A80, &
                /' is embedded in another of type : ',A20, &
                /' ===============================> One IGNORES it !!!')
        GO TO 900
      ENDIF
      CALL GIV_VAL_XML_ATT(Iobj,'X_Y_Z', IA,II,IR,ATAB,ITAB,RTAB)
      TRA(1:3) = RTAB(1:3) *CM
    ELSEIF( CAR80E == 'posRPhiZ' ) THEN
!>> ----------------- ********** -------------------------------------
      IF( CAR20P /= ACOMPO .AND. CAR20P /= AINTER .AND.   &
          CAR20P /= ASUBTR .AND. CAR20P /= AUNION .AND.   &
          CAR20P /= AFOREA ) THEN
        PRINT 7771,CAR80E,CAR20P
        GO TO 900
      ENDIF
      DO IATT=1,Nb_xml_att(IELE)
        CALL GIV_NAM_XML_ATT(IELE,IATT, CAR80A,NCAR)
        IF( CAR80A == 'R_Phi_Z' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATRR =  RTAB(1)
          ATFI =  RTAB(2)
          ATZZ =  RTAB(3)
        ELSEIF( CAR80A == 'S' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATSS =  RTAB(1)
        ELSEIF( CAR80A == 'impliedRot' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          IATImpRot = 0
          IF( ATAB(1) == 'true' ) IATImpRot = 1
        ENDIF
      ENDDO
      SINA = SIN(ATFI * RAD)
      COSA = COS(ATFI * RAD)
      TRA(1)   = (ATRR*COSA - ATSS*SINA) * CM
      TRA(2)   = (ATRR*SINA + ATSS*COSA) * CM
      TRA(3)   =  ATZZ                   * CM
      IF( IATImpRot >= 1 ) THEN
        ROT(1,1) =  COSA
        ROT(2,1) = -SINA
        ROT(1,2) =  SINA
        ROT(2,2) =  COSA
      ENDIF
    ELSEIF( CAR80E == 'mposZ' ) THEN
!>> ----------------- ******* ----------------------------------------
      IF( CAR20P /= ACOMPO .AND. CAR20P /= AINTER .AND.   &
          CAR20P /= ASUBTR .AND. CAR20P /= AUNION ) THEN
        PRINT 7771,CAR80E,CAR20P
        GO TO 900
      ENDIF
      DO IATT=1,Nb_xml_att(IELE)
        CALL GIV_NAM_XML_ATT(IELE,IATT, CAR80A,NCAR)
        IF( CAR80A == 'Z0' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATZ0 =  RTAB(1)
        ELSEIF( CAR80A == 'dZ' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATDZ =  RTAB(1)
        ELSEIF( CAR80A == 'X_Y' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATRRxy = SQRT( RTAB(1)**2 + RTAB(2)**2 )
          IF( ATRRxy > EpsLoc ) THEN
            ATFIxy = ATAN2( RTAB(2) , RTAB(1) ) / RAD
          ELSE
            ATFIxy = 0.D0
          ENDIF
        ELSEIF( CAR80A == 'R_Phi' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATRRrf =  RTAB(1)
          ATFIrf =  RTAB(2)
        ELSEIF( CAR80A == 'S' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATSS =  RTAB(1)
        ENDIF
      ENDDO
      IF( ATRRxy > ATRRrf ) THEN
        ATRR = ATRRxy
        ATFI = ATFIxy
      ELSE
        ATRR = ATRRrf
        ATFI = ATFIrf
      ENDIF
      SINA = SIN(ATFI * RAD)
      COSA = COS(ATFI * RAD)
      TRA(1)   = (ATRR*COSA - ATSS*SINA) * CM
      TRA(2)   = (ATRR*SINA + ATSS*COSA) * CM
      TRA(3)   = (ATZ0 + (Jrep-1)*ATDZ)  * CM
    ELSEIF( CAR80E == 'mposX' ) THEN
!>> ----------------- ******* ----------------------------------------
      IF( CAR20P /= ACOMPO .AND. CAR20P /= AINTER .AND.   &
          CAR20P /= ASUBTR .AND. CAR20P /= AUNION ) THEN
        PRINT 7771,CAR80E,CAR20P
        GO TO 900
      ENDIF
      DO IATT=1,Nb_xml_att(IELE)
        CALL GIV_NAM_XML_ATT(IELE,IATT, CAR80A,NCAR)
        IF( CAR80A == 'X0' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATX0 =  RTAB(1)
        ELSEIF( CAR80A == 'dX' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATDX =  RTAB(1)
        ELSEIF( CAR80A == 'Y_Z' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATYY =  RTAB(1)
          ATZZ =  RTAB(2)
        ENDIF
      ENDDO
      TRA(1)   = (ATX0 + (Jrep-1)*ATDX) * CM
      TRA(2)   =  ATYY                  * CM
      TRA(3)   =  ATZZ                  * CM
    ELSEIF( CAR80E == 'mposY' ) THEN
!>> ----------------- ******* ----------------------------------------
      IF( CAR20P /= ACOMPO .AND. CAR20P /= AINTER .AND.   &
          CAR20P /= ASUBTR .AND. CAR20P /= AUNION ) THEN
        PRINT 7771,CAR80E,CAR20P
        GO TO 900
      ENDIF
      DO IATT=1,Nb_xml_att(IELE)
        CALL GIV_NAM_XML_ATT(IELE,IATT, CAR80A,NCAR)
        IF( CAR80A == 'Y0' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATY0 =  RTAB(1)
        ELSEIF( CAR80A == 'dY' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATDY =  RTAB(1)
        ELSEIF( CAR80A == 'Z_X' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATZZ =  RTAB(1)
          ATXX =  RTAB(2)
        ENDIF
      ENDDO
      TRA(1)   =  ATXX                  * CM
      TRA(2)   = (ATY0 + (Jrep-1)*ATDY) * CM
      TRA(3)   =  ATZZ                  * CM
    ELSEIF( CAR80E == 'mposR' ) THEN
!>> ----------------- ******* ----------------------------------------
      IF( CAR20P /= ACOMPO .AND. CAR20P /= AINTER .AND.   &
          CAR20P /= ASUBTR .AND. CAR20P /= AUNION ) THEN
        PRINT 7771,CAR80E,CAR20P
        GO TO 900
      ENDIF
      DO IATT=1,Nb_xml_att(IELE)
        CALL GIV_NAM_XML_ATT(IELE,IATT, CAR80A,NCAR)
        IF( CAR80A == 'R0' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATR0 =  RTAB(1)
        ELSEIF( CAR80A == 'dR' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATDR =  RTAB(1)
        ELSEIF( CAR80A == 'Z_Phi' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATZZ =  RTAB(1)
          ATFI =  RTAB(2)
        ELSEIF( CAR80A == 'S' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATSS =  RTAB(1)
        ENDIF
      ENDDO
      SINA = SIN(ATFI * RAD)
      COSA = COS(ATFI * RAD)
      ATRR     =  ATR0 + (Jrep-1)*ATDR
      TRA(1)   = (ATRR*COSA - ATSS*SINA) * CM
      TRA(2)   = (ATRR*SINA + ATSS*COSA) * CM
      TRA(3)   =  ATZZ                   * CM
    ELSEIF( CAR80E == 'mposPhi' .OR. CAR80E == 'mposWedge' ) THEN
!>> ----------------- ********* -------------- *********** -----------
      IF( CAR20P /= ACOMPO .AND. CAR20P /= AINTER .AND.   &
          CAR20P /= ASUBTR .AND. CAR20P /= AUNION ) THEN
        PRINT 7771,CAR80E,CAR20P
        GO TO 900
      ENDIF
      DO IATT=1,Nb_xml_att(IELE)
        CALL GIV_NAM_XML_ATT(IELE,IATT, CAR80A,NCAR)
        IF( CAR80A == 'Phi0' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATF0 =  RTAB(1)
        ELSEIF( CAR80A == 'dPhi' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATDF =  RTAB(1)
        ELSEIF( CAR80A == 'R_Z' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATRR =  RTAB(1)
          ATZZ =  RTAB(2)
        ELSEIF( CAR80A == 'S' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATSS =  RTAB(1)
        ELSEIF( CAR80A == 'impliedRot' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          IATImpRot = 0
          IF( ATAB(1) == 'true' ) IATImpRot = 1
        ELSEIF( CAR80A == 'ncopy' .OR. CAR80A == 'wedge_number' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          NATCOPY = ITAB(1)
        ELSEIF( CAR80A == 'sectors' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          NATsectOK = II
          IATsectOK(1:II) = ITAB(1:II)
        ENDIF
      ENDDO
      IF( CAR80E == 'mposWedge' ) THEN
        IF( NATCOPY /= NATsectOK ) THEN
          PRINT 7772,CAR80E,NATCOPY,NATsectOK
7772      FORMAT( /' In an XML element of type : ',A80,      &
                  /' the attribute  wedge_number  is =',I6,  &
                  /' while the number of values in',         &
                  /' the attribute    sectors     is =',I6,  &
                  /' ===============================> One IGNORES it !!!')
          GO TO 900
        ENDIF
        IF( IATsectOK(Jrep) < 1 ) GO TO 900
      ENDIF
      IF( ABS(ATDF) < EpsLoc ) THEN
        IF( NATCOPY <= 0 ) NATCOPY = 36
        IF( ABS(RAD-0.001D0) < EpsLoc ) THEN
          ATDF = 1000.D0 * TWOPI / NATCOPY
        ELSEIF( ABS(RAD- PIS180) < EpsLoc ) THEN
          ATDF =      360.D0     / NATCOPY
        ENDIF
      ENDIF
      ATFI = ATF0 + (Jrep-1)*ATDF
      SINA = SIN(ATFI * RAD)
      COSA = COS(ATFI * RAD)
      TRA(1)   =  (ATRR*COSA - ATSS*SINA) * CM
      TRA(2)   =  (ATRR*SINA + ATSS*COSA) * CM
      TRA(3)   =   ATZZ                   * CM
      IF( IATImpRot >= 1 ) THEN
        ROT(1,1) =  COSA
        ROT(2,1) = -SINA
        ROT(1,2) =  SINA
        ROT(2,2) =  COSA
      ENDIF
    ELSEIF( CAR80E == 'axisPos' .OR. CAR80E == 'axisMPos' ) THEN
!>> ----------------- ********* ---------------**********-------------
      NATCOPY = 1
      DO IATT=1,Nb_xml_att(IELE)
        CALL GIV_NAM_XML_ATT(IELE,IATT, CAR80A,NCAR)
        IF( CAR80A == 'dX' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATDX =  RTAB(1)
        ELSEIF( CAR80A == 'dY' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATDY =  RTAB(1)
        ELSEIF( CAR80A == 'dZ' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATDZ =  RTAB(1)
        ELSEIF( CAR80A == 'gap' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATGA =  RTAB(1)
        ELSEIF( CAR80A == 'gap0' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATG0 =  RTAB(1)
        ELSEIF( CAR80A == 'shift' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATSH =  RTAB(1)
        ELSEIF( CAR80A == 'shift0' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATS0 =  RTAB(1)
        ELSEIF( CAR80A == 'rotation' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          ATRO =  RTAB(1)
        ELSEIF( CAR80A == 'ncopy' ) THEN
          CALL GIV_VAL_XML_ATT(Iobj,IATT,IA,II,IR,ATAB,ITAB,RTAB)
          NATCOPY = ITAB(1)
        ENDIF
      ENDDO
      IF( CAR20P == ACOMPX ) THEN
        IXYZ = 1
      ELSEIF( CAR20P == ACOMPY ) THEN
        IXYZ = 2
      ELSEIF( CAR20P == ACOMPZ ) THEN
        IXYZ = 3
      ELSE
        PRINT 7771,CAR80E,CAR20P
        GO TO 900
      ENDIF
      IXYZ2 = IXYZ + 1
      IXYZ3 = IXYZ + 2
      IF( IXYZ2 > 3 ) IXYZ2 = IXYZ2 - 3
      IF( IXYZ3 > 3 ) IXYZ3 = IXYZ3 - 3
      SINA = SIN(ATRO * RAD)
      COSA = COS(ATRO * RAD)
      TRA(1)           = ATDX * CM
      TRA(2)           = ATDY * CM
      TRA(3)           = ATDZ * CM
      ROT(IXYZ2,IXYZ2) =   COSA
      ROT(IXYZ3,IXYZ2) = - SINA
      ROT(IXYZ2,IXYZ3) =   SINA
      ROT(IXYZ3,IXYZ3) =   COSA
      GAP              = ATGA * CM
      SHIFT            = ATSH * CM
      IF( NATCOPY >= 2 .AND. Jrep == NATCOPY ) THEN
        GAP           = ATG0 * CM
        SHIFT         = ATS0 * CM
      ENDIF
      IF( ABS(GAP) > EpsLoc .AND. ABS(SHIFT) > EpsLoc ) THEN
        CALL GIV_VAL_XML_ATT(Iobj,1,IA,II,IR,ATAB,ITAB,RTAB)
        PRINT 7773,CAR80E,ATAB(1)
7773    FORMAT( /' An XML element of type  : ',A80,                     &
                /' with volume attribute   = ',A80,                     &
                /' has both  gap  and  shift  attributes specified !',  &
                /'          *****     *******  ',                       &
                /' ========================> One STOPs !!!')
        STOP
      ELSEIF( ABS(GAP) > EpsLoc ) THEN
        GAP_SHIFT = GAP
      ELSEIF( ABS(SHIFT) > EpsLoc ) THEN
        I_G_S     = 1
        GAP_SHIFT = SHIFT
      ENDIF
    ENDIF
!
    RETURN
!
!
900 Jrep = 0
    RETURN
!
 END
