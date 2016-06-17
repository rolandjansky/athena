#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# $Id: job.py,v 1.2 2007-03-07 10:32:43 stelzer Exp $
# Created by Andre DOS ANJOS <Andre.dos.Anjos@cern.ch>, 10-Apr-2006

import sqlutils
from copy import deepcopy
import xml.dom.minidom
import jobasic
import hltmenu

class Job(object):
    """A type that defines how a job should look like. A job is a composition
    of a set of signatures, a setup and release or summary. A few use-cases are
    foreseen at this time:
    1. The user has a job and wants to validate it and write into the DB or
    to an XML file
    1.1 The user has a summary within the XML file
    1.1.1 The user has a summary but wants to ignore it
    1.2 The user has only the setup and knows a key (row or name/version) of a
    release inside the configuration database
    2. The user wants to read the job from the database and write it to XML"""

    def __init__(self, *args):
        """Defines how you can initialize this object. Here are the current
        implemented options:
        1. name, version, list of setups, a release name and a version
        2. name, list of setups, a release name and a version
        3. cursor, job row identifier
        4. top-level XML node"""

        menu = None
        
        if len(args) == 5:
            #INSERTION_POINT: Here, you have to extend the number of
            #arguments to 6 and transform the input list into the following:
            #(name, version, setup, menu, release_name, release_version)
            (name, version, setup, release_name, release_version) = args
            setup = deepcopy(setup)
            
        elif len(args) == 4:
            #INSERTION_POINT: Here, you have to extend the number of
            #arguments to 5 and transform the input list into the following:
            #(name, setup, menu, release_name, release_version)
            (name, setup, release_name, release_version) = args
            version = ''
            setup = deepcopy(setup)

        elif len(args) == 2:
            (cursor, row) = args  # row is the HLT master key
            (name, version) = sqlutils.read_job(cursor, row)
            print "Found configuration '%s', version %s" % (name,version)
            #setup_ids = sqlutils.get_job_setups(cursor, row)
            #setup = []
            #for sid in setup_ids: setup.append(jobasic.Setup(cursor, sid))
            #(release_name, release_version) = \
            #               sqlutils.read_job_release(cursor, row)
            (menu_id, ps_id) = sqlutils.get_job_menu_and_ps(cursor, row)
            print "Found trigger menu %i, ps %i" % (menu_id, ps_id)
            if menu_id is not None and ps_id is not None:
                menu = hltmenu.HLTMenu(cursor,menu_id,ps_id)
            
            #INSERTION_POINT: Here, we can insert the readout of the menu, from
            #the DB.

        elif len(args) == 1 and isinstance(args[0], xml.dom.minidom.Node):
            #The job is composed of several parts. One of them are the
            #setups. Read them all at once:

            #1. Read the summary information, to get the release info
            name = args[0].getAttribute('name')
            version = args[0].getAttribute('version')
            info = args[0].getElementsByTagName('release_info')
            release_name = info[0].getAttribute('name')
            release_version = info[0].getAttribute('version')

            #2. Read the setups available
            setup = []
            for s in args[0].getElementsByTagName('setup'):
                setup.append(jobasic.Setup(s))

            #3. Read the chains and sequences
            menu = None
            xml_signature = args[0].getElementsByTagName('CHAIN_LIST')
            if len(xml_signature) > 1:
                raise SyntaxError, "I can only have one chain list per XML file"
            if len(xml_signature) == 0:
                raise SyntaxError, "No chain list specified in XML file"
            xml_sequence = args[0].getElementsByTagName('SEQUENCE_LIST')
            if len(xml_sequence) > 1:
                raise SyntaxError, "I can only have one sequence list per XML file"
            if ((len(xml_signature) + len(xml_sequence)) != 0 and
                (len(xml_signature) + len(xml_sequence)) != 2):
                raise SyntaxError, "If you have a sequence you have to have a chain list!"
            if len(xml_signature) == 1:
                menu = hltmenu.HLTMenu(xml_signature[0], xml_sequence[0])

        else:
            raise SyntaxError, "Unsupported initialization method for Job"

        self.name = name
        self.version = version
        self.release_name = release_name
        self.release_version = release_version
        self.setup = setup
        self.hltmenu = menu

    def __getitem__(self, key):
        """Implements the [] operator, so this looks like a dictionary."""
        if key == 'name': return self.name
        elif key == 'version': return self.version
        elif key == 'setup': return self.setup
        elif key == 'release_name': return self.release_name
        elif key == 'release_version': return self.release_version
        elif key == 'hltmenu': return self.hltmenu
        raise KeyError, "key %s not found in Job object" % key

    def __eq__(self, other):
        """Compares myself to another Job"""
        #Do not compare versions here!
        if self.name == other.name and \
           self.release_version == other.release_version and \
           self.release_name == other.release_name and \
           len(self.setup) == len(other.setup):
            for s in range(0, len(self.setup)):
                if self.setup[s] not in other.setup: return False
                if other.setup[s] not in self.setup: return False
            #INSERTION_POINT: Here, you can put an entry for checking if the
            #hltmenus match
            #self.hltmenu == other.hltmenu
            return True
        return False
    
    def __ne__(self, other):
        """Compares myself to another parameter"""
        return not self == other

    def validate(self, release):
        """Validates this Job against a given release. This procedure
        is a placeholder for the multiple checks that have to done for a job to
        be inserted into the trigger configuration database."""
        for s in self.setup: s.validate(release)

        #INSERTION_POINT:
        #2. Make sure one of the two conditions is fulfilled:
        #a) There are no hltmenus and there is a single setup
        #b) There are hltmenus and _at least_ one setup

        #3. Make sure all elements point to existing setups
        #4. Make sure all elements point to components in a setup and they are
        #   algorithms
        return True

    def validate_from_db(self, cursor):
        """Validates this Job against a release in the database. This procedure
        is a placeholder for the multiple checks that have to done for a job to
        be inserted into the trigger configuration database."""
        #1. Make sure all the setups validate against the release:
        release_row = sqlutils.get_release_id(cursor, self.release_name,
                                              self.release_version)
        release = jobasic.Release(cursor, release_row) #read the release
        return self.validate(release)

    def rebind(self, release):
        """Rebind this Job against a different release. This will also trigger
        validation."""
        try:
            self.validate(release)
        except:
            print "[Error] Job %s cannot bind against release %s-%s" % \
                  (self.name, release.name, release.version)
            raise
        self.release_name = release.name
        self.release_version = release.version
        return True

    def xml_node(self, doc):
        """Builds itself, recursively and into the document"""
        retval = doc.createElement('job')
        retval.setAttribute('name', self.name)
        retval.setAttribute('version', self.version)
        release_info = doc.createElement('release_info')
        release_info.setAttribute('name', self.release_name)
        release_info.setAttribute('version', self.release_version)
        retval.appendChild(release_info)
        for s in self.setup: retval.appendChild(s.xml_node(doc))
        (seq,sig) = self.hltmenu.xml_node(doc)
        #         hltmenu_nodes = self.hltmenu.xml_node(doc)
        retval.appendChild(seq)
        retval.appendChild(sig)
        #return (retval, seq, sig)
        return (retval)

    def py_print(self, release):
        """Prints this job in python"""
        retval = ''
        for s in self.setup: retval += s.py_print(release)
        # retval += self.hltmenu.py_print()
        
        return retval

    def sql_insert(self, cursor, verbose=True):

        """ Inserts myself into the DB. This is actually about inserting all
        the setups concerned and creating an entry in the HLT_MASTER_TABLE. The
        procedure that is executed and the checks depend on the type of job
        being inserted:
        1. Jobs without menus (pure offline jobs), will have a NULL in the
        column that is supposed to point to the HLT_MENU to use and a single 
        2. Jobs with menus will have a NULL pointer poiting to the HLT_SETUP
        table, from the HMT_FORCED_SETUP_ID column.
        """
        # 1. Jobs without menus (assuming that this is the case for now since I
        # don't have the menus variables for testing yet).
        if self.hltmenu is None and len(self.setup) != 1:
            raise AssertionError, \
                  "I cannot have a Job with multiple setups and no HLT menu!"

        if self.hltmenu is None:
            return sqlutils.insert_offline_job(cursor, self, verbose)

        #default use case (a trigger job, with a menu)
        return sqlutils.insert_trigger_job(cursor, self)
    
