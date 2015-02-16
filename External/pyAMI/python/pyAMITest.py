#!/usr/bin/env python
# -*- coding: utf-8 -*-
#############################################################################
# Author  : Jerome ODIER, Jerome FULACHIER, Fabian LAMBERT, Solveig ALBRAND
#
# Email   : jerome.odier@lpsc.in2p3.fr
#           jerome.fulachier@lpsc.in2p3.fr
#           fabian.lambert@lpsc.in2p3.fr
#           solveig.albrand@lpsc.in2p3.fr
#
#############################################################################

import sys, pyAMI.utils, pyAMI.client, pyAMI.exception, pyAMI.atlas.api

#############################################################################

if __name__ == '__main__':

	ret = 0

	#####################################################################

	print('#################### TEST 1 ####################')

	try:
		client = pyAMI.client.Client('atlas')

		print('[SUCCESS]')

	except pyAMI.exception.Error as e:
		pyAMI.utils.safeprint('[ERROR] %s' % e)

		sys.exit(1)

	#####################################################################

	print('#################### TEST 2 ####################')

	try:
		command = [
			'GetUserInfo',
			'-amiLogin=jodier',
		]

		pyAMI.utils.safeprint(client.execute(command))

		print('[SUCCESS]')

	except pyAMI.exception.Error as e:
		pyAMI.utils.safeprint('[ERROR] %s' % e)

		ret = 1

	#####################################################################

	print('#################### TEST 3 ####################')

	try:
		pyAMI.utils.safeprint(client.execute('GetUserInfo -amiLogin=jodier', format = 'xml'))
		pyAMI.utils.safeprint(client.execute('GetUserInfo -amiLogin=jodier', format = 'json'))
		pyAMI.utils.safeprint(client.execute('GetUserInfo -amiLogin=jodier', format = 'csv'))
		pyAMI.utils.safeprint(client.execute('GetUserInfo -amiLogin=jodier', format = 'text'))

		print('[SUCCESS]')

	except pyAMI.exception.Error as e:
		pyAMI.utils.safeprint('[ERROR] %s' % e)

		ret = 1

	#####################################################################

	print('#################### TEST 4 ####################')

	try:
		pyAMI.utils.safeprint(client.execute('GetUserInfo -amiLogin=jodier', format = 'dom_object').get_rows())

		pyAMI.utils.safeprint(client.execute('GetUserInfo -amiLogin=jodier', format = 'dict_object').get_rows())

		print('[SUCCESS]')

	except pyAMI.exception.Error as e:
		pyAMI.utils.safeprint('[ERROR] %s' % e)

		ret = 1

	#####################################################################

	print('#################### TEST 5 ####################')

	try:
		pyAMI.utils.safeprint(pyAMI.atlas.api.list_datasets(client, patterns = 'data14_cos%', type = 'AOD', order = 'run_number', limit = 10))

		print('[SUCCESS]')

	except pyAMI.exception.Error as e:
		pyAMI.utils.safeprint('[ERROR] %s' % e)

		ret = 1

	#####################################################################

	sys.exit(ret)

#############################################################################
