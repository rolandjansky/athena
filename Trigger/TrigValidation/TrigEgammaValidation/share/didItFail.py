
import os
import smtplib

RTTToolsAvailable = True

try:
  import RttLibraryToolsImp
except:
  RTTToolsAvailable = False
 
class TrigEgammaValidationLogger:
  def __init__(self):
    self.debugL = []
    self.warningL = []
    self.infoL = []
    self.errorL = []

  def debug(self, line):
    self.debugL.append(line)
    print "[TrigEgammaValidationLogger] [DEBUG]    " + str(line)
  def info(self, line):
    self.infoL.append(line)
    print "[TrigEgammaValidationLogger] [INFO]     " + str(line)
  def warning(self, line):
    self.debugL.append(line)
    print "[TrigEgammaValidationLogger] [WARNING]  " + str(line)
  def error(self, line):
    self.errorL.append(line)
    print "[TrigEgammaValidationLogger] [ERROR]    " + str(line)
  def critical(self, line):
    self.criticalL.append(line)
    print "[TrigEgammaValidationLogger] [CRITICAL] " + str(line)
 
   
class didItFail:
    def __init__(self, argDict={}):
        try:
	   rttDesc = argDict.get('JobDescriptor')
           self.release = rttDesc.paths.release
           self.branch = rttDesc.paths.branch
           self.jobLog = rttDesc.log
           self.jobGroup = rttDesc.jobGroup
           self.jobRunDir = rttDesc.runPath
           if RTTToolsAvailable:
	     self.logger = RttLibraryToolsImp.Logger()
	   else:
	     self.logger = TrigEgammaValidationLogger()

	except:
           self.jobGroup = "Test"
           self.release = "TestRelease"
           self.jobLog = "JobLog"
           self.jobRunDir = "."
           self.branch = ""
           if RTTToolsAvailable:
	     self.logger = RttLibraryToolsImp.Logger()
	   else:
	     self.logger = TrigEgammaValidationLogger()


	self.emails = [];
        fEmails = open("/afs/cern.ch/user/d/dferreir/public/rtt_emails.txt")
	lines = fEmails.readlines();
	for line in lines:
		self.emails.append(line[:-1]);

        fEmails.close()
        #self.emails = ["daniloefl@gmail.com"]

    def run(self):
        self.logger.debug("didItFail.py starts")

        openedFileOk = True
	sendMail = True
	ret = 1
	
	try:
          mailSubject = "TrigEgammaValidation Report - " + self.jobLog + " - " + self.release + ", " + self.branch
          try:
	    self.logger.debug("Trying to open "+os.path.join(self.jobRunDir, "results.txt"))
	    fRes = open(os.path.join(self.jobRunDir, "results.txt"), "r")
	    self.logger.debug("Openned file successfully")
            res = fRes.read()
            mailBody  = "\nRTT Test Report - " + self.jobLog + " - " + self.jobGroup + "\n"
	    mailBody += "Release: " + self.release + "\n"
	    mailBody += "Branch: " + self.branch + "\n\n"
	    mailBody += "Go to https://info-trigegammavalidation.web.cern.ch/info-TrigEgammaValidation/ to see a more precise log.\n\n"
	    mailBody += "Kolmogorov-Smirnov, Jensen-Shannon and Kullback-Leibler (respectively) Test Results:\n"
	    mailBody += res + "\n"
	    mailBody += "Job Log: \n"
	    mailBody += self.jobLog + "\n"
	    mailBody += "Job Run Directory:\n"
	    mailBody += self.jobRunDir + "\n\n"

            fRes.close()
          except IOError:
	    self.logger.error("Couldn't open file "+os.path.join(self.jobRunDir, "results.txt"))
            mailBody  = "\nRTT Test Report - " + self.jobLog + " - " + self.jobGroup + "\n"
	    mailBody += "Release: " + self.release + "\n"
	    mailBody += "Branch: " + self.branch + "\n\n"
	    mailBody += "Go to https://info-trigegammavalidation.web.cern.ch/info-TrigEgammaValidation/ to see a more precise log.\n\n"
	    mailBody += "Test Results NOT available!\n"
	    mailBody += "File with results (results.txt) wasn't generated.\n"
	    mailBody += "This should be reported!\n\n"
	    mailBody += "Job Log:\n" + self.jobLog + "\n"
	    mailBody += "Job Run Directory:\n" + self.jobRunDir + "\n\n"
	    
            openedFileOk = False
	  
          try:
	    self.logger.debug("Trying to connect to SMTP server on localhost")
            smtpServer = smtplib.SMTP('localhost')
	    self.logger.debug("Connected to SMTP server on localhost")
	    self.logger.debug("Trying to send mail ...")
	    smtpServer.sendmail("dferreir@mail.cern.ch", self.emails, "From: dferreir@mail.cern.ch\nSubject: " + mailSubject + "\nTo: " + ", ".join(self.emails) + "\n\n" + mailBody)
	    self.logger.debug("Mail sent")
            smtpServer.quit()
	    self.logger.debug("Closed connection with SMTP server")
	  except smtplib.SMTPException:
	    self.logger.error("SMTPException thrown!")
	    sendMail = False
	  
	  self.logger.debug("Trying to open "+os.path.join(self.jobRunDir, "mail.txt"))
	  try:
            fMail = open(os.path.join(self.jobRunDir, "mail.txt"), "w")
	    self.logger.debug("Openned file successfully")
	    if (sendMail == True):
              fMail.write("Sent mail to " + ", ".join(self.emails) + "\nAbout " + mailSubject + ".\nMail body:\n" + mailBody)
	    else:
	      fMail.write("I wasn't able to send the following mail to " + ", ".join(self.emails) + "\nAbout " + mailSubject + ".\nMail body:\n" + mailBody)
	    fMail.close()
	    self.logger.debug("Closed file successfully")
	  except IOError:
	    self.logger.error("Error trying to open/write at mail.txt file")
            fMail.close()

          # Grep the OK line in the results
	  fileEmpty = True
	  try:
	    self.logger.debug("Trying to open file "+os.path.join(self.jobRunDir, "results.txt")+" for grepping OK line ...")
            fRes_n = open(os.path.join(self.jobRunDir, "results.txt"), "r")
	    self.logger.debug("Openned file successfully")
            res_n = fRes_n.readlines()
            for x in res_n:
	        if (x != ""):
		   fileEmpty = False
                if (x == "Results OK!\n"):
                   ret = 0

            fRes_n.close()
	    self.logger.debug("Read and closed file fine")
          except IOError:
	    self.logger.error("Error trying to open/read file")
	    fRes_n.close()
	  
	except:
	  self.logger.error("Exception thrown!")
	
	if (fileEmpty == True):
	  ret = 1
	  
	self.logger.info("End of test, going to return "+str(ret))
	
	return ret

if __name__ == "__main__":
  import sys
  d = didItFail()
  sys.exit(d.run())

