#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""Tests for TestTools.logfiles"""

import io
import unittest
import contextlib
from TestTools.logfiles import grep_with_context

class TestPrintError(unittest.TestCase):
    """Tests for grep_with_context"""

    sample = '''\
1
2
3
4
FATAL
5
6
7
8
9
10
ERROR
11
12
13
14
ERROR
15
16
17
18'''

    def do(self, log, **argv):
        """Run command under test and capture stdout"""
        with contextlib.redirect_stdout(io.StringIO()) as output:
            if 'pattern' not in argv:
                argv['pattern']='ERROR|FATAL'
            grep_with_context(io.StringIO(log), **argv)
        return output.getvalue()

    def test_no_input(self):
        self.assertEqual(0, len(self.do('', lines=100)))

    def test_head(self):
        self.assertEqual("1\n2\n", self.do(self.sample, lines=2, print_tail=False, pattern='xy'))

    def test_tail(self):
        self.assertEqual("\n[Skipped 19 lines]\n\n17\n18", self.do(self.sample, lines=2, print_head=False, pattern='xy'))

    def test_headtail(self):
        self.assertEqual("1\n2\n\n[Skipped 17 lines]\n\n17\n18", self.do(self.sample, lines=2, pattern='xy'))

    def test_no_error(self):
        text = '\n'.join(str(i) for i in range(1,10))
        self.assertEqual(text, self.do(text, lines=100))

    def test_contextFull(self):
        # output should agree to input
        self.assertEqual(self.sample, self.do(self.sample, lines=100))

    def test_context100(self):
        # 1-500, ERROR, 501-999:
        text = '\n'.join(str(i) for i in range(1,500))
        text += '\nERROR\n'
        text += '\n'.join(str(i) for i in range(500,1000))

        # Result should be: first 100 lines, 100+100 around error, last 100 lines:
        result = list(range(1,101))+list(range(400,600))+list(range(900,1000))
        trimmed = [int(line) for line in self.do(text, lines=100).splitlines() if line.isdigit()]
        self.assertEqual(result, trimmed)

    def test_context1(self):
        result = '''\
1

[Skipped 2 lines]

4
FATAL
5

[Skipped 4 lines]

10
ERROR
11

[Skipped 2 lines]

14
ERROR
15

[Skipped 2 lines]

18'''
        self.assertEqual(result, self.do(self.sample, lines=1))


if __name__ == '__main__':
    unittest.main()
