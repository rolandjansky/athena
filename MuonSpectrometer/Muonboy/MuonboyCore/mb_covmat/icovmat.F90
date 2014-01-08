!
!> Matrix inversion
!> \author A.Ouraou
!
 SUBROUTINE ICOVMAT(npp,covmat,iret,nparam,nparmax,iprloc)
 USE M_MB_Cholevski
 IMPLICIT NONE
 INTEGER, INTENT(IN)    :: npp, nparam, nparmax, iprloc
 INTEGER, INTENT(OUT)   :: iret                    !< should be 0 when successful inversion
 REAL(8), INTENT(INOUT) :: covmat(nparmax,nparmax) !< in input = matrix to be inverted, in output = inverse matrix 
 REAL(8) :: covpanic(npp), covbak(npp,npp)
 INTEGER :: i, ifail
!>>
!>>  Inverse matrices
!>> npp is the number of fitted parameters (usually 5, but may be 4 when momentum fixed)
!>>
   covbak(1:npp,1:npp) = covmat(1:npp,1:npp)
!>>>> Do inversion
   call cholinv(covmat,nparam,nparmax,ifail)
   iret = ifail
!
   IF( ifail /= 0 ) THEN  ! let's try with only the nppxnpp matrix, i.e. without scattering
     IF( iprloc >= 1 ) WRITE(*,'(" Inversion Pb : trial without MS terms !")')
     covpanic(1:npp) = (/ (covbak(i,i), i=1,npp) /)  ! be prepared for emergency solution  
     call cholinv(covbak,npp,npp,ifail)
     IF( ifail == 0 ) THEN
       covmat(1:npp,1:npp) = covbak(1:npp,1:npp)   ! only these termes will be used later
     ELSE                 ! let's keep only the diagonal terms
       IF( iprloc >= 1 ) WRITE(*,'(" Inversion Pb : Panic Solution !")')
       iret = -2
       covmat(1:npp,1:npp) = 0.d0
       DO i=1,npp
         covmat(i,i) = 1.d0/covpanic(i)
       ENDDO
     ENDIF
   ENDIF
!
 END SUBROUTINE ICOVMAT
!
