!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE PUT_XML_VOL(Iobj,TRA,ROT,INDEX,Kremov, IvP)
 USE M_MB_Xml
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Iobj,Kremov,INDEX(*)
 REAL(8), INTENT(IN)  :: TRA(3),ROT(3,3)
 INTEGER, INTENT(OUT) :: IvP
 CHARACTER (len=6)   :: CAR6
 CHARACTER (len=800) :: CAR800,CAR800Ind,CAR800c
 INTEGER             :: Info,Iv0,K,IobjParent,IobjChild
 CHARACTER (len=80)  :: NameIndex,NameMatter 
!
!>>  Position a copy of the (already created) generic volume coresponding
!>>  to the XML object Iobj with :
!>>  input  : a translation :  TRA
!>>           a rotation    :  ROT
!>>           indexes       :  INDEX
!>>  in the specified ref. frame ( I_Atlas_Ref_Syst )
!>>           If flag  Kremov  is /= 0, then flag the created volume
!>>           (IclVolu(iv) = Kremov) so that e.g. when Kremov is < -9
!>>           it can be latter discarded...
!>>
!>>  output : IvP = number of the created volume
!
   IvP = 0
!
   Info = Info_xob(Iobj)
   Iv0  = Ivol_info(1,Info)
   IF( Iv0 <= 0 ) THEN
     PRINT 7771
7771 FORMAT(' Cannot position an object which has not been created as a generic volume  !!!')
     RETURN
   ENDIF
!
!>> Form the generic name of the positionned volume(s)
   CALL GIV_NAM_XML_OBJ( Iobj, CAR800 )
!
!>> Form the index sequence of the positionned volume(s)
   CAR800Ind = '/'
   CALL GIV_XML_ATT( Iobj, 'material', NameMatter )
   IF( NameMatter == '' ) THEN
     IobjParent = Iobj
     DO
       IobjChild = 0
       CALL Get_Next_Xml_Child( IobjParent, IobjChild )
       IF( IobjChild > Nb_xob ) EXIT
       CALL GIV_XML_ATT( IobjChild, 'material', NameMatter )
       IF( NameMatter /= '' )   EXIT
       IobjParent = IobjChild
     ENDDO
   ENDIF
   IF( NameMatter /= '' )  CAR800Ind = TRIM(CAR800Ind)//TRIM(NameMatter)//'/'
   DO K=1,Nb_xob_ind
     IF( INDEX(K) == -999999 ) CYCLE
     WRITE (CAR6,'(I6)') INDEX(K)
     CAR6 = ADJUSTL(CAR6)
     CALL GIV_NAM_XML_IND( K, NameIndex )
     CAR800Ind = TRIM(CAR800Ind)//TRIM(NameIndex)//TRIM(CAR6)//'/'
   ENDDO
!
   CAR800c = CAR800
   DO Iv0=Ivol_info(1,Info),Ivol_info(2,Info)
!
!>> Form the name of the current positionned volume
     IF( Ivol_info(2,Info) > Ivol_info(1,Info) ) THEN
       WRITE (CAR6,'(I6)') Iv0-Ivol_info(1,Info)+1
       CAR6 = ADJUSTL(CAR6)
       CAR800c = TRIM(CAR800) //'_v' //TRIM(CAR6)
     ENDIF
     IF( CAR800Ind /= '/' ) CAR800c = TRIM(CAR800c) //TRIM(CAR800Ind)
!
!>> Position a copy of the volume(s) to be positionned
     CALL POS_COPY_VOL_IV( Iv0, CAR800c,Kremov,0,TRA,ROT, IvP )
!
   ENDDO
!
 END
