!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE PRI_VOL(IFLAG)
 USE M_MB_General_Volumes
 IMPLICIT NONE
 INTEGER :: IFLAG
 INTEGER :: IS,IV,IP,IE,L
!
   PRINT 1000,Nsuvo,Nvolu,Nplan,Nedge,Nadge,NbNamVol
   IF( IFLAG >= 1 ) PRINT 1010
   DO IS=1,Nsuvo
     DO IV=NvoluI(IS),NvoluA(IS)
       PRINT 1030,IS,IV,TRIM(CHASUV(IS)),TRIM(CHAVOL(IV)),(NplanA(L,IV)-NplanI(L,IV)+1,L=1,M4)
       IF( IFLAG >= 1 ) THEN
         DO IP=NplanI(1,IV),NplanA(1,IV)
           IF( NedgeA(IP)-NedgeI(IP) <= 11 ) THEN
             PRINT 1100,IS,IV,IP,(IE ,IE=NedgeI(IP)   ,NedgeA(IP)   )
             PRINT 1200, (IadEdge(IE),IE=NedgeI(IP)   ,NedgeA(IP)   )
           ELSE
             PRINT 1100,IS,IV,IP,(IE ,IE=NedgeI(IP)   ,NedgeI(IP)+11)
             PRINT 1200,         (IE ,IE=NedgeI(IP)+12,NedgeA(IP)   )
             PRINT 1200, (IadEdge(IE),IE=NedgeI(IP)   ,NedgeI(IP)+11)
             PRINT 1200, (IadEdge(IE),IE=NedgeI(IP)+12,NedgeA(IP)   )
           ENDIF
         ENDDO
       ENDIF
     ENDDO
   ENDDO
!
1000  FORMAT(/' N_Suvo_Volu_Plan_Edge_Adge_NamVol =',6I7)
1010  FORMAT( '     Is     Iv     Ip          Ie... / Ia....')
1030  FORMAT(' Is,Iv,Suv,Vol =',2I7,3X,A,3X,A,'   Nb_Planes =',9I5)
1100         FORMAT(3I7,4X,12I7)
1200         FORMAT(25X,12I7)
 END
