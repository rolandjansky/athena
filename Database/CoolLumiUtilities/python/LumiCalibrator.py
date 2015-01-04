# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# LumiCalibrator
#
# Eric Torrence - September 2011
#
# Contents:
# LumiCalibrator - utility class to apply the luminosity calibration stored in COOL
#                  to raw per-bcid lumi values.
#
# This must be initialized with the payload of a calibration record from COOL, then the raw luminosity
# will be calibrated with a call to calibrate

import math

from CoolLumiUtilities.LumiBlobConversion import bConvertList

class LumiCalibrator:

    def __init__(self):

        self.nPar = 0
        self.fType = 'None'
        self.muToLumi = 0.
        self.parVec = []
        self.rflag = 0

        self.currVersion = 0
        self.currParA = 0.
        self.currParB = 0.
        self.currParC = 0.
        self.currType = 1 # Use A+C by default
        
        self.polyVersion = 0
        self.polyParA = 0.
        self.polyParB = 0.
        self.polyParC = 0.

        self.nBunches = 1
        
        # PMT currents
        self.currentA = -1.
        self.currentC = -1.

        self.oldCurrentA = -1.
        self.oldCurrentC = -1.
        
        self.verbose = False
        # self.verbose = True
        
        self.facTable = [1, 1]

        self.nParUsed = 0
        
    # Cached factorial values
    def factorial(self, n): 

        if n >= len(self.facTable):

            last = len(self.facTable) - 1
            total = self.facTable[last]
            for i in range(last+1, n+1):
                total *= i
                self.facTable.append(total)

        return self.facTable[n]
    
    def setCalibration(self, calibPayload):
        self.nPar = calibPayload['NumOfParameters']
        self.fType = calibPayload['Function']
        self.muToLumi = calibPayload['MuToLumi']
        blob = calibPayload['Parameters']
        s=blob.read()
        self.parVec = bConvertList(s, 4, self.nPar)

        if self.verbose:
            print 'LumiCalibrator.setCalibration for %s found %d parameters: ' % (self.fType, self.nPar)
            print self.parVec
            print ' muToLumi = %f' % self.muToLumi


        algname = self.fType
        polyPos = algname.find("_PolyCorr")
        currPos = algname.find("_CurrCorr")

        if polyPos > 0 and currPos > 0:
            algname = algname[:-18]
        elif polyPos > 0 or currPos > 0:
            algname = algname[:-9]
            
        if self.verbose:
            print 'LumiCalibrator.setCalibration - Found poly: %d curr: %d' % (polyPos, currPos)
            
        if polyPos > currPos:

            if self.verbose:
                print 'LumiCalibrator.setCalibration - reading polynomial from', self.parVec[-4:-1]
                
            self.polyParC = self.parVec.pop()
            self.polyParB = self.parVec.pop()
            self.polyParA = self.parVec.pop()
            self.polyVersion = self.parVec.pop()
            self.nPar -= 4

            if currPos > 0:

                # Call calibrate to get number of parameters
                self.calibrate(0.1)
                npar = self.nParUsed
                self.currType = 1 # Use A+C by default

                self.currVersion = int(self.parVec[npar])
                if self.verbose:
                    print 'LumiCalibrator.setCalibration - found %d parameters in %s' % (npar, algname)
                    print 'LumiCalibrator.setCalibration - reading current vers %d from position %d' % (self.currVersion, npar)                    

                if self.currVersion == 4:
                    self.currType = int(self.parVec.pop())
                    self.nPar -= 1
                    
                if self.currVersion >= 3:
                    self.currParC = self.parVec.pop()
                    self.nPar -= 1
                    
                self.currParB = self.parVec.pop()                    
                self.currParA = self.parVec.pop()
                self.currVersion = self.parVec.pop()
                self.nPar -= 3

                if self.verbose:
                    print 'LumiCalibrator.setCalibration found current correction %d type %d' % (self.currVersion, self.currType) 

        elif currPos > polyPos:

            # Call calibrate to get number of parameters
            self.calibrate(0.1)
            npar = self.nParUsed
            self.currType = 1 # Use A+C by default

            if self.verbose:
                print 'LumiCalibrator.setCalibration - found %d parameters in %s' % (npar, algname)
            if polyPos > 0:
                npar += 4

            self.currVersion = int(self.parVec[npar])

            if self.verbose:
                print 'LumiCalibrator.setCalibration - reading current vers %d from position %d' % (self.currVersion, npar)
                    
            if self.currVersion == 4:
                self.currType = int(self.parVec.pop())
                self.nPar -= 1
                
            if self.currVersion >= 3:
                self.currParC = self.parVec.pop()
                self.nPar -= 1

            self.currParB = self.parVec.pop()                    
            self.currParA = self.parVec.pop()
            self.currVersion = self.parVec.pop()
            self.nPar -= 3
            
            if self.verbose:
                print 'LumiCalibrator.setCalibration found current correction %d type %d' % (self.currVersion, self.currType)
                
            if polyPos > 0:    
                if self.verbose:
                    print 'LumiCalibrator.setCalibration - reading polynomial from  list' , self.parVec[-4:]
                
                self.polyParC = self.parVec.pop()
                self.polyParB = self.parVec.pop()
                self.polyParA = self.parVec.pop()
                self.polyVersion = self.parVec.pop()
                self.nPar -= 4


    def setLucidCurrent(self, currPayload):

        self.oldCurrentA = self.currentA
        self.oldCurrentC = self.currentC
        
        if currPayload == None:
            self.currentA = -1.
            self.currentC = -1.
        else:
            self.currentA = currPayload['CurrentSideA']
            self.currentC = currPayload['CurrentSideC']


        if self.verbose:
            print 'LumiCalibrator.setLucidCurrent found Side A: %f Side C: %f' % (self.currentA, self.currentC)
            
    def dump(self):

        print 'Calibration: %s, muToLumi: %f', (self.fType, self.muToLumi)
        if self.fType == 'Logarithm':
            print 'par[0]: %f', self.parVec[0]

        elif self.fType == 'LookupTable_EventAND_Lin':
            print 'par[0]: %f', self.parVec[0]
            print 'par[1]: %f', self.parVec[1]
            print 'par[5]: %f', self.parVec[5]
            
    # Take raw luminosity (rate) value and return calibrated luminosity
    def calibrate(self, rawLumi):

        cal = -1.
        self.rflag = 0 # Success unless we fail

        if (rawLumi < 0.):
            print 'LumiCalibrator.calibrate(%f) - non-physical value!' % rawLumi
            self.rflag = 1
            return 0.

        elif (rawLumi == 0.):
            return 0.

        # Check for polynomial correction
        calibstr = self.fType
            
        if calibstr.find('Polynomial') == 0:

            cal = self.calibPolynomial(rawLumi)

        elif calibstr.find('Logarithm') == 0:

            cal = self.calibLogarithm(rawLumi)

        elif calibstr.find('HitLogarithm') == 0:

            cal = self.calibHitLogarithm(rawLumi)
            
        elif calibstr.find('LookupTable_EventAND_Lin') == 0:

            try:
                cal = self.calibLookupTable(rawLumi)
            except Exception as e:
                cal = 0.
                self.rflag = 1
                print 'LumiCalibrator.calibLookupTable(%f) - Error: %s' % (rawLumi, e)
                
        elif calibstr.find('LookupTable_EventAND_Log') == 0:

            try:
                cal = self.calibLookupTableLog(rawLumi)
            except Exception as e:
                cal = 0.
                self.rflag = 1
                print 'LumiCalibrator.calibLookupTableLog(%f) - Error: %s' % (rawLumi, e)

        elif calibstr.find('LookupTable_EventANDFull_Log') == 0:

            try:
                cal = self.calibLookupTableFullLog(rawLumi)
            except Exception as e:
                cal = 0.
                self.rflag = 1
                print 'LumiCalibrator.calibLookupTableFullLog(%f) - Error: %s' % (rawLumi, e)
                
        elif calibstr.find('LookupTablePoisson_Lin') == 0:

            try:
                cal = self.calibLookupTablePoisson(rawLumi)
            except Exception as e:
                cal = 0.
                self.rflag = 1
                print 'LumiCalibrator.calibLookupTablePoisson(%f) - Error: %s' % (rawLumi, e)
                
        elif calibstr.find('LookupTableZeroPoisson_Lin') == 0:

            try:
                cal = self.calibLookupTableZeroPoisson(rawLumi)
            except Exception as e:
                cal = 0.
                self.rflag = 1
                print 'LumiCalibrator.calibLookupTableZeroPoisson(%f) - Error: %s' % (rawLumi, e)

        else:    
            print 'LumiCalibrator.calibrate(%f) - Unknown calibration type %s!' % (rawLumi, self.fType)
            self.rflag = 1
            cal = 0.

        if self.rflag == 1:
            return cal

        # Try to do polynomial and current correction in proper order
        polyPos = calibstr.find("_PolyCorr")
        currPos = calibstr.find("_CurrCorr")

        # No extra corrections
        if polyPos == -1 and currPos == -1:
            pass

        elif polyPos == -1 and currPos > 0:
            cal = self.currentCorrection(cal)

        elif polyPos > 0 and currPos == -1:
            cal = self.polyCorrection(cal)

        elif polyPos < currPos:
            cal = self.polyCorrection(cal)
            cal = self.currentCorrection(cal)

        elif currPos < polyPos:
            cal = self.currentCorrection(cal)
            cal = self.polyCorrection(cal)

        else:
            print 'LumiCalibrator.calibrate() - I am so confused: %s' % calibstr
            
        return self.muToLumi * cal

    def currentCorrection(self, mu):

        correction = 1.

        current = -1.
        if self.currType == 1:
            current = self.currentA+self.currentC

        elif self.currType == 2:
            current = self.currentA

        elif self.currType == 3:
            current = self.currentC

        else:
            print 'LumiCalibrator.currentCorrection() - unknown current type: %d' % self.currType 
            self.rflag = 1
            return mu

        if self.currVersion == 0:
            return mu

        if current == -1.:
            if self.verbose:
                print 'LumiCalibrator.currentCorrection() - invalid LUCID currents found - A: %f C: %f' % (self.currentA, self.currentC)
                print 'LumiCalibrator.currentCorrection() - try using last values found - A: %f C: %f' % (self.oldCurrentA, self.oldCurrentC)
                
            self.rflag = 1 # Flag this as bad

            # Try using previous version
            if self.currType == 1:
                current = self.oldCurrentA+self.oldCurrentC

            elif self.currType == 2:
                current = self.oldCurrentA

            elif self.currType == 3:
                current = self.oldCurrentC
            
        if current == -1.:
            return mu
        
        elif self.currVersion == 1:
            correction = 100./(100.+self.currParA+self.currParB*current/self.nBunches)

        elif self.currVersion == 2:
            correction = 100./(100.+self.currParA+self.currParB*current)

        elif self.currVersion == 3:
            correction = 100./(100.+self.currParA+self.currParB*current+self.currParC*current**2)

        elif self.currVersion == 4: # Same as 3
            correction = 100./(100.+self.currParA+self.currParB*current+self.currParC*current**2)

        else:
            print 'LumiCalibrator.currentCorrection() - unknown calibration version %d' % self.currVersion

        if self.verbose and self.currVersion != 0:
            print 'LumiCalibrator.currentCorrection() - version %d -> currentA = %f, currentC = %f, correction = %f' % (self.currVersion, self.currentA, self.currentC, correction)

        return mu*correction

    def polyCorrection(self, mu):

        mucorr = mu

        # Parameters have already been parsed

        if self.polyVersion == 0:
            pass

        elif self.polyVersion == 1:
            mucorr = self.polyParA + self.polyParB * mu + self.polyParC * mu * mu

        else:
            print 'LumiCalibrator.polyCorrection() - unknown calibration version %d' % self.polyVersion

        return mucorr    

    def calibPolynomial(self, rawLumi):
        cal = 0.
        self.rflag = 1
        
        nrange = int(self.parVec[0])
        self.nParUsed = 8 * nrange + 1

        for i in range(nrange):
            rmin = self.parVec[i+1]
            rmax = self.parVec[i+2]
            if rawLumi < rmax and rawLumi >= rmin:
                a = [self.parVec[i+3]]
                a.append(self.parVec[i+4])
                a.append(self.parVec[i+5])
                a.append(self.parVec[i+6])
                a.append(self.parVec[i+7])
                a.append(self.parVec[i+8])
                for k in range(6):
                    cal += a[k]*pow(rawLumi, k)

                self.rflag = 0    
                break

        if self.rflag == 1:
            print 'LumiCalibrator.calibPolynomial(%f) - Value out of range' % rawLumi

        return cal

    def calibLogarithm(self, rawLumi):

        cal = 0.
        self.rflag = 0

        self.nParUsed = 1
        
        invEff = self.parVec[0]

        try:
            cal = -invEff*math.log(1.-rawLumi)
            
        except:
            cal = 0.
            self.rflag = 1
            # Don't print for simple saturation
            if rawLumi != 1.:
                print 'LumiCalibrator.calibLogarithm(%f) - Unphysical input!' % rawLumi
            
        return cal

    def calibHitLogarithm(self, rawLumi):

        cal = 0.
        
        invEff = self.parVec[0]
        channels = self.parVec[1]
        offset = self.parVec[2]
        maxRawLumiperBX = self.parVec[3]

        self.nParUsed = 4
        
        # rawLumi can be > 1 here, check value against maxRawLumiPerBX
        if rawLumi > maxRawLumiperBX:
            self.rflag = 1
            print 'LumiCalibrator.calibHitLogarithm(%f) - input greater than max range %f!' % (rawLumi, maxRawLumiperBX)
            return cal
        
        try:
            cal = -invEff*math.log(1.-rawLumi/channels)/(1-offset)
            
        except:
            cal = 0.
            self.rflag = 1
            print 'LumiCalibrator.calibHitLogarithm(%f) - Unphysical input!' % rawLumi
            
        return cal
    
    # LookupTable_EventAND_Lin
    def calibLookupTable(self, rawLumi):

        cal = 0.

        self.nParUsed = 6
        
        if rawLumi < 0.:
            self.rflag = 1
            print 'LumiCalibrator.calibLookupTable(%f) - Unphysical input to LUT!' % rawLumi

        elif rawLumi > 0.:
            sigo = self.parVec[0]
            siga = self.parVec[1]
            # Try fast algorithm first
            cal = self.parVec[5]*self.getMuvis(rawLumi, sigo, siga)
            if self.rflag == 1:
                # Try again with slower algorithm
                cal = self.parVec[5]*self.getMuvis2(rawLumi, sigo, siga)
                    
        return cal
    
    # LookupTable_EventAND_Log
    def calibLookupTableLog(self, rawLumi):

        cal = 0.

        self.nParUsed = 8
        
        if (rawLumi < 0.) or (rawLumi >= 1.):
            self.rflag = 1
            print 'LumiCalibrator.calibLookupTableLog(%f) - Unphysical input to LUT!' % rawLumi

        elif rawLumi > 0.:
            sigo = self.parVec[0]
            siga = self.parVec[1]
            # Try fast algorithm first
            cal = self.parVec[7]*self.getMuvis(rawLumi, sigo, siga)
            if self.rflag == 1:
                # Try again with slower algorithm
                cal = self.parVec[7]*self.getMuvis2(rawLumi, sigo, siga)
                    
        return cal
    
    # From Vincent
    def getMuvis(self, rawPerBX, sigo, siga):

        # Guess?
        munew = 0.01
        
        b = sigo/siga
        a = (b + 1)/2.

        # Set error, and clear below if we converge
        self.iflag = 1
        
        # Set a fixed number of cycles, but break if we converge faster
        for i in range(30):
            mu = munew
            y = 1 - 2 * math.exp(-a * mu) + math.exp(-b * mu) 
            dy = 2 * a * math.exp(-a * mu) - b * math.exp(-b * mu) 

            munew = mu - (y-rawPerBX)/dy

            #print "Iteration: %d, Munew: %f, Muold: %f" % (i, munew, mu)
            if munew <= 0.:
                return 0.

            if math.fabs(munew-mu)/munew < 1.e-5:
                self.iflag = 0
                return munew


        print 'LumiCalibrator.calibLookupTable(', rawPerBX, ') - did not converge (Vincent method)!'
        return munew
    
    # From Mika
    def getMuvis2(self, rawPerBX, sigo, siga):
        muvl=1e-10
        muvu=100
        muvm=10
        sr=sigo/siga
        rbxl=self.rpbx(sr,muvl)
        rbxu=self.rpbx(sr,muvu)
        rbxm=self.rpbx(sr,muvm)

        # Set error and clear below if we converge
        self.rflag = 1

        # Check that starting value is in the valid range
        if rawPerBX < rbxl or rawPerBX > rbxu:
            print 'LumiCalibrator.calibLookupTable(', rawPerBX, ') - raw lumi value outside of LUT range', rbxl, 'to', rbxu, '!'
            return 0.

        # Restrict to a fixed number of iterations
        for i in range(50):
            if rbxl<rawPerBX and rbxm>rawPerBX:
                rbxu=rbxm
                muvu=muvm
                muvm=0.5*(muvu+muvl)
            else:
                rbxl=rbxm
                muvl=muvm
                muvm=0.5*(muvu+muvl)
                
            rbxm = self.rpbx(sr, muvm)
            
            if (muvu-muvl)/muvl < 1e-5:
                self.rflag = 0
                return muvm

        # Did not converge
        print 'LumiCalibrator.calibLookupTable(', rawPerBX, ') - did not converge (Mika method)!'
        return muvm
    
    def rpbx(self, sr, muvis):
        return 1 - 2*math.exp(-(1+sr)*muvis/2) + math.exp(-sr*muvis)

    # LookupTable_EventANDFull_Log
    def calibLookupTableFullLog(self, rawLumi):

        cal = 0.

        self.nParUsed = 9
        
        if (rawLumi < 0.) or (rawLumi >= 1.):
            self.rflag = 1
            print 'LumiCalibrator.calibLookupTableFullLog(%f) - Unphysical input to LUT!' % rawLumi

        elif rawLumi > 0.:
            sigA = self.parVec[0]
            sigC = self.parVec[1]
            sigAND = self.parVec[2]

            # Try fast algorithm first
            cal = self.parVec[8]*self.getMuvisFull(rawLumi, sigA, sigC, sigAND)
            if self.rflag == 1:
                # Try again with slower algorithm
                cal = self.parVec[8]*self.getMuvisFull2(rawLumi, sigA, sigC, sigAND)
                    
        return cal
    
    # Full version
    def getMuvisFull(self, rawPerBX, sigA, sigC, sigAND):

        # Guess?
        munew = 0.01

        ra = sigA/sigAND
        rc = sigC/sigAND
        b = ra+rc-1 # sigOR/sigAND

        # Set error, and clear below if we converge
        self.iflag = 1
        
        # Set a fixed number of cycles, but break if we converge faster
        for i in range(30):
            mu = munew
            y = 1 - math.exp(-ra * mu) - math.exp(-rc * mu) + math.exp(-b * mu) 
            dy = ra * math.exp(-ra * mu) + rc * math.exp(-rc * mu) - b * math.exp(-b * mu) 

            munew = mu - (y-rawPerBX)/dy

            #print "Iteration: %d, Munew: %f, Muold: %f" % (i, munew, mu)
            if munew <= 0.:
                return 0.

            if math.fabs(munew-mu)/munew < 1.e-5:
                self.iflag = 0
                return munew


        print 'LumiCalibrator.calibLookupTable(', rawPerBX, ') - did not converge (Vincent method)!'
        return munew
    
    # From Mika
    def getMuvisFull2(self, rawPerBX, sigA, sigC, sigAND):
        muvl=1e-10
        muvu=100
        muvm=10

        ra = sigA/sigAND
        rc = sigC/sigAND
        sr=ra+rc-1 # = sigOR/sigAND
        rbxl=self.rpbxFull(ra, rc, sr,muvl)
        rbxu=self.rpbxFull(ra, rc, sr,muvu)
        rbxm=self.rpbxFull(ra, rc, sr,muvm)

        # Set error and clear below if we converge
        self.rflag = 1

        # Check that starting value is in the valid range
        if rawPerBX < rbxl or rawPerBX > rbxu:
            print 'LumiCalibrator.calibLookupTable(', rawPerBX, ') - raw lumi value outside of LUT range', rbxl, 'to', rbxu, '!'
            return 0.

        # Restrict to a fixed number of iterations
        for i in range(50):
            if rbxl<rawPerBX and rbxm>rawPerBX:
                rbxu=rbxm
                muvu=muvm
                muvm=0.5*(muvu+muvl)
            else:
                rbxl=rbxm
                muvl=muvm
                muvm=0.5*(muvu+muvl)
                
            rbxm = self.rpbxFull(ra, rc, sr, muvm)
            
            if (muvu-muvl)/muvl < 1e-5:
                self.rflag = 0
                return muvm

        # Did not converge
        print 'LumiCalibrator.calibLookupTable(', rawPerBX, ') - did not converge (Mika method)!'
        return muvm
    
    def rpbxFull(self, ra, rc, sr, muvis):
        return 1 - math.exp(-ra*muvis) - mathexp(-rc*muvis) + math.exp(-sr*muvis)

    # LookupTablePoisson_Lin':
    def calibLookupTablePoisson(self, rawLumi):

        offset = self.parVec[0]
        maxRawLumiperBX = self.parVec[1]
        mu_max = self.parVec[2]
        points = self.parVec[3]
        nRefs = int(self.parVec[4])
        ref = self.parVec[5:]

        self.nParUsed = 5+nRefs
        
        cal = 0.
        self.rflag = 1

        if rawLumi > maxRawLumiperBX:
            print 'LumiCalibrator.calibLookupTablePoisson(%f) - input greater than max range %f!' % (rawLumi, maxRawLumiperBX)
            return cal

        # Iteratively solve
        munew = 0.001

        # Set a fixed number of cycles, but break faster if we converge
        for i in range(30):
            mu = munew
            # No constant term in hit counting
            y = 0
            dy = 0
            # Now add additional terms in sum, divide out factor of e^(-mu)
            # Use running product to calculate mu^n/n!
            fact = 1
            for j in range(nRefs):
                n = j+1
                #term = ref[j] * math.pow(mu, n) / self.factorial(n) 
                #term = ref[j] * math.pow(mu, n) / math.factorial(n)
                fact *= (mu/n)
                term = ref[j] * fact 
                y += term
                dy += term*(n/mu-1)

            # Here we are computing the probability of a hit, so we compare to the hit rate
            munew = mu - (y-rawLumi/math.exp(-mu))/dy  # Remember to put back in e^(-mu)
            cal = munew

            if munew <= 0.:
                cal = 0.
                print 'LumiCalibrator.calibLookupTablePoisson(%f) - failed to converge (went negative)!'% (rawLumi)
                return cal

            if math.fabs(munew-mu)/munew < 1.e-5:
                self.rflag = 0
                return cal/(1-offset)
                

        print 'LumiCalibrator.calibLookupTablePoisson(%f) - failed to converge (trials)!'% (rawLumi)
        return cal/(1-offset)

    # LookupTableZeroPoisson_Lin':
    def calibLookupTableZeroPoisson(self, rawLumi):

        offset = self.parVec[0]
        maxRawLumiperBX = self.parVec[1]
        mu_max = self.parVec[2]
        points = self.parVec[3]
        nRefs = int(self.parVec[4])
        ref = self.parVec[5:]

        self.nParUsed = 5+nRefs
        
        cal = 0.
        self.rflag = 1

        if rawLumi > maxRawLumiperBX:
            print 'LumiCalibrator.calibLookupTableZeroPoisson(%f) - input greater than max range %f!' % (rawLumi, maxRawLumiperBX)
            return cal

        # Iteratively solve
        munew = 0.001

        # Set a fixed number of cycles, but break faster if we converge
        for i in range(30):
            mu = munew
            # Constant term first (n=0) - divide out constant factor of e^(-mu)
            y = 1
            dy = -1
            fact = 1
            # Now add additional terms in sum
            for j in range(nRefs):
                n = j+1
                fact *= (mu/n)
                # term = ref[j] * math.pow(mu, n) / self.factorial(n)
                term = ref[j] * fact
                y += term
                dy += term*(n/mu-1)

            # Here we are computing the probability of a *zero*, so compare to observed *zero* rate: (1-rawLumi)
            zRate = (1-rawLumi)
            munew = mu - (y-zRate/math.exp(-mu))/dy  # Remember to put back in e^(-mu)
            cal = munew
            
            if munew <= 0.:
                print 'LumiCalibrator.calibLookupTableZeroPoisson(%f) - failed to converge (negative)!'% (rawLumi)
                cal = 0.
                return cal

            if math.fabs(munew-mu)/munew < 1.e-5:
                self.rflag = 0
                return cal/(1-offset)

        print 'LumiCalibrator.calibLookupTableZeroPoisson(%f) - failed to converge (trials)!'% (rawLumi)
        return cal/(1-offset)
