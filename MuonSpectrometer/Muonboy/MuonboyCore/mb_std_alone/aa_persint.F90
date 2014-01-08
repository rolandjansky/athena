!
  MODULE M_MB_Persint
    IMPLICIT NONE
!
    TYPE track
      INTEGER       :: code, char, colo, ishap
      REAL(8)       :: posx, posy, posz
      REAL(8)       :: posxe, posye, posze
      REAL(8)       :: momx, momy, momz
      REAL(8)       :: rtmax, ztmax
      CHARACTER(40) :: name, cont
    END TYPE track
!
    TYPE cell
      INTEGER       :: icalo, layer, color, CellId
      REAL(8)       :: rayon, energ, eta, phi, deta, dphi, dr, dx, dy, dz
      CHARACTER(3)  :: name
    END TYPE cell
!
    INTEGER, PARAMETER :: MIDEN=100
    TYPE elga   ! electron-gamma object
      INTEGER       :: char, NbCell, color, CellId(MIDEN)
      REAL(8)       :: pos(3), ene, eta, phi
      REAL(8)       :: mom(3)
      CHARACTER(40) :: name
    END TYPE elga
!
    TYPE trthit
      CHARACTER(3) :: name
      INTEGER      :: phi, modu, layer, straw, thres
      REAL(8)      :: timOthr, radius, xyz1(3), xyz2(3)
    END TYPE trthit
!
    TYPE vertex
      CHARACTER(40) :: name
      REAL(8)       :: xver, yver, zver
    END TYPE vertex
!
    TYPE jet
      CHARACTER(40) :: type
      REAL(8)       :: eta, phi, radius, energy
    END TYPE jet
!
! Missing transverse energy (MET)
    TYPE met
      CHARACTER(40) :: type
      REAL(8)       :: phi, energy ! phi [deg], energy [GeV]
    END TYPE met
!
! Calorimeter cell trapezoid
    TYPE celltrd
      INTEGER       :: cellIndex ! index of calorimeter cell
      REAL(8)       :: norm ! scale
    END TYPE celltrd
!
! Road Of Interest at Level 1
    TYPE l1roi
      CHARACTER(40) :: type         ! ROI type (L1Muon, L1Jet, L1EMTau)
      REAL(8)       :: eta          ! eta
      REAL(8)       :: phi          ! phi [deg]
      REAL(8)       :: threshold    ! highest threshold [GeV]
    END TYPE l1roi
!
! Egamma object trapezoid
    TYPE egatrd
      INTEGER       :: egaIndex     ! index of egamma object
    END TYPE egatrd
!
    INTEGER, PARAMETER :: MTRK=1000
    INTEGER    ,  SAVE :: nbtrk=0
    TYPE(track),  SAVE, ALLOCATABLE :: Trk(:)
    REAL(8), PARAMETER :: PtminTrk=0.2D0
!
    INTEGER, PARAMETER :: MTRAID=250
    INTEGER, SAVE :: NBTRAID=0, JCOLTRAID=0
    INTEGER, SAVE :: JCTRAID(MTRAID), NNTRAID(MTRAID)
    INTEGER, SAVE :: IReprTRAID(MTRAID), ISignTRAID(MTRAID)
    REAL(8), SAVE :: EETRAID(MTRAID), FFTRAID(MTRAID), PTTRAID(MTRAID)
    REAL(8), SAVE :: XXTRAID(MTRAID), YYTRAID(MTRAID), ZZTRAID(MTRAID)
    REAL(8), PARAMETER :: PTMIN_TRAID=0.50D0
!
    INTEGER, PARAMETER :: MCALO=2000
    INTEGER   ,   SAVE :: nbcal=0, jcolcalo=0
    TYPE(cell),   SAVE, ALLOCATABLE :: Cal(:)
    REAL(8), PARAMETER :: EminCalo=0.1D0
!
    INTEGER, PARAMETER :: MELGA=20
    INTEGER   ,   SAVE :: nbega=0
    TYPE(elga),   SAVE, ALLOCATABLE :: Ega(:)
!
    INTEGER, PARAMETER :: M2NDLEV=50
    INTEGER, SAVE :: NB2NDLEV=0, JCOL2NDLEV=0
    REAL(8), SAVE :: Z02NDLEV(M2NDLEV), T02NDLEV(M2NDLEV), F02NDLEV(M2NDLEV)
    REAL(8), SAVE :: AA2NDLEV(M2NDLEV), BB2NDLEV(M2NDLEV), TC2NDLEV(M2NDLEV), DT2NDLEV(M2NDLEV)
!
    INTEGER, PARAMETER :: MaxSegAth=500, MaxSegMoo=500
    INTEGER, SAVE :: NSegAth=0
    REAL(8), SAVE :: PtSegAth(3,MaxSegAth), VeSegAth(3,MaxSegAth), ChSegAth(MaxSegAth), T0SegAth(MaxSegAth)
    CHARACTER(40), SAVE :: ContSegAth(MaxSegAth)
    INTEGER, SAVE :: NSegMoo=0
    REAL(8), SAVE :: PtSegMoo(3,MaxSegMoo), VeSegMoo(3,MaxSegMoo), ChSegMoo(MaxSegMoo), T0SegMoo(MaxSegMoo) 
    CHARACTER(40), SAVE :: ContSegMoo(MaxSegMoo)
!
    INTEGER, PARAMETER :: MTRT=2000
    INTEGER,      SAVE :: nbtrt=0
    TYPE(trthit), SAVE, ALLOCATABLE :: Trt(:)
!
    INTEGER, PARAMETER :: MVERT=10
    INTEGER,      SAVE :: nbver=0
    TYPE(vertex), SAVE, ALLOCATABLE :: Ver(:)
!
    INTEGER, PARAMETER :: MJET=10
    INTEGER,      SAVE :: nbjet=0
    TYPE(jet), SAVE, ALLOCATABLE :: Jets(:)
!
! Missing transverse energy
    INTEGER, PARAMETER :: MMET=2                ! Maximumber number of MET
    INTEGER,      SAVE :: nbmet=0               ! Number of MET
    TYPE(met), SAVE, ALLOCATABLE :: Mets(:)     ! Array of MET
!
! Calorimeter cell trapezoid
    INTEGER, PARAMETER :: MCELLTRD=1            ! Maximumber number
    INTEGER,      SAVE :: nbcelltrd=0           ! Number of trapezoids
    TYPE(celltrd), SAVE, ALLOCATABLE :: CaloCellTrd(:)     ! Array of trapezoids
!
! Roads of Interest (ROI) at Level 1
    INTEGER, PARAMETER :: ML1ROI=2
    INTEGER,      SAVE :: nbRoiLvl1=0
    TYPE(l1roi), SAVE, ALLOCATABLE :: RoiLvl1(:)
! Egamma object trapezoid
    INTEGER, PARAMETER :: MEGATRD=1                  ! Maximumber number
    INTEGER,      SAVE :: nbegatrd=0                 ! Number of trapezoids
    TYPE(egatrd), SAVE, ALLOCATABLE :: EgammaTrd(:)     ! Array of trapezoids
!
  CONTAINS
    SUBROUTINE Reset_MB_Persint
      nbtrk    = 0
      nbcal    = 0
      nbega    = 0
      NBTRAID  = 0
      NSegAth  = 0
      NSegMoo  = 0
      NB2NDLEV = 0
      nbtrt    = 0
      nbver    = 0
      nbjet    = 0
      nbmet    = 0
      nbcelltrd = 0
      nbRoiLvl1 = 0
      nbegatrd = 0
      IF( .NOT.ALLOCATED(Trk) ) ALLOCATE( Trk(MTRK ) )
      IF( .NOT.ALLOCATED(Cal) ) ALLOCATE( Cal(MCALO) )
      IF( .NOT.ALLOCATED(Ega) ) ALLOCATE( Ega(MELGA) )
      IF( .NOT.ALLOCATED(Trt) ) ALLOCATE( Trt(MTRT ) )
      IF( .NOT.ALLOCATED(Ver) ) ALLOCATE( Ver(MVERT) )
      IF( .NOT.ALLOCATED(Jets) ) ALLOCATE( Jets(MJET) )
      IF( .NOT.ALLOCATED(Mets) ) ALLOCATE( Mets(MMET) )
      IF( .NOT.ALLOCATED(CaloCellTrd) ) ALLOCATE( CaloCellTrd(MCELLTRD) )
      IF( .NOT.ALLOCATED(RoiLvl1) ) ALLOCATE( RoiLvl1(ML1ROI) )
      IF( .NOT.ALLOCATED(EgammaTrd) ) ALLOCATE( EgammaTrd(MEGATRD) )
    END SUBROUTINE Reset_MB_Persint
!
    SUBROUTINE FillTRK(NAME, CONT, XYZ, PXYZ, Charge, codePDG, XYZE)
    CHARACTER(*), INTENT(IN) :: NAME, CONT
    REAL(8),      INTENT(IN) :: XYZ(3), PXYZ(3), Charge, codePDG,  XYZE(3)
    TYPE(track), ALLOCATABLE :: Trktmp(:)
      IF( SQRT( PXYZ(1)**2 + PXYZ(2)**2 ) < PtminTrk ) RETURN
      IF( nbtrk >= SIZE(Trk) ) THEN
        ALLOCATE( Trktmp(nbtrk) )
        Trktmp(1:nbtrk) = Trk(1:nbtrk)
        DEALLOCATE( Trk )
        ALLOCATE( Trk(nbtrk+MTRK) )
        Trk(1:nbtrk) = Trktmp(1:nbtrk)
        DEALLOCATE( Trktmp )
      ENDIF
      nbtrk = nbtrk + 1
      Trk(nbtrk)%posx = XYZ(1)
      Trk(nbtrk)%posy = XYZ(2)
      Trk(nbtrk)%posz = XYZ(3)
      Trk(nbtrk)%momx = PXYZ(1)
      Trk(nbtrk)%momy = PXYZ(2)
      Trk(nbtrk)%momz = PXYZ(3)
      Trk(nbtrk)%code = NINT(codePDG)
      Trk(nbtrk)%char = NINT(Charge)
      Trk(nbtrk)%colo = 1     ! black by default
      Trk(nbtrk)%ishap = -1   ! -1=default shape
      Trk(nbtrk)%name = NAME
      Trk(nbtrk)%posxe = XYZE(1)
      Trk(nbtrk)%posye = XYZE(2)
      Trk(nbtrk)%posze = XYZE(3)
      Trk(nbtrk)%rtmax = -1.d0
      Trk(nbtrk)%ztmax = -1.d0
      Trk(nbtrk)%cont = CONT
    END SUBROUTINE FillTRK
!
    SUBROUTINE FillCell(NOM, LL, RR, EN, ET, FI, DE, DF, DR, DX, DY, DZ, ID)
    CHARACTER(3), INTENT(IN) :: NOM
    INTEGER     , INTENT(IN) :: LL, ID
    REAL(8)     , INTENT(IN) :: RR, EN, ET, FI, DE, DF, DR, DX, DY, DZ
    TYPE(cell), ALLOCATABLE :: Caltmp(:)
      IF( EN < EminCalo ) RETURN
      IF( nbcal >= SIZE(Cal) ) THEN
        ALLOCATE( Caltmp(nbcal) )
        Caltmp(1:nbcal) = Cal(1:nbcal)
        DEALLOCATE( Cal )
        ALLOCATE( Cal(nbcal+MCALO) )
        Cal(1:nbcal) = Caltmp(1:nbcal)
        DEALLOCATE( Caltmp )
      ENDIF
      nbcal = nbcal + 1
      Cal(nbcal)%name  = NOM
      Cal(nbcal)%icalo = -1
      Cal(nbcal)%layer = MIN( MAX( 1 , LL ), 11 )
      Cal(nbcal)%rayon = RR
      Cal(nbcal)%energ = EN
      Cal(nbcal)%eta   = ET
      Cal(nbcal)%phi   = FI
      Cal(nbcal)%deta  = DE
      Cal(nbcal)%dphi  = DF
      Cal(nbcal)%dr    = DR
      Cal(nbcal)%dx    = DX
      Cal(nbcal)%dy    = DY
      Cal(nbcal)%dz    = DZ
      Cal(nbcal)%CellId    = ID
    END SUBROUTINE FillCell
!
    SUBROUTINE FillElga(NOM, ICH, X, Y, Z, PX, PY, PZ, EN, ET, FI, NC, IAD)
    CHARACTER(40), INTENT(IN) :: NOM
    INTEGER      , INTENT(IN) :: ICH, NC, IAD(NC)
    REAL(8)      , INTENT(IN) :: X, Y, Z, PX, PY, PZ, EN, ET, FI
    TYPE(elga), ALLOCATABLE :: Egatmp(:)
    INTEGER :: NNC
      IF( nbega >= SIZE(Ega) ) THEN
        ALLOCATE( Egatmp(nbega) )
        Egatmp(1:nbega) = Ega(1:nbega)
        DEALLOCATE( Ega )
        ALLOCATE( Ega(nbega+MELGA) )
        Ega(1:nbega) = Egatmp(1:nbega)
        DEALLOCATE( Egatmp )
      ENDIF
      nbega = nbega + 1
      Ega(nbega)%name = NOM
      Ega(nbega)%char = ICH
      Ega(nbega)%pos(1)  = X
      Ega(nbega)%pos(2)  = Y
      Ega(nbega)%pos(3)  = Z
      Ega(nbega)%mom(1)  = PX
      Ega(nbega)%mom(2)  = PY
      Ega(nbega)%mom(3)  = PZ
      Ega(nbega)%ene  = EN
      Ega(nbega)%eta  = ET
      Ega(nbega)%phi  = FI
      NNC = NC
      IF( NC > MIDEN ) THEN
         WRITE(*,*) ' Increase MIDEN to',NC
         NNC = MIDEN
      ENDIF
      Ega(nbega)%NbCell  = NNC
      Ega(nbega)%CellId(1:NNC) = IAD(1:NNC)
    END SUBROUTINE FillElga
!
    SUBROUTINE FillTRT(nom, i, tot, rad, xyz1, xyz2)
    CHARACTER(3), INTENT(IN) :: nom
    INTEGER,      INTENT(IN) :: i(5)
    REAL(8),      INTENT(IN) :: tot, rad, xyz1(3), xyz2(3)
    TYPE(trthit), ALLOCATABLE :: Trttmp(:)
      IF( rad <= 0.d0 )  RETURN
      IF( nbtrt >= SIZE(Trt) ) THEN
        ALLOCATE( Trttmp(nbtrt) )
        Trttmp(1:nbtrt) = Trt(1:nbtrt)
        DEALLOCATE( Trt )
        ALLOCATE( Trt(nbtrt+MTRT) )
        Trt(1:nbtrt) = Trttmp(1:nbtrt)
        DEALLOCATE( Trttmp )
      ENDIF
      nbtrt = nbtrt + 1
      Trt(nbtrt)%name      = nom
      Trt(nbtrt)%phi       = i(1)
      Trt(nbtrt)%modu      = i(2)
      Trt(nbtrt)%layer     = i(3)
      Trt(nbtrt)%straw     = i(4)
      Trt(nbtrt)%thres     = i(5)
      Trt(nbtrt)%timOthr   = tot
      Trt(nbtrt)%radius    = rad
      Trt(nbtrt)%xyz1(1:3) = xyz1(1:3)
      Trt(nbtrt)%xyz2(1:3) = xyz2(1:3)
    END SUBROUTINE FillTRT
!
    SUBROUTINE FillVert( nom, xyz )
    CHARACTER(40), INTENT(IN) :: nom
    REAL(8),       INTENT(IN) :: xyz(3)
    TYPE(vertex), ALLOCATABLE :: Vertmp(:)
      IF( nbver >= SIZE(Ver) ) THEN
        ALLOCATE( Vertmp(nbver) )
        Vertmp(1:nbver) = Ver(1:nbver)
        DEALLOCATE( Ver )
        ALLOCATE( Ver(nbver+MVERT) )
        Ver(1:nbver) = Vertmp(1:nbver)
        DEALLOCATE( Vertmp )
      ENDIF
      nbver = nbver + 1
      Ver(nbver)%name = nom
      Ver(nbver)%xver = xyz(1)
      Ver(nbver)%yver = xyz(2)
      Ver(nbver)%zver = xyz(3)
    END SUBROUTINE FillVert
!
    SUBROUTINE FillJet( type, param )
    CHARACTER(40), INTENT(IN) :: type
    REAL(8),       INTENT(IN) :: param(4)
    TYPE(jet), ALLOCATABLE :: tmpJets(:)
      IF( nbjet >= SIZE(Jets) ) THEN
        ALLOCATE( tmpJets(nbjet) )
        tmpJets(1:nbjet) = Jets(1:nbjet)
        DEALLOCATE( Jets )
        ALLOCATE( Jets(nbjet+MJET) )
        Jets(1:nbjet) = tmpJets(1:nbjet)
        DEALLOCATE( tmpJets )
      ENDIF
      nbjet = nbjet + 1
      Jets(nbjet)%type = type
      Jets(nbjet)%eta = param(1)
      Jets(nbjet)%phi = param(2)
      Jets(nbjet)%radius = param(3)
      Jets(nbjet)%energy = param(4)
    END SUBROUTINE FillJet
!
    SUBROUTINE FillMet( type, param )
    CHARACTER(40), INTENT(IN) :: type
    REAL(8),       INTENT(IN) :: param(2)
    TYPE(met), ALLOCATABLE :: tmpMets(:)
      IF( nbmet >= SIZE(Mets) ) THEN
        ALLOCATE( tmpMets(nbmet) )
        tmpMets(1:nbmet) = Mets(1:nbmet)
        DEALLOCATE( Mets )
        ALLOCATE( Mets(nbmet+MMET) )
        Mets(1:nbmet) = tmpMets(1:nbmet)
        DEALLOCATE( tmpMets )
      ENDIF
      nbmet = nbmet + 1
      Mets(nbmet)%type = type
      Mets(nbmet)%phi = param(1)
      Mets(nbmet)%energy = param(2)
    END SUBROUTINE FillMet
!
    SUBROUTINE FillRoi( type, param )
    CHARACTER(40), INTENT(IN) :: type
    REAL(8),       INTENT(IN) :: param(3)
    TYPE(l1roi), ALLOCATABLE :: tmpRoiLvl1(:)
      IF( nbRoiLvl1 >= SIZE(RoiLvl1) ) THEN
        ALLOCATE( tmpRoiLvl1(nbRoiLvl1) )
        tmpRoiLvl1(1:nbRoiLvl1) = RoiLvl1(1:nbRoiLvl1)
        DEALLOCATE( RoiLvl1 )
        ALLOCATE( RoiLvl1(nbRoiLvl1+ML1ROI) )
        RoiLvl1(1:nbRoiLvl1) = tmpRoiLvl1(1:nbRoiLvl1)
        DEALLOCATE( tmpRoiLvl1 )
      ENDIF
      nbRoiLvl1 = nbRoiLvl1 + 1
      RoiLvl1(nbRoiLvl1)%type = type
      RoiLvl1(nbRoiLvl1)%eta = param(1)
      RoiLvl1(nbRoiLvl1)%phi = param(2)
      RoiLvl1(nbRoiLvl1)%threshold = param(3)
    END SUBROUTINE FillRoi
  END MODULE M_MB_Persint
!
