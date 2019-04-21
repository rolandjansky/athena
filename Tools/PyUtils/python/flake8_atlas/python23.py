# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# This is a modified version of the 'hacking' flake8 plugin:
#   https://github.com/openstack-dev/hacking
#
# The unmodified original checks are kept with the "hacking_" prefix.
# For modified versions (including bugfixes) the prefix was removed.
#

# Original licence:
# -----------------
#  Licensed under the Apache License, Version 2.0 (the "License"); you may
#  not use this file except in compliance with the License. You may obtain
#  a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
#  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
#  License for the specific language governing permissions and limitations
#  under the License.

from PyUtils.flake8_atlas import utils
import re
import tokenize
import ast

def import_normalize(line):
    # convert "from x import y" to "import x.y"
    # handle "from x import y as z" to "import x.y as z"
    split_line = line.split()
    if ("import" in line and line.startswith("from ") and "," not in line and
            split_line[2] == "import" and split_line[3] != "*" and
            split_line[1] != "__future__" and
            (len(split_line) == 4 or
             (len(split_line) == 6 and split_line[4] == "as"))):
        return "import %s.%s" % (split_line[1], split_line[3])
    else:
       return line


#def hacking_python3x_except_compatible(logical_line, noqa):
#Deleted as this is covered by pycodestyle E722

RE_OCTAL = re.compile(r"0+([1-9]\d*)")

@utils.flake8_atlas
def hacking_python3x_octal_literals(logical_line, tokens, noqa):
    r"""Check for octal literals in Python 3.x compatible form.

    As of Python 3.x, the construct "0755" has been removed.
    Use "0o755" instead".

    Okay:   f(0o755)
    Okay:   f(0755)
    Okay:   f(755)
    Okay:   f(0)
    Okay:   f(000)
    Okay:   MiB = 1.0415
    Fail:   f(0755)
    Okay:   f(0755)  # noqa
    """
    if noqa:
        return

    for token_type, text, _, _, _ in tokens:
        if token_type == tokenize.NUMBER:
            match = RE_OCTAL.match(text)
            if match:
                yield 0, ("ATL231: Python 3.x incompatible octal %s should be "
                          "written as 0o%s " %
                          (match.group(0)[1:], match.group(1)))


@utils.flake8_atlas
class incompatible_print_statement(object):
    r"""Check if a Py3 incompatible print statement is used.

    Check for the use of print statements. But only flag those that are
    indeed Py3 incompatible. If print_function has been imported, by definition
    there are no print statements in the code, and this check will never fire.

    Okay: print msg       # but caught by ATL233
    Okay: print(msg)
    Fail: print("a","b")  # unless 'from __future__ import print_function'
    Fail: print
    """
    msg = ('ATL232: Python 3.x incompatible use of print statement', 'ATL232')

    def __init__(self, tree):
        self.tree = tree

    def run(self):
        for node in ast.walk(self.tree):
            if isinstance(node, ast.Print):   # Py2 print statement
                # no arguments
                if len(node.values)==0:
                    yield (node.lineno, node.col_offset) + self.msg
                # tuple as argument
                if len(node.values)>0 and isinstance(node.values[0], ast.Tuple) and len(node.values[0].elts)>1:
                    yield (node.lineno, node.col_offset) + self.msg


RE_PRINT = re.compile(r"\bprint\b\s*[^\(]")
@utils.flake8_atlas
def print_statement(logical_line):
    r"""Check if a Py3 incompatible print statement is used.

    Check if a print statement without brackets is used. This check
    complements ATL232.

    Okay:   print(msg)
    Fail:   print msg
    """
    for match in RE_PRINT.finditer(logical_line):
        yield match.start(0), ("ATL233: Python 3.x incompatible use of non function-like print statement")


@utils.flake8_atlas
def hacking_no_assert_equals(logical_line, tokens, noqa):
    r"""assert(Not)Equals() is deprecated, use assert(Not)Equal instead.

    Okay:   self.assertEqual(0, 0)
    Okay:   self.assertNotEqual(0, 1)
    ATL234: self.assertEquals(0, 0)
    ATL234: self.assertNotEquals(0, 1)
    Okay:   self.assertEquals(0, 0)  # noqa
    Okay:   self.assertNotEquals(0, 1)  # noqa
    """

    if noqa:
        return
    for token_type, text, start_index, _, _ in tokens:

        if token_type == tokenize.NAME:
            if text == "assertEquals" or text == "assertNotEquals":
                yield (start_index[1],
                       "ATL234: %s is deprecated, use %s" % (text, text[:-1]))


@utils.flake8_atlas
def hacking_no_assert_underscore(logical_line, tokens, noqa):
    r"""assert_() is deprecated, use assertTrue instead.

    Okay: self.assertTrue(foo)
    ATL235: self.assert_(foo)
    Okay: self.assert_(foo)  # noqa
    """
    if noqa:
        return
    for token_type, text, start_index, _, _ in tokens:

        if token_type == tokenize.NAME and text == "assert_":
            yield (start_index[1],
                   "ATL235: assert_ is deprecated, use assertTrue")


@utils.flake8_atlas
def hacking_python3x_metaclass(logical_line, noqa):
    r"""Check for metaclass to be Python 3.x compatible.

    Okay:   @six.add_metaclass(Meta)\nclass Foo(object):\n    pass
    Okay:   @six.with_metaclass(Meta)\nclass Foo(object):\n    pass
    Okay:   class Foo(object):\n    '''docstring\n\n    __metaclass__ = Meta\n'''
    ATL236: class Foo(object):\n    __metaclass__ = Meta
    ATL236: class Foo(object):\n    foo=bar\n    __metaclass__ = Meta
    ATL236: class Foo(object):\n    '''docstr.'''\n    __metaclass__ = Meta
    ATL236: class Foo(object):\n    __metaclass__ = \\\n        Meta
    Okay:   class Foo(object):\n    __metaclass__ = Meta  # noqa
    """
    if noqa:
        return
    split_line = logical_line.split()
    if(len(split_line) > 2 and split_line[0] == '__metaclass__' and
       split_line[1] == '='):
        yield (logical_line.find('__metaclass__'),
               "ATL236: Python 3.x incompatible __metaclass__, "
               "use six.add_metaclass()")


# NOTE(guochbo): This is removed module list:
# http://python3porting.com/stdlib.html#removed-modules
removed_modules = [
    'audiodev', 'Bastion', 'bsddb185', 'bsddb3',
    'Canvas', 'cfmfile', 'cl', 'commands', 'compiler'
    'dircache', 'dl', 'exception', 'fpformat',
    'htmllib', 'ihooks', 'imageop', 'imputil'
    'linuxaudiodev', 'md5', 'mhlib', 'mimetools'
    'MimeWriter', 'mimify', 'multifile', 'mutex',
    'new', 'popen2', 'posixfile', 'pure', 'rexec'
    'rfc822', 'sha', 'sgmllib', 'sre', 'stat'
    'stringold', 'sunaudio' 'sv', 'test.testall',
    'thread', 'timing', 'toaiff', 'user'
]


@utils.flake8_atlas
def hacking_no_removed_module(logical_line, noqa):
    r"""Check for removed modules in Python 3.

    Examples:
    Okay:   from os import path
    Okay:   from os import path as p
    Okay:   from os import (path as p)
    Okay:   import os.path
    ATL237: import thread
    Okay:   import thread  # noqa
    ATL237: import commands
    ATL237: import md5 as std_md5
    """
    if noqa:
        return
    line = import_normalize(logical_line.strip())
    if line and line.split()[0] == 'import':
        module_name = line.split()[1].split('.')[0]
        if module_name in removed_modules:
            yield 0, ("ATL237: module %s is "
                      "removed in Python 3" % module_name)


RE_NEW_STYLE_CLASS = re.compile(r"^class[^(]+\(.+\):")

@utils.flake8_atlas
def no_old_style_class(logical_line, noqa):
    r"""Check for old style classes.

    Examples:
    Okay:   class Foo(object):\n    pass
    Okay:   class Foo(Bar, Baz):\n    pass
    Okay:   class Foo(object, Baz):\n    pass
    Okay:   class Foo(somefunc()):\n    pass
    ATL238: class Bar:\n    pass
    ATL238: class Bar():\n    pass
    """
    if noqa:
        return
    line = logical_line.replace(' ','')
    if line.startswith("class") and not RE_NEW_STYLE_CLASS.match(line):
       yield (0, "ATL238: old style class declaration, "
              "use new style (inherit from `object`)")
