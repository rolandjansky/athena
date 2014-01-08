!
 SUBROUTINE CLOSE(ICOLOR,IDRAW)
 USE M_MB_General_Volumes
 IMPLICIT NONE
 INTEGER :: ICOLOR, IDRAW
 INTEGER :: NNN, NNNP, IADD, I1, I2, IADRAW
!
   NNN  = NedgeA(Nplan  ) - NedgeI(Nplan  ) + 1
   NNNP = NedgeA(Nplan-1) - NedgeI(Nplan-1) + 1
   IF( NNN /= NNNP ) THEN
     WRITE(*,'(/" In CLOSE : NNN,NNNP =",2I5,"  =====>  STOP !")') NNN,NNNP
     STOP
   ENDIF
!
   IADD = 0
   IF( SIZE(NedgeI)  < Nplan+NNN   ) CALL Push_MB_Gen_Vol_MPLAN
   IF( SIZE(IvuEdge) < Nedge+NNN*4 ) CALL Push_MB_Gen_Vol_MEDGE
   DO I1=0,NNN-1
     IF( IvuEdge(NedgeI(Nplan)+I1)*IvuEdge(NedgeI(Nplan-1)+I1) /= 0 .OR. IDRAW <= -1 ) THEN
       IADD = IADD + 1
       I2 = MOD(I1+1,NNN)
       IADRAW = IABS(IDRAW)
       NedgeI( Nplan+IADD) = Nedge + 1
       NedgeA( Nplan+IADD) = Nedge + 4
       IcoPlan(Nplan+IADD) = ICOLOR
       IvuEdge(Nedge+1) = IADRAW
       IvuEdge(Nedge+2) = IADRAW * IvuEdge(NedgeI(Nplan)+I1)
       IvuEdge(Nedge+3) = IADRAW
       IvuEdge(Nedge+4) = IADRAW * IvuEdge(NedgeI(Nplan-1)+I1)
       IadEdge(Nedge+1) = IadEdge(NedgeI(Nplan-1)+I1)
       IadEdge(Nedge+2) = IadEdge(NedgeI(Nplan  )+I1)
       IadEdge(Nedge+3) = IadEdge(NedgeI(Nplan  )+I2)
       IadEdge(Nedge+4) = IadEdge(NedgeI(Nplan-1)+I2)
       Nedge = Nedge + 4
     ENDIF
   ENDDO
   Nplan = Nplan + IADD
!
 END SUBROUTINE CLOSE
!
