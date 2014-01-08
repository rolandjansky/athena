!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE MB_Vol_Garbage_Collection
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
!>> Clean Volume structure i.e.:
!>> Eliminate all volumes  Iv  for which  IclVolu(Iv) < -9
!>>                                      ******************
!>>  WARNING : This routine changes the volume numbering (IV)
!>> *********
!
 INTEGER :: IS,ISnew,IV,IVnew,IP,IPnew,IE,IEnew,IA,IAnew,IN,INnew
 INTEGER, ALLOCATABLE :: IaddIS(:)
 INTEGER, ALLOCATABLE :: IaddIV(:)
 INTEGER, ALLOCATABLE :: IaddIP(:)
 INTEGER, ALLOCATABLE :: IaddIE(:)
 INTEGER, ALLOCATABLE :: IaddIA(:)
 INTEGER, ALLOCATABLE :: IaddIN(:)
!
   IF( MINVAL( IclVolu(1:Nvolu) ) >= -9 ) RETURN
!
   ALLOCATE( IaddIS(Nsuvo) )
   ALLOCATE( IaddIV(Nvolu) )
   ALLOCATE( IaddIP(Nplan) )
   ALLOCATE( IaddIE(Nedge) )
   ALLOCATE( IaddIA(Nadge) )
   ALLOCATE( IaddIN(NbNamVol) )
   IaddIS(:) = 0
   IaddIV(:) = 0
   IaddIP(:) = 0
   IaddIE(:) = 0
   IaddIA(:) = 0
   IaddIN(:) = 0
   DO IS=1,Nsuvo
    DO IV=NvoluI(IS),NvoluA(IS)
     IF( IclVolu(IV) < -9 ) CYCLE
        IaddIS(IS)   = 1
        IaddIV(IV)   = 1
        DO IP=NplanI(1,IV),NplanA(M4,IV)
         IaddIP(IP)  = 1
         DO IE=NedgeI(IP),NedgeA(IP)
          IaddIE(IE) = 1
          IA = IadEdge(IE)
          IaddIA(IA) = 1
         ENDDO
        ENDDO
        IaddIN(NamSuv(1,IS):NamSuv(2,IS)) = 1
        IaddIN(NamVol(1,IV):NamVol(2,IV)) = 1
    ENDDO
   ENDDO
!
!>> CharacNamVol -------------------------------
   INnew = 0
   DO IN=1,NbNamVol
     IF( IaddIN(IN) > 0 ) THEN
       INnew = INnew + 1
       CharacNamVol(INnew) = CharacNamVol(IN)
     ENDIF
     IaddIN(IN) = INnew
   ENDDO
!
!>> XYZadge ------------------------------------
   IAnew = 0
   DO IA=1,Nadge
     IF( IaddIA(IA) > 0 ) THEN
       IAnew = IAnew + 1
       XYZadge(1:3,IAnew) = XYZadge(1:3,IA)
     ENDIF
     IaddIA(IA) = IAnew
   ENDDO
!
!>> IadEdge IvuEdge ----------------------------
   IEnew = 0
   DO IE=1,Nedge
     IF( IaddIE(IE) > 0 ) THEN
       IEnew = IEnew + 1
       IadEdge(IEnew) = IaddIA(IadEdge(IE))
       IvuEdge(IEnew) = IvuEdge(IE)
     ENDIF
     IaddIE(IE) = IEnew
   ENDDO
!
!>> NedgeI NedgeA IcoPlan ----------------------
   IPnew = 0
   DO IP=1,Nplan
     IF( IaddIP(IP) > 0 ) THEN
       IPnew = IPnew + 1
       NedgeI( IPnew) = IaddIE(NedgeI(IP))
       NedgeA( IPnew) = IaddIE(NedgeA(IP))
       IcoPlan(IPnew) =       IcoPlan(IP)
     ENDIF
     IaddIP(IP) = IPnew
   ENDDO
!
!>> NplanI NplanA NamVol IclVolu CenVolu D4Volu -
   IVnew = 0
   DO IV=1,Nvolu
     IF( IaddIV(IV) > 0 ) THEN
       IVnew = IVnew + 1
       NplanI(1:M4,IVnew) = IaddIP(NplanI(1:M4,IV))
       NplanA(1:M4,IVnew) = IaddIP(NplanA(1:M4,IV))
       NamVol( 1:2,IVnew) = IaddIN(NamVol( 1:2,IV))
       IclVolu(    IVnew) =       IclVolu(     IV)
       CenVolu(1:3,IVnew) =       CenVolu( 1:3,IV)
       D4Volu(1:M4,IVnew) =        D4Volu(1:M4,IV)
     ENDIF
     IaddIV(IV) = IVnew
   ENDDO
!
!>> NvoluI NvoluA NamSuv -----------------------
   ISnew = 0
   DO IS=1,Nsuvo
     IF( IaddIS(IS) > 0 ) THEN
       ISnew = ISnew + 1
       NvoluI(    ISnew) = MAX( IaddIV(NvoluI(    IS)) , 1 )
       NvoluA(    ISnew) =      IaddIV(NvoluA(    IS))
       NamSuv(1:2,ISnew) =      IaddIN(NamSuv(1:2,IS))
     ENDIF
     IaddIS(IS) = ISnew
   ENDDO
!
!>> Nadge Nedge Nplan Nvolu Nsuvo NbNamVol -----
   NbNamVol = INnew
   Nadge    = IAnew
   Nedge    = IEnew
   Nplan    = IPnew
   Nvolu    = IVnew
   Nsuvo    = ISnew
!
   DEALLOCATE( IaddIS )
   DEALLOCATE( IaddIV )
   DEALLOCATE( IaddIP )
   DEALLOCATE( IaddIE )
   DEALLOCATE( IaddIA )
   DEALLOCATE( IaddIN )
!
 END
