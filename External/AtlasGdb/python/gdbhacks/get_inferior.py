import gdb

def get_inferior():
    """Return the PID of the current inferior, or None."""
    inf = gdb.inferiors()
    if len(inf) < 1: return None
    return inf[0].pid

