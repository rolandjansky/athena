import os, time,sys
from PyCool import cool
from CoolConvUtilities import AtlCoolLib
import argparse
# For resolving tags
sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')

#try:
#    runnumber=int(input('Input:'))
#except ValueError:
#    print ("Not a number")

p = argparse.ArgumentParser()
p.add_argument('-f', '--input_file', type=str, help='Please type the path to the input file!',default='000000')
p.add_argument('-r', '--run', type=int, help='Please type the runnumber!',default='000000')
args=p.parse_args()
print("Input file: ",args.input_file)
print("Run number: ",args.run)

dbFile = 'PixelModuleFeMask_run'+str(args.run)+'.db'
dbName = 'CONDBR2'
path='/PIXEL/PixelModuleFeMask'
tag = 'PixelModuleFeMask-RUN2-DATA-UPD4-05'
print("Tag for database: ",tag)

if os.path.isfile(dbFile):
    os.remove(dbFile)


dbSvc = cool.DatabaseSvcFactory.databaseService()
dbString = "sqlite://;schema=%s;dbname=%s" % (dbFile, dbName)
try:
    db = dbSvc.createDatabase(dbString)
except e:
    print('Problem creating database', e)
    sys.exit(-1)
print ("Created database", dbString)

spec = cool.RecordSpecification()
spec.extend('data_array', cool.StorageType.String16M)
desc = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader><typeName>CondAttrListCollection</typeName>'
folderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, spec)
folder = db.createFolder(path, folderSpec, desc, True)
data = cool.Record(spec)

input_file = open(args.input_file,'r')
nModules = sum(1 for line in input_file) 
input_file.close()

input_file = open(args.input_file,'r')
input_list=[[0 for x in range(4)] for y in range(nModules)]
iterator=0
for line in input_file:
  input_list[iterator] = list(map(int, line.split()[0:4]))
  iterator=iterator+1
input_file.close()

input_list = sorted(input_list, key=lambda x: x[0])

runnumber_large=args.run << 32
iov_start_min=100000000000000000000
iov_end_max=0
payload_list=[]
module_old=0;
FE_code_old=0;

for k in range(10000):				#number maybe needs to be set to a higher value if run has more than 10000 LBs
 input_file = open(args.input_file,'r')
 payload = ""
 
 for i in range(nModules):
  module=input_list[i][0]
  FEcode=input_list[i][1]
  iov_start=input_list[i][2]
  iov_end=input_list[i][3]
  stop=0
  if (iov_start<=runnumber_large+k and iov_end>=runnumber_large+k) or (iov_start==0 and iov_end==0):
    if(module_old==module):
     if module==input_list[i-1][0] and FEcode!=0:
       FEcode=0
     if(FEcode!=0):
       continue
     remove='"'+str(module_old)+'":'+str(FE_code_old)+","
     payload=payload.replace(remove,"")
    module_old=module
    FE_code_old=FEcode
    #if(FEcode!=0):#only if FE information not wanted
      #continue 
    payload=payload+'"'+str(module)+'":'+str(FEcode)+','
  if iov_start<iov_start_min and iov_start!=0:
    iov_start_min=iov_start
  if iov_end>iov_end_max:
    iov_end_max=iov_end
 payload=payload[:-1]
 payload="{"+payload+"}"
 payload_list.append(payload)
 
iov_end_max=iov_end_max-runnumber_large
iov_start_min=iov_start_min-runnumber_large
since_bool=False
since=0
until=0
channel=0
print(iov_end_max,iov_start_min, runnumber_large)
for k in range(1000000):#4294967295
 if(k>=iov_start_min and k<=iov_end_max):
  if(k==iov_start_min):
   since=runnumber_large+k

  if (payload_list[k]==payload_list[k+1]):
   if(since_bool==False):
    since = runnumber_large+k
    since_bool=True
  else:
   since_bool=False
   until = runnumber_large+k+1
   data['data_array'] = payload_list[k]
   folder.storeObject(since,until,data,channel,tag)

  if(since_bool==False):
   since = runnumber_large+k+1

input_file.close()
db.closeDatabase()
