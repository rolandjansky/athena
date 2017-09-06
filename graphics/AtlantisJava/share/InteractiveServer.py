#!/usr/bin/env python

# InteractiveServer.py
#
# author: Zdenek Maxa, UCL HEP group (zdenek.maxa --at-- hep.ucl.ac.uk)
#
# CVS location: offline/graphics/AtlantisJava/share/InteractiveServer.py
#
# Interface between the Athena Python interactive prompt and the Atlantis
# event display. This server script takes over the interactive prompt of
# Athena. The Atlantis user can then type Athena commands directly in the
# Atlantis dialog from where they are sent to Athena through this server.
# Commands: e.g. changing/querying job options and executing .initialize(),
# .execute() and .finalize() methods of algorithms, etc.
#
#
# setting up Atlantis - Interactive Athena:
#  0) get_files InteractiveServer.py [from 'run' directory]
#  1) athena -i <job_options.py> [launch Athena in the interactive mode]
#  2) theApp.initialize()
#  3) execfile ("InteractiveServer.py")
#  4) if the server started successfully an info message is printed and
#     denoted string has to be copied into the Atlantis Interactive Athena
#     dialog (Atlantis menu: File -> Interactive Athena)
#
#     Once the InteractiveServer is started, the Athena prompt is taken over
#     by the server and the user can no longer access it. The server can be
#     shut down by Ctrl+C after which the interactive prompt should be given
#     back to the user.
#
#
# setting up for Athenaeum:
#  argumentInteractiveServerNoAlgTools = True
#  argumentInteractiveServerServerPort = 2323
#  argumentInteractiveServerServerKey = "someKey"
#  execfile ("InteractiveServer.py")

# Multithreaded version, modification made by Julius.Hrivnac -- at -- cern.ch
# Athenaeum commands can be executed in several parallel threads.
# Unlike in the single-thread version, all stdout and stderr output
# goes into system streams, so into the server log file. The result
# of the "remote script" should be put into answer array so that
# they are given to the client.


# ---------------------------------------------------------------------------

import os, sys, math, traceback, random, time, StringIO, getopt, md5, base64
import SocketServer
from SimpleXMLRPCServer import SimpleXMLRPCServer,SimpleXMLRPCRequestHandler
import cppyy

class AsyncXMLRPCServer(SocketServer.ThreadingMixIn, SimpleXMLRPCServer): pass

# ---------------------------------------------------------------------------
class InteractiveServer:
     """ """

     # <configuration attributes -------->
     debug = False           # debug messages to print  (False / True)
     keyLength = 15
     minCharValue = 33       # ascii table range (generating key)
     maxCharValue = 126      # ascii table range (generating key)
     minPortNumber = 2000
     maxPortNumber = 20000
     maxAttempts = 5         # number of attempts when creating the server
     resultLengthLimit = 400
     # </configuration attributes -------->

     # other attributes
     serverName = ""
     serverPort = 0
     key = ""
     server = None       # XMLRPC server instance

     vxTool = None       # access to vertexing tool (Athena AlgTool)
     visTool = None      # access to JiveXML (EventData2XML) (Athena AlgTool)

     keyForbiddenChars = ("'", '"', ':')
     noAlgTools = False  # if False, AlgTools will be initialised

     # answer in a multhreaded invironment are passed in the global array
     global answer
     answer = {}

     # -----------------------------------------------------------------------
     def __init__(self, noAlgTools = False, serverPort = 0, key = ""):
         """ """

         self.serverName = os.uname()[1]
         self.noAlgTools = noAlgTools
         self.serverPort = serverPort
         self.key = key
         SocketServer.ThreadingTCPServer.daemon_threads = True

     # __init__() ------------------------------------------------------------


     # -----------------------------------------------------------------------
     def makeServer(self):
         """ """

         try:
             SimpleXMLRPCServer.allow_reuse_address = 1
             self.server = AsyncXMLRPCServer((self.serverName, self.serverPort), SimpleXMLRPCRequestHandler)

         except Exception, why:
             print "\n%s\nCould not create the server on '%s' port: " \
                   "'%s'" % (why, self.serverName, self.serverPort)
             if self.debug:
                 trace = traceback.format_exception(*sys.exc_info())
                 traceString = '\n '.join(trace)
                 errorMsg = "ERROR:\n%s\n" % (traceString)
                 print errorMsg
             raise
         else:
             print "XMLRPC server created"

     # makeServer() ----------------------------------------------------------


     # -----------------------------------------------------------------------
     def generatePortAndMakeServer(self):
         """ """

         alreadyTriedPorts = []  # already tried port numbers

         for i in range(self.maxAttempts):
             while 1:
                 self.serverPort = random.randint(self.minPortNumber,
                         self.maxPortNumber)
                 if self.serverPort not in alreadyTriedPorts:
                     break
             alreadyTriedPorts.append(self.serverPort)

             try:
                 self.makeServer()
             except:
                 continue
             else:
                 break
         else:
             # for loop iterated through exhaustion of the list and wasn't
             # terminated by the break, so the server wasn't created, notify
             # the caller by raising an exception
             raise

     # generatePortAndMakeServer() -------------------------------------------


     # -----------------------------------------------------------------------
     def registerMethod(self, name):
         """ """

         methodName = name.__name__
         try:
             self.server.register_function(name)
         except Exception, why:
             print "%s\nCould not register method (%s)" % (why, methodName)
             raise
         else:
             if self.debug:
                 print "method '%s()' registered" % methodName

     # registerMethod() ------------------------------------------------------


     # -----------------------------------------------------------------------
     def runServer(self):
         """ """
         # this will probably have to be in a thread if the interactive
         # prompt is to remain at the user's disposal

         print "Waiting for requests..."
         try:
             self.server.serve_forever()
         except KeyboardInterrupt:
             try:
                 print "Stopped from keyboard, exit"
                 print "Closing InteractiveServer:",
                 self.server.server_close()
             except:
                 print "can't close the server"
             else:
                 print "server closed"
         except Exception, why:
             print "%s\nException occured" % why
             raise

     # runServer() -----------------------------------------------------------


     # -----------------------------------------------------------------------
     def execute(self, param = "<empty>"):

         print "execute(param = '%s')" % param
         resultMsg = "SUCCESS"

         try:
             exec param
         except Exception, why:
             trace = traceback.format_exception(*sys.exc_info())
             traceString = '\n '.join(trace)
             resultMsg = "ERROR:\n%s\n%s\n" % (traceString, why)
             print resultMsg

         return str(resultMsg)

     # execute() -------------------------------------------------------------


     # -----------------------------------------------------------------------
     def get(self, name = "<empty>"):
         """ """

         print "get(name = '%s')" % name
         resultMsg = "SUCCESS"

         try:
             result = eval(name)
         except Exception, why:
             trace = traceback.format_exception(*sys.exc_info())
             traceString = '\n '.join(trace)
             resultMsg = "ERROR:\n%s\n%s\n" % (traceString, why)
             print resultMsg
             return str(resultMsg)
         else:
             return str(result)

     # get() -----------------------------------------------------------------



     # -----------------------------------------------------------------------
     def callVertexFit(self, tracksId):
         """Track IDs (tracksId array) are passed to vertex fit algtool"""

         print "callVertexFit(tracksId = '%s')" % tracksId

         cppyy.loadDictionary('SealSTLDict')

         g.std.vector(int)
         vi = g.std.vector(int)(0)

         for i in tracksId:
             vi.push_back(i)

         print "    calling vertexing tool: vxTool.fitAtlantisTracks(",vi,")"

         r = self.vxTool.fitAtlantisTracks(vi)
         if r:
             resultMsg = "SUCCESS (get updated event data)"
         else:
             resultMsg = "ERROR while calling vertexing tool"

         return resultMsg

     # callVertexFit() -------------------------------------------------------


     # -----------------------------------------------------------------------
     def executeScript(self, param = "<empty>", pid = ""):
         """Execute Python code snippets from Athenaeum plugin"""

         errorMsg = "SUCCESS"
         resultMsg = ""

         try:
           exec param
           global answer
           try:
             resultMsg = answer[pid]
             del answer[pid]
           except:
             resultMsg = "No resultMsg"
         except Exception, why:
             trace = traceback.format_exception(*sys.exc_info())
             traceString = '\n '.join(trace)
             errorMsg = "ERROR:\n%s\n%s\n" % (traceString, why)

         return resultMsg + "\n" + str(errorMsg)

     # executeScript() -------------------------------------------------------


     # -----------------------------------------------------------------------
     # the only method which is exposed (registred) for remote calling
     def process(self, digest = "", request = "<empty>", tracksId = ''):
         """ """

         print "\n\n<incoming request> " + 50 * '-'
         if self.debug:
             print "process(request = '%s', tracksId = '%s')" % (request,
                     tracksId)
         result = "<empty_result>"

         # generate message digest and compare
         md5My = md5.new()
         md5My.update(request)
         md5My.update(self.key)
         myDigest = md5My.digest()
         # received digest is BAE64 encoded, so encode mine as well
         myDigest = base64.encodestring(myDigest)[:-1] # without list '\n'

         if digest == myDigest:
             # determine the nature of the request
             # request 'runVertexFit' -> pass tracksId to the algorithm
             if str(request) == "runVertexFit":
                 result = self.callVertexFit(tracksId)

             # request to process Python code snippet (e.g. from Athenaeum)
             # multiline request
             # tracksId is the client identifier
             elif len(str(request).split('\n')) > 1:
                 result = self.executeScript(request, tracksId)

             # get event data from JiveXML
             elif str(request) == "eventData":
                 self.visTool.execute()
                 result = self.visTool.getXML()

             # two last characters must be '()' if it's a method to execute
             elif request[-2:] == "()":
                 result = self.execute(request)

             # if the request contains '=' - it's an assignment to execute
             elif request.find('=') > -1:
                 result = self.execute(request)

             # it's likely name of a variable to evaluate (or wrong request)
             else:
                 result = self.get(request)
         else:
             result = "Request not processed, keys (digests) don't match"


         # don't print out result messages longer than resultLengthLimit chars
         # in such a case print just the beginning and the end of the message
         limit = self.resultLengthLimit
         print "result to send:"
         if len(result) < limit:
             print "%s" % result
         else:
             print "%s" % result[:limit/2]
             print "\n+++ %s other characters +++\n" % (len(result) - limit)
             print "%s" % result[-limit/2:]

         print "</incoming request> " + 50 * '-'
         return result

     # process() -------------------------------------------------------------


     # -----------------------------------------------------------------------
     def generateKey(self):
         """ """

         # generate keyLength characters long key
         random.seed()
         while len(self.key) < self.keyLength:
             k = str(chr(random.randint(self.minCharValue, self.maxCharValue)))
             if k in self.keyForbiddenChars:
                 continue
             self.key += k

     # generateKey() ---------------------------------------------------------

     # -----------------------------------------------------------------------
     def printInfo(self):
         """ """

         print "String between the quotation marks to copy & paste",
         print "into the dialog:"
         print "\t '%s:%s:%s'" % (self.serverName, self.serverPort, self.key)

     # printInfo() -----------------------------------------------------------


     # -----------------------------------------------------------------------
     def createAlgTools(self):

         print "Getting vertexing AlgTool (ToolSvc.VxWrapperTool)"
         # self.vxTool = \
         #       theApp.toolsvc().retrieve('ToolSvc.VxWrapperTool')._itool
         self.vxTool = \
                 theApp.toolsvc().create('VxWrapperTool','VxWrapperTool',
                         cppyy.Pointer(g.IInterface)(0))._itool

         print "Getting JiveXML AlgTool (ToolSvc.EventData2XML)"
         # self.visTool = \
         #       theApp.toolsvc().retrieve('ToolSvc.EventData2XML')._itool
         self.visTool = \
                 theApp.toolsvc().create('EventData2XML','EventData2XML',
                         cppyy.Pointer(g.IInterface)(0))._itool

     # createAlgTools() ------------------------------------------------------


     # -----------------------------------------------------------------------
     def start(self):
         """ """

         print "Starting Multithreaded InteractiveServer:",
         try:
             # generate key if it wasn't specified as a command line option
             if self.key == "":
                 self.generateKey()

             # choose random port before creating the server if the port number
             # wasn't specified as a command line option
             if self.serverPort == 0:
                 self.generatePortAndMakeServer()
             else:
                 self.makeServer()

             self.registerMethod(self.process)
             # self.server.register_introspection_functions() # what for?

             # don't initialise algtools if --no_algtools command line argument
             if self.noAlgTools == False:
                 self.createAlgTools()

             self.printInfo()
             self.runServer()
         except Exception, why:
             print "Exception occured."
             print "Reason: %s" % why

     # start() ---------------------------------------------------------------


# class InteractiveServer ===================================================


# ---------------------------------------------------------------------------
def getArguments():
     """Reading arguments when executing by execfile()"""

     # default values
     noAlgTools = False
     serverPort = 0
     serverKey = ""

     if globals().has_key("argumentInteractiveServerNoAlgTools"):
         if argumentInteractiveServerNoAlgTools:
             noAlgTools = True

     if globals().has_key("argumentInteractiveServerServerPort"):
         port = argumentInteractiveServerServerPort
         try:
             serverPort = int(port)
         except ValueError:
             msg = "Incorrectly specified port number: %s" % port
             raise Exception(msg)

     if globals().has_key("argumentInteractiveServerServerKey"):
         serverKey = argumentInteractiveServerServerKey

     return (noAlgTools, serverPort, serverKey)

# getArguments() ------------------------------------------------------------


# ---------------------------------------------------------------------------
def main():

     try:
         (noAlgTools, serverPort, key) = getArguments()
     except Exception, why:
         print "Exception occured while reading variable arguments, exit"
         print "Reason: %s" % why
         return

     server = InteractiveServer(noAlgTools, serverPort, key)
     server.start()

# main() --------------------------------------------------------------------


# ---------------------------------------------------------------------------
if __name__ == "__main__":
     main()
# ---------------------------------------------------------------------------
