!
!> Rejection of segments according to the pattern of tubes hit/missed/masked/inefficient
!
 LOGICAL FUNCTION rejectSegment(IT1, IT2, IWW, ISEL)
 USE M_MB_Geom_PARAMETERS, ONLY : N4M
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IT1(N4M,4), IT2(N4M,4), IWW(2), ISEL
 INTEGER :: NNML, NBON, NMAS, NMIS, NLAT, NDUB, NEFF, I, IP, K, NCUT
 INTEGER :: ITCHI(N4M,4)
!--  ISEL : = 0 ! looser rejection (called before segment final fit)
!--         = 1 ! final rejection
!--         = 2 ! tighter rejection for roa defined with only mdt
    NNML = 0
    NBON = 0
    NMAS = 0
    NMIS = 0
    NLAT = 0
    NDUB = 0
    NEFF = 0
!
    rejectSegment = .false.
    DO I=1,2
      IF( IWW(I) <= 0 )  CYCLE
      IF( I == 1 ) THEN
        ITCHI(:,:) = IT1(:,:)
      ELSE
        ITCHI(:,:) = IT2(:,:)
      ENDIF
      NNML = NNML + 1
      DO IP=1,N4M
        DO K=1,4
          SELECT CASE ( ITCHI(IP,K) )
            CASE (        1:)          ; NBON = NBON + 1
            CASE ( -9999999:-1)        ; NMAS = NMAS + 1 ! masked
            CASE (-10000000)           ; NMIS = NMIS + 1 ! not crossed
            CASE (-16000000:-15000000) ; NLAT = NLAT + 1 ! hit not compatible, much too late
            CASE (-20000029:-20000000) ; NDUB = NDUB + 1 ! inefficient, but dubious (< 3 sig)
            CASE (         :-20000030) ; NEFF = NEFF + 1 ! inefficient
            CASE DEFAULT
              IF( ITCHI(IP,K) /= 0 )  WRITE(*,*) ' Muonboy rejectSegment : ITCHI =',ITCHI(IP,K)
          END SELECT
        ENDDO
      ENDDO
    ENDDO
    NCUT = NMAS + NLAT + 2*NEFF
    IF( NMIS >= 3 )  NMIS = NMIS + 1
    IF( ISEL >= 1 )  NCUT = NCUT + NDUB + NLAT + NMIS/2
    IF( NCUT >= 2*NBON )            rejectSegment = .true.
    IF( ISEL >= 2 .AND. NBON <= 3 ) rejectSegment = .true.
!
 END FUNCTION rejectSegment
!
