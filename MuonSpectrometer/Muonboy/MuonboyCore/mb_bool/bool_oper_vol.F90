!
!> \author SaMuSoG
!
 SUBROUTINE Bool_Oper_Vol( Ndim_volu,Ndim_plan,Ndim_edge,  Ioverf,  &
                           Jbool1,Jbool2,  Jcolor,  Nplan,Icolo,Nedge,Ptxyz,Iseen )
!>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
!>>
!>> Create a geometrical volume (Volume_3) which is the result of a boolean
!>> operation between two given geometrical volumes (Volume_1 and Volume_2) :
!>>                  Volume_3  =  Volume_1 . Volume_2
!>>                                        |
!>>            Jbool1  Jbool2              V
!>>               1       1    =====>    Union
!>>               0       0    =====> Intersection
!>>               1       0    =====>  Subtraction (1-2)
!>>               0       1    =====>  Subtraction (2-1)
!>>
!>> Volume_i  is made of  Nplan(2,i)-Nplan(1,i)+1   Planar_polygons_j
!>>                                          (j = Nplan(1,i) to Nplan(2,i))
!>>
!>> Planar_polygon_j  of color  Icolo(j) ,
!>>           is made of  Nedge(2,j)-Nedge(1,j)+1       Edges_k
!>>                                          (k = Nedge(1,j) to Nedge(2,j))
!>>
!>> Edge_k  is at coordinates  x = Ptxyz(1,k)
!>>                            y = Ptxyz(2,k)
!>>                            z = Ptxyz(3,k)
!>>
!>> The line connecting  Edge_k  with  Edge_k+1  is seen if  Iseen(k)  is > 0
!>>
!>> Returned value of  Ioverf :  0  ===>  Dimentions are OK
!>>                              1  ===>  Dimention Ndim_volu is too small
!>>                              2  ===>  Dimention Ndim_plan is too small
!>>                              3  ===>  Dimention Ndim_edge is too small
!>>
!>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 USE M_MB_General_CONSTANTS, ONLY : IdebugBOO
 IMPLICIT NONE
 INTEGER :: Ndim_volu,Ndim_plan,Ndim_edge
 INTEGER :: Ioverf,Jbool1,Jbool2, Jcolor
 INTEGER :: Nplan(2,Ndim_volu)
 INTEGER :: Icolo(  Ndim_plan)
 INTEGER :: Nedge(2,Ndim_plan)
 REAL(8) :: Ptxyz(3,Ndim_edge)
 INTEGER :: Iseen(  Ndim_edge)
 INTEGER :: Itry,Ndim_poly,Ndim_gon,iv,ip
!
   Itry = 0
!
   Ndim_Poly = 6
!
   Ndim_Gon = MAX(Nplan(2,1)-Nplan(1,1), Nplan(2,2)-Nplan(1,2)) + 1
   DO iv=1,2
     DO ip=Nplan(1,iv),Nplan(2,iv)
       Ndim_Gon = MAX( Ndim_Gon , Nedge(2,ip)-Nedge(1,ip)+1 )
     ENDDO
   ENDDO
   Ndim_Gon = MIN( 3*Ndim_Gon , 200 )
!
10 CONTINUE
!###############################################################################
   IF( IdebugBOO >= 1 ) PRINT 7701,Ndim_volu,Ndim_plan,Ndim_edge,Ndim_poly,Ndim_gon
7701 FORMAT(//' Ndim_Volu_Plan_Edge_Poly_Gon =',5I6)
!###############################################################################
   CALL Bool_Oper_Vol_Dim(Ndim_Volu,Ndim_Plan,Ndim_Edge,  Ndim_Poly,Ndim_Gon,  Ioverf,  &
                          Jbool1,Jbool2,  Jcolor,  Nplan,Icolo,Nedge,Ptxyz,Iseen )
!
   IF( Ioverf >= 4 .AND. Ioverf <= 5 .AND. Itry <= 10 ) THEN
     Itry = Itry + 1
     IF( Ioverf == 4 ) THEN
       Ndim_Poly = 2*Ndim_Poly
     ELSEIF( Ioverf == 5 ) THEN
       Ndim_Gon  = 2*Ndim_Gon
     ENDIF
     GO TO 10
   ENDIF
!
 END SUBROUTINE Bool_Oper_Vol
!
