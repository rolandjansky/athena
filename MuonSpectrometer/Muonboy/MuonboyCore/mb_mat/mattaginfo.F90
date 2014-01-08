!-----------------------------------------------------
 SUBROUTINE ReSetMatTagInfo
 USE M_MB_Mat_TagInfo
 IMPLICIT NONE
 
 NewTagInfoMat = 0
 NewTagInfoRpc = 0
 NberTagInfo = 0

 END SUBROUTINE ReSetMatTagInfo

!-----------------------------------------------------
 SUBROUTINE F1SetMatTagInfo( LongA, LongB, NameA, NameB)
 IMPLICIT NONE

 INTEGER,           INTENT(IN) :: LongA
 INTEGER,           INTENT(IN) :: LongB
 CHARACTER (Len=1), INTENT(IN) :: NameA(*)
 CHARACTER (Len=1), INTENT(IN) :: NameB(*)
 
 CHARACTER (len=LongA) :: VarNameA
 CHARACTER (len=LongB) :: VarNameB
 INTEGER              :: I

 DO I=1,LongA
  VarNameA(I:I) = NameA(I)
 ENDDO
 
 DO I=1,LongB
  VarNameB(I:I) = NameB(I)
 ENDDO
 CALL SetMatTagInfo( LongA, LongB, VarNameA, VarNameB)

 END SUBROUTINE F1SetMatTagInfo

!-----------------------------------------------------
 SUBROUTINE SetMatTagInfo( LongA, LongB, NameA, NameB)
 USE M_MB_Mat_TagInfo
 IMPLICIT NONE

 INTEGER,           INTENT(IN) :: LongA
 INTEGER,           INTENT(IN) :: LongB
 CHARACTER (len=*), INTENT(IN) :: NameA
 CHARACTER (len=*), INTENT(IN) :: NameB

 CHARACTER(100) :: ChaTemp
 INTEGER        :: I
 
 NewTagInfoMat = 1
 NewTagInfoRpc = 1
 NberTagInfo = NberTagInfo + 1

 DO I=1,100
  ChaTemp(I:I) =" "
  if (I.le.LongA) then
    ChaTemp(I:I) = NameA(I:I)
  endif
 ENDDO
 ChaTagInfoA(NberTagInfo) = ChaTemp
 
 DO I=1,100
  ChaTemp(I:I) =" "
  if (I.le.LongB) then
    ChaTemp(I:I) = NameB(I:I)
  endif
 ENDDO
 ChaTagInfoB(NberTagInfo) = ChaTemp

 END SUBROUTINE SetMatTagInfo
