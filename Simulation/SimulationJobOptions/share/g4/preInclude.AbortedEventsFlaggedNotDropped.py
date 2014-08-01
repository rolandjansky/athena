# SimulationJobOptions/preInclude.AbortedEventsFlaggedNotDropped.py
#
#Use-case: Overlay input simulation
#
# Effect: When FlagAbortedEvents is True and KillAbortedEvents is
# False, when an event is aborted the EventInfo is altered to have an
# Error state, rather than dropping the event from the output
# file completely.

from G4AtlasApps.SimFlags import simFlags
simFlags.FlagAbortedEvents = True
simFlags.KillAbortedEvents = False
