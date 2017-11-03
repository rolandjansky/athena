#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
# @file DCubeClient/python/DCubeTester.py
# @author Krzysztof Daniel Ciba (Krzyszotf.Ciba@NOSPAMgmail.com)
# @brief implementation of DCubeTester and test_DCubeTester classes

import sys
from DCubeUtils import DCubeObject, DCubeException
from DCubePlotter import DCubePlotter
import ROOT
import unittest

##
# @class DCubeTester
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief perform statictics tests and plotting  
class DCubeTester( DCubeObject ):

    ## XML DOM Document instance
    xmldoc = None

    ## XML DOM Element instance 
    node = None

    ## handle for monitored root object
    mon = None

    ## handle for reference root object
    ref = None

    ## errors counters
    errors = { }

    ## intarnal histogram counter
    __nbObjs = 0
    
    ## statistics summary table
    sumTable = { "KS"   : { "OK"   : 0,
                            "WARN" : 0,
                            "FAIL" : 0 },
                 "chi2" : { "OK"   : 0,
                            "WARN" : 0,
                            "FAIL" : 0 },
                 "bbb"  : { "OK"   : 0,
                            "WARN" : 0,
                            "FAIL" : 0 },
                 "meanY": { "OK"   : 0,
                            "WARN" : 0,
                            "FAIL" : 0 }}


    ## chi2 error communicates
    chi2igood = { 0 : "no problems",
                  1 : "there is bin in mon hist with low then 1 exp number of event",
                  2 : "there is bin in ref hist with low then 1 exp number of event",
                  3 : "there are bins in both histograms with less than 1 event" }

    
    ## summary status
    __status = ""
    
    ## c'tor
    # @param self "Me, myself and Irene"
    # @param xmldoc XML DOM Document instance
    def __init__( self, xmldoc, parsed ):
        
        super( DCubeTester, self ).__init__( self )
        self.xmldoc = xmldoc
        self.opts, self.args = parsed
        self.tests = { "KS"   : self.__testKS,
                       "bbb"  : self.__testBBB,
                       "chi2" : self.__testChi2,
                       "meanY": self.__testMeanY }
       

        if ( self.opts.makeplots ):
            self.info("will produce plot files")
            self.__plotter = DCubePlotter( xmldoc, parsed )
        else:
            self.warn("making of plots disabled")
            
    ## 
    # @param self "Me, myself and Irene"
    # @param node DOM XML  node 
    # @param mon monitored ROOT object
    # @param ref reference ROOT object
    # @return modified XML node 
    def test( self, node, mon=None, ref=None ):

        self.__nbObjs += 1

        self.node = self.mon = self.ref = None

        self.node = node
        self.parent = self.node.parentNode

        self.mon = mon
        self.ref = ref

        # mon exists?
        if ( not self.mon ):
            status = "FAIL;monitored not found"
            self.node.setAttribute( "status", status )
            if status not in self.errors.keys():
                self.errors[ status ] = 1
            else:
                self.errors[ status ] = self.errors[ status ] + 1
            self.error("monitored object not found!")
            return "FAIL"

        cl = self.mon.Class().GetName() 
        isHist = False
        if ( cl[:2] in ( "TH", "TP") ): isHist = True
       
        # dimension OK?
        if ( isHist and self.mon.GetDimension() > 2 ):
            status = "FAIL;unsupported object, dimension bigger than 2"
            self.node.setAttribute( "status", status )
            if ( status not in self.errors.keys() ):
                self.errors[ status ] = 1
            else:
                self.errors[ status ] = self.errors[ status ] + 1
            self.error( "unsuported object found" )
            return "FAIL"

        # reference exists?
        if ( not self.ref ):
            self.warn( "reference object not found!" )
            status = "WARN;reference histogram not found"
            self.node.setAttribute( "status", status )
            if ( status not in self.errors.keys() ):
                self.errors[ status ] = 1
            else:
                self.errors[ status ] = self.errors[ status ] + 1
    
        if ( self.mon and self.ref ):
            # same class?
            monClassName = self.mon.Class().GetName()
            refClassName = self.ref.Class().GetName()
            if ( monClassName != refClassName ):
                status = "FAIL;different root types"
                self.node.setAttribute( "status", status + " mon=%s ref=%s" % ( monClassName,
                                                                                refClassName ) )
                if ( status not in self.errors.keys() ):
                    self.errors[ status ] = 1
                else:
                    self.errors[ status ] = self.errors[ status ] + 1
                self.error("different types for mon and ref objects!")
                return "FAIL"


        if ( isHist ):
            self.monBins = [ self.mon.GetNbinsX(), self.mon.GetNbinsY() ]  
        if ( isHist and self.ref ):
            self.refBins = [ self.ref.GetNbinsX(), self.ref.GetNbinsY() ]
      
        if ( isHist and self.mon and self.ref ):
            # same binnig?
            monBins = [ "%s=%d" % ( x, y) for ( x, y) in zip(["x", "y"], self.monBins ) ]
            refBins = [ "%s=%d" % ( x, y) for ( x, y) in zip(["x", "y"], self.refBins ) ]                         
            diffBins = [ "mon %s ref %s" % (x, y) for (x,y) in zip(monBins,refBins) if x != y ]
            if ( diffBins  ):
                status = "FAIL;different binnig"
                self.node.setAttribute( "status", status + " %s" % " ".join(diffBins) )
                if ( status not in self.errors.keys() ):
                    self.errors[ status ] = 1
                else:
                    self.errors[ status ] = self.errors[ status ] + 1
                self.error("different binning for mon and ref objects!")
                return "FAIL" 

        status = "OK"                    
        if ( isHist ):
            status = self.__grabHistogramStat()
        else:
            status = self.__grabGraphStat() 

        plotStatus = "OK"
        if ( self.opts.makeplots ):
            plotStatus = self.__makePlots() 
        else:
            plotStatus = "WARN;plots not reqested"
        self.node.setAttribute( "plots", plotStatus)

        return status
        

    ## grab statistic info for TGraphXXX  
    # @param self "Me, myself and Irene"
    def __grabGraphStat( self ):
        statNode = self.xmldoc.createElement( "stat" )
        self.node.appendChild( statNode )

        nbPointsNode = self.xmldoc.createElement( "points" )
        nbPointsCData = self.xmldoc.createTextNode( "%d" % self.mon.GetN() )
        if ( self.ref ):
            nbPointsNode.setAttribute( "ref" , "%d" % self.ref.GetN() )
        nbPointsNode.appendChild( nbPointsCData )

        meanNode = self.xmldoc.createElement( "mean" )
        meanCData = self.xmldoc.createTextNode( "%4.3f" % self.mon.GetMean() )
        if ( self.ref ):
            meanNode.setAttribute( "ref" , "%4.3f" % self.ref.GetMean() )
        meanNode.appendChild( meanCData )

        rmsNode = self.xmldoc.createElement( "rms" )
        rmsCData = self.xmldoc.createTextNode( "%d" % self.mon.GetRMS() )
        if ( self.ref ):
            rmsNode.setAttribute( "ref" , "%d" % self.ref.GetRMS() )
        rmsNode.appendChild( rmsCData )

        statNode.appendChild( nbPointsNode )
        statNode.appendChild( meanNode )
        statNode.appendChild( rmsNode )

        return "OK"
        
        
        
    ## grab basic statistics - nb of entries, mean and RMS
    # @param self "Me, myself and Irene"
    def __grabHistogramStat( self ):

        statNode = self.xmldoc.createElement( "stat" )

        self.node.appendChild( statNode )

        entriesNode = self.xmldoc.createElement("entries")
        entriesCDataNode = self.xmldoc.createTextNode( str( self.mon.GetEntries() ) )
        if ( self.ref ):
            entriesNode.setAttribute( "ref", str( self.ref.GetEntries() ) )
        entriesNode.appendChild( entriesCDataNode )

        statNode.appendChild( entriesNode )

        dim = self.mon.GetDimension() 

        ### To dump MeanY values
        if ( "TProfile" in self.mon.Class().GetName() ): dim = 2

        # store under and overflows 
        monU = []
        monO = []
        refU = []
        refO = []

        if ( dim == 1 ):
            monU.append( self.mon.GetBinContent(0) )
            monO.append( self.mon.GetBinContent( self.mon.GetNbinsX()+1 ) )
            if ( self.ref ):
                refU.append( self.ref.GetBinContent(0) )
                refO.append( self.ref.GetBinContent( self.ref.GetNbinsX()+1 ) )
        elif ( dim == 2 ):
            monU.append( self.mon.GetBinContent( 0, 0 ) )
            monO.append( self.mon.GetBinContent( self.mon.GetNbinsX()+1, 0 ) )
            monU.append( self.mon.GetBinContent( 0,  self.mon.GetNbinsY()+1) )
            monO.append( self.mon.GetBinContent( self.mon.GetNbinsX()+1, self.mon.GetNbinsY()+1 ) )  
            if ( self.ref ):
                refU.append( self.ref.GetBinContent( 0, 0 ) )
                refO.append( self.ref.GetBinContent( self.ref.GetNbinsX()+1, 0 ) )
                refU.append( self.ref.GetBinContent( 0,  self.ref.GetNbinsY()+1) )
                refO.append( self.ref.GetBinContent( self.ref.GetNbinsX()+1, self.ref.GetNbinsY()+1 ) )  

        underflowNode = self.xmldoc.createElement( "underflow" )
        underflowCData = self.xmldoc.createTextNode( "%d" % sum( monU ) )
        if ( self.ref ):
            underflowNode.setAttribute( "ref", "%d" % sum( refU) )
        underflowNode.appendChild( underflowCData )

        overflowNode = self.xmldoc.createElement( "overflow" )
        overflowCData = self.xmldoc.createTextNode( "%d" % sum( monO ) )
        if ( self.ref ):
            overflowNode.setAttribute( "ref", "%d" % sum( refO ) )
        overflowNode.appendChild( overflowCData )

        statNode.appendChild( underflowNode )
        statNode.appendChild( overflowNode )
        
        dimName = [ "x", "y", "z" ]
        for i in range( 1, dim+1 ):
            self.debug( "gathering statistics along %s axis" % dimName[i-1] )

            dimNode = self.xmldoc.createElement( "dim" )
            dimNode.setAttribute( "name", dimName[i-1] )
            dimNode.setAttribute( "bins", "%d" % self.monBins[i-1])
        
            underflowNode = self.xmldoc.createElement( "underflow" )
            underflowCData  = self.xmldoc.createTextNode( "%d" % monU[i-1] )
            if ( self.ref ):
                underflowNode.setAttribute( "ref", "%d" % refU[i-1] )
            underflowNode.appendChild( underflowCData )

            dimNode.appendChild( underflowNode )
        
            overflowNode = self.xmldoc.createElement( "overflow" )
            overflowCData  = self.xmldoc.createTextNode( "%d" % monO[i-1] )
            if ( self.ref ):
                overflowNode.setAttribute( "ref", "%d" % refO[i-1] )
            overflowNode.appendChild( overflowCData )

            dimNode.appendChild( overflowNode )
            
            meanNode = self.xmldoc.createElement( "mean" )
            if ( self.ref ):
                meanNode.setAttribute( "ref", "%3.2e" % self.ref.GetMean(i) )

            meanCData = self.xmldoc.createTextNode( "%3.2e" % self.mon.GetMean( i ) )
            meanNode.appendChild( meanCData )
            
            meanErrorNode = self.xmldoc.createElement( "mean_unc" )
            if ( self.ref ):
                meanErrorNode.setAttribute( "ref", "%3.2e" % self.ref.GetMeanError(i) )

            meanErrorCData = self.xmldoc.createTextNode( "%3.2e" % self.mon.GetMeanError( i ) )
            meanErrorNode.appendChild( meanErrorCData )
            
            rmsNode = self.xmldoc.createElement( "rms" )
            if ( self.ref ):
                rmsNode.setAttribute( "ref", "%3.2e" % self.ref.GetRMS(i) )
            rmsCData = self.xmldoc.createTextNode( "%3.2e" % self.mon.GetRMS(i) )
            rmsNode.appendChild( rmsCData )
        
            rmsErrorNode = self.xmldoc.createElement( "rms_unc" )
            if ( self.ref ):
                rmsErrorNode.setAttribute( "ref", "%3.2e" % self.ref.GetRMSError(i) )
            rmsErrorCData = self.xmldoc.createTextNode( "%3.2e" % self.mon.GetRMSError(i))
            rmsErrorNode.appendChild( rmsErrorCData )

            dimNode.appendChild( meanNode )
            dimNode.appendChild( meanErrorNode )
            dimNode.appendChild( rmsNode )
            dimNode.appendChild( rmsErrorNode )

            statNode.appendChild( dimNode )



        status = [ ]

        tests = [t for t in self.node.getAttribute('tests').strip().split(',') if t]
        
        if ( "all" in tests ): tests = [ "KS", "chi2", "bbb", "meany" ]

        if ( None not in ( self.mon, self.ref ) ):
             
            if ( "TProfile" in self.mon.Class().GetName() ):
                if ( "meany" in tests ):
                    status.append( self.__testMeanY.__call__( statNode ) )
            else:
                for test in tests:
                    if ( test != "meany" ):
                        status.append( self.tests[ test ].__call__( statNode ) )
                    
        statusAttr = "OK"
        if ( "FAIL" in status ): statusAttr = "FAIL"
        elif ( "WARN" in status ): statusAttr = "WARN"
        
        self.node.setAttribute( "stest", statusAttr )

        return statusAttr
                    
    ## perform @f$\chi^2@f$ test
    # @param self "Me, myself and Irene"
    # @param statNode <stat> element
    def __testChi2( self, statNode ):

        chi2 = ROOT.Double( 0.0 )
        igood = ROOT.Long( 0 )
        ndf = ROOT.Long( 0 )


        nbBins,nbBinsSame,nbBinsDiff = self.__testEqual()
        self.debug("*** Pearson's chi2 *** all none-empty bins=%d all equal non-empty bins=%d" % ( nbBins, nbBinsSame ) )

        pval = 1.0
        if nbBins==0.0 and nbBinsSame==0.0:
            self.warn( "*** Pearson's chi2 *** both histograms are empty!" )
        elif nbBinsSame==nbBins:
            self.debug( "*** Pearson's chi2 *** both histograms are equal!" )
        else:
            pval = self.mon.Chi2TestX( self.ref, chi2, ndf, igood, "UUDNORM")

            self.debug( "*** Pearson's chi2 *** (UUDNORM) p-value= %4.3f" % pval )
            ig = "*** Pearson's chi2 *** igood = %d, %s" % ( igood, self.chi2igood[igood] )
            if ( igood == 0 ):
                self.debug( ig )
            else:
                self.warn( ig )

        status = self.__getTestStatus( pval ) 
        self.sumTable["chi2"][status] = self.sumTable["chi2"][status] + 1   

        if ( ndf != 0 ):
            self.info( "*** Pearson's chi2 *** chi2 (/ndf) = %4.3f (/%d = %4.3f) status=%s" % ( chi2,
                                                                                                ndf,
                                                                                                chi2/ndf,
                                                                                                status ) )
        else:
            self.info( "*** Pearson's chi2 *** chi2 = %4.3f ndf = %d status=%s" % ( chi2,
                                                                                    ndf,
                                                                                    status ) )

        pValueNode = self.xmldoc.createElement( "pvalue" )
        pValueCData = self.xmldoc.createTextNode( "%4.3f" % pval )

        pValueNode.setAttribute( "test", "chi2" )
        pValueNode.setAttribute( "status", status )
       
        counter = self.parent.getAttribute( "chi2" + status )
        if ( not counter ): counter = "0"
        self.parent.setAttribute( "chi2" + status, "%d" % ( int(counter) + 1 )  )

        pValueNode.appendChild( pValueCData )

        statNode.appendChild( pValueNode )


        return status

    ## perform Kolmogorov-Smirnoff test
    # @param self "Me, myself and Irene"
    # @param statNode <stat> element
    def __testKS( self, statNode ):

        nbBins,nbBinsSame,nbBinsDiff = self.__testEqual()
        self.debug("*** Kolmogorov-Smirnoff *** all none-empty bins=%d all equal non-empty bins=%d" % ( nbBins, nbBinsSame ) )

        pval = 1.0
        if nbBins==0.0 and nbBinsSame==0.0:
            self.warn( "*** Kolmogorov-Smirnoff *** both histograms are empty!" )
        elif nbBinsSame==nbBins:
            self.debug( "*** Kolmogorov-Smirnoff *** both histograms are equal!" )
        else:
            pval = self.mon.KolmogorovTest( self.ref, "D" )
        
        status = self.__getTestStatus( pval ) 
        self.sumTable["KS"][status] = self.sumTable["KS"][status] + 1     

        self.info( "*** Kolmogorov-Smirnoff *** (D) p-value=%4.3f status=%s" % ( pval, status ) )

        pValueNode = self.xmldoc.createElement( "pvalue" )
        pValueCData = self.xmldoc.createTextNode( "%4.3f" % pval )

        pValueNode.setAttribute( "test", "KS" )
        pValueNode.setAttribute( "status", status )

        counter = self.parent.getAttribute( "KS" + status )
        if ( not counter ): counter = "0"
        self.parent.setAttribute( "KS" + status, "%d" % ( int(counter) + 1 )  )

        pValueNode.appendChild( pValueCData )

        statNode.appendChild( pValueNode )
        return status
        
    ##  ** provide check if equal **
    # returns (nbBins,nbBinsSame,nbBinsDiff)
    #   nbBins - nb of bins not zero
    #   nbBinsSame - nb of bins not zero but equal
    #   nbBinsDiff - nb of bins not zero and different
    def __testEqual( self):
        nbBins = 0.0
        nbBinsSame = 0.0

        binX = self.mon.GetNbinsX()
        binY = self.mon.GetNbinsY()

        for i in range( binX+1 ):
            for j in range( binY + 1 ):
                mon = self.mon.GetBinContent( i, j )
                ref = self.ref.GetBinContent( i, j )
                if ( mon or ref ):
                    nbBins += 1
                    if ( mon == ref ):
                        nbBinsSame += 1
        return (nbBins,nbBinsSame,nbBins-nbBinsSame)

    ## perform "bin-by-bin" statistic test
    # @param self "Me, myself and Irene"
    # @param statNode <stat> element
    def __testBBB( self, statNode ):

        nbBins,nbBinsSame,nbBinsDiff = self.__testEqual()
        self.debug("*** bin-by-bin *** all none-empty bins=%d all equal non-empty bins=%d" % ( nbBins, nbBinsSame ) )

        pval = 0.0
        if ( nbBins != 0.0 ):
            pval = nbBinsSame / nbBins  
            self.debug("*** bin-by-bin *** p-value=%4.3f" % pval )
        elif ( nbBinsSame == 0.0 ):
            pval = 1.0
            self.warn( "*** bin-by-bin *** both histograms are empty!" )
            self.debug( "*** bin-by-bin *** p-value=%4.3f" % pval )
        else:
            # AS: this part is never called?!
            self.warn( "*** bin-by-bin *** reference histogram is empty, while monitored has some entries" )
            self.debug( "*** bin-by-bin *** test failed" )
        
        status = self.__getTestStatus( pval )
        self.info("*** bin-by-bin *** p-value=%4.3f status=%s" % ( pval, status ) )

        self.sumTable["bbb"][status] = self.sumTable["bbb"][status] + 1     

        pValueNode = self.xmldoc.createElement( "pvalue" )
        pValueCData = self.xmldoc.createTextNode( "%4.3f" % pval )

        pValueNode.setAttribute( "test", "bbb" )
        pValueNode.setAttribute( "status", status )

        counter = self.parent.getAttribute( "bbb" + status )
        if ( not counter ): counter = "0"
        self.parent.setAttribute( "bbb" + status, "%d" % ( int(counter) + 1 )  )

        pValueNode.appendChild( pValueCData )

        statNode.appendChild( pValueNode )

        return status


    ## perform "diff(MeanY)" test for TProfile histos
    # @param self "Me, myself and Irene"
    # @param statNode <stat> element
    def __testMeanY( self, statNode ):

        avgEffmon = self.mon.GetMean( 2 ) * 100
        avgEffref = self.ref.GetMean( 2 ) * 100

        self.debug("*** MeanY Test *** refMean=%4.3f and monMean=%4.3f" % ( avgEffref, avgEffmon ) )
        
        pval = abs(avgEffmon - avgEffref)

        status = self.__getMeanTestStatus( pval )
        self.info("*** Mean Test *** p-value=%4.3f status=%s" % ( pval, status ) )

        self.sumTable["meanY"][status] = self.sumTable["meanY"][status] + 1     

        pValueNode = self.xmldoc.createElement( "pvalue" )
        pValueCData = self.xmldoc.createTextNode( "%4.3f" % pval )

        pValueNode.setAttribute( "test", "meanY" )
        pValueNode.setAttribute( "status", status )

        counter = self.parent.getAttribute( "meanY" + status )
        if ( not counter ): counter = "0"
        self.parent.setAttribute( "meanY" + status, "%d" % ( int(counter) + 1 )  )

        pValueNode.appendChild( pValueCData )

        statNode.appendChild( pValueNode )

        return status
    
    
    ## get test status for given pvalue 
    # @param self "Me, myself and Irene"
    # @param pval p-value from test
    def __getTestStatus( self, pval ):
        if ( ( pval < 0.0 or pval > 1.0 ) or  
             ( pval <= self.opts.pfail ) ): return "FAIL"
        if ( pval > self.opts.pwarn ): return "OK"
        return "WARN"


    ## get test status for given pvalue 
    # @param self "Me, myself and Irene"
    # @param pval p-value from test
    def __getMeanTestStatus( self, pval ):
        if ( pval > 1.0 and pval <= 5.0 ): return "WARN"
        if ( pval <= 1.0  ): return "OK"
        return "FAIL"


    ## make plots
    # @param self "Me, myself and Irene"
    def __makePlots( self ):
        if ( self.__plotter ):
            plotOpts = self.node.getAttribute( "plotopts" )
            status = self.__plotter.plot( self.node, self.mon, self.ref, plotOpts  )
            return status
    

    ## string representation of DCubeTester
    # @param self "Me, myself and Irene" 
    def __str__( self ):
        out  = "*"*61 + "\n"
        out += "* RUN SUMMARY\n"
        out += "*"*61 + "\n"
        out += "* objects processed = %d\n" % self.__nbObjs
        out += "* STATISTICS TESTS TABLE\n"
        out += "* " + "-"*45 + "\n"
        out += "* | %-8s | %8s | %8s | %8s |\n" % ( "test", "OK", "WARN", "FAIL") 
        self.allGOOD = 0
        self.allWARN = 0
        self.allFAIL = 0
        for key, value  in self.sumTable.iteritems():
            nbGOOD = value["OK"]
            nbWARN = value["WARN"]
            nbFAIL = value["FAIL"]

            self.allGOOD += nbGOOD
            self.allWARN += nbWARN
            self.allFAIL += nbFAIL
            out += "* " + "-"*45+"\n"
            out += "* | %-8s | %8d | %8d | %8d |\n" % ( key, nbGOOD, nbWARN, nbFAIL )
            
        out += "* " + "-"*45+"\n"

        
        out += "* | %-8s | %8d | %8d | %8d |\n" % ( "all", self.allGOOD, self.allWARN, self.allFAIL )
        out += "* " + "-"*45+"\n"

        self.fracGOOD = 0.0
        self.fracWARN = 0.0
        self.fracFAIL = 0.0
        all = float( self.allGOOD + self.allWARN + self.allFAIL ) 
        if ( all ):
            self.fracGOOD = 100 * float( self.allGOOD ) / all 
            self.fracWARN = 100 * float( self.allWARN ) / all 
            self.fracFAIL = 100 * float( self.allFAIL ) / all
        out += "* | %%        |    %04.2f |    %04.2f |    %04.2f |\n" % ( self.fracGOOD, self.fracWARN, self.fracFAIL )
        out += "* " + "-"*45+"\n"

        out += "*"*61+"\n"
        self.nbErrors = sum( self.errors.itervalues() )
        out += "* WARNINGS = %3d\n" % self.nbErrors
        i = 1
        for key, value in self.errors.iteritems( ):
            sev, what = key.split(";")
            
            out += "* [%02d] %4s %-30s - occured %d " %  (i, sev, what, value )
            if ( value == 1 ): out += "time\n"
            else: out += "times\n"
            i += 1

        out += "*"*61+"\n"
        
        self.__status = ""
        
        if ( self.allFAIL != 0 ):
            self.__status = "FAIL"
        elif ( self.allWARN != 0 or self.nbErrors != 0 ):
            self.__status = "WARN"
        else:
            self.__status = "OK"

        out += "* OVERALL STATISTICS STATUS: %-4s\n" % self.__status
                    
        out += "*"*61
        
        return out

    ## put summary to the logger and create summary node 
    # @param self "Me, myself and Irene"
    # @return summary XML Element
    def summary( self ):
        
        for line in str(self).split("\n"): 
            self.info( line )

        summaryNode = self.xmldoc.createElement( "summary" )
        summaryNode.setAttribute( "status" , self.__status )
        summaryNode.setAttribute( "objs", "%d" % self.__nbObjs )
        summaryNode.setAttribute( "errors", "%d" % self.nbErrors  )

        testTable = self.xmldoc.createElement( "table" )
        testTable.appendChild( self.__sumTableRow( [ "test", "OK", "WARN", "FAIL" ] ) )
        for test, results in self.sumTable.iteritems(): 
            testTable.appendChild( self.__sumTableRow( [ test,
                                                         results["OK"],
                                                         results["WARN"],
                                                         results["FAIL"] ] ) )
            
        testTable.appendChild( self.__sumTableRow( ["sum", self.allGOOD, self.allWARN, self.allFAIL ] ) )
        testTable.appendChild( self.__sumTableRow( ["fraction",
                                                    "%4.2f" % self.fracGOOD,
                                                    "%4.2f" % self.fracWARN,
                                                    "%4.2f" % self.fracFAIL ] ) )
        
        summaryNode.appendChild( testTable )

        errorsNode = self.xmldoc.createElement( "errors" )
        errorsNode.setAttribute( "nb", "%d" % self.nbErrors )

        for error,times in self.errors.iteritems():
            error = error.replace(";", " ")
            errorNode = self.xmldoc.createElement( "error" )
            errorNode.setAttribute( "times", "%d" % times )
            errorNode.setAttribute( "what", error )
            errorsNode.appendChild( errorNode )
            
        summaryNode.appendChild( errorsNode )
        
        return summaryNode 

    ## produce summary table row 
    # @param self "Me, myself and Irene"
    def __sumTableRow( self, what ):
        row = self.xmldoc.createElement( "tr" )
        for item in what:
            cellNode = self.xmldoc.createElement("td")
            cellCData = self.xmldoc.createTextNode( str(item) )
            cellNode.appendChild( cellCData )
            row.appendChild( cellNode )
        return row

    ## return overall summary as string
    # @param self "Me, myself and Irene"
    def status( self ):
        return self.__status
        
        
##
# @class test_DCubeTester
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com) 
# @brief test case for DCubeTester class
class test_DCubeTester( unittest.TestCase ):

    ## setup test cases
    # @param self "Me, myself and Irene"
    def setUp( self ):
        pass

    ## ctor
    # @param self "Me, myself and Irene"
    def test_01_ctor( self ):
        pass


## test case execution
if __name__ == "__main__":
    pass
    

