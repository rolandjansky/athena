!
!> Compute the parameters of the muons candidates at their perigee
!> \author M.Virchaux, J.F.Laporte
!
 SUBROUTINE TOPERI
 USE M_MB_Control
 USE M_MB_Fit
 USE M_MB_COMUSC
 IMPLICIT NONE
 INTEGER, PARAMETER :: ipamax=2000
 INTEGER :: ipas
 REAL(8) :: papas, alpha
 REAL(8) :: vin(8), vout(8)
 REAL(8) :: dotin, dotout
!--
!--  From Muon vector VDEB, do backtracking to vertex and set VFIN
!--
   vin(1:8) = vdeb(1:8,1)
!
!-- Orient momentum toward the vertex
   if( DOT_PRODUCT(vin(1:2),vin(4:5)) > 0.d0 ) then
     vin(4:6) = - vin(4:6)
     vin(8)   = - vin(8)
   endif
!
!-- Perform tracking toward the vertex
   idmasc(1:nbmasc) = 0
   call setSignEloss(2,vin(4:6))
   do ipas=1,ipamax
     call vinvou(vin,vout,papas,1,2)
     if( DOT_PRODUCT(vout(1:2),vout(4:5)) > 0.d0 ) exit     
     if( ipas >= ipamax ) then
       if( MBPRI >= 0 ) print  '(" >>>>> error in TOPERI : ipamax too small ")'
       exit
     endif
     vin(:) = vout(:)
   enddo
!
!-- Compute Muon vector at d.c.a.
   dotin  = DOT_PRODUCT( vin(1:2), vin(4:5))
   dotout = DOT_PRODUCT(vout(1:2),vout(4:5))
   alpha = ( 0.d0 - dotin ) / ( dotout - dotin )
   vfin(1:7) = (1.d0-alpha)*vin(1:7) + alpha*vout(1:7)
   vfin( 8 ) = vin(8)
!-- Re-oriente the track 
   vfin(4:6) = - vfin(4:6)
   vfin( 8 ) = - vfin( 8 )
!
 END SUBROUTINE TOPERI
!
