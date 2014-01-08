!
 SUBROUTINE ktime_mubo(name,j)
 IMPLICIT NONE
 CHARACTER(8), INTENT(IN) :: name
 INTEGER,      INTENT(IN) :: j
!---------------------------------------------------------------------
! input : name subroutine name
!         j = 1 begin
!           = 2 end
!           = 3 final call (print out)
!
! adapted from routine ptime of m.rudowicz
!---------------------------------------------------------------------
 INTEGER, PARAMETER :: maxnam = 30
 CHARACTER(8), SAVE :: lname(maxnam)
 INTEGER,      SAVE :: itimea(2,maxnam)
 REAL(4),      SAVE :: ktimea(2,maxnam)
 REAL   ,      SAVE :: timpre = 0.
 LOGICAL,      SAVE :: first = .true., flaok = .false.
 INTEGER,      SAVE :: nkount = 1, nwarning = 0
 INTEGER :: i, ik, iev
 REAL    :: timeto, time
!
!>> initialisation
    IF( first ) then
      timpre=0.
      first=.false.
      flaok=.true.
!>> check consistent use of ktime_mubo
      if( j /= 1 ) then
        flaok=.false.
        print *,'**ktime_mubo** First call to ktime_mubo with j /= 1 and ',name
        print *,'  ======> the routine is switched off!!'
        RETURN
      endif
!>> put to zero arrays
      lname(:)    = ' '
      itimea(:,:) = 0
      ktimea(:,:) = 0.
!>> initialisation of array for first call
      lname(1)=name
      nkount=1
    endif
!
!>> return if bad uses of ktime_mubo have been detected
    if( .not.flaok ) then
      nwarning = nwarning + 1
      IF( nwarning <= 6 ) print *,'**ktime_mubo** The routine is switched off!!'
      RETURN
    endif
!
!>> check consistent use of ktime_mubo
    if( j < 1 .or. j > 3 ) then
      flaok=.false.
      print *,'**ktime_mubo** Call to ktime_mubo with j<1 or j>3 and ',name
      print *,'  ======> The routine is switched off!!'
      RETURN
    endif
!
!>> Final call (print out)
    if(j == 3) then
      iev = 1
      DO i=1,nkount
        if( lname(i)(1:5) == 'event' ) iev = i
        flaok=.false.
!>> check consistent use of ktime_mubo
        if( itimea(1,i) == 1 ) then
          print *,'**ktime_mubo** Call to ktime_mubo with j == 3 but'
          print *,'**ktime_mubo** without all end calls : ',lname(i) 
          print *,'  ======> the routine is switched off!!'
          RETURN
        endif
      ENDDO
!>> Print out
      PRINT 1000
1000  FORMAT(/3X,'**ktime_mubo** Time statistic (in sec?) :')
      PRINT 2000
2000  FORMAT('           |             |   #   |   time of   | time / call |','             |             |'/       &
             '   name    |  total time | calls |  first call | w.o. 1st c. |',' time / call | time / event|'/       &
             '   --------+-------------+-------+-------------+-------------+','-------------+-------------|')
      DO i=1,nkount
        PRINT 3000,lname(i),ktimea(1,i),itimea(2,i),ktimea(2,i),(ktimea(1,i)-ktimea(2,i))/max(1.,itimea(2,i)-1.),    &
                            ktimea(1,i)/itimea(2,i),ktimea(1,i)/itimea(2,iev)
3000    FORMAT(3X,A8,'|',F13.2,'|',I7,'|',F13.2,'|',F13.2,'|',F13.2,'|',F13.2,'|')
      ENDDO
      RETURN
    endif
!
!>> find the index for name
    do ik=1,nkount
      if( name == lname(ik) ) goto 21
    enddo
    ik=0
21  CONTINUE
!
!>> if first call update arrays and counter
    if( ik == 0 ) then
!>> check if max capacity has been reached
      if( nkount >= maxnam ) then
        print *,'**ktime_mubo** Too many counters to be set'
        print *,'  ======> No new counter set!!'
      else
        nkount=nkount+1
        ik=nkount
        lname(ik)=name
      endif
    endif
!
!>> check consistent use of ktime_mubo
    if( itimea(1,ik) == 0 .and. j /= 1 ) then
      flaok=.false.
      print *,'**ktime_mubo** First call to ktime_mubo with j /= 1 and ',name
      print *,'  ======> the routine is switched off!!'
      RETURN
    endif
!
!>> update time counters
!*****************************
    call cpu_time(timeto)
    time = timeto - timpre
    timpre = timeto
!*****************************
    ktimea(1,1:nkount) = ktimea(1,1:nkount) + time*itimea(1,1:nkount)
!
!>> switch on/off counter and set nber of calls counter
    if( j == 1 ) then
      itimea(1,ik) = 1
      itimea(2,ik) = itimea(2,ik) + 1
    else
      itimea(1,ik) = 0
      if(itimea(2,ik) == 1)  ktimea(2,ik) = ktimea(1,ik)
    endif
!
 END SUBROUTINE ktime_mubo
!
