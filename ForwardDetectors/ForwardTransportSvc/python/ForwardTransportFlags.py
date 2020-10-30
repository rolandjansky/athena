# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class detectorFlag(JobProperty):
    """ Detector Flag. """

    statusOn=True
    allowedTypes=['str']
    allowedValues = ["ALFA", 
                     "ZDC"]
    StoredValue='ALFA'

class beamEnergy(JobProperty):
    """ Energy of the beam. """

    statusOn=True
    allowedTypes=['str']
    allowedValues = ["3.5TeV", 
                     "4.0TeV", 
                     "7.0TeV"]
    StoredValue='4.0TeV'

class collisionBeta(JobProperty):
    """ Beta function at the interaction point. """

    statusOn=True
    allowedTypes=['str']  
    allowedValues = ["0000.55m", 
                     "0000.60m",
                     "0000.65m",
                     "0001.00m",
                     "0001.50m",
                     "0090.00m",
                     "0500.00m",
                     "1000.00m",
                     "2625.00m"]
    StoredValue='0090.00m'

class twissFileType(JobProperty):
    """ Twiss file type: nominal(for performance studies) or real(measured). """

    statusOn=True
    allowedTypes=['str']
    allowedValues = ["nominal", 
                     "real"]
    StoredValue='nominal'

class twissFileVersion(JobProperty):
    """ Twiss file version. """

    statusOn=True
    allowedTypes=['str']
    allowedValues = ["v01", 
                     "v02",
                     "v03",
                     "v04"]
    StoredValue='v01'

class twissFilePath1(JobProperty):
    """ Path to TwissFile1 """

    statusOn=True
    allowedTypes=['str']
    StoredValue='beam1.tfs'

class twissFilePath2(JobProperty):
    """ Path to TwissFile2 """

    statusOn=True
    allowedTypes=['str']
    StoredValue='beam2.tfs'

class ForwardTransportFlags(JobPropertyContainer):
    """ Container of all ForwardTransport flags."""

    def SetTwissFilePaths(self):
        beamEnergy       = self.beamEnergy.get_Value()
        collisionBeta    = self.collisionBeta.get_Value()
        twissFileType    = self.twissFileType.get_Value()
        twissFileVersion = self.twissFileVersion.get_Value()

        base_path = os.getenv('TwissFilesPATH') + "/" + beamEnergy + "/" + collisionBeta + "/" + twissFileType + "/" + twissFileVersion

        twiss1 = base_path + '/beam1.tfs'           
        twiss2 = base_path + '/beam2.tfs'

        self.twissFilePath1.set_Value(twiss1)
        self.twissFilePath2.set_Value(twiss2)

        return

jobproperties.add_Container(ForwardTransportFlags)

list_jobproperties=[detectorFlag,beamEnergy,collisionBeta,twissFileType,twissFileVersion,twissFilePath1,twissFilePath2]

for i in list_jobproperties:
    jobproperties.ForwardTransportFlags.add_JobProperty(i)

forwardTransportFlags = jobproperties.ForwardTransportFlags
