# Use verbose tracking for all G4 tracks. Very noisy and unrefined: you
# may find the VerboseSelector or VerboseSelectorArea preIncludes to be
# more appropriate, but this one gives you all the output you can eat.

from G4AtlasApps import callbacks
callbacks.use_verbose_tracking()
