# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Runs single HelloWorld job on LSF batch using rttProvides.
   Mentions a non-existant dataset in the package unidied configuration file. 
   This should provoke the generation of an error message in the job descripor
   which in turn should trigger the construction of an ErrorMinder"""
