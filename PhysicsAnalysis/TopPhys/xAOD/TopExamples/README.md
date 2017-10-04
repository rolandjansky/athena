This package can be used to submit grid jobs with AnalysisTop and TOPQ datasets.

The classes are flexible enough to allow the user to define new sample/derivation lists in a few lines.

Usage (after setting up AnalysisTop as usual)

> getExamples
# This will copy the scripts using PathResolver to ${PWD}/grid

> lsetup rucio pyami panda
# These tools need to be available for the TopExample scripts to function

> ./01SubmitToGrid.py
# This will run through the preconfigured list of samples and submit grid jobs
# You will want to edit this file to specify your own grid name, your own job suffixes and point to your cut file
# You may also want to define new samples (see MC16.py to see how they are added to the classes)


