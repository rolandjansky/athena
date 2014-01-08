!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE CREAT_XML_MDT_Chamber(Iobj)
 USE M_MB_Xml
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Iobj
 CHARACTER (len=80) :: Name,Envel,Type
 CHARACTER (len=80) :: NameVolu,Name1,Name2,Name3
 CHARACTER (len=80) :: NameML(2),NameSpacer,NameCcha
 CHARACTER (len=80) :: NameTube(2),NameSupport(2),NamePlug(2)
 INTEGER            :: Iobj1,Iele1,Iref,Iobj2,Iobj3,Iele3
 INTEGER            :: Icolor,IUpLo,IobjPre
 INTEGER            :: IobjML(2),IobjTube(2)
 INTEGER            :: NLayer(2),NTubPerStep(2)
 REAL(8)            :: Xws,Xwl,Xle,Xspa, Xdim(20)
 REAL(8)            :: RtubI(2),RtubO(2),Xsupp(2),Xplug(2)
 INTEGER, SAVE      :: Ifatal = 0
 REAL(8), PARAMETER :: Zero = 0.D0
!
!>>  Create the generic volume coresponding to the XML "compact" object Iobj
!
   IobjPre = Iob_xob_sup(Iobj)
   CALL GET_COMPACT_INFO(IobjPre, Name,Envel,Type,Iobj1,Iele1)
   IF( Iobj /= Iobj1 )         GO TO 900
   IF( Type /= 'MDT_Chamber' ) GO TO 901
!
   NameVolu = Name
!* Icolor   = Icol_info(Info_xob(Iobj))
!* IF( Icolor <= 0 ) Icolor = 1
   Icolor = 4
!
!>> MDT_Chamber
   CALL GIV_XML_ATT(Iobj1, 'ShortWidth'     , Xws        )
   CALL GIV_XML_ATT(Iobj1, 'LongWidth'      , Xwl        )
   CALL GIV_XML_ATT(Iobj1, 'Length'         , Xle        )
   CALL GIV_XML_ATT(Iobj1, 'UpperMultilayer', NameML(1)  )
   CALL GIV_XML_ATT(Iobj1, 'LowerMultilayer', NameML(2)  )
   CALL GIV_XML_ATT(Iobj1, 'Spacer'         , NameSpacer )
!
!>> MDT_Chamber  --->  MDT_Multilayer's  and  MDT_Spacer
   DO Iref=1,3
     Iobj2 = Iob_xob_ref(Iobj1,Iref)
     CALL GET_COMPACT_INFO(Iobj2, Name,Envel,Type,Iobj3,Iele3)
     IF( Type == 'MDT_Multilayer' ) THEN
       IF(     Name == NameML(1) ) THEN
         IUpLo = 1
       ELSEIF( Name == NameML(2) ) THEN
         IUpLo = 2
       ELSE
         GO TO 902
       ENDIF
       IobjML(IUpLo) = Iobj3
       CALL GIV_XML_ATT(Iobj3, 'nLayers'      , NLayer(IUpLo)      )
       CALL GIV_XML_ATT(Iobj3, 'nTubesPerStep', NTubPerStep(IUpLo) )
       CALL GIV_XML_ATT(Iobj3, 'Tube'         , NameTube(IUpLo)    )
       CALL GIV_XML_ATT(Iobj3, 'Support'      , NameSupport(IUpLo) )
     ELSEIF( Type == 'MDT_Spacer' ) THEN
       IF( Name /= NameSpacer ) GO TO 903
       CALL GIV_XML_ATT(Iobj3, 'Height'   , Xspa     )
       CALL GIV_XML_ATT(Iobj3, 'C_Channel', NameCcha )
     ELSE
       GO TO 904
     ENDIF
   ENDDO
!
!>> MDT_Multilayer's  --->  MDT_Tube's  and  MDT_Support's
   DO IUpLo=1,2
     DO Iref=1,2
       Iobj2 = Iob_xob_ref(IobjML(IUpLo),Iref)
       CALL GET_COMPACT_INFO(Iobj2, Name,Envel,Type,Iobj3,Iele3)
       IF( Type == 'MDT_Tube' ) THEN
         IF( Name /= NameTube(IUpLo) ) GO TO 905
         IobjTube(IUpLo) = Iobj3
         CALL GIV_XML_ATT(Iobj3, 'InnerRadius', RtubI(IUpLo)    )
         CALL GIV_XML_ATT(Iobj3, 'OuterRadius', RtubO(IUpLo)    )
         CALL GIV_XML_ATT(Iobj3, 'Plug'       , NamePlug(IUpLo) )
       ELSEIF( Type == 'MDT_Support' ) THEN
         IF( Name /= NameSupport(IUpLo) ) GO TO 906
         CALL GIV_XML_ATT(Iobj3, 'Thickness', Xsupp(IUpLo) )
       ELSE
         GO TO 907
       ENDIF
     ENDDO
   ENDDO
!
!>> MDT_Tube's  --->  MDT_TubePlug's
   DO IUpLo=1,2
     Iobj2 = Iob_xob_ref(IobjTube(IUpLo),1)
     CALL GET_COMPACT_INFO(Iobj2, Name,Envel,Type,Iobj3,Iele3)
     IF( Type == 'MDT_TubePlug' ) THEN
       IF( Name /= NamePlug(IUpLo) ) GO TO 908
       CALL GIV_XML_ATT(Iobj3, 'Length', Xplug(IUpLo) )
     ELSE
       GO TO 909
     ENDIF
   ENDDO
!
!>>  Xws  Xwl  Xle  Xspa  NLayer(.)  NTubPerStep(.)  RtubI(.)  RtubO(.)
   Xdim(1)   = Xws
   Xdim(2)   = Xwl
   Xdim(3:4) = NLayer(1)*RtubO(1)
   Xdim(5)   = Xle
   Name1 = 'Copy_1_'//TRIM(NameVolu)
   Name2 = 'Copy_2_'//TRIM(NameVolu)
   Name3 = 'Copy_3_'//TRIM(NameVolu)
!* Name4 = 'Copy_4_'//TRIM(NameVolu)
   CALL DEF_BOX_DX12DY12DZ( Name1, Icolor, Xdim )
   Xdim(3:4) = NLayer(2) * RtubO(2)
   CALL DEF_BOX_DX12DY12DZ( Name2, Icolor, Xdim )
   CALL TRANSLAT_VOL_XYZ( Name1, Zero, Xspa/2.D0,Zero )
   CALL DEF_CENVOL( Zero, Zero, Zero )
   CALL TRANSLAT_VOL_XYZ( Name2, Zero,-Xspa/2.D0,Zero )
   CALL DEF_CENVOL( Zero, Zero, Zero )
!* CALL VOLABOOLB_NAME( 1,1, 0, Name1,Name2, Name3 )
!* CALL DEF_CENVOL_MOY
!* CALL ELIM_VOL_NAME(Name2)
!* CALL ELIM_VOL_NAME(Name1)
!
   Xdim(3:4) = Xsupp(1)
   CALL DEF_BOX_DX12DY12DZ( Name3, 9, Xdim )
   Xdim(9) = Xspa/2.D0 + (NLayer(1)*RtubO(1)+Xsupp(1))/2.D0
   Xdim(9) = Xdim(9) + 0.0100D0
   CALL TRANSLAT_VOL_XYZ( Name3, Zero,Xdim(9),Zero )
   CALL DEF_CENVOL( Zero, Zero, Zero )
!* CALL VOLABOOLB_NAME( 1,1, 0, Name3,Name1, NameVolu )
!* CALL ELIM_VOL_NAME(Name1)
!* CALL ELIM_VOL_NAME(Name3)
!
!* Xdim(1)   = RtubO(1)
!* Xdim(2)   = 0.D0
!* Xdim(3)   = Xle
!* CALL DEF_TUBE_R1R2DZN( Name1, Icolor, Xdim, 8 )
!* CALL ROTATE_VOL( Name1, 1, 2, 90.D0 )
!* CALL COPY_VOL_NAME( Name1,Name2,0 )
!* CALL TRANSLAT_VOL_XYZ( Name1, Zero,Zero, Xle/2.D0 )
!* CALL TRANSLAT_VOL_XYZ( Name2, Zero,Zero,-Xle/2.D0 )
!* CALL VOLABOOLB_NAME( 1,1, Icolor, Name1,Name4, Name3 )
!* CALL ELIM_VOL_NAME(Name1)
!* CALL ELIM_VOL_NAME(Name4)
!* CALL VOLABOOLB_NAME( 1,1, Icolor, Name2,Name3, NameVolu )
!* CALL ELIM_VOL_NAME(Name2)
!* CALL ELIM_VOL_NAME(Name3)
!
   RETURN
!
909  Ifatal = Ifatal + 1
908  Ifatal = Ifatal + 1
907  Ifatal = Ifatal + 1
906  Ifatal = Ifatal + 1
905  Ifatal = Ifatal + 1
904  Ifatal = Ifatal + 1
903  Ifatal = Ifatal + 1
902  Ifatal = Ifatal + 1
901  Ifatal = Ifatal + 1
900  PRINT 9000,Ifatal
9000 FORMAT(' In CREAT_XML_MDT_Chamber : Fatal error',I2,'  ===>  STOP !')
     STOP
!
 END
