!
!> Clusterize the RPC/TGC strips and define the S position for the segment reconstruction
!> \author A.Ouraou
!
 SUBROUTINE doClusters
 USE M_MB_Control
 USE M_MB_General_CONSTANTS, ONLY : C1P000
 USE M_MB_MuGeom
 USE M_MB_Digis
 USE M_MB_SSBAN
 IMPLICIT NONE
 REAL(8), PARAMETER :: FACLUS = 1.9D0
 INTEGER, PARAMETER :: NSTRIMU=200, NSTRIMI=4*NSTRIMU
 INTEGER :: JSBUN(NSTRIMU), ISBUN(NSTRIMU), LSBUN(NSTRIMU), JSBIN(NSTRIMI), ISBIN(NSTRIMI)
 REAL(8) :: XSBUN(NSTRIMU),                                 XSBIN(NSTRIMI)
 INTEGER :: IC, IW, INOF, INOZ, IGEOM, NbSBUN, NbZBUN, NbSBIN, NbSBINpre
 INTEGER :: ILIN, NlinkIwIc, ItrCHA, IL, ICT, IRPC, ItrTRI, IP, IS, LS, L, ISBANtot, LLS
 REAL(8) :: TlinkIwIc, DXSBIN, TI,ZI, DIREC, TIP, Strue,Ztrue,Ttrue, Sidea,Zidea,Tidea
 REAL(8) :: T0,Z0, Smin,Smax, SiF0,CoF0, SiFi,CoFi, X,Y,Z
!!$ LOGICAL :: notBOG
!
    DO 210 IC=1,NCOUCH
      DO 200 IW=1,NCHAHI(IC)
        NSBAN0(IC,IW) = 1
        NSBAN1(IC,IW) = 0
        INOF   = IFCHA(IC,IW)
        INOZ   = IZCHA(IC,IW)
        IGEOM  = IGEOMA(IC,INOF,INOZ)
        ILIN   = ISTLINV(IGEOM)
        ItrCHA = ItransCHA(IGEOM)
        T0     = TCECHAV(IGEOM)
        Z0     = ZCECHAV(IGEOM)
        ILIN   = ISTLINV(IGEOM)
        SiF0   = SINCHAV(IGEOM)
        CoF0   = COSCHAV(IGEOM)
        Smin   = 0.5d0 * ( SNICHAV(IGEOM) + SNACHAV(IGEOM) ) - 50.d0
        Smax   = 0.5d0 * ( SPICHAV(IGEOM) + SPACHAV(IGEOM) ) + 50.d0
!
        NbSBIN    = 0
        NbSBINpre = 0
        NlinkIwIc = 0
        TlinkIwIc = 0.D0
        DXSBIN    = 0.D0
        DO 100 IL=1,NBLINK(ILIN)
          ICT = ICTRIG(IL,ILIN)
          IRPC  = IRPTGC(ICT)
          ItrTRI= ItransTRI(ICT)
          TI    = TCETRI(ICT)
          ZI    = ZCETRI(ICT)
          SiFi  = SINTRI(ICT)
          CoFi  = COSTRI(ICT)
          DXSBIN = DXSBIN + FACLUS*DSBAND(IRPC)
!>> DIREC < 0 if one is in the "negative" end cap
          DIREC = SIGN( C1P000, TI )
!
!>> Loops on all the strips of all the sensitive layers (IP) of trigger chamber ICT
          NbSBUN = 0
          NbZBUN = 0
          DO IP=1,N2BAND(IRPC)

            TIP = TI + DIREC * TBAND(IRPC,IP)
            DO IS=NBANH0(ICT,IP)+1,NBANH0(ICT,IP)+NBANHI(ICT,IP)
              IF( LBANHI(IS) < 0  )  CYCLE
!             IF( rejectStrip(IS) )  CYCLE
              IF( IP <= N1BAND(IRPC) ) THEN
                IF( NbSBUN >= NSTRIMU ) EXIT
!>> Define the allowed S value for each hit S strip of trigger chamber ICT -> XSBUN
                CALL Pos_True_from_Ideal( ItrTRI, SBANHI(IS), ZI, TIP, Strue,Ztrue,Ttrue )
                CALL MuboSZTtoXYZ( ICT<=NTRIBA, SiFi,CoFi, Strue,Ztrue,Ttrue, X,Y,Z )
                CALL MuboXYZtoSZT( IC<=NCOUBA , SiF0,CoF0, X,Y,Z, Strue,Ztrue,Ttrue )
                CALL Pos_Ideal_from_True( ItrCHA, Strue,Ztrue,Ttrue, Sidea,Zidea,Tidea )
                IF( IC <= NCOUBA ) THEN
                  Sidea = T0 * Sidea / Tidea
                ELSE
                  Sidea = Z0 * Sidea / Zidea
                ENDIF
                IF( Sidea < Smin .OR. Sidea > Smax ) CYCLE
                NbSBUN = NbSBUN + 1
                JSBUN(NbSBUN) = 1
                ISBUN(NbSBUN) = IS
                LSBUN(NbSBUN) = LBANHI(IS)/10 + 1000*IP
                XSBUN(NbSBUN) = Sidea
                NlinkIwIc = NlinkIwIc + 1
                TlinkIwIc = TlinkIwIc + Tidea
              ELSE
                NbZBUN = NbZBUN+1
              ENDIF
            ENDDO
          ENDDO
!
          IF( NbSBUN <= 0 ) CYCLE
!
!!$          IF( AtCosm == 0 ) THEN
!!$! is this to take into account ambiguity in RPC S strips ?? so should be done only for RPC ?? to be checked
!!$! modif 01 sep 2006 : in the meantime, do not apply it for cosmics
!!$! modif 04 jan 2011 : do not apply it for BOG because of problems with cutout
!!$            notBOG = .true.
!!$            IF( isBOG(INOZ) ) THEN
!!$              IF( INOF == 6 .OR. INOF == 7 ) THEN
!!$                IF( CARISC((IC+1)/2) == 'BOS' ) THEN
!!$                  notBOG = .false.
!!$                ENDIF
!!$              ENDIF
!!$            ENDIF
!!$            IF( notBOG ) THEN
!!$              IF( NbZBUN <= 0 )  CYCLE
!!$            ENDIF
!!$          ENDIF
!
!-- Clusterizes the strips in trigger chamber ICT :  XSBUN  and  XZBUN
          CALL Mubo_Cluster( FACLUS*DSBAND(IRPC), NbSBUN, XSBUN,JSBUN )
!-- Merge clusters when noisy trigger chamber if consecutives strips
          CALL MergeCluster( NbSBUN, XSBUN,JSBUN,LSBUN, N1BAND(IRPC) )
!
!-- Accumulate the allowed S values for multilayer IC,IW in XSBIN :  XSBUN -> XSBIN
          DO LS=1,NbSBUN
            IF( NbSBIN >= NSTRIMI ) EXIT
            NbSBIN = NbSBIN + 1
            XSBIN(NbSBIN) = XSBUN(LS)
            ISBIN(NbSBIN) = ISBUN(LS)
            IF( JSBUN(LS) > 0 ) THEN
!****         JSBIN(NbSBIN) = JSBUN(LS)
              JSBIN(NbSBIN) = 1
            ELSE
              JSBIN(NbSBIN) = JSBUN(LS) - NbSBINpre
            ENDIF
          ENDDO
          NbSBINpre = NbSBIN
!
100     ENDDO
        IF( NBLINK(ILIN) > 0 ) DXSBIN    = DXSBIN    / NBLINK(ILIN)
        IF( NlinkIwIc    > 0 ) TlinkIwIc = TlinkIwIc / NlinkIwIc
!
!
!-- Clusterize the allowed S values in multilayer IC,IW :  XSBIN -> SSBAN
!-- and also store in SSBAN the coresponding T value (TlinkIwIc)
        CALL Mubo_Cluster( DXSBIN, NbSBIN, XSBIN,JSBIN )
        NSBAN0(IC,IW) = NSBANt + 1
        DO LS=1,NbSBIN
          IF( JSBIN(LS) < 1 ) CYCLE
          NSBANt = NSBANt + 1
          IF( SIZE(SSBAN) < NSBANt ) CALL Push_MB_SSBAN
          SSBAN(          NSBANt) = XSBIN(LS)
          ISBAN(  1      ,NSBANt) = ISBIN(LS)
          ISBAN( 2:NSTRBA,NSBANt) = 0
          L = 1
          DO LLS=1,NbSBIN
            IF( JSBIN(LLS) /= -LS ) CYCLE
            L = MIN( L+1 , NSTRBA )
            ISBAN(L,NSBANt) = ISBIN(LLS)
          ENDDO
        ENDDO
        NSBANt = NSBANt + 1
        IF( SIZE(SSBAN) < NSBANt+1 ) CALL Push_MB_SSBAN
        SSBAN(         NSBANt) = 999999.D0
        ISBAN(1:NSTRBA,NSBANt) = 0
        NSBAN1(IC,IW) = NSBANt
        NSBANt = NSBANt + 1
        IF( IC <= NCOUBA ) THEN
          SSBAN( NSBANt) = T0
        ELSE
          SSBAN( NSBANt) = TlinkIwIc
        ENDIF
        ISBAN(1:NSTRBA,NSBANt) = 0
!
        IF(MBPri >= 3) THEN
          PRINT 902,IC,IW,NSBAN1(IC,IW)-NSBAN0(IC,IW)+1,(SSBAN(LS),LS=NSBAN0(IC,IW),NSBAN1(IC,IW)+1)
902       FORMAT(' SSBAN for (',I3,',',I3,') =',I3,8F11.2,12(/25X,8F11.2))
          DO L=1,NSTRBA
            ISBANtot = 0
            DO LS=NSBAN0(IC,IW),NSBAN1(IC,IW)-1
              ISBANtot = ISBANtot + ISBAN(L,LS)
            ENDDO
            IF( ISBANtot > 0 ) WRITE(*,'(25X,8I11)') (ISBAN(L,LS),LS=NSBAN0(IC,IW),NSBAN1(IC,IW)-1)
          ENDDO
        ENDIF
!
200   ENDDO
210 ENDDO
!
 END SUBROUTINE doClusters
!
!
!> merge clusters if consecutive in all layers
!
 SUBROUTINE MergeCluster( Nclus, Tclus,Jclus,Lclus, NPmax )
 IMPLICIT NONE
 INTEGER, INTENT(IN)    :: Nclus, NPmax
 REAL(8), INTENT(INOUT) :: Tclus(*)
 INTEGER, INTENT(INOUT) :: Jclus(*)
 INTEGER, INTENT(IN)    :: Lclus(*)
 INTEGER :: I, J, IP, IS, N, L, K, Jsum, NCL
 INTEGER :: MAP(NPmax,200), IL(200), I1(200), I2(200), LP(200)
 INTEGER, PARAMETER :: NCLmin=10
!
    NCL = 0       ! number of current clusters
    IL(:) = 0     ! liste of the layers inside a cluster
    MAP(:,:) = 0
    DO I=1,Nclus
      IP = Lclus(I) / 1000
      IS = Lclus(I) - 1000*IP
      IF( Jclus(I) > 0 ) THEN
        J = I
        NCL = NCL + 1
      ELSE
        J = - Jclus(I)
      ENDIF
      MAP(IP,IS) = J
      IL(J) = IBSET(IL(J), IP)
    ENDDO
    IF( NCL <= NCLmin ) RETURN  ! do not try to merge if too few clusters
!
    N = 0
    DO IP=1,NPmax
bIS:  DO IS=2,200
        IF( MAP(IP,IS-1) == 0 )           CYCLE
        IF( MAP(IP,IS) == 0 )             CYCLE
        IF( MAP(IP,IS) == MAP(IP,IS-1) )  CYCLE
! we have two consecutive clusters ... check if they were not already found in another layer
        DO L=1,N
          IF( I1(L) /= MAP(IP,IS-1) .AND. I2(L) /= MAP(IP,IS-1) ) CYCLE
          IF( I1(L) /= MAP(IP,IS  ) .AND. I2(L) /= MAP(IP,IS  ) ) CYCLE
          LP(L) = IBSET(LP(L), IP)
          CYCLE bIS
        ENDDO
        N = N + 1
        LP(N) = IBSET(0, IP)
        I1(N) = MAP(IP,IS-1)
        I2(N) = MAP(IP,IS)
      ENDDO bIS
    ENDDO
!
! now do the real merging (only when consecutive on all layers)
! stop merging when only NCLmin clusters left
    DO L=1,N
      IF( I1(L) == I2(L) )  CYCLE
      IF( IL(I1(L)) /= LP(L) .OR. IL(I2(L)) /= LP(L) )  CYCLE
      Jsum = Jclus(I1(L)) + Jclus(I2(L))
      Tclus(I1(L)) = ( Jclus(I1(L))*Tclus(I1(L)) + Jclus(I2(L))*Tclus(I2(L)) ) / Jsum
      Jclus(I1(L)) =   Jsum
      Jclus(I2(L)) = - I1(L)
      DO I=1,Nclus
        IF( Jclus(I) == - I2(L) )  Jclus(I) = -I1(L)
      ENDDO
      DO K=L+1,N
        IF( I1(K) == I2(L) ) I1(K) = I1(L)
        IF( I2(K) == I2(L) ) I2(K) = I1(L)
      ENDDO
      NCL = NCL - 1
      IF( NCL <= NCLmin )  EXIT
    ENDDO
!
 END SUBROUTINE MergeCluster
!
