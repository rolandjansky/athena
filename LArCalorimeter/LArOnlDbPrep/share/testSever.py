import commands
from SimpleXMLRPCServer import SimpleXMLRPCServer

# Create server
server = SimpleXMLRPCServer(("localhost", 8000))
#server.register_introspection_functions()

def runAthena():
   stat,out=commands.getstatusoutput("athena.py LArOnlDbPrep/LArOnlineDBAlg.py > athLog")
   retval=""
   if stat==0:
     xmlfile=open("output.xml")
     for l in xmlfile:
       retval+=l
		       	
     return retval

server.register_function(runAthena)

# Run the server's main loop
server.serve_forever()

#runAthena()
