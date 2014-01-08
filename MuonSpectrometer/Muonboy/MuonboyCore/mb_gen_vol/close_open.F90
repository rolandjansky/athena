!
 SUBROUTINE CLOSE_OPEN(Icolor,Iop1,Iop2,Idraw)
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
 INTEGER, INTENT(IN) :: Icolor,Iop1,Iop2,Idraw
!
 INTEGER :: NNN,NNNP,IADD,I1,I2,IADRAW,I,Jplan
!
   IF( Iop1 > 0 .AND. Iop2 > 0 ) THEN
      CALL CLOSE(Icolor,Idraw)
      RETURN
   ENDIF
!
   NNN  = NedgeA(Nplan  ) - NedgeI(Nplan  ) + 1
   NNNP = NedgeA(Nplan-1) - NedgeI(Nplan-1) + 1
   IF( NNN /= NNNP ) THEN
      WRITE(*,'(/," In CLOSE_OPEN : NNN,NNNP =",2I5,"  =====>  STOP !")') NNN,NNNP
      STOP
   ENDIF
!
   IADD = 0
   IF( SIZE(NedgeI)  < Nplan+NNN   ) CALL Push_MB_Gen_Vol_MPLAN
   IF( SIZE(IvuEdge) < Nedge+NNN*4 ) CALL Push_MB_Gen_Vol_MEDGE
   DO I1=0,NNN-1
    IF( IvuEdge(NedgeI(Nplan)+I1)*IvuEdge(NedgeI(Nplan-1)+I1) == 0 .AND. Idraw > -1 ) CYCLE
       IADD = IADD + 1
       I2 = MOD(I1+1,NNN)
       IADRAW = IABS(Idraw)
       NedgeI( Nplan+IADD) = Nedge + 1
       NedgeA( Nplan+IADD) = Nedge + 4
       IcoPlan(Nplan+IADD) = Icolor
       IvuEdge(Nedge+1) = IADRAW
       IvuEdge(Nedge+2) = IADRAW * IvuEdge(NedgeI(Nplan  )+I1)
       IvuEdge(Nedge+3) = IADRAW
       IvuEdge(Nedge+4) = IADRAW * IvuEdge(NedgeI(Nplan-1)+I1)
       IadEdge(Nedge+1) = IadEdge(NedgeI(Nplan-1)+I1)
       IadEdge(Nedge+2) = IadEdge(NedgeI(Nplan  )+I1)
       IadEdge(Nedge+3) = IadEdge(NedgeI(Nplan  )+I2)
       IadEdge(Nedge+4) = IadEdge(NedgeI(Nplan-1)+I2)
       Nedge = Nedge + 4
   ENDDO
!
   Jplan = Nplan - 2
   DO I=1,2+IADD
    IF( I == 1 .AND. Iop1 < 1 ) CYCLE
    IF( I == 2 .AND. Iop2 < 1 ) CYCLE
       Jplan = Jplan + 1
       NedgeI( Jplan) = NedgeI( Nplan-2+I)
       NedgeA( Jplan) = NedgeA( Nplan-2+I)
       IcoPlan(Jplan) = IcoPlan(Nplan-2+I)
   ENDDO
   Nplan = Jplan
!
 END SUBROUTINE CLOSE_OPEN
!
