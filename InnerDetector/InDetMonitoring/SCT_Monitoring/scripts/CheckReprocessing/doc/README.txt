Simple Grid Operation

Just run ./pandaSubmitJobs.sh

This will submit one grid job per line in conf/RunList.txt. Edit this list as required.
To change which versions of the reprocessed data to compare, you'll have to edit pandaSubmitJobs.sh, at these lines:

#Get the full names of the data sets
NEW_DATASET=`dq2-ls "d*$RUN*$STREAM*merge.HIST*1093*_p94_p94/"`
OLD_DATASET=`dq2-ls "d*$RUN*$STREAM*merge.HIST.r9*/"`

The script should work out the correct grid username for you, but if that fails you'll have to edit the script to specify it explicitly.


LSF

Run lsfSubmitJobs. Should submit one LSF job per line in conf/RunList.txt.
Returns the results to the results folder


Local Operation (simple)

Run dq2DataToJobs.


Local Operation (more complex)

First compile the code with make. There is a Makefile provided that should work on any system with Root set up.
The executable can be run like this:

./bin/histogramCompare -new %IN -old %IN2

Replace %IN and %IN2 with comma-separated lists of the paths to the input files to compare (should be local copies, from a single run only). Eg:

./bin/histogramCompare -new Input.1.newVersion.root,Input.2.newVersion.root -old Input.1.oldVersion.root,Input.2.oldVersion.root,Input.3.oldVersion.root

Note that the number of input files from each reprocessing version can differ - they will be merged before comparion.
