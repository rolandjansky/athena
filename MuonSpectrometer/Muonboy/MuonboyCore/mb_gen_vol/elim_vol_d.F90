!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE ELIM_VOL_D(NADMI,NADMA,IS0,IV0)
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
!>>  WARNING : This routine changes the volume numbering (IV)
!>> *********
!
 INTEGER :: NADMI,NADMA,IS0,IV0
 INTEGER :: IEDEBV,IEFINV,IEDEBS,IEFINS,IEDIFF
 INTEGER :: IPDEBV,IPFINV,IPDIFF
 INTEGER :: IS,IV,IP,IE,IA,IN0,IN1,INDIFF,ISDIFF,J
 INTEGER :: IADNEW,IADOLD,IADIFF
 INTEGER :: JAA(NADMI:NADMA)
!
   IEDEBV = Nedge
   IEFINV =     1
   JAA(NADMI:NADMA) = 1
   DO IP=NplanI(1,IV0),NplanA(M4,IV0)
    DO IE=NedgeI(IP),NedgeA(IP)
     IEDEBV = MIN( IEDEBV , IE )
     IEFINV = MAX( IEFINV , IE )
     IA = IadEdge(IE)
     JAA(IA) = 0
    ENDDO
   ENDDO
   IEDEBS = IEDEBV
   IEFINS = IEFINV
   DO IV=NvoluI(IS0),NvoluA(IS0)
    IF( IV == IV0 ) CYCLE
       DO IP=NplanI(1,IV),NplanA(M4,IV)
        DO IE=NedgeI(IP),NedgeA(IP)
         IEDEBS = MIN( IEDEBS , IE )
         IEFINS = MAX( IEFINS , IE )
         IA = IadEdge(IE)
         JAA(IA) = 1
        ENDDO
       ENDDO
   ENDDO
!
!>> NbNamVol -----------------------------------
   IN0    = NamVol(1,IV0)
   IN1    = NamVol(2,IV0) + 1
   INDIFF = IN1 - IN0
!>>---- Compiler problem ? -----------------------------------------
!**** CharacNamVol(IN0:NbNamVol-INDIFF) = CharacNamVol(IN1:NbNamVol)
   DO J=IN0,NbNamVol-INDIFF
    CharacNamVol(J) = CharacNamVol(J+INDIFF)
   ENDDO
!>>---- Compiler problem ? -----------------------------------------
   NbNamVol = NbNamVol - INDIFF
   WHERE( NamSuv >= IN1 ) NamSuv = NamSuv - INDIFF
   WHERE( NamVol >= IN1 ) NamVol = NamVol - INDIFF
!
!>> Nadge --------------------------------------
   IADNEW = NADMI - 1
   DO IADOLD=NADMI,NADMA
    IF( JAA(IADOLD) >= 1 ) THEN
       IADNEW = IADNEW + 1
       JAA(IADOLD) = IADNEW
       XYZadge(1:3,IADNEW) = XYZadge(1:3,IADOLD)
    ENDIF
   ENDDO
   DO IADOLD=NADMA+1,Nadge
    IADNEW = IADNEW + 1
    XYZadge(1:3,IADNEW) = XYZadge(1:3,IADOLD)
   ENDDO
   IADIFF = Nadge - IADNEW
   Nadge = IADNEW
!
!>> Nedge --------------------------------------
   IEDIFF = IEFINV - IEDEBV + 1
   DO IE=IEDEBS,IEDEBV-1
    IadEdge(IE) = JAA(IadEdge(IE))
   ENDDO
   DO IE=IEFINV+1,IEFINS
    IadEdge(IE-IEDIFF) = JAA(IadEdge(IE))
    IvuEdge(IE-IEDIFF) = IvuEdge(IE)
   ENDDO
   DO IE=IEFINS+1,Nedge
    IadEdge(IE-IEDIFF) = IadEdge(IE)  - IADIFF
    IvuEdge(IE-IEDIFF) = IvuEdge(IE)
   ENDDO
   Nedge = Nedge - IEDIFF
!
!>> Nplan --------------------------------------
   IPDEBV = NplanI( 1,IV0)
   IPFINV = NplanA(M4,IV0)
   IPDIFF = IPFINV - IPDEBV + 1
   DO IP=IPFINV+1,Nplan
    NedgeI( IP-IPDIFF) = NedgeI( IP) - IEDIFF
    NedgeA( IP-IPDIFF) = NedgeA( IP) - IEDIFF
    IcoPlan(IP-IPDIFF) = IcoPlan(IP)
   ENDDO
   Nplan = Nplan - IPDIFF
!
!>> Nvolu --------------------------------------
   DO IV=IV0+1,Nvolu
    NplanI(1:M4,IV-1) = NplanI(1:M4,IV) - IPDIFF
    NplanA(1:M4,IV-1) = NplanA(1:M4,IV) - IPDIFF
    NamVol( 1:2,IV-1) = NamVol( 1:2,IV)
    IclVolu(    IV-1) = IclVolu(    IV)
    CenVolu(1:3,IV-1) = CenVolu(1:3,IV)
    D4Volu(1:M4,IV-1) = D4Volu(1:M4,IV)
   ENDDO
   Nvolu = Nvolu - 1
!
!>> Nsuvo --------------------------------------
   NvoluA(IS0) = NvoluA(IS0) -1
   ISDIFF = 0
   IF( NvoluA(IS0) < NvoluI(IS0) ) THEN
      ISDIFF = 1
      IN0    = NamSuv(1,IS0)
      IN1    = NamSuv(2,IS0) + 1
      INDIFF = IN1 - IN0
!>>---- Compiler problem ? --------------------------------------------
!******* CharacNamVol(IN0:NbNamVol-INDIFF) = CharacNamVol(IN1:NbNamVol)
      DO J=IN0,NbNamVol-INDIFF
       CharacNamVol(J) = CharacNamVol(J+INDIFF)
      ENDDO
!>>---- Compiler problem ? --------------------------------------------
      NbNamVol = NbNamVol - INDIFF
      WHERE( NamSuv >= IN1 ) NamSuv = NamSuv - INDIFF
      WHERE( NamVol >= IN1 ) NamVol = NamVol - INDIFF
   ENDIF
   DO IS=IS0+1,Nsuvo
    NvoluI(    IS-ISDIFF) = NvoluI(    IS) - 1
    NvoluA(    IS-ISDIFF) = NvoluA(    IS) - 1
    NamSuv(1:2,IS-ISDIFF) = NamSuv(1:2,IS)
   ENDDO
   Nsuvo = Nsuvo - ISDIFF
!
 END
