!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE AMDCREAD
!
      INTEGER :: LUN,IFLAG
!--------------------------------------------------------------------
! Steering routine for reading the  AMDC_SIMREC  data base
! asked by Serguei Baranov for  ATLSIM
!--------------------------------------------------------------------
!
!-- IFLAG =   0  ===>  Conversion CYLINDRICAL -> CARTESIAN during reading
!-- IFLAG = 100  ===>  No conversion !
      DATA LUN,IFLAG/ 32, 100 /
!
      open( unit=LUN, file='amdb_simrec.data', status='OLD' )
      write(*,'(/,'' ReAMDC: amdb_simrec.data file  was opened'',/)')
      CALL REAMDC(LUN,IFLAG)
      close( unit=LUN )
!
      END SUBROUTINE AMDCREAD
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE AMDCREADN(FileName,SizeName,Istate)
!
      character(1) :: FileName(*)
      integer      :: SizeName
      integer      :: Istate
!
      integer      :: IFLAG
!--------------------------------------------------------------------
! Steering routine for reading the  AMDC_SIMREC  data base
! asked by Serguei Baranov for  ATLSIM
!--------------------------------------------------------------------
!
!-- IFLAG =   0  ===>  Conversion CYLINDRICAL -> CARTESIAN during reading
!-- IFLAG = 100  ===>  No conversion !
      DATA IFLAG/ 100 /
!
      call AMDCREADNN(FileName,SizeName,Istate,IFLAG)
!
      END SUBROUTINE AMDCREADN
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE AMDCREADNN(FileName,SizeName,Istate,IFLAG)
!
      character(1) :: FileName(*)
      integer      :: SizeName
      integer      :: Istate
      integer      :: IFLAG
!--------------------------------------------------------------------
! Steering routine for reading the  AMDC_SIMREC  data base
! asked by Serguei Baranov for  ATLSIM
!--------------------------------------------------------------------
!
!-- IFLAG =   0  ===>  Conversion CYLINDRICAL -> CARTESIAN during reading
!-- IFLAG = 100  ===>  No conversion !
!
      logical     :: FileExists
!
      character(500) :: BigFileName
      INTEGER :: LUN
!
      DATA LUN/ 32 /
!
      Istate = 0
!
      if (SizeName.gt.500) then
       write(*,'(/,'' ReAMDC: File too big'',/)')
       return
      endif
!
      do i=1,SizeName
       BigFileName(i:i)=FileName(i)
      enddo
      inquire( file=BigFileName(1:SizeName), exist=FileExists )
      if( .not.FileExists ) then
       write(*,'(/,'' ReAMDC: File not existing'',/)')
       print *,BigFileName(1:SizeName)
       return
      endif
!
      Istate = 1
!
      write(*,'(/,''  ReAMDC opening '',/)')
      print *,BigFileName(1:SizeName)
      open( unit=LUN, file=BigFileName(1:SizeName), status='OLD' )
      write(*,'(/,''   opened'',/)')
      CALL REAMDC(LUN,IFLAG)
      close( unit=LUN )
!
      END SUBROUTINE AMDCREADNN
!
