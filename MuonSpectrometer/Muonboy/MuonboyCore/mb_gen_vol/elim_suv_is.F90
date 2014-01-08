!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE ELIM_SUV_IS(IS0)
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
!>>  WARNING : This routine changes the volume numbering (IV)
!>> *********
!
 INTEGER :: IS0
 INTEGER :: IAMI,IAMA,IEMI,IEMA,IPMI,IPMA,IVMI,IVMA
 INTEGER :: IS,IV,IP,IE,IA,IN0,IN1,INDIFF,J
 INTEGER :: IANEW,IADIFF,IENEW,IEDIFF,IPNEW,IPDIFF,IVNEW,IVDIFF
!
   IF( NvoluA(IS0)-NvoluI(IS0) < 0 ) THEN
     DO IS=IS0+1,Nsuvo
       NvoluI(    IS-1) = NvoluI(    IS)
       NvoluA(    IS-1) = NvoluA(    IS)
       NamSuv(1:2,IS-1) = NamSuv(1:2,IS)
     ENDDO
     Nsuvo = Nsuvo - 1
     RETURN
   ENDIF
!
   IAMI = Nadge
   IAMA = 1
   IEMI = Nedge
   IEMA = 1
   IPMI = Nplan
   IPMA = 1
   IVMI = NvoluI(IS0)
   IVMA = NvoluA(IS0)
   DO IV=NvoluI(IS0),NvoluA(IS0)
     DO IP=NplanI(1,IV),NplanA(M4,IV)
       IPMI = MIN( IPMI , IP )
       IPMA = MAX( IPMA , IP )
       DO IE=NedgeI(IP),NedgeA(IP)
         IEMI = MIN( IEMI , IE )
         IEMA = MAX( IEMA , IE )
         IA = IadEdge(IE)
         IAMI = MIN( IAMI , IA )
         IAMA = MAX( IAMA , IA )
       ENDDO
     ENDDO
   ENDDO
!
!>> NbNamVol -----------------------------------
   IN0    = NamVol(1,IVMI)
   IN1    = NamVol(2,IVMA) + 1
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
   IF( IAMA >= IAMI ) THEN
     IANEW = IAMI - 1
     DO IA=IAMA+1,Nadge
       IANEW = IANEW + 1
       XYZadge(1:3,IANEW) = XYZadge(1:3,IA)
     ENDDO
     IADIFF = Nadge - IANEW
     Nadge = IANEW
   ENDIF
!
!>> Nedge --------------------------------------
   IF( IEMA >= IEMI ) THEN
     IENEW = IEMI - 1
     DO IE=IEMA+1,Nedge
       IENEW = IENEW + 1
       IadEdge(IENEW) = IadEdge(IE) - IADIFF
       IvuEdge(IENEW) = IvuEdge(IE)
     ENDDO
     IEDIFF = Nedge - IENEW
     Nedge = IENEW
   ENDIF
!
!>> Nplan --------------------------------------
   IF( IPMA >= IPMI ) THEN
     IPNEW = IPMI - 1
     DO IP=IPMA+1,Nplan
       IPNEW = IPNEW + 1
       NedgeI( IPNEW) = NedgeI( IP) - IEDIFF
       NedgeA( IPNEW) = NedgeA( IP) - IEDIFF
       IcoPlan(IPNEW) = IcoPlan(IP)
     ENDDO
     IPDIFF = Nplan - IPNEW
     Nplan = IPNEW
   ENDIF
!
!>> Nvolu --------------------------------------
   IVNEW = IVMI - 1
   DO IV=IVMA+1,Nvolu
     IVNEW = IVNEW + 1
     NplanI(1:M4,IVNEW) =  NplanI(1:M4,IV) - IPDIFF
     NplanA(1:M4,IVNEW) =  NplanA(1:M4,IV) - IPDIFF
     NamVol( 1:2,IVNEW) =  NamVol( 1:2,IV)
     IclVolu(    IVNEW) = IclVolu(     IV)
     CenVolu(1:3,IVNEW) = CenVolu( 1:3,IV)
     D4Volu(1:M4,IVNEW) =  D4Volu(1:M4,IV)
   ENDDO
   IVDIFF = Nvolu - IVNEW
   Nvolu = IVNEW
!
!>> Nsuvo --------------------------------------
   IN0    = NamSuv(1,IS0)
   IN1    = NamSuv(2,IS0) + 1
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
   DO IS=IS0+1,Nsuvo
     NvoluI(    IS-1) = NvoluI(    IS) - IVDIFF
     NvoluA(    IS-1) = NvoluA(    IS) - IVDIFF
     NamSuv(1:2,IS-1) = NamSuv(1:2,IS)
   ENDDO
   Nsuvo = Nsuvo - 1
!
 END
