!
 MODULE M_MB_Cholevski
!> Cholesky decomposition of a positive-definite symmetric matrix, taken from numerical
!> recipes in Fortran
 IMPLICIT NONE

 CONTAINS
  SUBROUTINE choldc(a,n,np,p,ier)
!> Given a positive-definite symmetric matrix A(1:n,1:n) with physical dimension np,
!> this routine constructs its Cholesky decomposition A=L.Lt On input, only the
!> upper triangle of A need be given; it is not modified. The Cholevsky factor L
!> is returned in the lower triangle of A, except for its diagonal elements which
!> are returned in p(1:n). ier returns 0 is A is positive-definite, 1 otherwise.
  INTEGER, INTENT(IN)    :: n, np
  INTEGER, INTENT(OUT)   :: ier
  REAL(8), INTENT(OUT)   :: p(n)
  REAL(8), INTENT(INOUT) :: a(np,np)
  INTEGER :: i, j, k
  REAL(8) :: sum
   ier = 0
   do i=1,n
     do j=i,n
       sum = a(i,j)
       do k=i-1,1,-1
         sum = sum - a(i,k)*a(j,k)
       enddo
       if( i == j ) then
         if( sum <= 0.d0 ) then
           ier = 1  ! a is not positive definite
           return
         endif
         p(i) = sqrt(sum)
       else
         a(j,i) = sum / p(i)
       endif
     enddo
   enddo
  END SUBROUTINE choldc
!
  SUBROUTINE cholsl(a,n,np,p,b,x)
!> Solves the set of n linear equations A.x=b where A is a positive-definite
!> symmetric matrix with physical dimension np. A and p are input as the output
!> of the routine choldc. only the lower subdiagonal portion of A is accessed.
!> b(1:n) is input as the right-hand side vector. The solution vector is returned
!> in x(1:n). a, n, np and p are not modified and can be left in place for succesive
!> calls with different right-hand sides b. b is not modified unless you identify b and
!> x in the calling sequence, which is allowed. 
  INTEGER, INTENT(IN)  :: n, np
  REAL(8), INTENT(IN)  :: a(np,np), b(n), p(n)
  REAL(8), INTENT(OUT) :: x(n)
  REAL(8) :: sum
  INTEGER :: i, k
   do i=1,n
     sum = b(i)
     do k=i-1,1,-1
       sum = sum - a(i,k)*x(k)
     enddo
     x(i) = sum / p(i)
   enddo
   do i=n,1,-1
     sum = x(i)
     do k=i+1,n
       sum = sum - a(k,i)*x(k)
     enddo
     x(i) = sum / p(i)
   enddo
  END SUBROUTINE cholsl
!
  SUBROUTINE cholinv(a,n,np,ier)
  INTEGER, INTENT(IN)    :: n, np
  INTEGER, INTENT(OUT)   :: ier
  REAL(8), INTENT(INOUT) :: a(np,np)
  REAL(8) :: sum, p(n)
  INTEGER :: i, j, k
   call choldc(a,n,np,p,ier)
   if( ier == 0 ) then
     do i=1,n
! first compute lower triangle of L-1
       a(i,i) = 1.d0 / p(i)
       do j=i+1,n
         sum = 0.d0
         do k=i,j-1
           sum = sum - a(j,k)*a(k,i)
         enddo
         a(j,i) = sum / p(j)
       enddo
     enddo
!
     do i=1,n
       a(i,i+1:n) = 0.d0
     enddo
!
     do i=1,n
       a(i,i) = a(i,i)*a(i,i)
       do k=i+1,n
         a(i,i) = a(i,i) + a(k,i)*a(k,i)
       enddo
       do j=i+1,n
         do k=j,n
           a(i,j) = a(i,j) + a(k,i)*a(k,j)
         enddo
       enddo
     enddo
     do i=1,n
       a(i,1:i-1) = a(1:i-1,i)
     enddo
   endif
  END SUBROUTINE cholinv
 END MODULE M_MB_Cholevski
!
