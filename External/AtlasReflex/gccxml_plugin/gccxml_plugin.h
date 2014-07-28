/* gccxml_plugin - A GCC plugin, dumping translation units as XML.

Copyright 2013 - Alex Leach <beamesleach /at/ gmail /dot/ com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the

  Free Software Foundation, Inc.
  51 Franklin Street, Fifth Floor
  Boston, MA  02110-1301  USA

*/

/**
 * \file gccxml_plugin.h
 *
 * Provides gccxml_plugin.c with external definitions.
 */

#ifdef PACKAGE_VERSION
# undef PACKAGE_VERSION
#endif
#include "plugin/include/config.h"
#ifdef HAVE_DECL_GETOPT
# undef HAVE_DECL_GETOPT
#endif
#define HAVE_DECL_GETOPT 1

#include "gcc-plugin.h"

#define GCC_XML_C_VERSION "GCC-XML Plugin Version: " GCCXML_PLUGIN_VERSION

/* These were hacked in to GCC's c-common/c.opt file, so don't exist for
 * the plugin. We populate them in the callback routine,
 * `collect_callback_args`.   */
static char *flag_xml_start;

/* Counter for number of missed GCCXML_DECL_ERROR checks */
static int n_gccxml_decl_errors_missed;

#ifdef __cplusplus
  extern "C" {
#endif

void do_xml_output (const char*);
void do_pass(void* gcc_data);
void do_type(void* gcc_data);

#ifdef __cplusplus
  }
#endif
