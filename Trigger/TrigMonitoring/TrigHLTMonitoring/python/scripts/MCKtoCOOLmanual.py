# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Script to update MCKs in COOL for use at P1
# Author: Xanthe Hoad (xanthe.hoad@cern.ch)
# See https://twiki.cern.ch/twiki/bin/view/Atlas/MaDQM for more information

import argparse
import sys
from PyCool import cool
import os
os.environ['MAM_CORAL_DBLOOKUP_PATH']='/det/tdaq/hlt/mam/authentication/'
os.environ['MAM_CORAL_AUTH_PATH']=os.environ['MAM_CORAL_DBLOOKUP_PATH']
from TrigHLTMonitoring.MenuAwareMonitoringStandalone import MenuAwareMonitoringStandalone
from CoolConvUtilities.AtlCoolLib import indirectOpen

parser = argparse.ArgumentParser(description="Store an MCK in COOL for a run or run range. If MCK = 0, the offline project and version to store as the tag can be specifed, otherwise, the tag is taken from the MCK info stored in the TriggerDB")
parser.add_argument('account', action='store', help='account with write privileges for oracle://ATONR_COOL;schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2', type=str)
parser.add_argument('password', action='store', help='password for account', type=str)
parser.add_argument('--run', action='store', help='run to store MCK for, or first run of range if --runend is used (required)', type=int, required=True)
parser.add_argument('--mck', action='store', help='MCK to store (required)', type=int, required=True)
parser.add_argument('--info',action='store', default="", help='info to store (optional)', type=str)
parser.add_argument('--runend', action='store', help='final run of range (inclusive) (optional)', type=int)
parser.add_argument('--project', action='store', default="", help='offline AtlasProject to use for tag if MCK = 0 (ignored if MCK != 0)', type=str)
parser.add_argument('--version', action='store', default="", help='offline AtlasVersion to use for tag if MCK = 0 (ignored if MCK != 0)', type=str)
args=parser.parse_args()

if args.runend and args.run >= args.runend:
    print "runend must be greater than run"
    sys.exit(2)

if args.mck == 0 and ( not args.project or not args.version ):
    print "Please give the AtlasProject and AtlasVersion that you want to write MCK 0 to COOL for."
    print "project and version arguments are compulsory for MCK 0."
    sys.exit(2)

if args.mck and ( args.project or args.version ):
    print "The AtlasProject and AtlasVersion are taken from the MCK info, and should not be specified manually except for MCK 0."
    sys.exit(2)

print "Checking MCK is valid..."

mam = MenuAwareMonitoringStandalone()

if args.mck and not mam.oi.check_if_mck_id_exists(args.mck):
    # check if a non-zero MCK is a real MCK
    print "MCK does not exist"
    sys.exit(1)
else:
    print "MCK is valid"

connstring = "oracle://ATONR_COOL;schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2;user="+args.account+";password="+args.password
print "Using",connstring
coolDB=indirectOpen(connstring,readOnly=False,oracle=True,debug=True)
if coolDB is None:
    print "Unable to connect to",connstring,"to write MCK to COOL."
    sys.exit(1)

# open the folder for writing
folder_name = 'MenuAwareMonConfigKey'
folder = coolDB.getFolder('/TRIGGER/HLT/' + folder_name )

# set up payload format
schema = [('MonConfigKey', cool.StorageType.UInt32),    # the mck
          ('Info', cool.StorageType.String4k)]          # optional info (currently unused)
rspec = cool.RecordSpecification()
for col, type in schema:
    rspec.extend(col, type)

# set up the payload and tag
iov_since = (int(args.run)<<32)
if args.runend:
    iov_until = (int(args.runend+1)<<32)
else:
    iov_until = (int(args.run+1)<<32)
data = cool.Record(rspec)
data['MonConfigKey'] = args.mck
data['Info'] = args.info
chan = 0
if args.mck:
    release = mam.oi.read_mck_info_from_db(args.mck)['MCK_ATHENA_VERSION']
else:
    release = args.project + '-' + args.version
tag = folder_name.split('/')[-1] + '-' + release

print "Storing MCK",args.mck,"with tag",tag,"for IOV",iov_since,"to",iov_until
folder.storeObject(iov_since,iov_until,data,chan,tag)
