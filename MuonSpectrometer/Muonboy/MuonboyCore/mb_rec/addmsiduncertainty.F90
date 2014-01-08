!
 SUBROUTINE AddMsIduncertainty
 USE M_MB_Control
 USE M_MB_MuGeom, ONLY : NCOUBAS2
 USE M_MB_Digis
 USE M_MB_Reco_Tracks
 IMPLICIT NONE
 REAL(8) :: errzmsid
 INTEGER :: ican, nbarrel, nendcap, i, isc
!
    do ican=1,nbcan
      if( chican(ican) >= 999.d0 ) CYCLE
      nbarrel = 0
      nendcap = 0
      do i=1,nzxcan(ican)
         isc = ( 1 + ktubhi(izxcan(i,ican))/10000 ) / 2
         if( isc <= ncoubas2 ) then
           nbarrel = nbarrel + 1
         else
           nendcap = nendcap + 1
         endif
      enddo      
      errzmsid = ErrZMsIdBa
      if( nendcap > nbarrel ) errzmsid = ErrZMsIdEc
      cozcan(2,2,ican) = cozcan(2,2,ican) + errzmsid**2
    enddo
!
 END SUBROUTINE AddMsIduncertainty
!
