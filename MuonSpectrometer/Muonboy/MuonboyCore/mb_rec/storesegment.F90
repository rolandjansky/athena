!
 SUBROUTINE StoreSegment(ISC, segm, looseORtight)
 USE M_MB_Control
 USE M_MB_Types
 USE M_MB_MuGeom
 USE M_MB_Digis
 USE M_MB_Reco_Segments
 USE M_MB_SSBAN
 IMPLICIT NONE
 TYPE(Segment), INTENT(IN) :: segm
 INTEGER, INTENT(IN) :: ISC, looseORtight
 INTEGER :: L, J, ISS, iaw1, iaw2, nono, nnh, typcod
 REAL(8) :: XUM,YUM,ZUM, XUD,YUD,ZUD
 REAL(8) :: dxaxisX,dxaxisY,dxaxisZ,dyaxisX,dyaxisY,dyaxisZ,dzaxisX,dzaxisY,dzaxisZ
 INTEGER :: ISCUR, IRpcTgc,I1rst2nd
 INTEGER :: igeom, jtyp, jff, jzz, job, jsl, jsz, jtube, Ident
 REAL(8) :: XYZ1(3),XYZ2(3)
 REAL(8) :: Lambda1,Lambda2,Lambda, Xdca,Ydca,Zdca, szflag
 REAL(8) :: AssociatedReso, t0
 REAL(8) :: VecNorm(3)
 REAL(8) :: LambdaKEEP,Guard,LambdaPlus,LambdaMoins
 CHARACTER(3) :: ctyp
 REAL(8) :: getT0fromTubes

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!
    iaw1 = abs( segm%iw(1) )
    iaw2 = abs( segm%iw(2) )
    CALL Shift_to_True_Seg( 2*ISC  ,iaw1,segm%spos(1),segm%zpos(1),segm%tpos(1),      &
                            2*ISC-1,iaw2,segm%spos(2),segm%zpos(2),segm%tpos(2),      &
                            segm%phi0, XUM,YUM,ZUM, XUD,YUD,ZUD,                      &
                            dxaxisX,dxaxisY,dxaxisZ,dyaxisX,dyaxisY,dyaxisZ,dzaxisX,dzaxisY,dzaxisZ )
    NBINTR = NBINTR + 1
    IF( SIZE(IKINTR) < NBINTR ) CALL Push_MB_Reco_Segments_MAINTR
    ISCINTR(   NBINTR) = ISC
    PTINTR(1:3,NBINTR) = (/ XUM, YUM, ZUM /)
    VEINTR(1:3,NBINTR) = (/ XUD, YUD, ZUD /)
    DXINTR(1:3,NBINTR) = (/  dxaxisX,dxaxisY,dxaxisZ /)
    DYINTR(1:3,NBINTR) = (/  dyaxisX,dyaxisY,dyaxisZ /)
    DZINTR(1:3,NBINTR) = (/  dzaxisX,dzaxisY,dzaxisZ /)
    ERINTR(1:6,NBINTR) = (/  0.,  0.,  0., 0.,  0.,  0. /)
    IERINTR(   NBINTR) = 0
    CHINTR(    NBINTR) = segm%chi2
    IKINTR(    NBINTR) = iaw1*1000 + iaw2
    IF( looseORtight == 0 )  IKINTR(NBINTR) = - IKINTR(NBINTR)  ! loose segment
    NUINTR(NBINTR) = segm%nu
    DO L=1,NUINTR(NBINTR)
      IUINTR(L,NBINTR)   = segm%iu(L)
      RdigINTR(L,NBINTR) = segm%rd(L)
      RtraINTR(L,NBINTR) = segm%rt(L)
      XDINTR(L,NBINTR)   = segm%xda(L)
      YDINTR(L,NBINTR)   = segm%yda(L)
      ZDINTR(L,NBINTR)   = segm%zda(L)
      XDdINTR(L,NBINTR)  = segm%xdd(L)
      YDdINTR(L,NBINTR)  = segm%ydd(L)
      ZDdINTR(L,NBINTR)  = segm%zdd(L)
      RDINTR(L,NBINTR)   = segm%rda(L)
    ENDDO
    IF( DoT0fit ) THEN
      T0INTR(NBINTR)  = getT0fromTubes(NUINTR(NBINTR), IUINTR(1:NUINTR(NBINTR),NBINTR))
    ELSE
      CALL fitSegmentT0(ISC, segm, t0)
      T0INTR(NBINTR)  = t0
    ENDIF
    ET0INTR(NBINTR) = 9999.d0
!--- filling of holes ----
    nnh = 0
    DO L=1,segm%nh
      IF( nnh >= MaxHol ) CYCLE
      igeom = MOD( segm%hide(L) , 100000 )
      jsl   = MOD( segm%hide(L)/100000, 10 )
      CALL IDECI( ISTTYPV(igeom), jtyp,jff,jzz,job )
      CALL FGetStationType( jtyp, ctyp)
      IF( ctyp(1:2) /= 'CS' ) THEN
        jsz   = 1   ! not used for mdt
        jtube =      segm%hide(L)/1000000
        CALL Code_Ident_AMDC(jtyp, jff, jzz, job, jsl, jsz, jtube, Ident)
        typcod = 1
      ELSE
        jsz    = segm%hide(L)/1000000  ! 1  means Z,  2 means S
        szflag = 3.d0 - 2.d0*jsz       ! >0 means Z, <0 means S
        xyz1(1) = 10.d0*segm%xhda(L)
        xyz1(2) = 10.d0*segm%yhda(L)
        xyz1(3) = 10.d0*segm%zhda(L)
        CALL FGetCSCindexFromStripPosition(ctyp,jff,jzz,job,jsl,szflag,xyz1,jtube)
        IF( jtube == 0 ) THEN
          IF( MBPri >= 1 ) WRITE(*,*) ' Muonboy:storesegment hole typ/jsl/jsz ',ctyp,jsl,jsz,' outside station'
          CYCLE
        ENDIF
        CALL Code_Ident_AMDC(jtyp, jff, jzz, job, jsl, jsz, jtube, ident)
        typcod = 4
      ENDIF
      nnh = nnh + 1
      IHINTR( nnh,NBINTR)  = Ident
      TYHINTR(nnh,NBINTR)  = typcod
      XHINTR( nnh,NBINTR)  = segm%xhda(L)
      YHINTR( nnh,NBINTR)  = segm%yhda(L)
      ZHINTR( nnh,NBINTR)  = segm%zhda(L)
      XHdINTR(nnh,NBINTR)  = segm%xhdd(L)
      YHdINTR(nnh,NBINTR)  = segm%yhdd(L)
      ZHdINTR(nnh,NBINTR)  = segm%zhdd(L)
    ENDDO
    NHINTR(NBINTR) = nnh
!--- end of holes --------
    NAINTR(     NBINTR) = 0
    DO J=1,2
      ISS = segm%is(J)
      IF( ISS < 1 ) CYCLE
      DO L=1,NSTRBA
        IF( ISBAN(L,ISS) < 1 )                   CYCLE
        IF( NUINTR(NBINTR)+NAINTR(NBINTR) > 19 ) EXIT
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!>> JFL Fri Mar 16 18:15:27 CET 2007
        ISCUR = ISBAN(L,ISS)
        call GetStripEnds( ISCUR, IRpcTgc,I1rst2nd, XYZ1,XYZ2,AssociatedReso , VecNorm )
        Lambda1 = (  XYZ1(1) - PTINTR(1,NBINTR) ) * VecNorm(1) &
                + (  XYZ1(2) - PTINTR(2,NBINTR) ) * VecNorm(2) &
                + (  XYZ1(3) - PTINTR(3,NBINTR) ) * VecNorm(3)
        Lambda2 =       VEINTR(1,NBINTR)          * VecNorm(1) &
                +       VEINTR(2,NBINTR)          * VecNorm(2) &
                +       VEINTR(3,NBINTR)          * VecNorm(3)
        Lambda = Lambda1 / Lambda2
!!>> Protection against segment parallel to the strip (i.e. BO strips with EM segment)
        Xdca = PTINTR(1,NBINTR) + Lambda * VEINTR(1,NBINTR)
        Ydca = PTINTR(2,NBINTR) + Lambda * VEINTR(2,NBINTR)
        Zdca = PTINTR(3,NBINTR) + Lambda * VEINTR(3,NBINTR)
        IF( sqrt(Xdca**2+Ydca**2) > 3000.d0 .OR. abs(Zdca) > 5000.d0 )  CYCLE
        Lambda1 = ( XYZ1(1) - Xdca ) * ( XYZ1(1) - XYZ2(1) ) &
                + ( XYZ1(2) - Ydca ) * ( XYZ1(2) - XYZ2(2) ) &
                + ( XYZ1(3) - Zdca ) * ( XYZ1(3) - XYZ2(3) )
        Lambda2 = ( XYZ1(1) - XYZ2(1) )**2 &
                + ( XYZ1(2) - XYZ2(2) )**2 &
                + ( XYZ1(3) - XYZ2(3) )**2
        Lambda = Lambda1 / Lambda2
        IF( Lambda < 0.d0 .OR. Lambda > 1.d0 )  CYCLE

!!>> Patch JFL Fri Feb 20 13:22:26 CET 2009
        LambdaKEEP = Lambda
        Guard = 0.5d0
        Lambda1 = VEINTR(1,NBINTR) * ( XYZ1(1) - XYZ2(1) ) &
                + VEINTR(2,NBINTR) * ( XYZ1(2) - XYZ2(2) ) &
                + VEINTR(3,NBINTR) * ( XYZ1(3) - XYZ2(3) )
        Lambda2 = VEINTR(1,NBINTR) * VecNorm(1) &
                + VEINTR(2,NBINTR) * VecNorm(2) &
                + VEINTR(3,NBINTR) * VecNorm(3)
        Lambda = Guard*Lambda1 / Lambda2
        Lambda = Lambda*sqrt( VecNorm(1)**2 &
                             +VecNorm(2)**2 &
                             +VecNorm(3)**2 &
                            )
        Lambda = Lambda/(  ( XYZ1(1) - XYZ2(1) )**2 &
                         + ( XYZ1(2) - XYZ2(2) )**2 &
                         + ( XYZ1(3) - XYZ2(3) )**2 &
                        )
        LambdaPlus  = LambdaKEEP + Lambda
        LambdaMoins = LambdaKEEP - Lambda
        IF( LambdaPlus  < 0.d0 .OR. LambdaPlus  > 1.d0 )  CYCLE
        IF( LambdaMoins < 0.d0 .OR. LambdaMoins > 1.d0 )  CYCLE

        NAINTR(NBINTR) = NAINTR(NBINTR) + 1
        IUINTR( NUINTR(NBINTR)+NAINTR(NBINTR), NBINTR) = ISBAN(L,ISS)
        RdigINTR(NUINTR(NBINTR)+NAINTR(NBINTR),NBINTR) = 0.d0
        RtraINTR(NUINTR(NBINTR)+NAINTR(NBINTR),NBINTR) = 0.d0
        XDINTR(NUINTR(NBINTR)+NAINTR(NBINTR),NBINTR) = Xdca
        YDINTR(NUINTR(NBINTR)+NAINTR(NBINTR),NBINTR) = Ydca
        ZDINTR(NUINTR(NBINTR)+NAINTR(NBINTR),NBINTR) = Zdca
        XDdINTR(NUINTR(NBINTR)+NAINTR(NBINTR),NBINTR)= VEINTR(1,NBINTR)
        YDdINTR(NUINTR(NBINTR)+NAINTR(NBINTR),NBINTR)= VEINTR(2,NBINTR)
        ZDdINTR(NUINTR(NBINTR)+NAINTR(NBINTR),NBINTR)= VEINTR(3,NBINTR)
!!>> JFL Fri Mar 16 18:15:27 CET 2007
!!>>    not clear which reso should be taken
        RDINTR(NUINTR(NBINTR)+NAINTR(NBINTR),NBINTR) = 1.D0

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!
      ENDDO
    ENDDO
! now filling of outliers
    NOINTR(NBINTR) = 0
    DO L=1,segm%no
      nono = NUINTR(NBINTR) + NAINTR(NBINTR) + NOINTR(NBINTR) + 1
      IF( nono > MaxTub )                                   EXIT
      IF( ISC <= NMATTR .AND. LTUBHI(-segm%iu(17-L)) /= 1 ) CYCLE ! to avoid problem with ROT
      NOINTR(NBINTR) = NOINTR(NBINTR) + 1
      IUINTR(nono,NBINTR)   = -segm%iu(17-L)
      RdigINTR(nono,NBINTR) =  segm%rd(17-L)
      RtraINTR(nono,NBINTR) =  segm%rt(17-L)
      XDINTR(nono,NBINTR)   =  segm%xda(17-L)
      YDINTR(nono,NBINTR)   =  segm%yda(17-L)
      ZDINTR(nono,NBINTR)   =  segm%zda(17-L)
      XDdINTR(nono,NBINTR)  =  segm%xdd(17-L)
      YDdINTR(nono,NBINTR)  =  segm%ydd(17-L)
      ZDdINTR(nono,NBINTR)  =  segm%zdd(17-L)
      RDINTR(nono,NBINTR)   =  segm%rda(17-L)
    ENDDO
!
! we must initialise the unused variables to avoid crashes which sometimes happen
! because pusharray copies all values (in Push_MB_Reco_Segments_MAINTR)
    L = NUINTR(NBINTR) + NAINTR(NBINTR) + NOINTR(NBINTR) + 1
    IF( L <= MaxTub ) THEN
        IUINTR(L:MaxTub,NBINTR) = 0
      RdigINTR(L:MaxTub,NBINTR) = 0.d0
      RtraINTR(L:maxTub,NBINTR) = 0.d0
        XDINTR(L:MaxTub,NBINTR) = 0.d0
        YDINTR(L:MaxTub,NBINTR) = 0.d0
        ZDINTR(L:MaxTub,NBINTR) = 0.d0
       XDdINTR(L:MaxTub,NBINTR) = 0.d0
       YDdINTR(L:MaxTub,NBINTR) = 0.d0
       ZDdINTR(L:MaxTub,NBINTR) = 0.d0
        RDINTR(L:MaxTub,NBINTR) = 0.d0
    ENDIF
! and same thing for holes
    L = NHINTR(NBINTR) + 1
    IF( L <= MaxHol ) THEN
       IHINTR(L:MaxHol,NBINTR) = 0
      TYHINTR(L:MaxHol,NBINTR) = 0
       XHINTR(L:MaxHol,NBINTR) = 0.d0
       YHINTR(L:MaxHol,NBINTR) = 0.d0
       ZHINTR(L:MaxHol,NBINTR) = 0.d0
      XHdINTR(L:MaxHol,NBINTR) = 0.d0
      YHdINTR(L:MaxHol,NBINTR) = 0.d0
      ZHdINTR(L:MaxHol,NBINTR) = 0.d0
    ENDIF
!
 END SUBROUTINE StoreSegment
!
 REAL(8) FUNCTION getT0fromTubes(Ntub, IdTub)
 USE M_MB_MuGeom
 USE M_MB_Digis
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Ntub
 INTEGER, INTENT(IN) :: IdTub(Ntub)
 INTEGER :: nt0, i, kt, ic, iw, inof, inoz, igeom
 REAL(8) :: st0
!
   nt0 = 0
   st0 = 0.d0
   do i=1,Ntub
     kt    = ktubhi(IdTub(i))
     ic    = kt / 10000
     if( ic > ncouch )            cycle
     iw    = (kt-ic*10000)/10
     inof  = ifcha(ic,iw)
     inoz  = izcha(ic,iw)
     igeom = igeoma(ic,inof,inoz)
     if( igeom <= 0 )             cycle
     if( dt0val(igeom) > 900.d0 ) cycle
     nt0 = nt0 + 1
     st0 = st0 + dt0val(igeom)
   enddo
   if( nt0 == 0 ) then
     getT0fromTubes = 9999.d0
   else
     getT0fromTubes = st0 / nt0
   endif
!
 END FUNCTION getT0fromTubes
!
