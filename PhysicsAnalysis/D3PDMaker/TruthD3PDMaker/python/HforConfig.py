# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

class HforConfig:
    """
    Class to work out the DSID and find the corresponding string used to configure the Hfortool in a file on CVMFS 
    
    Config strings are usually of the form isBB, isCC etc.

     """
    def __init__(self):
        """
        Sets up logging and class variables needed

        """      
        from AthenaCommon.Logging import logging
        self.hforLog = logging.getLogger('Hfor')
        self.hforLog.debug("****************** STARTING Hfor Configuration *****************")

        self.file = ""
        self.curr_DSID = ""
        self.config_String = "fail"
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        self.fList = athenaCommonFlags.FilesInput()




    def getSampleType(self, runArgs, filepath):
        """ 
        function called from outside the class to work out the config string, gives the class access to runArgs variables
 
        (runArgs are the arguaments given to Reco_trf at the command line) 

        """
        self.getConfigFile(runArgs, filepath)
        self.getDSID(runArgs)
        self.getHforType()
        return self.config_String




    def getConfigFile(self, runArgs, filepath):
        """
            Function to determine the location of the text file that contains the DSIDs and their corresponding configuration strings

            There is a default location that can be overridden

        """

        #Identify Hfor type
        import sys

        try:
            self.file = open(filepath)
        except Exception:
            currentError = "Exiting. Configuration file should be in "+str(filepath)
            self.hforLog.error(currentError)
            sys.exit(0)


        #find and open the file to configure mode based on DSID
        currentError = "using file found in "+filepath
        self.hforLog.info(currentError)




    def getDSID(self, runArgs):

            """
            Finds the DSID of the dataset being used. 
       
            Note: It must be specified as a command line option for running on the grid
            For local running it is possible to configure it from the container name

            """
           #try getting the DSID from the RunNumber variable in the file
            import PyUtils.AthFile as af
            f = af.fopen( self.fList[0] )
            if len(f.run_numbers) > 0:
                self.curr_DSID = f.run_numbers[0]
            else:
                import re
                try:
                    #try to get the DSID from runArgs
                    self.curr_DSID = runArgs.RunNumber
                except Exception:
                    #if cannot access runargs parse input file name for DSID
                    if len(self.fList) != 0:
                        files = self.fList 
                        firstFile = files[0].split(".")

                        for index, x in enumerate(firstFile):
                            if re.search('mc[1234567890]{2}', x) is not None:
                                self.curr_DSID = firstFile[index+1]
                        try:
                            int(self.curr_DSID)
                        except Exception:
                            self.hforLog.error("Could not find DSID from filename. The Hfor tool will not be correctly configured! Have you obeyed the naming convention?")
                            self.curr_DSID = 0

                    else:
                        self.hforLog.error("No DSID found. Is the naming convention correct?")




    def getHforType(self):

            """
            Find the configuration string that corresponds to the DSID
    

            """

            proc_dict = {}
            for line in self.file:
                #Dont want to look at comments so lets remove anything beginning with a # symbol
                line = line[:line.find("#")].strip()
                line = line.rstrip()
                if not line.strip():
                    continue



                if line:
                    #split the line into a DSID and a sample config string
                    proc_list = line.split()
                    proc_dict[proc_list[0]] = proc_list[1]
                    if int(proc_list[0]) == int(self.curr_DSID):
                        self.config_String = proc_list[1]
                        break

            self.file.close()


            if self.config_String == "fail":
                self.hforLog.warning("failed to find DSID in configuration file. Hfor has not been activated. Does this sample require Hfor? ")
