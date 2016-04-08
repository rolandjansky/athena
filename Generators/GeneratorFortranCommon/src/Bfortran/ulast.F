*CMZ :          02/08/2001  11.48.54  by  Maria Smizanska
*-- Author :    Maria Smizanska   20/11/96

      SUBROUTINE ulast
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : ulast                                                         *
*        (module)                                                      *
*                                                                      *
* Description :                                                        *
*        Booking of NTUPLES 500 5001 5002                              *
*        Another histogram/ntuple booking can be added by user here    *
* Arguments   :                                                        *
*        i1 , i2   not used       [description]                        *
*                                                                      *
* Banks/Tables:                                                        *
*        [name]  ([filled | used | modified])                          *
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Author : Maria Smizanska                    Date : 20/11/2001          *
*                                                                      *
*                                                                      *
* Last modifications :                                                 *
*        [name]                               Date : [date]            *
*                                                                      *
* Keywords :                                                           *
*        [keywords]                                                    *
*----------------------------------------------------------------------*
*
C +CDE, TYPING.

         print *,'ULAST: my ulast is called'
        CALL HROUT(0)
c        CALL HREND('AT')
*
      END          ! ulast
