!
!> Compute the intersection or the cut-out or the union
!> of the polygon made of the     JPOL(0)  points   VPOL(i,1:2)
!> with the polygon made of the   JCUT(0)  points   VCUT(i,1:2)
!> and give the result as
!> LRR polygons made of the     JRR(0,LRR) points    VRR(i,1:2,LRR)
!> \author SaMuSoG
!
 SUBROUTINE Bool_Cut_XX( IPM1A,IPM1B, Ndim_poly,Ndim_gon,Ndim_cut,  &
                         JPOL,VPOL, JCUT,VCUT, Ioverf, LRR,JRR,VRR )
 USE M_MB_General_CONSTANTS, ONLY : IdebugBOO, EPSBOO
 IMPLICIT NONE
 INTEGER :: IPM1A,IPM1B
 INTEGER :: Ndim_poly,Ndim_gon,Ndim_cut,Ioverf
 INTEGER :: JPOL(0:Ndim_gon  ), JCUT(0:Ndim_gon  )
 REAL(8) :: VPOL(  Ndim_gon,2), VCUT(  Ndim_gon,2)
 INTEGER :: LRR
 INTEGER :: JRR(0:Ndim_gon,  Ndim_poly)
 REAL(8) :: VRR(  Ndim_gon,2,Ndim_poly)
 LOGICAL :: INSIDE
 INTEGER :: I,J,L,ILIMIT,NE,IP1,IP2,INSI,Nt
 INTEGER :: IELIM,I1,I2,L1,L2,NCO,ITRY,LINTERN_MAX
 INTEGER :: L1_MIN,L2_MIN,I1_MIN,I2_MIN,NNEW,LNEW
 REAL(8) :: Xcur,Ycur,VX,VY,XEX,YEX,DDD,DD_MIN
 REAL(8) :: DpolXY(2),DcutXY(2)
 INTEGER :: LINTERN(Ndim_poly)
 REAL(8) :: EE(0:Ndim_cut)
 INTEGER :: Nceg,NcegNEW
 INTEGER :: Iceg(    Ndim_cut)
 REAL(8) :: Vceg(2,2,Ndim_cut)
!
!>> Store in  Vceg  all the
!>> VPOL (sub-)segments that are inside (IPM1A=+1)
!>>                          or outside (IPM1A=-1) the VCUT polygon
!>> and the
!>> VCUT (sub-)segments that are inside (IPM1B=+1)
!>>                          or outside (IPM1B=-1) the VPOL polygon
!######################################################################################
   IF( IdebugBOO >= 1 ) PRINT 4440,IPM1A,IPM1B,JPOL(0),JCUT(0)
4440  FORMAT(' Debut Bool_Cut_XX : IPM1A,IPM1B,NPOL,NCUT =',4I4)
!######################################################################################
   EE(0)  = 0.D0
   Nceg   = 0
   ILIMIT = 0
!
   DO I=1,JPOL(0)
     IF( JPOL(I) >= 1 ) THEN
       IP1 = MOD(I,JPOL(0)) + 1
       CALL Bool_Sub_Seg( JCUT(0),VCUT(1,1),VCUT(1,2),VPOL( I ,1),VPOL( I ,2),  &
                          VPOL(IP1,1),VPOL(IP1,2), Ndim_cut-1, Ioverf,  NE,EE(1))
       IF( Ioverf /= 0 ) RETURN
       NE = NE + 1
       EE(NE) = 1.D0
       DpolXY(1:2) = VPOL(IP1,1:2) - VPOL(I,1:2)
       DO J=1,NE
         Xcur = VPOL(I,1) + (EE(J-1)+EE(J)) * DpolXY(1) / 2.D0
         Ycur = VPOL(I,2) + (EE(J-1)+EE(J)) * DpolXY(2) / 2.D0
         CALL Bool_Is_It_In(Xcur,Ycur,JCUT(0),VCUT(1,1),VCUT(1,2),INSI)
         IF( INSI == IPM1A ) THEN
           CALL Bool_Incr(Nceg,Ndim_cut,'Ndim_cut',Ioverf)
           IF( Ioverf /= 0 ) GO TO 96
           Iceg(      Nceg) = INSI
           Vceg(1,1:2,Nceg) = VPOL(I,1:2) + EE(J-1) * DpolXY(1:2)
           Vceg(2,1:2,Nceg) = VPOL(I,1:2) + EE(J  ) * DpolXY(1:2)
!######################################################################################
           IF( IdebugBOO >= 1 )  PRINT 4442,Nceg,Iceg(Nceg),Vceg(1,1:2,Nceg),Vceg(2,1:2,Nceg)
4442       FORMAT(' POL ---> CEG ',2I5,5X,2F9.2,5X,2F9.2)
!######################################################################################
         ENDIF
       ENDDO
     ENDIF
   ENDDO
!
   DO I=1,JCUT(0)
     IF( JCUT(I) >= 1 ) THEN
       IP1 = MOD(I,JCUT(0)) + 1
       CALL Bool_Sub_Seg( JPOL(0),VPOL(1,1),VPOL(1,2),VCUT( I ,1),VCUT( I ,2),  &
                          VCUT(IP1,1),VCUT(IP1,2), Ndim_cut-1, Ioverf,  NE,EE(1))
       IF( Ioverf /= 0 ) RETURN
       NE = NE + 1
       EE(NE) = 1.D0
       DcutXY(1:2) = VCUT(IP1,1:2) - VCUT(I,1:2)
       DO J=1,NE
         Xcur = VCUT(I,1) + (EE(J-1)+EE(J)) * DcutXY(1) / 2.D0
         Ycur = VCUT(I,2) + (EE(J-1)+EE(J)) * DcutXY(2) / 2.D0
         CALL Bool_Is_It_In(Xcur,Ycur,JPOL(0),VPOL(1,1),VPOL(1,2),INSI)
         IF( INSI == IPM1B .OR. INSI == 0 ) THEN
           IF( INSI == 0 ) ILIMIT = ILIMIT + 1
           CALL Bool_Incr(Nceg,Ndim_cut,'Ndim_cut',Ioverf)
           IF( Ioverf /= 0 ) GO TO 96
           Iceg(  Nceg) = INSI
           Vceg(1,1:2,Nceg) = VCUT(I,1:2) + EE(J-1) * DcutXY(1:2)
           Vceg(2,1:2,Nceg) = VCUT(I,1:2) + EE(J  ) * DcutXY(1:2)
!######################################################################################
           IF( IdebugBOO >= 1 )  PRINT 4443,Nceg,Iceg(Nceg),Vceg(1,1:2,Nceg),Vceg(2,1:2,Nceg)
4443       FORMAT(' CUT ---> CEG ',2I5,5X,2F9.2,5X,2F9.2)
!######################################################################################
         ENDIF
       ENDDO
     ENDIF
   ENDDO
!
!>> In cases where  POL and CUT (sub-)segments overlap (ILIMIT>0),
!>> remove (sub-)segments that cannot be connected to others on both sides
   IF( ILIMIT >= 1 ) THEN
80   IELIM = 0
     DO I1=1,Nceg
       IF( Iceg(I1) /= 0 ) CYCLE
       DO J=1,2
         NCO = 0
         DO I2=1,Nceg
           IF( I2 /= I1 .AND. Iceg(I2) >= -1 ) THEN
             IF(  ABS(Vceg(J,1,I1)-Vceg(1,1,I2)) < EPSBOO .AND.  &
                  ABS(Vceg(J,2,I1)-Vceg(1,2,I2)) < EPSBOO ) NCO=NCO+1
             IF(  ABS(Vceg(J,1,I1)-Vceg(2,1,I2)) < EPSBOO .AND.  &
                  ABS(Vceg(J,2,I1)-Vceg(2,2,I2)) < EPSBOO ) NCO=NCO+1
           ENDIF
         ENDDO
         IF( MOD(NCO,2) == 0 ) THEN
           IELIM = IELIM + 1
           Iceg(I1) = -9
           CYCLE
         ENDIF
       ENDDO
     ENDDO
     IF( IELIM >= 1 ) THEN
       NcegNEW = 0
       DO I=1,Nceg
         IF( Iceg(I) >= -1 ) THEN
           NcegNEW = NcegNEW + 1
           Iceg(    NcegNEW) = Iceg(    I)
           Vceg(:,:,NcegNEW) = Vceg(:,:,I)
!######################################################################################
         ELSE
           IF( IdebugBOO >= 1 )  PRINT 4444,I,Iceg(I),Vceg(1,1:2,I),Vceg(2,1:2,I)
4444       FORMAT(' ---> Eliminate CEG ',2I5,5X,2F9.2,5X,2F9.2)
!######################################################################################
         ENDIF
       ENDDO
       Nceg = NcegNEW
       GO TO 80
     ENDIF
   ENDIF
!
!>> From the  Vceg  segments, construct  LRR  polygons  VRR
   Iceg(1:Nceg) = 0
   DO I=1,Nceg
     IF(Iceg(I) >= 1) CYCLE
     Iceg(I) = 1
     CALL Bool_Incr(LRR,Ndim_poly,'Ndim_poly',Ioverf)
     IF( Ioverf /= 0 ) GO TO 94
     Nt = 2
     JRR(  0,    LRR) = Nt
     JRR(1:2,    LRR) = 1
     VRR(1:2,1:2,LRR) = Vceg(1:2,1:2,I)
     DO ITRY=1,1000
       DO J=I,Nceg
         IF( Iceg(J) >= 1 ) CYCLE
         IF( ABS(VRR(Nt,1,LRR)-Vceg(1,1,J)) < EPSBOO .AND.  &
             ABS(VRR(Nt,2,LRR)-Vceg(1,2,J)) < EPSBOO ) THEN
           Iceg(J) = 1
           JRR(Nt,      LRR) = 1
           VRR(Nt+1,1:2,LRR) = Vceg(2,1:2,J)
           IF( ABS(VRR(1,1,LRR)-Vceg(2,1,J)) < EPSBOO .AND.  &
               ABS(VRR(1,2,LRR)-Vceg(2,2,J)) < EPSBOO ) GO TO 170
           CALL Bool_Incr(Nt,Ndim_gon,'Ndim_gon',Ioverf)
           IF( Ioverf /= 0 ) GO TO 95
         ELSEIF( ABS(VRR(Nt,1,LRR)-Vceg(2,1,J)) < EPSBOO .AND.  &
                 ABS(VRR(Nt,2,LRR)-Vceg(2,2,J)) < EPSBOO ) THEN
           Iceg(J) = 1
           JRR(Nt,      LRR) = 1
           VRR(Nt+1,1:2,LRR) = Vceg(1,1:2,J)
           IF( ABS(VRR(1,1,LRR)-Vceg(1,1,J)) < EPSBOO .AND.  &
               ABS(VRR(1,2,LRR)-Vceg(1,2,J)) < EPSBOO ) GO TO 170
           CALL Bool_Incr(Nt,Ndim_gon,'Ndim_gon',Ioverf)
           IF( Ioverf /= 0 ) GO TO 95
         ENDIF
       ENDDO
     ENDDO
170  JRR(0,LRR) = Nt
   ENDDO
!######################################################################################
       IF( IdebugBOO >= 1 )  PRINT 4447,LRR,(JRR(0,L),L=1,LRR)
4447   FORMAT(' LRR,JRR(0,i) =',I5,3X,30I5)
!######################################################################################
!
!>> Merge consecutive RR segments that are aligned
   IF( ILIMIT >= 1 ) THEN
     DO L=1,LRR
       DO I=1,JRR(0,L)
         IP1 = MOD(I  ,JRR(0,L)) + 1
         IP2 = MOD(IP1,JRR(0,L)) + 1
         VX = VRR(IP1,1,L) - VRR(I,1,L)
         VY = VRR(IP1,2,L) - VRR(I,2,L)
         IF( ABS(VX) > ABS(VY) ) THEN
           YEX = VRR(I,2,L) + (VRR(IP2,1,L)-VRR(I,1,L)) * VY / VX
           IF( ABS(VRR(IP2,2,L)-YEX) < EPSBOO ) JRR(IP1,L) = 0
         ELSE
           XEX = VRR(I,1,L) + (VRR(IP2,2,L)-VRR(I,2,L)) * VX / VY
           IF( ABS(VRR(IP2,1,L)-XEX) < EPSBOO ) JRR(IP1,L) = 0
         ENDIF
       ENDDO
       NNEW = 0
       DO I=1,JRR(0,L)
         IF( JRR(I,L) /= 0 ) THEN
           NNEW = NNEW + 1
           JRR(NNEW,L)     = JRR(I,L)
           VRR(NNEW,1:2,L) = VRR(I,1:2,L)
!######################################################################################
         ELSE
           IF( IdebugBOO >= 1 )  PRINT 4445,VRR(I,1:2,L)
4445       FORMAT(' ---> Eliminate point',2F9.2)
!######################################################################################
         ENDIF
       ENDDO
       JRR(0,L) = NNEW
     ENDDO
   ENDIF
!
!>> Find out wich polygons are inside each others
20 LINTERN_MAX = 0
   DO L1=1,LRR
     LINTERN(L1) = 1
     DO L2=1,LRR
       IF( L2 /= L1 ) THEN
         IF( INSIDE(VRR(1,1,L1),VRR(1,2,L1),JRR(0,L2),VRR(1,1,L2),VRR(1,2,L2)) )  &
             LINTERN(L1) = LINTERN(L1) + 1
       ENDIF
     ENDDO
     LINTERN_MAX = MAX( LINTERN_MAX , LINTERN(L1) )
   ENDDO
   IF( MOD(LINTERN_MAX,2) /= 0 ) LINTERN_MAX = LINTERN_MAX -1
!
!#################################################################################
   IF( IdebugBOO >= 1 ) THEN
     PRINT 6661,LRR,LINTERN_MAX
6661 FORMAT(/' LRR,LINTERN_MAX ',2I6)
     PRINT 6662,(L,JRR(0,L),LINTERN(L),L=1,LRR)
6662 FORMAT(' i,JRR(0,i),LINTERN(i) ',3I6)
     IF( LINTERN_MAX <= 0 ) PRINT 6668
6668 FORMAT(' End Bool_Cut_XX !!!')
   ENDIF
!#################################################################################
   IF( LINTERN_MAX <= 0 ) RETURN
!
   DD_MIN = 999999999.D0
   L1_MIN = 0
   L2_MIN = 0
   I1_MIN = 0
   I2_MIN = 0
   DO L1=1,LRR
     IF( LINTERN(L1) == LINTERN_MAX ) THEN
       DO L2=1,LRR
         IF( L2 /= L1 .AND. LINTERN(L2) == LINTERN_MAX-1 ) THEN
           IF( INSIDE(VRR(1,1,L1),VRR(1,2,L1),JRR(0,L2),VRR(1,1,L2),VRR(1,2,L2)) ) THEN
             CALL Bool_Closest_Pt(JRR(0,L2),VRR(1,1,L2),VRR(1,2,L2),  &
                                  JRR(0,L1),VRR(1,1,L1),VRR(1,2,L1), I2,I1)
             DDD = (VRR(I2,1,L2)-VRR(I1,1,L1))**2 + (VRR(I2,2,L2)-VRR(I1,2,L1))**2
             IF( DDD < DD_MIN ) THEN
               DD_MIN = DDD
               L1_MIN = L1
               L2_MIN = L2
               I1_MIN = I1
               I2_MIN = I2
             ENDIF
           ENDIF
         ENDIF
       ENDDO
     ENDIF
   ENDDO
!-----------------------------------------------------------
   IF( L1_MIN <= 0 ) THEN
     PRINT 7771,IPM1A,IPM1B
7771 FORMAT(/' In Bool_Cut_XX with Ipm1ab =',2I3,' STOP !!!')
     STOP
   ENDIF
!-----------------------------------------------------------
!#################################################################################
!**      PRINT 6663,L1_MIN,L2_MIN,I1_MIN,I2_MIN
!**6663  FORMAT(' L1_MIN,L2_MIN,I1_MIN,I2_MIN ',4I6)
!#################################################################################
!
!>> Polygon L1_MIN is directly inside polygon L2_MIN
!>> ===> merge them in polygon LRR !
   CALL Bool_Incr(LRR,Ndim_poly,'Ndim_poly',Ioverf)
   IF( Ioverf /= 0 ) GO TO 94
   Nt = 1
   JRR(Nt,    LRR)  = 0
   VRR(Nt,1:2,LRR)  = VRR(I2_MIN,1:2,L2_MIN)
   DO J=I1_MIN,JRR(0,L1_MIN)+I1_MIN-1
     I = MOD(J-1,JRR(0,L1_MIN)) + 1
     CALL Bool_Incr(Nt,Ndim_gon,'Ndim_gon',Ioverf)
     IF( Ioverf /= 0 ) GO TO 95
     JRR(Nt,    LRR) = JRR(   I  ,    L1_MIN)
     VRR(Nt,1:2,LRR) = VRR(   I  ,1:2,L1_MIN)
   ENDDO
   CALL Bool_Incr(Nt,Ndim_gon,'Ndim_gon',Ioverf)
   IF( Ioverf /= 0 ) GO TO 95
   JRR(Nt,    LRR)  = 0
   VRR(Nt,1:2,LRR)  = VRR(I1_MIN,1:2,L1_MIN)
   DO J=I2_MIN,JRR(0,L2_MIN)+I2_MIN-1
     I = MOD(J-1,JRR(0,L2_MIN)) + 1
     CALL Bool_Incr(Nt,Ndim_gon,'Ndim_gon',Ioverf)
     IF( Ioverf /= 0 ) GO TO 95
     JRR(Nt,    LRR) = JRR(   I  ,    L2_MIN)
     VRR(Nt,1:2,LRR) = VRR(   I  ,1:2,L2_MIN)
   ENDDO
   JRR(0,LRR) = Nt
!
!>> Then, eliminate polygons L1_MIN and L2_MIN and go back
   LNEW = 0
   DO L=1,LRR
     IF( L == L1_MIN .OR. L == L2_MIN ) CYCLE
     LNEW = LNEW + 1
     IF( LNEW /= L ) THEN
       JRR(:,  LNEW) = JRR(:,  L)
       VRR(:,:,LNEW) = VRR(:,:,L)
     ENDIF
   ENDDO
   LRR = LNEW
   GO TO 20
!
94 Ioverf = 4
   RETURN
!
95 Ioverf = 5
   RETURN
!
96 Ioverf = 6
   RETURN
!
 END SUBROUTINE Bool_Cut_XX
!
