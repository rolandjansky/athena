!
!> \author SaMuSoG
!
 SUBROUTINE Bool_Prep_Vol(Ndim_volu,Ndim_plan,Ndim_edge,Ndim_gon, Ioverf, Nplan,Icolo,Nedge,  &
                          Ptxyz,Iseen, Is_outside, Nplan_max,Nedge_max )
 USE M_MB_General_CONSTANTS, ONLY : IdebugBOO, EPSBOO, IproblemBOO
 IMPLICIT NONE
 INTEGER :: Ndim_volu,Ndim_plan,Ndim_edge,Ndim_gon,Ioverf
 INTEGER :: Nplan(2,Ndim_volu)
 INTEGER :: Icolo(  Ndim_plan)
 INTEGER :: Nedge(2,Ndim_plan)
 REAL(8) :: Ptxyz(3,Ndim_edge)
 INTEGER :: Iseen(  Ndim_edge)
 LOGICAL :: Is_outside(Ndim_plan)
 INTEGER :: Nplan_max,Nedge_max
 INTEGER :: IV,IoV,IP,IE,ITRY,I12AB,IVA,IVB,IPA,IPB,NB,I,I1,Jxyz
 INTEGER :: IPB2,IE2,Nzer,Nstretch
 LOGICAL :: Lout(6)
 REAL(8) :: ShiftXYZ,Stretch,Ez,XYloc(2),PtoMove(3),PtDiff(3)
 REAL(8) :: Sys(3,0:3)
 REAL(8) :: Pcoo(Ndim_gon,3),Pco3Tot
 REAL(8) :: VolXYZmax(3,2),VolXYZmin(3,2)
!
!>> Find   Nplan_max, Nedge_max   and   VolXYZmax, VolXYZmin
   Nplan_max = 0
   Nedge_max = 0
   VolXYZmax(:,:) = -999999999.D0
   VolXYZmin(:,:) =  999999999.D0
   DO IV=1,2
     Nplan_max = MAX( Nplan_max , Nplan(2,IV) )
     DO IP=Nplan(1,IV),Nplan(2,IV)
       Nedge_max = MAX( Nedge_max , Nedge(2,IP) )
       DO IE=Nedge(1,IP),Nedge(2,IP)
         VolXYZmax(1:3,IV) = MAX( VolXYZmax(1:3,IV) , Ptxyz(1:3,IE) )
         VolXYZmin(1:3,IV) = MIN( VolXYZmin(1:3,IV) , Ptxyz(1:3,IE) )
       ENDDO
     ENDDO
   ENDDO
   VolXYZmax(:,:) = VolXYZmax(:,:) + 20.D0*EPSBOO
   VolXYZmin(:,:) = VolXYZmin(:,:) - 20.D0*EPSBOO
!#############################################################
   IF( IdebugBOO >= 1 ) THEN
     PRINT 7771,(VolXYZmin(1,I),VolXYZmax(1,I),I=1,2)
7771 FORMAT(/' Vol X min max =',2F12.4,5X,2F12.4)
     PRINT 7772,(VolXYZmin(2,I),VolXYZmax(2,I),I=1,2)
7772 FORMAT(/' Vol Y min max =',2F12.4,5X,2F12.4)
     PRINT 7773,(VolXYZmin(3,I),VolXYZmax(3,I),I=1,2)
7773 FORMAT(/' Vol Z min max =',2F12.4,5X,2F12.4)
   ENDIF
!#############################################################
!
!>> Compute  Is_outside(ip)  which tells whether plane ip
!>> is completely outside the other volume  or not
   Is_outside(:)=.FALSE.
   DO IV=1,2
     IoV = 3 - IV
     DO IP=Nplan(1,IV),Nplan(2,IV)
       Lout(:)=.TRUE.
       DO IE=Nedge(1,IP),Nedge(2,IP)
         WHERE( Ptxyz(1:3,IE) > VolXYZmin(1:3,IoV) ) Lout(1:3)=.FALSE.
         WHERE( Ptxyz(1:3,IE) < VolXYZmax(1:3,IoV) ) Lout(4:6)=.FALSE.
       ENDDO
       Is_outside(IP) = ANY( Lout )
!#############################################################
       IF( IdebugBOO >= 1 ) THEN
         IF( Is_outside(IP) ) THEN
           PRINT 7774,IV,IP
7774       FORMAT(' Vol',I3,'  Plane',I4,'  IS outside !')
         ELSE
           PRINT 7775,IV,IP
7775       FORMAT(' Vol',I3,'  Plane',I4,'  is NOT outside !')
         ENDIF
       ENDIF
!#############################################################
     ENDDO
   ENDDO
!
!>> Create new volume 4 (=2) that might later be slightly displaced
   Nplan(1,4) = Nplan_max + 1
   DO IP=Nplan(1,2),Nplan(2,2)
     CALL Bool_Incr(Nplan_max,Ndim_plan,'Ndim_plan',Ioverf)
     IF( Ioverf /= 0 ) THEN
       Ioverf = 2
       RETURN
     ENDIF
     Icolo(Nplan_max)      = Icolo(IP)
     Is_outside(Nplan_max) = Is_outside(IP)
     Nedge(1,Nplan_max)    = Nedge_max + 1
     DO IE=Nedge(1,IP),Nedge(2,IP)
       CALL Bool_Incr(Nedge_max,Ndim_edge,'Ndim_edge',Ioverf)
       IF( Ioverf /= 0 ) THEN
         Ioverf = 3
         RETURN
       ENDIF
       Ptxyz(1:3,Nedge_max) = Ptxyz(1:3,IE)
       Iseen(    Nedge_max) = Iseen(    IE)
     ENDDO
     Nedge(2,Nplan_max) = Nedge_max
   ENDDO
   Nplan(2,4) = Nplan_max
!
!=======================================================================
   IVA = 1
   IVB = 4
   ITRY = 0
   Nzer = 0
10 ITRY = ITRY + 1
   ShiftXYZ = (4.D0+ITRY**3) * EPSBOO
!
!>> Loop on the planes IPA of volume A (= 1)
!>> Compute orthonormed reference system of plane IPA
!>> Loop on the planes IPB of volume B (= 4)
!>> Compute coordinates of the edges of plane IPB in the IPA ref. syst.
!>> Find out whether >= 3 of these edges has a z=0 coordinate in this ref. syst.
!>> (i.e. whether IPA and IPB are coplanar)
!>> If yes, slightly stretch volume B (= 4)
   Nstretch = 0
   DO IPA=Nplan(1,IVA),Nplan(2,IVA)
     IF( Is_outside(IPA) ) CYCLE
     CALL Bool_Ort_Nor_Sys(Ndim_gon,Nedge(2,IPA)-Nedge(1,IPA)+1,Ptxyz(1,Nedge(1,IPA)),Sys)
     DO IPB=Nplan(1,IVB),Nplan(2,IVB)
       IF( Is_outside(IPB) ) CYCLE
       NB = Nedge(2,IPB)-Nedge(1,IPB)+1
       CALL Bool_Coo_Edg_Sys(Ndim_gon,NB,Ptxyz(1,Nedge(1,IPB)),Sys,Pcoo)
       Nzer = 0
       DO I=1,NB
         I1 = MOD(I,NB) + 1
         IF( ABS(Pcoo(I,3)) > 1.1D0*EPSBOO ) CYCLE
         Nzer = Nzer + 1
       ENDDO
       IF( Nzer < 3 ) CYCLE
       Nstretch = Nstretch + 1
       Pco3Tot = 0.D0
       DO IPB2=Nplan(1,IVB),Nplan(2,IVB)
         IF( IPB2 == IPB ) CYCLE
         NB = Nedge(2,IPB2)-Nedge(1,IPB2)+1
         CALL Bool_Coo_Edg_Sys(Ndim_gon,NB,Ptxyz(1,Nedge(1,IPB2)),Sys,Pcoo)
         Pco3Tot = Pco3Tot + SUM(Pcoo(1:NB,3))
       ENDDO
       Stretch = - SIGN( ShiftXYZ , Pco3Tot )
       DO IE=Nedge(1,IPB),Nedge(2,IPB)
         PtoMove(1:3) = Ptxyz(1:3,IE)
         DO IPB2=Nplan(1,IVB),Nplan(2,IVB)
           DO IE2=Nedge(1,IPB2),Nedge(2,IPB2)
             PtDiff(1:3) = ABS( Ptxyz(1:3,IE2) - PtoMove(1:3) )
             IF( MAXVAL(PtDiff(1:3)) > 1.1D0*EPSBOO ) CYCLE
             Ptxyz(1:3,IE2) = Ptxyz(1:3,IE2) + Stretch*Sys(1:3,3)
           ENDDO
         ENDDO
       ENDDO
     ENDDO
   ENDDO
!
   IF( Nstretch > 0 .AND. ITRY > 1 ) THEN
     PRINT 7777,ITRY,Nstretch
7777 FORMAT(' In Bool_Prep_Vol - Stretch :  Itry,Nstretch =',2I5)
     IproblemBOO = IproblemBOO + 1
   ENDIF
   IF( Nstretch > 0 .AND. ITRY < 2 ) GO TO 10
!=======================================================================
!
!
!=======================================================================
   ITRY = 0
   Jxyz = 0
15 ITRY = ITRY + 1
   ShiftXYZ = (4.D0+ITRY**2) * EPSBOO
!
   IF( ITRY > 20 ) RETURN
!
   DO 20 I12AB=1,2
!>> IVA,IVB  =  1,4  or  4,1
     IVA = 3*I12AB - 2
     IVB = 7 - 3*I12AB
!
!>> Loop on the planes IPA of volume A (= 1 or 4)
!>> Compute orthonormed reference system of plane IPA
!>> Loop on the planes IPB of volume B (= 4 or 1)
!>> Compute coordinates of the edges of plane IPB in the IPA ref. syst.
!>> Find out whether >= 1 of these edges has a z=0 coordinate in this ref. syst.
!>> or, whether an intersection of an IPB edge line with the IPA plane 
!>> has a x=0 or y=0 coordinate in this ref. syst.
!>> If yes, slightly move volume 4 and go back
     DO IPA=Nplan(1,IVA),Nplan(2,IVA)
       IF( Is_outside(IPA) ) CYCLE
       CALL Bool_Ort_Nor_Sys(Ndim_gon,Nedge(2,IPA)-Nedge(1,IPA)+1,Ptxyz(1,Nedge(1,IPA)),Sys)
       DO IPB=Nplan(1,IVB),Nplan(2,IVB)
         IF( Is_outside(IPB) ) CYCLE
         NB = Nedge(2,IPB)-Nedge(1,IPB)+1
         CALL Bool_Coo_Edg_Sys(Ndim_gon,NB,Ptxyz(1,Nedge(1,IPB)),Sys,Pcoo)
         DO I=1,NB
           I1 = MOD(I,NB) + 1
           Jxyz = 0
           IF( ABS(Pcoo(I,3)) < 1.1D0*EPSBOO ) THEN
             Jxyz = 3
           ELSEIF( Pcoo(I,3)*Pcoo(I1,3) < 0.D0 ) THEN
             Ez = Pcoo(I,3)/(Pcoo(I1,3)-Pcoo(I,3))
             XYloc(1:2) = Pcoo(I,1:2) - (Pcoo(I1,1:2)-Pcoo(I,1:2)) * Ez
             IF( ABS(XYloc(1)) < 1.1D0*EPSBOO ) Jxyz = 1
             IF( ABS(XYloc(2)) < 1.1D0*EPSBOO ) Jxyz = 2
           ENDIF
           IF( Jxyz < 1 ) CYCLE
           DO IP=Nplan(1,4),Nplan(2,4)
             DO IE=Nedge(1,IP),Nedge(2,IP)
               Ptxyz(1:3,IE) = Ptxyz(1:3,IE) + ShiftXYZ*Sys(1:3,Jxyz)
             ENDDO
           ENDDO
           IF( ITRY > 4 ) THEN
             PRINT 7778,ITRY,Jxyz
7778         FORMAT(' In Bool_Prep_Vol - Displace :  Itry,Jxyz =',2I5)
             IproblemBOO = IproblemBOO + 1
           ENDIF
           GO TO 15
         ENDDO
       ENDDO
     ENDDO
!
20 ENDDO
!=======================================================================
 END SUBROUTINE Bool_Prep_Vol
!
