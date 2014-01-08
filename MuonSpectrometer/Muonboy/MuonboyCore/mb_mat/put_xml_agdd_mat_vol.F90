!
!> \author SaMuSoG
!
 SUBROUTINE PUT_XML_AGDD_MAT_VOL(Iobj,TRA,ROT,INDEX,IvP)
 USE M_MB_Increm_Index
 USE M_MB_Xml
 USE M_MB_General_Volumes
 USE M_MB_COMATI
 IMPLICIT NONE
!
!>>  Position a copy of the (already created) generic volume coresponding
!>>  to the XML object Iobj with :
!>>  input  : a translation :  TRA
!>>           a rotation    :  ROT
!>>           indexes       :  INDEX
!>>  in the AGDD Muonboy matter
!
 INTEGER, INTENT(IN)  :: Iobj,INDEX(*)
 INTEGER, INTENT(OUT) :: IvP
 REAL(8), INTENT(IN)  :: TRA(3),ROT(3,3)
 CHARACTER (len=8)    :: CAR8
 INTEGER, ALLOCATABLE :: IaddNew(:)
 CHARACTER (len=80)   :: ATAB( 5)
 INTEGER              :: ITAB(10),Iv0,IA,II,IR,IMANAT,JFF,JZZ,K
 INTEGER              ::          Info,IAMI,IAMA,IP,IE,L1,L2
 INTEGER              ::          IvMat,IT0,IT1,IT2,ITresult
 REAL(8)              :: RTAB(10),V3a(3),V3b(3),V33(3,3)
!
!>> Get the number of the generic volume defining the shape
!>> of the matter volume to be positioned
   IvP  = 0
   Info = Info_xob(Iobj)
   Iv0  = Ivol_info(1,Info)
   IF( Iv0 <= 0 ) THEN
     PRINT 7771
7771 FORMAT(' Cannot position an AGDD Muonboy matter volume ',   &
            'which has not been created as a generic volume  !!!')
     RETURN
   ENDIF
!
!>> Get the name of the matter volume to be positioned
   CAR8 = '?'
   CALL GIV_VAL_XML_ATT(Iobj,'name', IA,II,IR,ATAB,ITAB,RTAB)
   IF( IA == 1 ) THEN
     L2 = LEN_TRIM( ATAB(1) )
     L1 = MAX( 1 , L2-7 )
     CAR8 = ATAB(1)(L1:L2)
   ENDIF
!
!>> Get the indexes of the matter volume to be positioned
   IMANAT = Imat_info(Info)
   JFF    = 1
   JZZ    = 1
   DO K=1,Nb_xob_ind
     IF( INDEX(K) == -999999 ) CYCLE
     CALL GIV_NAM_XML_IND( K, ATAB(1) )
     IF( ATAB(1) == 'sector' ) JFF = INDEX(K)
     IF( ATAB(1) == 'ring'   ) THEN
       IF( INDEX(K) >= 0 ) THEN
         JZZ = INDEX(K)
       ELSE
         JZZ = INDEX(K) + 50
       ENDIF
     ENDIF
   ENDDO
!
   IvMat = Ivma_info(1,Info)
   IF( IvMat < 1 ) THEN
!>> Create matter volumes at the right positions
!     ------------------------------------------
     DO Iv0=Ivol_info(1,Info),Ivol_info(2,Info)
       CALL NEWMAT(CAR8,IMANAT,JFF,JZZ)
!
       IAMI = Nedge
       IAMA = 1
       DO IP=NplanI(1,Iv0),NplanA(1,Iv0)
         DO IE=NedgeI(IP),NedgeA(IP)
           IA = IadEdge(IE)
           IAMI = MIN( IAMI , IA )
           IAMA = MAX( IAMA , IA )
         ENDDO
       ENDDO
       ALLOCATE( IaddNew(IAMI:IAMA) )
       IaddNew(IAMI:IAMA) = 0
!
       DO IP=NplanI(1,Iv0),NplanA(1,Iv0)
         CALL Increm_Index(NPMATI,KPLAN,'KPLAN')
         NEMAT0(NPMATI) = NEMATI + 1
         DO IE=NedgeI(IP),NedgeA(IP)
           CALL Increm_Index(NEMATI,KEDGE,'KEDGE')
           IA = IadEdge(IE)
           IF( IaddNew(IA) > 0 ) THEN
             IAMATI(NEMATI) = IaddNew(IA)
           ELSE
             CALL Increm_Index(NAMATI,KADGE,'KADGE')
             V3a(1:3) = XYZadge(1:3,IA) - CenVolu(1:3,Iv0)
             CALL MULT3M3V(ROT,V3a, V3b)
             XYZMATI(1:3,NAMATI) = V3b(1:3)+CenVolu(1:3,Iv0)+TRA(1:3)
             IAMATI(NEMATI) = NAMATI
             IaddNew(IA)    = NAMATI
           ENDIF
         ENDDO
         NEMAT1(NPMATI) = NEMATI
       ENDDO
!
       DEALLOCATE( IaddNew )
       CALL ENDMAT
!
       V3a(1:3) = - TRA(1:3)
       V33 = TRANSPOSE(ROT)       ! inversion
       CALL MULT3M3V(V33,V3a, V3b)
       IF( Iv0 == Ivol_info(1,Info) ) Ivma_info(1,Info) = NVMATI
       IF( Iv0 == Ivol_info(2,Info) ) Ivma_info(2,Info) = NVMATI
       Tra_info(1:3,    Info) = V3b(1:3)
       Rot_info(1:3,1:3,Info) = V33(1:3,1:3)
     ENDDO
!     ------------------------------------------
   ELSE
!>> Put at the right positions copies of already created matter volumes
!     --------------------------------------------
     DO IvMat=Ivma_info(1,Info),Ivma_info(2,Info)
       IT0 = NTMATI
       CALL INCR_NTMATI
       IT1 = NTMATI
       V33(1:3,1:3) = Rot_info(1:3,1:3,Info)
       TRANSMATI(1:3,1:3,IT1) = TRANSPOSE(V33)
       TRANSMATI(1:3, 4 ,IT1) = Tra_info(1:3,Info)
       CALL INCR_NTMATI
       IT2 = NTMATI
       V33(1:3,1:3) = ROT(1:3,1:3)
       TRANSMATI(1:3,1:3,IT2) = TRANSPOSE(V33)
       TRANSMATI(1:3, 4 ,IT2) = TRA(1:3)
       CALL COMPOS_TRANSMATI( IT1,IT2, ITresult )
       IF( ITresult > IT0 ) THEN
         TRANSMATI(1:3,1:4,IT1) = TRANSMATI(1:3,1:4,ITresult)
         ITresult = IT1
         NTMATI   = IT1
       ELSE
         NTMATI   = IT0
       ENDIF
       CALL COPYMAT( IvMat, ITresult, CAR8,IMANAT,JFF,JZZ )
     ENDDO
!     --------------------------------------------
   ENDIF
!
   IvP = NVMATI
!
 END SUBROUTINE PUT_XML_AGDD_MAT_VOL
!
