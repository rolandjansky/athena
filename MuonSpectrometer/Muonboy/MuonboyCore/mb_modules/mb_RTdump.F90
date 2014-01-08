!
!> \author SaMuSoG
!
  MODULE M_MB_RTDump
    IMPLICIT NONE
! variables used when using R parametrization from onlone cosmics
    INTEGER, SAVE :: IRTpara = 0
    REAL(8), SAVE :: fixedRadius = 1.45D0
! variables used when RT written on Dump
    LOGICAL, SAVE :: RTfromDump = .FALSE.
    LOGICAL, SAVE :: SWfromDump = .FALSE.
    INTEGER, SAVE              :: NRTdump
    INTEGER, SAVE, ALLOCATABLE :: IdeRTdump(:)  ,NpaRTdump(:)
    REAL(8), SAVE, ALLOCATABLE :: PosRTdump(:,:),DirRTdump(:,:)
    REAL(8), SAVE, ALLOCATABLE :: PasRTdump(:)  ,RayRTdump(:,:)
    REAL(8), SAVE, ALLOCATABLE :: SXPdump(:,:)  ,SYPdump(:,:)  ,SZPdump(:,:)
    REAL(8), SAVE, ALLOCATABLE :: SXDdump(:,:)  ,SYDdump(:,:)  ,SZDdump(:,:)
  CONTAINS
    SUBROUTINE ALLOC_RT
      IF(ALLOCATED(IdeRTdump)) THEN
        DEALLOCATE(IdeRTdump)
        DEALLOCATE(NpaRTdump)
        DEALLOCATE(PasRTdump)
        DEALLOCATE(PosRTdump)
        DEALLOCATE(DirRTdump)
        DEALLOCATE(RayRTdump)
        DEALLOCATE(SXPdump)
        DEALLOCATE(SYPdump)
        DEALLOCATE(SZPdump)
        DEALLOCATE(SXDdump)
        DEALLOCATE(SYDdump)
        DEALLOCATE(SZDdump)
      ENDIF
      ALLOCATE(IdeRTdump(NRTdump))
      ALLOCATE(NpaRTdump(NRTdump))
      ALLOCATE(PasRTdump(NRTdump))
      ALLOCATE(PosRTdump(3,NRTdump))
      ALLOCATE(DirRTdump(3,NRTdump))
      ALLOCATE(RayRTdump(0:100,NRTdump))
      ALLOCATE(SXPdump(0:100,NRTdump))
      ALLOCATE(SYPdump(0:100,NRTdump))
      ALLOCATE(SZPdump(0:100,NRTdump))
      ALLOCATE(SXDdump(0:100,NRTdump))
      ALLOCATE(SYDdump(0:100,NRTdump))
      ALLOCATE(SZDdump(0:100,NRTdump))
    END SUBROUTINE ALLOC_RT
  END MODULE M_MB_RTDump
!
