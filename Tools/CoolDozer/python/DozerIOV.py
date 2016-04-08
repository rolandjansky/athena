#!/bin/env python 

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
# @file DozerIOV.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief DozerIOV class definition

import sys
sys.argv.append("-b") 


import datetime
import time 
import re
from types import *

import DozerObject
from DozerUtils import DozerLogger, DozerError
import unittest

from PyCool import cool, coral
import ROOT

## 
# @class DozerIOV
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief Class for various timestamp manipulation.
#
# @htmlinclude cool_dozer_api.css
# <h3>Date and time representation</h3>
#
# DozerIOV (Dozer <b>I</b>nterval <b>O</b>f <b>V</b>alidity) class does the transformation between different time stamp representation:
# <ul>
# <li> Unix timestamp is a number of seconds since the Epoch (01/01/1970), on 32bit systems it's valid until 19/01/2038 
# because of the overflow, for 64bit this overflow may occure in about 290 billion years from now    
# <li> Cool timestamp is a number of nanosecond staring from the Unix Epoch and it's valid until 04/12/2262 
# <li> ROOT timestamp (TDatime) object is a number of seconds since the ROOT Epoch (01/01/1995)
# (and it's valid until 19/01/2038 or A.D. 290 billion depending of 32bit-64bit binary)
# </ul>
# 
# All those timestamps are not human readable, but for this one can use ISO representation of the date:
# <pre>YYYY-MM-DD hh:mm:ss</pre>
# where:
# <ul>
# <li> <i>YYYY</i> is a year number in the Gregorian calendar
# <li> <i>MM</i> is a month (01-January, ... ,12-December)
# <li> <i>DD</i> is a day number (01-31) 
# </ul>
# Time of the day is using the 24-hour timekeeping system, where:
# <ul>
# <li> <i>hh</i> is the number of complete hours that have passed since midnight (00)
# <li> <i>mm</i> is the number of complete minutes since the start of the hour
# <li> <i>ss</i> is the number of complete seconds since the start of the minute
# </ul>
#
# All the timestamps are using local time (not UTC/GMT), e.g. in Geneve region it's
# CEST or CET (depending if the daylight saving time) and the offset from UTC/GMT  
# is +1 hour for CET and +2 houres for CEST. 
# 
# For the Atlas test frameworks the natural unit of time is not a nanosecond (as in Cool) or second (Unix or ROOT) and the 
# best choice following the rules of software building is one day. This is why the timestamps produced by DozerIOV are
# <b>always</b> rounded to the midnight - the time of day is substracted. 
#
# <h3>Usage</h3>
# 
# <ul>
# <li> <h4>Constructing</h4>
#
# To construct DozerIOV object one can choose between a few different methods. The most natural way is to call the constructor 
# DozerIOV::__init__, e.g.:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#[lxplus236] ~/CoolDozer/python % python
#Python 2.5 (r25:51908, Oct 18 2007, 16:04:48) 
#[GCC 3.4.6 20060404 (Red Hat 3.4.6-8)] on linux2
#Type "help", "copyright", "credits" or "license" for more information.
#>>> import datetime
#>>> str( datetime.datetime.today() )
#'2008-06-20 11:49:47.784523'
#>>> import DozerIOV
#>>> d = DozerIOV.DozerIOV()
#>>> d.AsISO()
#'2008-06-20 00:00:00' 
#</pre></b>
#</div></div></div>
#
# which by default sets the cool time stamp to today's midnight. Optional arguments are:
# <ul> 
# <li> <tt>coolStamp</tt> long int or int e.g.:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> d = DozerIOV.DozerIOV(0)
#>>> d.AsISO()
#'1970-01-01 00:00:00'
#>>> d = DozerIOV.DozerIOV(84600000000000L)
#>>> d.AsISO()
#'1970-01-02 00:00:00'
#</pre></b>
#</div></div></div>
#
#<li> <tt>log</tt> DozerUtils::DozerLogger instance 
# </ul>
#
# Another method to construct DozerIOV is to call one of it's classmethods (which are very similar to the C++ static member functions), particulary
# DozerIOV::fromUnix, DozerIOV::fromISO, DozerIOV.fromTDatime, DozerIOV.fromRelease, e.g.:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> isoString = "2008-06-20 13:45:12"
#>>> d = DozerIOV.DozerIOV.fromISO( isoString )
#>>> d.AsISO()
#'2008-06-20 00:00:00'
#>>> import ROOT
#>>> rootStamp = TDatime()
#>>> print rootStamp.AsString()
#Fri Jun 20 12:29:50 2008
#>>> d = DozerIOV.DozerIOV.fromTDatime( rootStamp )
#>>> d.AsISO()
#'2008-06-20 00:00:00'
#>>> releaseStamp = "rel_5"
#>>> d = DozerIOV.DozerIOV.fromRelease( releaseStamp )
#>>> d.AsISO()
#'2008-06-20 00:00:00'
#>>> import time
#>>> unixStamp = time.time()
#1213958147.187655
#>>> d = DozerIOV.DozerIOV.fromUnix( unixStamp )
#>>> d.AsISO()
#'2008-06-20 00:00:00'
#</pre></b> 
#</div></div></div>
#
# <li> <h4>Reading back and converting</h4>
#  
# Calling DozerIOV instance returns cool time stamp, e.g.:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> d = DozerIOV.DozerIOV()
#>>> d
#1213912800000000000L
#>>> d()
#1213912800000000000L
#>>> d.__call__()
#1213912800000000000L
#</pre></b>
#</div></div></div>
#
# Once the instance is created one can convert is to different representation using AsXXX methods, e.g.:
#
# <ul>
# <li> DozerIOV::AsUnix
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> d = DozerIOV.DozerIOV()
#>>> d.AsUnix()
#1213912800L
#</pre></b>  
#</div></div></div>
#
#<li> DozerIOV::AsTDatime
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> import ROOT
#>>> d = DozerIOV.DozerIOV()
#>>> d.AsTDatime()
#<ROOT.TDatime object at 0x1f3d020>
#>>> d.AsTDatime.AsString()
#'Fri Jun 20 00:00:00 2008'
#</pre></b>
#</div></div></div>
#
# <li> DozerIOV::AsISO (already used in examples)
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> d = DozerIOV.DozerIOV()
#>>> d.AsISO()
#'2008-06-20 00:00:00'
#</pre></b>
#</div></div></div>
#
# <li> DozerIOV::AsCool (added for compatilbility reason) 
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> d = DozerIOV.DozerIOV()
#>>> d.AsCool()
#1213912800000000000L
#</pre></b>
#</div></div></div>
#
# <li> DozerIOV.AsRelease
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> d = DozerIOV.DozerIOV(1213912800000000000L)
#>>> d.AsRelease()
#'rel_5'
#</pre></b>
#</div></div></div>
# </ul>
#
# <li> <h4>Other (not so useful?) methods</h4>
# 
# <ul>
# <li> DozerIOV::lastNDays to get the cool time stamp for a few last days (in the past or the future,
# the name is a bit misleading)
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> d = DozerIOV.DozerIOV()
#>>> d.AsISO()
#'2008-06-20 00:00:00'
#>>> lastWeek = d.lastNDays( 7 )
#>>> DozerIOV.DozerIOV( lastWeek ).AsISO()
#'2008-06-13 00:00:00'
#>>> nextWeek = d.lastNDays( -7 )
#>>> DozerIOV.DozerIOV( nextWeek ).AsISO()
#'2008-06-27 00:00:00'
#</pre></b>
#</div></div></div>
#
# <li>  DozerIOV::max to get cool time stamp upper limit 
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> d = DozerIOV.DozerIOV().max()
#9223369200000000000L
#</pre></b> 
#</div></div></div>
#
# <li> DozerIOV::__str__ to get string representation of DozerIOV 
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#>>> d = DozerIOV.DozerIOV()
#>>> print d
#cool=1213912800000000000 unix=1213912800 ISO=2008-06-20 00:00:00
#>>> str(d)
#'cool=1213912800000000000 unix=1213912800 ISO=2008-06-20 00:00:00'
#>>> d.__str__()
#'cool=1213912800000000000 unix=1213912800 ISO=2008-06-20 00:00:00'
#</pre></b>
#</div></div></div>
# </ul>
# 
# </ul>
#
class DozerIOV( DozerObject.DozerObject ):

    ## constant factor (number of nanoseconds in one second) between unix and cool
    __unix2cool = 1000000000
    
    ## constant factor between cool and unix
    __cool2unix = 1./1000000000.
    
    #__maxAsUnix = sys.maxint

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param coolStamp cool time stamp
    # @param log DozerLogger instance
    def __init__( self, coolStamp=None, log=None ):
        super( DozerIOV, self ).__init__( log )
        if ( type(coolStamp) in ( LongType, IntType ) ):
            try:
                self.__coolStamp = self.__atMidnight( coolStamp )
            except ValueError:
                self.__coolStamp = long( coolStamp )
        else:
            self.__coolStamp = self.today().AsCool()
            
    @classmethod 
    def __atMidnight( cls, coolStamp ):
        stamp = datetime.datetime.fromtimestamp( coolStamp * cls.__cool2unix )
        stamp = time.mktime( datetime.datetime( stamp.year, 
                                                stamp.month, 
                                                stamp.day ).timetuple() ) 
        return long( stamp * cls.__unix2cool )


    ## cool.ValidityKeyMax getter
    # @param cls class reference
    @classmethod
    def max( cls ):
        return cls( cool.ValidityKeyMax ).AsCool()

    ## return cool stamp for next day from current time stamp  
    # @param self "Me, myself and Irene"
    def nextDay( self ):
        
        nextDay = datetime.datetime.fromtimestamp( self.AsUnix() )
        dt = datetime.timedelta( +1 )
        nextDay += dt
        nextDay = time.mktime( datetime.datetime( nextDay.year, 
                                                  nextDay.month, 
                                                  nextDay.day ).timetuple() ) 

        return  long( self.__unix2cool * nextDay )

    ## today cool imestamp getter
    # @param cls class reference
    # @return DozerIOV for today midnight
    @classmethod
    def today( cls ):
        today = datetime.datetime.today()
        today = time.mktime( datetime.datetime( today.year, 
                                                today.month, 
                                                today.day ).timetuple() ) 
        return DozerIOV( long( cls.__unix2cool * today ) )

    ## last N days cool time stamp getter
    # @param cls class reference
    # @param ndays number of days (default = 1, may be positive or negative number)
    # @return cool timestamp for the midnight of last N days from today
    @classmethod
    def lastNDays( cls, ndays=1 ):
        lastNDays = datetime.datetime.today()
        dt = datetime.timedelta( -ndays )
        lastNDays += dt
        lastNDays = time.mktime( datetime.datetime( lastNDays.year, 
                                                    lastNDays.month, 
                                                    lastNDays.day ).timetuple() ) 
        return long( cls.__unix2cool * lastNDays )

    ## cool timestamp representation 
    # @param self "Me, myself and Irene"
    # @return cool timestamp (added for compatibility)
    def AsCool( self ):
        return self.__coolStamp

    ## unix timestamp representation 
    # @param self "Me, myself and Irene"
    # @return unix timestamp 
    def AsUnix( self ):
        return long( self.__coolStamp * self.__cool2unix )
        
    ## ROOT.TDatime representation 
    # @param self "Me, myself and Irene"
    # @return ROOT.TDatime object
    # @throw DozerError if year < 1995 
    def AsTDatime( self ):
        unix = datetime.datetime.fromtimestamp( self.AsUnix() )
        if ( unix.year < 1995 ): raise DozerError("cannot construct ROOT.TDatime object, year must be >= 1995!")
        return ROOT.TDatime( unix.year,
                             unix.month,
                             unix.day,
                             unix.hour, 
                             unix.minute,
                             unix.second  )

    ## ISO representation 
    # @param self "Me, myself and Irene"
    # @return string with ISO representation of timestamp 
    def AsISO( self ):
        unix = None
        try:
            unix = datetime.datetime.fromtimestamp( self.AsUnix() )
        except ValueError, value:
            self.error( value )
        return str( unix )

    ## Atlas release name representation
    # @param self "Me, myself and Irene"
    # @return string with rel_N, where N is a number of week day (0-Sunday,1-Monday,...,6-Saturday)
    def AsRelease( self ):
        weekday = datetime.datetime.fromtimestamp( self.AsUnix() ).weekday() + 1 
        if ( weekday == 7 ): weekday = 0
        return "rel_%d" % weekday

    ## () operator
    # @param self "Me, myself and Irene"
    # @return cool timestamp
    def __call__( self ):
        return self.__coolStamp

    ## str() operator
    # @param self "Me, myself and Irene"
    # @return string representation of DozerIOV
    def __str__( self ):
        asCool = None
        asUnix = None
        asISO = None
        asRelease = None
        try:
            asCool = self.AsCool()
            asUnix = self.AsUnix()
            asISO = self.AsISO()
            asRelease = self.AsRelease()
        except ValueError:
            pass
        return "cool=%s unix=%s ISO=%s rel=%s" % ( str(asCool), str(asUnix), str(asISO), str(asRelease) )

    ##
    # @param cls clas reference
    # @param iso string with ISO date
    # @param log CoolDozerLogger instance
    # @brief DozerIOV builder from ISO string
    @classmethod
    def fromISO(cls, iso, log=None):
        words = re.findall('\d+', iso )
        if ( len(words) != 6 ):
            raise DozerError("too few or too many value(=%d) to unpack from iso string='%s'" % ( len(words),
                                                                                     iso ) )
        else:
            try:
                words = [int(word) for word in words ]
                unix = time.mktime( datetime.datetime( words[0],
                                                       words[1],
                                                       words[2],
                                                       words[3],
                                                       words[4],
                                                       words[5] ).timetuple() )
                return cls.fromUnix( unix, log ) 
            except TypeError, value:
                raise DozerError("cannot convert string to int in DozerIOV.fromISO() call")

    ##
    # @param cls class reference
    # @param tdatime ROOT.TDatime object
    # @param log CoolDozerLogger instance
    # @brief DozerIOV builder from ROOT.TDatime
    @classmethod 
    def fromTDatime( cls, tdatime, log=None ):
        unix = time.mktime( datetime.datetime( tdatime.GetYear(),
                                               tdatime.GetMonth(),
                                               tdatime.GetDay(),
                                               tdatime.GetHour(),
                                               tdatime.GetMinute(),
                                               tdatime.GetSecond() ).timetuple() )
        return cls.fromUnix( unix, log ) 
        
    ##
    # @param release string with release name i.e. "rel_0", "rel_1"
    # @param cls class reference
    # @param log DozerLogger instance
    # @brief DozerIOV builder from release name
    # @warning it goes only back in time 
    @classmethod
    def fromRelease( cls, release, log=None ):
        if ( type(release) in StringTypes and release.startswith("rel_") ):
            try:
                release = int( release.lstrip("rel_") ) 
            except ValueError, value:
                raise DozerError("cannot convert release to the day of the week") 
            if ( release not in range( 7 ) ):
                raise DozerError("cannot construct DozerIOV, release day of week out of range [0,6]")
            today = datetime.datetime.today()
            weekday = today.weekday() + 1
            if ( weekday == 7 ): weekday = 0
            if ( weekday == release ):
                return cls( log = log )
            elif ( weekday > release ):
                return cls( cls.lastNDays( weekday - release ), log = log )
            else:
                return cls( cls.lastNDays( ( 6 - release ) + weekday + 1 ), log=log )
        
    ##
    # @param cls class reference
    # @param coolStamp cool time stamp
    # @param log CoolDozerLogger instance
    # @brief DozerIOV builder from cool time stamp 
    @classmethod
    def fromCool( cls, coolStamp, log=None ):
        return cls( long(coolStamp), log)

    ## 
    # @param cls class reference
    # @param unixStamp unixt time stamp
    # @param log CoolDozerLogger instance
    # @brief DozerIOV builder from Unix timestamp
    @classmethod
    def fromUnix( cls, unixStamp, log=None ):
        return cls( long(unixStamp*cls.__unix2cool), log )
    
    ##
    # __cmp__ operator
    # @param self "Me, myself and Irene"
    # @param other DozerIOV instance
    def __cmp__( self, other ):
        if ( isinstance( other, DozerIOV) ): 
            diff = long( self.AsCool() - other.AsCool() )    
            if ( diff > 0L ): return 1
            if ( diff == 0L): return 0
        return -1
        
    ## == operator
    # @param self "Me, myself and Irene"
    # @param other DozerIOV instance 
    #def __eq__( self, other ):
    #    if ( isinstance( other, DozerIOV ) ):
    #        if ( self.__coolStamp == other() ): return True
    #    return False 
        

    ## != operator
    # @param self "Me, myself and Irene"
    # @param other DozerIOV instance
    #def __ne__( self, other ):
    #    if ( isinstance( other, DozerIOV) ):
    #        if ( self.__coolStamp != other() ): return True
    #
    #    return False

    ## < operator 
    # @param self "Me, myself and Irene"
    # @param other DozerIOV instance
    #def __lt__(self, other):
    #    if ( isinstance(other, DozerIOV ) ):
    #        if ( self.__coolStamp < other() ): return True
    #    return False 

    ## <= operator
    # @param self "Me, myself and Irene"
    # @param other DzoerIOV instance
    #def __le__( self, other ):
    #    if ( isinstance(other, DozerIOV) ): 
    #        if ( self.__coolStamp <= other() ): return True
    #    return False

    ## > operator
    # @param self "Me, myself and Irene"
    # @param other DozerIOV instance
    #def __gt__( self, other ):
    #    if ( isinstance(other, DozerIOV) ):
    #        if ( self.__coolStamp > other() ): return True
    #    return False

    ## >= operator
    # @param self "Me, myself and Irene"
    # @param other DozerIOV instance
    #def __ge__(self, other):
    #    if ( isinstance(other, DozerIOV) ):
    #        if ( self.__coolStamp >= other() ): return True
    #    return False 

            
##
# @class test_DozerIOV
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com) 
# @brief test case fro DozerIOV
class test_DozerIOV( unittest.TestCase ):

    ## setup 
    # @param self "Me, myself and Irene"
    def setUp( self ):
        self.__log = DozerLogger("DozerIOV_unittest.log", "test_DozerIOV", toConsole=True)
        self.__log.toConsole( True )

    ## __init__()
    # @param self "Me, myself and Irene"
    def test_01_constructor( self ):
        iov = DozerIOV( log = self.__log )
        self.assertEqual( isinstance(iov, DozerIOV), True)
    
    ## AsCool(), AsUnix(), AsTDatime(), AsISO(), AsRelease()
    # @param self "Me, myself and Irene"
    def test_02_conversions( self ):
        iov = DozerIOV( log = self.__log )
        
        self.assertEqual( iov(), iov.AsCool() )
        self.__log.getLogger( self ).info( iov )
        self.__log.getLogger( self ).info( iov() )
        self.__log.getLogger( self ).info( iov.AsCool() )
        self.__log.getLogger( self ).info( iov.AsUnix() )
        self.__log.getLogger( self ).info( iov.AsTDatime().AsString() )
        self.__log.getLogger( self ).info( iov.AsISO() )
        self.__log.getLogger( self ).info( iov.AsRelease() )

    ##  fromCool(), fromUnix(), fromTDatime(), fromISO(), fromReleaes() max()
    # @param self "Me, myself and Irene"
    def test_03_from( self ):

        iov = DozerIOV( log=self.__log )
        self.__log.getLogger( self ).info( iov )
        asCool = iov.AsCool()
        asUnix = iov.AsUnix()
        asTDatime = iov.AsTDatime()
        asISO = iov.AsISO()
        asRelease = iov.AsRelease()

        iov = DozerIOV.fromCool( asCool, log=self.__log )
        self.__log.getLogger( self ).info( iov )
        self.assertEqual( isinstance(iov, DozerIOV), True )

        iov = DozerIOV.fromUnix( asUnix, log=self.__log )
        self.__log.getLogger( self ).info( iov )
        self.assertEqual( isinstance(iov, DozerIOV), True )

        iov = DozerIOV.fromTDatime( asTDatime, log=self.__log )
        self.__log.getLogger( self ).info( iov )
        self.assertEqual( isinstance(iov, DozerIOV), True )

        iov = DozerIOV.fromISO( asISO, log=self.__log )
        self.__log.getLogger( self ).info( iov )
        self.assertEqual( isinstance(iov, DozerIOV), True )

        iov = DozerIOV.fromRelease( asRelease, log=self.__log )
        asTDatime = iov.AsTDatime()
        self.__log.getLogger( self ).info( iov )
        self.assertEqual( isinstance(iov, DozerIOV), True )

        iov = DozerIOV.fromRelease( "rel_0", log=self.__log )
        asRelease = iov.AsRelease()
        self.__log.getLogger( self ).info( str(iov) + " asRelease " + asRelease )
        self.assertEqual( isinstance(iov, DozerIOV), True )
        self.assertEqual( asRelease, "rel_0")

        iov = DozerIOV.fromRelease( "rel_1", log=self.__log )
        asRelease = iov.AsRelease()
        self.__log.getLogger( self ).info( str(iov) + " asRelease " + asRelease )
        self.assertEqual( isinstance(iov, DozerIOV), True )
        self.assertEqual( asRelease, "rel_1")

        iov = DozerIOV.fromRelease( "rel_5", log=self.__log )
        nextDay = DozerIOV( iov.lastNDays(-1) )
        asRelease = iov.AsRelease()
        self.__log.getLogger( self ).info( str(iov) + " asRelease " + asRelease )
        self.__log.getLogger( self ).info( str(nextDay)  )
                
        self.assertEqual( isinstance(iov, DozerIOV), True )
        self.assertEqual( asRelease, "rel_5")


    ## max()
    # @param self "Me, myself and Irene"
    def test_04_max( self ):
        iov = DozerIOV( DozerIOV.max(), log=self.__log )
        self.__log.getLogger( self ).info( iov )
        self.assertEqual( isinstance(iov, DozerIOV), True )


    ## == , != , <, <=, >, >= operators
    # @param self "Me, myself and Irene"
    def test_05_eqnegtltgele( self ):
        
        tod = DozerIOV()
        tom = DozerIOV( tod.lastNDays(-1) )
        yes = DozerIOV( tod.lastNDays(1) ) 
        
        self.assertEqual( tod == DozerIOV.today(), True )
        self.assertEqual( tod != tom, True )

        self.assertEqual( tod > yes, True )
        self.assertEqual( tod >= yes, True )

        self.assertEqual( tod < tom, True )
        self.assertEqual( tod <= tom, True )


    def test_06_fromRelease( self ):

        for i in range(7):
            print i, DozerIOV.fromRelease( "rel_%d" % i )
    

## test suite execution
if __name__ == "__main__":
    testLoader = unittest.TestLoader()
    suite = testLoader.loadTestsFromTestCase(test_DozerIOV)      
    unittest.TextTestRunner(verbosity=3).run(suite)
