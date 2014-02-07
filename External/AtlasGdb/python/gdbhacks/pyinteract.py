# Start a python read-eval-print loop from within gdb.
# FIXME: Ctrl-C won't interrupt python.

import gdb
import sys
import code

class PyInteract (gdb.Command):
    """Start a python read-eval-print loop from within gdb."""

    def __init__ (self):
        super (PyInteract, self).__init__ ("pyinteract", gdb.COMMAND_OBSCURE)
        return

    def invoke (self, arg, from_tty):
        self.dont_repeat()
        code.interact("Press Ctrl-D to return to gdb.",
                      gdb.input,
                      sys.modules['__main__'].__dict__)
        return


PyInteract()
