#!/usr/bin/env python

#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

import unittest

from AthenaConfiguration.AccumulatorCache import AccumulatorCache , AccumulatorDecorator

class TestCache(unittest.TestCase):
    def test_basic(self):
        """Basic tests. Simple caching and introspection."""

        class hashwrapper:
            def __init__(self , x):
                self.x = x
            def athHash(self):
                return hash(self.x)

        @AccumulatorCache(verifyResult = AccumulatorDecorator.VERIFY_NOTHING , deepCopy = False)
        def returnSame(x):
            """returns the same number"""
            return x
        y1 = returnSame(hashwrapper(1))
        y2 = returnSame(hashwrapper(2))
        y3 = returnSame(hashwrapper(2))
        y4 = returnSame(hashwrapper(3))

        self.assertEqual(y1.x , 1)
        self.assertEqual(y2.x , 2)
        self.assertEqual(y3.x , 2)
        self.assertEqual(y4.x , 3)
        
        info = returnSame.getInfo()
        self.assertEqual(info["cache_size"] , 3)
        self.assertEqual(info["hits"] , 1)
        self.assertEqual(info["misses"] , 3)

        self.assertEqual(returnSame.__name__ , "returnSame")
        self.assertEqual(returnSame.__doc__ , "returns the same number")

        AccumulatorDecorator.printStats()

        @AccumulatorCache(verifyResult = AccumulatorDecorator.VERIFY_NOTHING , deepCopy = False)
        def fac(x):
            return hashwrapper(1 if x.x == 0 else x.x * fac(hashwrapper(x.x - 1)).x)

        y = fac(hashwrapper(10))
        self.assertEqual(y.x , 3628800)
        y = fac(hashwrapper(11))
        info = fac.getInfo()
        self.assertEqual(info["hits"] , 1)

    def test_exception(self):
        """Test cache when function throws exception."""

        class MyException(BaseException):
            pass

        @AccumulatorCache
        def throw():
            raise MyException()

        # Make sure no other exception is thrown by decorator itself
        self.assertRaises(MyException, throw)

    def test_cache_limit(self):
        """Test cache limits."""

        class hashwrapper:
            def __init__(self , x):
                self.x = x
            def athHash(self):
                return hash(self.x)

        @AccumulatorCache(verifyResult = AccumulatorDecorator.VERIFY_NOTHING , deepCopy = False)
        def returnDouble(x):
            return hashwrapper(2 * x.x)

        for ii in range(256):
            y = returnDouble(hashwrapper(ii))
            self.assertEqual(y.x , 2 * ii)
            if(ii > 127):
                info = returnDouble.getInfo()
                self.assertEqual(info["cache_size"] , 128)

        @AccumulatorCache(maxSize = 10 , verifyResult = AccumulatorDecorator.VERIFY_NOTHING , deepCopy = False)
        def returnTripple(x):
            return hashwrapper(3 * x.x)

        for ii in range(256):
            y = returnTripple(hashwrapper(ii))
            self.assertEqual(y.x , 3 * ii)
            if(ii > 9):
                info = returnTripple.getInfo()
                self.assertEqual(info["cache_size"] , 10)

        for ii in range(128 , 138):
            y = returnDouble(hashwrapper(ii))
            info = returnDouble.getInfo()
            self.assertEqual(info["hits"] , ii - 128 + 1)

        for ii in range(246 , 256):
            y = returnTripple(hashwrapper(ii))
            info = returnTripple.getInfo()
            self.assertEqual(info["hits"] , ii - 246 + 1)
        y = returnTripple(hashwrapper(245))
        self.assertEqual(info["hits"] , 10)

    def test_validation(self):
        """Test if cached results were modified."""

        class hashwrapper:
            def __init__(self , x):
                self.x = x
            def athHash(self):
                return hash(self.x)

        class tamper:
            def __init__(self , x):
                self.x = x
            def athHash(self):
                return hash(self.x)
            def __hash__(self):
                raise DeprecationWarning("Please use athHash().")

        @AccumulatorCache(verifyResult = AccumulatorDecorator.VERIFY_HASH , deepCopy = False)
        def checkVerify(x):
            return tamper(x.x)

        r1 = checkVerify(hashwrapper(1))
        r1.x = 821
        r2 = checkVerify(hashwrapper(1))

        self.assertEqual(r2.x , 1)
        info = checkVerify.getInfo()
        self.assertEqual(info["hits"] , 0)
        self.assertEqual(info["misses"] , 2)
        self.assertEqual(info["cache_size"] , 1)
        
        r3 = checkVerify(hashwrapper(1))
        self.assertEqual(r3.x , 1)
        info = checkVerify.getInfo()
        self.assertEqual(info["hits"] , 1)

        @AccumulatorCache(verifyResult = AccumulatorDecorator.VERIFY_HASH , deepCopy = False)
        def checkVerify1(x):
            return tamper(x.x)
        
        for ii in range(256):
            y = checkVerify1(hashwrapper(ii))
            self.assertEqual(y.x , ii)
            info = checkVerify1.getInfo()
            self.assertEqual(info["cache_size"] , info["result_cache_size"])
            if(ii > 127):
                self.assertEqual(info["cache_size"] , 128)
                self.assertEqual(info["result_cache_size"] , 128)
        
        @AccumulatorCache(verifyResult = AccumulatorDecorator.VERIFY_HASH , maxSize = 10 , deepCopy = False)
        def checkVerify2(x):
            return tamper(x.x)
        
        for ii in range(256):
            y = checkVerify2(hashwrapper(ii))
            self.assertEqual(y.x , ii)
            info = checkVerify2.getInfo()
            self.assertEqual(info["cache_size"] , info["result_cache_size"])
            if(ii > 9):
                self.assertEqual(info["cache_size"] , 10)
                self.assertEqual(info["result_cache_size"] , 10)

    def test_suspend(self):
        """Test suspendCaching and resumeCaching."""

        class hashwrapper:
            def __init__(self , x):
                self.x = x
            def athHash(self):
                return hash(self.x)

        @AccumulatorCache(verifyResult = AccumulatorDecorator.VERIFY_HASH , deepCopy = False)
        def plusone(x):
            return hashwrapper(x.x + 1)

        AccumulatorDecorator.suspendCaching()
        
        r1 = plusone(hashwrapper(1))
        r2 = plusone(hashwrapper(2))
        r3 = plusone(hashwrapper(2))
        info = plusone.getInfo()
        self.assertEqual(info["hits"] , 0)
        self.assertEqual(info["misses"] , 0)
        self.assertEqual(info["cache_size"] , 0)
        self.assertEqual(r1.x , 2)
        self.assertEqual(r2.x , 3)
        self.assertEqual(r3.x , 3)

        AccumulatorDecorator.resumeCaching()

        r1 = plusone(hashwrapper(1))
        r2 = plusone(hashwrapper(2))
        r3 = plusone(hashwrapper(2))
        info = plusone.getInfo()
        self.assertEqual(info["hits"] , 1)
        self.assertEqual(info["misses"] , 2)
        self.assertEqual(info["cache_size"] , 2)
        self.assertEqual(r1.x , 2)
        self.assertEqual(r2.x , 3)
        self.assertEqual(r3.x , 3)

    def test_verify_copy(self):

        class hashwrapper:
            def __init__(self , x):
                self.x = x
            def athHash(self):
                return hash(self.x)

        @AccumulatorCache(verifyResult = AccumulatorDecorator.VERIFY_NOTHING , deepCopy = True)
        def appendone(x):
            return [x.x] + [1]

        r1 = appendone(hashwrapper(1))
        r2 = appendone(hashwrapper(2))
        r2[0] = 321
        r3 = appendone(hashwrapper(2))

        self.assertEqual(r1 , [1 , 1])
        self.assertEqual(r2 , [321 , 1])
        self.assertEqual(r3 , [2 , 1])

        info = appendone.getInfo()
        self.assertEqual(info["hits"] , 1)
        self.assertEqual(info["misses"] , 2)
        self.assertEqual(info["cache_size"] , 2)

        class tamperB:
            def __init__(self , x):
                self.x = x
            def athHash(self):
                return hash(self.x)
            def __hash__(self):
                raise DeprecationWarning("Please use athHash().")

        @AccumulatorCache(verifyResult = AccumulatorDecorator.VERIFY_NOTHING , deepCopy = True)
        def checkTamper(x):
            return tamperB(x)

        r1 = checkTamper(1)
        r1.x = 123
        r2 = checkTamper(1)
        r2.x = 321
        r3 = checkTamper(1)
        r3.x = 213
        r4 = checkTamper(1)
        self.assertEqual(r4.x , 1)


    def test_verify_default(self):

        class hashwrapper:
            def __init__(self , x):
                self.x = x
            def athHash(self):
                return hash(self.x)

        @AccumulatorCache # The default value shuld be DEEP_COPY
        def appendone1(x):
            return [x.x] + [1]

        r1 = appendone1(hashwrapper(1))
        r2 = appendone1(hashwrapper(2))
        r2[0] = 321
        r3 = appendone1(hashwrapper(2))
        r4 = appendone1(hashwrapper(2))

        self.assertEqual(r1 , [1 , 1])
        self.assertEqual(r2 , [321 , 1])
        self.assertEqual(r3 , [2 , 1])

        r3[0] = 123
        
        self.assertEqual(r4 , [2 , 1])

        info = appendone1.getInfo()
        self.assertEqual(info["hits"] , 2)
        self.assertEqual(info["misses"] , 2)
        self.assertEqual(info["cache_size"] , 2)

        @AccumulatorCache(verifyResult = AccumulatorDecorator.VERIFY_NOTHING , deepCopy = False) 
        def appendone2(x):
            return x + [1]

        r1 = appendone2([1])
        r2 = appendone2([2])
        r2[0] = 321
        r3 = appendone2([2])

        self.assertEqual(r1 , [1 , 1])
        self.assertEqual(r2 , [321 , 1])
        self.assertEqual(r3 , [2 , 1]) #!

        info = appendone2.getInfo()
        self.assertEqual(info["hits"] , 0)
        self.assertEqual(info["misses"] , 3)
        self.assertEqual(info["cache_size"] , 0)


class TestCA(unittest.TestCase):
    """
    ComponentAccumulator specific tests
    """

    def setUp(self):
        """Add handler for ERROR messages"""
        import io
        from AthenaCommon.Logging import logging

        self.errors = io.StringIO()
        handler = logging.StreamHandler(self.errors)
        handler.setLevel(logging.ERROR)
        logging.getLogger().addHandler(handler)

    def test_private_tools(self):
        """Test caching of CAs with private tools."""
        from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
        from AthenaConfiguration.ComponentFactory import CompFactory

        @AccumulatorCache(deepCopy = True)
        def cfg():
            acc = ComponentAccumulator()
            acc.setPrivateTools(CompFactory.AthenaOutputStreamTool())
            return acc

        acc = cfg()
        acc.popPrivateTools()
        del acc  # no ERROR here as we consumed the private tools
        del cfg  # this produces an ERROR if private tools of cached CAs are not deleted
        self.assertTrue(len(self.errors.getvalue())==0)

if __name__ == '__main__':
    unittest.main()
