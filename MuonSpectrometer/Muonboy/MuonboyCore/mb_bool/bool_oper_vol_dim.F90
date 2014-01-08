!
!> \author SaMuSoG
!
 SUBROUTINE Bool_Oper_Vol_Dim( Ndim_volu,Ndim_plan,Ndim_edge,  Ndim_poly,Ndim_gon,  Ioverf,  &
                               Jbool1,Jbool2,  Jcolor,  Nplan,Icolo,Nedge,Ptxyz,Iseen )
 USE M_MB_General_CONSTANTS, ONLY : IdebugBOO
 IMPLICIT NONE
 INTEGER :: Ndim_volu,Ndim_plan,Ndim_edge,Ndim_poly,Ndim_gon
 INTEGER :: Ioverf,Jbool1,Jbool2, Jcolor
 INTEGER :: Nplan(2,Ndim_volu)
 INTEGER :: Icolo(  Ndim_plan)
 INTEGER :: Nedge(2,Ndim_plan)
 REAL(8) :: Ptxyz(3,Ndim_edge)
 INTEGER :: Iseen(  Ndim_edge)
 LOGICAL :: INSIDE
 INTEGER :: Ntest,Nplan_max,Nedge_max
 INTEGER :: I,J,L,N,Jcolor_Def,I12AB,I1,IVA,IVB,IPA,IPB
 INTEGER :: L1,L2,LA,LB,LC,LZ,INTLEV,NA,NB
 LOGICAL :: Is_outside(Ndim_plan)
 REAL(8) :: X,Y
 REAL(8) :: PAA(Ndim_gon,3),PBA(Ndim_gon,3)
 REAL(8) :: SYSA(3,0:3)
 INTEGER :: Npoi,Nseg
 REAL(8) :: Vpoi(Ndim_gon,2)
 REAL(8) :: Vseg(Ndim_gon,2,2),VsegMin(2),VsegMax(2)
 INTEGER :: NORDER(Ndim_gon)
 INTEGER :: INTERNALB_MAX
 INTEGER :: INTERNALB(Ndim_poly)
 INTEGER :: JBOOL(2)
 INTEGER ::                     LPOLYA,LPOLYB,LPOLYC,LPOLYX,LPOLYZ
 INTEGER, DIMENSION( 0:Ndim_gon,    Ndim_poly ) :: JPOLYA, JPOLYB, JPOLYC, JPOLYX, JPOLYZ
 REAL(8), DIMENSION(   Ndim_gon, 2, Ndim_poly ) :: VPOLYA, VPOLYB, VPOLYC, VPOLYX, VPOLYZ
 REAL(8), DIMENSION(2) :: VPOLYAMin,VPOLYAMax
!
   JBOOL(1) = Jbool1
   JBOOL(2) = Jbool2
!
   JPOLYA(:,:) = 0
   JPOLYB(:,:) = 0
   JPOLYC(:,:) = 0
   VPOLYA(:,:,:) = 0.d0
   VPOLYB(:,:,:) = 0.d0
   VPOLYC(:,:,:) = 0.d0
!
!>> Check wether there is enough space (Ndim_volu >= 4)
   Ntest = 3
   CALL Bool_Incr(Ntest,Ndim_volu,'Ndim_volu',Ioverf)
   IF( Ioverf /= 0 ) GO TO 91
!
!>> Get default color
   IF( Jcolor == -2 ) THEN
     Jcolor_Def = Icolo(Nplan(1,2))
   ELSEIF( Jcolor == -1 ) THEN
     Jcolor_Def = Icolo(Nplan(1,1))
   ELSE
     Jcolor_Def = MAX( 1 , MIN( 9 , Jcolor ) )
   ENDIF
!
!>> Create temporary volume 4 which is a copy of volume 2
!>> possibly very slightly displaced to avoid problems
   CALL Bool_Prep_Vol( Ndim_volu,Ndim_plan,Ndim_edge,Ndim_gon,Ioverf,   &
                       Nplan,Icolo,Nedge,Ptxyz,Iseen, Is_outside,Nplan_max,Nedge_max )
   IF( Ioverf /= 0 ) RETURN
!
!>> Initialise volume 3
   Nplan(1,3) = Nplan_max + 1
   Nplan(2,3) = Nplan_max
!
!=======================================================================
   DO I12AB=1,2
!>> IVA,IVB  =  1,4  or  4,1
     IVA = 3*I12AB - 2
     IVB = 7 - 3*I12AB
!
!>> Loop on the planes of volume A
b300: DO 300 IPA=Nplan(1,IVA),Nplan(2,IVA)
!#############################################################
       IF( IdebugBOO >= 1 ) THEN
         PRINT 7776,I12AB,JBOOL(I12AB),IPA
7776     FORMAT('------------------------------------------------'/' I12AB,JBOOL(I12AB),IPA =',2I4,I6)
         IF( Is_outside(IPA) ) PRINT 7777
7777     FORMAT('  Is Outside !!!!')
       ENDIF
!#############################################################
       IF( Is_outside(IPA) .AND. JBOOL(I12AB) == 0 ) CYCLE
       NA = Nedge(2,IPA) - Nedge(1,IPA)
       CALL Bool_Incr(NA,Ndim_gon,'Ndim_gon',Ioverf)
       IF( Ioverf /= 0 ) GO TO 95
!>> Find orthonormed reference system (SYSA) of plane IPA of volume A
       CALL Bool_Ort_Nor_Sys(Ndim_gon,NA,Ptxyz(1,Nedge(1,IPA)), SYSA)
!>> Find coordinates PAA of the edges of plane IPA in this SYSA ref. syst.
       CALL Bool_Coo_Edg_Sys(Ndim_gon,NA,Ptxyz(1,Nedge(1,IPA)),SYSA, PAA)
!>> With all these edges of plane IPA, construct one polygon A
       LPOLYA             = 1
       JPOLYA(  0 ,    1) = NA
       JPOLYA(1:NA,    1) = Iseen(Nedge(1,IPA):Nedge(2,IPA))
       VPOLYA(1:NA,1:2,1) = PAA(1:NA,1:2)
       IF( Is_outside(IPA) ) GO TO 270
       DO J=1,2
         VPOLYAMin(J) = MINVAL( PAA(1:NA,J) )
         VPOLYAMax(J) = MAXVAL( PAA(1:NA,J) )
       ENDDO
!
!>> Find all the segments from intersection of volume B with plane IPA
       Nseg = 0
       VsegMax(1:2) = -999999999.D0
       VsegMin(1:2) =  999999999.D0
!>> Loop on the planes of volume B
       DO 150 IPB=Nplan(1,IVB),Nplan(2,IVB)
         NB = Nedge(2,IPB) - Nedge(1,IPB) + 1
!>> Find coordinates PBA of the edges of plane IPB in the SYSA ref. syst.
         CALL Bool_Coo_Edg_Sys(Ndim_gon,NB,Ptxyz(1,Nedge(1,IPB)),SYSA, PBA)
!>> Find intersecting points of segments of plane IPB with the plane IPA
         Npoi = 0
         DO I=1,NB
           IF( Iseen(Nedge(1,IPB)-1+I) <= 0 ) CYCLE
           I1 = MOD(I,NB) + 1
           IF( PBA(I,3)*PBA(I1,3) >= 0.D0 )   CYCLE
           CALL Bool_Incr(Npoi,Ndim_gon,'Ndim_gon',Ioverf)
           IF( Ioverf /= 0 ) GO TO 95
           Vpoi(Npoi,1:2) = PBA(I,1:2) - (PBA(I1,1:2)-PBA(I,1:2)) * PBA(I,3)/(PBA(I1,3)-PBA(I,3))
           VsegMin(1:2) = MIN( VsegMin(1:2) , Vpoi(Npoi,1:2) )
           VsegMax(1:2) = MAX( VsegMax(1:2) , Vpoi(Npoi,1:2) )
         ENDDO
!>> With these points, construct intersecting segments in the plane IPA
         IF( Npoi <= 0 ) CYCLE
!############################################################################
         IF( IdebugBOO >= 1 )  PRINT 8881,(L,Vpoi(L,1:2),L=1,Npoi)
8881     FORMAT(' POI',5(I6,2F9.2))
!############################################################################
         J = 1
         IF(ABS(Vpoi(2,1)-Vpoi(1,1)) < ABS(Vpoi(2,2)-Vpoi(1,2))) J = 2
         CALL CROISS(Vpoi(1,J),Npoi, NORDER)
         DO I=1,Npoi-1,2
           CALL Bool_Incr(Nseg,Ndim_gon,'Ndim_gon',Ioverf)
           IF( Ioverf /= 0 ) GO TO 95
           Vseg(Nseg,1,1:2) = Vpoi(NORDER(I  ),1:2)
           Vseg(Nseg,2,1:2) = Vpoi(NORDER(I+1),1:2)
!############################################################################
           IF( IdebugBOO >= 1 )  PRINT 8882,Nseg,Vseg(Nseg,1,1:2),Vseg(Nseg,2,1:2)
8882       FORMAT(' SEG',I6,4F9.2)
!############################################################################
         ENDDO
150    ENDDO
!############################################################################
       IF( IdebugBOO >= 1 )  PRINT 8883,VsegMin(1:2),VsegMax(1:2),VPOLYAMin(1:2),VPOLYAMax(1:2)
8883   FORMAT(' SEGMin   ',2F9.2/' SEGMax   ',2F9.2/'  A Min   ',2F9.2/'  A Max   ',2F9.2)
!############################################################################
       DO J=1,2
         IF( VsegMin(J) <= VPOLYAMax(J) .AND. VsegMax(J) >= VPOLYAMin(J) ) CYCLE
         IF( JBOOL(I12AB) == 0 ) CYCLE b300
         GO TO 270
       ENDDO
!
!>> With these Nseg segments, construct LPOLYB polygons B
       CALL Bool_Creat_Poly( Ndim_gon,Ndim_poly, Ioverf, Nseg,Vseg, LPOLYB,JPOLYB,VPOLYB )
       IF( Ioverf /= 0 ) RETURN
!############################################################################
       IF( IdebugBOO >= 1 ) THEN
         DO LA=1,LPOLYA
           CALL Bool_PanicPoly('A', LA, JPOLYA(0,LA),JPOLYA(1,LA),VPOLYA(1,1,LA),VPOLYA(1,2,LA) )
         ENDDO
         DO LB=1,LPOLYB
           CALL Bool_PanicPoly('B', LB, JPOLYB(0,LB),JPOLYB(1,LB),VPOLYB(1,1,LB),VPOLYB(1,2,LB) )
         ENDDO
       ENDIF
!############################################################################
!
!>> Find out wich polygons B are inside each others
       INTERNALB_MAX = 0
       DO L1=1,LPOLYB
         INTERNALB(L1) = 1
         DO L2=1,LPOLYB
           IF( L2 /= L1 ) THEN
             IF( INSIDE(VPOLYB(1,1,L1),VPOLYB(1,2,L1),JPOLYB(0,L2),VPOLYB(1,1,L2),VPOLYB(1,2,L2)) )  &
                INTERNALB(L1) = INTERNALB(L1) + 1
           ENDIF
         ENDDO
         INTERNALB_MAX = MAX( INTERNALB_MAX , INTERNALB(L1) )
       ENDDO
!############################################################################
       IF( IdebugBOO >= 1 )  PRINT 7778,INTERNALB_MAX,(INTERNALB(L),L=1,LPOLYB)
7778   FORMAT(' INTERNALB_MAX,INTERNALB =',I5,5X,20I5)
!############################################################################
!
!>> Cut out polygon(s) A with all these polygons B
       LPOLYZ               = LPOLYA
       JPOLYZ(:,  1:LPOLYA) = JPOLYA(:,  1:LPOLYA)
       VPOLYZ(:,:,1:LPOLYA) = VPOLYA(:,:,1:LPOLYA)
!--------------------------------------------------------------------
       IF( JBOOL(I12AB) == 0 ) LPOLYA = 0
       DO 201 INTLEV=1,INTERNALB_MAX
         DO 200 LB=1,LPOLYB
           IF( INTERNALB(LB) /= INTLEV ) CYCLE
           IF( MOD(INTLEV,2) == JBOOL(I12AB) ) THEN
             LPOLYX = 0
             DO 180 LA=1,LPOLYA
               CALL Bool_Cut_10(Ndim_poly,Ndim_gon,JPOLYA(0,LA),VPOLYA(1,1,LA),   &
                   JPOLYB(0,LB),VPOLYB(1,1,LB),Ioverf,LPOLYC, JPOLYC(0, 1),VPOLYC(1,1, 1))
               IF( Ioverf /= 0 ) RETURN
!############################################################################
               IF( IdebugBOO >= 1 ) THEN
                 PRINT 7779,INTLEV,LB,LA
7779             FORMAT(' INTLEV,LB,LA =',3I5)
                 DO LC=1,LPOLYC
                   CALL Bool_PanicPoly('C', LC, JPOLYC(0,LC),JPOLYC(1,LC),VPOLYC(1,1,LC),VPOLYC(1,2,LC))
                 ENDDO
               ENDIF
!############################################################################
               DO L=1,LPOLYC
                 CALL Bool_Incr(LPOLYX,Ndim_poly,'Ndim_poly',Ioverf)
                 IF( Ioverf /= 0 ) GO TO 94
                 JPOLYX(:,  LPOLYX) = JPOLYC(:,  L)
                 VPOLYX(:,:,LPOLYX) = VPOLYC(:,:,L)
               ENDDO
180          ENDDO
             LPOLYA               = LPOLYX
             JPOLYA(:,  1:LPOLYX) = JPOLYX(:,  1:LPOLYX)
             VPOLYA(:,:,1:LPOLYX) = VPOLYX(:,:,1:LPOLYX)
           ELSE
             DO 190 LZ=1,LPOLYZ
               CALL Bool_Cut_11(Ndim_poly,Ndim_gon,JPOLYZ(0,LZ),VPOLYZ(1,1,LZ),   &
                    JPOLYB(0,LB),VPOLYB(1,1,LB),Ioverf,LPOLYC, JPOLYC(0, 1),VPOLYC(1,1, 1))
               IF( Ioverf /= 0 ) RETURN
!############################################################################
               IF( IdebugBOO >= 1 ) THEN
                 PRINT 7780,INTLEV,LB,LZ
7780             FORMAT(' INTLEV,LB,LZ =',3I5)
                 DO LC=1,LPOLYC
                   CALL Bool_PanicPoly('C', LC, JPOLYC(0,LC),JPOLYC(1,LC),VPOLYC(1,1,LC),VPOLYC(1,2,LC) )
                 ENDDO
               ENDIF
!############################################################################
               DO L=1,LPOLYC
                 CALL Bool_Incr(LPOLYA,Ndim_poly,'Ndim_poly',Ioverf)
                 IF( Ioverf /= 0 ) GO TO 94
                 JPOLYA(:,  LPOLYA) = JPOLYC(:,  L)
                 VPOLYA(:,:,LPOLYA) = VPOLYC(:,:,L)
               ENDDO
190          ENDDO
           ENDIF
200      ENDDO
201    ENDDO
!--------------------------------------------------------------------
!
!>> Add the LPOLYA new planes to the volume being created (i.e. volume 3)
270    DO L=1,LPOLYA
         CALL Bool_Incr(Nplan_max,Ndim_plan,'Ndim_plan',Ioverf)
         IF( Ioverf /= 0 ) GO TO 92
         IF( Jcolor == 0 ) THEN
           Icolo(Nplan_max)   = Icolo(IPA)
         ELSE
           Icolo(Nplan_max)   = Jcolor_Def
         ENDIF
         Nedge( 1,Nplan_max)   = Nedge_max + 1
         DO N=1,JPOLYA(0,L)
           X = VPOLYA(N,1,L)
           Y = VPOLYA(N,2,L)
           CALL Bool_Incr(Nedge_max,Ndim_edge,'Ndim_edge',Ioverf)
           IF( Ioverf /= 0 ) GO TO 93
           Ptxyz(1:3,Nedge_max) = SYSA(1:3,0) + X*SYSA(1:3,1) + Y*SYSA(1:3,2)
           Iseen(    Nedge_max) = JPOLYA(N,L)
         ENDDO
         Nedge( 2,Nplan_max) = Nedge_max
       ENDDO
       Nplan(2,3) = Nplan_max
!
300  ENDDO b300
!
   ENDDO
!=======================================================================
!
   RETURN
!
91 Ioverf = 1
   RETURN
!
92 Ioverf = 2
   RETURN

93 Ioverf = 3
   RETURN
!
94 Ioverf = 4
   RETURN
!
95 Ioverf = 5
   RETURN
!
 END SUBROUTINE Bool_Oper_Vol_Dim
!
