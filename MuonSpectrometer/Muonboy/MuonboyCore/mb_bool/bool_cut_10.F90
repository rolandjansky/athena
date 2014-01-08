!
!> Cuts the polygon made of the   JPOL(0)  points   VPOL(i,1:2)
!> by the polygon made of the     JCUT(0)  points   VCUT(i,1:2)
!> and give the result as
!> LRR polygons made of the     JRR(0,LRR) points    VRR(i,1:2,LRR)
!> \author SaMuSoG
!
 SUBROUTINE Bool_Cut_10( Ndim_poly,Ndim_gon, JPOL,VPOL, JCUT,VCUT, Ioverf, LRR,JRR,VRR )
 IMPLICIT NONE
 INTEGER :: Ndim_poly,Ndim_gon,Ioverf
 INTEGER :: JPOL(0:Ndim_gon  )
 REAL(8) :: VPOL(  Ndim_gon,2)
 INTEGER :: JCUT(0:Ndim_gon  )
 REAL(8) :: VCUT(  Ndim_gon,2)
 INTEGER :: LRR
 INTEGER :: JRR(0:Ndim_gon,  Ndim_poly)
 REAL(8) :: VRR(  Ndim_gon,2,Ndim_poly)
 INTEGER :: Itry,I,J,Nt,ICUT,IPOL,INSI,IP0,IC0
 INTEGER :: Ndim_cut
!
   Itry = 0
!
   LRR = 0
   JRR(0,1) = 0
   IF( JPOL(0) <= 0 ) RETURN
   IF( JCUT(0) <= 0 ) THEN
     LRR        = 1
     JRR(:,  1) = JPOL(:)
     VRR(:,:,1) = VPOL(:,:)
     RETURN
   ENDIF
!
!>> Is the cutout  VCUT  entirely inside the polygon  VPOL  ?
   DO ICUT=1,JCUT(0)
     CALL Bool_Is_It_In( VCUT(ICUT,1), VCUT(ICUT,2), JPOL(0), VPOL(1,1), VPOL(1,2), INSI)
     IF( INSI == -1 ) GO TO 30
   ENDDO
   DO IPOL=1,JPOL(0)
     CALL Bool_Is_It_In( VPOL(IPOL,1), VPOL(IPOL,2), JCUT(0), VCUT(1,1), VCUT(1,2), INSI)
     IF( INSI == 1 )  GO TO 30
   ENDDO
   CALL Bool_Closest_Pt( JPOL(0), VPOL(1,1), VPOL(1,2), JCUT(0), VCUT(1,1), VCUT(1,2), IP0,IC0)
   LRR = 1
   Nt = 1
   JRR(Nt,    LRR)  = 0
   VRR(Nt,1:2,LRR)  = VPOL(IP0,1:2)
   DO J=IC0,JCUT(0)+IC0-1
     I = MOD(J-1,JCUT(0)) + 1
     Nt = Nt + 1
     JRR(Nt,    LRR) = JCUT(I)
     VRR(Nt,1:2,LRR) = VCUT(I,1:2)
   ENDDO
   Nt = Nt + 1
   JRR(Nt,    LRR)  = 0
   VRR(Nt,1:2,LRR)  = VCUT(IC0,1:2)
   DO J=IP0,JPOL(0)+IP0-1
     I = MOD(J-1,JPOL(0)) + 1
     Nt = Nt + 1
     JRR(Nt,    LRR) = JPOL(I)
     VRR(Nt,1:2,LRR) = VPOL(I,1:2)
   ENDDO
   JRR(0,LRR) = Nt
   RETURN
!
!>> Is the polygon  VPOL  entirely inside the cutout  VCUT  ?
30 CONTINUE
   IF( ICUT >= 2 )    GO TO 50
   DO ICUT=2,JCUT(0)
     CALL Bool_Is_It_In( VCUT(ICUT,1), VCUT(ICUT,2), JPOL(0), VPOL(1,1), VPOL(1,2), INSI)
     IF( INSI == 1 )  GO TO 50
   ENDDO
   DO IPOL=1,JPOL(0)
     CALL Bool_Is_It_In( VPOL(IPOL,1), VPOL(IPOL,2), JCUT(0), VCUT(1,1), VCUT(1,2), INSI)
     IF( INSI == -1 ) GO TO 50
   ENDDO
   RETURN
!
!>> General case
50 Ndim_cut = 200
   CALL Bool_Cut_XX( -1,1, Ndim_poly,Ndim_gon,Ndim_cut, JPOL,VPOL, JCUT,VCUT, Ioverf, LRR,JRR,VRR )
   IF( Ioverf == 6 .AND. Itry <= 10 ) THEN
     Itry = Itry + 1
     Ndim_cut = 2*Ndim_cut
     GO TO 50
   ENDIF
   RETURN
!
 END SUBROUTINE Bool_Cut_10
!
