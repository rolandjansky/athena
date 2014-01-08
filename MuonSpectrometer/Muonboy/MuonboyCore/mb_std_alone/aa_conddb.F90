!
  MODULE M_MB_CondDB
  IMPLICIT NONE
!
  INTEGER, PARAMETER :: ncdbmax=200
  INTEGER, SAVE :: ncdbtag=0
  CHARACTER(80), SAVE :: cdbfoldername(ncdbmax), cdbtagname(ncdbmax)
!  
  INTEGER, SAVE :: ncdbalitag=0
  CHARACTER(80), SAVE :: cdbalifoldername(ncdbmax), cdbalitagname(ncdbmax)
!  
  END MODULE M_MB_CondDB
!
