# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import ROOT
import pyAMI.client
from pyAMI.atlas.api import get_dataset_info

def SampleHandler_QueryAmi(samples) :
    # set up an AMI client
    # This is the basic minimum - and it will look for an encrypted file with your user credentals
    # If it does not find that it will try for a VOMS proxy
    # Make the encrypted file by running the amo command
    # ami auth
    # first.
    # In the pyAMI doc you will find an example of how to get your program to request a
    # the user to make a file.
    # https://atlas-ami.cern.ch/AMI/pyAMI/examples/api.html

    amiClient=pyAMI.client.Client('atlas')
    # Extract from your mail
    ###############
    # The quantities in the class MetaDataSample are pretty much all I need:
    #    * whether it is data or MC
    #    * the luminosity of the sample
    #    * the k-factor of the sample (only for MC)
    #    * the number of events in the sample
    #    * the cross section of the sample (only for MC)
    #    * the filter efficiency
    
    data = ROOT.SH.MetaDataQuery()
    data.messages = 'done by ami query'
    # I am assuming that "samples" is a list of dataset names, and that
    # the user already checked that they exist and are valid
    for sample in samples :
        sample_noscope=sample.split(':')[-1]

        mydata = ROOT.SH.MetaDataSample(sample)
        # The first question you ask is it data or mc.
        # Actually you should be able to tell this without ambiguity from the name
        # without going to the trouble of a request to AMI.
        #  description: 1 for data, 0 for MC, or -1 if this is not known.

        mydata.source = 'https://atlas-ami.cern.ch/AMI/pyAMI/'
        mydata.unknown = 0

        if (sample.startswith("mc")) :
            mydata.isData=0
            pass
        elif (sample.startswith("data")) :
            mydata.isData=1
            pass
        else :
            mydata.isData=-1
            pass

        # You are calling ths AMI functions with tid suffixes.
        # AMI does not specifically catalogue TID datasets so
        # I am stripping off the suffix.
        # Normally uses should not be concerned with these datasets
        # but only with the containers.
        # However if you are really only interested in the output of a particular
        # prodsys task then we can do it - but it would be more complex
        # as we need to redo the event and cross section calculations
        # just for those tasks.
        if (sample.find("_tid")):
            print ("Stripping tid suffix from " + sample)
            sample = sample.split("_tid")[0]
            pass


        # All datasets should have the number of events.
        # have to convert this to a long int I suppose?
        amiinfo=get_dataset_info(amiClient, sample_noscope)[0]
        mydata.nevents = long(amiinfo['totalEvents'])

        # AMI does not yet have a function for getting luminosity.
        # It IS on the todo list, as luminosity info per run is available
        # in COMA, and AMI has access to the information in principle
        # So this is in part a place holder
        # I do not know anything about k-factor. We have no such parameter sent to us.
        # This should be taken up with the MC people I suppose.
        
        if (mydata.isData==1):
            # get luminosity for the run
            mydata.crossSection=-1
            mydata.filterEfficiency=-1
            pass
        else:
            mydata.luminosity = -1
            # MC - can get cross-section and filter efficiency
            xsec=float(amiinfo['approx_crossSection'])
            effic=float(amiinfo['approx_GenFiltEff'])
            # + conversion string to float.
            mydata.crossSection= xsec
            mydata.filterEfficiency= effic
            if mydata.crossSection > 0 and mydata.filterEfficiency > 0:
                mydata.luminosity= float (float(mydata.nevents)/(mydata.crossSection*mydata.filterEfficiency))
                pass
            pass
          
        data.addSample (mydata)
        # print "cross section = "+str(mydata.crossSection)+", filter efficiency = "+str(mydata.filterEfficiency)+", nEvents= "+str(mydata.nevents)
        pass
    return data




 
