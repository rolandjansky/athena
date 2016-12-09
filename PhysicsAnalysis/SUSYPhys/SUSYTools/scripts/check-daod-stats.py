#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys
import os

# keep ROOT from stealing our command line arguments
_orig_argv = sys.argv[:]
sys.argv = [_orig_argv[0]]

import ROOT
ROOT.gROOT.Macro("$ROOTCOREDIR/scripts/load_packages.C")
ROOT.gROOT.ProcessLine("gErrorIgnoreLevel = 1001;")
from ROOT import *

sys.argv = _orig_argv

try:
	import pyAMI.client
	import pyAMI.atlas.api as atlas_api
except:
	print "Please call"
	print "  localSetupPyAMI"
	print "  voms-proxy-init -voms atlas"
	print "before using this script"
	sys.exit(1)


class term:
    GREEN = '\033[92m'
    RED = '\033[91m'
    ENDC = '\033[0m'
    CLEAR_LINE = "\33[2K\r"

def get_ami_events(ami_client, dataset):
	results = atlas_api.get_dataset_info(ami_client, dataset)
	if len(results) != 1:
		print "WARNING: %d results returned from AMI, expected 1" % len(results)

	return int(results[0]["totalEvents"])

def get_xaod_name(dataset):
	if ":" in dataset:
		dataset = dataset.split(":")[1]

	parts = dataset.split(".")
	xaod_parts = []
	for p in parts:
		if "DAOD" in p: # remove derivation type
			p = "AOD"

		if "_p" in p: # remove p-tag
			if p.endswith("/"):
				p = p[:-7] # 7 == len("_p1234/")
			else:
				p = p[:-6] # 6 == len("_p1234")

		xaod_parts.append(p)

	return ".".join(xaod_parts)

def get_cbk_events(root_file):
	tree = xAOD.MakeTransientMetaTree(root_file)
	tree.GetEntry(0)

	max_cycle = -1
	all_events_cbk = None
	for cbk in tree.CutBookkeepers:
		if cbk.cycle() > max_cycle and cbk.name() == "AllExecutedEvents" and cbk.inputStream() == "StreamAOD":
			max_cycle = cbk.cycle()
			all_events_cbk = cbk

	return all_events_cbk.nAcceptedEvents()

def get_local_events(files):
	num_files = len(files)

	daod_events = 0
	cbk_events = 0

	for i in xrange(num_files):
		sys.stdout.write("Reading local statistics, file %d/%d (%d events so far)..." % (i+1, num_files, daod_events))
		sys.stdout.flush()

		rf = TFile.Open(files[i])
		daod_events += rf.Get("CollectionTree").GetEntries()
		cbk_events += get_cbk_events(rf)
		rf.Close()

		sys.stdout.write(term.CLEAR_LINE)
		sys.stdout.flush()

	return daod_events, cbk_events

def check_sample_stats(ami_client, dataset, cbk_events):
	xaod = get_xaod_name(dataset)
	ami_events = get_ami_events(ami_client, xaod)

	if ami_events == cbk_events:
		print term.GREEN + "CutBookkeepers good: %d events" % ami_events, term.ENDC
	else:
		print term.RED + "CutBookkeepers bad: %d events (local), %d events (AMI)" % (cbk_events, ami_events), term.ENDC

	return ami_events == cbk_events

def check_derivation_stats(ami_client, dataset, daod_events):
	ami_events = get_ami_events(ami_client, dataset)

	if ami_events == daod_events:
		print term.GREEN + "Derviation events good: %d events" % ami_events, term.ENDC
	else:
		print term.RED + "Derviation events bad: %d events (local), %d events (AMI)" % (daod_events, ami_events), term.ENDC

	return ami_events == daod_events

def load_files(opts):
	infile = open(opts.file_list, "r")

	files = []
	for line in infile:
		line = line.strip()
		if line == "" or line.startswith("#"):
			continue

		files.append(line)

	return files

def parse_opts():
	import argparse

	parser = argparse.ArgumentParser()
	parser.add_argument("dataset", help="Name of the dataset, something like mc15_13TeV.123456.SomeGenerator.DAOD_SUSY5.e1234_p4321")
	parser.add_argument("file_list", help="List of files, belonging to the dataset")

	opts = parser.parse_args()

	return opts

def main():
	opts = parse_opts()

	files = load_files(opts)

	print "Initializing PyAMI"
	ami_client = pyAMI.client.Client("atlas")
	atlas_api.init()

	print "Checking dataset %s with %d files" % (opts.dataset, len(files))

	daod_events, cbk_events = get_local_events(files)

	good = True
	if not check_derivation_stats(ami_client, opts.dataset, daod_events):
		good = False
	if not check_sample_stats(ami_client, opts.dataset, cbk_events):
		good = False

	if not good:
		sys.exit(1)

if __name__ == '__main__':
	main()
